#include "game.h"

/* frogger 3D */

/* projectile related initilizations */
const vec3f respawnPos = {0, 0, 1};
vec3fPolar initVel = {0.3, 60.0, 180.0};

global_t global = { false, false, false, 0.0, numerical, false, 0, 0.0, 0.2, 0.0, 0.0, -1, false, 4, false, false, false, false, 0.0, false, 5, 0, false};

/* initilizations */
const int max_num_segments = 128;
const int num_cars = 4;
const int num_logs = 4;
const int num_cubes_frog = 9;
const float log_length = 0.3;
const float log_radius = 0.04;
const int num_vert_each_car = 24;
const int num_vert_each_cube = num_vert_each_car;
const float maxMagnitude = 0.7;
const enum directions goLeft = left;
const enum directions goRight = right;

static GLuint texture_road, texture_grass, texture_sand, texture_wood;

road_o road = {0.8, 0.4, 0.1};

car_o cars[num_cars] = {
    { -0.76, 0.1, 0.1, 0.06, goLeft },
    { -0.62, 0.1, 0.1, 0.06, goRight },
    { 0.5, 0.1, 0.1, 0.06, goLeft },
    { 0.37, 0.1, 0.1, 0.06, goRight }
};
log_o logs[num_logs] = {
    {-0.6, goLeft},
    {0.0, goRight},
    {0.4, goLeft},
    {0.55, goRight},
};

river_o river = {0, 0.5, 0.25};
camera_g camera = {0, 0, 0, 0, -1, 300.0, 1.0};

state projectile = {
    { 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0 }
};

sphere frog = {8, 8, 0.07};

// arrays to store geometry vertices 
vec3f vertices_world[max_num_segments + 1][max_num_segments + 1];
vec3f vertices_cars[num_cars][num_vert_each_car];
vec3f vertices_water[max_num_segments + 1][max_num_segments + 1];
vec3f vertices_logs[num_logs][(max_num_segments + 1) * 4];
vec3f vertices_frog_body[num_vert_each_cube];
vec3f vertices_frog_arm[num_vert_each_cube];
vec3f vertices_frog_elbow[num_vert_each_cube];

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
	int num_segments = global.num_segments;
	int num_vert_each_log = (num_segments + 1) * 4;

	if (global.texture)
		glEnable(GL_TEXTURE_2D);	// enable texture
	else
		glDisable(GL_TEXTURE_2D);

    setupCamera(&camera, &projectile);

    /* lighting */
    if (global.lighting)
    	setLighting();
    else
    	glDisable(GL_LIGHTING);


    displayProjectile(&projectile, &frog, global.filled, global.end, global.showAxes, global.visualizeNormal, num_vert_each_cube, vertices_frog_body, vertices_frog_arm, vertices_frog_elbow);
    
	/* draw cars */
    drawCars(cars, sizeof(cars)/sizeof(cars[0]), road, num_cars, num_vert_each_car, vertices_cars, global.showAxes, global.dtScene, global.filled);

    drawLogs(logs, log_length, log_radius, sizeof(logs)/sizeof(logs[0]), num_segments, river, num_logs, num_vert_each_log, vertices_logs, global.showAxes, global.tSinWave, global.dtScene, global.filled, texture_wood);

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* draw road */
    drawRiver(river, num_segments + 1, num_segments + 1, vertices_water, global.showAxes, global.tSinWave, global.filled, texture_sand);
    drawRoad(road, global.filled, texture_road);

    drawWorld(num_segments + 1, num_segments + 1, vertices_world, global.showAxes, global.filled, &river, texture_grass);

    glDisable(GL_BLEND);

    /* draw normals */
    if (global.visualizeNormal) {
        drawNormWorld(num_segments + 1, num_segments + 1, vertices_world);
        drawNormCars(num_cars, num_vert_each_car, vertices_cars);
        drawNormWater(num_segments + 1, num_segments + 1, vertices_water, river, global.tSinWave);
        drawNormLogs(num_logs, num_vert_each_log, vertices_logs, river, num_segments, logs, log_length, log_radius, global.tSinWave);
    }

    // draw vector and trajactory
    if (!global.end) {
        drawJumpVector(&projectile);
        displayTrajectoryNumerical(&projectile, &frog, cars, &road, &river, logs, 0.001, global.tSinWave, sizeof(cars)/sizeof(cars[0]), sizeof(logs)/sizeof(logs[0]), log_length, log_radius);
    }

    displayOSD();

    /* Always check for errors! */
    int err;
    while ((err = glGetError()) != GL_NO_ERROR)
    printf("display: %s\n", gluErrorString(err));
    
    glutSwapBuffers();
    global.frames++;
    glutPostRedisplay();
}

