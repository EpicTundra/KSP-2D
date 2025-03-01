#include "KSP2D.h"


ZBuffer* frameBuffer;
camera_t camera;

sprite_t planet_sprite, rocket_sprite;

game_t game;
rocket_t rocket;
float psat = 0;
float dd = 0;

int currentFPS;

#define ZOOMSPEED (1)
#define ROTATESPEED (1)


void game_init()
{

    frameBuffer = ZB_open(480, 240, ZB_MODE_RGBA, 0);

    targetFPS = 60;

    rendererInit(frameBuffer, &camera);

    planet_sprite = rendererCreateTexture(planet_map, 32, 32, GL_NEAREST);

    rocket_sprite = rendererCreateTexture(rocket_map, 21, 36, GL_NEAREST);
    
    gameInit(&game);
    rocketInit(&rocket);

}

void game_update() {  //Add together speeds and positions
    SDL_PumpEvents(); // Update the state array
    Uint8* keyboard = SDL_GetKeyboardState(NULL);

    if (input(keyboard[SDL_SCANCODE_ESCAPE]) > 0) {
        if (game.mode != 2) {
            game.mode = 2;
        }
        else game.mode = 0;
    }

    if (input(keyboard[SDL_SCANCODE_M]) > 0) {
        if (!game.pressingModeSwitch) {
            game.mode = (game.mode == 1 ? 0 : 1);
        }
        game.pressingModeSwitch = true;
    }
    else game.pressingModeSwitch = false;

    switch (game.mode)
    {
    case 0:
        physics();

        game.zoom += ((input(keyboard[SDL_SCANCODE_UP]) - input(keyboard[SDL_SCANCODE_DOWN])) / ZOOMSPEED) *
            (((-(2 / game.zoom) + 2.1) / 2.2) + square(game.zoom * 0.006666));
        game.zoom = clamp(game.zoom, 1.0, 800);
        updateHeading(&rocket, (input(keyboard[SDL_SCANCODE_A]) - input(keyboard[SDL_SCANCODE_D])) * ROTATESPEED);
        updateThrottle(&rocket, (input(keyboard[SDL_SCANCODE_W]) - input(keyboard[SDL_SCANCODE_S])));
        break;

    case 1: 
        physics();

        game.zoom += ((input(keyboard[SDL_SCANCODE_UP]) - input(keyboard[SDL_SCANCODE_DOWN])) / ZOOMSPEED) *
            (((-(2 / game.zoom) + 2.1) / 2.2) + square(game.zoom * 0.006666));
        game.zoom = clamp(game.zoom, 1.0, 100);
        updateHeading(&rocket, (input(keyboard[SDL_SCANCODE_A]) - input(keyboard[SDL_SCANCODE_D])) * ROTATESPEED);
        updateThrottle(&rocket, (input(keyboard[SDL_SCANCODE_W]) - input(keyboard[SDL_SCANCODE_S])));
        break;

    case 2:
        //pause inputs
        break;

    default:
        printf("%s", "Game Mode Error");
        break;
    }


}

