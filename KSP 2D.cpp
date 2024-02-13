
// KSP 2D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <chrono>
#include <thread>
#define PI 3.14159265358979323846
#include <cmath> 
#include "Rocket.h"

using namespace std;


float dist(float x, float y) {
    return sqrt(pow(x, 2) + pow(y, 2));
}


int main()
{
    int tickIntervalMS = 500;

    Game game;
    Rocket rocket;

    float psat = 0;
    float dd = 0;


    //Game loop functions
    while (true) { 

        //Add together speeds and positions
        rocket.speedUpdate(game.planetPos, game);
        for (size_t i = 0; i < 2; i++)
        {
            game.planetPos[i] -= rocket.speed[i];
        }

        while (dist(game.planetPos[1], game.planetPos[0]) < game.planetRad) //Checks if distances are too small and then fixed them
        {
            float angle = atan2(game.planetPos[1], game.planetPos[0]);
            for (size_t i = 0; i < 2; i++)
            {
                game.planetPos[i] -= cos(angle + (i * PI / 2))*0.1 * rocket.sign(game.planetPos[i]);
                rocket.speed[i] = 0;
            }
        }

        //Displays data
        cout << "Distances: " << game.planetPos[0] << ", " << game.planetPos[1] << "  Speed: " << rocket.speed[0] << "  Total Dist: " << dist(game.planetPos[1], game.planetPos[0]) << "  Deriv. of speed * 1000: " << ((dist(rocket.speed[1], rocket.speed[0]) - psat) - dd) * 1000 << std::endl;
        dd = dist(rocket.speed[1], rocket.speed[0]) - psat;
        psat = dist(rocket.speed[1], rocket.speed[0]); 
        this_thread::sleep_for(std::chrono::milliseconds(tickIntervalMS));


        //Display functionm, resolustion = 480, 240
        vector<float> screenDist;
        for (size_t i = 0; i < 2; i++)
        {
            screenDist[i] = game.planetPos[i] / game.zoom;
        }
        RenderKSP(screenDist[0], screenDist[1], game.zoom, rocket.throttle, rocket. heading);
    }
}






// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
