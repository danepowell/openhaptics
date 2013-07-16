/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

  hduDecompose.h

Description: 

 Declarations for polar_decomp algorithm from Graphics Gems IV,
 by Ken Shoemake <shoemake@graphics.cis.upenn.edu>

*******************************************************************************/

#ifndef hduDecompose_H_
#define hduDecompose_H_

/**** Decompose.h - Basic declarations ****/
typedef struct {double x, y, z, w;} Quat; /* Quaternion */
enum QuatPart {X, Y, Z, W};
typedef Quat HVect; /* Homogeneous 3D vector */
typedef double HMatrix[4][4]; /* Right-handed, for column vectors */
typedef struct {
    HVect t;	/* Translation components */
    Quat  q;	/* Essential rotation	  */
    Quat  u;	/* Stretch rotation	  */
    HVect k;	/* Stretch factors	  */
    double f;	/* Sign of determinant	  */
} AffineParts;

#ifdef __cplusplus
extern "C" {
#endif

double polar_decomp(HMatrix M, HMatrix Q, HMatrix S);
HVect spect_decomp(HMatrix S, HMatrix U);
Quat snuggle(Quat q, HVect *k);
void decomp_affine(HMatrix A, AffineParts *parts);
void invert_affine(AffineParts *parts, AffineParts *inverse);

#ifdef __cplusplus
}
#endif

#endif /* hduDecompose_H_ */
