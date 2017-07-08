#ifndef LBP_OLBP_HPP
#define LBP_OLBP_HPP

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

template< typename T, size_t R, size_t P >
struct olbp_t {
    static constexpr size_t radius = R;
    static constexpr size_t size = P;

public:
    explicit olbp_t ();

    cv::Mat operator() (const cv::Mat&) const;
    cv::Mat operator() (const cv::Mat&, const cv::Mat&) const;

private:
    size_t operator() (const cv::Mat&, const cv::Mat&, size_t, size_t) const;

private:
    std::vector< std::pair< int, int > > N;
};

template< typename T >
struct olbp_t< T, 1, 8 > {
    static constexpr size_t radius = 1;
    static constexpr size_t size = 8;

public:
    cv::Mat operator() (const cv::Mat&) const;
    cv::Mat operator() (const cv::Mat&, const cv::Mat&) const;
};

template< typename T >
struct olbp_t< T, 2, 12 > {
    static constexpr size_t radius = 2;
    static constexpr size_t size = 12;

public:
    cv::Mat operator() (const cv::Mat&) const;
    cv::Mat operator() (const cv::Mat&, const cv::Mat&) const;
};

template< typename T >
struct olbp_t< T, 2, 16 > {
    static constexpr size_t radius = 2;
    static constexpr size_t size = 16;

public:
    cv::Mat operator() (const cv::Mat&) const;
    cv::Mat operator() (const cv::Mat&, const cv::Mat&) const;
};

} // namespace lbp

#include <lbp/olbp.cc>

#endif // LBP_OLBP_HPP
