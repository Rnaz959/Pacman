#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <cmath>

class GameObject {
protected:
    float x, y; // Position of objects
public:
    GameObject(float x, float y) : x(x), y(y) {}
    virtual ~GameObject() {}
    virtual void draw() = 0;    //creating object
    virtual void update() = 0;  //maintaing object
    float getX() const { return x; }
    float getY() const { return y; }
    virtual float getRadius() const = 0;
};

class Pacman : public GameObject {
private:
    float radius;
    float speed;
    float directionX;
    float directionY;
public:
    Pacman(float x, float y) : GameObject(x, y), radius(0.04f), speed(0.01f), directionX(0), directionY(0) {}
    void draw() override {
        glColor3f(1.0f, 1.0f, 0.0f); // Draws pacman as yellow circle
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= 360; i++) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
        }
        glEnd();
    }
    void update() override {
        x += directionX * speed;
        y += directionY * speed;
    }
    void setDirection(float dx, float dy) {
        directionX = dx;
        directionY = dy;
    }
    float getRadius() const override {
        return radius;
    }
};

class Ghost : public GameObject {
private:
    Pacman* target;
    float speed;
public:
    Ghost(float x, float y, Pacman* target) : GameObject(x, y), target(target), speed(0.008f) {}
    void draw() override {
        glColor3f(1.0f, 0.0f, 1.0f); // Draws ghost as purple square
        glBegin(GL_QUADS);
        glVertex2f(x - 0.03f, y - 0.03f);
        glVertex2f(x + 0.03f, y - 0.03f);
        glVertex2f(x + 0.03f, y + 0.03f);
        glVertex2f(x - 0.03f, y + 0.03f);
        glEnd();
    }
    void update() override {
        float dx = target->getX() - x;                  // Calculate direction towards the target (Pacman)
        float dy = target->getY() - y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance != 0) {
            dx /= distance;
            dy /= distance;
        }

        if (fabs(dx) > fabs(dy)) {              // Move towards the target only along the x and y axes
            x += (dx > 0) ? speed : -speed;
        }
        else {
            y += (dy > 0) ? speed : -speed;
        }
    }
    float getRadius() const override {
        return 0.03f; // Adjust the radius for collision detection
    }
};


class Pellet : public GameObject {
private:
    float radius;
public:
    Pellet(float x, float y) : GameObject(x, y), radius(0.02f) {}
    void draw() override {
        glColor3f(1.0f, 1.0f, 1.0f); // Create pellet
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; ++i) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
        }
        glEnd();
    }
    void update() override {} // Pellets don't move
    float getRadius() const override {
        return radius;
    }
};

/*
    ***Tried putting everything into a maze but could not get it to work in time
 
class Maze {
private:
    std::vector<std::pair<float, float>> walls;
public:
    Maze() {

        walls.push_back({ -0.6f, 0.6f });
        walls.push_back({ -0.6f, -0.6f });         // Add maze walls
        walls.push_back({ 0.6f, 0.6f });
        walls.push_back({ 0.6f, -0.6f });
    }
    void draw() {
        glColor3f(1.0f, 1.0f, 1.0f); // White color for walls
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        for (const auto& wall : walls) {
            glVertex2f(wall.first, 1.0f);
            glVertex2f(wall.first, -1.0f);
            glVertex2f(wall.second, 1.0f);
            glVertex2f(wall.second, -1.0f);
        }
        glEnd();
    }
    bool isColliding(float x, float y, float radius) {
        for (const auto& wall : walls) {
            if (x + radius > wall.first && x - radius < wall.second)
                return true;
        }
        return false;
    }
};
*/
class Game {
private:
    std::vector<GameObject*> gameObjects;
    int score;
  //  Maze maze;
public:
    Game() : score(0) {}
    ~Game() {
        for (auto obj : gameObjects) {
            delete obj;
        }
    }
    void addGameObject(GameObject* obj) {
        gameObjects.push_back(obj);
    }
    void update() {
        for (size_t i = 0; i < gameObjects.size(); ++i) {         // Check for collisions
            for (size_t j = i + 1; j < gameObjects.size(); ++j) {
                GameObject* obj1 = gameObjects[i];
                GameObject* obj2 = gameObjects[j];

                if (checkCollision(obj1, obj2)) {    
                    if (dynamic_cast<Pacman*>(obj1) && dynamic_cast<Pellet*>(obj2)) {   //pellets collide
                        gameObjects.erase(gameObjects.begin() + j); // Remove pellet
                        score++;
                        std::cout << "Score: " << score << std::endl;
                    }
                    else if ((dynamic_cast<Pacman*>(obj1) && dynamic_cast<Ghost*>(obj2)) ||
                        (dynamic_cast<Pacman*>(obj2) && dynamic_cast<Ghost*>(obj1))) { //ghost collide
                        std::cout << "Game Over!\n";
                        exit(EXIT_SUCCESS); // Game over
                    }
                }
            }
        }

        for (auto obj : gameObjects) {          // Update all game objects
            obj->update();
        }
    }
    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       // maze.draw(); // Draw maze
        for (auto obj : gameObjects) { //Draw the objects pacman, pellets, ghost
            obj->draw();
        }
        glutSwapBuffers();
    }
    bool checkCollision(GameObject* obj1, GameObject* obj2) {  //Checks for if objects collide
        float dx = obj1->getX() - obj2->getX();
        float dy = obj1->getY() - obj2->getY();
        float distance = sqrt(dx * dx + dy * dy);
        return distance < obj1->getRadius() + obj2->getRadius();
    }
    std::vector<GameObject*>& getGameObjects() {
        return gameObjects;
    }
};

Game game;

void idle_func() {
    game.update();
    glutPostRedisplay();
}

void display_func() {
    game.draw();
}

void keyboard_func(unsigned char key, int x, int y) {
    Pacman* pacman = dynamic_cast<Pacman*>(game.getGameObjects()[0]); // Assuming Pacman is the first object
    if (!pacman)
        return;

    switch (key) {
    case 'w':
        pacman->setDirection(0, 1); // Move up
        break;
    case 'a':
        pacman->setDirection(-1, 0); // Move left
        break;
    case 's':
        pacman->setDirection(0, -1); // Move down
        break;
    case 'd':
        pacman->setDirection(1, 0); // Move right
        break;
    default:
        break;
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("Pacman");

    init();

    Pacman* pacman = new Pacman(0.0f, 0.0f);
    game.addGameObject(pacman); // Adding Pacman and Ghost to the game

    Ghost* ghost = new Ghost(0.5f, 0.5f, pacman);
    game.addGameObject(ghost);  


    for (int i = -3; i <= 3; ++i) {         //Adds pellets
        for (int j = -3; j <= 3; ++j) {
            Pellet* pellet = new Pellet(i * 0.2f, j * 0.2f);
            if (!game.checkCollision(pellet, pacman)) // Avoid placing pellet on Pacman
                game.addGameObject(pellet);
            else
                delete pellet; // Delete the pellet if it's colliding with Pacman
        }
    }

    glutDisplayFunc(display_func);
    glutIdleFunc(idle_func);
    glutKeyboardFunc(keyboard_func); // Register keyboard input

    glutMainLoop();
    return EXIT_SUCCESS;
}
