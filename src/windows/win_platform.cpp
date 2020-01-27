#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <xinput.h>

#include "win_platform.h"
#include "../game/quickmake_game.cpp"

#include "opengl_renderer.cpp"
#include "directsound_audio.cpp"

static bool programRunning = false;

static int gameWidth = 768;
static int gameHeight = 432;
static float targetMSPerFrame = 1000.0f / 60.0f;

// xinput functions
// NOTE(ebuchholz): avoiding redefinition of functions in xinput.h
typedef DWORD WINAPI x_input_get_state(DWORD dwUserIndex, XINPUT_STATE *pState);
static x_input_get_state *XInputGetState_;
#define XInputGetState XInputGetState_
typedef DWORD WINAPI x_input_set_state(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration);
static x_input_set_state *XInputSetState_;
#define XInputSetState XInputSetState_

void DEBUGPrintString (char *string) {
    OutputDebugStringA(string);
}

LRESULT CALLBACK windowCallback (HWND window, unsigned int message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    // TODO(ebuchholz): Determine whether WM_DESTROY is necessary
    switch (message) {
        case WM_CLOSE: {
            programRunning = false;
        } break;
        case WM_SIZE: {
            RECT clientRect;
            GetClientRect(window, &clientRect);
            gameWidth = clientRect.right - clientRect.left;
            gameHeight = clientRect.bottom - clientRect.top;
        } break;
        default: {
            result = DefWindowProcA(window, message, wParam, lParam);
            break;
        }
    }
    return result;
}

static void processWindowsMessages (HWND window, game_input *input, render_command_list *renderCommands) {
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
        switch (message.message) {
            case WM_QUIT: {
                programRunning = false;
            } break; 
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                unsigned int keyCode = (unsigned int)message.wParam;
                //bool wasDown = (message.lParam & (1 << 30)) != 0;
                bool keyDown = (message.lParam & (1 << 31)) == 0;
                // NOTE(ebuchholz): don't care whether the button was just pressed for now
                if (keyCode == 'W') {
                    if (!input->wKey.down && keyDown) {
                        input->wKey.justPressed = true;
                    }
                    input->wKey.down = keyDown;
                }
                else if (keyCode == 'E') {
                    if (!input->eKey.down && keyDown) {
                        input->eKey.justPressed = true;
                    }
                    input->eKey.down = keyDown;
                }
                else if (keyCode == 'T') {
                    if (!input->tKey.down && keyDown) {
                        input->tKey.justPressed = true;
                    }
                    input->tKey.down = keyDown;
                }
                else if (keyCode == 'Y') {
                    if (!input->yKey.down && keyDown) {
                        input->yKey.justPressed = true;
                    }
                    input->yKey.down = keyDown;
                }
                else if (keyCode == 'U') {
                    if (!input->uKey.down && keyDown) {
                        input->uKey.justPressed = true;
                    }
                    input->uKey.down = keyDown;
                }
                else if (keyCode == 'K') {
                    if (!input->kKey.down && keyDown) {
                        input->kKey.justPressed = true;
                    }
                    input->kKey.down = keyDown;
                }
                else if (keyCode == 'J') {
                    if (!input->jKey.down && keyDown) {
                        input->jKey.justPressed = true;
                    }
                    input->jKey.down = keyDown;
                }
                else if (keyCode == 'H') {
                    if (!input->hKey.down && keyDown) {
                        input->hKey.justPressed = true;
                    }
                    input->hKey.down = keyDown;
                }
                else if (keyCode == 'G') {
                    if (!input->gKey.down && keyDown) {
                        input->gKey.justPressed = true;
                    }
                    input->gKey.down = keyDown;
                }
                else if (keyCode == 'F') {
                    if (!input->fKey.down && keyDown) {
                        input->fKey.justPressed = true;
                    }
                    input->fKey.down = keyDown;
                }
                else if (keyCode == 'D') {
                    if (!input->dKey.down && keyDown) {
                        input->dKey.justPressed = true;
                    }
                    input->dKey.down = keyDown;
                }
                else if (keyCode == 'S') {
                    if (!input->sKey.down && keyDown) {
                        input->sKey.justPressed = true;
                    }
                    input->sKey.down = keyDown;
                }
                else if (keyCode == 'A') {
                    if (!input->aKey.down && keyDown) {
                        input->aKey.justPressed = true;
                    }
                    input->aKey.down = keyDown;
                }
                else if (keyCode == VK_UP) {
                    if (!input->upKey.down && keyDown) {
                        input->upKey.justPressed = true;
                    }
                    input->upKey.down = keyDown;
                }
                else if (keyCode == VK_DOWN) {
                    if (!input->downKey.down && keyDown) {
                        input->downKey.justPressed = true;
                    }
                    input->downKey.down = keyDown;
                }
                else if (keyCode == VK_LEFT) {
                    if (!input->leftKey.down && keyDown) {
                        input->leftKey.justPressed = true;
                    }
                    input->leftKey.down = keyDown;
                }
                else if (keyCode == VK_RIGHT) {
                    if (!input->rightKey.down && keyDown) {
                        input->rightKey.justPressed = true;
                    }
                    input->rightKey.down = keyDown;
                }

                if (keyDown) {
                    int altDown = (message.lParam & (1 << 29));
                    if((keyCode == VK_F4) && altDown) {
                        programRunning = false;
                    }
                }
            } break;
            case WM_MOUSEMOVE: {
                POINT newMousePos;
                GetCursorPos(&newMousePos);
                ScreenToClient(window, &newMousePos);
                input->pointerX = newMousePos.x;
                input->pointerY = newMousePos.y;
            } break;
            case WM_LBUTTONDOWN: {
                if (!input->pointerDown) {
                    input->pointerJustDown = true;
                }
                input->pointerDown = true;
            } break;
            case WM_LBUTTONUP: {
                input->pointerDown = false;
            } break;
            default: {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } break;
        }
    }
}

