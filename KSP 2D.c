
// KSP 2D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdlib.h"
#include "stdio.h"
#define PI 3.14159265358979323846
#include "math.h"
#include "Rocket.h"
#include "stdbool.h"

float dist(float x, float y) {
    return sqrt(pow(x, 2) + pow(y, 2));
}

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
        speedUpdate(&rocket, game.planetPos, game.planetRad);
        for (size_t i = 0; i < 2; i++)
        {
            game.planetPos[i] -= rocket.speed[i];
        }

        while (dist(game.planetPos[1], game.planetPos[0]) < game.planetRad) //Checks if distances are too small and then fixed them
        {
            float angle = atan2(game.planetPos[1], game.planetPos[0]);
            for (size_t i = 0; i < 2; i++)
            {
                game.planetPos[i] -= cos(angle + (i * PI / 2))*0.1 * sign(game.planetPos[i]);
                rocket.speed[i] = 0;
            }
        }

        //Displays data
        //cout << "Distances: " << game.planetPos[0] << ", " << game.planetPos[1] << "  Speed: " << rocket.speed[0] << "  Total Dist: " << dist(game.planetPos[1], game.planetPos[0]) << "  Deriv. of speed * 1000: " << ((dist(rocket.speed[1], rocket.speed[0]) - psat) - dd) * 1000 << std::endl;
        printf("Distances: %lf, %lf; Speed: %lf; Total Dist: %f; Deriv. of speed * 1000: %f\n", game.planetPos[0], game.planetPos[1], rocket.speed[0], dist(game.planetPos[1], game.planetPos[0]), ((dist(rocket.speed[1], rocket.speed[0]) - psat) - dd) * 1000);
        dd = dist(rocket.speed[1], rocket.speed[0]) - psat;
        psat = dist(rocket.speed[1], rocket.speed[0]); 
        wait(tickIntervalMS);
        //this_thread::sleep_for(std::chrono::milliseconds(tickIntervalMS));


        //Display functionm, resolustion = 480, 240
        vector<float> screenDist;
        for (size_t i = 0; i < 2; i++)
        {
            screenDist[i] = game.planetPos[i] / game.zoom;
        }
    }
}
