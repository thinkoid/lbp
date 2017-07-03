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

    cv::Mat dst (src.size (), CV_8UC1);

    for (int i = 1; i < int (src.rows) - 1; ++i) {
        const T* p = src.ptr< T > (i - 1);
        const T* q = src.ptr< T > (i);
        const T* r = src.ptr< T > (i + 1);

        T* s = dst.ptr< T > (i);

        for (int j = 1; j < int (src.cols) - 1; ++j) {
#define T(a, b, c) (cmp_ (a [b], q [j]) << c)
            const unsigned char k =
                T (p, j - 1, 7) |
                T (p, j    , 6) |
                T (p, j + 1, 5) |
                T (r, j + 1, 3) |
                T (r, j    , 2) |
                T (r, j - 1, 1) |
                T (q, j - 1, 0) |
                T (q, j + 1, 4);
#undef T

            s [j] = U [k];
        }
    }

    return dst;
}

template< typename T, typename C >
inline cv::Mat
ojala_t< T, 2, 16, C >::operator() (const cv::Mat& src) const
{
    cv::Mat dst (src.size (), CV_8UC1);

    for (int i = 2; i < int (src.rows) - 2; ++i) {
        const T* p = src.ptr< T > (i - 2);
        const T* q = src.ptr< T > (i - 1);
        const T* r = src.ptr< T > (i);
        const T* s = src.ptr< T > (i + 1);
        const T* t = src.ptr< T > (i + 2);

        T* u = dst.ptr< T > (i);

        for (int j = 2; j < int (src.cols) - 2; ++j) {
#define T(a, b, c) (cmp_ (a [b], r [j]) << c)
            const unsigned k =
                T (p, j -1,   0) |
                T (p, j,      1) |
                T (p, j + 1,  2) |
                T (q, j - 2,  3) |
                T (q, j - 1,  4) |
                T (q, j + 1,  5) |
                T (q, j + 2,  6) |
                T (r, j - 2,  7) |
                T (r, j + 2,  8) |
                T (s, j - 2,  9) |
                T (s, j - 1, 10) |
                T (s, j + 1, 11) |
                T (s, j + 2, 12) |
                T (t, j - 1, 13) |
                T (t, j + 1, 14) |
                T (t, j,     15);
#undef T

            u [j] = ojala_uniformity_measure (k, 16);
        }
    }

    return dst;
}

} // namespace lbp
