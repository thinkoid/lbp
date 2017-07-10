#ifndef LBP_VARLBP_HPP
#define LBP_VARLBP_HPP

#include <lbp/defs.hpp>

#include <utility>
#include <vector>

#include <opencv2/core.hpp>

//
// @inproceedings{Ojala:2001:GLB:646260.685274,
//  author = {Ojala, Timo and Pietik\"{a}inen, Matti and M\"{a}enp\"{a}\"{a}, Topi},
//  title = {A Generalized Local Binary Pattern Operator for Multiresolution Gray Scale and Rotation Invariant Texture Classification},
//  booktitle = {Proceedings of the Second International Conference on Advances in Pattern Recognition},
//  series = {ICAPR '01},
//  year = {2001},
//  isbn = {3-540-41767-2},
//  pages = {397--406},
//  numpages = {10},
//  url = {http://dl.acm.org/citation.cfm?id=646260.685274},
//  acmid = {685274},
//  publisher = {Springer-Verlag},
//  address = {London, UK, UK},
// }
//

namespace lbp {

struct varlbp_t {
    cv::Mat operator() (const cv::Mat&) const;
};

} // namespace lbp

#endif // LBP_VARLBP_HPP
