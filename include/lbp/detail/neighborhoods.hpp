#ifndef LBP_DETAIL_NEIGHBORHOODS_HPP
#define LBP_DETAIL_NEIGHBORHOODS_HPP

#include <lbp/defs.hpp>

#include <boost/hana/fold.hpp>
#include <boost/hana/range.hpp>
#include <boost/hana/take_front.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/tuple.hpp>
namespace hana = boost::hana;

namespace lbp {
namespace detail {

template< size_t R, size_t P >
auto x_at = [](auto i) {
    return 1. * R * cos ((2. * M_PI * i.value) / P);
};

template< size_t R, size_t P >
auto y_at = [](auto i) {
    return -1. * R * sin ((2. * M_PI * i.value) / P);
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
const auto semicircular_neighborhood = hana::take_front (
    circular_neighborhood< R, P >, hana::size_c< P/2 >);

#define T(...)  hana::make_tuple (__VA_ARGS__)
#define P(x, y) hana::make_tuple (x, y)

template< >
constexpr auto semicircular_neighborhood< 1, 4 > = T (
    P ( 0.00000, 1.00000),
    P (-1.00000, 0.00000));

template< >
constexpr auto semicircular_neighborhood< 1, 8 > = T (
    P ( 0.00000,   1.00000),
    P (-0.707107,  0.707107),
    P (-1.00000,   0.00000),
    P (-0.707107, -0.707107));

template< >
constexpr auto semicircular_neighborhood< 2, 8 > = T (
    P ( 0.00000,  2.00000),
    P (-1.41421,  1.41421),
    P (-2.00000,  0.00000),
    P (-1.41421, -1.41421));

template< >
constexpr auto semicircular_neighborhood< 2, 10 > = T (
    P ( 0.00000,  2.00000),
    P (-1.17557,  1.61803),
    P (-1.90211,  0.618034),
    P (-1.90211, -0.618034),
    P (-1.17557, -1.61803));

template< >
constexpr auto semicircular_neighborhood< 2, 12 > = T (
    P ( 0.00000,  2.00000),
    P (-1.00000,  1.73205),
    P (-1.73205,  1.00000),
    P (-2.00000,  0.00000),
    P (-1.73205, -1.00000),
    P (-1.00000, -1.73205));

template< >
constexpr auto semicircular_neighborhood< 2, 16 > = T (
    P ( 0.00000,   2.00000),
    P (-0.765367,  1.84776),
    P (-1.41421,   1.41421),
    P (-1.84776,   0.765367),
    P (-2.00000,   0.00000),
    P (-1.84776,  -0.765367),
    P (-1.41421,  -1.41421),
    P (-0.765367, -1.84776));

template< >
constexpr auto circular_neighborhood< 1, 4 > = T (
    P (0.00000, 1.00000),
    P (-1.00000, 0.00000),
    P (0.00000, -1.00000),
    P (1.00000, 0.00000));

template< >
constexpr auto circular_neighborhood< 1, 8 > = T (
    P ( 0.00000,   1.00000),
    P (-0.707107,  0.707107),
    P (-1.00000,   0.00000),
    P (-0.707107, -0.707107),
    P ( 0.00000,  -1.00000),
    P ( 0.707107, -0.707107),
    P ( 1.00000,   0.00000),
    P ( 0.707107,  0.707107));

template< >
constexpr auto circular_neighborhood< 2, 8 > = T (
    P ( 0.00000,  2.00000),
    P (-1.41421,  1.41421),
    P (-2.00000,  0.00000),
    P (-1.41421, -1.41421),
    P ( 0.00000, -2.00000),
    P ( 1.41421, -1.41421),
    P ( 2.00000,  0.00000),
    P ( 1.41421,  1.41421));

template< >
constexpr auto circular_neighborhood< 2, 10 > = T (
    P ( 0.00000,  2.00000),
    P (-1.17557,  1.61803),
    P (-1.90211,  0.618034),
    P (-1.90211, -0.618034),
    P (-1.17557, -1.61803),
    P ( 0.00000, -2.00000),
    P ( 1.17557, -1.61803),
    P ( 1.90211, -0.618034),
    P ( 1.90211,  0.618034),
    P ( 1.17557,  1.61803));

template< >
constexpr auto circular_neighborhood< 2, 12 > = T (
    P ( 0.00000,  2.00000),
    P (-1.00000,  1.73205),
    P (-1.73205,  1.00000),
    P (-2.00000,  0.00000),
    P (-1.73205, -1.00000),
    P (-1.00000, -1.73205),
    P ( 0.00000, -2.00000),
    P ( 1.00000, -1.73205),
    P ( 1.73205, -1.00000),
    P ( 2.00000,  0.00000),
    P ( 1.73205,  1.00000),
    P ( 1.00000,  1.73205));

template< >
constexpr auto circular_neighborhood< 2, 16 > = T (
    P ( 0.00000,   2.00000),
    P (-0.765367,  1.84776),
    P (-1.41421,   1.41421),
    P (-1.84776,   0.765367),
    P (-2.00000,   0.00000),
    P (-1.84776,  -0.765367),
    P (-1.41421,  -1.41421),
    P (-0.765367, -1.84776),
    P ( 0.00000,  -2.00000),
    P ( 0.765367, -1.84776),
    P ( 1.41421,  -1.41421),
    P ( 1.84776,  -0.765367),
    P ( 2.00000,   0.00000),
    P ( 1.84776,   0.765367),
    P ( 1.41421,   1.41421),
    P ( 0.765367,  1.84776));

#undef T
#undef P

}}

#endif // LBP_DETAIL_NEIGHBORHOODS_HPP
