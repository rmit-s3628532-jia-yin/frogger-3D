#include "scene.h"

/* frogger 3D */

const bool normalized = true;
const float white[] = {1.0, 1.0, 1.0};
const float cyan[] = {0, 1.0, 1.0, 0.3};
const float yellow[] = {1, 1, 0};
const float brown[] = {0.8, 0.2, 0.2};
const float green[] = {0, 1, 0.1, 0.2};
const float red[] = {1, 0, 0};
const float chocolate[] = {0.9, 0.4, 0.1};
const float gray[] = {0.5, 0.5, 0.5, 1};
const float dark_brown[] = {0.3, 0.1, 0.1, 1};

void setLighting()
{
    float position[] = {1.0, 1.0, 1.0, 0.0};
    
    glMatrixMode(GL_MODELVIEW);
    
    glPushMatrix();
    glLoadIdentity();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glPopMatrix();
}

void setMaterial(const float amb_dif[3], const float specular[3], float shininess)
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, amb_dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

//  calculate normal vectors for the water wave
struct vec3f calculateWaterNormal(float x, float y, float t)
{
    float x_vec, y_vec;
    float gradient, b;
    vec3f normal;
    
    gradient = -1 / (0.2 * M_PI * cos(2 * M_PI * x + M_PI / 4.0 * t)); //  get the gradient of the normal, if two lines are perpendicular, the product of their gradients is -1
    b = y - gradient * x;
    // reverse the normal if pointing downwards
    if (gradient >= 0)
        x_vec = 0.1;
    else
        x_vec = -0.1;
        
    y_vec = gradient * (x + x_vec) + b - y;
        
    normal.x = x_vec;
    normal.y = y_vec;
    normal.z = 0;
    
    return normal;
}

// draw a grid representing the world
void drawWorld(int rows, int cols, vec3f vertices_world[rows][cols], bool showAxes, bool filled, river_o *river, GLuint texture)
{
    int nX, nZ;
    float xStep, zStep;
    bool have_texture = false;  // don't texture above the river

    nX = rows - 1;
    nZ = cols - 1;

    xStep = 2.0 / nX;                
    zStep = 2.0 / nZ;

    if (showAxes)
        drawAxes(1.0);

    glColor4f(0, 1.0, 0.0, 0.6);

    if (filled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    setMaterial(green, white, 10.0);

    glBindTexture(GL_TEXTURE_2D, texture);

    for (int j = 0; j < nZ; j++) {
        glBegin(GL_TRIANGLE_STRIP);
        float z = -1.0 + j * zStep;
        for (int i = 0; i <= nX; i++) {
            float x = -1.0 + i * xStep;

            glNormal3f(0, 1, 0);

            if (z < river->start - river->width || z >= river->start) {
                have_texture = true;
                if (i % 2 == 0)
                    glTexCoord2f(0, 0);
                else
                    glTexCoord2f(1, 1);
            }

            glVertex3f(x, 0, z);

            //store vertex
            vertices_world[j][i].x = x;
            vertices_world[j][i].y = 0;
            vertices_world[j][i].z = z;

            z += zStep;
            glNormal3f(0, 1, 0);

            if (have_texture) {

                if (i % 2 == 0)
                    glTexCoord2f(1, 0);
                else
                    glTexCoord2f(0, 1);
            }

            glVertex3f(x, 0, z);

            //store vertex
            vertices_world[j + 1][i].x = x;
            vertices_world[j + 1][i].y = 0;
            vertices_world[j + 1][i].z = z;

            z -= zStep; // reset z value
            have_texture = false;
        }
        glEnd();                 
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawRoad(road_o road, bool filled, GLuint texture) 
{
    if (filled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glColor4f(0.5, 0.5, 0.5, 1);
    setMaterial(gray, white, 20);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0, 0, road.start - road.width); 

    glBegin(GL_POLYGON);    // back

    glNormal3f(0, 0, -1);   // normals for lighting
    glVertex3f(-1, 0, 0);
    glVertex3f(-1, road.height, 0);
    glVertex3f(1, road.height, 0);
    glVertex3f(1, 0, 0);
    
    glEnd();

    glBegin(GL_POLYGON);    // left

    glNormal3f(-1, 0, 0);
    glVertex3f(-1, 0, 0);
    glVertex3f(-1, road.height, 0);
    glVertex3f(-1, road.height, road.width);
    glVertex3f(-1, 0, road.width);
    
    glEnd();

    glBegin(GL_POLYGON);    // front

    glNormal3f(0, 0, 1);
    glVertex3f(-1, 0, road.width);
    glVertex3f(-1, road.height, road.width);
    glVertex3f(1, road.height, road.width);
    glVertex3f(1, 0, road.width);
    
    glEnd();

    glBegin(GL_POLYGON);    // right

    glNormal3f(1, 0, 0);
    glVertex3f(1, 0, road.width);
    glVertex3f(1, road.height, road.width);
    glVertex3f(1, road.height, 0);
    glVertex3f(1, 0, 0);
    
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_POLYGON);    // top

    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 1);
    glVertex3f(-1, road.height, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-1, road.height, road.width);
    glTexCoord2f(1, 0);
    glVertex3f(1, road.height, road.width);
    glTexCoord2f(1, 1);
    glVertex3f(1, road.height, 0);
    
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);

    glPopMatrix();
}

