# LBP Library

A collection of Local Binary Pattern (LBP) algorithms.

## OLBP

The implementation of requires single-channel images for input and allows for
any radius and number of neighborhood pixels ([2001Ojala](#2001Ojala)).

The neighborhood is computed using the formula in the cited paper, with the
elements visited counter-clockwise, starting with the element to the right of
center. This follows the paper formula for calculating the neighboring and takes
into consideration the Y-axis flipping in the OpenCV coordinate system. All
neighborhoods are visited in the same fashion.

I have supplied a specialization for a more efficient traversal of neighbors for
*(8,1)* case. The usage is pretty straightforward:

    // One plane frame:
    auto op = lbp::olbp< unsigned char, 1, 8 >;
    auto result = op (frame);

## OC-LBP

The implementation requires a 3-plane image and assumes that incoming frames are
RGB (not BGR). It computes a set of 6 frames out of each incoming frame, where
each of the six is a plain application of Ojala LBP operator of radius 1 and 8
neighbors between pairs of planes ([2002Mäenpää](#2002Mäenpää), [2003Mäenpää](#2003Mäenpää)):

- R v. R
- G v. G
- B v. B
- R v. G
- R v. B
- G v. B

Other operators can be used as well.

The code to apply the operator and display the images can be as simple as this
(using Boost format):

    const auto op = lbp::oclbp< unsigned char, 1, 8 >;
    const auto images = op (frame);

    size_t i = 0;
    
    for (const auto& image : images) {
        imshow ((format ("OC-LBP, frame %1%") % (i++)).str (), image);
    }
    
## VAR-LBP

The implementation requires a single-plane, floating point image for input
([2002Ojala](#2002Ojala)). It applies the operator over a neighborhood of 8 pixels, using
the Welford online algorithm for calculating variance. Usage:

    const auto op = lbp::varlbp< unsigned char, 1, 8 >;
    const auto result = op (frame);

Visualizing the result requires scaling the resulting floating point image
values to fit in `[0,1]`.

## CS-LBP

As in Center-Symmetric LBP ([2006Heikkilä](#2006Heikkilä)). The implementation follows the
description in chapter 2.2, *Feature Extraction with Center-Symmetric Local
Binary Patterns*. The usage is straighforward, create  the operator object, then
apply to image:

    auto op = lbp::cslbp< float, 2, 8 >;
    const auto result = op (frame, 0.05);

There is no requirement on the input image type other than being single-channel.

The resulting Mat type is the smallest type that can accommodate the result of
the operator. E.g., a neighborhood of 12, makes 6 comparisons, generating values
in the range [0,2<sup>5</sup>], and it requires at least 8 bits to store it,
i.e., an unsigned char.

## CS-LDP

The operator amounts to computing the second derivative in the center pixel (it
detects a minimum or maximum in the center pixel) and concatenates the bits for
each center-symmetric triplet of pixels ([2011Xue](#2011Xue)).

## SILTP

Because of the choice of states per pixel (3: 01, 10, 00) there is a maximum of
16 neighboring pixels, resulting in a 32 bit long descriptor, the maximum
integral OpenCV type ([2010Liao](#2010Liao)).

## CS-SILTP2

The 2D center-symmetric version of the above ([2014Wu](#2014Wu)).

## CS-SILTP

The full operator described in ([2014Wu](#2014Wu)).

## E-LBP

Described in [2012Mdakane](#2012Mdakane) -- an application of both Ojala LBP
operator ([2001Ojala](#2001Ojala)) and the Ojala VAR-LBP operator ([2002Ojala](#2002Ojala)). 
It is implemented as a standalone example with a composition of elementary
operations:

- conversion of image to grayscale
- application of Ojala LBP operator
- histogram equalization
- conversion of image to floating point
- application of VAR-LBP operator
- last histogram equalization

OLBP and VAR-LBP radius and neighborhood size can be different, see the
example. 

## XCS-LBP

A variant of CS-LBP (see [2006Heikkilä](#2006Heikkilä)) that incorporates the value of the
center pixel in the descriptor ([2015Silva](#2015Silva)). As with CS-LBP, the resulting
Mat type is the smallest type that has enough bits to encode the descriptor, and
is dependent on the (half) size of the neighborhood.

There is no explanation on the omission of a threshold similar to the one in
CS-LBP other than: "It is worth noting that the threshold function does not need
a user-defined threshold value, contrary to CS-LBP". It is not clear how an
explicit global threshold is replaced by "...by thresholding the neighbourhood
of each pixel with the center value...". Therefore, I left in place a threshold
argument, similar to the one in CS-LBP, defaulted to 0.

## SCS-LBP

Based on CS-LBP (see [2006Heikkilä](#2006Heikkilä)), with a temporal extension that
thresholds on the variance of the central pixel. As with CS-LBP, the resulting
Mat type is the smallest type that has enough bits to encode the descriptor, and
is dependent on the (half) size of the neighborhood. However, the size of the
descriptor is half the size of the neighborhood plus one, so that might bump the
descriptor in the next integral size ([2010Xue](#2010Xue)).

The implementation uses the algorithms in [2009Gil-Jiménez](#2009Gil-Jiménez) to estimate the
mean and the variance.

## Parallelization

The implementations are reasonably streamlined within the boundaries of their
idioms. E.g., each operator returns a new frame(s) out of the input image and
when accounting for the cost of that paradigm the code is as fast or faster than
other similar implementations. Optimizing the implementation is something I wish
to do with greater care after some more tinkering with the design.

In the meantime I am using OpenMP in places with nice (expected) results. More
improvements, soon.

## Utilities

There are a bunch of one-line internal utilities in the library that are useful
to me, mostly related to conversion between formats, scaling, etc. Find them in
`src/utils.cpp`. 

I copied Eric Niebler's getline range code and adapted it to fetching frames
from a `cv::VideoCapture` object. It allows for some sweetness:

    cv::VideoCapture cap = ...
    for (const auto& frame : lbp::getframes_from (cap)) {
        // ... use the frame
    }

The code for that is in `include/lbp/frame_range.hpp`, enjoy.

## CMake and Windows

No.

## References

<a name="2001Ojala">[2001Ojala]</a> Ojala, Timo, Matti Pietikäinen, and Topi
Mäenpää. "A generalized local binary pattern operator for multiresolution gray
scale and rotation invariant texture classification." *International Conference
on Advances in Pattern Recognition.* Springer, Berlin, Heidelberg, 2001. 

<a name="2002Mäenpää">[2002Mäenpää]</a> Mäenpää T, Pietikäinen M & Viertola J
(2002) Separating color and pattern information for color texture
discrimination. *Proc. 16th International Conference on Pattern Recognition*,
Québec City, Canada, 1: 668–671.

<a name="2003Mäenpää">[2003Mäenpää]</a> Mäenpää, Topi. The local binary pattern
approach to texture analysis: extensions and applications. Oulun yliopisto,
2003.

<a name="2002Ojala">[2002Ojala]</a> Ojala, Timo, Matti Pietikäinen, and Topi
Mäenpää. "Multiresolution gray-scale and rotation invariant texture
classification with local binary patterns." *IEEE Transactions on pattern
analysis and machine intelligence* 24.7 (2002): 971-987.

<a name="2006Heikkilä">[2006Heikkilä]</a> Heikkilä, Marko, Matti Pietikäinen, and
Cordelia Schmid. "Description of interest regions with center-symmetric local
binary patterns." ICVGIP. Vol. 6. 2006.

<a name="2011Xue">[2011Xue]</a> Xue, Gengjian, et al. "Hybrid center-symmetric
local pattern for dynamic background subtraction." *Multimedia and Expo (ICME),
2011 IEEE International Conference on.* IEEE, 2011. 

<a name="2010Liao">[2010Liao]</a> Liao, Shengcai, et al. "Modeling pixel process
with scale invariant local patterns for background subtraction in complex
scenes." *Computer Vision and Pattern Recognition (CVPR), 2010 IEEE Conference
on.* IEEE, 2010. 

<a name="2014Wu">[2014Wu]</a> Wu, Hefeng, et al. "Real-time background
subtraction-based video surveillance of people by integrating local texture
patterns." *Signal, Image and Video Processing* 8.4 (2014): 665-676. 

<a name="2012Mdakane">[2012Mdakane]</a> Mdakane, L., and F. Van den
Bergh. "Extended local binary pattern features for improving settlement type
classification of quickbird images." (2012). 

<a name="2015Silva">[2012Mdakane]</a> Silva, Caroline, Thierry Bouwmans, and
Carl Frélicot. "An eXtended center-symmetric local binary pattern for background
modeling and subtraction in videos." International Joint Conference on Computer
Vision, Imaging and Computer Graphics Theory and Applications,
VISAPP 2015. 2015. 

<a name="2010Xue">[2010Xue]</a> Xue, Gengjian, Jun Sun, and Li Song. "Dynamic
background subtraction based on spatial extended center-symmetric local binary
pattern." *Multimedia and Expo (ICME), 2010 IEEE International Conference
on.* IEEE, 2010. 

<a name="2009Gil-Jiménez">[2009Gil-Jiménez]</a> Gil-Jiménez, Pedro, et
al. "Continuous variance estimation in video surveillance sequences with high
illumination changes." *Signal Processing* 89.7 (2009): 1412-1416.
