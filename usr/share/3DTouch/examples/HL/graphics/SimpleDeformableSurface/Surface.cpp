/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  Surface.cpp

Description:

  Deformable surface implemented using particle system.

*******************************************************************************/

#include "Surface.h"

static const double kDefaultTotalMass = 1;

Surface::Surface() :
    surfaceParticlesX(0),
    surfaceParticlesZ(0),
    massProportion(kDefaultTotalMass)
{
    SetSpecularColor(1.0, 1.0, 1.0, 1.0);
    SetAmbientColor(1.0, 1.0, 1.0, 1.0);
    SetDiffuseColor(1.0, 1.0, 1.0, 1.0);
    SetShininess(100);
}

Surface::~Surface()
{
}

void Surface::ConstructSurface(const int inSurfaceParticles, const double inSurfaceSize)
{
    ps->StartConstructingSystem();
    ps->ClearSystem();
        
    surfaceParticlesX = inSurfaceParticles;
    surfaceParticlesZ = inSurfaceParticles;
    surfaceSizeX = inSurfaceSize;
    surfaceSizeZ = inSurfaceSize;
    surfaceSpacingX = (float)surfaceSizeX / (surfaceParticlesX - 1);
    surfaceSpacingZ = (float)surfaceSizeZ / (surfaceParticlesZ - 1);
        
    vertexNormalsCached.resize(surfaceParticlesX * surfaceParticlesZ);
    vertexNormals.resize(surfaceParticlesX * surfaceParticlesZ);

    InvalidateVertexCache();

    int p = 0; // particle index
    for (int i=0; i<surfaceParticlesX; i++)
    {
        for (int k=0; k<surfaceParticlesZ; k++)
        {
            ps->AddParticle(-surfaceSizeX / 2 + i*surfaceSpacingX, 0, -surfaceSizeZ / 2 + k*surfaceSpacingZ);

            // nail down the edges
            if (i == 0 || k == 0 || i == surfaceParticlesX-1 || k == surfaceParticlesZ-1)
                ps->AddNailConstraint(p);

            // connect to previous particle in x direction
            if (i > 0)
                ps->AddSpringConstraint(p, p-surfaceParticlesX, 0);
                        
            // connect to previous particle in z direction
            if (k > 0)
                ps->AddSpringConstraint(p, p-1, 0);

            p++;
        }
    }
        
    // Re-evaluate particle masses with new number of particles
    SetMassProportion(massProportion);
    ps->FinishConstructingSystem();
}


void Surface::DrawSurface(void)
{
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);

    GLfloat noMat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, noMat);
        
    for (int i=0; i<surfaceParticlesX-1; i++)
    {
        // draw two traiangles for each square
        glBegin(GL_TRIANGLE_STRIP);
        for (int k=0; k<surfaceParticlesZ; k++)
        {
            glNormal3dv(GetSurfaceVertexNormal(i, k));
            glVertex3dv(GetSurfacePosition(i, k));

            glNormal3dv(GetSurfaceVertexNormal(i+1, k));
            glVertex3dv(GetSurfacePosition(i+1, k));
        }
        glEnd();
    }

}

void Surface::DrawSurfaceNormals(void)
{
    hduVector3Dd normVertex;
        
    glColor3f( 0.0, 0.0, 1.0);
    glBegin(GL_LINES);

    for (int i=0; i<surfaceParticlesX; i++)
    {
        for (int k=0; k<surfaceParticlesZ; k++)
        {
            normVertex = GetSurfaceVertexNormal(i, k);

            hduVector3Dd p = GetSurfacePosition(i,k);

            // length of the normals will be 1/20 of the surface size
            hduVector3Dd normVertexEnd = p + normVertex * (surfaceSizeX / 20);

            glVertex3dv(p);
            glVertex3dv(normVertexEnd);
        }
    }

    glEnd();
}

