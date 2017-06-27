#include <lbp/utils.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace lbp {
namespace detail {

template< typename T >
struct threshold_base {
    using value_type = T;

    explicit threshold_base (value_type threshold, value_type value)
        : threshold (threshold), value (value)
        { }

    value_type threshold, value;
};

template< typename T >
struct threshold_binary : threshold_base< T > {
    using  base_type = threshold_base< T >;
    using value_type = typename base_type::value_type;

    explicit threshold_binary (value_type threshold, value_type value)
        : base_type (threshold, value)
        { }

    value_type
    operator() (const T& arg) const {
        if (arg > base_type::threshold)
            return base_type::value;
        else
            return value_type { };
    }
};

template< typename T >
struct threshold_binary_inv : threshold_base< T > {
    using  base_type = threshold_base< T >;
    using value_type = typename base_type::value_type;

    explicit threshold_binary_inv (value_type threshold, value_type value)
        : base_type (threshold, value)
        { }

    value_type
    operator() (const T& arg) const {
        if (arg < base_type::threshold)
            return base_type::value;
        else
            return value_type { };
    }
};

template< typename T >
struct threshold_trunc : threshold_base< T > {
    using  base_type = threshold_base< T >;
    using value_type = typename base_type::value_type;

    explicit threshold_trunc (value_type threshold, value_type value)
        : base_type (threshold, value)
        { }

    value_type
    operator() (const T& arg) const {
        if (arg > base_type::threshold)
            return base_type::threshold;
        else
            return arg;
    }
};

template< typename T >
struct threshold_tozero : threshold_base< T > {
    using  base_type = threshold_base< T >;
    using value_type = typename base_type::value_type;

    explicit threshold_tozero (value_type threshold, value_type value)
        : base_type (threshold, value)
        { }

    value_type
    operator() (const T& arg) const {
        if (arg > base_type::threshold)
            return arg;
        else
            return value_type { };
    }
};

template< typename T >
struct threshold_tozero_inv : threshold_base< T > {
    using  base_type = threshold_base< T >;
    using value_type = typename base_type::value_type;

    explicit threshold_tozero_inv (value_type threshold, value_type value)
        : base_type (threshold, value)
        { }

    value_type
    operator() (const T& arg) const {
        if (arg < base_type::threshold)
            return arg;
        else
            return value_type { };
    }
};

template< typename T >
inline cv::Mat
threshold (const cv::Mat& src, const T& t) {
    using value_type = typename T::value_type;

    cv::Mat dst = src.clone ();

    size_t rows = dst.rows;
    size_t cols = dst.cols;

    if (dst.isContinuous ()) {
        cols *= rows;
        rows = 1;
    }

    for (size_t i = 0; i < rows; ++i) {
        value_type* p = reinterpret_cast< value_type* > (
            dst.ptr< unsigned char > (i));

        for (size_t j = 0; j < cols; ++j)
            p [j] = t (p [j]);
    }

    return dst;
}

template< typename T >
inline cv::Mat
threshold (const cv::Mat& src, T threshold_, T value, int type) {
    switch (type) {
    case CV_THRESH_BINARY:
        return threshold (src, threshold_binary< T > (threshold_, value));

    case CV_THRESH_BINARY_INV:
        return threshold (src, threshold_binary_inv< T > (threshold_, value));

    case CV_THRESH_TRUNC:
        return threshold (src, threshold_trunc< T > (threshold_, value));

    case CV_THRESH_TOZERO:
        return threshold (src, threshold_tozero< T > (threshold_, value));

    case CV_THRESH_TOZERO_INV:
        return threshold (src, threshold_tozero_inv< T > (threshold_, value));

    default:
        throw std::invalid_argument ("unsupported thresholding type");
    }
}

inline cv::Mat
scale_frame (cv::Mat& frame, double factor) {
    cv::Mat bw;
    cv::cvtColor (frame, bw, cv::COLOR_BGR2GRAY);

    cv::Mat tiny;
    cv::resize (bw, tiny, cv::Size (), factor, factor, cv::INTER_LINEAR);

    // this adds a lot of noise in dark images
    // cv::equalizeHist (tiny, tiny);

    return tiny;
}

} // namespace detail

