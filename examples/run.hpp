// -*- mode: c++ -*-

#ifndef LBP_EXAMPLES_RUN_HPP
#define LBP_EXAMPLES_RUN_HPP

#include <iostream>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <options.hpp>

template< typename Function >
void
run_from_stream (Function f, cv::VideoCapture& cap, const options_t& opts) {
    f (cap, opts);
}

template< typename Function >
void
run_from_file_with (Function f, const options_t& opts) {
    const fs::path& filename (opts ["input"].as< std::string > ());
    const fs::path ext = filename.extension ();

    if (ext == ".avi" || ext == ".mp4" || ext == ".mpg" || ext == ".mov") {
        cv::VideoCapture cap;

        if (cap.open (filename.generic_string ())) {
            cap.set (CV_CAP_PROP_FPS, 25);
            run_from_stream (f, cap, opts);
        }
    }
    else
        std::cerr << "unsupported file type" << std::endl;
}

template< typename Function >
void
run_from_camera_with (Function f, const options_t& opts) {
    cv::VideoCapture cap;

    const int stream = std::stoi (opts ["input"].as< std::string > ());

    if (cap.open (stream)) {
        cap.set (CV_CAP_PROP_FPS, 25);
        run_from_stream (f, cap, opts);
    }
}

template< typename Function >
void
run_with (Function f, const options_t& opts) {
    const auto input = opts ["input"].as< std::string > ();

    if (fs::exists (input))
        run_from_file_with (f, opts);
    else
        run_from_camera_with (f, opts);
}

#endif // LBP_EXAMPLES_RUN_HPP
