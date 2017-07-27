#ifndef LBP_SCSLBP_HPP
#define LBP_SCSLBP_HPP

#include <lbp/defs.hpp>
#include <lbp/utils.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <opencv2/core.hpp>

#include <boost/hana/fold.hpp>
#include <boost/hana/size.hpp>
#include <boost/hana/tuple.hpp>

#include <boost/integer.hpp>

//
// @inproceedings{xue2010dynamic,
//   title={Dynamic background subtraction based on spatial extended center-symmetric local binary pattern},
//   author={Xue, Gengjian and Sun, Jun and Song, Li},
//   booktitle={Multimedia and Expo (ICME), 2010 IEEE International Conference on},
//   pages={1050--1054},
//   year={2010},
//   organization={IEEE}
// }
//

namespace lbp {
namespace scslbp_detail {

template< typename T >
auto scslbp = [](auto neighborhood, auto sampler) {
    namespace hana = boost::hana;
    using namespace hana::literals;

    const auto N = hana::size (neighborhood);

    return [=](const cv::Mat& src, const cv::Mat& m, const cv::Mat& s,
               size_t i, size_t j) {

        const auto f = abs (
            src.at< T > (i, j) - m.at< T > (i, j)) >= 2.5 * s.at< T > (i, j);

        return hana::fold_left (
            neighborhood, 0, [&, S = 0](auto accum, auto x) mutable {
                const auto a = sampler (src, i + x [0_c], j + x [1_c]);
                const auto b = sampler (src, i - x [0_c], j - x [1_c]);

                return accum + ((a >= b) << S++);
            }) + (int (f) << N);
    };
};

} // namespace scslbp_detail

template< typename T, size_t R, size_t P >
auto scslbp = [](double alpha = 0.05) {
    //
    // Learning rate for both mean and variance:
    //
    LBP_ASSERT (0. <= alpha && alpha <= 1.);

    //
    // Mean, variance, standard deviation and previous frame:
    //
    cv::Mat m, v, s, I;

    //
    // Elementary operator:
    //
    auto op = scslbp_detail::scslbp< float > (
        detail::semicircular_neighborhood< R, P >,
        detail::nearest_sampler< float >);

    return [=, init = 0](const cv::Mat& arg) mutable {
        using value_type = typename boost::uint_t< P/2 + 1 >::least;

        //
        // Only works on single-channel, gray images:
        //
        LBP_ASSERT (1 == arg.channels ());

        cv::Mat src = lbp::equalize (lbp::convert (arg, CV_32F));

        if (0 == init && 1 == ++init) {
            I = src.clone ();

            m = I.clone ();
            v = I.clone ();
            s = I.clone ();

            return I;
        }

        cv::Mat dst (
            src.size (), opencv_type< (sizeof (value_type) << 3) >,
            cv::Scalar (0));

        //
        // Exponential moving average for estimating the mean:
        //
        cv::Mat m_ = (1 - alpha) * m + alpha * src;

        //
        // Rolling variance with a twist from [2009Gil-Jiménez] (see chapter 2):
        //
        cv::Mat v_ = (1 - alpha) * v + (alpha / 2) * lbp::pow (src - I, 2);
        cv::Mat s_ = lbp::sqrt (v_);

#pragma omp parallel for
        for (size_t i = R; i < src.rows - R - 1; ++i) {
            for (size_t j = R; j < src.cols - R - 1; ++j) {
                dst.at< value_type > (i, j) = op (src, m, s, i, j);
            }
        }

        m = m_;
        v = v_;
        s = s_;

        I = src.clone ();

        return dst;
    };
};

} // namespace lbp

//
// References:
//
// [2009Gil-Jiménez] Gil-Jiménez, Pedro, et al. "Continuous variance estimation
// in video surveillance sequences with high illumination changes." Signal
// Processing 89.7 (2009): 1412-1416.
//
//

#endif // LBP_SCSLBP_HPP