static void updateControllerButton (XINPUT_GAMEPAD gamepad, input_key *button, WORD buttonBitmask) {
    button->justPressed = false;
    if (gamepad.wButtons & buttonBitmask) {
        if (!button->down) {
            button->justPressed = true;
        }
        button->down = true;
    }
    else {
        button->down = false;
    }
}

static void updateControllerTrigger (float *trigger, input_key *triggerButton, BYTE triggerValue, BYTE triggerMax) {
    triggerButton->justPressed = false;
    if (triggerValue >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
        triggerValue -= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
        *trigger = (float)((float)triggerValue / (float)triggerMax);
        if (!triggerButton->down) {
            triggerButton->justPressed = true;
        }
        triggerButton->down = true;
    }
    else {
        *trigger = 0.0f;
        triggerButton->down = false;
    }

}

static void updateControllerStick (SHORT gamepadStickX, SHORT gamepadStickY, float *stickX, float *stickY, SHORT deadzone) {
    float floatStickX = (float)gamepadStickX;
    float floatStickY = -(float)gamepadStickY;
    float magnitude = sqrtf(floatStickX * floatStickX + floatStickY * floatStickY);
    float normalizedSX = floatStickX / magnitude;
    float normalizedSY = floatStickY / magnitude;

    if (magnitude > (float)deadzone) {
        if (magnitude > 32767.0f) {
            magnitude = 32767.0f;
        }
        magnitude -= deadzone;
        float normalizedMagnitude = magnitude / (32767.0f - deadzone);
        *stickX = normalizedSX * normalizedMagnitude;
        *stickY = normalizedSY * normalizedMagnitude;
    }
    else {
        *stickX = 0.0f;
        *stickY = 0.0f;
    }
}

