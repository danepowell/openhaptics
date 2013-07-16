#ifndef SPRING_H
#define SPRING_H

#include "Mass.h"

class Spring
{
public:
	//Indices of the Mass at either end of the spring
	int imass1;
	int imass2;

	//Tension in the spring
	float tension;

	float springConstant;
	float naturalLength;

	Spring()	:	imass1(-1), imass2(-1)
	{}
	~Spring()
	{}
};

#endif	//SPRING_H