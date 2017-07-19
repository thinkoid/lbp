// -*- mode: c++; -*-

#include <lbp/cslbp.hpp>
#include <lbp/utils.hpp>
#include <lbp/detail/neighborhoods.hpp>

#include <functional>

namespace lbp {

template< size_t R, size_t P >
template< typename T >
inline cv::Mat
cslbp_t< R, P >::operator() (const cv::Mat& src, const T& epsilon) const
{
    using namespace cslbp_detail;
    using namespace hana::literals;

    const auto N = detail::semicircular_neighborhood< R, P >;

    auto dst = cv::Mat (
        src.size (), opencv_type< (sizeof (value_type) << 3) >,
        cv::Scalar (0));

#pragma omp parallel for
    for (size_t i = R; i < src.rows - R; ++i) {
        for (size_t j = R; j < src.cols - R; ++j) {
            dst.at< value_type > (i, j) = hana::fold (
                N, 0, [&, S = 0](auto accum, auto x) mutable {
                    const auto a = src.at< T > (i + x [0_c], j + x [1_c]);
                    const auto b = src.at< T > (i - x [0_c], j - x [1_c]);

                    //
                    // Note: the paper describes the operator:
                    //
                    // CS-LBP_{R,N,T}(x,y) = \sum_0^{(N/2)-1}s(n_i+n_{i+(N/2)})2^i,
                    // s(x)= \[ \begin{cases}
                    //          1 & x \geq \epsilon
                    //          0 & x <    \epsilon
                    //       \end{cases}
                    //       \]
                    //
                    // with no mention of the sign of the difference between
                    // pixels and how that should be interpreted. I take it to mean
                    // the absolute difference value.
                    //

                    return accum + ((std::abs (a - b) >= epsilon) << S++);
                });
        }
    }

    return dst;
}

} // namespace lbp
