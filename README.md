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

## OCLBP

The implementation requires a 3-plane image and assumes that incoming frames are
RGB (not BGR). It computes a set of 6 frames out of each incoming frame, where
each of the six is a plain application of Ojala LBP operator of radius 1 and 8
neighbors between pairs of planes ([2002Mäenpää](#2002Mäenpa)): 

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
        imshow ((format ("OCLBP, frame %1%") % (i++)).str (), image);
    }
    
## VARLBP

The implementation requires a single-plane, floating point image for input
([2002Ojala](#2002Ojala)). It applies the operator over a neighborhood of 8 pixels, using
the Welford online algorithm for calculating variance. Usage:

    const auto op = lbp::varlbp< unsigned char, 1, 8 >;
    const auto result = op (frame);

Visualizing the result requires scaling the resulting floating point image
values to fit in `[0,1]`.

## CSLBP

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

## CSLDP

The operator amounts to computing the second derivative in the center pixel (it
detects a minimum or maximum in the center pixel) and concatenates the bits for
each center-symmetric triplet of pixels ([2011Xue](#2011Xue)).

## SILTP

Because of the choice of states per pixel (3: 01, 10, 00) there is a maximum of
16 neighboring pixels, resulting in a 32 bit long descriptor, the maximum
integral OpenCV type ([2010Liao](#2010Liao)).

## CS-SILTP2

The 2D center-symmetric version of the above ([2014Wu](#2014Wu)).

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

<a name="2002Ojala">[2002Ojala]</a> Ojala, Timo, Matti Pietikainen, and Topi
Maenpaa. "Multiresolution gray-scale and rotation invariant texture
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
