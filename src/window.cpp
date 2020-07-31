// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗   ██╗  ██╗██╗   ██╗███████╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝   ╚██╗██╔╝╚██╗ ██╔╝╚══███╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝     ╚███╔╝  ╚████╔╝   ███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗     ██╔██╗   ╚██╔╝   ███╔╝
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗██╗██╔╝ ██╗   ██║   ███████╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝
// Author:  AlexHG @ NEOHEX.XYZ
// License: MIT License
// Website: https://neohex.xyz

#include "window.hpp"

Window::Window()
{
    this->pWindowManager = new SGL_Window();
    //seed RNG
    srand(time(NULL));
}

Window::~Window()
{
    delete pWindowManager;
}

int Window::createSGLWindow()
{
    SGL_Log("Initializing...");
    SGL_Log("Creating window...");
    try
    {
        //this->pWindowManager->initializeWindow(0, 0, 1024, 576, 480, 270, std::string("With_his_jimmy "), false, std::string("skeletongl.ini"));
        this->pWindowManager->initializeWindow(0, 0, 1280, 720, 640, 352, std::string("RISK VECTOR"), false, std::string("skeletongl.ini"));
    }
    catch (SGL_Exception &e)
    {
        SGL_Log("Aborting window creating procedure.");
        SGL_Log(e.what());
        assert(false);
        return -1;
    }
    SGL_Log("Window ready.");
    return 0;
}

void Window::mainLoop()
{
    pWindowManager->setLogLevel(LOG_LEVEL::SGL_DEBUG);
    SGL_Log("Loading the game...", LOG_LEVEL::DEBUG);
    pWindowManager->checkForErrors();
    this->loadAssets();
    pGameState = GAME_STATE::INTRO_SEQUENCE;
    pMainLoopActive = true;
    pDrawDebugPanel = false;
    pCameraZoom = 1.0f;
    pMenuCursor = 0;

    // Splash screen
    pSplashScreenAlpha = 1.0f;
    pSplashScreenControl = 1;


   // Frame counter
    pFPSCounter = 60;
    pCurrentFrame = 1;
    pAvgDT = 0.016;
    pFrameDTCounter = 0.0f;
    pTicksPerFrame = 0;
    pUpdateFrame = false;

    // Intro sequence

    // Get the current (external / window) resolution
    if (this->pWindowManager->getWindowCreationSpecs().fullScreen)
        pCurrentResolution = MENU_SETTINGS_RESOLUTION_SELECTION::RES_FULLSCREEN;
    else
    {
        switch (this->pWindowManager->getWindowCreationSpecs().currentH)
        {
        case 360:
            pCurrentResolution = MENU_SETTINGS_RESOLUTION_SELECTION::RES_360P; break;
        case 720:
            pCurrentResolution = MENU_SETTINGS_RESOLUTION_SELECTION::RES_720P; break;
        default:
        {
            SGL_Log("Requested height: " + std::to_string(this->pWindowManager->getWindowCreationSpecs().currentH));
            SGL_Log("Unsupported resolution selected, changing to 360P.");
            pCurrentResolution = MENU_SETTINGS_RESOLUTION_SELECTION::RES_360P;
            break;
        }

        }
    }

    pVSYNCStatus = pWindowManager->getWindowCreationSpecs().activeVSYNC;
    pShowFPSSTatus = false;

    WindowCreationSpecs windowSpecs = pWindowManager->getWindowCreationSpecs();

    // Load UI
    pUISprite = std::make_shared<SGL_Sprite>();
    pUISprite->shader = pWindowManager->assetManager->getShader("customSprite");
    pUISprite->position = glm::vec2(150, 32);
    pUISprite->size = glm::vec2(304, 126);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    pUISprite->texture = pWindowManager->assetManager->getTexture("risk_vector");
    pUISprite->changeUVCoords(336, 0, 304, 116);

    // Intro sequence BG
    pIntroSequenceBG = std::make_shared<SGL_Sprite>();
    pIntroSequenceBG->position = glm::vec2(0, 0);
    pIntroSequenceBG->texture = pWindowManager->assetManager->getTexture("splashScreen");
    pIntroSequenceBG->size = glm::vec2(windowSpecs.internalW, (windowSpecs.internalH));
    pIntroSequenceBG->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    pIntroSequenceBG->blending = BLENDING_TYPE::DEFAULT_RENDERING;
    pIntroSequenceBG->shader = pWindowManager->assetManager->getShader("spriteUV");
    pIntroSequenceBG->resetUVCoords();

    // Debug panel background image
    pDebugPanelBackground = std::make_shared<SGL_Sprite>();
    pDebugPanelBackground->position = glm::vec2(0, 0);
    pDebugPanelBackground->texture = pWindowManager->assetManager->getTexture("box");
    pDebugPanelBackground->size = glm::vec2(windowSpecs.internalW, (windowSpecs.internalH));
    pDebugPanelBackground->color = SGL_Color(0.0f, 0.0f, 0.0f, 0.70f);
    pDebugPanelBackground->blending = BLENDING_TYPE::DEFAULT_RENDERING;
    pDebugPanelBackground->shader = pWindowManager->assetManager->getShader("spriteUV");
    pDebugPanelBackground->resetUVCoords();

    uint8_t tileSize = 16;
    pGameGrid = std::make_shared<Grid>(0, 0, (pWindowManager->getWindowCreationSpecs().internalW / tileSize), (pWindowManager->getWindowCreationSpecs().internalH / tileSize), tileSize, SGL_Color(1.0, 1.0, 1.0, 1.0), pWindowManager->assetManager->getTexture("risk_vector"));


    //random number generator (thanks C++11)
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, 1.0);


    // FIXED TIME STEP
    pFixedTimeStepMS = 1.0f / 60.0f;
    pMaxUpdatesPerFrame = 5;
    pUpdateCycles = 0;
    pChronoStart = std::chrono::steady_clock::now();
    pChronoEnd = pChronoStart;
    // TIME ELAPSED & FRAME META DATA
    pTimeElapsed = 0;
    pDeltaTimeElapsed = 0;
    pFPS = 0;
    pFrameCounter = 0;

    // Must be called before entering the game loop to properly initialize
    // the post processor's FBO
    this->pWindowManager->setClearColor(SGL_Color(0.1f, 0.1f, 0.1f, 1.0f));
    while (pMainLoopActive)
    {
        // Time since epoch
        pChronoStart = std::chrono::steady_clock::now();
        auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(pChronoStart - pChronoEnd);
        pTimeElapsed += timeElapsed.count();

        // Delta time
        auto timeDiff = std::chrono::duration_cast<std::chrono::nanoseconds>(pChronoStart - pChronoEnd);
        pDeltaTimeMS = timeDiff.count();
        pDeltaTimeMS /= 1000000000;

        // Collect frame meta data?
        pFrameCounter++;
        if ( ( (pTimeElapsed / 1000) - pDeltaTimeElapsed) >= 1.0) // A second has passed, collect meta data
        {
            pFPS = pFrameCounter;
            pFrameCounter = 0;
            pDeltaTimeElapsed = pTimeElapsed / 1000;
        }

        // START COUNTING THE FRAME TIME
        pChronoEnd = pChronoStart;

        // FIXED TIME STEP
        pDTAccumulator += pDeltaTimeMS;
        // Total number of updates to perform this frame
        pUpdateCycles = static_cast<int>(std::floor(pDTAccumulator / pFixedTimeStepMS));

        // Consume the cycles from the time accumulator
        if (pUpdateCycles > 0)
            pDTAccumulator -= pUpdateCycles * pFixedTimeStepMS;

        // Just crash it at this point
        assert("Accumulator must have a value lower than the fixes time step" && pDTAccumulator < pFixedTimeStepMS + FLT_EPSILON);

        // Should the program lag behind its predefined time step, force it to recover a max amount
        // of times each frame to prevent a sprial-of-death scenario
        pUpdateCycles = std::min(pUpdateCycles, pMaxUpdatesPerFrame);


        GAME_STATE currentFrameState = pGameState; // Don't pass pGameState directly, it might change mid frame

        this->input(currentFrameState);
        while ( pUpdateCycles > 0)
        {
            this->update(currentFrameState);
            pUpdateCycles--;
        }
        // Must be called as soon as the render frame starts to properly calculate delta time values and initiate the postprocessor FBO
        this->pWindowManager->startFrame();
        this->render(currentFrameState);
        this->pWindowManager->endFrame();

        this->pWindowManager->checkForErrors();
    }
    SGL_Log("Exiting main loop");
}

