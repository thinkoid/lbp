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

auto bgr2gray = [](const cv::Mat& src) {
    return lbp::bgr2gray (src);
};

auto gray2float = [](const cv::Mat& src) {
    return lbp::gray2float (src);
};

auto equalize = [](const cv::Mat& src) {
    return lbp::equalize (src);
};

static void
f (cv::VideoCapture& cap, const options_t& opts) {
    const bool display = opts.have ("display");

    auto olbp = lbp::olbp< unsigned char, 2, 10 >;
    auto varlbp = lbp::varlbp< float, 1, 8 >;

    auto op = hana::compose (
        equalize, varlbp, gray2float, equalize, olbp, bgr2gray);

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