void drawCars(car_o cars[], int size, road_o road, int rows, int cols, vec3f vertices_cars[rows][cols], bool showAxes, float dt, bool filled)
{
    setMaterial(red, white, 25.0);

    float start, length, height, width;
    float lane, laneOffset, xTranslation, yTranslation;
    int row_counter, col_counter;
    int scaleT;

    if (filled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    scaleT = 3;
    row_counter = 0;

    laneOffset = 0.1;
    lane = (road.start - road.width) + laneOffset;    // translation along z axis

    for (int i = 0; i < size; i++) {   
        col_counter = 0;    

        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();

        start = cars[i].start;
        length = cars[i].length;
        height = cars[i].height;
        width = cars[i].width;

        yTranslation = road.height + height / 2.0;

        /* go left or right */
        if (cars[i].direction == left)
            cars[i].start -= dt / scaleT;
        if (cars[i].direction == right)
            cars[i].start += dt / scaleT;

        // turn left when the car reaches the right end of the road
        if (cars[i].start + width / 2.0 >= 1.0 - width) {
            cars[i].direction = left;
            cars[i].start -= dt / scaleT;
        }

        // turn right when the car reaches the left end of the road
        if (cars[i].start - width / 2.0 <= -1.0 + width) {
            cars[i].direction = right;
            cars[i].start += dt / scaleT;
            // printf("car: %d going %d x translation: %f \n", i, cars[i].direction, cars[i].start);
        }

        glTranslatef(cars[i].start, yTranslation, lane);

        if (showAxes)
            drawAxes(0.3);

        glColor3f(1, 0, 0);

        glBegin(GL_POLYGON);    // back

        glNormal3f(0, 0, -1);   // normals for lighting
        glVertex3f(-length / 2.0, height / 2.0, -width / 2.0);
        glVertex3f(length / 2.0, height / 2.0, -width / 2.0);
        glVertex3f(length / 2.0, -height / 2.0, -width / 2.0);
        glVertex3f(-length / 2.0, -height / 2.0, -width / 2.0);
        
        glEnd();

        // store vertices
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;

        glBegin(GL_POLYGON);    // left

        glNormal3f(-1, 0, 0);
        glVertex3f(-length / 2.0, height / 2, -width / 2.0);
        glVertex3f(-length / 2.0, height / 2, width / 2.0);
        glVertex3f(-length / 2.0, -height / 2, width / 2.0);
        glVertex3f(-length / 2.0, -height / 2, -width / 2.0);
        
        glEnd();

        // store vertices
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;

        glBegin(GL_POLYGON);    // front

        glNormal3f(0, 0, 1);
        glVertex3f(-length / 2.0, height / 2, width / 2.0);
        glVertex3f(length / 2.0, height / 2, width / 2.0);
        glVertex3f(length / 2.0, -height / 2, width / 2.0);
        glVertex3f(-length / 2.0, -height / 2, width / 2.0);
        
        glEnd();

        // store vertices
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;

        glBegin(GL_POLYGON);    // right

        glNormal3f(1, 0, 0);
        glVertex3f(length / 2.0, height / 2, -width / 2.0);
        glVertex3f(length / 2.0, height / 2, width / 2.0);
        glVertex3f(length / 2.0, -height / 2, width / 2.0);
        glVertex3f(length / 2.0, -height / 2, -width / 2.0);
        
        glEnd();

        // store vertices
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;

        glBegin(GL_POLYGON);    // top

        glNormal3f(0, 1, 0);
        glVertex3f(-length / 2.0, height / 2, -width / 2.0);
        glVertex3f(length / 2.0, height / 2, -width / 2.0);
        glVertex3f(length / 2.0, height / 2, width / 2.0);
        glVertex3f(-length / 2.0, height / 2, width / 2.0);
        
        glEnd();

        // store vertices
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;

        glBegin(GL_POLYGON);    // bottom

        glNormal3f(0, 1, 0);
        glVertex3f(-length / 2.0, -height / 2, -width / 2.0);
        glVertex3f(length / 2.0, -height / 2, -width / 2.0);
        glVertex3f(length / 2.0, -height / 2, width / 2.0);
        glVertex3f(-length / 2.0, -height / 2, width / 2.0);
        
        glEnd();

        // store vertices
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = -width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;
        vertices_cars[row_counter][col_counter].x = -length / 2.0 + cars[i].start;
        vertices_cars[row_counter][col_counter].y = -height / 2.0 + yTranslation;
        vertices_cars[row_counter][col_counter].z = width / 2.0 + lane;
        col_counter++;

        glPopMatrix();

        row_counter++;
        lane += road.width / 5.0;
    }
}

void drawRiver(river_o river, int rows, int num_segments, vec3f vertices_water[rows][num_segments], bool showAxes, float t, bool filled, GLuint texture)
{
    if (filled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else 
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindTexture(GL_TEXTURE_2D, texture);

    /* draw river bed */
    setMaterial(dark_brown, gray, 15.0);
    glColor4fv(dark_brown);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0, 0, -(river.width - river.start)); 

    glBegin(GL_POLYGON);    // back

    glNormal3f(0, 0, 1);   // normals for lighting
    glVertex3f(-1, 0, 0);
    glVertex3f(-1, -river.depth, 0);
    glVertex3f(1, -river.depth, 0);
    glVertex3f(1, 0, 0);
    
    glEnd();

    glBegin(GL_POLYGON);    // front

    glNormal3f(0, 0, -1);   
    glVertex3f(-1, 0, river.width);
    glVertex3f(-1, -river.depth, river.width);
    glVertex3f(1, -river.depth, river.width);
    glVertex3f(1, 0, river.width);
    
    glEnd();

    glBegin(GL_POLYGON);    // left

    glNormal3f(1, 0, 0);   
    glVertex3f(-1, 0, 0);
    glVertex3f(-1, -river.depth, 0);
    glVertex3f(-1, -river.depth, river.width);
    glVertex3f(-1, 0, river.width);
    
    glEnd();

    glBegin(GL_POLYGON);    // right

    glNormal3f(-1, 0, 0);   
    glVertex3f(1, 0, 0);
    glVertex3f(1, -river.depth, 0);
    glVertex3f(1, -river.depth, river.width);
    glVertex3f(1, 0, river.width);
    
    glEnd();

    glBegin(GL_POLYGON);    // bottom

    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);   
    glVertex3f(-1, -river.depth, 0);
    glTexCoord2f(1, 0);
    glVertex3f(-1, -river.depth, river.width);
    glTexCoord2f(1, 1);
    glVertex3f(1, -river.depth, river.width);
    glTexCoord2f(0, 1);
    glVertex3f(1, -river.depth, 0);
    
    glEnd();

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);

    /* draw water wave */
    setMaterial(cyan, white, 50.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    // translate water wave to the right position in the world
    glTranslatef(0, -river.depth / 2.0, -(river.width - river.start) + river.width / 2.0);

    if (showAxes)
        drawAxes(1.0);  // draw local coordinates

    glColor4f(0, 1, 1, 0.3);

    int n = num_segments - 1;
    float xStep = 2.0 / n;
    float zStep = river.width / n; 
    vec3f normal;
    
    for (int j = 0; j < n; j++) {
        glBegin(GL_TRIANGLE_STRIP);
        float z = -river.width / 2.0 + j * zStep;
        for (int i = 0; i <= n; i++) {
            float x = -1.0 + i * xStep;
            float y = 0.1 * sin (2 * M_PI * x + M_PI / 4 * t);  // sin function
            
            normal = calculateWaterNormal(x, y, t);     // calculate normal
            
            glNormal3f(normal.x, normal.y, 0);
            glVertex3f(x, y, z);
            
            //store vertex
            vertices_water[j][i].x = x;
            vertices_water[j][i].y = y;
            vertices_water[j][i].z = z;

            z += zStep;     // work out next z value
            
            normal = calculateWaterNormal(x, y, t);     // calculate normal
            
            glNormal3f(normal.x, normal.y, 0);
            glVertex3f(x, y, z);
            
            //store vertex
            vertices_water[j + 1][i].x = x;
            vertices_water[j + 1][i].y = y;
            vertices_water[j + 1][i].z = z;
            
            z -= zStep;
        }
        glEnd();
    }
    glPopMatrix();
}

