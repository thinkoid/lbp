// -*- mode: c++; -*-

#include <iostream>
using namespace std;

#include <lbp/frame_range.hpp>
#include <lbp/cslbp.hpp>
#include <lbp/csldp.hpp>
#include <lbp/utils.hpp>

#include <options.hpp>
#include <run.hpp>

using namespace cv;

static void
f (cv::VideoCapture& cap, const options_t& opts) {
    const bool display = opts.have ("display");

    auto op = lbp::csldp< float, 2, 12 >;

    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 0 };

        const auto dst = op (lbp::gray2float (lbp::bgr2gray (frame)), .01);

        if (display) {
            imshow ("Second Order Center-Symmetric LBP", lbp::equalize (dst));
        }

        if (temp.wait_for_key (27))
            break;
    }
}

int main (int argc, char** argv) {
    program_options_from (argc, argv);
    return run_with (f, global_options ()), 0;
}
