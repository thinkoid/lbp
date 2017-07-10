// -*- mode: c++; -*-

#include <iostream>
using namespace std;

#include <lbp/frame_range.hpp>
#include <lbp/olbp.hpp>
#include <lbp/utils.hpp>

#include <options.hpp>
#include <run.hpp>

static void
f (cv::VideoCapture& cap, const options_t& opts) {
    const bool display = opts.have ("display");

    const auto op = lbp::olbp_t< unsigned char, 1, 8 > { };

    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 0 };

        const auto result = lbp::convert (
            op (lbp::gray_from (frame)), CV_8U, 255./9);

        if (display) {
            imshow ("Ojala (2001) LBP operator", result);
        }

        if (temp.wait_for_key (27))
            break;
    }
}

int main (int argc, char** argv) {
    program_options_from (argc, argv);
    return run_with (f, global_options ()), 0;
}