void Window::update(GAME_STATE gameState)
{
    // Fade the press enter button
    if (gameState == GAME_STATE::INTRO_SEQUENCE)
    {
        pSplashScreenAlpha += (pDeltaTimeMS * pSplashScreenControl);
        if (pSplashScreenAlpha < 0.0)
            pSplashScreenControl = 1;
        if (pSplashScreenAlpha > 1.0)
            pSplashScreenControl = -1;
    }

    if (gameState == GAME_STATE::IN_GAME)
    {
        // pm->update(pDeltaTimeMS, player1->getHitBox().quadX, player1->getHitBox().quadY, player1->getSprite().rotation, 1);
        // pPM->update(pDeltaTimeMS);
        if (!pGameGrid->update())
        {
            pGameState = GAME_STATE::GAME_OVER;
        }

    }

    // ------------- //
    // UPDATE CAMERA //
    // ------------- //
    //position the center of the camera on the center of the FBO texture (which has a size of internalW x interalH)
    this->pWindowManager->setCameraPosition(glm::vec2(pWindowManager->getWindowCreationSpecs().internalW / 2, pWindowManager->getWindowCreationSpecs().internalH / 2));
    // this->pWindowManager->cameraPosition(pPlayerOne->center());
    //zoom
    this->pWindowManager->setCameraScale(pCameraZoom);
    //update the internal offset matrix for rendering
    this->pWindowManager->updateCamera();

    // ---------------------- //
    // POSTPROCESSOR EFFECTS  //
    // ---------------------- //
    if (pShakeTime > 0.0f)
    {
        this->pWindowManager->setShakeScreen(true);
        this->pWindowManager->setChaosEffect(true);
        this->pWindowManager->setConfuseEffect(true);

        pShakeTime -= pDeltaTimeMS;
        if (pShakeTime <= 0.0f)
        {
            this->pWindowManager->setShakeScreen(false);
            this->pWindowManager->setChaosEffect(false);
            this->pWindowManager->setConfuseEffect(false);
        }
    }

}


