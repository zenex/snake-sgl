// Author:  AlexHG @ ZEN3X.COM
// License: MIT License
// Website: https://ZEN3X.COM

#ifndef GRID_HPP
#define GRID_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <random>
#include <cstdint>
#include <list>
#include "skeletonGL/skeletonGL.hpp"

enum class DIRECTION : std::int8_t {UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3, NO_DIRECTION = 4, END_ENUM = 5};
enum class COLLISION : std::int8_t {NO_COLLISION = 0, WALL = 1, SNAKE = 2, FOOD = 3, NECK = 4, END_ENUM = 5};


struct vec2d
{
    int x, y;
};

struct Tile
{
    glm::vec4 position;
    SGL_Color color;
    bool selected;
    int id;
    SGL_Sprite sprite;

    Tile() : position(0.0f), selected(false), id(0) {}
};

struct Snake
{
    int x, y;
    uint8_t bodyPiece;
    SGL_Sprite sprite;
};

struct Food
{
    int x, y;
    SGL_Sprite sprite;
    bool active;
};

class Grid
{
private:
    int pWidth, pHeight, pTileSize;
    uint8_t pXTiles, pYTiles; // Might as well have save these two
    int pPosX, pPosY;
    SGL_Color pColor;
    SGL_Texture pTexture;
    DIRECTION pNextMove;
    std::uint8_t pSpeed, pSpeedCounter; // in milliseconds
    float pTimeCounter, pGameTime; // keep track of when its time to move
    // Player (snake's head)
    Snake pHead;
    std::list<Snake> pBody;
    std::vector<Tile *> pTiles;
    std::vector<Food *> pFood;
    std::vector<Tile *> pTransitionTiles;
    uint16_t pTransitionTileAnimation = 0;
    float pTransitionSpeed;
    uint16_t pSelectedTileIndex = 0;

    // Random number generator (used by respawnFood() function)
    std::random_device pRnd;
    std::mt19937 pMt;
    // std::uniform_int_distribution<> pDist;


    // Game status
    bool pAlive = true;

    void generateGrid();
    COLLISION checkForCollision(int dx, int dy);
    void increaseScore();
    void moveSnake(int dx, int dy);

public:
    Grid(int x, int y, int w, int h, int size, const SGL_Color &color, SGL_Texture texture);
    ~Grid();

    bool renderTransition(SGL_Renderer &r, SGL_AssetManager &am);

    void setPlayerTexture(SGL_Texture texture);

    bool update();
    bool update(int mouseX, int mouseY);
    void render(SGL_Renderer &r, SGL_AssetManager &am);
    void setColor(SGL_Color &color);

    void resetGame();
    void respawnFood(uint8_t amount);

    // Player
    void moveSnake(DIRECTION dir);

    bool checkTileAvailability();

    uint16_t getScore();
    void destroyTiles();
    uint16_t selectedTile();
};

#endif
