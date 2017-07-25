// -*- mode: c++ -*-

#include <lbp/utils.hpp>

#include <functional>
#include <limits>
using namespace std;

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

namespace lbp {

std::pair< double, double >
minmax (const Mat& src) {
    double a, b;
    minMaxLoc (src, &a, &b);
    return { a, b };
}

std::pair< double, double >
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

Mat
convert (const Mat& src, int type, double a, double b) {
    Mat dst (src.size (), type);
    return src.convertTo (dst, type, a, b), dst;
}

Mat
equalize (const Mat& src) {
    double a, b, c, d;

    tie (a, b) = minmax (src);
    tie (c, d) = visual_range (src.type ());

    const auto s = (d - c) / (b - a);

    Mat dst;
    return convert (src, src.type (), s, c - s * a);
};

Mat
gray2float (const Mat& src) {
    LBP_ASSERT (1 == src.channels ());

    double a, b, c, d;

    tie (a, b) = minmax (src);
    tie (c, d) = visual_range (CV_32F);

    const auto s = (d - c) / (b - a);

    Mat dst;
    return convert (src, CV_32F, s, c - s * a);
}

Mat
bgr2gray (const Mat& src) {
    Mat dst;
    return cvtColor (src, dst, COLOR_BGR2GRAY), dst;
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

    return key == waitKey (remaining);
}

} // namespace lbp
