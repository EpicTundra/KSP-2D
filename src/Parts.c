#include "Parts.h"


void PartInit(Engine* engine, char* name, float massF, float mass, float thrust) {
    engine->name = name;
    engine->thrust = thrust * 1000000000;
    engine->mass = mass;
    engine->massF = massF;
};


	