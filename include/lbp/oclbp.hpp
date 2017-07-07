#ifndef LBP_OCLBP_HPP
#define LBP_OCLBP_HPP

#include <lbp/defs.hpp>

#include <utility>
#include <vector>

#include <opencv2/core.hpp>

//
// @book{maenpaa2003local,
//   title={The local binary pattern approach to texture analysis: extensions and applications},
//   author={M{\"a}enp{\"a}{\"a}, Topi},
//   year={2003},
//   publisher={Oulun yliopisto}
// }
//

namespace lbp {

struct oclbp_t {
    std::vector< cv::Mat > operator() (const cv::Mat&) const;
};

} // namespace lbp

#endif // LBP_OCLBP_HPP
