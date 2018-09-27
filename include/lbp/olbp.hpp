#ifndef LBP_OLBP_HPP
#define LBP_OLBP_HPP

#include <lbp/defs.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <opencv2/core.hpp>

#include <boost/hana/integral_constant.hpp>
#include <boost/hana/functional/demux.hpp>

//
// @inproceedings{Ojala:2001:GLB:646260.685274,
//  author = {Ojala, Timo and Pietik\"{a}inen, Matti and M\"{a}enp\"{a}\"{a}, Topi},
//  title = {A Generalized Local Binary Pattern Operator for Multiresolution Gray Scale and Rotation Invariant Texture Classification},
//  booktitle = {Proceedings of the Second International Conference on Advances in Pattern Recognition},
//  series = {ICAPR '01},
//  year = {2001},
//  isbn = {3-540-41767-2},
//  pages = {397--406},
//  numpages = {10},
//  url = {http://dl.acm.org/citation.cfm?id=646260.685274},
//  acmid = {685274},
//  publisher = {Springer-Verlag},
//  address = {London, UK, UK},
// }
//

namespace lbp {
namespace olbp_detail {

template< typename T, size_t N >
using enable_if_integral_t = typename std::enable_if<
    std::is_integral< T >::value, T >::type;

template< typename T, size_t S >
auto uniformity_measure = [](enable_if_integral_t< T, S > arg) {
    auto a = 0, b = 0;
    const auto c = typename std::make_unsigned_t< T > (arg);

    for (auto x = c; x; x >>= 1)
        a += x & 1;

    for (auto x = ((c >> 1) | ((c & 1) << (S - 1))) ^ c; x; x >>= 1)
        b += x & 1;

    return b < 3 ? a : (S + 1);
};

template< typename T >
auto uniformity_measure< T, 8 > = [](enable_if_integral_t< T, 8 > arg) {
    static constexpr unsigned char arr [] = {
        0, 1, 1, 2, 1, 9, 2, 3, 1, 9, 9, 9, 2, 9, 3, 4,
        1, 9, 9, 9, 9, 9, 9, 9, 2, 9, 9, 9, 3, 9, 4, 5,
        1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        2, 9, 9, 9, 9, 9, 9, 9, 3, 9, 9, 9, 4, 9, 5, 6,
        1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        2, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        3, 9, 9, 9, 9, 9, 9, 9, 4, 9, 9, 9, 5, 9, 6, 7,
        1, 2, 9, 3, 9, 9, 9, 4, 9, 9, 9, 9, 9, 9, 9, 5,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 6,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7,
        2, 3, 9, 4, 9, 9, 9, 5, 9, 9, 9, 9, 9, 9, 9, 6,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7,
        3, 4, 9, 5, 9, 9, 9, 6, 9, 9, 9, 9, 9, 9, 9, 7,
        4, 5, 9, 6, 9, 9, 9, 7, 5, 6, 9, 7, 6, 7, 7, 8
    };

    const auto x = typename std::make_unsigned< T >::type (arg);
    LBP_ASSERT (x < sizeof arr / sizeof *arr);

    return arr [x];
};

const auto olbp = [](auto N, auto S) {
    return [=](const cv::Mat& src, size_t i, size_t j) {
        using namespace boost::hana::literals;

        const auto c = S (src, i, j);

        return boost::hana::fold_left (
            N, 0, [&, shift = 0](auto accum, auto x) mutable {
                const auto g = S (src, i + x [0_c], j + x [1_c]);
                return accum | (size_t (c >= g) << shift++);
            });
    };
};

} // namespace olbp_detail

template< typename T, size_t R, size_t P >
auto olbp = [](const cv::Mat& src) {
    using namespace hana::literals;

    cv::Mat dst (src.size (), CV_8U, cv::Scalar (0));

    auto op = boost::hana::demux
        (olbp_detail::uniformity_measure< unsigned char, P >)
        (olbp_detail::olbp (
            detail::circular_neighborhood< R, P >,
            detail::nearest_sampler< T >));

#pragma omp parallel for
    for (size_t i = R; i < src.rows - R; ++i) {
        for (size_t j = R; j < src.cols - R; ++j) {
            dst.at< unsigned char > (i, j) = op (src, i, j);
        }
    }

    return dst;
};

} // namespace lbp

#endif // LBP_OLBP_HPP
