#ifndef LBP_FRAME_RANGE_HPP
#define LBP_FRAME_RANGE_HPP

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <boost/range/iterator_range.hpp>

namespace lbp {

struct frames_iterator : boost::iterator_facade <
    frames_iterator, cv::Mat, std::forward_iterator_tag > {

    frames_iterator () : pvc_ { }, pmat_ { } { }

    frames_iterator (cv::VideoCapture* pvc, cv::Mat* pmat)
        : pvc_(pvc), pmat_(pmat) {
        increment ();
    }

private:
    friend class boost::iterator_core_access;

    void increment () {
        *pvc_ >> *pmat_;

        if (pmat_->empty ())
            *this = frames_iterator { };
    }

    bool equal (frames_iterator const& that) const {
        return pmat_ == that.pmat_;
    }

    cv::Mat& dereference() const {
        return *pmat_;
    }

    cv::VideoCapture* pvc_;
    cv::Mat* pmat_;
};

using frames_range_base = boost::iterator_range< frames_iterator >;

struct frames_range_data {
    cv::Mat mat_;
};

struct frames_range : private frames_range_data, frames_range_base {
    explicit frames_range (cv::VideoCapture& vc)
        : frames_range_base (
            frames_iterator { &vc, &mat_ },
            frames_iterator { })
    { }
};

static inline frames_range
getframes_from (cv::VideoCapture& vc) {
    return frames_range { vc };
}

} // namespace lbp

#endif // LBP_FRAME_RANGE_HPP
