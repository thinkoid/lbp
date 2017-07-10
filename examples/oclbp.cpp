// -*- mode: c++; -*-

#include <iostream>
using namespace std;

#include <lbp/frame_range.hpp>
#include <lbp/oclbp.hpp>
#include <lbp/utils.hpp>

#include <options.hpp>
#include <run.hpp>

#include <boost/format.hpp>
using fmt = boost::format;

static void
f (cv::VideoCapture& cap, const options_t& opts) {
    const bool display = opts.have ("display");

    const auto op = lbp::oclbp_t { };

    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 0 };

        const auto images = op (frame);

        if (display) {
            size_t i = 0;

            for (const auto& image : images) {
                imshow ((fmt ("Opponent Color LBP #%1%") % (i++)).str (), image);
            }
        }

        if (temp.wait_for_key (27))
            break;
    }
}

int main (int argc, char** argv) {
    program_options_from (argc, argv);
    return run_with (f, global_options ()), 0;
}
