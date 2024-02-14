#include "Rocket.h"

float dist(float x, float y) {
    return sqrt(square(x) + square(y));
}

float aero(rocket_t* rocket, float speed, float height) {  //Linear atmopshere fall off            Temp Value below
    if (height <= atmoHeight) return pow(speed, 2) * rocket->dragCeo * square(1 - height / atmoHeight) * 0.1;
    else return 0;
}

float accel(rocket_t* rocket) { //Accelation of rocket with engines
    double force = (rocket->thrust * rocket->throttle);
    rocket->fuel -= rocket->massF * rocket->throttle;
    return force / (rocket->mass + rocket->fuel);
}

float gravity(double r2, float trig, int i) {
    float grav = (G * PMass / r2);   //Total gravity
    float temp;

    if (i == 1) {                 //Use angle to find side ratios to total gravity vector
        temp = grav * sin(trig);
    }
    else {
        temp = grav * cos(trig);
    }
    return temp;

}

void speedUpdate(rocket_t* rocket, float x, float y, float planetRad) { //Updates speed according to engines and gravity, then returns those values. 
    double distance_squared = square(dist(y, x));  //Takes in x and y distances to gravitation attractor and sqaures them for gravity
    float distance = dist(y, x);

    for (int i = 0; i < 2; i++) //Loops through, finding angles to use in gravity function
    {
        float trig;
        if (i == 1) {
            trig = asin(y / distance);
        }
        else {
            trig = acos(x / distance);
        }

        rocket->speed[i] += cos((rocket->heading * (PI / 180)) - i * (PI / 2)) * accel(rocket) + gravity(distance_squared, trig, i);
        rocket->speed[i] -= aero(rocket, rocket->speed[i], (dist(x, y) - planetRad)) * sign(rocket->speed[i]); //Aerodyamics
        //printf("%lf", rocket->speed[i]);
    }
}

void updateHeading(rocket_t* rocket, float value) {
    rocket->heading += value;
    rocket->heading = fmod(rocket->heading, 360);
    if (rocket->heading < 0) rocket->heading += 360;
}

void updateThrottle(rocket_t* rocket, float value) {
    rocket->throttle += value;
    if (rocket->throttle >= 0) {
        rocket->throttle = fmod(rocket->throttle, 100);
    }
    else rocket->throttle = 0;
}

//Gives sign of the value
double sign(double x) {
    return (x > 0) - (x < 0);
}

void rocketInit(rocket_t* rocket) {
    rocket->engines[0][0] = "test";
    rocket->engines[0][1] = "230";
    rocket->engines[0][2] = "2";
    rocket->engines[0][3] = "2";
    rocket->engines[1][0] = "test2";
    rocket->engines[1][1] = "350";
    rocket->engines[1][2] = "3.25";
    rocket->engines[1][3] = "0.3";

    rocket->engSelect = 1;

    rocket->speed[0] = 0;
    rocket->speed[1] = 0;

    rocket->heading = 0;
    rocket->throttle = 0;
    rocket->fuel = 100;
    rocket->mass = 10 + atof(rocket->engines[rocket->engSelect][2]);  //Engine mass in tons + vehicle mass 
    rocket->thrust = atof(rocket->engines[rocket->engSelect][3]) * 1000000000;  //Engine Thrust, value to compinate for MN
    rocket->exhV = atof(rocket->engines[rocket->engSelect][1]); //Exhaust Velociy of Engine in m/sec
    rocket->massF = rocket->thrust / rocket->exhV; //Massflow of Engine in kg/sec
    rocket->dragCeo = 1; //Of total rocket, implemetation of calculator later
}

void gameInit(game_t* game) {
    game->planetPos[0] = -10;
    game->planetPos[1] = 340;
    game->zoom = 1;
    game->planetRad = 300;
}

bool inOrbit(float height, float mass, float speed) {
    return (sqrt((G * mass) / height) <= speed);
}
