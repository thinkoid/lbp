// -*- mode: c++; -*-

#include <iostream>
using namespace std;

#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <lbp/frame_range.hpp>
#include <lbp/ojala.hpp>
#include <lbp/utils.hpp>

#include <options.hpp>
#include <run.hpp>

#include <opencv2/core.hpp>
using namespace cv;

//
// options_t::options_t is specific to each example:
//
options_t::options_t (int argc, char** argv) {
    {
        auto tmp = std::make_pair (
            "program", po::variable_value (std::string (argv [0]), false));
        map_.insert (tmp);
    }

    po::options_description generic ("Generic options");
    po::options_description config ("Configuration options");

    generic.add_options ()
        ("version,v", "version")
        ("help,h", "this");

    config.add_options ()
        ("display,d", "display frames.")

        ("input,i",   po::value< std::string > ()->default_value ("0"),
         "input (file or stream index).")

        ("algorithm,a", po::value< std::string > ()->default_value ("ojala"),
         "algorithm");

    desc_ = boost::make_shared< po::options_description > ();

    desc_->add (generic);
    desc_->add (config);

    store (po::command_line_parser (argc, argv).options (*desc_).run (), map_);

    notify (map_);
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

////////////////////////////////////////////////////////////////////////

static void
process_ojala (cv::VideoCapture& cap, const options_t& opts) {
    const bool display = opts.have ("display");

    const lbp::ojala_t< unsigned char, 1, 8 > c;

    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 40 };

        const auto result = lbp::convert (
            c (lbp::scale_frame (frame)), CV_8U, 255. / 9);

        if (display) {
            imshow ("Ojala (2001), radius 1, samples 8", result);
        }

        if (temp.wait_for_key (27))
            break;
    }
}

static void
process (cv::VideoCapture& cap, const options_t& opts)
{
    const auto a = opts ["algorithm"].as< string > ();

    cout << a << endl;

    if (a == "ojala") {
        process_ojala (cap, opts);
    }
}

////////////////////////////////////////////////////////////////////////

int main (int argc, char** argv) {
    program_options_from (argc, argv);
    return run_with (process, global_options ()), 0;
}