void update()
{
    static float lastT = -1.0;
    float t, dt;

    if (global.animationOff) {
        lastT = glutGet(GLUT_ELAPSED_TIME) / (float)MILLI - global.startTime;
        global.dtScene = 0;
        return;
    }

    /* game over check */
    if (global.score == 5 || global.lives_left == 0)  
        global.gameOver = true;

    if (global.gameOver)
        return;  

    /* player has won the game when th frog hits the opposite side of the river */
    if (gameWon(projectile.r0.z, &river)) {
        respawn(&projectile, &initVel, &respawnPos, frog.radius);
        global.score++;
    }
    
    global.tSinWave = glutGet(GLUT_ELAPSED_TIME) / (float)MILLI;
    t = glutGet(GLUT_ELAPSED_TIME) / (float)MILLI - global.startTime;
    
    if (lastT < 0.0) {
        lastT = t;
        return;
    }
    
    dt = t - lastT;

    lastT = t;
    
    // printf("%f %f\n", t, dt);
    if (dt <= 0)
        return;

    if (global.respawned) {
        lastT = -1.0;
        global.respawned = !global.respawned;
        return;
    }

    global.dtScene = dt;	// dt used for animating cars

    /* collision with cars and water wave */
    // lost the game 
    if (collidedWithCars(projectile.r.x, projectile.r.y, projectile.r.z, &road, &frog, cars, num_cars) || collidedWithWave(projectile.r.x, projectile.r.y, projectile.r.z, &river, global.tSinWave)) {
        global.end = true;
        global.go = false;
        lastT = -1.0;

        global.lives_left--;
        respawn(&projectile, &initVel, &respawnPos, frog.radius);
        global.go = false;
        global.respawned = true;
        global.end = false;
        global.logHit = -1;

        return;
    }
    // frog is not jumping and it's resting on the log
    if (!global.go && global.logHit != -1) {
//        printf("resting on the log \n");
        updateFrogInitialPosition(&projectile, &frog, &road, true, logs[global.logHit], &river, global.tSinWave, log_radius);
        updateProjectileInitialState(&projectile, &initVel);
        glutPostRedisplay();
        return;
    }
    
    // frog not moving if space has not been hit or animation is off
    if (!global.go) {
        glutPostRedisplay();
        return;
    }
    
    updateProjectileStateNumerical(&projectile, dt);

    /* Frame rate */
    dt = t - global.lastFrameRateT;
    if (dt > global.frameRateInterval) {
        global.frameRate = global.frames / dt;
        global.lastFrameRateT = t;
        global.frames = 0;
    }

    /* collision with ground and road */
    if (collidedWithGround(projectile.r.y, projectile.r.z, &frog, &river) || collidedWithRoad(projectile.r.x, projectile.r.y, projectile.r.z, &frog, &road)) {
        updateFrogInitialPosition(&projectile, &frog, &road, false, logs[global.logHit], &river, global.tSinWave, log_radius);
        updateProjectileInitialState(&projectile, &initVel);
        global.go = false;
        lastT = -1.0;
        return;
    }

    global.logHit = collidedWithLogs(projectile.r.x, projectile.r.y, projectile.r.z, &frog, logs, &river, global.tSinWave, sizeof(logs)/sizeof(logs[0]), log_length, log_radius);
    // if frog has hit a log, step on the log
    if (global.logHit != -1) {
        updateFrogInitialPosition(&projectile, &frog, &road, true, logs[global.logHit], &river, global.tSinWave, log_radius);
        updateProjectileInitialState(&projectile, &initVel);
        global.go = false;
        lastT = -1.0;
        return;
    }

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    	case 't':
    		global.texture = !global.texture;
    		break;
        case 'g':
            global.animationOff = !global.animationOff;
            break;
        case 'r':
            respawn(&projectile, &initVel, &respawnPos, frog.radius);
            global.go = false;
            global.respawned = true;
            global.end = false;
            global.logHit = -1;
            break;
    	case 'o':
            global.OSD = !global.OSD;
            break;
    	case '+':
            if (global.num_segments >= 128)
                break;
            global.num_segments *= 2;
            break;
        case '-':
            if (global.num_segments <= 4)
                break;
            global.num_segments /= 2;
            break;
    	case 'l':
    		global.lighting = !global.lighting;
    		break;
    	case 'f':
            global.filled = !global.filled;
            break;
        case 27:
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        case 'n':
            global.visualizeNormal = !global.visualizeNormal;
        	break;
        case 'x':
            global.showAxes = !global.showAxes;
            break;
        case 'a':
	        initVel.rotationY += 1.0;
            updateProjectileInitialState(&projectile, &initVel);
        	break;
    	case 'd':
	        initVel.rotationY -= 1.0;
	        updateProjectileInitialState(&projectile, &initVel);
	    	break;
        case ' ':
            if (global.end) {
                break;
            }
            if (!global.go) {
                global.startTime = glutGet(GLUT_ELAPSED_TIME) / (float)MILLI;
                global.go = true;
            }
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

//  keyboard callback for direction keys
void specialKeyboard(int key, int x, int y)
{
    // if game ends, disable adjustments of magnitude and angle
    if (global.end) {
        return;
    }
    
    switch (key) {
        case GLUT_KEY_UP:
            if (initVel.magnitude >= maxMagnitude)
                break;
            initVel.magnitude += 0.01;
           // printf("magnitude is increased to %f\n", initVel.magnitude);
            break;
        case GLUT_KEY_DOWN:
            if (initVel.magnitude <= 0.0)
                break;
            initVel.magnitude -= 0.01;
           // printf("magnitude is decreased to %f\n", initVel.magnitude);
            break;
        case GLUT_KEY_LEFT:
            if (initVel.angle >= 180.0)
                break;
            initVel.angle += 1.0;
//            printf("angle is increased to %f\n", initVel.angle);
            break;
        case GLUT_KEY_RIGHT:
            if (initVel.angle <= 0.0)
                break;
            initVel.angle -= 1.0;
//            printf("angle is decreased to %f\n", initVel.angle);
            break;
        default:
            break;
    }
    
    updateProjectileInitialState(&projectile, &initVel);
    glutPostRedisplay();
}

void init()
{
	projectile.r0.x = respawnPos.x;
    projectile.r0.y = respawnPos.y + frog.radius;
    projectile.r0.z = respawnPos.z;
    
    updateProjectileInitialState(&projectile, &initVel);

    initCubeVertices(5, 3, 3, num_vert_each_cube, vertices_frog_body);
    initCubeVertices(1.5, 1.5, 2, num_vert_each_cube, vertices_frog_arm);
    initCubeVertices(1.2, 1.2, 2, num_vert_each_cube, vertices_frog_elbow);

    glEnable(GL_NORMALIZE); 
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void displayOSD()
{
    char buffer[30];
    char *bufp;
    int w, h;
    
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    /* Set up orthographic coordinate system to match the
     window, i.e. (0,0)-(w,h) */
    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /* game title */
    glColor3f(0, 1.0, 0.0);
    glRasterPos2i(w / 2.0 - 25, h - 20);
    snprintf(buffer, sizeof buffer, "Frogger");
    for (bufp = buffer; *bufp; bufp++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

    /* number of lives left */
    glRasterPos2i(w / 2.0 - 25, h - 40);
    snprintf(buffer, sizeof buffer, "Lives left: %d", global.lives_left);
    for (bufp = buffer; *bufp; bufp++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

    /* score */
    glRasterPos2i(w / 2.0 - 25, h - 60);
    snprintf(buffer, sizeof buffer, "Score: %d", global.score);
    for (bufp = buffer; *bufp; bufp++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

    if (global.OSD) {
        /* Frame rate */
        glColor3f(1.0, 1.0, 0.0);
        glRasterPos2i(10, 60);
        snprintf(buffer, sizeof buffer, "fr (f/s): %6.0f", global.frameRate);
        for (bufp = buffer; *bufp; bufp++)
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
        
        /* Time per frame */
        glColor3f(1.0, 1.0, 0.0);
        glRasterPos2i(10, 40);
        snprintf(buffer, sizeof buffer, "ft (ms/f): %5.0f", 1.0 / global.frameRate * 1000.0);
        for (bufp = buffer; *bufp; bufp++)
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
        
        /* tesselation control */
        glColor3f(1.0, 1.0, 0.0);
        glRasterPos2i(10, 20);
        snprintf(buffer, sizeof buffer, "tess: %i", global.num_segments);
        for (bufp = buffer; *bufp; bufp++)
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
    }

    if (global.gameOver) {
        glColor3f(1.0, 1.0, 0.0);
        glRasterPos2i(w / 2.0 - 25, h / 2.0);
        snprintf(buffer, sizeof buffer, "GAME OVER");
        for (bufp = buffer; *bufp; bufp++)
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
    }

    /* Pop modelview */
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    
    /* Pop projection */
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    /* Pop attributes */
    glPopAttrib();

    glEnable(GL_LIGHTING);
}

void passiveMotion(int x, int y)
{
    camera.lastX = x;
    camera.lastY = y;
}

void mouseMotion(int x, int y)
{
    //    left button for rotation
    if (camera.buttonPressed == 0)
    {
        camera.rotation_angle_y += x - camera.lastX;
        camera.lastX = x;
        camera.rotation_angle_x += y - camera.lastY;
        camera.lastY = y;
    }
    
    //    right button for zoom
    if (camera.buttonPressed == 2)
    {
        camera.zoomFactor += y - camera.lastY;
        camera.lastY = y;
    }
    
    glutPostRedisplay();
}

void mousePress(int button, int state, int x, int y)
{
    camera.buttonPressed = button;
}

static GLuint loadTexture(const char *filename)
{
	GLuint tex = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (!tex)
		return 0;

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(500, 500);
    glutCreateWindow("Frogger");
    
    if (argc < 5)
	{
		printf("Specify image filenames to display.\n");
		return EXIT_FAILURE;
	}

	texture_road = loadTexture(argv[1]);
	texture_grass = loadTexture(argv[2]);
	texture_sand = loadTexture(argv[3]);
	texture_wood = loadTexture(argv[4]);

	// if (!texture)
	// {
	// 	printf("No texture created; exiting.\n");
	// 	return EXIT_FAILURE;
	// }

    init();
    
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutIdleFunc(update);
    glutMouseFunc(mousePress);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(passiveMotion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutMainLoop();
    
    return EXIT_SUCCESS;
}
