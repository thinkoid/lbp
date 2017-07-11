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
                T (p, j - 1, 3) |
                T (p, j    , 2) |
                T (p, j + 1, 1) |
                T (r, j - 1, 5) |
                T (r, j    , 6) |
                T (r, j + 1, 7) |
                T (q, j - 1, 4) |
                T (q, j + 1, 0);
#undef T

            s [j] = value;
        }
    }

    return dst;
}

static vector< Mat >
oclbp (const Mat& src)
{
    static constexpr size_t arr [][2] = {
        { 0, 0 }, { 1, 1 }, { 2, 2 }, { 1, 0 }, { 2, 0 }, { 2, 1 }
    };

    vector< Mat > planes (3);
    split (src, planes);

    vector< Mat > dsts (6);

#pragma omp parallel for
    for (size_t i = 0; i < sizeof arr / sizeof *arr; ++i) {
        dsts [i] = do_oclbp (planes [arr [i][0]], planes [arr [i][1]]);
    }

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
