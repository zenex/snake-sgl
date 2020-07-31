// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗   ██╗  ██╗██╗   ██╗███████╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝   ╚██╗██╔╝╚██╗ ██╔╝╚══███╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝     ╚███╔╝  ╚████╔╝   ███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗     ██╔██╗   ╚██╔╝   ███╔╝
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗██╗██╔╝ ██╗   ██║   ███████╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝
// Author:  AlexHG @ NEOHEX.XYZ
// License: MIT License
// Website: https://neohex.xyz

#include "grid.hpp"

void Grid::generateGrid()
{
    int horizontalLimit = pWidth / pTileSize, verticalLimit = pHeight / pTileSize;
    int tileNum = 1;
    for (int x = 0; x < horizontalLimit; ++x)
    {
        for (int y = 0; y < verticalLimit; ++y)
        {
            auto newTile = new Tile();
            newTile->sprite.position.x = (pTileSize * x);
            newTile->sprite.position.y = (pTileSize * y);
            newTile->sprite.size.x = pTileSize;
            newTile->sprite.size.y = pTileSize;
            newTile->sprite.color = pColor;
            newTile->id = tileNum;
            tileNum++;
            pTiles.push_back(std::move(newTile));
        }
    }
    SGL_Log("total tiles: " + std::to_string(pTiles.size()));
    std::cout << "H LIMITs: "  << horizontalLimit << "\n";
    std::cout << "V LIMITs: "  << verticalLimit << "\n";
}

Grid::~Grid()
{
    destroyTiles();
}

void Grid::destroyTiles()
{
    SGL_Log("Destroying game data...");
    for (std::vector<Tile *>::const_iterator iter = pTiles.begin(); iter != pTiles.end(); ++iter)
        delete (*iter);
    for (std::vector<Food *>::const_iterator iter = pFood.begin(); iter != pFood.end(); ++iter)
        delete (*iter);

    pTiles.clear();
    pFood.clear();
    pBody.clear();
    SGL_Log("Done.");
}



Grid::Grid(int x, int y, int w, int h, int size, const SGL_Color &color, SGL_Texture texture)
{
    // General data
    pPosX = x;
    pPosY = y;
    pWidth = (size * w);
    pHeight = (size * h);
    pTileSize = size;
    pTexture = texture;
    pTransitionSpeed = 0.0;
    pColor = color;
    pSpeed = 30;
    pSpeedCounter = 0;
    pTimeCounter = 0.0;
    pGameTime = 0.0;
    pXTiles = pWidth / pTileSize;
    pYTiles = pHeight / pTileSize;

    this->resetGame();


    pNextMove = DIRECTION::NO_DIRECTION;
}

void Grid::resetGame()
{
    // Destroy the current tiles before proceeding
    this->destroyTiles();

    int horizontalLimit = pWidth / pTileSize, verticalLimit = pHeight / pTileSize;
    int tileNum = 1;
    for (int x = 0; x < horizontalLimit; ++x)
    {
        for (int y = 0; y < verticalLimit; ++y)
        {
            auto newTile = new Tile();
            newTile->sprite.position.x = (pTileSize * pPosX);
            newTile->sprite.position.y = (pTileSize * pPosY);
            newTile->sprite.size.x = pTileSize;
            newTile->sprite.size.y = pTileSize;
            newTile->sprite.color = pColor;
            newTile->sprite.texture = pTexture;
            newTile->sprite.resetUVCoords();
            newTile->id = tileNum;
            tileNum++;
            pTiles.push_back(std::move(newTile));
        }
    }
    SGL_Log("total tiles: " + std::to_string(pTiles.size()));
    std::cout << "H LIMITs: "  << horizontalLimit << "\n";
    std::cout << "V LIMITs: "  << verticalLimit << "\n";


    // Player
    pHead.sprite.position.x = pPosX;
    pHead.sprite.position.y = pPosY;
    pHead.sprite.size.x = pTileSize;
    pHead.sprite.texture = pTexture;
    pHead.sprite.size.y = pTileSize;
    pHead.sprite.color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    pHead.sprite.changeUVCoords(128, 0, 16, 16);
    pHead.bodyPiece = 0;

    respawnFood(14);
    pNextMove = DIRECTION::NO_DIRECTION;
}

void Grid::moveSnake(DIRECTION dir)
{
    pNextMove = dir;
}