void game_draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (game.mode)
    {
    case 0: //Gameplay mode
        planet_sprite.scale = (vector_t){ 3.125, 3.125 }; //By default sprite renders at half the height of screen (0.5, -0.05)
        rocket_sprite.scale = (vector_t){ rocket.size / 240, rocket.size / 240 };
        rocket_sprite.angle = (rocket.heading + 180);

        /*
        Math for planet scale:
        26 = 300  //texutre is 26 of full space, 30 full
        240 = 2  Height to screen space
        120 = 1
        300 = 2.5

        24/30 = 0.8

        2.5/0.8 = 3.125
        */

        camera.zoom = game.zoom * 0.1f;

        planet_sprite.dstrect.x = game.planetPos[0] / 240;
        planet_sprite.dstrect.y = game.planetPos[1] / 240;

        rendererDrawSprite(planet_sprite, camera);
        rendererDrawSprite(rocket_sprite, camera);

        telemetryRender(true);//true value means currently shows telemetry
        break;


    case 1: //Map mode
        camera.zoom = game.zoom * 0.1f;

        calcOrbit(&game, &rocket);
        renderOrbit(&game, camera.zoom, &rocket);

        planet_sprite.scale = (vector_t){ 3.125, 3.125 }; //By default sprite renders at half the height of screen (0.5, -0.05)
        planet_sprite.dstrect.x = game.planetPos[0] / 240;
        planet_sprite.dstrect.y = game.planetPos[1] / 240;

        rendererDrawSprite(planet_sprite, camera);

        //for new orbit code have a variable with the rocket that is set to true when engines fire and will then recalculate orbit, but doesn't otherwise do anything

        
        //drawEllipse(game.orbitRenderPos[0], game.orbitRenderPos[1], game.orbitRenderPos[3], game.planetPos[0], game.planetPos[1], camera.zoom, 100, 1, 1, 1, false);

        //float renderFidely = 4; 
        //rocket.calcOrbit > targetFPS * 0.25
        /*/if (rocket.calcOrbit > renderFidely) { //Every fourth of a second, updates orbital prediction lines. Else, renders from prestored data;
            disOrb(game, rocket, camera.zoom, &game);
            rocket.calcOrbit = 0;
        }
        else {
            renderOrbit(game.renderPos, camera.zoom, game.planetPos[0], game.planetPos[1], &rocket, &game);
        }*/

        rocket.calcOrbit++;

        telemetryRender(true);

        drawIsoTriangle(0, 0, 7, 3.2, camera.zoom, (rocket.heading + 180), 1, 1, 1);



        break;

    case 2: 
        //stuff
        break;
       
    default:
        printf("%s", "Game Mode Error");
        break;
    }
        
    
}

int input(int input) {
    return ((input - 0.5) * 2);
}

void deltaT() {
    tNow = SDL_GetTicks64();
    dt = tNow - tLast;
    tLast = tNow;
    return dt;
}

void physics() { //Abstract so can be called in map and normal game screen
    float x = game.planetPos[0];
    float y = game.planetPos[1];

    deltaT();//Updates DeltaT, variable dt delcared in KSP2D
    speedUpdate(&rocket, x, y, game.planetRad, game.planetMass);

    for (size_t i = 0; i < 2; i++) //Move rocket according to deltaTime
    {
        game.planetPos[i] -= (rocket.speed[i] / (1000.0f / (dt + 1)));
    }

    while (dist(game.planetPos[0], game.planetPos[1]) < game.planetRad + rocket.size) //Checks if distances are too small and then fixed them
    {
        double coFishCent = 0.1 / max(game.planetPos[0], game.planetPos[1]);
        if ((game.planetPos[0] < 0) && (game.planetPos[1] < 0)) coFishCent *= -1;
        for (size_t i = 0; i < 2; i++)
        {
            game.planetPos[i] += game.planetPos[i] * coFishCent;
            rocket.speed[i] = 0;
        }
    }

}

void telemetryRender(bool mapView) {
    const char distText[50]; //Create text for renderinh
    const char throttleText[50];
    const char fuelText[50];

    //Text and other things that should always be rendered
    sprintf(throttleText, "Throttle: %g%%", round(rocket.throttle));
    glDrawText((unsigned char*)throttleText, 10, 210, 0xffffff, 0);
    drawRect(-1.91f + (rocket.throttle / 220), -0.9f, 0.05, rocket.throttle / 110, 0, camera.zoom, 1, 1, 1, false);


    if (mapView) { //Only when in map view
        sprintf(fuelText, "Fuel: %f", game.orbitRenderPos[0] + (game.orbitRenderPos[0] * game.orbitRenderPos[2]));
        sprintf(distText, "Dist: %f", dist(game.planetPos[1], game.planetPos[0]));//dist(game.planetPos[1], game.planetPos[0])

        glDrawText((unsigned char*)distText, 10, 10, 0xffffff, 0);
        glDrawText((unsigned char*)fuelText, 10, 30, 0xffffff, 0);
    }
}

