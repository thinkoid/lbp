#ifndef LBP_XCSLBP_HPP
#define LBP_XCSLBP_HPP

#include <lbp/defs.hpp>
#include <lbp/utils.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <opencv2/core.hpp>

#include <boost/hana/fold.hpp>
#include <boost/hana/tuple.hpp>

#include <boost/integer.hpp>

//
// @inproceedings{silva2015extended,
//   title={An eXtended center-symmetric local binary pattern for background modeling and subtraction in videos},
//   author={Silva, Caroline and Bouwmans, Thierry and Fr{\'e}licot, Carl},
//   booktitle={International Joint Conference on Computer Vision, Imaging and Computer Graphics Theory and Applications, VISAPP 2015},
//   year={2015}
// }
//

namespace lbp {
namespace xcslbp_detail {

template< typename T >
auto xcslbp = [](auto N, auto S) {
    return [=](const cv::Mat& src, size_t i, size_t j, const T& epsilon) {
        using namespace cv;
        using namespace hana::literals;

        const auto c = S (src, i, j);

        return boost::hana::fold (
            N, 0, [&, shift = 0](auto accum, auto x) mutable {
                const auto a = S (src, i + x [0_c], j + x [1_c]);
                const auto b = S (src, i - x [0_c], j - x [1_c]);

                const auto g1 = a - b + c;
                const auto g2 = (a - c) * (b - c);

                return accum | (((g1 + g2) >= epsilon) << shift++);
            });
    };
};

} // namespace xcslbp_detail

template< typename T, size_t R, size_t P >
auto xcslbp = [](const cv::Mat& src, const T& epsilon = T { }) {
    using value_type = typename boost::uint_t< P/2 >::least;
        
    cv::Mat dst (src.size (), opencv_type< value_type >, cv::Scalar (0));

    auto op = xcslbp_detail::xcslbp< T > (
        detail::semicircular_neighborhood< R, P >,
        detail::nearest_sampler< T >);

#pragma omp parallel for
    for (size_t i = R; i < src.rows - R; ++i) {
        for (size_t j = R; j < src.cols - R; ++j) {
            dst.at< value_type > (i, j) = op (src, i, j, epsilon);
        }
    }

    return dst;
};

} // namespace lbp

#endif // LBP_XCSLBP_HPP