bool Grid::update()
{
    pSpeedCounter++;
    int dx = pHead.sprite.position.x;
    int dy = pHead.sprite.position.y;

    if (pSpeedCounter >= (60/pSpeed))
    {
        // Update the snake
        switch (pNextMove)
        {
        case DIRECTION::UP:
        {
            dy -= pTileSize;
            break;
        }
        case DIRECTION::DOWN:
        {
            dy += pTileSize;
            break;
        }
        case DIRECTION::LEFT:
        {
            dx -= pTileSize;
            break;
        }
        case DIRECTION::RIGHT:
        {
            dx += pTileSize;
            break;
        }
        default:
            break;
        }
        //pNextMove = DIRECTION::NO_DIRECTION;

        switch (checkForCollision(dx, dy))
        {
        case COLLISION::FOOD:
        {
            increaseScore();
            moveSnake(dx, dy);
            respawnFood(1);
            break;
        }
        case COLLISION::NO_COLLISION:
        {
            moveSnake(dx, dy);
            break;
        }
        case COLLISION::WALL:
        {
            SGL_Log("WALL COLLISION");
            break;
        }
        case COLLISION::SNAKE:
        {
            SGL_Log("BODY COLLISION");
            return false;
            break;
        }

        }

        pSpeedCounter = 0;
    }

    pTimeCounter = 0.0;
    //pGameTime = 0;
    return true;
}

bool Grid::renderTransition(SGL_Renderer &r, SGL_AssetManager &am)
{
    // Get the initial tile
    // int tileH = (pHeight / 2) * pTileSize;
    // int tileW = (pWidth / 2) * pTileSize;
    // auto newTile = new Tile();
    // newTile->position.x = (pWidth / 2) * pTileSize;
    // newTile->position.y = (pWidth / 2) * pTileSize;
    // newTile->position.w = pTileSize;
    // newTile->position.z = pTileSize;
    // newTile->color = pColor;
    // newTile->id = 0;
    // // Add it to the que
    // pTransitionTiles.push_back(std::move(newTile));


    // for (auto iter = pTiles.begin(); iter != pTiles.end() ; ++iter)
    // {
    //     (*iter)->sprite.shader = am.getShader("customSprite");
    //     r.renderSprite((*iter)->sprite);
    // }

    // for (int x = 0; x < pTransitionTileAnimation; ++x)
    // {
    //     pTiles[x]->sprite.shader = am.getShader("customSprite");
    //     r.renderSprite(pTiles[x]->sprite);

    // }

    pTiles[pTransitionTileAnimation]->sprite.shader = am.getShader("customSprite");
    r.renderSprite(pTiles[pTransitionTileAnimation]->sprite);

    pTiles[(pTiles.size() - 1) - pTransitionTileAnimation]->sprite.shader = am.getShader("customSprite");
    r.renderSprite(pTiles[(pTiles.size() -1) - pTransitionTileAnimation]->sprite);

    pTransitionTileAnimation++;
    if (pTransitionTileAnimation >= pTiles.size())
    {
        pTransitionTileAnimation = 0;
        return true;
    }
    else
        return false;
    // for (auto iter = pTiles.begin(); iter != pTiles.end() ; ++iter)
    // {
    //     // (*iter)->sprite.texture = am.getTexture("nuPepe");
    //     // (*iter)->sprite.resetUVCoords();
    //     (*iter)->sprite.shader = am.getShader("customSprite");
    //     r.renderSprite((*iter)->sprite);
    //     // SGL_Log("render tile X: " + std::to_string((*iter)->sprite.position.x));

    // }

    // Is the animation finished?
}

uint16_t Grid::getScore()
{
    return (static_cast<uint16_t>(this->pBody.size()));
}

