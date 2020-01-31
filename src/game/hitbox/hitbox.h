#ifndef HITBOX_H
#define HITBOX_H

#define MAX_NUM_HITBOXES 20
#define MAX_NUM_HURTBOXES 20

struct character_frame_data {
    char *frameKey;
    int xOffset;
    int yOffset;
    int duration;
    int numHitboxes;
    rectangle hitboxes[MAX_NUM_HITBOXES];
    int numHurtboxes;
    rectangle hurtboxes[MAX_NUM_HURTBOXES];
};

struct character_animation_data {
    character_frame_data frames[50];
    int numFrames;
};

#endif
