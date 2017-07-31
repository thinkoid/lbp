#ifndef LBP_CSSILTP_HPP
#define LBP_CSSILTP_HPP

#include <lbp/defs.hpp>
#include <lbp/utils.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <opencv2/core.hpp>

#include <boost/hana/integral_constant.hpp>
#include <boost/hana/functional/demux.hpp>

#include <boost/circular_buffer.hpp>
#include <boost/integer.hpp>

//
// @article{wu2014real,
//   title={Real-time background subtraction-based video surveillance of people by integrating local texture patterns},
//   author={Wu, Hefeng and Liu, Ning and Luo, Xiaonan and Su, Jiawei and Chen, Liangshi},
//   journal={Signal, Image and Video Processing},
//   volume={8},
//   number={4},
//   pages={665--676},
//   year={2014},
//   publisher={Springer}
// }
//

namespace lbp {
namespace cssiltp_detail {

template< typename T >
auto cssiltp = [](auto N, auto S) {
    return [=](const cv::Mat& lhs, const cv::Mat& rhs,
               size_t i, size_t j, const T& tau) {
        using namespace cv;
        using namespace hana::literals;

        return boost::hana::fold_left (
            N, 0, [&, shift = 0](auto accum, auto x) mutable {
                const auto a = S (lhs, i + x [0_c], j + x [1_c]);
                const auto b = S (rhs, i - x [0_c], j - x [1_c]);

                const auto lhs = saturate_cast< T > ((1. - tau) * a);
                const auto rhs = saturate_cast< T > ((1. + tau) * a);

                return accum | (
                    (b > rhs ? 1 : (b < lhs ? 2 : 0)) << (shift++ * 2));
            });
    };
};

} // namespace cssiltp_detail

template< typename T, size_t R, size_t P >
auto cssiltp = [](const T& tau = T { }) {
    //
    // The operator operates on center-symmetric pixels from
    // center-symmetric frames (see 2.1:CS-SILTP descriptor):
    //
    boost::circular_buffer< cv::Mat > framebuf (R << 1);

    return [=](const cv::Mat& src) mutable {
        LBP_STATIC_ASSERT_MSG (0 == (P % 2), "odd-sized neighborhood");

        framebuf.push_back (src.clone ());

        if (framebuf.size () < R * 2)
            return src;

        //
        // Limited by the largest integral type of cv::Mat:
        //
        using value_type = typename boost::uint_t< R * P >::least;

        cv::Mat dst (src.size (), opencv_type< value_type >, cv::Scalar (0));

        auto op = cssiltp_detail::cssiltp< T > (
            detail::semicircular_neighborhood< R, P >,
            detail::nearest_sampler< T >);

#pragma omp parallel for
        for (size_t i = R; i < src.rows - R; ++i) {
            for (size_t j = R; j < src.cols - R; ++j) {
                size_t n = 0;

                for (size_t r = 0; r < 2 * R; ++r) {
                    n <<= P/2;

                    const auto& lhs = framebuf [r];
                    const auto& rhs = framebuf [2 * R - r - 1];

                    n |= op (lhs, rhs, i, j, tau);
                }

                dst.at< value_type > (i, j) = n;
            }
        }

        return dst;
    };
};

} // namespace lbp

#endif // LBP_CSSILTP_HPP