void Window::input(GAME_STATE gameState)
{
    SGL_InputFrame desiredKeys = this->pWindowManager->getFrameInput();

    switch (gameState)
    {
    case GAME_STATE::INTRO_SEQUENCE:
    {
        if (desiredKeys.enter && !pDeltaInput.enter)
        {
            pGameState = GAME_STATE::START_SCREEN_MENU;
        }
        if (desiredKeys.esc && !pDeltaInput.esc)
            pMainLoopActive = false;
        break;
    }
    case GAME_STATE::START_SCREEN_MENU:
    {
        if (desiredKeys.esc && !pDeltaInput.esc)
            pMainLoopActive = false;

        if (desiredKeys.down && !pDeltaInput.down)
        {
            pMenuCursor++;
            if (pMenuCursor >= static_cast<int8_t>(MENU_START_SCREEN_SELECTION::END_ENUM))
                pMenuCursor = 0;

        }
        if (desiredKeys.up && !pDeltaInput.up)
        {
            pMenuCursor--;
            if (pMenuCursor < 0)
                pMenuCursor = (static_cast<int8_t>(MENU_START_SCREEN_SELECTION::END_ENUM) - 1);

        }
        if (desiredKeys.enter && !pDeltaInput.enter)
        {
            this->processMenu(GAME_STATE::START_SCREEN_MENU, pMenuCursor);
        }
        break;
    }
    case GAME_STATE::SETTINGS_MENU:
    {
        if (desiredKeys.esc && !pDeltaInput.esc)
            pMainLoopActive = false;

        if (desiredKeys.down && !pDeltaInput.down)
        {
            pMenuCursor++;
            if (pMenuCursor >= static_cast<int8_t>(MENU_SETTINGS_SELECTION::END_ENUM))
                pMenuCursor = 0;

        }
        if (desiredKeys.up && !pDeltaInput.up)
        {
            pMenuCursor--;
            if (pMenuCursor < 0)
                pMenuCursor = (static_cast<int8_t>(MENU_SETTINGS_SELECTION::END_ENUM) - 1);

        }
        if (desiredKeys.enter && !pDeltaInput.enter)
        {
            this->processMenu(GAME_STATE::SETTINGS_MENU, pMenuCursor);
        }
        break;
    }
    case GAME_STATE::CREDITS_MENU:
    {
        if (desiredKeys.enter && !pDeltaInput.enter)
        {
            this->processMenu(GAME_STATE::CREDITS_MENU, 0);
        }
        break;
    }
    case GAME_STATE::IN_GAME:
    {
        if (desiredKeys.esc && !pDeltaInput.esc)
            pGameState = GAME_STATE::GAME_PAUSED;

        // Send input to the player
        //player1->input(desiredKeys);

        if (desiredKeys.mouseMiddleReleased)
            SGL_Log("scroll up");
        if (desiredKeys.mouseMiddlePressed)
            SGL_Log("scroll down");

        if (desiredKeys.mouseLeftPressed)
            pMouseLeftHeld = true;
        if (desiredKeys.mouseLeftReleased)
            pMouseLeftHeld = false;

        if (desiredKeys.up && !pDeltaInput.up)
            pGameGrid->moveSnake(DIRECTION::UP);
        if (desiredKeys.down && !pDeltaInput.down)
            pGameGrid->moveSnake(DIRECTION::DOWN);
        if (desiredKeys.left && !pDeltaInput.left)
            pGameGrid->moveSnake(DIRECTION::LEFT);
        if (desiredKeys.right && !pDeltaInput.right)
            pGameGrid->moveSnake(DIRECTION::RIGHT);

        break;
    }
    case GAME_STATE::GAME_PAUSED:
    {
        if (desiredKeys.esc && !pDeltaInput.esc)
            pGameState = GAME_STATE::IN_GAME;
        if (desiredKeys.m && !pDeltaInput.m)
        {
            this->processMenu(GAME_STATE::IN_GAME, static_cast<uint8_t>(MENU_IN_GAME_OPTIONS::BACK_TO_MAIN_MENU));
        }
        if (desiredKeys.q && !pDeltaInput.q)
        {
            this->pMainLoopActive = false;
        }
        break;
    }
    case GAME_STATE::GAME_OVER:
    {
        if (desiredKeys.m && !pDeltaInput.m)
        {
            this->processMenu(GAME_STATE::IN_GAME, static_cast<uint8_t>(MENU_IN_GAME_OPTIONS::BACK_TO_MAIN_MENU));
        }
        if (desiredKeys.q && !pDeltaInput.q)
        {
            this->pMainLoopActive = false;
        }
        if (desiredKeys.r && !pDeltaInput.r)
        {
            pGameGrid->resetGame();
            pGameState = GAME_STATE::IN_GAME;
        }
        break;
    }
    } // switch end

    // if (desiredKeys.esc)
    // {
    //     SGL_Log("Exiting through EXC key.");
    //     this->pMainLoopActive = false;
    // }
    if (desiredKeys.sdlInternalQuit)
    {
        SGL_Log("Exiting through internal SDL quit.");
        this->pMainLoopActive = false;
    }
    if (desiredKeys.c)
    {
        this->pShakeTime = 0.010f;
    }
    if (desiredKeys.p && !pDeltaInput.p)
    {
        this->pDrawDebugPanel = !pDrawDebugPanel;
    }
    if (desiredKeys.t)
    {
        pShakeTime += 1.0f;
    }
    if (desiredKeys.up && !pDeltaInput.up)
    {

    }
    if (desiredKeys.down && !pDeltaInput.down)
    {

    }
    if (desiredKeys.enter && !pDeltaInput.enter)
    {

    }
    if (desiredKeys.left && !pDeltaInput.left)
    {

    }

    if (desiredKeys.right && !pDeltaInput.right)
    {

    }
    if (desiredKeys.n)
    {
        pWindowManager->resizeWindow(640, 360, true);
        pCurrentResolution = MENU_SETTINGS_RESOLUTION_SELECTION::RES_360P;
    }
    if (desiredKeys.b)
    {
    }
    if (desiredKeys.m)
    {
        pWindowManager->resizeWindow(1280, 720, true);
        pCurrentResolution = MENU_SETTINGS_RESOLUTION_SELECTION::RES_720P;
    }
    if (desiredKeys.f)
    {
        pWindowManager->toggleFullScreen(true);
    }
    if (desiredKeys.g)
    {
        pWindowManager->toggleFullScreen(false);
    }
    if (desiredKeys.e && !pDeltaInput.e)
    {
        pWindowManager->setPostProcessorShader(pWindowManager->assetManager->getShader("customPP"));
        SGL_Log("Custom PP shader.");
    }
    if (desiredKeys.t && !pDeltaInput.t)
    {
        pWindowManager->setPostProcessorShader(pWindowManager->assetManager->getShader("postProcessor"));
        SGL_Log("Default PP shader.");
    }
    if (desiredKeys.y)
        pCameraZoom += 0.010;
    if (desiredKeys.u)
        pCameraZoom -= 0.010;

    if (desiredKeys.z)
        this->pWindowManager->toggleVSYNC(true);
    if (desiredKeys.x)
        this->pWindowManager->toggleVSYNC(false);

    if (pCameraZoom < 0.0)
        pCameraZoom = 0.0;
    if (pCameraZoom > 2.0)
        pCameraZoom = 2.0;

    pDeltaInput = desiredKeys;
}


