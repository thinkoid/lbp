// -*- mode: c++; -*-

#include <lbp/ojala.hpp>
#include <lbp/utils.hpp>

#include <functional>

namespace lbp {

template< typename T, size_t R, size_t P, typename C >
/* explicit */ ojala_t< T, R, P, C >::ojala_t (const compare_type& cmp)
    : cmp_ (cmp)
{
    for (size_t i = 0; i < P; ++i) {
        N.emplace_back (
            nearbyint (-1.0 * R * sin (2. * M_PI * i / P)),
            nearbyint ( 1.0 * R * cos (2. * M_PI * i / P)));
    }
}

template< typename T, size_t R, size_t P, typename C >
inline size_t
ojala_t< T, R, P, C >::operator() (const cv::Mat& src, size_t i, size_t j) const
{
    const auto& center = src.at< T > (i, j);

    size_t n = 0, S = 0;

    for (const auto& off : N) {
        const auto& g = src.at< T > (i + off.first, j + off.second);
        n |= (size_t (cmp_ (g, center)) << S++);
    }

    return ojala_uniformity_measure (n, P);
}

template< typename T, size_t R, size_t P, typename C >
inline cv::Mat
ojala_t< T, R, P, C >::operator() (const cv::Mat& src) const
{
    auto dst = cv::Mat (src.size (), CV_8UC1, cv::Scalar (0));

    for (size_t i = R; i < src.rows - R; ++i) {
        for (size_t j = R; j < src.cols - R; ++j) {
            dst.at< unsigned char > (i, j) = this->operator() (src, i, j);
        }
    }

    return dst;
}

template< typename T, typename C >
inline cv::Mat
ojala_t< T, 1, 8, C >::operator() (const cv::Mat& src) const
{
    static constexpr int
        N [8][2] = {
        { -1, -1 },
        {  0, -1 },
        {  1, -1 },
        { -1,  0 },
        {  1,  0 },
        { -1,  1 },
        {  0,  1 },
        {  1,  1 }
    };

    static constexpr size_t S [] = {
        7, 6, 5, 0, 4, 1, 2, 3
    };

    static constexpr size_t U [] = {
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

    auto dst = cv::Mat (src.size (), CV_8UC1, cv::Scalar (0));

    for (size_t i = 1; i < size_t (src.rows) - 1; ++i) {
        auto s = dst.ptr< unsigned char > (i);

        for (size_t j = 1; j < size_t (src.cols) - 1; ++j) {
            const T* p [3] = {
                src.ptr< T > (i - 1) + j,
                src.ptr< T > (i) + j,
                src.ptr< T > (i + 1) + j
            };

            const auto c = p [1][0];

#define T(k) (size_t (cmp_ (p [1 + N [k][1]][N [k][0]], c)) << S [k])
            const auto k = T(0) | T(1) | T(2) | T(3) | T(4) | T(5) | T(6) | T(7);
            assert (k < sizeof U / sizeof *U);
#undef T

            s [j] = U [k];
        }
    }

    return dst;
}

} // namespace lbp
