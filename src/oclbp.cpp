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
    using value_type = unsigned char;

    cv::Mat dst (src.size (), CV_8UC1);

    for (int i = 1; i < int (src.rows) - 1; ++i) {
        const value_type* p = src.ptr< value_type > (i - 1);
        const value_type* q = src.ptr< value_type > (i);
        const value_type* r = src.ptr< value_type > (i + 1);

        value_type* s = dst.ptr< value_type > (i);

        const value_type* t = ref.ptr< value_type > (i);

        for (int j = 1; j < int (src.cols) - 1; ++j) {
#define T(x, y, z) ((t [j] > x [y]) << z)
            const value_type value =
                T (p, j - 1, 0) |
                T (p, j    , 1) |
                T (p, j + 1, 2) |
                T (r, j - 1, 6) |
                T (r, j    , 5) |
                T (r, j + 1, 4) |
                T (q, j - 1, 7) |
                T (q, j + 1, 3);
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
    return oclbp_detail::oclbp (convert_color (frame, CV_BGR2RGB));
}

} // namespace lbp
