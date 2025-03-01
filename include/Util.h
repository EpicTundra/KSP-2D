#pragma once

#include "math.h"
#include "stdbool.h"
#include "stdlib.h"
#include "GL/gl.h"
#define square(value) pow(value, 2)
#define PI 3.14159265358979323846
#define METER2GL (1.0f / 240.0f)
#define DEGREETORAD 0.01745329251
#define RAD2DEG 57.2957795131
#define G 1

float clamp(float d, float min, float max);
double sign(double x);
float dist(float x, float y);
float crossproduct(float a[], float b[]);
void drawRect(float x, float y, float height, float width, float angle, float zoom, float r, float g, float b, bool inGameSpace);
void drawEllipse(float major, float minor, float angle, float focalx, float focaly, float zoom, int pointcount, float r, float g, float b, bool dashed);
void drawHyperbola(float major, float minor, float angle, float focalx, float focaly, float zoom, int pointcount, float r, float g, float b, bool dashed);
void drawCircle(float radius, float cx, float cy, float zoom, int pointcount, float r, float g, float b);
void drawIsoTriangle(float cx, float cy, float height, float base, float zoom, float angle, float r, float g, float b);
float** initializeArray(size_t rows, size_t cols);
void freeArray(float** array, size_t rows);
bool inOrbit(float height, float mass, float speed);//Returns bool if speed is great enough to be in a circular orbit, try to fix for all orbits.
float deltaV(float vex, float massW, float massD);