void Window::render(GAME_STATE gameState)
{
    // ----------- //
    // CAMERA MODE //
    // ----------- //
    this->pWindowManager->setCameraMode(CAMERA_MODE::DEFAULT);

    switch (gameState)
    {
    case GAME_STATE::START_SCREEN_MENU:
    {
        renderMainMenu();
        break;
    }
    case GAME_STATE::SETTINGS_MENU:
    {
        renderOptionsMenu();
        break;
    }
    case GAME_STATE::IN_GAME:
    {
        renderGame();
        break;
    }
    case GAME_STATE::GAME_PAUSED:
    {
        renderGame(true);
        break;
    }

    case GAME_STATE::INTRO_SEQUENCE:
    {
        renderGameIntro();
        break;
    }
    case GAME_STATE::GAME_OVER:
    {
        renderGameOver();
        break;
    }
    case GAME_STATE::GAME_COUNTDOWN:
    {
        renderGameCountdown(3);
        break;
    }
    }

    // Show FPS?
    if (pShowFPSSTatus)
    {
        SGL_Bitmap_Text text;
        text.position.x = 0;
        text.position.y = 0;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "FPS: " + std::to_string(pFPS);
        text.color = SGL_Color{0.8f, 0.2f, 0.2f, 1.0f};
        text.scale = 10;
        this->pWindowManager->renderer->renderBitmapText(text);
    }

    this->pWindowManager->setCameraMode(CAMERA_MODE::OVERLAY);
    if (pDrawDebugPanel)
        this->drawDebugPanel(16.0f, 16.0f, 0.18, SGL_Color(1.0f,1.0f,1.0f,1.0f));

    // END RENDER
    this->pWindowManager->setCameraMode(CAMERA_MODE::DEFAULT);
}

void Window::renderGameIntro()
{
    // Render Background image
    this->pWindowManager->renderer->renderSprite(*pIntroSequenceBG);
    // PRESS ENTER fade in/outer
    pBMPText.position.x = 150;
    pBMPText.position.y = 220;
    pBMPText.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
    pBMPText.text = "PRESS ENTER";
    pBMPText.color = SGL_Color{1.0f, 1.8f, 1.0f, pSplashScreenAlpha};
    pBMPText.scale = 30;
    this->pWindowManager->renderer->renderBitmapText(pBMPText);

}

void Window::renderGameOver()
{
        SGL_Sprite darkenBG;
        darkenBG.position.x = 0;
        darkenBG.position.y = 0;
        darkenBG.size.x = pWindowManager->getWindowCreationSpecs().internalW;
        darkenBG.size.y = pWindowManager->getWindowCreationSpecs().internalH;
        darkenBG.texture = pWindowManager->assetManager->getTexture("box");
        darkenBG.color = SGL_Color{0.0f, 0.0f, 0.0f, 0.40f};
        darkenBG.resetUVCoords();
        this->pWindowManager->renderer->renderSprite(darkenBG);

        SGL_Bitmap_Text text;
        // PAUSED
        text.scale = 22;
        text.position.x = (pWindowManager->getWindowCreationSpecs().internalW / 2) - (text.scale * 4);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) - 40;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "GAME OVER";
        text.color = SGL_Color{1.0f, 1.0f, 01.0f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);
        // ESC
        text.scale = 8 ;
        text.position.x = (pWindowManager->getWindowCreationSpecs().internalW / 2) - (10 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) - 20;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "SCORE: " + std::to_string(pGameGrid->getScore());
        text.color = SGL_Color{0.2f, 0.9f, 0.9f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);

        // [Q]UIT?
        text.scale = 14;
        text.position.x = (pWindowManager->getWindowCreationSpecs().internalW / 2) - (3 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) + 10;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "[M]";
        text.color = SGL_Color{0.70f, 0.20f, 0.30f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);
        text.scale = 14;
        text.position.x += (3 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) + 10;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "ENU";
        text.color = SGL_Color{1.0f, 1.0f, 1.0f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);
        // [M[ENU]
        text.scale = 14;
        text.position.x = (pWindowManager->getWindowCreationSpecs().internalW / 2) - (3 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) + 30;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "[Q]";
        text.color = SGL_Color{0.70f, 0.20f, 0.30f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);
        text.scale = 14;
        text.position.x += (3 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) + 30;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "UIT";
        text.color = SGL_Color{1.0f, 1.0f, 1.0f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);
        // [R]ESTART
        text.scale = 14;
        text.position.x = (pWindowManager->getWindowCreationSpecs().internalW / 2) - (3 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) + 50;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "[R]";
        text.color = SGL_Color{0.70f, 0.20f, 0.30f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);
        text.scale = 14;
        text.position.x += (3 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) + 50;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "ESTART";
        text.color = SGL_Color{1.0f, 1.0f, 01.0f, 1.0f};

        this->pWindowManager->renderer->renderBitmapText(text);
}


