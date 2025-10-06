#pragma once
/* camera offset */
struct Offset {
    float x;
    float y;
};
extern Offset offset;

void update_offset(struct Offset& offset, struct PlayerData& player);
