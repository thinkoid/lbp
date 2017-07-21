#ifndef LBP_CSLBP_HPP
#define LBP_CSLBP_HPP

#include <lbp/defs.hpp>
#include <lbp/utils.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <utility>
#include <vector>

#include <opencv2/core.hpp>

#include <boost/hana/fold.hpp>
#include <boost/hana/tuple.hpp>

#include <boost/integer.hpp>

//
// @article{Heikkila:2009:DIR:1456722.1456979,
//  author = {Heikkil\"{a}, Marko and Pietik\"{a}inen, Matti and Schmid, Cordelia},
//  title = {Description of Interest Regions with Local Binary Patterns},
//  journal = {Pattern Recogn.},
//  issue_date = {March, 2009},
//  volume = {42},
//  number = {3},
//  month = mar,
//  year = {2009},
//  issn = {0031-3203},
//  pages = {425--436},
//  numpages = {12},
//  url = {http://dx.doi.org/10.1016/j.patcog.2008.08.014},
//  doi = {10.1016/j.patcog.2008.08.014},
//  acmid = {1456979},
//  publisher = {Elsevier Science Inc.},
//  address = {New York, NY, USA},
//  keywords = {Image matching, Local binary patterns, Object recognition, Region description, Region detection, SIFT},
// }
//

namespace lbp {
namespace cslbp_detail {

template< typename T >
auto cslbp = [](auto neighborhood, auto sampler) {
    using namespace boost::hana::literals;

    return [=](const cv::Mat& src, size_t i, size_t j, const T& epsilon) {
        return boost::hana::fold (
            neighborhood, 0, [&, S = 0](auto accum, auto x) mutable {
                const auto a = sampler (src, i + x [0_c], j + x [1_c]);
                const auto b = sampler (src, i - x [0_c], j - x [1_c]);
                return accum + ((std::abs (a - b) >= epsilon) << S++);
            });
    };
};

} // namespace cslbp_detail

template< typename T, size_t R, size_t P >
auto cslbp = [](const cv::Mat& src, const T& epsilon = T { }) {
    using value_type = typename boost::uint_t< P >::least;
        
    cv::Mat dst (src.size (), opencv_type< (sizeof (value_type) << 3) >);

    auto op = cslbp_detail::cslbp< T > (
        detail::semicircular_neighborhood< R, P >,
        detail::nearest_sampler< T >);

#pragma omp parallel for
    for (size_t i = R; i < src.rows - R; ++i) {
        for (size_t j = R; j < src.cols - R; ++j) {
            dst.at< unsigned char > (i, j) = op (src, i, j, epsilon);
        }
    }

    return dst;
};

} // namespace lbp

#endif // LBP_CSLBP_HPP