void Window::renderGameCountdown(int8_t seconds)
{

}

void Window::renderGame(bool paused)
{
    SGL_Bitmap_Text text;

    pGameGrid->render(*pWindowManager->renderer.get(), *pWindowManager->assetManager.get());
    if (paused)
    {
        SGL_Sprite darkenBG;
        darkenBG.position.x = 0;
        darkenBG.position.y = 0;
        darkenBG.size.x = pWindowManager->getWindowCreationSpecs().internalW;
        darkenBG.size.y = pWindowManager->getWindowCreationSpecs().internalH;
        darkenBG.texture = pWindowManager->assetManager->getTexture("box");
        darkenBG.color = SGL_Color{0.0f, 0.0f, 0.0f, 0.40f};
        darkenBG.resetUVCoords();
        this->pWindowManager->renderer->renderSprite(darkenBG);

        SGL_Bitmap_Text text;
        // PAUSED
        text.scale = 22;
        text.position.x = (pWindowManager->getWindowCreationSpecs().internalW / 2) - (6 * 11);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) - 40;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "PAUSED";
        text.color = SGL_Color{1.0f, 1.0f, 01.0f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);
        // ESC
        text.scale = 8 ;
        text.position.x = (pWindowManager->getWindowCreationSpecs().internalW / 2) - (11 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) - 20;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "PRESS [ESC] TO RETURN";
        text.color = SGL_Color{0.2f, 0.9f, 0.9f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);

        // [Q]UIT?
        text.scale = 14;
        text.position.x = (pWindowManager->getWindowCreationSpecs().internalW / 2) - (3 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) + 10;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "[M]";
        text.color = SGL_Color{0.70f, 0.20f, 0.30f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);
        text.scale = 14;
        text.position.x += (3 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) + 10;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "ENU";
        text.color = SGL_Color{1.0f, 1.0f, 01.0f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);
        // [M[ENU]
        text.scale = 14;
        text.position.x = (pWindowManager->getWindowCreationSpecs().internalW / 2) - (3 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) + 30;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "[Q]";
        text.color = SGL_Color{0.70f, 0.20f, 0.30f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);
        text.scale = 14;
        text.position.x += (3 * text.scale);
        text.position.y = (pWindowManager->getWindowCreationSpecs().internalH / 2) + 30;
        text.texture = pWindowManager->assetManager->getTexture("defaultBitmapFont");
        text.text = "UIT";
        text.color = SGL_Color{1.0f, 1.0f, 01.0f, 1.0f};
        this->pWindowManager->renderer->renderBitmapText(text);

        // pWindowManager->renderer->renderText("PAUSED", 40, 80, 0.58, SGL_Color(1.0f,1.0f,1.0f,1.0f));
        // pWindowManager->renderer->renderText("press 1 to unpause", 40, 110, 0.32, SGL_Color(1.0f,1.0f,1.0f,1.0f));
        // pWindowManager->renderer->renderText("press 'j' to return to the main menu", 30, 160, 0.32, SGL_Color(1.0f,1.0f,1.0f,1.0f));
    }

}


//MENUS
void Window::renderMainMenu()
{
    // Title
    pUISprite->shader = pWindowManager->assetManager->getShader("customSprite");
    // pUISprite->position = glm::vec2(150, 32);
    pUISprite->size = glm::vec2(304, 126);
    pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), 32);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    pUISprite->texture = pWindowManager->assetManager->getTexture("risk_vector");
    pUISprite->changeUVCoords(336, 0, 304, 116);
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    SGL_Color selectedColor{0.8f, 0.30f, 0.30f, 1.0f};
    std::int16_t menuOffsetX = 80;
    std::int16_t menuOffsetY = 200;
    // std::int16_t textHeight = 32;
    // Start
    pUISprite->size = glm::vec2(64, 16);
    pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), menuOffsetY);
    pUISprite->changeUVCoords(0, 224, 64, 16);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (pMenuCursor == 0)
        pUISprite->color = selectedColor;
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    // Settings
    pUISprite->size = glm::vec2(96, 16);
    pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), menuOffsetY + 30);
    pUISprite->changeUVCoords(0, 240, 96, 16);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (pMenuCursor == 1)
        pUISprite->color = selectedColor;
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    // Exit
    pUISprite->size = glm::vec2(48, 16);
    pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), menuOffsetY + 60);
    pUISprite->changeUVCoords(0, 256, 48, 16);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (pMenuCursor == 2)
        pUISprite->color = selectedColor;
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    // Shameless shill
    pUISprite->size = glm::vec2(128, 16);
    pUISprite->position = glm::vec2(4, pWindowManager->getWindowCreationSpecs().internalH - pUISprite->size.y);
    pUISprite->changeUVCoords(0, 336, 128, 16);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    this->pWindowManager->renderer->renderSprite((*pUISprite));
}


