#include <iostream>
#include <GL/glut.h>
#include <cmath>

//Propriedades da matriz
const int rows = 10;
const int columns = 10;
const int depth = 10;
bool matrix[rows][columns][depth]{};
int matrixChange[rows][columns][depth]{0};

// Posição de câmera
float cameraRotationSpeed = 0.005f;
float cameraAngleX = 0.3;
float cameraAngleY = 3.5;
const float cameraDistance = std::max(rows, depth) + std::max(rows, depth)*1.3;

//Tempo
float totalTime = 0;
float elapsedTime = 0;
const float iterationTime = 6000;

int seed = 237382;
float prob = 0.2;

int ALIVE = 3;
int BORNING = 2;
int DYING = 1;
int DEAD = 0;



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

bool willLive(int x, int y, int z) {
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

void copyArray() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            for (int k = 0; k < depth; ++k) {
                if (matrixChange[i][j][k] == DYING || matrixChange[i][j][k] == BORNING) {
                    matrix[i][j][k] = !matrix[i][j][k];
                }
            }
        }
    }
}

void generateNextMatrix() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            for (int k = 0; k < depth; ++k) {
                bool alive = willLive(i, j, k);

                if (matrix[i][j][k] && !alive) {
                    matrixChange[i][j][k] = DYING;
                } else if (!matrix[i][j][k] && alive) {
                    matrixChange[i][j][k] = BORNING;
                } else if (matrix[i][j][k] && alive) {
                    matrixChange[i][j][k] = ALIVE;
                } else {
                    matrixChange[i][j][k] = DEAD;
                }
            }
        }
    }

    copyArray();
}

void drawCube( int type, GLfloat color[3], float size) {
    // Definição dos vértices do cubo
    GLfloat vertices[][3] = {
        {-size / 2, -size / 2, -size / 2},
        {size / 2, -size / 2, -size / 2},
        {size / 2, size / 2, -size / 2},
        {-size / 2, size / 2, -size / 2},
        {-size / 2, -size / 2, size / 2},
        {size / 2, -size / 2, size / 2},
        {size / 2, size / 2, size / 2},
        {-size / 2, size / 2, size / 2}
    };

    // Definição dos índices de vértices das faces
    GLuint indices[][4] = {
        {0, 1, 2, 3},
        {1, 5, 6, 2},
        {5, 4, 7, 6},
        {4, 0, 3, 7},
        {4, 5, 1, 0},
        {3, 2, 6, 7}
    };

    // Definição das normais para iluminação
    GLfloat normals[][3] = {
        {0, 0, -1},
        {1, 0, 0},
        {0, 0, 1},
        {-1, 0, 0},
        {0, -1, 0},
        {0, 1, 0}
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glNormalPointer(GL_FLOAT, 0, normals);

    // Define a cor do cubo
    glColor3fv(color);

    // Desenha cada face do cubo
    for (int i = 0; i < 6; i++) {
        glDrawElements(type, 4, GL_UNSIGNED_INT, indices[i]);
    }

    // Desabilita o uso de arrays de vértices e normais
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}


void drawCubeAnimation(int x, int y, int z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    float cubeSizeMultiplier = 1.0f;
    GLfloat color[] = {0.85, 0.85, 0.85};

    const float animationSlice = 0.8; // Parcela da iteração com animação
    const float scaleRate = elapsedTime / (iterationTime*animationSlice); // Velocidade da animação
    const float colorRateRed = elapsedTime / (iterationTime*animationSlice/4); // Velocidade mudança de cor vermelha
    const float degradeGreenThreshold = 0.5;
    float colorRateGreen = 0;

    if (elapsedTime > iterationTime*0.5) { // Inicia a degradação da cor verde
        colorRateGreen = // Velocidade mudança de cor
                (elapsedTime - iterationTime*degradeGreenThreshold)
                / (iterationTime*animationSlice - iterationTime*degradeGreenThreshold);
    }

    if( elapsedTime / iterationTime < animationSlice) {
        if(matrixChange[x][y][z] == BORNING) { // Animação e coloração de células nascendo
            color[0] = 0.8*colorRateGreen;
            color[1] = 0.8;
            color[2] = 0.8*colorRateGreen;
            cubeSizeMultiplier = scaleRate;
        } else if (matrixChange[x][y][z] == DYING) { // Animação e coloração de células morrendo
            color[0] = 0.8;
            color[1] = 0.8 - 0.8*colorRateRed;
            color[2] = 0.8 - 0.8*colorRateRed;
            cubeSizeMultiplier = 1 - scaleRate;
        }
    } else if (matrixChange[x][y][z] == DYING) { // Pula células mortas
        glPopMatrix();
        return;
    }

    // Desenha o contorno
    GLfloat grey[] = {0.3, 0.3, 0.3};
    drawCube(GL_LINE_LOOP, grey, 0.5*cubeSizeMultiplier);

    // Desenha os cubos
    drawCube(GL_QUADS, color, 0.5*cubeSizeMultiplier);

    glPopMatrix();
}

void drawMatrix() {
   for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            for (int k = 0; k < depth; ++k) {
                if (matrixChange[i][j][k] != DEAD) {
                    drawCubeAnimation(i, j, k);
                }
            }
        }
    }
}

void setupLighting() {
    const GLfloat lightPosition[] = { 1.0, 1.0, 1.0, 0.0 };  // Posição da fonte de luz
    const GLfloat lightAmbient[] = { 0.2, 0.2, 0.2, 1.0 };   // Cor ambiente da luz
    const GLfloat lightDiffuse[] = { 0.8, 0.8, 0.8, 1.0 };   // Cor difusa da luz
    const GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };  // Cor especular da luz
    const GLfloat lightDirection[] = {columns/2, rows/2, depth/2};  // Direção do feixe de luz

    const GLfloat matAmbient[] = { 0.8, 0.8, 0.8, 1.0 };     // Cor ambiente do material
    const GLfloat matDiffuse[] = { 0.8, 0.8, 0.8, 1.0 };     // Cor difusa do material
    const GLfloat matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };    // Cor especular do material
    const GLfloat matShininess[] = { 50.0 };                 // Rugosidade do material

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_SPECULAR);

    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_COLOR_MATERIAL);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Reposiciona a câmera
    gluLookAt(float(rows)/2 + cameraDistance * sin(cameraAngleY) * cos(cameraAngleX),
              float(columns)/2 + cameraDistance * sin(cameraAngleX),
              float(depth)/2 + cameraDistance * cos(cameraAngleY) * cos(cameraAngleX),
              float(rows)/2,
              float(columns)/2,
              float(depth)/2,
              0.0, 1.0, 0.0);

    drawMatrix(); // Desenha a matriz
    setupLighting(); // Configura iluminação

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

void timer(int value) {
    static int previousTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    int deltaTime = currentTime - previousTime;

    previousTime = currentTime;

    totalTime += deltaTime;
    elapsedTime += deltaTime;

    if (elapsedTime >= iterationTime) {
        elapsedTime = 0; // Inicia nova iteração

        generateNextMatrix(); // Gera matriz da próxima iteração
    }

    // Atualiza os ângulos das câmeras
    cameraAngleY += cameraRotationSpeed;
    cameraAngleX = sin(totalTime/(iterationTime/2))/3;

    glutPostRedisplay();

    glutTimerFunc(16, timer, 0);
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(2560, 1440 );
    glutCreateWindow("Game of Life 3D");

    initializeMatrix();

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();

    return 0;
}
