#include "crsp.h"

#define sign(x) (x) < 0 ? -1 : 1

double CRSP_z_buffer[MAX_WIDTH] = {0};

void render_map() {
    i32 w = CRSP_window_data.width;
    i32 h = CRSP_window_data.height;
    v2f_t dir = CRSP_player.dir;
    v2f_t pos = CRSP_player.pos;
    v2f_t plane = CRSP_player.plane;
    u32 tex_size = CRSP_texture_size;
    //Wall casting
    for (int16_t column = 0; column < w; column++) {
        //Normalise column to values between -1 to 1
        f64 camera_x = 2 * column / (f64)(w) - (f64) 1; 
        
        //The direction the ray will travel
        v2f_t ray_dir = {
            .x = dir.x + plane.x * camera_x,
            .y = dir.y + plane.y * camera_x};

        //The players position in grid coordinates
        v2i_t grid_pos = {
            .x = (i32) pos.x,
            .y = (i32) pos.y};

        //The distance from one x or y-side to the next in the rays direction
        v2f_t delta_dist = {
            .x = (ray_dir.x == 0) ? INFINITY : fabs(1 / ray_dir.x),
            .y = (ray_dir.y == 0) ? INFINITY : fabs(1 / ray_dir.y)};

        //The direction to do x or y steps in grid coordinates
        v2i_t step = {
            .x = sign(ray_dir.x),
            .y = sign(ray_dir.y)};

        //The distance to the first side on the left or right(x) and top or bottom(y)
        v2f_t side_dist = {
            .x = delta_dist.x * (
                ray_dir.x < 0 ? 
                    (pos.x - grid_pos.x) : 
                    (grid_pos.x + 1.0 - pos.x)),
            .y = delta_dist.y * (
                ray_dir.y < 0 ?
                    (pos.y - grid_pos.y) :
                    (grid_pos.y + 1.0 - pos.y))};

        //Raycast towards next wall
        enum {HORIZONTAL, VERTICAL} side;
        bool hit = false;

        while(!hit)
        {
            if (side_dist.x < side_dist.y){
                side_dist.x += delta_dist.x;
                grid_pos.x += step.x;
                side = HORIZONTAL;
            }
            else {
                side_dist.y += delta_dist.y;
                grid_pos.y += step.y;
                side = VERTICAL;
            }

            if(CRSP_map.tiles[grid_pos.x * CRSP_map.width + grid_pos.y].wall_id > 0)
                hit = true;
        }

        f64 perp_wall_dist =
            (side == HORIZONTAL) ?
                (side_dist.x - delta_dist.x):
                (side_dist.y - delta_dist.y);

        CRSP_z_buffer[column] = perp_wall_dist;

        i32 line_height = (i32) (h / perp_wall_dist);

        i32 line_top, line_bottom;
        if(perp_wall_dist <= 1) {
            line_top    = 0;
            line_bottom = h -1;
        }
        else {
            line_top    = -line_height/2 + h/2;
            line_bottom =  line_height/2 + h/2;
        }

        u8 tex_index = CRSP_map.tiles[grid_pos.x * CRSP_map.width + grid_pos.y].wall_id -1;

        f64 wall_x = 
            (side == HORIZONTAL) ?
                pos.y + perp_wall_dist * ray_dir.y : //Horizontal wall
                pos.x + perp_wall_dist * ray_dir.x;  //Vertical wall
        wall_x -= floor(wall_x);

        i32 tex_x = (i32) (wall_x * (f64)tex_size);

        //Texture Wrap-around
        if(side == HORIZONTAL && ray_dir.x > 0) tex_x = tex_size - tex_x -1;
        if(side == VERTICAL   && ray_dir.y < 0) tex_x = tex_size - tex_x -1;

        f64 tex_step = 1.0 * tex_size / line_height;
        f64 tex_pos = (line_top - h/ 2 + line_height / 2) * tex_step;

        //Draw vertical Line
        for (i32 y = line_top; y < line_bottom; y++) {
            i32 tex_y = (i32) tex_pos & (tex_size-1);
            tex_pos += tex_step;

            
            pixel_t color = get_texture(tex_index)[tex_y * CRSP_texture_size + tex_x];
            if(side == VERTICAL) {
                color.r /= 2;
                color.g /= 2;
                color.b /= 2;
            }  
            CRSP_screen.pixels[y * w + column] = color;
        }

        //Floor and Ceiling

        v2f_t floor_ray = {
            .x = (f64)(2*column) / (f64)w -1};

        for (int32_t y = 0; y < (h - line_height)/2; y++){
            floor_ray.y = ((f64)h/2) / ((f64)h/2 - (line_bottom + y));

            v2f_t floor_coord = {
                .x = pos.x - floor_ray.y * (dir.x + plane.x * floor_ray.x),
                .y = pos.y - floor_ray.y * (dir.y + plane.y * floor_ray.x)};

            v2i_t tex_coord = {
                .x = tex_size * (floor_coord.x - floor(floor_coord.x)),
                .y = tex_size * (floor_coord.y - floor(floor_coord.y))};

            pixel_t color = get_texture(3)[tex_coord.y * CRSP_texture_size + tex_coord.x];
            CRSP_screen.pixels[(y + line_bottom) * w + column] = color; //floor
            CRSP_screen.pixels[(line_top - y) * w + column] = color;    //ceiling
        }
    }

    //SPRITE CAST
    for (u32 i = 0; i < CRSP_sprites_amount; i++) {
        if(!CRSP_sprites[i]) continue;
        CRSP_sprites[i]->dist = ((pos.x - CRSP_sprites[i]->pos.x) * (pos.x - CRSP_sprites[i]->pos.x) +
                          (pos.y - CRSP_sprites[i]->pos.y) * (pos.y - CRSP_sprites[i]->pos.y));
    }
    sort_sprites(CRSP_sprites, CRSP_sprites_amount);

    //Project and draw Sprites
    for(u32 i = 0; i < CRSP_sprites_amount; i++) {
        if(!CRSP_sprites[i]) continue;
        v2f_t rel_sprite = {
            .x = CRSP_sprites[i]->pos.x - pos.x,
            .y = CRSP_sprites[i]->pos.y - pos.y};
        
        f64 inv_det = 1.0 / (plane.x * dir.y - 
                                dir.x * plane.y);

        v2f_t trans_pos = {
            .x = inv_det * (dir.y * rel_sprite.x - dir.x * rel_sprite.y),
            .y = inv_det * (-plane.y * rel_sprite.x + plane.x * rel_sprite.y)};

        i32 sprite_screen_x = (i32) ((w/2) * (1 + trans_pos.x / trans_pos.y));

        i32 sprite_height   = abs((i32)(h / trans_pos.y));
        i32 draw_start_y = -sprite_height / 2 + h / 2;
        if(draw_start_y < 0) draw_start_y = 0;
        i32 draw_end_y = sprite_height / 2 + h / 2;
        if(draw_end_y >= h) draw_end_y = h - 1;

        i32 sprite_width = abs((i32) (h / trans_pos.y));
        i32 draw_start_x = -sprite_width / 2 + sprite_screen_x;
        if(draw_start_x < 0) draw_start_x = 0;
        i32 draw_end_x = sprite_width / 2 + sprite_screen_x;
        if(draw_end_x >= w) draw_end_x = w - 1;

        for (i32 column = draw_start_x; column < draw_end_x; column++) {
            i32 tex_x = (i32) (256 * (column - (-sprite_width / 2 + sprite_screen_x)) * 
                                tex_size / sprite_width) / 256;
            if(trans_pos.y > 0 && column > 0 && column < w && trans_pos.y < CRSP_z_buffer[column]) {
                for(i32 y = draw_start_y; y < draw_end_y; y++) {
                    i32 d = (y) * 256 - h * 128 + sprite_height * 128;
                    i32 tex_y = ((d * tex_size) / sprite_height) / 256;
                    pixel_t color = get_texture(CRSP_sprites[i]->tex_id)[tex_y * CRSP_texture_size + tex_x];
                    if(!(color.r == 0 && color.g == 0 && color.b == 0))
                        CRSP_screen.pixels[y * w + column] = //color;
                            (pixel_t) {
                                .r = CRSP_screen.pixels[y * w + column].r / 2 + color.r / 2,
                                .g = CRSP_screen.pixels[y * w + column].g / 2 + color.g / 2,
                                .b = CRSP_screen.pixels[y * w + column].b / 2 + color.b / 2};
                }
            }

        }
    }
}
