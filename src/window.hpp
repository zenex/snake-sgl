// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗   ██╗  ██╗██╗   ██╗███████╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝   ╚██╗██╔╝╚██╗ ██╔╝╚══███╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝     ╚███╔╝  ╚████╔╝   ███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗     ██╔██╗   ╚██╔╝   ███╔╝
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗██╗██╔╝ ██╗   ██║   ███████╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝
// Author:  AlexHG @ NEOHEX.XYZ
// License: MIT License
// Website: https://neohex.xyz

#ifndef WINDOW_HPP
#define WINDOW_HPP


#include <iostream>
#include <sstream>
#include <ctime>
#include <vector>
#include <memory>
#include <stdio.h>
#include <thread>
#include <cstdint>
#include <random>
#include <array>

#include "skeletonGL/skeletonGL.hpp"
#include "grid.hpp"



enum class GAME_STATE : std::int8_t
{
    START_SCREEN_MENU = 0,
    SETTINGS_MENU = 1,
    CREDITS_MENU = 2,
    IN_GAME = 3,
    INTRO_SEQUENCE = 4,
    GAME_COUNTDOWN = 5,
    GAME_OVER = 6,
    GAME_PAUSED = 7,
    END_ENUM = 8
};
// Start menu options
enum class MENU_START_SCREEN_SELECTION : std::int8_t
{
    START_GAME = 0,
    SETTINGS = 1,
    EXIT = 2,
    END_ENUM = 3
};
// Settings menu options
enum class MENU_SETTINGS_SELECTION : std::int8_t
{
    RESOLUTION = 0,
    VSYNC = 1,
    FPS = 2,
    RETURN = 3,
    END_ENUM = 4
};
// Supported resolutions
enum class MENU_SETTINGS_RESOLUTION_SELECTION : std::int8_t
{
    RES_360P = 0,
    RES_720P = 1,
    RES_FULLSCREEN = 2,
    END_ENUM = 3
};
// In game options
enum class MENU_IN_GAME_OPTIONS : std::uint8_t
{
    TOGGLE_PAUSE = 0,
    BACK_TO_MAIN_MENU = 1,
    END_ENUM = 2
};


class Window
{
private:
    // Disable all copying and moving (singelton pattern)
    Window(const Window&) = delete;
    Window &operator = (const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator = (Window &&) = delete;

    // SkeletonGL
    SGL_Window *pWindowManager;
    // Cap delta time to a max value to stabilize physics in slower machines
    SGL_InputFrame pDeltaInput;
    bool pMouseLeftHeld;

    // Splash screen
    float pSplashScreenAlpha;
    std::int8_t pSplashScreenControl;

    // Frame counter
    std::uint8_t pFPSCounter;
    std::uint8_t pCurrentFrame;
    float pAvgDT;
    float pFrameDTCounter, pPreviousFrameDT;
    std::int16_t pTicksPerFrame;
    bool pUpdateFrame;

    std::vector<std::shared_ptr<SGL_Sprite>> pSprites;
    GAME_STATE pGameState;
    bool pMainLoopActive, pDrawDebugPanel, pVSYNCStatus, pShowFPSSTatus;;
    int8_t pMenuCursor;
    // Effects controllers
    GLfloat pShakeTime;
    float pCameraZoom;
    // Current settings
    MENU_SETTINGS_RESOLUTION_SELECTION pCurrentResolution;

    SGL_Bitmap_Text pBMPText;
    std::shared_ptr<Grid>pGameGrid;

    //required UI sprites
    std::shared_ptr<SGL_Sprite>pUISprite, pDebugPanelBackground,
        pIntroSequenceBG, pAvatar, pCursor;

    // FIXED TIME STEP
    float pDeltaTimeMS, pFixedTimeStepMS, pDTAccumulator;
    std::uint16_t pUpdateCycles, pMaxUpdatesPerFrame;
    std::vector<SGL_FrameData> pRenderedFrames;
    std::chrono::steady_clock::time_point pChronoStart, pChronoEnd; ///< C++11 chrono based counter
    // FPS META DATA
    float pTimeElapsed, pDeltaTimeElapsed; // Used to time when to collect fps meta data
    std::uint16_t pFrameCounter; // Counts frames, reset every second
    std::uint16_t pFPS;         // Current FPS, updated every second


public:

    Window();
    ~Window();

    int createSGLWindow();

    void mainLoop();
    void introSequence();
    void render(GAME_STATE gameState);
    void update(GAME_STATE gameState);
    void input(GAME_STATE gameState);

    //game assets loading, textures, shaders, fonts etc
    void loadAssets();
    //renders all the menus
    void renderMainMenu();
    void renderOptionsMenu();
    void renderGameIntro();
    void renderGame(bool paused = false);
    void renderGameOver();
    void renderGameCountdown(int8_t seconds);
    //process the current cursor position
    void processMenu(GAME_STATE menu, int8_t cursor);

    // Small window with program data for debbuging
    void drawDebugPanel(float x, float y, float fontSize, SGL_Color color);
};

#endif
