#include "crsp.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

tile_t t0 = {0,3,3};
tile_t t1 = {1,3,3};
tile_t t2 = {2,3,3};
tile_t t3 = {3,3,3};



int main() {
    CRSP_texture_size = 64;
    tile_t map_data[] =
    {t1 ,t2,t1,t2,t1,t2,t1,t1,
     t1,t0,t0,t0,t0,t0,t0,t1,
     t1,t0,t0,t0,t0,t0,t0,t2,
     t3,t0,t0,t0,t1,t0,t0,t1,
     t2,t0,t0,t3,t2,t0,t0,t3,
     t2,t0,t0,t0,t0,t0,t0,t1,
     t1,t0,t0,t0,t0,t0,t0,t1,
     t1,t1,t1,t2,t1,t3,t1,t1};
    screen(320 * 4, 180 * 4, "TEST");

    set_map(8, 8, map_data);
    move_player((v2f_t) {.x = 2,.y = 2}, (v2f_t) {.x = -1,.y = 0});
    set_fov(.66);
    
    add_dev_texture(0);
    add_dev_texture(1);
    add_dev_texture(2);
    add_dev_texture(3);
    add_dev_texture(4);


    add_sprite((sprite_t) {
            .pos = (v2f_t) {.x = 2.5, .y =2.5},
            .tex_id = 4});
    
    SDL_Event event;

    bool running = true;
    bool keys[4] = {0,0,0,0};
    while(running){
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if(event.key.keysym.scancode == SDL_SCANCODE_W)
                    keys[0] = true;
                if(event.key.keysym.scancode == SDL_SCANCODE_S)
                    keys[1] = true;
                if(event.key.keysym.scancode == SDL_SCANCODE_A)
                    keys[2] = true;
                if(event.key.keysym.scancode == SDL_SCANCODE_D)
                    keys[3] = true;
                break;

            case SDL_KEYUP:
                if(event.key.keysym.scancode == SDL_SCANCODE_W)
                    keys[0] = false;
                if(event.key.keysym.scancode == SDL_SCANCODE_S)
                    keys[1] = false;
                if(event.key.keysym.scancode == SDL_SCANCODE_A)
                    keys[2] = false;
                if(event.key.keysym.scancode == SDL_SCANCODE_D)
                    keys[3] = false;
                break;
            }
        }


        if(keys[0]) {
            CRSP_player.pos.x += CRSP_player.dir.x * 0.1;
            CRSP_player.pos.y += CRSP_player.dir.y * 0.1;
        }
        if(keys[1]) {
            CRSP_player.pos.x -= CRSP_player.dir.x * 0.1;
            CRSP_player.pos.y -= CRSP_player.dir.y * 0.1;
        }
        if(keys[2]) {
            CRSP_player.dir = v2f_rotate(CRSP_player.dir, -.1);
            CRSP_player.plane = v2f_rotate(CRSP_player.plane, -.1);
        }
        if(keys[3]) {
            CRSP_player.dir = v2f_rotate(CRSP_player.dir, .1);
            CRSP_player.plane = v2f_rotate(CRSP_player.plane, .1);
        }

        render_map();
        present();
    }

    cleanup();

}
