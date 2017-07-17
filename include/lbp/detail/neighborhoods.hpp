#ifndef LBP_DETAIL_NEIGHBORHOODS_HPP
#define LBP_DETAIL_NEIGHBORHOODS_HPP

#include <lbp/defs.hpp>

#include <boost/hana/fwd/take_front.hpp>
#include <boost/hana/fold.hpp>
#include <boost/hana/range.hpp>
#include <boost/hana/tuple.hpp>
namespace hana = boost::hana;

#include <boost/integer.hpp>

namespace lbp {
namespace detail {

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

template< size_t R, size_t P >
const auto semicircular_neighborhood = hana::transform (
    hana::to< hana::tuple_tag > (
        hana::make_range (hana::int_c< 0 >, hana::int_c< P/2 >)),
    circle_coord_at< R, P >);

#define T(...)  hana::make_tuple (__VA_ARGS__)
#define P(x, y) hana::make_tuple (x, y)

template< >
constexpr auto semicircular_neighborhood< 1, 4 > = T (P (-0, 1), P (-1, 0));

template< >
constexpr auto semicircular_neighborhood< 1, 8 > = T (
    P ( -0,  1 ), P ( -1,  1 ), P ( -1,  0 ), P ( -1, -1 ));

template< >
constexpr auto semicircular_neighborhood< 2, 8 > = T (
    P (-0, 2), P (-1, 1), P (-2, 0), P (-1, -1));

template< >
constexpr auto semicircular_neighborhood< 2, 10 > = T (
    P (-0,  2), P (-1,  2), P (-2,  1), P (-2, -1), P (-1, -2));

template< >
constexpr auto semicircular_neighborhood< 2, 12 > = T (
    P (-0,  2), P (-1,  2), P (-2,  1), P (-2,  0), P (-2, -1), P (-1, -2));

template< >
constexpr auto semicircular_neighborhood< 2, 16 > = T (
    P (-0,  2), P (-1,  2), P (-1,  1), P (-2,  1),
    P (-2,  0), P (-2, -1), P (-1, -1), P (-1, -2));

template< >
constexpr auto circular_neighborhood< 1, 4 > = T (
    P (-0, 1), P (-1, 0), P (-0, -1), P (1, -0));

template< >
constexpr auto circular_neighborhood< 1, 8 > = T (
    P ( -0,  1 ), P ( -1,  1 ), P ( -1,  0 ), P ( -1, -1 ),
    P ( -0, -1 ), P (  1, -1 ), P (  1, -0 ), P (  1,  1 ));

template< >
constexpr auto circular_neighborhood< 2, 8 > = T (
    P (-0, 2), P (-1, 1), P (-2, 0), P (-1, -1),
    P (-0, -2), P (1, -1), P (2, -0), P (1, 1));

template< >
constexpr auto circular_neighborhood< 2, 10 > = T (
    P (-0,  2), P (-1,  2), P (-2,  1), P (-2, -1),
    P (-1, -2), P (-0, -2), P ( 1, -2), P ( 2, -1),
    P ( 2,  1), P ( 1,  2));

template< >
constexpr auto circular_neighborhood< 2, 12 > = T (
    P (-0,  2), P (-1,  2), P (-2,  1), P (-2,  0),
    P (-2, -1), P (-1, -2), P (-0, -2), P ( 1, -2),
    P ( 2, -1), P ( 2, -0), P ( 2,  1), P ( 1,  2));

template< >
constexpr auto circular_neighborhood< 2, 16 > = T (
    P (-0,  2), P (-1,  2), P (-1,  1), P (-2,  1),
    P (-2,  0), P (-2, -1), P (-1, -1), P (-1, -2),
    P (-0, -2), P ( 1, -2), P ( 1, -1), P ( 2, -1),
    P ( 2, -0), P ( 2,  1), P ( 1,  1), P ( 1,  2));

#undef T
#undef P

}}

#endif // LBP_DETAIL_NEIGHBORHOODS_HPP
