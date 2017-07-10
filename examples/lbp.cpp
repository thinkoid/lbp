// -*- mode: c++; -*-

#include <iostream>
using namespace std;

#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <lbp/frame_range.hpp>
#include <lbp/oclbp.hpp>
#include <lbp/olbp.hpp>
#include <lbp/utils.hpp>
#include <lbp/varlbp.hpp>

#include <options.hpp>
#include <run.hpp>

#include <opencv2/core.hpp>
using namespace cv;

#include <boost/timer/timer.hpp>
namespace timers = boost::timer;

template< typename F >
void process_olbp (cv::VideoCapture& cap, const options_t& opts, const F& f) {
    const bool display = opts.have ("display");

    stringstream ss;
    ss << "Olbp (2001), radius " << F::radius << ", samples " << F::size;

    const auto title = ss.str ();

    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 1 };

        const auto result = lbp::convert (
            f (lbp::gray_from (frame)), CV_8U, 255. / (F::size + 1));

        if (display) {
            imshow (title, result);
        }

        if (temp.wait_for_key (27))
            break;
    }
}

static void
process_olbp_1_8 (cv::VideoCapture& cap, const options_t& opts) {
    process_olbp (cap, opts, lbp::olbp_t< unsigned char, 1, 8 > { });
}

static void
process_olbp_2_10 (cv::VideoCapture& cap, const options_t& opts) {
    process_olbp (cap, opts, lbp::olbp_t< unsigned char, 2, 10 > { });
}

static void
process_olbp_2_12 (cv::VideoCapture& cap, const options_t& opts) {
    process_olbp (cap, opts, lbp::olbp_t< unsigned char, 2, 12 > { });
}

static void
process_olbp_2_16 (cv::VideoCapture& cap, const options_t& opts) {
    process_olbp (cap, opts, lbp::olbp_t< unsigned char, 2, 16 > { });
}

static void
process_olbp_3_32 (cv::VideoCapture& cap, const options_t& opts) {
    process_olbp (cap, opts, lbp::olbp_t< unsigned char, 3, 32 > { });
}

static void
process_oclbp (cv::VideoCapture& cap, const options_t& opts) {
    const bool display = opts.have ("display");

    const lbp::oclbp_t op;

    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 1 };

        const auto images = op (frame);

        if (display) {
            const string s ("Opponent Color LBP");

            size_t i = 0;

            for (size_t i = 0; i < images.size (); ++i) {
                stringstream ss (s);
                ss << " part " << i;
                imshow (ss.str (), images [i]);
            }
        }

        if (temp.wait_for_key (27))
            break;
    }
}

static void
process_varlbp (cv::VideoCapture& cap, const options_t& opts) {
    const bool display = opts.have ("display");

    const lbp::varlbp_t op;

    for (const auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 1 };

        const auto src = lbp::float_from (lbp::gray_from (frame));
        const auto result = op (src);

        double a, b;
        minMaxLoc (result, &a, &b);

        const auto normal = lbp::convert (result, CV_32FC1, 1./b);

        if (display) {
            imshow ("Variance measure operator", normal);
        }

        if (temp.wait_for_key (27))
            break;
    }
}

////////////////////////////////////////////////////////////////////////

static void
process (cv::VideoCapture& cap, const options_t& opts)
{
    const auto a = opts ["algorithm"].as< string > ();

    cout << a << endl;

    if (a == "olbp-1-8") {
        process_olbp_1_8 (cap, opts);
    }
    else if (a == "olbp-2-10") {
        process_olbp_2_10 (cap, opts);
    }
    else if (a == "olbp-2-12") {
        process_olbp_2_12 (cap, opts);
    }
    else if (a == "olbp-2-16") {
        process_olbp_2_16 (cap, opts);
    }
    else if (a == "olbp-3-32") {
        process_olbp_3_32 (cap, opts);
    }
    else if (a == "oclbp") {
        process_oclbp (cap, opts);
    }
    else if (a == "varlbp") {
        process_varlbp (cap, opts);
    }
}

////////////////////////////////////////////////////////////////////////

static void
program_options_from (int& argc, char** argv) {
    bool complete_invocation = false;

    options_t program_options (argc, argv);

    if (program_options.have ("version")) {
        std::cout << "OpenCV v3.1\n";
        complete_invocation = true;
    }

    if (program_options.have ("help")) {
        std::cout << program_options.description () << std::endl;
        complete_invocation = true;
    }

    if (complete_invocation)
        exit (0);

    global_options (program_options);
}

int main (int argc, char** argv) {
    program_options_from (argc, argv);
    return run_with (process, global_options ()), 0;
}