void Surface::CalculateNormal(hduVector3Dd &normal, const hduVector3Dd &v1, const hduVector3Dd &v2, const hduVector3Dd &v3)
{
    normal = (v1 - v2).crossProduct(v2 - v3);
}

void Surface::InvalidateVertexCache(void)
{
    vertexNormalsCached.assign(vertexNormalsCached.size(), false);
}

//
// Support on-the-fly calculation and caching
// By caching, we don't have to re-calculate the normal when
// we need to re-visit the same vertex while rendering
// an adjacent strip of triangles.
//
const hduVector3Dd& Surface::GetSurfaceVertexNormal(int i, int k)
{
    std::vector<bool>::reference isCached = vertexNormalsCached.at(i * surfaceParticlesZ + k);
    hduVector3Dd& normal = vertexNormals.at(i * surfaceParticlesZ + k);
        
    if (!isCached)
    {
        CalculateSurfaceVertexNormal(normal, i, k);
        isCached = true;
    }

    return normal;
}

void Surface::CalculateSurfaceVertexNormal(hduVector3Dd &normVertex, int i, int k)
{
    hduVector3Dd normSurface(0,0,0);
    hduVector3Dd normAccum(0,0,0);
        
    // take the 4 adjacent "triangles" and average their normals
    //                                           
    //         Triangle order: 1, 2, 3, 4
    //                                           
    //                / | \                      
    //              / 1 | 3 \                   
    //  |---->x   /____ . ____\     |---->i             
    //  |         \     |     /     |          
    // \|/          \ 2 | 4 /      \|/           
    //  z             \ | /         k            
    //                                           
    //      (normVertex is at the ".")      

    if (i > 0 && k > 0)
    {
        CalculateNormal(normSurface, 
                        GetSurfacePosition(i, k),
                        GetSurfacePosition(i, k-1),
                        GetSurfacePosition(i-1, k));
        normAccum += normSurface;
    }
    if (i > 0 && k < surfaceParticlesZ-1)
    {
        CalculateNormal(normSurface, 
                        GetSurfacePosition(i, k),
                        GetSurfacePosition(i-1, k),
                        GetSurfacePosition(i, k+1));
        normAccum += normSurface;
    }
    if (i < surfaceParticlesX-1 && k > 0)
    {
        CalculateNormal(normSurface, 
                        GetSurfacePosition(i, k),
                        GetSurfacePosition(i+1, k),
                        GetSurfacePosition(i, k-1));
        normAccum += normSurface;
    }
    if (i < surfaceParticlesX-1 && k < surfaceParticlesZ-1)
    {
        CalculateNormal(normSurface, 
                        GetSurfacePosition(i, k),
                        GetSurfacePosition(i, k+1),
                        GetSurfacePosition(i+1, k));
        normAccum += normSurface;
    }
        
    // normalize (average) the result
    normVertex = normAccum / normAccum.magnitude();
}

Particle *Surface::GetSurfaceParticle(int i, int k)
{
    return ps->particles[i*surfaceParticlesZ + k];
}

const hduVector3Dd& Surface::GetSurfacePosition(int i, int k)
{
    return ps->particles[i*surfaceParticlesZ + k]->x;
}

void Surface::SetSpecularColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    matSpecular[0] = r;
    matSpecular[1] = g;
    matSpecular[2] = b;
    matSpecular[3] = a;
}

void Surface::SetAmbientColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    matAmbient[0] = r;
    matAmbient[1] = g;
    matAmbient[2] = b;
    matAmbient[3] = a;
}

void Surface::SetDiffuseColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    matDiffuse[0] = r;
    matDiffuse[1] = g;
    matDiffuse[2] = b;
    matDiffuse[3] = a;
}

void Surface::SetShininess(GLfloat s)
{
    matShininess[0] = s;
}

void Surface::SetMassProportion(double inMass)
{
    massProportion = inMass;
    int hangingParticles = (surfaceParticlesX - 2);
    if (hangingParticles > 0)
        Particle::SetMass(massProportion / hangingParticles);
}

/******************************************************************************/
