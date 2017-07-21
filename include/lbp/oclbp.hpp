#ifndef LBP_OCLBP_HPP
#define LBP_OCLBP_HPP

#include <lbp/defs.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <opencv2/core.hpp>

#include <boost/hana/fold.hpp>
#include <boost/hana/tuple.hpp>

//
// @book{maenpaa2003local,
//   title={The local binary pattern approach to texture analysis: extensions and applications},
//   author={M{\"a}enp{\"a}{\"a}, Topi},
//   year={2003},
//   publisher={Oulun yliopisto}
// }
//

namespace lbp {
namespace oclbp_detail {

template< typename T >
auto do_oclbp = [](auto neighborhood, auto sampler) {
    return [=](const cv::Mat& src, const cv::Mat& ref, size_t i, size_t j) {
        namespace hana = boost::hana;
        using namespace hana::literals;

        const auto c = ref.at< T > (i, j);

        return hana::fold_left (
            neighborhood, 0, [&, S = 0](auto accum, auto x) mutable {
                const auto g = sampler (src, i + x [0_c], j + x [1_c]);
                return accum | (size_t (c >= g) << S++);
            });
    };
};

template< typename T, size_t R, size_t P >
auto oclbp = [](const cv::Mat& src, const cv::Mat& ref) {
    cv::Mat dst (src.size (), CV_8U);

    auto op = do_oclbp< T > (
        detail::circular_neighborhood< R, P >,
        detail::nearest_sampler< T >);

#pragma omp parallel for
    for (size_t i = R; i < src.rows - R; ++i) {
        for (size_t j = R; j < src.cols - R; ++j) {
            dst.at< unsigned char > (i, j) = op (src, ref, i, j);
        }
    }

    return dst;
};

} // namespace oclbp_detail

template< typename T, size_t R, size_t P >
auto oclbp = [](const cv::Mat& src) {
    static constexpr size_t arr [][2] = {
        { 0, 0 }, { 1, 1 }, { 2, 2 }, { 1, 0 }, { 2, 0 }, { 2, 1 }
    };

    vector< cv::Mat > dsts (6);

    vector< cv::Mat > planes (3);
    split (src, planes);

    auto op = oclbp_detail::oclbp< T, R, P >;

#pragma omp parallel for
    for (size_t i = 0; i < sizeof arr / sizeof *arr; ++i) {
        dsts [i] = op (planes [arr [i][0]], planes [arr [i][1]]);
    }

    return dsts;
};

} // namespace lbp

#endif // LBP_OCLBP_HPP
