#include "Rocket.h"
#include "KSP2D.h"


float aero(rocket_t* rocket, float speed, float height) {  //Linear atmopshere fall off            Temp Value below
    if (height <= atmoHeight) return pow(speed, 2) * rocket->dragCeo * square(1 - height / atmoHeight) * 0.1 / (1000.0f / (dt + 1));
    else return 0;
}

float accel(rocket_t* rocket) { //Accelation of rocket with engines
    double force = (rocket->rocketEngine.thrust * rocket->throttle);
    if (rocket->throttle > 0)
    {
        if (rocket->fuel > rocket->rocketEngine.massF / (1000.0f / (dt + 1)) * (rocket->throttle / 100))
        {
            rocket->fuel -= ((rocket->rocketEngine.massF * (rocket->throttle / 100)) / (1000.0f / (dt + 1)));
            return (force / (rocket->mass + rocket->fuel)) / (1000 / (dt + 1)); //DeltaT expresion
        }
    }
}

float gravity(double r2, float trig, int i, float planetMass) {
    float grav = (G * planetMass / r2);   //Total gravity
    float temp;

    if (i == 1) {                 //Use angle to find side ratios to total gravity vector
        temp = grav * sin(trig);
    }
    else {
        temp = grav * cos(trig);
    }
    return temp / (1000.0f / (dt + 1));

}

void speedUpdate(rocket_t* rocket, float x, float y, float planetRad, float planetMass) { //Updates speed according to engines and gravity, then returns those values. 
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

        float grav = gravity(distance_squared, trig, i, planetMass);
        rocket->speed[i] += sin((rocket->heading * (PI / 180)) - i * (PI / 2)) * accel(rocket) + grav;
        rocket->speed[i] -= aero(rocket, rocket->speed[i], (dist(x, y) - planetRad)) * sign(rocket->speed[i]); //Aerodyamics, "-" because it removes speed;
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
        rocket->throttle = clamp(rocket->throttle, 0, 100);
    }
    else rocket->throttle = 0;
}


void rocketInit(rocket_t* rocket) {

    tNow = SDL_GetTicks(); //Init DT
    dt = 0;
    tLast = tNow;

    rocket->engSelect = 0;

    rocket->size = 6.0f;

    rocket->speed[0] = 0;
    rocket->speed[1] = 0;

    rocket->calcOrbit = 0;

    Engine test;
    PartInit(&test, "test", 30, 2, 0.0000008); //name, fuel flow rate (kg/sec), mass, thrust

    rocket->heading = 180;
    rocket->throttle = 0;
    rocket->fuel = 10000;
    rocket->rocketEngine = test;
    rocket->mass = 2000 + test.mass;  //Engine mass in tons + vehicle mass 
    rocket->dragCeo = 1; //Of total rocket, implemetation of calculator later

}

void gameInit(game_t* game) {
    game->planetPos[0] = -10;
    game->planetPos[1] = 340;
    game->zoom = 4;
    game->planetRad = 300;
    game->planetMass = 200000;
    for (size_t y = 0; y < 4; y++) //Just so doesn't spew errors
    {
        game->orbitRenderPos[y] = 0;
    }
    game->mode = 0;
    game->pressingModeSwitch = false;
}

void calcOrbit(game_t* game, rocket_t* rocket) { //Finds orbitals paramaters
    float mu = G * game->planetMass;
    float distance = dist(game->planetPos[0], game->planetPos[1]);
    float velocity = sqrt(square(rocket->speed[0]) + square(rocket->speed[1]));

    //Is actually semi major...
    float major = 1 / (2 / distance - (square(velocity) / mu));//Derived from the vis viva equation


    //Find eccentricity of orbit from givens
    float h = -game->planetPos[0] * rocket->speed[1] + (game->planetPos[1] * rocket->speed[0]); //Positive means counter clockwise roation
    float e = sqrt(1 - (square(h) / (mu * major)));
    float minor = major * (1 - square(e));

    //Find argument of periapsis
    float trueAnom = acosf( (square(2 * major * e) + square(distance) - square(2 * major - distance)) / (4 * major * e * distance) );

    float futurePos = square(rocket->speed[0] - game->planetPos[0]) + square(rocket->speed[1] - game->planetPos[1]);
    
    //if true anomly up or down

    //float periArg = atan2(y, );
    float periArg = 0;

    game->orbitRenderPos[0] = major;
    game->orbitRenderPos[1] = minor;
    game->orbitRenderPos[2] = e;
    game->orbitRenderPos[3] = trueAnom;//periArg * (1 / DEGREETORAD) + (M_PI / 2)
}