bool Grid::update(int mouseX, int mouseY)
{
    for (std::vector<Tile *>::const_iterator iter = pTiles.begin(); iter != pTiles.end(); ++iter)
    {
        (*iter)->selected = false;
        if ( ( (*iter)->position.x) <= mouseX &&
             ( ( (*iter)->position.x + (*iter)->position.w) >= mouseX ) &&
             ( ( (*iter)->position.y) <= mouseY ) &&
             ( (*iter)->position.y + (*iter)->position.z) >= mouseY )
        {
            (*iter)->selected = true;
            pSelectedTileIndex = (iter - pTiles.begin());
        }

    }

    int dx = pHead.sprite.position.x;
    int dy = pHead.sprite.position.y;
    // Update the snake
    switch (pNextMove)
    {
    case DIRECTION::UP:
    {
        dy -= pTileSize;
        break;
    }
    case DIRECTION::DOWN:
    {
        dy += pTileSize;
        break;
    }
    case DIRECTION::LEFT:
    {
        dx -= pTileSize;
        break;
    }
    case DIRECTION::RIGHT:
    {
        dx += pTileSize;
        break;
    }
    default:
        break;
    }
    //pNextMove = DIRECTION::NO_DIRECTION;

    switch (checkForCollision(dx, dy))
    {
    case COLLISION::NO_COLLISION:
    {
        SGL_Log("NO COLLISION");
        pHead.sprite.position.x = dx;
        pHead.sprite.position.y = dy;
        break;
    }
    case COLLISION::WALL:
    {
        SGL_Log("WALL COLLISION");
        break;
    }
    case COLLISION::SNAKE:
    {
        SGL_Log("BODY COLLISION");
        return false;
        break;
    }
    case COLLISION::FOOD:
    {
        increaseScore();
        respawnFood(1);
        break;
    }
    }

    return true;
}

void Grid::moveSnake(int dx, int dy)
{
    int previousPosX = pHead.sprite.position.x;
    int previousPosY = pHead.sprite.position.y;
    int tmpX, tmpY;
    // Update the snake's head
    pHead.sprite.position.x = dx;
    pHead.sprite.position.y = dy;

    for (auto iter = pBody.begin(); iter != pBody.end() ; ++iter)
    {
        tmpX = (*iter).sprite.position.x;
        tmpY = (*iter).sprite.position.y;

        (*iter).sprite.position.x = previousPosX;
        (*iter).sprite.position.y = previousPosY;

        previousPosX = tmpX;
        previousPosY = tmpY;
    }
}

void Grid::increaseScore()
{
    Snake newBodyPart;
    if (pBody.size() == 0)
        newBodyPart = pHead;
    else
        newBodyPart = pBody.back();

    newBodyPart.bodyPiece++;
    newBodyPart.x -= pTileSize;
    newBodyPart.sprite.color = {static_cast<float>(rand() / (RAND_MAX + 1.0)), static_cast<float>(rand() / (RAND_MAX + 1.0)), static_cast<float>(rand() / (RAND_MAX + 1.0)), 1.0f};
    pBody.push_back(newBodyPart);
    SGL_Log("score increased: " + std::to_string(pBody.size()));
    SGL_Log("color R: " + std::to_string(newBodyPart.sprite.color.r));
}

COLLISION Grid::checkForCollision(int dx, int dy)
{
    // Check if the upcoming position collides with any solid object
    // Against walls
    if (dx < pPosX)
        SGL_Log("COLLISION LEFT");
    if ((dx + pTileSize) > (pWidth + pPosX))
        SGL_Log("COLLISION RIGHT");
    if (dy < pPosY)
        SGL_Log("COLLISION DOWN");
    if (((dy + pTileSize) > (pHeight + pPosY)))
        SGL_Log("COLLISION UP");


    if (dx < pPosX || (dx + pTileSize) > (pWidth + pPosX) ||
        dy < pPosY || (dy + pTileSize) > (pHeight + pPosY))
        return COLLISION::WALL;

    // Against a body part
    std::list<Snake>::iterator iter;
    for (iter = pBody.begin(); iter != pBody.end();)
    {
        if (dx == iter->sprite.position.x &&
            dy == iter->sprite.position.y)
        {
            if ((*iter).bodyPiece == 1) // Neck collision, continue forward
                return COLLISION::NECK;

            SGL_Log("Collided with snake body part #" + std::to_string((*iter).bodyPiece));
            return COLLISION::SNAKE;
        }
        else
            ++iter;
    }


    // Food
    for (auto iter = pFood.begin(); iter != pFood.end();)
    {
        if (dx == (*iter)->sprite.position.x &&
            dy == (*iter)->sprite.position.y)
        {
            delete (*iter);
            iter = pFood.erase(iter);
            return COLLISION::FOOD;
        }
        else
            ++iter;
    }

    return COLLISION::NO_COLLISION;
}

