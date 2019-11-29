#include "frog.h"

/* frogger 3D */

const float g = -0.25;
vec3f whiteC = {1.0, 1.0, 1.0}, purple = {1.0, 0.0, 1.0};

void displayProjectile(state *projectile, sphere *frog, bool filled, bool gameEnds, bool showAxes, bool visualizeNormal, int size, vec3f vertices_frog_body[size], vec3f vertices_frog_arm[size], vec3f vertices_frog_elbow[size])
{
    float rotationY, angle;
    float xVec, yVec, zVec;
    float magnitude;

    xVec = projectile->v.x;
    yVec = projectile->v.y;
    zVec = projectile->v.z;

    magnitude = sqrt(xVec * xVec + yVec * yVec + zVec * zVec);
    
    rotationY = -asin(xVec / magnitude);
    rotationY = radToDeg(rotationY);    // convert to degrees

    angle = asin(yVec / magnitude);
    angle = radToDeg(angle);    // convert to degrees
    
    glMatrixMode(GL_MODELVIEW);
    
    glPushMatrix();
    
    glTranslatef(projectile->r.x, projectile->r.y, projectile->r.z);
    glRotatef(rotationY, 0, 1, 0);
    glRotatef(angle, 1, 0, 0);

    /* draw frog */
    glColor3f(0, 1, 0);
    drawFrog(frog, showAxes, filled, visualizeNormal, size, vertices_frog_body, vertices_frog_arm, vertices_frog_elbow);
    
    glPopMatrix();
}

void drawJumpVector(state *projectile) 
{
    bool notNormalized = false;
    drawVector(projectile->r.x, projectile->r.y, projectile->r.z, projectile->v.x, projectile->v.y, projectile->v.z, 0.8, notNormalized);
}

