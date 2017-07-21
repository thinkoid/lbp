// -*- mode: c++; -*-

#include <lbp/olbp.hpp>

namespace lbp {

template< typename T >
auto olbp< T, 1, 8 > = [](const cv::Mat& src) {
    static auto U = olbp_detail::uniformity_measure< T, 8 >;

    cv::Mat dst (src.size (), CV_8U);

#pragma omp parallel for
    for (int i = 1; i < src.rows - 1; ++i) {
        const T* p = src.ptr< T > (i - 1);
        const T* q = src.ptr< T > (i);
        const T* r = src.ptr< T > (i + 1);

        T* pdst = dst.ptr< T > (i);

        for (int j = 1; j < src.cols - 1; ++j) {
            const auto center = q [j];

#define T(a, b, c) ((center >= a [b]) << c)
            const auto value =
                T (p, j - 1, 3) |
                T (p, j    , 2) |
                T (p, j + 1, 1) |
                T (q, j - 1, 4) |
                T (q, j + 1, 0) |
                T (r, j - 1, 5) |
                T (r, j    , 6) |
                T (r, j + 1, 7);
#undef T

            pdst [j] = U (value);
        }
    }

    return dst;
};

} // namespace lbp
