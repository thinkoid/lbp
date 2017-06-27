// -*- mode: c++ -*-

#include <iostream>
#include <exception>
using namespace std;

#include <lbp/frame_range.hpp>
#include <lbp/ojala.hpp>
#include <lbp/utils.hpp>

#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <boost/timer/timer.hpp>

////////////////////////////////////////////////////////////////////////

static void
run_from_stream (VideoCapture& cap) {
    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 0 };

        Mat scaled_frame = lbp::scale_frame (frame), output_frame;

        {
            using namespace boost::timer;

            auto_cpu_timer timer;
            output_frame = lbp::ojala (scaled_frame);
        }

        imshow ("Original (Ojala) LBP algorithm", output_frame);

        if (temp.wait_for_key (27))
            break;
    }
}

static void
run_from_file (const char* s) {
    const fs::path& filename (s);
    const fs::path ext = filename.extension ();

    if (ext == ".avi" || ext == ".mp4" || ext == ".mov") {
        VideoCapture cap;

        if (cap.open (filename.generic_string ())) {
            cap.set (CV_CAP_PROP_FPS, 25);
            run_from_stream (cap);
        }
    }
    else
        cerr << "unsupported file type" << endl;
}

static void
run_from_camera () {
    VideoCapture cap;

    if (cap.open (0)) {
        cap.set (CV_CAP_PROP_FPS, 25);
        run_from_stream (cap);
    }
}

static void
run_with (const char* s) {
    if (fs::exists (s))
        run_from_file (s);
    else
        run_from_camera ();
}

////////////////////////////////////////////////////////////////////////

int main (int argc, char** argv) {
    return run_with (argv [1]), 0;
}
