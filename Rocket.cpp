#include "Rocket.h"

float Rocket::dist(float x, float y) {
    return sqrt(pow(x, 2) + pow(y, 2));
}

float Rocket::aero(float speed, float height) {  //Linear atmopshere fall off            Temp Value below
    if (height <= atmoHeight) return pow(speed, 2) * dragCeo * (1 - height / atmoHeight) * 0.1;
    else return 0;
}

float Rocket::accel() { //Accelation of rocket with engines
    double force = (thrust * throttle);
    fuel -= massF * throttle;
    return force / (mass + fuel);
}

float Rocket::gravity(double r2, float trig, int i) {
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

vector<double> Rocket::speedUpdate(vector<double> planPos, Game game) { //Updates speed according to engines and gravity, then returns those values. 
    double distance_squared = square(dist(game.planetPos[1], game.planetPos[0]));  //Takes in x and y distances to gravitation attractor and sqaures them for gravity
    if (distance_squared == 0) { cout << "Error:distance_sqaured_speedUpdate=0" << endl; } //Error if distance ever equals 0
    float distance = dist(game.planetPos[1], game.planetPos[0]);

    for (size_t i = 0; i < 2; i++) //Loops through, finding angles to use in gravity function
    {
        float trig;
        if (i == 1) {
            trig = asin(game.planetPos[1] / distance);
        }
        else {
            trig = acos(game.planetPos[0] / distance);
        }

        speed[i] += cos((heading * (PI / 180)) - i * (PI / 2)) * accel() + gravity(distance_squared, trig, i);
        speed[i] -= aero(speed[i], (dist(game.planetPos[0], game.planetPos[1]) - game.planetRad)) * sign(speed[i]); //Aerodyamics
    }
    return speed;
}

void Rocket::updateHeading(float value) {
    heading += value;
    heading = fmod(heading, 360);
    if (heading < 0) heading += 360;
}

void Rocket::updateThrottle(float value) {
    throttle += value;
    if (throttle >= 0) {
        throttle = fmod(throttle, 100);
    }
    else throttle = 0;
}

double Rocket::sign(double x) { //Gives sign of the value
    if (x > 0) {
        return 1; // Positive sign
    }
    else if (x < 0) {
        return -1; // Negative sign
    }
    else {
        return 0; // Zero
    }
}