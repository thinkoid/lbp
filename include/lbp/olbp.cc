// -*- mode: c++; -*-

#include <lbp/olbp.hpp>
#include <lbp/utils.hpp>
#include <lbp/detail/neighborhoods.hpp>

#include <functional>

namespace lbp {
namespace olbp_detail {

static inline size_t
count_ones (size_t arg) {
    size_t n = 0;

    for (; arg; arg >>= 1) {
        n += arg & 1;
    }

    return n;
}

static inline size_t
count_flips (size_t n, size_t P) {
    return count_ones (size_t (((n >> 1) | ((n & 1) << (P - 1))) ^ n));
}

static inline size_t
uniformity_measure (size_t n, size_t P) {
    const auto a = count_ones (n), b = count_flips (n, P);
    return b < 3 ? a : (P + 1);
}

} // namespace olbp_detail

////////////////////////////////////////////////////////////////////////

template< size_t R, size_t P >
template< typename T >
inline cv::Mat
olbp_t< R, P >::operator() (const cv::Mat& src, const cv::Mat& ref) const
{
    using namespace hana::literals;

    LBP_ASSERT (src.size () == ref.size ());
    LBP_ASSERT (src.type () == ref.type ());

    const auto N = detail::circular_neighborhood< R, P >;

    auto dst = cv::Mat (src.size (), CV_8UC1, cv::Scalar (0));

#pragma omp parallel for
    for (size_t i = R; i < src.rows - R; ++i) {
        for (size_t j = R; j < src.cols - R; ++j) {
            const auto& c = ref.at< T > (i, j);

            const auto n = boost::hana::fold (
                N, 0, [&, S = 0](auto accum, auto x) mutable {
                    const auto g = src.at< T > (i + x [0_c], j + x [1_c]);
                    return accum | (size_t (c >= g) << S++);
                });

            dst.at< unsigned char > (i, j) =
                olbp_detail::uniformity_measure (n, P);
        }
    }

    return dst;
}

template< size_t R, size_t P >
template< typename T >
inline cv::Mat
olbp_t< R, P >::operator() (const cv::Mat& src) const
{
    return this->operator()< T > (src, src);
}

template< typename T >
inline cv::Mat
olbp_t< 1, 8 >::operator() (const cv::Mat& src, const cv::Mat& ref) const
{
    LBP_ASSERT (src.size () == ref.size ());
    LBP_ASSERT (src.type () == ref.type ());

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

#pragma omp parallel for
    for (int i = 1; i < src.rows - 1; ++i) {
        const T* p = src.ptr< T > (i - 1);
        const T* q = src.ptr< T > (i);
        const T* r = src.ptr< T > (i + 1);

        T* s = dst.ptr< T > (i);

        const T* t = ref.ptr< T > (i);

        for (int j = 1; j < src.cols - 1; ++j) {
            const auto center = t [j];

#define T(a, b, c) ((center >= a [b]) << c)
            const unsigned char value =
                T (p, j - 1, 3) |
                T (p, j    , 2) |
                T (p, j + 1, 1) |
                T (r, j - 1, 5) |
                T (r, j    , 6) |
                T (r, j + 1, 7) |
                T (q, j - 1, 4) |
                T (q, j + 1, 0);
#undef T

            s [j] = U [value];
        }
    }

    return dst;
}

template< typename T >
inline cv::Mat
olbp_t< 1, 8 >::operator() (const cv::Mat& src) const
{
    return this->operator()< T > (src, src);
}

template< typename T >
inline cv::Mat
olbp_t< 2, 10 >::operator() (const cv::Mat& src, const cv::Mat& ref) const
{
    LBP_ASSERT (src.size () == ref.size ());
    LBP_ASSERT (src.type () == ref.type ());

    cv::Mat dst (src.size (), CV_8UC1);

#pragma omp parallel for
    for (int i = 2; i < src.rows - 2; ++i) {
        const T* p = src.ptr< T > (i - 2);
        const T* q = src.ptr< T > (i - 1);
        const T* r = src.ptr< T > (i);
        const T* s = src.ptr< T > (i + 1);
        const T* t = src.ptr< T > (i + 2);

        T* u = dst.ptr< T > (i);

        const T* w = ref.ptr< T > (i);

        for (int j = 2; j < src.cols - 2; ++j) {
            const auto center = w [j];

#define T(a, b, c) ((center >= a [b]) << c)
            const unsigned value =
                T (p, j - 1, 3) |
                T (p, j + 1, 2) |
                T (q, j - 2, 4) |
                T (q, j + 2, 1) |
                T (r, j - 2, 5) |
                T (r, j + 2, 0) |
                T (s, j - 2, 6) |
                T (s, j + 2, 9) |
                T (t, j - 1, 7) |
                T (t, j + 1, 8);
#undef T

            u [j] = olbp_detail::uniformity_measure (value, 12);
        }
    }

    return dst;
}

template< typename T >
inline cv::Mat
olbp_t< 2, 10 >::operator() (const cv::Mat& src) const
{
    return this->operator()< T > (src, src);
}

template< typename T >
inline cv::Mat
olbp_t< 2, 12 >::operator() (const cv::Mat& src, const cv::Mat& ref) const
{
    LBP_ASSERT (src.size () == ref.size ());
    LBP_ASSERT (src.type () == ref.type ());

    cv::Mat dst (src.size (), CV_8UC1);

#pragma omp parallel for
    for (int i = 2; i < src.rows - 2; ++i) {
        const T* p = src.ptr< T > (i - 2);
        const T* q = src.ptr< T > (i - 1);
        const T* r = src.ptr< T > (i);
        const T* s = src.ptr< T > (i + 1);
        const T* t = src.ptr< T > (i + 2);

        T* u = dst.ptr< T > (i);

        const T* w = ref.ptr< T > (i);

        for (int j = 2; j < src.cols - 2; ++j) {
            const auto center = w [j];

#define T(a, b, c) ((center >= a [b]) << c)
            const unsigned value =
                T (p, j - 1,   4) |
                T (p, j,       3) |
                T (p, j + 1,   2) |
                T (q, j - 2,   5) |
                T (q, j + 2,   1) |
                T (r, j - 2,   6) |
                T (r, j + 2,   0) |
                T (s, j - 2,   7) |
                T (s, j + 2,  11) |
                T (t, j - 1,   8) |
                T (t, j,       9) |
                T (t, j + 1,  10);
#undef T

            u [j] = olbp_detail::uniformity_measure (value, 12);
        }
    }

    return dst;
}

template< typename T >
inline cv::Mat
olbp_t< 2, 12 >::operator() (const cv::Mat& src) const
{
    return this->operator()< T > (src, src);
}

template< typename T >
inline cv::Mat
olbp_t< 2, 16 >::operator() (const cv::Mat& src, const cv::Mat& ref) const
{
    LBP_ASSERT (src.size () == ref.size ());
    LBP_ASSERT (src.type () == ref.type ());

    cv::Mat dst (src.size (), CV_8UC1);

#pragma omp parallel for
    for (int i = 2; i < src.rows - 2; ++i) {
        const T* p = src.ptr< T > (i - 2);
        const T* q = src.ptr< T > (i - 1);
        const T* r = src.ptr< T > (i);
        const T* s = src.ptr< T > (i + 1);
        const T* t = src.ptr< T > (i + 2);

        T* u = dst.ptr< T > (i);

        const T* w = ref.ptr< T > (i);

        for (int j = 2; j < src.cols - 2; ++j) {
            const auto center = w [j];

#define T(a, b, c) ((center >= a [b]) << c)
            const unsigned value =
                T (p, j - 1,   5) |
                T (p, j,       4) |
                T (p, j + 1,   3) |
                T (q, j - 2,   7) |
                T (q, j - 1,   6) |
                T (q, j + 1,   2) |
                T (q, j + 2,   1) |
                T (r, j - 2,   8) |
                T (r, j + 2,   0) |
                T (s, j - 2,   9) |
                T (s, j - 1,  10) |
                T (s, j + 1,  14) |
                T (s, j + 2,  15) |
                T (t, j - 1,  11) |
                T (t, j,      12) |
                T (t, j + 1,  13);
#undef T

            u [j] = olbp_detail::uniformity_measure (value, 16);
        }
    }

    return dst;
}

template< typename T >
inline cv::Mat
olbp_t< 2, 16 >::operator() (const cv::Mat& src) const
{
    return this->operator()< T > (src, src);
}

} // namespace lbp
