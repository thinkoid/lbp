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

template< size_t > constexpr int opencv_type;

#define OPENCV_TYPEDEF(n, t) template< > constexpr int opencv_type< n > = t

OPENCV_TYPEDEF ( 8, CV_8U);
OPENCV_TYPEDEF (16, CV_16U);
OPENCV_TYPEDEF (32, CV_32S);

#undef OPENCV_TYPE_DEF

} // namespace lbp

#endif // LBP_COMMON_HPP
