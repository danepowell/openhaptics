#ifndef MASS_H
#define MASS_H

#include <HDU/hduVector.h>

#pragma pack(push)
#pragma pack(4)

class Mass
{
public:
	hduVector3Df position;
	hduVector3Df velocity;

	float mass;

	//Is this ball held in position?
	bool fixed;

	//Vertex normal for this Mass
	hduVector3Df normal;
};

#endif	//MASS_H
