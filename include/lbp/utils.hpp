#ifndef LBP_COMMON_HPP
#define LBP_COMMON_HPP

#include <lbp/defs.hpp>
#include <opencv2/imgproc.hpp>

#include <chrono>

namespace lbp {

cv::Mat
absdiff (const cv::Mat&, const cv::Mat&);

std::pair< double, double >
minmax (const cv::Mat&);

std::pair< double, double >
visual_range (int);

cv::Mat
convert (const cv::Mat&, int type, double a = 1., double b = 0.);

cv::Mat
equalize (const cv::Mat&);

cv::Mat
gray2float (const cv::Mat&);

cv::Mat
bgr2gray (const cv::Mat&);

cv::Mat
pow (const cv::Mat&, double);

cv::Mat
sqrt (const cv::Mat&);

struct frame_delay {
    explicit frame_delay (size_t value = 40 /* milliseconds */);
    bool wait_for_key (int key) const;

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