void drawLogs(log_o logs[], float log_length, float r, int size, int num_segments, river_o river, int rows, int cols, vec3f vertices_logs[rows][cols], bool showAxes, float t, float dt, bool filled, GLuint texture)
{
    const int slices = num_segments;
    float theta;
    float x, y, z;
    float xTranslation;
    int col_counter = 0;
    int lanes = 4;
    float scaleDt = 3;

    if (filled)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    setMaterial(chocolate, white, 50.0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glMatrixMode(GL_MODELVIEW);
    
    for (int i = 0; i < size; i++) {
        float lane = (float)i / (lanes + 4) + 2 * r;   // the z coordinate of the lane

        glPushMatrix();
        glTranslatef(0, -river.depth / 2.0, 0); // river depth offset

        /* go left */
        if (logs[i].direction == left) {
            logs[i].xPos -= dt / scaleDt;
            xTranslation = logs[i].xPos - log_length / 2.0;
        }
        /* go right */
        if (logs[i].direction == right) {
            logs[i].xPos += dt / scaleDt;
            xTranslation = logs[i].xPos - log_length / 2.0;
        }

        // turn left when the log reaches the right end of the river
        if (logs[i].xPos + log_length / 2 >= 1.0) {
            logs[i].direction = left;
            logs[i].xPos -= dt / scaleDt;
        }

        // turn right when the car reaches the left end of the river
        if (logs[i].xPos - log_length / 2 <= -1.0) {
            logs[i].direction = right;
            logs[i].xPos += dt / scaleDt;
            // printf("car: %d going %d x translation: %f \n", i, cars[i].direction, cars[i].start);
        }

        glTranslatef(xTranslation, 0.1 * sin (2 * M_PI * xTranslation + M_PI / 4 * t), -(river.width - river.start) + lane);
        glRotatef(90.0, 0, 1, 0);

        if (showAxes)
            drawAxes(log_length + 0.1);

        glColor3f(1, 0.5, 0);

        // draw the tube
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= slices; i++) {
            theta = i / (float)slices * 2.0 * M_PI;
            x = r * cosf(theta);
            y = r * sinf(theta);
            
            if (i % 2 == 0)
                glTexCoord2f(0, 0);
            else 
                glTexCoord2f(1, 1);

            glNormal3f(x, y, 0);
            glVertex3f(x, y, log_length);

            if (i % 2 == 0)
                glTexCoord2f(1, 0);
            else 
                glTexCoord2f(0, 1);

            glNormal3f(x, y, 0);
            glVertex3f(x, y, 0.0);

            // store vertices
            vertices_logs[0][col_counter].x = x;
            vertices_logs[0][col_counter].y = y;
            vertices_logs[0][col_counter].z = log_length;
            col_counter++;

            vertices_logs[0][col_counter].x = x;
            vertices_logs[0][col_counter].y = y;
            vertices_logs[0][col_counter].z = 0.0;
            col_counter++;
        }
        glEnd();

        // draw the top of cylinder
        glBegin(GL_POLYGON);
        for (int i = 0; i <= slices; i++) {
            theta = i / (float)slices * 2.0 * M_PI;
            x = r * cosf(theta);
            y = r * sinf(theta);
            
            glNormal3f(0, 0, 1);
            glVertex3f(x, y, log_length);
            // store vertices
            vertices_logs[0][col_counter].x = x;
            vertices_logs[0][col_counter].y = y;
            vertices_logs[0][col_counter].z = log_length;
            col_counter++;
        }
        glEnd();

        // draw the bottom of cylinder
        glBegin(GL_POLYGON);
        for (int i = 0; i <= slices; i++) {
            theta = i / (float)slices * 2.0 * M_PI;
            x = r * cosf(theta);
            y = r * sinf(theta);
            
            glNormal3f(0, 0, -1);
            glVertex3f(x, y, 0);
            // store vertices
            vertices_logs[0][col_counter].x = x;
            vertices_logs[0][col_counter].y = y;
            vertices_logs[0][col_counter].z = 0.0;
            col_counter++;
        }
        glEnd();
        glPopMatrix();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

//  draw normals for the flat grid(world) 
void drawNormWorld(int rows, int cols, vec3f vertices_world[rows][cols])
{
    glColor3f(1, 1, 0);
    setMaterial(yellow, white, 10);
    vec3f normal = {0, 1, 0};	// pointing upward
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            //  draw normal
            drawVector(vertices_world[i][j].x, vertices_world[i][j].y, vertices_world[i][j].z, normal.x, normal.y, normal.z, 0.2, normalized);
        }
    }
}

