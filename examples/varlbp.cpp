// -*- mode: c++; -*-

#include <iostream>
using namespace std;

#include <lbp/frame_range.hpp>
#include <lbp/varlbp.hpp>
#include <lbp/utils.hpp>

#include <options.hpp>
#include <run.hpp>

#include <opencv2/core.hpp>
using namespace cv;

static void
f (cv::VideoCapture& cap, const options_t& opts) {
    const bool display = opts.have ("display");

    auto op = lbp::varlbp< float, 1, 8 >;

    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 0 };

        const auto src = lbp::gray2float (lbp::bgr2gray (frame));
        const auto result = op (src);

        if (display) {
            imshow ("Ojala (2001) VAR operator", lbp::equalize (result));
        }

        if (temp.wait_for_key (27))
            break;
    }
}

int main (int argc, char** argv) {
    program_options_from (argc, argv);
    return run_with (f, global_options ()), 0;
}
