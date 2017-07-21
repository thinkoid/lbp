#ifndef LBP_DETAIL_SAMPLING_HPP
#define LBP_DETAIL_SAMPLING_HPP

#include <lbp/defs.hpp>

namespace lbp {
namespace detail {

template< typename T >
auto basic_sampler = [](const cv::Mat& src, size_t i, size_t j) {
    return src.at< T > (i, j);
};

template< typename T >
auto nearest_sampler = [](const cv::Mat& src, float i, float j) {
    return src.at< T > (int (i + .5), int (j + .5));
};

template< typename T >
auto bilinear_sampler = [](const cv::Mat& src, float i, float j) {
    const int ii = int (i);
    const int ij = int (j);

    assert (0 <= ii && ii < src.rows - 1);
    assert (0 <= ij && ij < src.cols - 1);

    const double fx = j - ij;
    const double fy = i - ii;

    const double w [] = {
        (1 - fx) * (1 - fy),
        (    fx) * (1 - fy),
        (    fx) * (    fy),
        (1 - fx) * (    fy)
    };

    return T (
        w [0] * src.at< T > (ii,     ij    ) +
        w [1] * src.at< T > (ii,     ij + 1) +
        w [2] * src.at< T > (ii + 1, ij    ) +
        w [3] * src.at< T > (ii + 1, ij + 1));
};

}}

#endif // LBP_DETAIL_SAMPLING_HPP
