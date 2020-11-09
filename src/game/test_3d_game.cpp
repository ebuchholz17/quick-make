#include "test_3D_game.h"

void debugCameraMovement (debug_camera *debugCamera, game_input *input) {
    const float CAMERA_SPEED = 3.0f;
    const float CAMERA_TURN_SPEED = 1.0f;

    // Position
    vector3 moveVector = {};
    if (input->upKey.down || input->wKey.down) {
        moveVector.z -= CAMERA_SPEED * DELTA_TIME;
    }
    if (input->downKey.down || input->sKey.down) {
        moveVector.z += CAMERA_SPEED * DELTA_TIME;
    }
    if (input->leftKey.down || input->aKey.down) {
        moveVector.x -= CAMERA_SPEED * DELTA_TIME;
    }
    if (input->rightKey.down || input->dKey.down) {
        moveVector.x += CAMERA_SPEED * DELTA_TIME;
    }

    // Rotation
    if (input->pointerJustDown) {
        debugCamera->lastPointerX = input->pointerX;
        debugCamera->lastPointerY = input->pointerY;
    }
    if (input->pointerDown) {
        int pointerDX = input->pointerX - debugCamera->lastPointerX;
        int pointerDY = input->pointerY - debugCamera->lastPointerY;

        float yaw = (float)pointerDX * 0.25f;
        float pitch = (float)pointerDY * 0.25f;

        debugCamera->rotation = quaternionFromAxisAngle(Vector3(0, 1, 0), -yaw * DELTA_TIME) * (debugCamera->rotation);
        debugCamera->rotation = (debugCamera->rotation) * quaternionFromAxisAngle(Vector3(1, 0, 0), -pitch * DELTA_TIME);

        debugCamera->lastPointerX = input->pointerX;
        debugCamera->lastPointerY = input->pointerY;
    }
    // Move in the direction of the current rotation
    moveVector = rotateVectorByQuaternion(moveVector, debugCamera->rotation);
    debugCamera->pos += moveVector;
}

void update3DGame (memory_arena *memory, memory_arena *tempMemory, game_assets *assets, game_input *input, 
                   test_3D_game *test3DGame, sprite_list *spriteList, render_command_list * renderCommands)
{
    if (!test3DGame->initialized) {
        test3DGame->initialized =true;
        test3DGame->cubeT = 0.0f;
        debug_camera *debugCamera = &test3DGame->debugCamera;
        debugCamera->pos = {};
        debugCamera->pos.x = 3.0f;
        debugCamera->pos.y = 3.0f;
        debugCamera->pos.z = 4.0f;
        debugCamera->rotation = 
            quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 40.0f * (PI / 180.0f)) *
            quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -33.0f * (PI / 180.0f));
        debugCamera->lastPointerX = 0;
        debugCamera->lastPointerY = 0;
    }

    // memory for dynamically created strings
    memory_arena stringMemory = {};
    stringMemory.capacity = 512 * 1024;
    stringMemory.base = allocateMemorySize(tempMemory, stringMemory.capacity);

    debugCameraMovement(&test3DGame->debugCamera, input);

    float screenWidth = (float)renderCommands->windowWidth;
    float screenHeight = (float)renderCommands->windowHeight;
    matrix4x4 projMatrix = createPerspectiveMatrix(0.1f, 1000.0f, (screenWidth / screenHeight), 80.0f);
    matrix4x4 viewMatrix = createViewMatrix(test3DGame->debugCamera.rotation, 
                                            test3DGame->debugCamera.pos.x,
                                            test3DGame->debugCamera.pos.y,
                                            test3DGame->debugCamera.pos.z);

    render_command_set_camera *setCameraCommand = 
        (render_command_set_camera *)pushRenderCommand(renderCommands,
                                                       RENDER_COMMAND_SET_CAMERA,
                                                       sizeof(render_command_set_camera));
    setCameraCommand->viewMatrix = viewMatrix;
    setCameraCommand->projMatrix = projMatrix;

    test3DGame->cubeT += DELTA_TIME;

    matrix4x4 transform = translationMatrix(0.0f, 0.0f, -6.0f) *
                          rotationMatrixFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), test3DGame->cubeT);

    drawModel("simplebunny", "snake_pattern", transform, renderCommands, assets);
}
