# LBP Library

A collection of Local Binary Pattern (LBP) algorithms.

## OLBP

The implementation works with single-channel frames, with an arbitrary radius
and number of neighbors. There are a number of specializations for a more
efficient processing of neighbors/radii: *(8,1)*, *(10,2)*, *(12,2)*,
*(16,2)*. The central pixel for a neighborhood and the neighborhood itself can
belong to different planes, e.g.:

    // One plane frame:
    lbp::olbp_t< unsigned char, 1, 8 > op;
    auto result = op (frame);

    // Multiple channel (3, maybe?) frame:
    vector< Mat > planes;
    split (frame, planes);
    lbp::olbp_t< unsigned char, 1, 8 > op;
    // Operator applied on plane 0 neighbors, against centers from plane 1:
    auto result = op (plane [0], plane [1]);
    
## OCLBP

The implementation assumes that the incoming frames are multiple channel(3)
frames. It computes a set of 6 frames out of each incoming frame, where each of
the six is a plain OLBP of radius 1 and 8 neighbors between pairs of planes:

- R v. R
- G v. G
- B v. B
- R v. G
- R v. B
- G v. B

where the first is the reference plane (center) and the second is the neighbors
plane.
