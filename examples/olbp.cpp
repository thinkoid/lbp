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

    auto op = lbp::olbp< unsigned char, 2, 10 >;

    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 0 };

        const auto result = op (lbp::bgr2gray (frame));

        if (display) {
            imshow ("Ojala (2001) LBP operator", lbp::equalize (result));
        }

        if (temp.wait_for_key (27))
            break;
    }
}

int main (int argc, char** argv) {
    program_options_from (argc, argv);
    return run_with (f, global_options ()), 0;
}
