#ifndef LBP_OCLBP_HPP
#define LBP_OCLBP_HPP

#include <lbp/defs.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <opencv2/core.hpp>

#include <boost/hana/integral_constant.hpp>
#include <boost/hana/fold.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/size.hpp>
#include <boost/hana/tuple.hpp>

//
// @book{maenpaa2003local,
//   title={The local binary pattern approach to texture analysis: extensions and applications},
//   author={M{\"a}enp{\"a}{\"a}, Topi},
//   year={2003},
//   publisher={Oulun yliopisto}
// }
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

const auto do_oclbp = [](auto N, auto S) {
    return [=](const cv::Mat& src, const cv::Mat& ref, size_t i, size_t j) {
        using namespace hana::literals;

        const auto c = S (ref, i, j);

        return boost::hana::fold_left (
            N, 0, [&, shift = 0](auto accum, auto x) mutable {
                const auto g = S (src, i + x [0_c], j + x [1_c]);
                return accum | (size_t (c >= g) << shift++);
            });
    };
};

template< typename T, size_t R, size_t P >
auto oclbp = [](const cv::Mat& src, const cv::Mat& ref) {
    cv::Mat dst (src.size (), CV_8U);

    auto op = do_oclbp (
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
    namespace hana = boost::hana;
    using namespace hana::literals;

#define T(a, b) hana::tuple_c< int, a, b >
    static constexpr auto combinations = hana::make_tuple (
        T (0, 0), T (1, 1), T (2, 2), T (1, 0), T (2, 0), T (2, 1));
#undef T

    vector< cv::Mat > dsts;
    dsts.reserve (hana::size (combinations).value);

    vector< cv::Mat > planes (3);
    split (src, planes);

    auto op = oclbp_detail::oclbp< T, R, P >;

    hana::for_each (combinations, [&](auto x) {
            dsts.emplace_back (op (planes [x [0_c]], planes [x [1_c]]));
        });

    return dsts;
};

} // namespace lbp

#endif // LBP_OCLBP_HPP
