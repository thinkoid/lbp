// -*- mode: c++ -*-

#include <lbp/varlbp.hpp>
#include <lbp/utils.hpp>

#include <vector>
using namespace std;

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

namespace lbp {
namespace varlbp_detail {

static double
variance_at (const Mat& src, size_t i, size_t j)
{
    static constexpr double W [][4] = {
        { 1.,        0.,        0.,        0.        },
        { 0.207107,  0.5,       0.0857864, 0.207107  },
        { 1,         0.,        0.,        0.        },
        { 0.5,       0.207107,  0.207107,  0.0857864 },
        { 0.,        0.,        1.,        0.        },
        { 0.207107,  0.0857864, 0.5,       0.207107  },
        { 0.,        1.,        0.,        0.        },
        { 0.0857864, 0.207107,  0.207107,  0.5       }
    };

    static constexpr int D [][4][2] = {
        { {  0,  1 }, {  0,  1 }, {  0,  1 }, {  0,  1 } },
        { { -1,  0 }, { -1,  1 }, {  0,  0 }, {  0,  1 } },
        { { -1,  0 }, { -1,  1 }, { -1,  0 }, { -1,  1 } },
        { { -1, -1 }, { -1,  0 }, {  0, -1 }, {  0,  0 } },
        { { -1, -1 }, { -1, -1 }, {  0, -1 }, {  0, -1 } },
        { {  0, -1 }, {  0,  0 }, {  1, -1 }, {  1,  0 } },
        { {  1, -1 }, {  1,  0 }, {  1, -1 }, {  1,  0 } },
        { {  0,  0 }, {  0,  1 }, {  1,  0 }, {  1,  1 } }
    };

    double x [8], d = 0., m = 0., s = 0.;

#define T(k)                                                            \
    x [k] =                                                             \
        W [k][0] * src.at< float > (i + D [k][0][0], j + D [k][0][1]) + \
        W [k][1] * src.at< float > (i + D [k][1][0], j + D [k][1][1]) + \
        W [k][2] * src.at< float > (i + D [k][2][0], j + D [k][2][1]) + \
        W [k][3] * src.at< float > (i + D [k][3][0], j + D [k][3][1])

    T (0); T (1); T (2); T (3); T (4); T (5); T (6); T (7);
#undef T

    //
    // Walford online algorithm for computing variance:
    //

#define T(k)                                    \
    d = x [k] - m;                              \
    m += d / (k + 1);                           \
    s += d * (x [k] - m)

    T (0); T (1); T (2); T (3); T (4); T (5); T (6); T (7);
#undef T

    return s / 7;
}

static inline Mat
varlbp (const Mat& src)
{
    Mat dst (src.size (), CV_32FC1);

#pragma omp parallel for
    for (int i = 1; i < src.rows - 1; ++i) {
        for (int j = 1; j < src.cols - 1; ++j) {
            dst.at< float > (i, j) = variance_at (src, i, j);
        }
    }

    return dst;
}

} // namespace varlbp_detail

cv::Mat
varlbp_t::operator() (const cv::Mat& src) const
{
    return varlbp_detail::varlbp (src);
}

} // namespace lbp
