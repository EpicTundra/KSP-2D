#pragma once

#define PI 3.14159265358979323846
#define E 2.718281828459045
#define atmoHeight 30
#include "Parts.h"

typedef struct {
    double planetPos[2];  //In relation to the craft, a negative x will render it to the right.
    float zoom;           //Zoom of ten, a distance of 100 = 10
    float planetRad;
    float planetMass;
    float orbitRenderPos[4]; //major, minor, e, and argument of periapsis
    int mode; //0 = gameplay, 1 = map
    bool pressingModeSwitch;
} game_t;

typedef struct {

    int engSelect;

    float heading;
    float throttle;
    float fuel;
    float mass;  //Engine mass in tons + vehicle mass
    float dragCeo; //Of total rocket, implemetation of calculator later
    float size;

    Engine rocketEngine;

    double speed[2];

    int calcOrbit;
} rocket_t;

void speedUpdate(rocket_t* rocket, float x, float y, float planetRad, float planetMass);
float aero(rocket_t* rocket, float speed, float height);
float accel(rocket_t* rocket);
float gravity(double r2, float trig, int i, float planetMass);
void updateHeading(rocket_t* rocket, float value);
void updateThrottle(rocket_t* rocket, float value);

void rocketInit(rocket_t* rocket);
void gameInit(game_t* game);

bool inOrbit(float height, float mass, float speed); //Base off of calcorbit paramaters
float deltaV(float vex, float massW, float massD);

void calcOrbit(game_t* game, rocket_t* rocket);
void disOrb(game_t game, rocket_t rocket, float zoom, game_t *gamestr); //DEPRECATED Calculates for renderOrb, float (*renderPositions)[101][3] 
void renderOrbit(game_t* game, float zoom); //renders orbit

int tNow;
int tLast;
int dt;