static void updateControllerStickDirection (input_key *dir0, input_key *dir1, SHORT gamepadVal, SHORT deadzone) {
    dir0->justPressed = false;
    if (gamepadVal < -deadzone) {
        if (!dir0->down) {
            dir0->justPressed = true;
        }
        dir0->down = true;
    }
    else {
        dir0->down = false;
    }
    dir1->justPressed = false;
    if (gamepadVal > deadzone) {
        if (!dir1->down) {
            dir1->justPressed = true;
        }
        dir1->down = true;
    }
    else {
        dir1->down = false;
    }
}

static void updateController (game_controller_input *controller, XINPUT_STATE state) {
    XINPUT_GAMEPAD gamepad = state.Gamepad;
    updateControllerButton(gamepad, &controller->dPadUp, XINPUT_GAMEPAD_DPAD_UP);
    updateControllerButton(gamepad, &controller->dPadDown, XINPUT_GAMEPAD_DPAD_DOWN);
    updateControllerButton(gamepad, &controller->dPadLeft, XINPUT_GAMEPAD_DPAD_LEFT);
    updateControllerButton(gamepad, &controller->dPadRight, XINPUT_GAMEPAD_DPAD_RIGHT);
    updateControllerButton(gamepad, &controller->start, XINPUT_GAMEPAD_START);
    updateControllerButton(gamepad, &controller->back, XINPUT_GAMEPAD_BACK);
    updateControllerButton(gamepad, &controller->leftStick, XINPUT_GAMEPAD_LEFT_THUMB);
    updateControllerButton(gamepad, &controller->rightStick, XINPUT_GAMEPAD_RIGHT_THUMB);
    updateControllerButton(gamepad, &controller->leftBumper, XINPUT_GAMEPAD_LEFT_SHOULDER);
    updateControllerButton(gamepad, &controller->rightBumper, XINPUT_GAMEPAD_RIGHT_SHOULDER);
    updateControllerButton(gamepad, &controller->aButton, XINPUT_GAMEPAD_A);
    updateControllerButton(gamepad, &controller->bButton, XINPUT_GAMEPAD_B);
    updateControllerButton(gamepad, &controller->xButton, XINPUT_GAMEPAD_X);
    updateControllerButton(gamepad, &controller->yButton, XINPUT_GAMEPAD_Y);

    BYTE triggerMax = 255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
    updateControllerTrigger(&controller->leftTrigger, &controller->leftTriggerButton, gamepad.bLeftTrigger, triggerMax);
    updateControllerTrigger(&controller->rightTrigger, &controller->rightTriggerButton, gamepad.bRightTrigger, triggerMax);

    updateControllerStick(gamepad.sThumbLX, gamepad.sThumbLY, &controller->leftStickX, &controller->leftStickY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    updateControllerStick(gamepad.sThumbRX, gamepad.sThumbRY, &controller->rightStickX, &controller->rightStickY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

    updateControllerStickDirection(&controller->leftStickUp, &controller->leftStickDown, -gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    updateControllerStickDirection(&controller->leftStickLeft, &controller->leftStickRight, gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    updateControllerStickDirection(&controller->rightStickUp, &controller->rightStickDown, -gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    updateControllerStickDirection(&controller->rightStickLeft, &controller->rightStickRight, gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

static char *readEntireTextFile (char *path) {
    FILE *textFile; 
    fopen_s(&textFile, path, "rb");
    assert(textFile); // TODO(ebuchholz): better error check?

    fseek(textFile, 0, SEEK_END);
    int fileSize = ftell(textFile);
    fseek(textFile, 0, SEEK_SET);

    char *fileData = (char *)malloc(fileSize + 1);
    fread(fileData, fileSize, 1, textFile);
    fileData[fileSize] = 0;
    fclose(textFile);

    return fileData;
}

static size_t writeEntireFile (char *path, void *data, size_t size) {
    FILE *file; 
    fopen_s(&file, path, "w");
    assert(file); // TODO(ebuchholz): better error check?

    size_t resultSize = fwrite(data, 1, size, file);

    fclose(file);

    return resultSize;
}

static void loadXInput () {
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if(!XInputLibrary) {
        XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
    }
    if(!XInputLibrary) {
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }

    // TODO(ebuchholz): replace with error-handling that doesn't crash program
    if(XInputLibrary) {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        if(!XInputGetState) { 
            assert(0);
        }
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
        if(!XInputSetState) { 
            assert(0);
        }
    }
    else {
        assert(0); // couldn't load xinput
    }
}

int WINAPI WinMain (HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode) {
    LARGE_INTEGER perfCountFrequencyResult;
    QueryPerformanceFrequency(&perfCountFrequencyResult);
    long long perfCountFrequency = perfCountFrequencyResult.QuadPart;

    WNDCLASSA windowClass = {};
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);
    windowClass.hInstance = instance;
    windowClass.lpfnWndProc = windowCallback;
    windowClass.lpszClassName = "QuickMakeClass";

    if (RegisterClassA(&windowClass)) {

        //DEVMODE dmScreenSettings;                   
        //memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));  
        //dmScreenSettings.dmSize = sizeof(dmScreenSettings);     
        //dmScreenSettings.dmPelsWidth    = gameWidth;            
        //dmScreenSettings.dmPelsHeight   = gameHeight;          
        //dmScreenSettings.dmBitsPerPel   = 32;           
        //dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
        //if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
        //{
        //    return 0;
        //}

        RECT targetWindowSize;
        targetWindowSize.left = 0;
        targetWindowSize.top = 0;
        targetWindowSize.right = gameWidth;
        targetWindowSize.bottom = gameHeight;

        //DWORD windowStyle = WS_POPUP | WS_VISIBLE;
        DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE; 
        AdjustWindowRectEx(&targetWindowSize, windowStyle, false, WS_EX_APPWINDOW);

        HWND window = CreateWindowExA(
            WS_EX_APPWINDOW, //WS_EX_TOPMOST,
            windowClass.lpszClassName, 
            "Quick Make", 
            windowStyle,// | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            0, 
            0, 
            targetWindowSize.right - targetWindowSize.left, 
            targetWindowSize.bottom - targetWindowSize.top, 
            0, 
            0,
            instance, 
            0
        );
        if (window) {
            programRunning = true;
            HDC deviceContext = GetDC(window);

            renderer_memory rendererMemory;
            // TODO(ebuchholz): wonder how much memory the renderer needs
            rendererMemory.memoryCapacity = 10 * 1024 * 1024;
            rendererMemory.memory = malloc(rendererMemory.memoryCapacity);
            rendererMemory.debugPrintString = &DEBUGPrintString;

            initOpenGL(window, &rendererMemory);

            // Game memory (game state, etc.)
            game_memory gameMemory = {};
            gameMemory.memoryCapacity = 50 * 1024 * 1024; // 50MB arbitrarily decided
            gameMemory.memory = malloc(gameMemory.memoryCapacity);
            gameMemory.tempMemoryCapacity = 10 * 1024 * 1024; // another 10MB arbitrarily decided
            gameMemory.tempMemory = malloc(gameMemory.tempMemoryCapacity);

            for (unsigned int i = 0; i < gameMemory.memoryCapacity; ++i) {
                *((char *)gameMemory.memory + i) = 0;
            }

            // Load assets
            asset_list assetList = {};
            assetList.numAssetsToLoad = 0;
            assetList.maxAssetsToLoad = 100;
            assetList.assetsToLoad = 
                (asset_to_load *)malloc(assetList.maxAssetsToLoad * sizeof(asset_to_load));

            getGameAssetList(&assetList);

            memory_arena workingAssetMemory = {};
            workingAssetMemory.capacity = 30 * 1024 * 1024; // 30MB limit for working with asset files?
            workingAssetMemory.base = malloc(workingAssetMemory.capacity);

            // init sound
            win_sound_output soundOutput = {};
            soundOutput.samplesPerSecond = 44100;
            soundOutput.bytesPerSample = 2;
            soundOutput.secondaryBufferSize = soundOutput.samplesPerSecond * soundOutput.bytesPerSample;
            initDirectSound(window, &soundOutput);
            clearSecondaryBuffer(&soundOutput);
            soundOutput.secondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
            soundOutput.soundValid = false;

            sound_sample *soundSamples = (sound_sample *)malloc(soundOutput.samplesPerSecond * 4);
            assert(soundSamples);

            loadXInput();

            // set up windows-specific options that the game needs to know about
            platform_options options = {};
            options.audioSampleRate = soundOutput.samplesPerSecond;

            for (int i = 0; i < assetList.numAssetsToLoad; ++i) {
                asset_to_load *assetToLoad = assetList.assetsToLoad + i;
                workingAssetMemory.size = 0;
                switch (assetToLoad->type){
                    // catch-all for text files? no work to be done on platform except reading the file
                    case ASSET_TYPE_ANIMATION_DATA: {
                        // TODO(ebuchholz): define real skeleton+animations file format define real skeleton+animations file format define real skeleton+animations file format define real skeleton+animations file format define real skeleton+animations file format
                        char *fileData = 0;
                        //char *fileData = readEntireTextFile(assetToLoad->path);
                        parseGameAsset(fileData, 0, assetToLoad->type, assetToLoad->key, assetToLoad->secondKey, 
                                       &gameMemory, &workingAssetMemory, &options);
                        //free(fileData);
                    } break;
                    case ASSET_TYPE_OBJ: {
                        char *fileData = readEntireTextFile(assetToLoad->path);
                        parseGameAsset(fileData, 0, ASSET_TYPE_OBJ, assetToLoad->key, assetToLoad->secondKey, 
                                       &gameMemory, &workingAssetMemory, &options);
                        free(fileData);

                        loadRendererMesh(&rendererMemory, (loaded_mesh_asset *)workingAssetMemory.base);
                    } break;
                    case ASSET_TYPE_QMM: {
                        char *fileData = 0;
                        //char *fileData = readEntireTextFile(assetToLoad->path);
                        parseGameAsset(fileData, 0, ASSET_TYPE_QMM, assetToLoad->key, assetToLoad->secondKey, 
                                       &gameMemory, &workingAssetMemory, &options);
                        //free(fileData);

                        loadRendererAnimatedMesh(&rendererMemory, (loaded_animated_mesh_asset *)workingAssetMemory.base);
                    } break;
                    case ASSET_TYPE_BMP: {
                        FILE *bmpFile; 
                        fopen_s(&bmpFile, assetToLoad->path, "rb");
                        assert(bmpFile); // TODO(ebuchholz): better error check?

                        fseek(bmpFile, 0, SEEK_END);
                        int fileSize = ftell(bmpFile);
                        fseek(bmpFile, 0, SEEK_SET);

                        char *fileData = (char *)malloc(fileSize);
                        fread(fileData, fileSize, 1, bmpFile);
                        fclose(bmpFile);

                        parseGameAsset(fileData, 0, ASSET_TYPE_BMP, assetToLoad->key, assetToLoad->secondKey, 
                                       &gameMemory, &workingAssetMemory, &options);
                        free(fileData);

                        // load texture onto gpu
                        loadRendererTexture(&rendererMemory, (loaded_texture_asset *)workingAssetMemory.base);
                    } break;
                    case ASSET_TYPE_WAV: {
                        FILE *wavFile; 
                        fopen_s(&wavFile, assetToLoad->path, "rb");
                        assert(wavFile); // TODO(ebuchholz): better error check?

                        fseek(wavFile, 0, SEEK_END);
                        int fileSize = ftell(wavFile);
                        fseek(wavFile, 0, SEEK_SET);

                        char *fileData = (char *)malloc(fileSize);
                        fread(fileData, fileSize, 1, wavFile);
                        fclose(wavFile);

                        parseGameAsset(fileData, 0, ASSET_TYPE_WAV, assetToLoad->key, assetToLoad->secondKey, 
                                       &gameMemory, &workingAssetMemory, &options);
                        free(fileData);
                    } break;
                    case ASSET_TYPE_ATLAS: {
                        char *atlasData = readEntireTextFile(assetToLoad->path);
                        char bitmapPath[MAX_PATH];
                        int letterIndex = 0;
                        for (; assetToLoad->path[letterIndex] != 0; ++letterIndex) {
                            bitmapPath[letterIndex] = assetToLoad->path[letterIndex];
                        }
                        for (; letterIndex >= 0; --letterIndex) {
                            if (bitmapPath[letterIndex] == '.') {
                                ++letterIndex;
                                bitmapPath[letterIndex] = 'b';
                                ++letterIndex;
                                bitmapPath[letterIndex] = 'm';
                                ++letterIndex;
                                bitmapPath[letterIndex] = 'p';
                                ++letterIndex;
                                bitmapPath[letterIndex] = 0;
                                break;
                            }
                        }

                        FILE *bmpFile; 
                        fopen_s(&bmpFile, bitmapPath, "rb");
                        assert(bmpFile); // TODO(ebuchholz): better error check?

                        fseek(bmpFile, 0, SEEK_END);
                        int fileSize = ftell(bmpFile);
                        fseek(bmpFile, 0, SEEK_SET);

                        char *bitmapData = (char *)malloc(fileSize);
                        fread(bitmapData, fileSize, 1, bmpFile);
                        fclose(bmpFile);

                        parseGameAsset(atlasData, bitmapData, ASSET_TYPE_ATLAS, assetToLoad->key, assetToLoad->secondKey, 
                                       &gameMemory, &workingAssetMemory, &options);
                        free(bitmapData);
                        free(atlasData);

                        loadRendererTexture(&rendererMemory, (loaded_texture_asset *)workingAssetMemory.base);
                    } break;
                }
            }

            free(workingAssetMemory.base);

            render_command_list renderCommands = {};
            int memoryCapacity = 1 * 1024 * 1024;
            renderCommands.windowWidth = gameWidth;
            renderCommands.windowHeight = gameHeight;
            renderCommands.memory.base = malloc(memoryCapacity);
            renderCommands.memory.size = 0;
            renderCommands.memory.capacity = memoryCapacity;

            game_input input ={};

            LARGE_INTEGER lastCounter;
            QueryPerformanceCounter(&lastCounter);

            LARGE_INTEGER lastControllerCheckTime = lastCounter;

            bool shouldCheckForNewControllers = true;
            bool shouldLoadFile = false;
            while (programRunning) {
                input.pointerJustDown = false;
                input.aKey.justPressed = false;
                input.sKey.justPressed = false;
                input.dKey.justPressed = false;
                input.fKey.justPressed = false;
                input.gKey.justPressed = false;
                input.hKey.justPressed = false;
                input.jKey.justPressed = false;
                input.kKey.justPressed = false;
                input.wKey.justPressed = false;
                input.eKey.justPressed = false;
                input.tKey.justPressed = false;
                input.yKey.justPressed = false;
                input.uKey.justPressed = false;
                input.upKey.justPressed = false;
                input.downKey.justPressed = false;
                input.leftKey.justPressed = false;
                input.rightKey.justPressed = false;

                processWindowsMessages(window, &input, &renderCommands);

                LARGE_INTEGER controllerCheckTime;
                QueryPerformanceCounter(&controllerCheckTime);

                long long controllerTimeElapsed = controllerCheckTime.QuadPart - lastControllerCheckTime.QuadPart;
                float elapsedControllerSeconds = (float)controllerTimeElapsed / (float)perfCountFrequency;
                if (elapsedControllerSeconds >= 3.0f) {
                    shouldCheckForNewControllers = true;
                    lastControllerCheckTime = controllerCheckTime;
                }
                for (int i = 0; i < 4; ++i) {
                    game_controller_input *controller = &input.controllers[i];
                    if (controller->connected || shouldCheckForNewControllers) {
                        XINPUT_STATE state = {};
                        DWORD result = XInputGetState(i, &state);
                        if (result == ERROR_SUCCESS) {
                            controller->connected = true;
                            updateController(controller, state);
                        }
                        else {
                            controller->connected = false;
                        }
                    }
                }
                if (shouldCheckForNewControllers) {
                    shouldCheckForNewControllers = false;
                }

                renderCommands.windowWidth = gameWidth;
                renderCommands.windowHeight = gameHeight;
                renderCommands.memory.size = 0;
                memset(renderCommands.memory.base, 0, renderCommands.memory.capacity);
                memset(gameMemory.tempMemory, 0, gameMemory.tempMemoryCapacity);

                platform_triggers platformTriggers = {};

                if (shouldLoadFile) {
                    char fileName[MAX_PATH] = {};
                    OPENFILENAMEA openFileDialogInfo = {};
                    openFileDialogInfo.lStructSize = sizeof(OPENFILENAMEA);
                    openFileDialogInfo.lpstrFile = fileName;
                    openFileDialogInfo.nMaxFile = MAX_PATH;
                    GetOpenFileNameA(&openFileDialogInfo);

                    char *fileData = readEntireTextFile(fileName);
                    shouldLoadFile = false;

                    // TODO(ebuchholz): handle file size instead of passing 0
                    if (fileData) {
                        loadFile(&gameMemory, fileData, 0);
                    }
                }

                updateGame(&input, &gameMemory, &renderCommands, &options, &platformTriggers);

                LARGE_INTEGER audioStartTime;
                QueryPerformanceCounter(&audioStartTime);
                float timeFromFrameBeginning = ((float)(audioStartTime.QuadPart - lastCounter.QuadPart) /
                                                (float)perfCountFrequency);

                int numSoundSampleBytes;
                DWORD byteToLock;
                getNumSoundSamples(&soundOutput, &numSoundSampleBytes, &byteToLock, timeFromFrameBeginning);
                if (numSoundSampleBytes > 0) {
                    game_sound_output gameSoundOutput = {};
                    gameSoundOutput.samplesPerSecond = soundOutput.samplesPerSecond;
                    gameSoundOutput.sampleCount = numSoundSampleBytes / soundOutput.bytesPerSample;
                    gameSoundOutput.samples = soundSamples;

                    getGameSoundSamples(&gameMemory, &gameSoundOutput);
                    fillDirectSoundBuffer(&soundOutput, byteToLock, numSoundSampleBytes, &gameSoundOutput);
                }

                //char message[1000];
                //sprintf_s(message, "%f\n", randomFloat());
                //OutputDebugString(message);

                renderFrame(&rendererMemory, &renderCommands);

                if (platformTriggers.triggerFileWindow) {
                    shouldLoadFile = true;
                }
                if (platformTriggers.triggerFileSave) {
                    char fileName[MAX_PATH] = {};
                    OPENFILENAMEA saveFileDialogInfo = {};
                    saveFileDialogInfo.lStructSize = sizeof(OPENFILENAMEA);
                    saveFileDialogInfo.lpstrFile = fileName;
                    saveFileDialogInfo.nMaxFile = MAX_PATH;
                    GetSaveFileNameA(&saveFileDialogInfo);
                    writeEntireFile(fileName, platformTriggers.fileToSaveData, platformTriggers.fileToSaveSize);
                }

                // Sleep for any leftover time
                LARGE_INTEGER workCounter;
                QueryPerformanceCounter(&workCounter);
                long long counterElapsed = workCounter.QuadPart - lastCounter.QuadPart;
                float elapsedMS = (1000.0f * (float)counterElapsed) / (float)perfCountFrequency;
                // use vsync instead
                //if (elapsedMS < targetMSPerFrame) {
                //    DWORD sleepMS = (DWORD)(targetMSPerFrame - elapsedMS);
                //    if (sleepMS > 0) {
                //        Sleep(sleepMS);
                //    }
                //}

                QueryPerformanceCounter(&lastCounter);
            }
        }
    }
    return 0;
}
