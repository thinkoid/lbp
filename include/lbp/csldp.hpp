#ifndef LBP_CSLDP_HPP
#define LBP_CSLDP_HPP

#include <lbp/defs.hpp>
#include <lbp/utils.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <opencv2/core.hpp>

#include <boost/hana/fold.hpp>
#include <boost/hana/tuple.hpp>

#include <boost/integer.hpp>

//
// @inproceedings{xue2011hybrid,
//   title={Hybrid center-symmetric local pattern for dynamic background subtraction},
//   author={Xue, Gengjian and Song, Li and Sun, Jun and Wu, Meng},
//   booktitle={Multimedia and Expo (ICME), 2011 IEEE International Conference on},
//   pages={1--6},
//   year={2011},
//   organization={IEEE}
//}
//

namespace lbp {
namespace csldp_detail {

template< typename T >
auto csldp = [](auto neighborhood, auto sampler) {
    return [=](const cv::Mat& src, size_t i, size_t j, const T& e) {
        using namespace cv;

        namespace hana = boost::hana;
        using namespace hana::literals;

        const auto c = saturate_cast< T > (src.at< T > (i, j) + e);

        return hana::fold (
            neighborhood, 0, [&, S = 0](auto accum, auto x) mutable {
                const auto a = sampler (src, i + x [0_c], j + x [1_c]);
                const auto b = sampler (src, i - x [0_c], j - x [1_c]);
                return accum | (((a >= c) ^ (b >= c)) << S++);
            });
    };
};

} // namespace csldp_detail

template< typename T, size_t R, size_t P >
auto csldp = [](const cv::Mat& src, const T& epsilon = T { }) {
    LBP_STATIC_ASSERT_MSG (0 == (P % 2), "odd-sized CSLDP neighborhood");

    using value_type = typename boost::uint_t< P/2 >::least;

    cv::Mat dst (
        src.size (), opencv_type< (sizeof (value_type) << 3) >,
        cv::Scalar (0));

    auto op = csldp_detail::csldp< T > (
        detail::semicircular_neighborhood< R, P >,
        detail::bilinear_sampler< T >);

#pragma omp parallel for
    for (size_t i = R; i < src.rows - R - 1; ++i) {
        for (size_t j = R; j < src.cols - R - 1; ++j) {
            dst.at< value_type > (i, j) = op (src, i, j, epsilon);
        }
    }

    return dst;
};

} // namespace lbp

#endif // LBP_CSLDP_HPP
