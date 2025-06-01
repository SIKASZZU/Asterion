#include <SDL2/SDL.h>
#include <vector>


enum AbilityType {
    MELEE,
    ARROW
};

struct Ability {
    AbilityType type;
    int range;
    int damage;
    int speed;
    double direction;
    float x;
    float y;
    int length; // for arrows e projectiles
    Uint32 spawn_time;
};

extern std::vector<Ability> active_abilities;  // extern seest muidu compiler fucking

void update_abilities(SDL_Renderer* renderer, struct Offset& offset);
void use_melee(int x, int y, double direction);
void use_arrow(int x, int y, double direction);