void Grid::respawnFood(uint8_t amount)
{
    SGL_Log("Respawning food");

    std::mt19937 mt(pRnd());
    std::uniform_int_distribution<int> distX(0, pXTiles);
    std::uniform_int_distribution<int> distY(0, pYTiles);
    // uint16_t respawnX = distX(mt);
    // uint16_t respawnY = distY(mt);

    // SGL_Log("X RAND: " + std::to_string(respawnX));
    // SGL_Log("Y RAND: " + std::to_string(respawnY));


    // std::vector<vec2d> occupiedTiles;
    // // Get a list of occupied tiles
    // for (auto iter = pBody.begin(); iter != pBody.end(); iter++)
    // {
    //     vec2d tilePos;
    //     tilePos.x = ((*iter).sprite.position.x / pTileSize);
    //     tilePos.y = ((*iter).sprite.position.y / pTileSize);
    //     occupiedTiles.push_back(tilePos);
    //     SGL_Log("tilePosX: " + std::to_string(tilePos.x));
    //     SGL_Log("tilePosY: " + std::to_string(tilePos.y));
    // }
    // SGL_Log("total occupied tiles: " + std::to_string(occupiedTiles.size()));

    // for (int i=0; i<16; ++i)
    //     std::cout << "RANDOM: " << pDist(mt) << "\n";


    for (int i = 0; i < amount; ++i)
    {
        // Randomize a new position
        uint16_t respawnX = distX(mt);
        uint16_t respawnY = distY(mt);

        SGL_Log("X: " + std::to_string(respawnX) + " Y: " + std::to_string(respawnY));

        if (respawnX >= pXTiles)
        {
            SGL_Log("Food spawned on horizontal limit, fixing");
            respawnX = pXTiles - 1;
        }
        if (respawnY >= pYTiles)
        {
            SGL_Log("Food spawned on vertical limit, fixing");
            respawnY = pYTiles - 1;
        }

        if (checkForCollision(respawnX, respawnY) != COLLISION::NO_COLLISION)
        {
            SGL_Log("Food respawn failed, occupied tile. X: " + std::to_string(respawnX) + " Y: " + std::to_string(respawnY));
            return;
        }

        Food *testFood = new Food();
        testFood->sprite.position.x = (respawnX * pTileSize);
        testFood->sprite.position.y = (respawnY * pTileSize);
        testFood->sprite.size.x = pTileSize;
        testFood->sprite.size.y = pTileSize;
        testFood->sprite.color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
        testFood->sprite.texture = pTexture;

        switch(static_cast<uint8_t>(rand() % 6))
        {
        case 0:
        {
            // apple
            testFood->sprite.changeUVCoords(144, 32, 16, 16);
            break;
        }
        case 1:
        {
            // apple
            testFood->sprite.changeUVCoords(144, 0, 16, 16);
            break;
        }

        case 2:
        {
            // cucumber
            testFood->sprite.changeUVCoords(160, 0, 16, 16);
            break;
        }
        case 3:
        {
            // cyanberries
            testFood->sprite.changeUVCoords(176, 0, 16, 16);
            break;
        }
        case 4:
        {
            // leafberry
            testFood->sprite.changeUVCoords(192, 0, 16, 16);
            break;
        }
        case 5:
        {
            // black egg
            testFood->sprite.changeUVCoords(144, 16, 16, 16);
            break;
        }
        case 6:
        {
            // growing potato
            testFood->sprite.changeUVCoords(160, 16, 16, 16);
            break;
        }
        }

        this->pFood.push_back(std::move(testFood));
    }

}


void Grid::render(SGL_Renderer &r, SGL_AssetManager &am)

{

    pHead.sprite.shader = am.getShader("customSprite");
    for (auto iter = pFood.begin(); iter != pFood.end(); iter++)
    {
        (*iter)->sprite.shader = am.getShader("customSprite");
        r.renderSprite((*iter)->sprite);
    }
    for (auto iter = pBody.begin(); iter != pBody.end() ; ++iter)
    {
        r.renderSprite((*iter).sprite);
    }
    r.renderSprite(pHead.sprite);

}

void Grid::setColor(SGL_Color &color)
{
    pColor = color;
}

uint16_t Grid::selectedTile()
{
    return pSelectedTileIndex;
}
