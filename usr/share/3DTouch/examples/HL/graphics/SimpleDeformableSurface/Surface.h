/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  Surface.h

Description:

  Deformable surface implemented using particle system.

*******************************************************************************/

#ifndef Surface_H_
#define Surface_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ParticleSystem.h"

class Surface  
{
public:
    Surface();
    virtual ~Surface();
        
    void SetParticleSystem(ParticleSystem *inPs) { ps = inPs; }
    void ConstructSurface(const int inSurfaceParticles, 
                          const double inSurfaceSize);
    void DrawSurface(void);
    void DrawSurfaceNormals(void);
    void InvalidateVertexCache(void);
    void SetSpecularColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void SetAmbientColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void SetDiffuseColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void SetShininess(GLfloat s);
    void SetMassProportion(double inMass);
    double GetMassProportion(void) { return massProportion; }

private:
    ParticleSystem *ps;

    std::vector<bool> vertexNormalsCached;
    std::vector<hduVector3Dd> vertexNormals;

    GLfloat matSpecular[4];
    GLfloat matAmbient[4];
    GLfloat matDiffuse[4];
    GLfloat matShininess[1];

    int surfaceParticlesX;
    int surfaceParticlesZ;
    double surfaceSizeX;
    double surfaceSizeZ;
    float surfaceSpacingX;
    float surfaceSpacingZ;

    double massProportion;

    void CalculateNormal(hduVector3Dd &normal, const hduVector3Dd &v1, 
                         const hduVector3Dd &v2, const hduVector3Dd &v3);
    const hduVector3Dd& GetSurfaceVertexNormal(int i, int k);
    void CalculateSurfaceVertexNormal(hduVector3Dd &normVertex, int i, int k);
    Particle *GetSurfaceParticle(int i, int j);
    const hduVector3Dd& GetSurfacePosition(int i, int j);
};

#endif // Surface_H_

/******************************************************************************/