void Window::renderOptionsMenu()
{
    // Title
    pUISprite->shader = pWindowManager->assetManager->getShader("customSprite");
    // pUISprite->position = glm::vec2(150, 32);
    pUISprite->size = glm::vec2(304, 126);
    pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), 32);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    pUISprite->texture = pWindowManager->assetManager->getTexture("risk_vector");
    pUISprite->changeUVCoords(336, 0, 304, 116);
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    SGL_Color selectedColor{0.8f, 0.30f, 0.30f, 1.0f};
    SGL_Color valueColor{1.0f, 1.0f, 1.0f, 1.0f};
    std::int16_t menuOffsetX = 60;
    std::int16_t menuOffsetY = 200;

    // Resolution
    pUISprite->size = glm::vec2(128, 16);
    pUISprite->position = glm::vec2( (pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), menuOffsetY);
    pUISprite->changeUVCoords(112, 224, 128, 16);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (pMenuCursor == 0)
        pUISprite->color = selectedColor;
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    switch (pCurrentResolution)
    {
    case MENU_SETTINGS_RESOLUTION_SELECTION::RES_360P:
    {
        // 640 x 320
        pUISprite->size = glm::vec2(96, 16);
        pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), menuOffsetY + 18);
        pUISprite->changeUVCoords(112, 288, 96, 16);
        pUISprite->color = valueColor;
        break;
    }
    case MENU_SETTINGS_RESOLUTION_SELECTION::RES_720P:
    {
        // 1280 x 720 (HD)
        pUISprite->size = glm::vec2(96, 16);
        pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), menuOffsetY + 18);
        pUISprite->changeUVCoords(112, 304, 96, 16);
        pUISprite->color = valueColor;
        break;
    }
    case MENU_SETTINGS_RESOLUTION_SELECTION::RES_FULLSCREEN:
    {
        // FULLSCREEN
        pUISprite->size = glm::vec2(128, 16);
        pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), menuOffsetY + 18);
        pUISprite->changeUVCoords(112, 272, 128, 16);
        pUISprite->color = valueColor;
        break;
    }
    }
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    // VSYNC
    menuOffsetY += 40;
    pUISprite->size = glm::vec2(64, 16);
    pUISprite->position = glm::vec2( (pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), (menuOffsetY));
    pUISprite->changeUVCoords(112, 240, 64, 16);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (pMenuCursor == 1)
        pUISprite->color = selectedColor;
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    if (pWindowManager->getWindowCreationSpecs().activeVSYNC)
    {
        // ON
        pUISprite->size = glm::vec2(32, 16);
        pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - ((pUISprite->size.x / 2)), (menuOffsetY) + 18);
        pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
        pUISprite->changeUVCoords(0, 272, 32, 16);
        //pUISprite->color = valueColor;
    }
    else
    {
        // OFF
        pUISprite->size = glm::vec2(48, 16);
        pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), (menuOffsetY) + 18);
        pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
        pUISprite->changeUVCoords(32, 272, 48, 16);
    }
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    // SHOW FPS
    menuOffsetY += 40;
    pUISprite->size = glm::vec2(96, 16);
    pUISprite->position = glm::vec2( (pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), (menuOffsetY));
    pUISprite->changeUVCoords(0, 320, 96, 16);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (pMenuCursor == 2)
        pUISprite->color = selectedColor;
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    if (pShowFPSSTatus)
    {
        // ON
        pUISprite->size = glm::vec2(32, 16);
        pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - ((pUISprite->size.x / 2)), (menuOffsetY) + 18);
        pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
        pUISprite->changeUVCoords(0, 272, 32, 16);
        //pUISprite->color = valueColor;
    }
    else
    {
        // OFF
        pUISprite->size = glm::vec2(48, 16);
        pUISprite->position = glm::vec2((pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), (menuOffsetY) + 18);
        pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
        pUISprite->changeUVCoords(32, 272, 48, 16);
    }
    this->pWindowManager->renderer->renderSprite((*pUISprite));



    // RETURN
    menuOffsetY += 40;
    pUISprite->size = glm::vec2(80, 16);
    pUISprite->position = glm::vec2( (pWindowManager->getWindowCreationSpecs().internalW / 2 ) - (pUISprite->size.x / 2), (menuOffsetY));
    pUISprite->changeUVCoords(112, 256, 80, 16);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (pMenuCursor == 3)
        pUISprite->color = selectedColor;
    this->pWindowManager->renderer->renderSprite((*pUISprite));

    // Shameless shill
    pUISprite->size = glm::vec2(128, 16);
    pUISprite->position = glm::vec2(4, pWindowManager->getWindowCreationSpecs().internalH - pUISprite->size.y);
    pUISprite->changeUVCoords(0, 336, 128, 16);
    pUISprite->color = SGL_Color(1.0f, 1.0f, 1.0f, 1.0f);
    this->pWindowManager->renderer->renderSprite((*pUISprite));

}