/* Hierarchically model frog arms */
void drawArm(float rotation, float translationX, float translationZ, bool filled, bool visualizeNormal, int size, vec3f vertices_frog_arm[size], vec3f vertices_frog_elbow[size]) 
{
    /* shoulder */
    glPushMatrix();
    glTranslatef(translationX, -0.5, translationZ);
    glRotatef(-45, 1, 0, 0);
    glRotatef(rotation, 0, 1, 0);

    /* upper arm */
    glPushMatrix();
    glTranslatef(0, -1, 0);

    if (visualizeNormal)
        drawNormCube(size, vertices_frog_arm);
    drawCube(1.5, 1.5, 2, filled);

    /* elbow */
    glPushMatrix();
    glTranslatef(0, -1, 0);
    glRotatef(100, 1, 0, 0);

    glPushMatrix();
    glTranslatef(0, -1, 0);

    if (visualizeNormal)
        drawNormCube(size, vertices_frog_elbow);
    drawCube(1.2, 1.2, 2, filled);

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

void drawFrog(sphere *frog, bool showAxes, bool filled, bool visualizeNormal, int size, vec3f vertices_frog_body[size], vec3f vertices_frog_arm[size], vec3f vertices_frog_elbow[size])
{
    const int slices = frog->slices, stacks = frog->stacks;
    const float r = frog->radius;

    float length, width, height, scale;
    length = 5;
    width = 3;
    height = 3;
    scale = 0.02;

    const float green[] = {0, 1, 0.1, 0.2};
    const float white[] = {1.0, 1.0, 1.0};

    setMaterial(green, white, 10.0);

    glColor3f(0, 1, 0);

    if (filled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glPushMatrix();

    glScalef(scale, scale, scale);

    if (visualizeNormal)
        drawNormCube(size, vertices_frog_body);

    drawCube(length, width, height, showAxes);

    drawArm(45.0, -1.75, 2.5, showAxes, visualizeNormal, size, vertices_frog_arm, vertices_frog_elbow);
    drawArm(45.0, -1.75, -2, showAxes, visualizeNormal, size, vertices_frog_arm, vertices_frog_elbow);
    drawArm(-45.0, 1.75, 2.5, showAxes, visualizeNormal, size, vertices_frog_arm, vertices_frog_elbow);
    drawArm(-45.0, 1.75, -2, showAxes, visualizeNormal, size, vertices_frog_arm, vertices_frog_elbow);
    
    glPopMatrix();

    // for (int j = 0; j <= stacks; j++) {
    //     phi = j / (float)stacks * M_PI;
    //     glBegin(GL_QUAD_STRIP);
    //     for (int i = 0; i <= slices; i++) {
    //         theta = i / (float)slices * 2.0 * M_PI;
    //         x1 = r * sinf(phi) * cosf(theta);
    //         y1 = r * sinf(phi) * sinf(theta);
    //         z1 = r * cosf(phi);
    //         x2 = r * sinf(phi + step_phi) * cosf(theta);
    //         y2 = r * sinf(phi + step_phi) * sinf(theta);
    //         z2 = r * cosf(phi + step_phi);
    //         glVertex3f(x1, y1, z1);
    //         glVertex3f(x2, y2, z2);
    //     }
    //     glEnd();
    // }
}

void drawCube(float length, float width, float height, bool showAxes) 
{
    if (showAxes)
        drawAxes(3);

    glColor3f(0, 1, 0);
    // front
    glBegin(GL_POLYGON);

    glNormal3f(0, 0, 1);
    glVertex3f( -width / 2, height / 2 , length / 2 );       
    glVertex3f( -width / 2, -height / 2 , length / 2 );       
    glVertex3f( width / 2, -height / 2 , length / 2 );       
    glVertex3f( width / 2, height / 2 , length / 2);       
     
    glEnd();

    // back
    glBegin(GL_POLYGON);
    
    glNormal3f(0, 0, -1);
    glVertex3f( -width / 2, height / 2 , -length / 2 );       
    glVertex3f( -width / 2, -height / 2 , -length / 2 );       
    glVertex3f( width / 2, -height / 2 , -length / 2 );       
    glVertex3f( width / 2, height / 2 , -length / 2);       
     
    glEnd();

    // left
    glBegin(GL_POLYGON);
 
    glNormal3f(-1, 0, 0);
    glVertex3f( -width / 2, height / 2 , -length / 2 );       
    glVertex3f( -width / 2, -height / 2 , -length / 2 );       
    glVertex3f( -width / 2, -height / 2 , length / 2 );       
    glVertex3f( -width / 2, height / 2 , length / 2);       
     
    glEnd();

    // right
    glBegin(GL_POLYGON);
    
    glNormal3f(1, 0, 0);
    glVertex3f( width / 2, height / 2 , -length / 2 );       
    glVertex3f( width / 2, -height / 2 , -length / 2 );       
    glVertex3f( width / 2, -height / 2 , length / 2 );       
    glVertex3f( width / 2, height / 2 , length / 2);       
     
    glEnd();

    // top
    glBegin(GL_POLYGON);
 
    glNormal3f(0, 1, 0);
    glVertex3f( -width / 2, height / 2 , -length / 2 );       
    glVertex3f( -width / 2, height / 2 , length / 2 );       
    glVertex3f( width / 2, height / 2 , length / 2 );       
    glVertex3f( width / 2, height / 2 , -length / 2);       
     
    glEnd();

    // bottom
    glBegin(GL_POLYGON);
 
    glVertex3f( -width / 2, -height / 2 , -length / 2 );       
    glVertex3f( -width / 2, -height / 2 , length / 2 );       
    glVertex3f( width / 2, -height / 2 , length / 2 );       
    glVertex3f( width / 2, -height / 2 , -length / 2);       
     
    glEnd();
}

void updateProjectileInitialState(state *projectile, vec3fPolar *initVel)
{
    projectile->v0.x = sin(degToRad(initVel->rotationY)) * initVel->magnitude;
    projectile->v0.y = sin(degToRad(initVel->angle)) * initVel->magnitude;
    projectile->v0.z = cos(degToRad(initVel->angle)) * initVel->magnitude * cos(degToRad(initVel->rotationY));
    
    projectile->v.x = projectile->v0.x;
    projectile->v.y = projectile->v0.y;
    projectile->v.z = projectile->v0.z;
    
    projectile->r.x = projectile->r0.x;
    projectile->r.y = projectile->r0.y;
    projectile->r.z = projectile->r0.z;
}

void updateProjectileStateNumerical(state *projectile, float dt)
{
    // Position
    projectile->r.x += projectile->v.x * dt;
    projectile->r.y += projectile->v.y * dt;
    projectile->r.z += projectile->v.z * dt;   
    
    // Velocity
    projectile->v.y += g * dt;
}

void updateFrogInitialPosition(state *projectile, sphere *frog, road_o *road, bool collidedWithLogs, log_o logHit, river_o *river, float tSinWave, float log_radius)
{
    float positionOnLog;

    projectile->r0.x = projectile->r.x;
    projectile->r0.y = frog->radius;    // elevate the frog above the ground
    projectile->r0.z = projectile->r.z;

    if (collidedWithRoad(projectile->r.x, projectile->r.y, projectile->r.z, frog, road))
        projectile->r0.y = frog->radius + road->height; 

    if (collidedWithLogs) {
        projectile->r0.x = logHit.xPos;
        // elevate the frog by the sum of frog radius and log radius
        projectile->r0.y = 0.1 * sin (2 * M_PI * logHit.xPos + M_PI / 4 * tSinWave) - river->depth / 2.0 + frog->radius + log_radius;
    }
}

bool collidedWithGround(float frogY, float frogZ, sphere *frog, river_o *river)
{
    if (frogZ <= river->start - river->width || frogZ >= river->start) {
        if (frogY - frog->radius <= 0) {
            // printf("collided with ground\n");
            return true;
        }
    }
    return false;
}

bool collidedWithRoad(float frogX, float frogY, float frogZ, sphere *frog, road_o *road)
{
    float frogRadius;
    float roadStart, roadEnd, roadHeight;

    frogRadius = frog->radius;
    roadStart = road->start;
    roadEnd = roadStart - road->width;
    roadHeight = road->height;

    // printf("road start at: %f\n", roadStart);
    if (frogZ - frogRadius <= roadStart && frogY - frogRadius <= roadHeight && frogZ + frogRadius >= roadEnd) {
        return true;
    }
    return false;
}

bool collidedWithCars(float frogX, float frogY, float frogZ, road_o *road, sphere *frog, car_o cars[], int sizeOfCars)
{   
    float frogLeftBound, frogRightBound, frogFrontBound, frogBackBound, frogBotBound;
    float carLeftBound, carRightBound, carUpBound, carFrontBound, carBackBound, carCenterX, carLength;
    float laneOffset, lane, row_counter;

    // printf("frog x = %f, y = %f, z = %f\n", frogX, frogY, frogZ);

    frogLeftBound = frogX - frog->radius;
    frogRightBound = frogX + frog->radius;
    frogFrontBound = frogZ - frog->radius;
    frogBackBound = frogZ + frog->radius;
    frogBotBound = frogY - frog->radius;
    
    row_counter = 0;
    laneOffset = 0.1;
    lane = (road->start - road->width) + laneOffset;    // translation along z axis

    // collision detection for each car
    for (int i = 0; i < sizeOfCars; i++) {
        carLength = cars[i].length;

        carLeftBound = cars[i].start - carLength / 2;
        carRightBound = cars[i].start + carLength / 2;
        carUpBound = road->height + cars[i].height;
        carFrontBound = lane - cars[i].width / 2;
        carBackBound = lane + cars[i].width / 2;

        carCenterX = cars[i].start;

        if (frogFrontBound <= carBackBound && frogBackBound >= carFrontBound) {   // frog goes between the z coordinates of the car
            // frog is on the left side of the car
            if (frogX <= carCenterX && frogRightBound >= carLeftBound && frogBotBound <= carUpBound) {
                return true;
            }
            // frog is on the right side of the car
            if (frogX > carCenterX && frogLeftBound <= carRightBound && frogBotBound <= carUpBound) {
                return true;
            }
        }

        row_counter++;
        lane += road->width / 5.0;
    }
    
    return false;
}

// sphere-sphere collision detection using bounding circles
int collidedWithLogs(float frogX, float frogY, float frogZ, sphere *frog, log_o logs[], river_o *river, float t, int sizeOfLogs, float log_length, float logRadius)
{
    float logLeftBound, logRightBound, logCenterY, logCenterZ, logXTranslation;
    float frogLeftBound, frogRightBound;
    float radiusOfBoundingCircleOfLog, distanceCenter;
    int lanes = 4;

    frogLeftBound = frogX - frog->radius;
    frogRightBound = frogX + frog->radius;

    for (int i = 0; i < sizeOfLogs; i++) {
        float lane = (float)i / (lanes + 2) + 2 * logRadius;   // the z coordinate of the lane

        logLeftBound = logs[i].xPos - log_length / 2.0;
        logRightBound = logLeftBound + log_length;

        logXTranslation = logs[i].xPos - log_length / 2.0;
        logCenterY = 0.1 * sin (2 * M_PI * logXTranslation + M_PI / 4 * t) - river->depth / 2.0;
        logCenterZ = -(river->width - river->start) + lane;

        distanceCenter = sqrt((logCenterY - frogY) * (logCenterY - frogY) + (logCenterZ - frogZ) * (logCenterZ - frogZ));
        
        if (frogLeftBound <= logRightBound && frogRightBound >= logLeftBound) {
            if (distanceCenter < logRadius + frog->radius) {
                //    printf("distance between centers: %f, the sum of radius: %f\n", distanceCenter, radiusOfBoundingCircleOfCar + frog->rx);
                    // printf("hit log: %d\n", i);
                return i;
            }
        }
    }
    
    return -1;
}

bool collidedWithWave(float frogX, float frogY, float frogZ, river_o *river, float t)
{
    float waveXPos, waveYPos;
    
    waveYPos = 0.1 * sin (2 * M_PI * frogX + M_PI / 4 * t) - river->depth / 2.0;   // wave height
    
    if (frogZ >= river->start - river->width || frogZ <= river->start) {
        if (frogY <= waveYPos) {
            // printf("collided with wave\n");
            return true;
        }
    }
    
    return false;
}

void displayTrajectoryNumerical(state *projectile, sphere *frog, car_o *cars, road_o *road, river_o *river, log_o logs[], float dt, float tSinWave, int sizeOfCars, int sizeOfLogs, float log_length, float log_radius)
{
    float currentX, currentY, currentZ, currentVelocityY;
    
    currentX = projectile->r.x;
    currentY = projectile->r.y + dt;
    currentZ = projectile->r.z;
    currentVelocityY = projectile->v.y;
    
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 0);
    while (currentY >= -river->depth &&
           !trajectoryCollisionDetection(currentX, currentY, currentZ, frog, cars, road, river, logs, dt, tSinWave, sizeOfCars, sizeOfLogs, log_length, log_radius)) {
        glVertex3f(currentX, currentY, currentZ);
        currentX += projectile->v.x * dt;
        currentY += currentVelocityY * dt;
        currentZ += projectile->v.z * dt;
        currentVelocityY += g * dt;
    }
    glEnd();
}

bool trajectoryCollisionDetection(float currentX, float currentY, float currentZ, sphere *frog, car_o cars[], road_o *road, river_o *river, log_o logs[], float dt, float tSinWave, int sizeOfCars, int sizeOfLogs, float log_length, float log_radius)
{    
    // frog radius was added to the argument so that the parabola reaches all the way to the object
    // collision with ground
    if (collidedWithGround(currentY + frog->radius, currentZ, frog, river)) {
        return true;
    }

    // collision with road
    if (collidedWithRoad(currentX, currentY + frog->radius, currentZ, frog, road)) {
        return true;
    }
    
    // collision with cars
    if (collidedWithCars(currentX  + frog->radius, currentY + frog->radius, currentZ  + frog->radius, road, frog, cars, sizeOfCars))
        return true;
    
    // collision with logs
    if (collidedWithLogs(currentX + frog->radius, currentY + frog->radius, currentZ + frog->radius, frog, logs, river, tSinWave, sizeOfLogs, log_length, log_radius) != -1)
        return true;
    
    // collision with wave
    if (collidedWithWave(currentX, currentY, currentZ, river, tSinWave))
        return true;
    
    return false;
}

bool gameWon(float frogZ, river_o *river)
{
    if (frogZ <= river->start - river->width) {
        return true;
    }

    return false;
}

void respawn(state *projectile, vec3fPolar *initVel, const vec3f *respawnPos, float frogRadius)
{
    projectile->r0.x = respawnPos->x;
    projectile->r0.y = respawnPos->y + frogRadius;
    projectile->r0.z = respawnPos->z;
    
    updateProjectileInitialState(projectile, initVel);
}

void initCubeVertices(float length, float width, float height, int size, vec3f vertices_cube[size]) 
{
    int i = 0;

    // front
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = length / 2;
    i++;
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = length / 2;
    i++;


    // back
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = -length / 2;
    i++;
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = -length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = -length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = -length / 2;
    i++;


    // left

    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = -length / 2;
    i++;
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = -length / 2;
    i++;
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = length / 2;
    i++;
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = length / 2;
    i++;

    // right
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = -length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = -length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = length / 2;
    i++;

    // top
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = -length / 2;
    i++;
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = height / 2;
    vertices_cube[i].z = -length / 2;
    i++;

    // bottom
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = -length / 2;
    i++;
    vertices_cube[i].x = -width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = length / 2;
    i++;
    vertices_cube[i].x = width / 2;
    vertices_cube[i].y = -height / 2;
    vertices_cube[i].z = -length / 2;
    i++;
}

void drawNormCube(int size, vec3f vertices[size])
{
    vec3f normal = {0, 0, 0};
    glColor3f(1, 1, 0);
    for (int i = 0; i < size; i++) {
            // front
            if (0 <= i && i < 4) {
                normal.x = 0;
                normal.y = 0;
                normal.z = 1;
            }
            // back
            if (4 <= i && i < 8) {
                normal.x = 0;
                normal.y = 0;
                normal.z = -1;
            }
            // left
            if (8 <= i && i < 12) {
                normal.x = -1;
                normal.y = 0;
                normal.z = 0;
            }

            // right
            if (12 <= i && i < 16) {
                normal.x = 1;
                normal.y = 0;
                normal.z = 0;
            }
            // top
            if (16 <= i && i < 20) {
                normal.x = 0;
                normal.y = 1;
                normal.z = 0;
            }
            // bottom
            if (20 <= i && i < 24) {
                normal.x = 0;
                normal.y = -1;
                normal.z = 0;
            }
            drawVector(vertices[i].x, vertices[i].y, vertices[i].z, normal.x, normal.y, normal.z, 1, true);
    }
}
