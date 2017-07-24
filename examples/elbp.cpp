// -*- mode: c++; -*-

#include <iostream>
using namespace std;

#include <lbp/frame_range.hpp>
#include <lbp/olbp.hpp>
#include <lbp/varlbp.hpp>
#include <lbp/utils.hpp>

#include <boost/hana/integral_constant.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/functional/compose.hpp>
namespace hana = boost::hana;

using namespace hana::literals;

#include <opencv2/highgui.hpp>
using namespace cv;

#include <options.hpp>
#include <run.hpp>

auto minmax_ = [](const Mat& src) {
    double a, b;
    minMaxLoc (src, &a, &b);
    return hana::make_tuple (a, b);
};

auto bgr2gray = [](const cv::Mat& src) {
    Mat dst;
    return cv::cvtColor (src, dst, cv::COLOR_BGR2GRAY), dst;
};

auto normalize_float = [&](const cv::Mat& src) {
    Mat dst;

    const auto x = minmax_ (src);
    src.convertTo (dst, CV_32F, 1./(x [1_c] - x [0_c]), -x [0_c]);

    return dst;
};

auto normalize_gray = [&](const cv::Mat& src) {
    Mat dst;

    const auto x = minmax_ (src);
    src.convertTo (dst, CV_8U, 255./(x [1_c] - x [0_c]), -x [0_c]);

    return dst;
};

auto null_op = [](const cv::Mat& src) {
    return src.clone ();
};

static void
f (cv::VideoCapture& cap, const options_t& opts) {
    const bool display = opts.have ("display");

    auto olbp = lbp::olbp< unsigned char, 1, 8 >;
    auto varlbp = lbp::varlbp< float, 1, 8 >;

    auto op = hana::compose (
        normalize_float, varlbp, normalize_float, normalize_gray, olbp,
        bgr2gray);

    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 0 };

        const auto dst = op (frame);

        if (display) {
            imshow ("Extended LBP (E-LBP) operator", dst);
        }

        if (temp.wait_for_key (27))
            break;
    }
}

int main (int argc, char** argv) {
    program_options_from (argc, argv);
    return run_with (f, global_options ()), 0;
}
