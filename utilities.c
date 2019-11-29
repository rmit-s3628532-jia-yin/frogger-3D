#include "utilities.h"

void drawAxes(float length)
{
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(length, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, length, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, length);
    glEnd();
}

float degToRad(float degree)
{
    float rad = degree * M_PI / 180.0;
    return rad;
}

float radToDeg(float rad)
{
    float deg = rad * 180.0 / M_PI;
    return deg;
}

//  startPoint vertex(a, b, c) 
//  vector <x, y>
void drawVector(float a, float b, float c, float x, float y, float z, float scale, bool normalized)
{
    float magnitude;
    float endPointX, endPointY, endPointZ;
    magnitude = sqrt(x * x + y * y + z * z);
    
    // scale
    x *= scale;
    y *= scale;
    z *= scale;

    endPointX = a + x;
    endPointY = b + y;
    endPointZ = c + z;
    
    // calculate normalized vector's endpoints
    if (normalized) {
        endPointX = a + x / magnitude;
        endPointY = b + y / magnitude;
        endPointZ = c + z / magnitude;
    }
    
    glColor3f(1, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(a, b, c);
    glVertex3f(endPointX, endPointY, endPointZ);
    glEnd();
}