//small window with program data for debbuging
void Window::drawDebugPanel(float x, float y, float fontSize, SGL_Color color)
{
    float offset = (fontSize * 100) / 2;
    SGL_Color titleColor = {1.0f, 0.30f, 0.30f, 1.0f};
    SGL_Color subTitleColor = {0.30f, 1.0f, 0.30f, 1.0f};
    int center = (pWindowManager->getWindowCreationSpecs().internalW / 2) - (x * 4);
    int left = x;
    int right = (pWindowManager->getWindowCreationSpecs().internalW) - (x * 8);
    int spacing = offset * 6;
    WindowCreationSpecs windowSpecs = pWindowManager->getWindowCreationSpecs();
    std::stringstream textureMem;
    std::string timeElapsedStr = std::to_string(pTimeElapsed / 1000);
    timeElapsedStr.erase(timeElapsedStr.find('.') + 2, timeElapsedStr.back());
    // Background image
    this->pWindowManager->renderer->renderSprite((*pDebugPanelBackground));

    std::string windowFocus = "Has focus: ";
    windowFocus += (pWindowManager->hasKeyboardFocus() ? "YES" : "NO");
    std::string isFullscreen = "Fullscreen: ";
    isFullscreen += (windowSpecs.fullScreen ? "YES" : "NO");
    std::string VSYNC = "VSYNC: ";
    VSYNC += (windowSpecs.activeVSYNC ? "YES" : "NO");
    std::string LMB = "LMB: ";
    LMB += (pDeltaInput.mouseLeftPressed ? "PRESSED" : "-----");
    textureMem << "GPU Texture Memory (MB): " << (static_cast<float>(pWindowManager->assetManager->getTextureMemoryGPU()) / 1024) / 1024;

    // Title
    pWindowManager->renderer->renderText("SkeletonGL Debug Panel", center, y, fontSize, titleColor);
    pWindowManager->renderer->renderText("Ver 0.8.0", right, y, fontSize, titleColor);

    // Engine state block (top left)
    pWindowManager->renderer->renderText("--- ENGINE STATUS ---", left, y + (offset * 1.0f), fontSize, subTitleColor);
    pWindowManager->renderer->renderText("Host OS: " + pWindowManager->hostData->hostOS, left, y + (offset * 2.0f), fontSize, color);
    pWindowManager->renderer->renderText("Physics delta time: " + std::to_string(pDeltaTimeMS), left, y + (offset * 3.0f), fontSize, color);
    pWindowManager->renderer->renderText("Render delta time: " + std::to_string(pWindowManager->getRenderDeltaTime()), left, y + (offset * 4.0f), fontSize, color);
    pWindowManager->renderer->renderText("Time elapsed: " + timeElapsedStr, left, y + (offset * 5.0f), fontSize, color);
    pWindowManager->renderer->renderText(textureMem.str(), left, y + (offset * 6.0f), fontSize, color);

    pWindowManager->renderer->renderText("--- CAMERA ---", left, y + (offset * 7.0f), fontSize, subTitleColor);
    pWindowManager->renderer->renderText("X: " + std::to_string(pWindowManager->getCameraPosX()), left, y + (offset * 8.0f), fontSize, color);
    pWindowManager->renderer->renderText("Y: " + std::to_string(pWindowManager->getCameraPosY()), left + spacing, y + (offset * 8.0f), fontSize, color);
    pWindowManager->renderer->renderText("W: " + std::to_string(pWindowManager->getCameraPosW()), left, y + (offset * 9.0f), fontSize, color);
    pWindowManager->renderer->renderText("H: " + std::to_string(pWindowManager->getCameraPosH()), left + spacing, y + (offset * 9.0f), fontSize, color);
    pWindowManager->renderer->renderText("Zoom: " + std::to_string(pCameraZoom), left, y + (offset * 10.0f), fontSize, color);

    pWindowManager->renderer->renderText("--- CONSOLE ---", left, y + (offset * 12.0f), fontSize, subTitleColor);
    for (int i = 16; i >= 1; --i)
    {
        pWindowManager->renderer->renderText(SGL_LOG_HISTORY[SGL_LOG_HISTORY.size() - i], left, y + (offset * (29.0f - i)), fontSize, color);
    }

    // Center
    pWindowManager->renderer->renderText("--- MAIN WINDOW ---", center, y + (offset * 1.0f), fontSize, subTitleColor);
    pWindowManager->renderer->renderText("Display resolution: " + std::to_string(pWindowManager->getWindowCreationSpecs().currentW) + " x " +
                                         std::to_string(pWindowManager->getWindowCreationSpecs().currentH),
                                         center, y + (offset * 2.0f), fontSize, color);
    pWindowManager->renderer->renderText("Internal resolution: " + std::to_string(pWindowManager->getWindowCreationSpecs().internalW) + " x " +
                                         std::to_string(pWindowManager->getWindowCreationSpecs().internalH),
                                         center, y + (offset * 3.0f), fontSize, color);
    pWindowManager->renderer->renderText(windowFocus, center, y + (offset * 4.0f), fontSize, color);
    pWindowManager->renderer->renderText(isFullscreen, center, y + (offset * 5.0f), fontSize, color);
    pWindowManager->renderer->renderText(VSYNC, center, y + (offset * 6.0f), fontSize, color);
    pWindowManager->renderer->renderText("FPS: " + std::to_string(pFPS), center, y + (offset * 7.0f), fontSize, color);


    // Right
    pWindowManager->renderer->renderText("--- MOUSE ---", right, y + (offset * 1.0f), fontSize, subTitleColor);
    pWindowManager->renderer->renderText("X: " + std::to_string(pDeltaInput.rawMousePosX),
                                         right,  y + (offset * 2.0f), fontSize, color);
    pWindowManager->renderer->renderText("N: " + std::to_string(pDeltaInput.normalizedMousePosX),
                                         right + spacing,  y + (offset * 2.0f), fontSize, color);
    pWindowManager->renderer->renderText("Y: " + std::to_string(pDeltaInput.rawMousePosY),
                                         right,  y + (offset * 3.0f), fontSize, color);
    pWindowManager->renderer->renderText("N: " + std::to_string(pDeltaInput.normalizedMousePosY),
                                         right + spacing,  y + (offset * 3.0f), fontSize, color);
}

