#include "Util.h"

float clamp(float d, float min, float max) {
	const float t = (d < min) ? min : d;
	return t > max ? max : t;
}

//Gives sign of the value
double sign(double x) {
	return (x > 0) - (x < 0);
}

float dist(float x, float y) {
	return sqrt(square(x) + square(y));
}

float crossproduct(float a[], float b[]) {
    return a[0] * b[1] - a[1] * b[0];
}

void drawRect(float cx, float cy, float height, float width, float angle, float zoom, float r, float g, float b, bool inGameSpace) {
    float mod = 1;
    if (inGameSpace) mod = METER2GL;
    float zoomMod = zoom;
    if (!inGameSpace) zoomMod = 1;
    glPushMatrix();
    glTranslatef((cx * mod) * zoomMod, (cy * mod) * zoomMod, 0.0f);
    glRotatef(angle, 0, 0, 1.0);
    glScalef(zoomMod, zoomMod, 1);
    float widthVertex = (width * mod) / 2.0f;
    float heightVertex = (height * mod) / 2.0f;
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(-widthVertex, -heightVertex);

    glColor3f(r, g, b);
    glVertex2f(widthVertex, -heightVertex);

    glColor3f(r, g, b);
    glVertex2f(widthVertex, heightVertex);

    glColor3f(r, g, b);
    glVertex2f(-widthVertex, heightVertex);
    glEnd();
    glPopMatrix();
}

//At 0 angle, major is y and minor is x, and second foci is to down.  Also, all values are assumed to be their minors
void drawEllipse(float major, float minor, float angle, float focalx, float focaly, float zoom, int pointcount, float r, float g, float b, bool dashed)//Give one foci to draw ellipse
{
    float c = sqrt(major * major - minor * minor); // Focal distance

    glPushMatrix();

    // Step 1: Move to the focal position
    glTranslatef((focalx + c) * METER2GL * zoom, focaly * METER2GL * zoom, 0.0);

    // Step 2: Rotate the entire coordinate system
    glRotatef(angle / DEGREETORAD, 0.0, 0.0, 1.0);

    // Step 3: Scale for zoom
    glScalef(zoom, zoom, 1.0);

    // Start drawing the ellipse
    glBegin(GL_LINE_LOOP);
    glColor3f(r, g, b);

    for (int i = 0; i < pointcount; i++) {
        float theta = 2.0 * PI * i / pointcount;
        float x = cos(theta) * major;
        float y = sin(theta) * minor;

        // No need to manually rotate; OpenGL handles it
        glVertex2f(x * METER2GL, y * METER2GL);
    }

    glEnd();

    glPopMatrix();
}



/*
    float c = sqrt(square(major) - square(minor)); //focal distance
    float cx = focalx + (sin(angle * DEGREETORAD) * c);
    float cy = focaly + (-cos(angle * DEGREETORAD) * c);
    glPushMatrix();
    glRotatef(angle, 0, 0, 1.0);
    glTranslatef((cx * METER2GL) * zoom, (cy * METER2GL) * zoom, 0.0f);
    glScalef(zoom, zoom, 1);
    if (dashed)
    {
        glBegin(GL_LINE);
        glColor3f(r, g, b);
        for (int i = 0; i < pointcount; i++) {
            float x = cos   (2 * PI / pointcount * i);
            float y = sin(2 * PI / pointcount * i);
            glVertex2f(x * major * METER2GL, y * minor * METER2GL);
        }
    }
    else {
        glBegin(GL_LINE_LOOP);
        glColor3f(r, g, b);
        for (int i = 0; i < pointcount; i++) {
            float x = cos(2 * PI / pointcount * i);
            float y = sin(2 * PI / pointcount * i);
            glVertex2f(x * major * METER2GL, y * minor * METER2GL);
        }
    }
    glEnd();
    glPopMatrix();
*/

void drawCircle(float radius, float cx, float cy, float zoom, int pointcount, float r, float g, float b) { //Draw circle weeee
    glPushMatrix();
    glTranslatef((cx * METER2GL) * zoom, (cy * METER2GL) * zoom, 0.0f);
    glScalef(zoom, zoom, 1);
    glBegin(GL_POLYGON);
    glColor3f(r, g, b);
    for (int i = 0; i < pointcount; i++) {
        float x = cos(2 * PI / pointcount * i);
        float y = sin(2 * PI / pointcount * i);
        glVertex2f(x * radius * METER2GL, y * radius * METER2GL);
    }
    glEnd();
    glPopMatrix();
}

void drawIsoTriangle(float cx, float cy, float height, float base, float zoom, float angle, float r, float g, float b) {
    glPushMatrix();
    glTranslatef((cx * METER2GL) * zoom, (cy * METER2GL) * zoom, 0.0f);
    glRotatef(angle, 0, 0, 1.0);
    glScalef(zoom, zoom, 1);
    glBegin(GL_POLYGON);
    glColor3f(r, g, b);
    glVertex2f(0, height * METER2GL / 2);
    glVertex2f(base * METER2GL / 2, -height * METER2GL / 2);
    glVertex2f(-base * METER2GL / 2, -height * METER2GL / 2);
    glEnd();
    glPopMatrix();
}



float** initializeArray(size_t rows, size_t cols) { //initializes a 2D array of floats   DEPRECATED
    // Allocate memory for the array of pointers to float arrays (rows)
    float** array = (float**)malloc(rows * sizeof(float*));
    if (array == NULL) {
        //handle memory allocation failure
        return NULL;
    }

    // Allocate memory for each row and initialize values to 0.0
    for (size_t i = 0; i < rows; i++) {
        array[i] = (float*)malloc(cols * sizeof(float));
        if (array[i] == NULL) {
            // Handle memory allocation failure
            // Free previously allocated memory to prevent memory leaks
            for (size_t j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
        //initialize to 0,0
        for (size_t j = 0; j < cols; j++) {
            array[i][j] = 0.0f;
        }
    }

    return array;
}

void freeArray(float** array, size_t rows) {
    for (size_t i = 0; i < rows; i++) {
        free(array[i]);
    }
    free(array);
}

bool inOrbit(float height, float mass, float speed) {
    return (sqrt((G * mass) / height) <= speed);
}

float deltaV(float vex, float massW, float massD) {
    return vex * (log(massW / massD) / log(2.7182818284));
}