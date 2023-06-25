#include <iostream>
#include <GL/glut.h>
#include <cmath>

//Matrix properties
const int rows = 10;
const int columns = 10;
const int depth = 10;
bool matrix[rows][columns][depth]{};
int matrixChange[rows][columns][depth]{0};

// Camera position
float cameraRotationSpeed = 0.007f;
float cameraAngleX = 0.3;
float cameraAngleY = 0.0;
const float cameraDistance = std::min(rows, depth) + 12;
int mousePrevX = 0;
int mousePrevY = 0;
bool isMouseLeftButtonPressed = false;

int seed = 237382;
float prob = 0.2;



int ALIVE = 3;
int BORNING = 2;
int DYING = 1;
int DEAD = 0;

float elapsedTime = 0;
float iterationTime = 6000;

double randZO() {
    return ((double) rand() / (RAND_MAX));
}

void initializeMatrix() {
    srand(seed);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            for (int k = 0; k < depth; ++k) {
                // Assign a value to the array element
                matrix[i][j][k] = randZO() < prob;

                if (matrix[i][j][k]) {
                    matrixChange[i][j][k] = ALIVE;
                } else {
                    matrixChange[i][j][k] = DEAD;
                }
            }
        }
    }
}

bool isCoordinateValid(int x, int y, int z) {

    if (x > rows - 1 || x < 0) {
        return false;
    }

    if (y > columns -1 || y < 0) {
        return false;
    }

    if (z > depth - 1 || z < 0) {
        return false;
    }

    return true;
}

bool willLive(int x, int y, int z){
    int livingNeighbours = 0;
    int x_neighbour;
    int y_neighbour;
    int z_neighbour;
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            for (int k = -1; k < 2; ++k) {
                x_neighbour = x + i;
                y_neighbour = y + j;
                z_neighbour = z + k;

                //verifica se itera livingNeighbours
                if(i+j+k == 0 || !isCoordinateValid(x_neighbour, y_neighbour, z_neighbour)) {
                    continue;
                }

                if (matrix[x_neighbour][y_neighbour][z_neighbour]) {
                    livingNeighbours++;
                }
            }
        }
    }

    if(matrix[x][y][z]){
        if(livingNeighbours > 3 || livingNeighbours < 2){
            return false;
        } else {
            return true;
        }
    } else if (livingNeighbours == 3){
        return true;
    }

    return false;
}

void copyArray(bool (nextMatrix)[rows][columns][depth]) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            for (int k = 0; k < depth; ++k) {
                matrix[i][j][k] = nextMatrix[i][j][k];
            }
        }
    }
}

void generateNextMatrix() {
    bool nextMatrix[rows][columns][depth];

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            for (int k = 0; k < depth; ++k) {
                nextMatrix[i][j][k] = willLive(i, j, k);

                if (matrix[i][j][k] && !nextMatrix[i][j][k]) {
                    matrixChange[i][j][k] = DYING;
                } else if (!matrix[i][j][k] && nextMatrix[i][j][k]) {
                    matrixChange[i][j][k] = BORNING;
                } else if (matrix[i][j][k] && nextMatrix[i][j][k]) {
                    matrixChange[i][j][k] = ALIVE;
                } else {
                    matrixChange[i][j][k] = DEAD;
                }
            }
        }
    }

    copyArray(nextMatrix);
}

void drawCube(int x, int y, int z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Desenhar um cubo
    // GLfloat color[] = {randZO(), randZO(), randZO()};
    // glColor3fv(color);

    float cubeSizeMultiplier = 1.0f;
    GLfloat color[] = {0.8, 0.8, 0.8};

    const float animationSlice = 0.8;
    const float scaleRate = elapsedTime / (iterationTime*animationSlice);
    const float colorRateRed = elapsedTime / (iterationTime*animationSlice/4);
    const float degradeGreenThreshold = 0.5;
    float colorRateGreen = 0;

    if (elapsedTime > iterationTime*0.5) {
        colorRateGreen =
                (elapsedTime - iterationTime*degradeGreenThreshold)
                / (iterationTime*animationSlice - iterationTime*degradeGreenThreshold);
    }

    if( elapsedTime / iterationTime < animationSlice) {
        if(matrixChange[x][y][z] == BORNING) {
            color[0] = 0.8*colorRateGreen;
            color[1] = 0.8;
            color[2] = 0.8*colorRateGreen;
            cubeSizeMultiplier = scaleRate;
        } else if (matrixChange[x][y][z] == DYING) {
            color[0] = 0.8;
            color[1] = 0.8 - 0.8*colorRateRed;
            color[2] = 0.8 - 0.8*colorRateRed;
            cubeSizeMultiplier = 1 - scaleRate;
        }
    } else if (matrixChange[x][y][z] == DYING) {
        glPopMatrix();
        return;
    }



    // Enable wireframe rendering mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.3f, 0.3f, 0.3f);
    glutSolidCube(0.5*cubeSizeMultiplier);

    // Disable wireframe rendering mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3fv(color);
    glutSolidCube(0.5*cubeSizeMultiplier);

    glPopMatrix();
}

void drawMatrix() {
   for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            for (int k = 0; k < depth; ++k) {
                if (matrixChange[i][j][k] != DEAD) {
                    drawCube(i, j, k);
                }
            }
        }
    }
}

void drawFloor() {
    glBegin(GL_QUADS);

    GLfloat color[] = {0.65, 0.65, 0.65};

    const float height = -3.0;
    const float edge = 5;

    glColor3fv(color);
    glVertex3f(-edge, height, -edge);
    glVertex3f(-edge, height, depth+edge);
    glVertex3f(rows+edge, height, depth+edge);
    glVertex3f(rows+edge, height, -edge);

    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up camera
    gluLookAt(float(rows)/2 + cameraDistance * sin(cameraAngleY) * cos(cameraAngleX),
              float(columns)/2 + cameraDistance * sin(cameraAngleX),
              float(depth)/2 + cameraDistance * cos(cameraAngleY) * cos(cameraAngleX),
              float(rows)/2,
              float(columns)/2,
              float(depth)/2,
              0.0, 1.0, 0.0);

    drawFloor();
    drawMatrix();

    glFlush();
    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(width) / static_cast<double>(height), 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mousePrevX = x;
            mousePrevY = y;
            isMouseLeftButtonPressed = true;
        } else if (state == GLUT_UP) {
            isMouseLeftButtonPressed = false;
        }
    }
}

void motion(int x, int y) {
    if (isMouseLeftButtonPressed) {
        int deltaX = x - mousePrevX;
        int deltaY = y - mousePrevY;
        mousePrevX = x;
        mousePrevY = y;

        cameraAngleX += deltaY * 0.01;
        // cameraAngleY += deltaX * 0.01;

        glutPostRedisplay();
    }
}

void timer(int value) {
    // Get the elapsed time since the last frame
    static int previousTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    int deltaTime = currentTime - previousTime;

    previousTime = currentTime;

    elapsedTime += deltaTime;

    // Check if 2 seconds have passed
    if (elapsedTime >= iterationTime) {
        elapsedTime = 0; // Reset the elapsed time

        // Generate the next state and update the display
        generateNextMatrix();

    }

    cameraAngleY += cameraRotationSpeed;

    glutPostRedisplay();

    glutTimerFunc(16, timer, 0); // Continue the timer
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1200, 900);
    glutCreateWindow("Game of Life 3D");

    initializeMatrix();

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();

    return 0;
}