void drawNormCars(int rows, int cols, vec3f vertices_cars[rows][cols])
{
    vec3f normal = {0, 1, 0};
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            //  normals pointing backward
            if (0 <= j && j < 4) {
                normal.x = 0;
                normal.y = 0;
                normal.z = -1;
            }
            // pointing left
            if (4 <= j && j < 8) {
                normal.x = -1;
                normal.y = 0;
                normal.z = 0;
            }
            // front
            if (8 <= j && j < 12) {
                normal.x = 0;
                normal.y = 0;
                normal.z = 1;
            }
            // right
            if (12 <= j && j < 16) {
                normal.x = 1;
                normal.y = 0;
                normal.z = 0;
            }
            // top
            if (16 <= j && j < 20) {
                normal.x = 0;
                normal.y = 1;
                normal.z = 0;
            }
            drawVector(vertices_cars[i][j].x, vertices_cars[i][j].y, vertices_cars[i][j].z, normal.x, normal.y, normal.z, 0.1, normalized);
        }
    }
}

void drawNormWater(int rows, int cols, vec3f vertices_water[rows][cols], river_o river, float t) 
{
    vec3f normal;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0, -river.depth / 2.0, -(river.width - river.start) + river.width / 2.0);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {    
            normal = calculateWaterNormal(vertices_water[i][j].x, vertices_water[i][j].y, t);
            //  draw normal
            drawVector(vertices_water[i][j].x, vertices_water[i][j].y, vertices_water[i][j].z, normal.x, normal.y, normal.z, 0.2, normalized);
        }
    }
    glPopMatrix();
}

