#ifndef LBP_CSLBP_HPP
#define LBP_CSLBP_HPP

#include <lbp/defs.hpp>

#include <utility>
#include <vector>

#include <opencv2/core.hpp>

#include <boost/hana/fwd/take_front.hpp>
#include <boost/hana/fold.hpp>
#include <boost/hana/range.hpp>
#include <boost/hana/tuple.hpp>
namespace hana = boost::hana;

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

template< size_t R, size_t P >
auto x_at = [](auto i) {
    return int (nearbyint (1. * R * cos ((2. * M_PI * i.value) / P)));
};

template< size_t R, size_t P >
auto y_at = [](auto i) {
    return int (nearbyint (-1. * R * sin ((2. * M_PI * i.value) / P)));
};

template< size_t R, size_t P >
auto circle_coord_at = [](auto i) {
    return hana::make_tuple (y_at< R, P > (i), x_at< R, P > (i));
};

template< size_t R, size_t P >
const auto circular_neighborhood = hana::transform (
    hana::to< hana::tuple_tag > (
        hana::make_range (hana::int_c< 0 >, hana::int_c< P >)),
    circle_coord_at< R, P >);

#define T(...)  hana::make_tuple (__VA_ARGS__)
#define P(x, y) hana::make_tuple (x, y)

template< >
constexpr auto circular_neighborhood< 1, 4 > = T (P (-0, 1), P (-1, 0));

template< >
constexpr auto circular_neighborhood< 1, 8 > = T (
    P ( -0,  1 ), P ( -1,  1 ), P ( -1,  0 ), P ( -1, -1 ));

template< >
constexpr auto circular_neighborhood< 2, 8 > = T (
    P (-0, 2), P (-1, 1), P (-2, 0), P (-1, -1));

template< >
constexpr auto circular_neighborhood< 2, 10 > = T (
    P (-0,  2), P (-1,  2), P (-2,  1), P (-2, -1), P (-1, -2));

template< >
constexpr auto circular_neighborhood< 2, 12 > = T (
    P (-0,  2), P (-1,  2), P (-2,  1), P (-2,  0), P (-2, -1), P (-1, -2));

template< >
constexpr auto circular_neighborhood< 2, 16 > = T (
    P (-0,  2), P (-1,  2), P (-1,  1), P (-2,  1),
    P (-2,  0), P (-2, -1), P (-1, -1), P (-1, -2));

#undef T
#undef P

template< size_t, size_t, typename = hana::when< true > >
struct cslbp_base_t;

template< size_t R_, size_t P_ >
struct cslbp_base_t< R_, P_, hana::when < (P_ % 2 == 0) > >
{
    using value_type = typename boost::uint_t< (P_ + 1) / 2 >::least;

    static constexpr size_t R = R_;
    static constexpr size_t P = P_;
};

template< size_t > constexpr int opencv_type;

#define OPENCV_TYPEDEF(n, t) template< > constexpr int opencv_type< n > = t

OPENCV_TYPEDEF ( 8, CV_8U);
OPENCV_TYPEDEF (16, CV_16U);
OPENCV_TYPEDEF (32, CV_32S);

#undef OPENCV_TYPE_DEF

} // namespace cslbp_detail

template< size_t R, size_t P >
struct cslbp_t : cslbp_detail::cslbp_base_t< R, P > {
    using base_type = cslbp_detail::cslbp_base_t< R, P >;
    using value_type = typename base_type::value_type;

public:
    explicit cslbp_t () { }

    template< typename T >
    cv::Mat operator() (const cv::Mat&, const T& = T ()) const;
};

} // namespace lbp

#include <lbp/cslbp.cc>

#endif // LBP_CSLBP_HPP
