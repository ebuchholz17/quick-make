#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "win_platform.h"
#include "../game/quickmake_game.cpp"

#include "opengl_renderer.cpp"
#include "directsound_audio.cpp"

static bool programRunning = false;

static int gameWidth = 1152;
static int gameHeight = 648;
static float targetMSPerFrame = 1000.0f / 60.0f;

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
                    input->forwardButton = keyDown;
                }
                else if (keyCode == 'S') {
                    input->backButton = keyDown;
                }
                else if (keyCode == 'A') {
                    input->leftButton = keyDown;
                }
                else if (keyCode == 'D') {
                    input->rightButton = keyDown;
                }
                else if (keyCode == VK_UP) {
                    if (!input->turnUpButton && keyDown) {
                        input->turnUpButtonJustPressed = true;
                    }
                    input->turnUpButton = keyDown;
                }
                else if (keyCode == VK_DOWN) {
                    if (!input->turnDownButton && keyDown) {
                        input->turnDownButtonJustPressed = true;
                    }
                    input->turnDownButton = keyDown;
                }
                else if (keyCode == VK_LEFT) {
                    if (!input->turnLeftButton && keyDown) {
                        input->turnLeftButtonJustPressed = true;
                    }
                    input->turnLeftButton = keyDown;
                }
                else if (keyCode == VK_RIGHT) {
                    if (!input->turnRightButton && keyDown) {
                        input->turnRightButtonJustPressed = true;
                    }
                    input->turnRightButton = keyDown;
                }
                else if (keyCode == VK_SPACE) {
                    input->upButton = keyDown;
                }
                else if (keyCode == VK_SHIFT) {
                    input->downButton = keyDown;
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
        //dmScreenSettings.dmPelsWidth    = 1920;            
        //dmScreenSettings.dmPelsHeight   = 1080;          
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
            0, //WS_EX_APPWINDOW, //WS_EX_TOPMOST,
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
            workingAssetMemory.capacity = 30 * 1024 * 1024; // 10MB limit for working with asset files?
            workingAssetMemory.base = malloc(workingAssetMemory.capacity);

            for (int i = 0; i < assetList.numAssetsToLoad; ++i) {
                asset_to_load *assetToLoad = assetList.assetsToLoad + i;
                workingAssetMemory.size = 0;
                switch (assetToLoad->type){
                    case ASSET_TYPE_OBJ: {
                        char *fileData = readEntireTextFile(assetToLoad->path);
                        parseGameAsset(fileData, 0, ASSET_TYPE_OBJ, assetToLoad->key, assetToLoad->secondKey, 
                                       &gameMemory, &workingAssetMemory);
                        free(fileData);

                        loadRendererMesh(&rendererMemory, (loaded_mesh_asset *)workingAssetMemory.base);
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
                                       &gameMemory, &workingAssetMemory);
                        free(fileData);

                        // load texture onto gpu
                        loadRendererTexture(&rendererMemory, (loaded_texture_asset *)workingAssetMemory.base);
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
                                       &gameMemory, &workingAssetMemory);
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

            // init sound
            win_sound_output soundOutput = {};
            soundOutput.samplesPerSecond = 48000;
            soundOutput.bytesPerSample = 2;
            soundOutput.secondaryBufferSize = soundOutput.samplesPerSecond * soundOutput.bytesPerSample;
            initDirectSound(window, &soundOutput);
            clearSecondaryBuffer(&soundOutput);
            soundOutput.secondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
            soundOutput.soundValid = false;

            sound_sample *soundSamples = (sound_sample *)malloc(soundOutput.samplesPerSecond * 4);
            assert(soundSamples);

            LARGE_INTEGER lastCounter;
            QueryPerformanceCounter(&lastCounter);

            while (programRunning) {
                input.pointerJustDown = false;
                input.turnUpButtonJustPressed = false;
                input.turnDownButtonJustPressed = false;
                input.turnLeftButtonJustPressed = false;
                input.turnRightButtonJustPressed = false;
                processWindowsMessages(window, &input, &renderCommands);

                renderCommands.windowWidth = gameWidth;
                renderCommands.windowHeight = gameHeight;
                renderCommands.memory.size = 0;
                memset(renderCommands.memory.base, 0, renderCommands.memory.capacity);
                memset(gameMemory.tempMemory, 0, gameMemory.tempMemoryCapacity);
                updateGame(&input, &gameMemory, &renderCommands);

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
                else {
                    int t = 6;
                }

                renderFrame(&rendererMemory, &renderCommands);

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