cv::Mat
threshold (const cv::Mat& src, double threshold_, double value, int type) {
    cv::Mat dst;

    switch (src.type ()) {
    case CV_8U:
    case CV_32F:
        cv::threshold (src, dst, threshold_, value, type);
        break;

#define T(x, y) case x:                                                 \
        dst = detail::threshold< y > (src, threshold_, value, type);    \
        break

        T (CV_16U, unsigned short int);
        T (CV_16S, short int);
        T (CV_32S, int);

#undef T

    default:
        throw std::invalid_argument ("unsupported array type");
    }

    return dst;
}

cv::Mat
convert (cv::Mat src, int t, double a, double b) {
    return src.convertTo (src, t, a, b), src;
}

cv::Mat
float_from (const cv::Mat& src, double scale, double offset) {
    return convert (src, CV_32F, scale, offset);
}

cv::Mat
mono_from (const cv::Mat& src, double scale, double offset) {
    return convert (src, CV_8U, scale, offset);
}

cv::Mat
mono_integral_from (cv::Mat src, double scale, double offset) {
    return convert (src, CV_32S, scale, offset);
}

cv::Mat
median_blur (const cv::Mat& src, int size) {
    cv::Mat dst;
    return cv::medianBlur (src, dst, size), dst;
}

cv::Mat
multiply (const cv::Mat& lhs, const cv::Mat& rhs) {
    cv::Mat dst;
    return cv::multiply (lhs, rhs, dst), dst;
}

cv::Mat
convert_color (const cv::Mat& src, int type) {
    cv::Mat dst;
    return cv::cvtColor (src, dst, type), dst;
}

int
gray_from (int r, int g, int b) {
    return double (r + g + b) / 3;
}

int
gray_from (const cv::Vec3b& arg) {
    return gray_from (arg [0], arg [1], arg [2]);
}

cv::Mat
gray_from (const cv::Mat& src) {
    return convert_color (src, cv::COLOR_BGR2GRAY);
}

cv::Mat
power_of (const cv::Mat& arg, double power) {
    cv::Mat dst;
    return cv::pow (arg, power, dst), dst;
}

cv::Mat
absdiff (const cv::Mat& lhs, const cv::Mat& rhs) {
    cv::Mat dst;
    return cv::absdiff (lhs, rhs, dst), dst;
}

cv::Mat
square_of (const cv::Mat& arg) {
    return power_of (arg, 2.0);
}

cv::Mat
bitwise_and (const cv::Mat& lhs, const cv::Mat& rhs) {
    cv::Mat dst;
    return cv::bitwise_and (lhs, rhs, dst), dst;
}

cv::Mat
bitwise_and (const cv::Mat& lhs, const cv::Mat& rhs, const cv::Mat& mask) {
    cv::Mat dst;
    return cv::bitwise_and (lhs, rhs, dst, mask), dst;
}

cv::Mat
bitwise_not (cv::Mat src) {
    cv::Mat dst;
    return cv::bitwise_not (src, dst), dst;
}

cv::Mat
bitwise_not (cv::Mat src, const cv::Mat& mask) {
    return cv::bitwise_not (src, src, mask), src;
}

cv::Mat
mask (const cv::Mat& src, const cv::Mat& mask) {
    cv::Mat dst;
    return src.copyTo (dst, mask), dst;
}

cv::Mat
flip (const cv::Mat& src, int how) {
    cv::Mat dst;
    return cv::flip (src, dst, how), dst;
}

int
chebyshev (const cv::Vec3b& lhs, const cv::Vec3b& rhs) {
    int tmp [] = {
        std::abs (lhs [0] - rhs [0]),
        std::abs (lhs [1] - rhs [1]),
        std::abs (lhs [2] - rhs [2])
    };

    return (std::max)({ tmp [0], tmp [1], tmp [2] });
}

cv::Mat
scale_frame (cv::Mat& src, size_t to) {
    const double scale_factor = double (to) / src.cols;
    return detail::scale_frame (src, scale_factor);
}

frame_delay::frame_delay (size_t value)
    : value_ (value),
      begin_ (std::chrono::high_resolution_clock::now ())
{ }

bool
frame_delay::wait_for_key (int key) const {
    using namespace std::chrono;

    int passed = duration_cast< milliseconds > (
        high_resolution_clock::now () - begin_).count ();

    int remaining = value_ - passed;

    if (remaining < 1)
        remaining = 1;

    return key == cv::waitKey (remaining);
}

} // namespace lbp
