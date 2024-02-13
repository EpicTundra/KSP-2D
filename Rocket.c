#include "Rocket.h"

float dist(float x, float y) {
    return sqrt(pow(x, 2) + pow(y, 2));
}

float aero(rocket_t* rocket, float speed, float height) {  //Linear atmopshere fall off            Temp Value below
    if (height <= atmoHeight) return pow(speed, 2) * rocket->dragCeo * (1 - height / atmoHeight) * 0.1;
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

vector_t speedUpdate(rocket_t* rocket, vector_t planPos, float planetRad) { //Updates speed according to engines and gravity, then returns those values. 
    double distance_squared = square(dist(planPos[1], planPos[0]));  //Takes in x and y distances to gravitation attractor and sqaures them for gravity
    //if (distance_squared == 0) { cout << "Error:distance_sqaured_speedUpdate=0" << endl; } //Error if distance ever equals 0
    float distance = dist(planPos[1], planPos[0]);

    for (size_t i = 0; i < 2; i++) //Loops through, finding angles to use in gravity function
    {
        float trig;
        if (i == 1) {
            trig = asin(planPos[1] / distance);
        }
        else {
            trig = acos(planPos[0] / distance);
        }

        rocket->speed[i] += cos((heading * (PI / 180)) - i * (PI / 2)) * accel() + gravity(distance_squared, trig, i);
        rocket->speed[i] -= aero(rocket->speed[i], (dist(planPos[0], planPos[1]) - planetRad)) * sign(rocket->speed[i]); //Aerodyamics
    }
    return speed;
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
    return (x > 0) - (x < 0)
}

void rocketInit(rocket_t* rocket) {
    // init your default values here lucas
}
