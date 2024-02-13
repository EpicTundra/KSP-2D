#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <list>
#define PI 3.14159265358979323846
#define G 1
#define PMass 1000
#define atmoHeight 30
#define square(value) pow(value, 2)
#include <cmath> 
using namespace std;


class Game {
public:
    std::vector<double> planetPos = { 10, -299.8 };
    float zoom = 1;                                     //Zoom of ten, a distance of 100 = 10
    float planetRad = 300;
};

class Rocket {
public:

    string engines[2][4] = {
        {"test", "230", "2", "2"},
        {"test2", "350", "3.25", "0.3"} //Name, exhauhst velocity, mass, thrust (MN)
    };

    int engSelect = 1;

    float heading = 0;
    float throttle = 0;
    float fuel = 100;
    float mass = 10 + (std::stof(engines[engSelect][2]));  //Engine mass in tons + vehicle mass
    float thrust = std::stof(engines[engSelect][3]) * 1000000000;  //Engine Thrust, value to compinate for MN
    float exhV = std::stof(engines[engSelect][1]); //Exhaust Velociy of Engine in m/sec
    float massF = thrust / exhV; //Massflow of Engine in kg/sec
    float dragCeo = 1; //Of total rocket, implemetation of calculator later

    vector<double> speed = { 0.0f, 0.0f }; //x then y

    vector<double> speedUpdate(vector<double> planPos, Game game);
    float dist(float x, float y);
    float aero(float speed, float height);
    float accel();
    float gravity(double r2, float trig, int i);
    void updateHeading(float value);
    void updateThrottle(float value);
    double sign(double x);
};