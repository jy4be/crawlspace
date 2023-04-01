#ifndef CRSP_H
#define CRSP_H

#include <stdint.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_WIDTH  (320 * 4)
#define MAX_HEIGHT (180 * 4) 
#define MAX_SPRITES 256

#define WINDOW_FLAGS 0
#define RENDER_FLAGS SDL_RENDERER_ACCELERATED
#define DYN_ASSERT(asserted, ...) if(!(asserted)) {fprintf(stderr, __VA_ARGS__); exit(-1);}

typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t   i8;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef double   f64;
typedef float    f32;

typedef union __attribute__((packed)){
    struct __attribute__((packed)){
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    uint8_t value[3];
} pixel_t;

typedef union {
    struct __attribute__((packed)){
        double x;
        double y;
    };
    double v[2];
} v2f_t;

typedef union {
    struct __attribute__((packed)){
        int32_t x;
        int32_t y;
    };
    int32_t v[2];
} v2i_t;

typedef struct Sprite_S {
    v2f_t pos;
    u32 tex_id;
    double dist;
} sprite_t;

typedef struct Tile_s {
    u32 wall_id;
    u32 floor_id;
    u32 ceiling_id;
} tile_t;

typedef struct Window_Data_S {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  texture;
    uint32_t width;
    uint32_t height;
    char* name;
    uint32_t window_flags;
    uint32_t render_flags;
    bool is_running;
} window_data_t;

extern window_data_t CRSP_window_data;

typedef union Screen_U {
    pixel_t pixels[MAX_WIDTH * MAX_HEIGHT];
    uint8_t bytes[MAX_WIDTH * MAX_HEIGHT * sizeof(pixel_t)];
} screen_t;

extern screen_t CRSP_screen;

typedef struct Player_S {
    v2f_t pos;
    v2f_t dir;
    v2f_t plane;
} player_t;

extern player_t CRSP_player;

typedef struct Map_S {
    u32 width;
    u32 height;
    tile_t *tiles;
} map_t;
extern map_t CRSP_map;

extern u32 CRSP_texture_size;


extern sprite_t* CRSP_sprites[MAX_SPRITES];
//extern u32 CRSP_sprite_order[MAX_SPRITES];
extern u32 CRSP_sprites_amount;

v2f_t v2f_rotate(v2f_t v, double angle);

void screen(u32 width, u32 height, char* name);
void cleanup();
void present();

void render_map();
void set_map(u32 width, u32 height, tile_t* tiles);
void move_player(v2f_t new_pos, v2f_t new_dir);
void set_fov(f64 new_fov);

u32 add_sprite(sprite_t sprite_data);
void remove_sprite(u32 index);
void sort_sprites(sprite_t* sprites[MAX_SPRITES], uint32_t amount);

void set_texture_size(u32 size);
u32 add_texture(char* path);
pixel_t* get_texture(u32 index);
u32 add_dev_texture(u8 dev_index);
void destroy_textures();
#endif//CRSP_H
