#pragma once
#include "Util.h"

typedef struct
{
	char* name;
	float thrust;
	float mass;
	float massF;
} Engine;

void PartInit(Engine* engine, char* name, float massF, float mass, float thrust);