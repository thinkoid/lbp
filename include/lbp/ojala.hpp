#ifndef LBP_OJALA_HPP
#define LBP_OJALA_HPP

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

template< typename T, size_t R, size_t P, typename C = std::greater_equal< T > >
struct ojala_t {
    using compare_type = C;

public:
    explicit ojala_t (const compare_type& = compare_type ());

    cv::Mat operator() (const cv::Mat&) const;

private:
    size_t operator() (const cv::Mat&, size_t, size_t) const;

private:
    std::vector< std::pair< int, int > > N;
    compare_type cmp_;
};

template< typename T, typename C >
struct ojala_t< T, 1, 8, C > {
    using compare_type = C;

public:
    explicit ojala_t (const compare_type& cmp = compare_type ())
        : cmp_ (cmp)
        { }

    cv::Mat operator() (const cv::Mat&) const;

private:
    compare_type cmp_;
};

} // namespace lbp

#include <lbp/ojala.cc>

#endif // LBP_OJALA_HPP
