#ifndef LBP_COMMON_HPP
#define LBP_COMMON_HPP

#include <lbp/defs.hpp>

#include <opencv2/imgproc.hpp>

#include <chrono>
#include <functional>

namespace lbp {

inline cv::Mat
absdiff (const cv::Mat& lhs, const cv::Mat& rhs) {
    cv::Mat dst;
    return cv::absdiff (lhs, rhs, dst), dst;
}

inline std::pair< double, double >
minmax (const cv::Mat& src) {
    double a, b;
    minMaxLoc (src, &a, &b);
    return { a, b };
}

inline std::pair< double, double >
visual_range (int type) {
    using namespace std;

    switch (type % 8) {
#define T(a, b)                                     \
        case a: return {                            \
            double ((numeric_limits< b >::min)()),  \
            double ((numeric_limits< b >::max)()) }

        T (0, char unsigned);      // CV_8U
        T (1, char);               // CV_8S
        T (2, int short unsigned); // CV_16U
        T (3, int short);          // CV_16S
        T (4, int);                // CV_32S

#undef T

    case 5:
    case 6:
        return { 0., 1. }; // CV_32F, CV_64F
    default:
        LBP_ASSERT (0);
    }
}

inline cv::Mat
convert (const cv::Mat& src, int type, double a = 1., double b = 0.) {
    cv::Mat dst (src.size (), type);
    return src.convertTo (dst, type, a, b), dst;
}

inline cv::Mat
equalize (const cv::Mat& src) {
    double a, b, c, d;

    tie (a, b) = minmax (src);
    tie (c, d) = visual_range (src.type ());

    const auto s = (d - c) / (b - a);

    cv::Mat dst;
    return convert (src, src.type (), s, c - s * a);
};

inline cv::Mat
gray2float (const cv::Mat& src) {
    LBP_ASSERT (1 == src.channels ());

    double a, b, c, d;

    tie (a, b) = minmax (src);
    tie (c, d) = visual_range (CV_32F);

    const auto s = (d - c) / (b - a);

    cv::Mat dst;
    return convert (src, CV_32F, s, c - s * a);
}

inline cv::Mat
bgr2gray (const cv::Mat& src) {
    cv::Mat dst;
    return cvtColor (src, dst, cv::COLOR_BGR2GRAY), dst;
}

inline cv::Mat
pow (const cv::Mat& src, double d) {
    cv::Mat dst;
    return cv::pow (src, d, dst), dst;
}

inline cv::Mat
sqrt (const cv::Mat& src) {
    cv::Mat dst;
    return cv::sqrt (src, dst), dst;
}

struct frame_delay {
    explicit frame_delay (size_t value = 40 /* milliseconds */)
        : value_ (value),
          begin_ (std::chrono::high_resolution_clock::now ())
        { }

    bool wait_for_key (int key) const {
        using namespace std::chrono;

        int passed = duration_cast< milliseconds > (
            high_resolution_clock::now () - begin_).count ();

        int remaining = value_ - passed;

        if (remaining < 1)
            remaining = 1;

        return key == cv::waitKey (remaining);
    }

private:
    int value_;
    std::chrono::high_resolution_clock::time_point begin_;
};

namespace detail {
template< typename > struct undefined_value_t;
} // namespace detail

template< typename T > constexpr int
opencv_type = detail::undefined_value_t< T >::value;

#define OPENCV_TYPEDEF(T, t) template< > constexpr int opencv_type< T > = t

OPENCV_TYPEDEF (char,               CV_8S);
OPENCV_TYPEDEF (char unsigned,      CV_8U);
OPENCV_TYPEDEF (int short,          CV_16S);
OPENCV_TYPEDEF (int short unsigned, CV_16U);
OPENCV_TYPEDEF (int,                CV_32S);
OPENCV_TYPEDEF (int unsigned,       CV_32S);
OPENCV_TYPEDEF (float,              CV_32F);
OPENCV_TYPEDEF (double,             CV_64F);

#undef OPENCV_TYPE_DEF

} // namespace lbp

#endif // LBP_COMMON_HPP
