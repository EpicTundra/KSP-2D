#pragma once

#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "Rocket.h"
#include "stdbool.h"
#include "zbuffer.h"
#include "GL/gl.h"
#include "renderer.h"
#include "textures.h"
#include "Util.h"

extern ZBuffer* frameBuffer;

int targetFPS;

extern int currentFPS;

void game_init();
void game_update();
void game_draw();
void deltaT(); //DeltaTime function, call once per loop.
void physics(); //Runs the physics pipline
void telemetryRender(bool mapView);//Renders text and gauges that contain flight info