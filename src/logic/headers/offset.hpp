#pragma once
/* camera offset */
struct Offset {
    float x;
    float y;
};
extern Offset offset;

void update_offset(struct PlayerData& player);