void Window::processMenu(GAME_STATE menu, int8_t cursor)
{
    if (menu == GAME_STATE::START_SCREEN_MENU)
    {
        MENU_START_SCREEN_SELECTION optionSelected = static_cast<MENU_START_SCREEN_SELECTION>(cursor);
        switch (optionSelected)
        {
        case MENU_START_SCREEN_SELECTION::START_GAME:
        {
            SGL_Log("START GAME PRESSED");
            pGameState = GAME_STATE::IN_GAME;
            pMenuCursor = 0;
            break;
        }
        case MENU_START_SCREEN_SELECTION::SETTINGS:
        {
            SGL_Log("SETTINGS PRESSED");
            pGameState = GAME_STATE::SETTINGS_MENU;
            pMenuCursor = 0;
            break;
        }
        case MENU_START_SCREEN_SELECTION::EXIT:
        {
            SGL_Log("EXIT PRESSED");
            //
            pMenuCursor = -1;
            pMainLoopActive = false;
            break;
        }
        default:
        {
            SGL_Log("Processing out of bounds menu cursor. Exiting.");
            pMainLoopActive = false;
            break;
        }
        }
    }
    else if (menu == GAME_STATE::SETTINGS_MENU)
    {
        MENU_SETTINGS_SELECTION optionSelected = static_cast<MENU_SETTINGS_SELECTION>(cursor);
        switch (optionSelected)
        {
        case MENU_SETTINGS_SELECTION::RESOLUTION:
        {
            if ((static_cast<uint8_t>(pCurrentResolution) + 1) >= (static_cast<uint8_t>(MENU_SETTINGS_RESOLUTION_SELECTION::END_ENUM)))
                pCurrentResolution = static_cast<MENU_SETTINGS_RESOLUTION_SELECTION>(0);
            else
                pCurrentResolution = static_cast<MENU_SETTINGS_RESOLUTION_SELECTION>(static_cast<uint8_t>(pCurrentResolution) + 1);

            if (pCurrentResolution == MENU_SETTINGS_RESOLUTION_SELECTION::RES_FULLSCREEN)
                pWindowManager->toggleFullScreen(true);
            else if (pCurrentResolution == MENU_SETTINGS_RESOLUTION_SELECTION::RES_360P)
            {
                pWindowManager->toggleFullScreen(false);
                pWindowManager->resizeWindow(640, 360, true);
            }
            else if (pCurrentResolution == MENU_SETTINGS_RESOLUTION_SELECTION::RES_720P)
            {
                pWindowManager->toggleFullScreen(false);
                pWindowManager->resizeWindow(1280, 720, true);
            }


            SGL_Log("RESOLUTION PRESSED: " + std::to_string(static_cast<uint8_t>(pCurrentResolution)));
            break;
        }
        case MENU_SETTINGS_SELECTION::VSYNC:
        {
            if (pVSYNCStatus)
            {
                pVSYNCStatus = false;
                pWindowManager->toggleVSYNC(pVSYNCStatus);
            }
            else
            {
                pVSYNCStatus = true;
                pWindowManager->toggleVSYNC(pVSYNCStatus);
            }
            // pDrawDebugPanel = !pDrawDebugPanel;
            break;
        }
        case MENU_SETTINGS_SELECTION::FPS:
        {
            SGL_Log("TOGGLE FPS PRESSED");
            pShowFPSSTatus = !pShowFPSSTatus;
            break;
        }
        case MENU_SETTINGS_SELECTION::RETURN:
        {
            SGL_Log("RETURN PRESSED");
            pMenuCursor = 0;
            pGameState = GAME_STATE::START_SCREEN_MENU;
            break;
        }
        default:
        {
            SGL_Log("Processing out of bounds menu cursor. Exiting.");
            pMainLoopActive = false;
            break;
        }
        }
    } // end options
    else if (menu == GAME_STATE::IN_GAME)
    {
        MENU_IN_GAME_OPTIONS optionSelected = static_cast<MENU_IN_GAME_OPTIONS>(cursor);
        switch (optionSelected)
        {
        case MENU_IN_GAME_OPTIONS::TOGGLE_PAUSE:
        {

            break;
        }
        case MENU_IN_GAME_OPTIONS::BACK_TO_MAIN_MENU:
        {
            SGL_Log("Going back to main menu");
            pGameState = GAME_STATE::START_SCREEN_MENU;
            break;
        }
        default:
        {
            SGL_Log("Processing out of bounds menu cursor. Exiting.");
            pMainLoopActive = false;
            break;
        }

        }
    }
}


//game assets loading, textures, shaders, fonts etc
void Window::loadAssets()
{
    SGL_Log("Loading assets....");
    pWindowManager->checkForErrors();
    SGL_Log("Loading 2D textures.");

    pWindowManager->assetManager->loadTexture("assets/textures/snake_ui.png", true, "UI");
    pWindowManager->assetManager->loadTexture("assets/textures/risk_vector.png", true, "risk_vector");
    pWindowManager->assetManager->loadTexture("assets/textures/bg_space2.png", true, "bg_space");
    pWindowManager->assetManager->loadTexture("assets/textures/splash_screen.jpg", false, "splashScreen");
    pWindowManager->assetManager->loadTexture("assets/textures/sjw.png", true, "debug1");
    SGL_Log("Loading custom shaders.");
    pWindowManager->assetManager->loadShaders("assets/shaders/customSpriteV.c", "assets/shaders/customSpriteF.c", nullptr, "customSprite", SHADER_TYPE::SPRITE);
    pWindowManager->assetManager->loadShaders("assets/shaders/customLineV.c", "assets/shaders/customLineF.c",nullptr, "customLine", SHADER_TYPE::LINE);
    pWindowManager->assetManager->loadShaders("assets/shaders/customPointV.c", "assets/shaders/customPointF.c",nullptr, "customPoint", SHADER_TYPE::PIXEL);
    pWindowManager->assetManager->loadShaders("assets/shaders/customTextV.c", "assets/shaders/customTextF.c",nullptr, "customText", SHADER_TYPE::TEXT);
    pWindowManager->assetManager->loadShaders("assets/shaders/customFBOV.c", "assets/shaders/customFBOF.c",nullptr, "customPP", SHADER_TYPE::POST_PROCESSOR);

    pWindowManager->checkForErrors();
    SGL_Log("Exiting asset loading.");
}