void disOrb(game_t game, rocket_t rocket, float zoom, game_t* gamestr) { //DEPRECATED. displays future orbital trajectory
    float(*renderPositions)[301][3] = gamestr->orbitRenderPos;

    for (size_t i = 0; i < 301; i++)
    {
        for (size_t y = 0; y < 3; y++)
        {
            (*renderPositions)[i][y] = 0;
        }
    }

    float apoapsis[2];
    apoapsis[0] = 2;
    apoapsis[1] = 1;

    float minOrb = (M_PI * 2 * game.planetRad / (2 * (sqrt((G * game.planetMass) / (game.planetRad * 1.4)))));

    float originalPos[2];
    originalPos[0] = game.planetPos[0];
    originalPos[1] = game.planetPos[1];

    rocket.throttle = 0;

    int fidelity;
    int rectDrawSeperation; //In seconds

    fidelity = 2; //higher is worse
    rectDrawSeperation = 6; //higher = farther apart

    int s = 0;

    bool incompleteOrbit = true;

    if (dist(game.planetPos[0], game.planetPos[1]) > game.planetRad + rocket.size + 1) {

        while (dist(game.planetPos[0], game.planetPos[1]) > game.planetRad + rocket.size + 1 && s < 300 && incompleteOrbit) //Runs foward, mini Physics loop
        {
            for (size_t i = 0; i < rectDrawSeperation; i++) { //runs simulation a couple times foward before rendering rectangle
                float x = game.planetPos[0];
                float y = game.planetPos[1];

                dt = 1500 * fidelity / (sqrtf(square(rocket.speed[0]) + (square(rocket.speed[1]))));
                if (dt > 1000) dt = 1000;


                speedUpdate(&rocket, x, y, game.planetRad, game.planetMass);

                for (size_t i = 0; i < 2; i++) //Update pos
                {
                    game.planetPos[i] -= (rocket.speed[i] / (1000.0f / (dt + 1)));
                }

                float distance = dist(game.planetPos[0], game.planetPos[1]);

                if (distance > dist(apoapsis[0], apoapsis[1])) { // Is the apoapsis so far?
                    for (size_t i = 0; i < 2; i++)
                    {
                        apoapsis[i] = game.planetPos[i];
                    }
                }

                if (distance < game.planetRad + rocket.size + 1) //Have we reached the ground?
                {
                    break; //Position still gets added though
                }

                if ((game.planetPos[0] > originalPos[0] - 15 && game.planetPos[0] < originalPos[0] + 15) //Are we high enough to be in orbit and are we actually in orbit
                    && (game.planetPos[1] > originalPos[1] - 15 * rocket.speed[1] && game.planetPos[1] < originalPos[1] + 15)
                    && s > minOrb) {  //Fix this, inconsitent and will cut off at high points in an elliptical orbit
                    incompleteOrbit = false;
                    break;
                }
            }


            (*renderPositions)[s][0] = game.planetPos[0];
            (*renderPositions)[s][1] = game.planetPos[1];

            //Angle stuff, find perpendicular vector to angle
            if (s > 1) {
                float foward[2];
                float fFoward[2];
                for (size_t i = 0; i < 2; i++)
                {
                    float current = (*renderPositions)[s - 1][i];
                    float past = (*renderPositions)[s - 2][i];
                    float future = (*renderPositions)[s][i];
                    foward[i] = current - past;
                    fFoward[i] = future - current;
                }
                float angle = atan2f(foward[0], -foward[1]);
                float angle2 = atan2f(fFoward[0], -fFoward[1]);
                float angleAverage = (angle + angle2) / 2;
                (*renderPositions)[s - 1][2] = angleAverage * 180 / PI;
            }
            else {
                float foward[2];
                for (size_t i = 0; i < 2; i++)
                {
                    float current = (*renderPositions)[s][i];
                    float past = originalPos[i];
                    foward[i] = current - past;
                }
                float angle = atan2f(foward[0], -foward[1]);
                (*renderPositions)[s][2] = angle * 180 / PI - 6 * sign(angle * 180 / PI);
            }
            s++;
        }

        float current; //Fixes first point, same math as above
        float past;
        float future;
        float foward[2];
        float fFoward[2];
        for (size_t i = 0; i < 2; i++)
        {
            current = (*renderPositions)[0][i];
            past = originalPos[i];
            future = (*renderPositions)[1][i];
            foward[i] = current - past;
            fFoward[i] = future - current;
        }
        float angle = atan2f(foward[0], -foward[1]);
        float angle2 = atan2f(fFoward[0], -fFoward[1]);
        float angleAverage = (angle + angle2) / 2;
        (*renderPositions)[0][2] = angleAverage * 180 / PI;


        if (dist(apoapsis[0], apoapsis[1]) > dist(originalPos[0], originalPos[1])) {
            (*renderPositions)[300][0] = apoapsis[0];
            (*renderPositions)[300][1] = apoapsis[1];
        }

        renderOrbit(renderPositions, zoom, originalPos[0], originalPos[1], &rocket, &game); //Renders Orbit now
    }
}


void renderOrbit(float (*renderPositions)[301][3], float zoom, float originalPosx, float originalPosy, rocket_t *rocket, game_t *game) { //Renders orbit
    bool render = true;
    int count = 0;
    while (render) { //Renders line
        if (((*renderPositions)[count + 1][0] == 0 && (*renderPositions)[count + 1][1] == 0) || count > 299) break;
        drawRect(-((*renderPositions)[count][0] - originalPosx), -((*renderPositions)[count][1] - originalPosy), 4, 1.5, (*renderPositions)[count][2], zoom, 1, 1, 1, true);
        count++;
    }
    if ((dist((*renderPositions)[count][0], (*renderPositions)[count][1]) < ((*game).planetRad + (*rocket).size + 1))) {//Renders ground colision point, but checks that its touching the ground first
        drawCircle(1.5, originalPosx - (*renderPositions)[count][0], originalPosy - (*renderPositions)[count][1], zoom, 20, 1, 1, 1);
    }
    if ((*renderPositions)[300][0] != 0 && (*renderPositions)[300][1] != 0) { //Renders apoapsis
        drawCircle(2, originalPosx - (*renderPositions)[300][0], originalPosy - (*renderPositions)[300][1], zoom, 20, 0.3, 0.5, 0.86);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
}
