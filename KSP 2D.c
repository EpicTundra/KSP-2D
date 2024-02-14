
// KSP 2D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdlib.h"
#include "stdio.h"
#define PI 3.14159265358979323846
#include "math.h"
#include "Rocket.h"
#include "stdbool.h"


int main()
{
    int tickIntervalMS = 500;

    game_t game;
    gameInit(&game);
    rocket_t rocket;
    rocketInit(&rocket);

    float psat = 0;
    float dd = 0;


    //Game loop functions
    while (true) {
        //Add together speeds and positions

        float x = game.planetPos[0];
        float y = game.planetPos[1];
        speedUpdate(&rocket, x, y, game.planetRad);
      

        for (size_t i = 0; i < 2; i++)
        {
            game.planetPos[i] -= rocket.speed[i];
        }

        while (dist(game.planetPos[0], game.planetPos[1]) < game.planetRad) //Checks if distances are too small and then fixed them
        {

            float angle = atan2(game.planetPos[1], game.planetPos[0]);
            for (size_t i = 0; i < 2; i++)
            {
                game.planetPos[i] -= cos(angle + (i * PI / 2))*0.1 * sign(game.planetPos[i]);
                rocket.speed[i] = 0;
            }
        }

        //Displays data
        printf("Distances: %lf, %lf; Speed: %lf; Total Dist: %f; Deriv. of speed * 1000: %f\n", game.planetPos[0], game.planetPos[1], rocket.speed[0], dist(game.planetPos[1], game.planetPos[0]), ((dist(rocket.speed[1], rocket.speed[0]) - psat) - dd) * 1000);
        dd = dist(rocket.speed[1], rocket.speed[0]) - psat;
        psat = dist(rocket.speed[1], rocket.speed[0]); 
        Sleep(200);


        //Display functionm, resolustion = 480, 240
        
    }
}