void drawNormLogs(int rows, int cols, vec3f vertices_logs[rows][cols], river_o river, int num_segments, log_o logs[], float log_length, float r, float t) 
{
    vec3f normal;
    int lanes = 4;
    float xTranslation;

    glMatrixMode(GL_MODELVIEW);

    for (int i = 0; i < rows; i++) {
        glPushMatrix();

        float lane = (float)i / (lanes + 4) + 2 * r;   // the z coordinate of the lane

        glTranslatef(0, -river.depth / 2.0, 0); // river depth offset

        xTranslation = logs[i].xPos - log_length / 2.0;
        glTranslatef(xTranslation, 0.1 * sin (2 * M_PI * xTranslation + M_PI / 4 * t), -(river.width - river.start) + lane);
        glRotatef(90.0, 0, 1, 0);

        for (int j = 0; j < cols; j++) {   
            if (j < (num_segments + 1) * 2) {
                normal.x = vertices_logs[i][j].x;
                normal.y = vertices_logs[i][j].y;
                normal.z = 0;
            }
            if ((num_segments + 1) * 2 <= j && j < (num_segments + 1) * 3) {
                normal.x = 0;
                normal.y = 0;
                normal.z = 1;
            }
            if ((num_segments + 1) * 3 <= j && j < (num_segments + 1) * 4) {
                normal.x = 0;
                normal.y = 0;
                normal.z = -1;
            }
            drawVector(vertices_logs[i][j].x, vertices_logs[i][j].y, vertices_logs[i][j].z, normal.x, normal.y, normal.z, 0.1, normalized);
        }
        glPopMatrix();
    }
}


