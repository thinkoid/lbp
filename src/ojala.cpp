#include <lbp/utils.hpp>
#include <lbp/ojala.hpp>

#include <opencv2/imgproc.hpp>

namespace lbp {
namespace {

template< typename T >
inline cv::Mat
do_ojala (const cv::Mat& src) {
    static const int arr [] = {
        0, 1, 1, 2, 1, 9, 2, 3, 1, 9, 9, 9, 2, 9, 3, 4,
        1, 9, 9, 9, 9, 9, 9, 9, 2, 9, 9, 9, 3, 9, 4, 5,
        1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        2, 9, 9, 9, 9, 9, 9, 9, 3, 9, 9, 9, 4, 9, 5, 6,
        1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        2, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        3, 9, 9, 9, 9, 9, 9, 9, 4, 9, 9, 9, 5, 9, 6, 7,
        1, 2, 9, 3, 9, 9, 9, 4, 9, 9, 9, 9, 9, 9, 9, 5,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 6,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7,
        2, 3, 9, 4, 9, 9, 9, 5, 9, 9, 9, 9, 9, 9, 9, 6,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7,
        3, 4, 9, 5, 9, 9, 9, 6, 9, 9, 9, 9, 9, 9, 9, 7,
        4, 5, 9, 6, 9, 9, 9, 7, 5, 6, 9, 7, 6, 7, 7, 8
    };

    auto dst = cv::Mat (src.size (), src.type (), cv::Scalar (0));

    for (int i = 1; i < src.rows - 1; ++i) {
        const T* p = src.ptr< T > (i - 1);
        const T* q = src.ptr< T > (i);
        const T* r = src.ptr< T > (i + 1);

        T* s = dst.ptr< T > (i);
        ++s;

        for (int j = 1; j < src.cols - 1; ++j, ++p, ++q, ++r, ++s) {
            T t = q [1];

            unsigned u =
                ((p [0] >= t) << 7) +
                ((p [1] >= t) << 6) +
                ((p [2] >= t) << 5) +
                ((q [0] >= t)) +
                ((q [2] >= t) << 4) +
                ((r [0] >= t) << 1) +
                ((r [1] >= t) << 2) +
                ((r [2] >= t) << 3);

            BOOST_ASSERT (0 <= u && u < sizeof arr / sizeof *arr);
            s [0] = arr [u];
        }
    }

    return dst;
}

} // namespace detail

cv::Mat
ojala (const cv::Mat& src) {
    BOOST_ASSERT (1 == src.channels ());

#define T(x, y) case x: return do_ojala< y > (src)

    switch (src.type ()) {
        T (CV_8SC1,                 char);
        T (CV_8UC1,        unsigned char);
        T (CV_16SC1,           short int);
        T (CV_16UC1,  unsigned short int);
        T (CV_32SC1,                 int);
        T (CV_32FC1,               float);
        T (CV_64FC1,              double);
    default:
        throw std::invalid_argument ("unsupported type");
    }

#undef T

}

} // namespace lbp
