// -*- mode: c++ -*-

#include <lbp/oclbp.hpp>
#include <lbp/utils.hpp>

#include <vector>
using namespace std;

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

namespace lbp {
namespace oclbp_detail {

static Mat
do_oclbp (const Mat& src, const Mat& ref)
{
    cv::Mat dst (src.size (), CV_8UC1);

    for (int i = 1; i < src.rows - 1; ++i) {
        const unsigned char* p = src.ptr< unsigned char > (i - 1);
        const unsigned char* q = src.ptr< unsigned char > (i);
        const unsigned char* r = src.ptr< unsigned char > (i + 1);

        unsigned char* s = dst.ptr< unsigned char > (i);

        const unsigned char* t = ref.ptr< unsigned char > (i);

        for (int j = 1; j < src.cols - 1; ++j) {
            const unsigned char c = t [j];

#define T(x, y, z) ((c >= x [y]) << z)
            const unsigned char value =
                T (p, j - 1, 7) |
                T (p, j    , 6) |
                T (p, j + 1, 5) |
                T (r, j - 1, 1) |
                T (r, j    , 2) |
                T (r, j + 1, 3) |
                T (q, j - 1, 0) |
                T (q, j + 1, 4);
#undef T

            s [j] = value;
        }
    }

    return dst;
}

static vector< Mat >
oclbp (const Mat& src)
{
    vector< Mat > planes (3);
    split (src, planes);

    vector< Mat > dsts (6);

    dsts [0] = do_oclbp (planes [0], planes [0]);
    dsts [1] = do_oclbp (planes [1], planes [1]);
    dsts [2] = do_oclbp (planes [2], planes [2]);

    dsts [3] = do_oclbp (planes [1], planes [0]);
    dsts [4] = do_oclbp (planes [2], planes [0]);
    dsts [5] = do_oclbp (planes [2], planes [1]);

    return dsts;
}

} // namespace oclbp_detail

std::vector< cv::Mat >
oclbp_t::operator() (const cv::Mat& frame) const
{
    LBP_ASSERT (3 == frame.channels ());
    return oclbp_detail::oclbp (frame);
}

} // namespace lbp
