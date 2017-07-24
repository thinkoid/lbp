#ifndef LBP_SILTP_HPP
#define LBP_SILTP_HPP

#include <lbp/defs.hpp>
#include <lbp/utils.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <opencv2/core.hpp>

#include <boost/hana/integral_constant.hpp>
#include <boost/hana/functional/demux.hpp>

#include <boost/integer.hpp>

//
// @inproceedings{liao2010modeling,
//   title={Modeling pixel process with scale invariant local patterns for background subtraction in complex scenes},
//   author={Liao, Shengcai and Zhao, Guoying and Kellokumpu, Vili and Pietik{\"a}inen, Matti and Li, Stan Z},
//   booktitle={Computer Vision and Pattern Recognition (CVPR), 2010 IEEE Conference on},
//   pages={1301--1306},
//   year={2010},
//   organization={IEEE}
// }/
//

namespace lbp {
namespace siltp_detail {

template< typename T >
auto siltp = [](auto neighborhood, auto sampler) {
    return [=](const cv::Mat& src, size_t i, size_t j, const T& tau) {
        using namespace cv;

        namespace hana = boost::hana;
        using namespace hana::literals;

        const auto c = src.at< T > (i, j);

        const auto lhs = saturate_cast< T > ((1. - tau) * c);
        const auto rhs = saturate_cast< T > ((1. + tau) * c);

        return hana::fold_left (
            neighborhood, 0, [&, S = 0](auto accum, auto x) mutable {
                const auto g = sampler (src, i + x [0_c], j + x [1_c]);
                return accum | ((g > rhs ? 1 : (g < lhs ? 2 : 0)) << (S++ * 2));
            });
    };
};

} // namespace siltp_detail

template< typename T, size_t R, size_t P >
auto siltp = [](const cv::Mat& src, const T& tau = T { }) {
    using value_type = typename boost::uint_t< P * 2 >::least;

    cv::Mat dst (
        src.size (), opencv_type< (sizeof (value_type) << 3) >,
        cv::Scalar (0));

    auto op = siltp_detail::siltp< T > (
        detail::circular_neighborhood< R, P >,
        detail::nearest_sampler< T >);

#pragma omp parallel for
    for (size_t i = R; i < src.rows - R; ++i) {
        for (size_t j = R; j < src.cols - R; ++j) {
            dst.at< value_type > (i, j) = op (src, i, j, tau);
        }
    }

    return dst;
};

} // namespace lbp

#endif // LBP_SILTP_HPP
