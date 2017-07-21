#ifndef LBP_OLBP_HPP
#define LBP_OLBP_HPP

#include <lbp/defs.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <utility>
#include <vector>

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

template< typename T, size_t S >
auto uniformity_measure = [](size_t arg) {
    size_t a = 0;

    for (auto x = arg; x; x >>= 1)
        a += x & 1;

    size_t b = 0;

    for (auto x = ((arg >> 1) | ((arg & 1) << (S - 1))) ^ arg; x; x >>= 1)
        b += x & 1;

    return b < 3 ? a : (P + 1);
};

template< typename T >
auto uniformity_measure< T, 8 > = [](unsigned char value) {
    static constexpr unsigned char U [] = {
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

    return U [value];
};

template< typename T >
auto olbp = [](auto neighborhood, auto sampler) {
    using namespace boost::hana::literals;

    return [=](const cv::Mat& src, size_t i, size_t j) {
        const auto c = src.at< T > (i, j);

        return boost::hana::fold_left (
            neighborhood, 0, [&, S = 0](auto accum, auto x) mutable {
                const auto g = sampler (src, i + x [0_c], j + x [1_c]);
                return accum | (size_t (c >= g) << S++);
            });
    };
};

} // namespace olbp_detail

template< typename T, size_t R, size_t P >
auto olbp = [](const cv::Mat& src) {
    cv::Mat dst (src.size (), CV_8U);

    auto op = boost::hana::demux
        (olbp_detail::uniformity_measure< unsigned char, P >)
        (olbp_detail::olbp< T > (
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

#include <lbp/olbp.cc>

#endif // LBP_OLBP_HPP
