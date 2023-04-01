#include "crsp.h"

player_t CRSP_player;

void move_player(v2f_t new_pos, v2f_t new_dir) {
    CRSP_player.pos = new_pos;
    CRSP_player.dir = new_dir;
}

void set_fov(f64 new_fov) {
    CRSP_player.plane.y = new_fov;
}


