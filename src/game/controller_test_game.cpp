#include "controller_test_game.h"

struct status_text_pos {
    float x;
    float y;
};

void showButtonStatus (status_text_pos *currentPos, char *text, bool buttonDown, bool buttonJustPressed, game_assets *assets, sprite_list *spriteList) {
    addText(currentPos->x, currentPos->y, text, assets, TEXTURE_KEY_FONT, spriteList);
    char *status;
    if (buttonDown) {
        status = "1";
    }
    else {
        status = "0";
    }
    addText(currentPos->x + 70, currentPos->y, status, assets, TEXTURE_KEY_FONT, spriteList);
    if (buttonJustPressed) {
        status = "1";
    }
    else {
        status = "0";
    }
    addText(currentPos->x + 80, currentPos->y, status, assets, TEXTURE_KEY_FONT, spriteList);
    currentPos->y += 10;
}

void showFloatValue (status_text_pos *currentPos, char *text, float value, game_assets *assets, sprite_list *spriteList, memory_arena *stringMemory) {
    addText(currentPos->x, currentPos->y, text, assets, TEXTURE_KEY_FONT, spriteList);

    int intValue = (int)(value * 1000);
    addText(currentPos->x + 70, currentPos->y, numToString(intValue, stringMemory), assets, TEXTURE_KEY_FONT, spriteList);
    currentPos->y += 10;
}

void updateControllerTestGame (memory_arena *memory, memory_arena *tempMemory, game_assets *assets, game_input *input, 
                               sprite_list *spriteList)
{
    // memory for dynamically created strings
    memory_arena stringMemory = {};
    stringMemory.capacity = 512 * 1024;
    stringMemory.base = allocateMemorySize(tempMemory, stringMemory.capacity);

    // zoom out a bit
    pushSpriteTransform(spriteList, Vector2(GAME_WIDTH/2.0f, GAME_HEIGHT/2.0f), 0.5f);

    status_text_pos currentPos = {};
    for (int i = 0; i < 4; ++i) {
        game_controller_input controller = input->controllers[i];
        currentPos.x = (float)(-368 + i * 190);
        currentPos.y = (float)-200;
        if (!controller.connected) {
            addText(currentPos.x, currentPos.y, "n/a", assets, TEXTURE_KEY_FONT, spriteList);
        }
        else {
            showButtonStatus(&currentPos, "dpUp:", controller.dPadUp.down, controller.dPadUp.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "dpDown:", controller.dPadDown.down, controller.dPadDown.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "dpLeft:", controller.dPadLeft.down, controller.dPadLeft.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "dpRight:", controller.dPadRight.down, controller.dPadRight.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "start:", controller.start.down, controller.start.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "back:", controller.back.down, controller.back.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "lStick:", controller.leftStick.down, controller.leftStick.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "rStick:", controller.rightStick.down, controller.rightStick.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "lBump:", controller.leftBumper.down, controller.leftBumper.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "rBump:", controller.rightBumper.down, controller.rightBumper.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "A:", controller.aButton.down, controller.aButton.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "B:", controller.bButton.down, controller.bButton.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "X:", controller.xButton.down, controller.xButton.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "Y:", controller.yButton.down, controller.yButton.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "lTrig:", controller.leftTriggerButton.down, controller.leftTriggerButton.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "rTrig:", controller.rightTriggerButton.down, controller.rightTriggerButton.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "lStickU:", controller.leftStickUp.down, controller.leftStickUp.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "lStickD:", controller.leftStickDown.down, controller.leftStickDown.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "lStickL:", controller.leftStickLeft.down, controller.leftStickLeft.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "lStickR:", controller.leftStickRight.down, controller.leftStickRight.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "rStickU:", controller.rightStickUp.down, controller.rightStickUp.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "rStickD:", controller.rightStickDown.down, controller.rightStickDown.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "rStickL:", controller.rightStickLeft.down, controller.rightStickLeft.justPressed, assets, spriteList);
            showButtonStatus(&currentPos, "rStickR:", controller.rightStickRight.down, controller.rightStickRight.justPressed, assets, spriteList);

            showFloatValue(&currentPos, "lStickX:", controller.leftStickX, assets, spriteList, &stringMemory);
            showFloatValue(&currentPos, "lStickY:", controller.leftStickY, assets, spriteList, &stringMemory);
            showFloatValue(&currentPos, "rStickX:", controller.rightStickX, assets, spriteList, &stringMemory);
            showFloatValue(&currentPos, "rStickY:", controller.rightStickY, assets, spriteList, &stringMemory);
            showFloatValue(&currentPos, "lTrigVal:", controller.leftTrigger, assets, spriteList, &stringMemory);
            showFloatValue(&currentPos, "rTrigVal:", controller.rightTrigger, assets, spriteList, &stringMemory);
        }

    }
    //addText(-184, -100, appendString("score: ", numToString(17, &stringMemory), &stringMemory), assets, TEXTURE_KEY_FONT, spriteList);

    popSpriteMatrix(spriteList);
}
//struct game_controller_input {
//    bool connected;
//
//    input_key dPadUp;
//    input_key dPadDown;
//    input_key dPadLeft;
//    input_key dPadRight;
//
//    input_key start;
//    input_key back;
//
//    input_key leftStick;
//    input_key rightStick;
//
//    input_key leftBumper;
//    input_key rightBumper;
//
//    input_key aButton;
//    input_key bButton;
//    input_key xButton;
//    input_key yButton;
//
//    float leftTrigger;
//    input_key leftTriggerButton;
//    float rightTrigger;
//    input_key rightTriggerButton;
//
//    float leftStickX;
//    float leftStickY;
//    float rightStickX;
//    float rightStickY;
//
//    input_key leftStickUp;
//    input_key leftStickDown;
//    input_key leftStickLeft;
//    input_key leftStickRight;
//
//    input_key rightStickUp;
//    input_key rightStickDown;
//    input_key rightStickLeft;
//    input_key rightStickRight;
//};
