#include "crsp.h"

#define sign(x) (x) < 0 ? -1 : 1

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
        double camera_x = 2 * column / (double) (w) - (double) 1; 
        
        //The direction the ray will travel
        v2f_t ray_dir = {
            .x = dir.x + plane.x * camera_x,
            .y = dir.y + plane.y * camera_x};

        //The players position in grid coordinates
        v2i_t grid_pos = {
            .x = (int) pos.x,
            .y = (int) pos.y};

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

            if(CRSP_map.tiles[grid_pos.x][grid_pos.y].wall_id > 0)
                hit = true;
        }

        double perp_wall_dist =
            (side == HORIZONTAL) ?
                (side_dist.x - delta_dist.x):
                (side_dist.y - delta_dist.y);

        z_buffer[column] = perp_wall_dist;

        int32_t line_height = (int) (h / perp_wall_dist);

        int32_t line_top, line_bottom;
        if(perp_wall_dist <= 1) {
            line_top    = 0;
            line_bottom = h -1;
        }
        else {
            line_top    = -line_height/2 + h/2;
            line_bottom =  line_height/2 + h/2;
        }

        uint8_t tex_index = CRSP_map.tiles[grid_pos.x][grid_pos.y].wall_id -1;

        double wall_x = 
            (side == HORIZONTAL) ?
                pos.y + perp_wall_dist * ray_dir.y : //Horizontal wall
                pos.x + perp_wall_dist * ray_dir.x;  //Vertical wall
        wall_x -= floor(wall_x);

        int tex_x = (int) (wall_x * (double)tex_size);

        //Texture Wrap-around
        if(side == HORIZONTAL && ray_dir.x > 0) tex_x = tex_size - tex_x -1;
        if(side == VERTICAL   && ray_dir.y < 0) tex_x = tex_size - tex_x -1;

        double tex_step = 1.0 * tex_size / line_height;
        double tex_pos = (line_top - h/ 2 + line_height / 2) * tex_step;

        //Draw vertical Line
        for (int32_t h = line_top; h < line_bottom; h++) {
            int tex_y = (int) tex_pos & (tex_size-1);
            tex_pos += tex_step;

            pixel_t color = get_texture(tex_index)[tex_y][tex_x];
            if(side == VERTICAL) {
                color.r /= 2;
                color.g /= 2;
                color.b /= 2;
            }  
            CRSP_screen.pixels[h * w + column] = color;
        }

        //Floor and Ceiling

        v2f_t floor_ray = {
            .x = (double)(2*column) / (double)w -1};

        for (int32_t h = 0; h < (w - line_height)/2; h++){
            floor_ray.y = ((double)w/2) / ((double)w/2 - (line_bottom + h));

            v2f_t floor_coord = {
                .x = pos.x - floor_ray.y * (dir.x + plane.x * floor_ray.x),
                .y = pos.y - floor_ray.y * (dir.y + plane.y * floor_ray.x)};

            v2i_t tex_coord = {
                .x = tex_size * (floor_coord.x - floor(floor_coord.x)),
                .y = tex_size * (floor_coord.y - floor(floor_coord.y))};

            pixel_t color = get_texture(3)[tex_coord.y][tex_coord.x];
            CRSP_screen.pixels[(h + line_bottom) * w + column] = color; //floor
            CRSP_screen.pixels[(line_top - h) * w + column] = color;    //ceiling
        }
    }


    //SPRITE CAST
    for (uint32_t i = 0; i < NUM_SPRITES; i++) {
        sprite_order[i] = i;
        sprite_dist[i] = ((player.pos.x - sprites[i].pos.x) * (player.pos.x - sprites[i].pos.x) +
                          (player.pos.y - sprites[i].pos.y) * (player.pos.y - sprites[i].pos.y));
    }
    sort_sprites(sprite_order, sprite_dist, NUM_SPRITES);

    //Project and draw Sprites
    for(uint32_t i = 0; i < NUM_SPRITES; i++) {
        v2f_t rel_sprite = {
            .x = sprites[sprite_order[i]].pos.x - pos.x,
            .y = sprites[sprite_order[i]].pos.y - pos.y};
        
        double inv_det = 1.0 / (plane.x * dir.y - 
                                dir.x * plane.y);

        v2f_t trans_pos = {
            .x = inv_det * (dir.y * rel_sprite.x - dir.x * rel_sprite.y),
            .y = inv_det * (-plane.y * rel_sprite.x + plane.x * rel_sprite.y)};

        int32_t sprite_screen_x = (int) ((w/2) * (1 + trans_pos.x / trans_pos.y));

        int32_t sprite_height   = abs((int)(h / trans_pos.y));
        int32_t draw_start_y = -sprite_height / 2 + h / 2;
        if(draw_start_y < 0) draw_start_y = 0;
        int32_t draw_end_y = sprite_height / 2 + h / 2;
        if(draw_end_y >= h) draw_end_y = h - 1;

        int32_t sprite_width = abs((int) (h / trans_pos.y));
        int32_t draw_start_x = -sprite_width / 2 + sprite_screen_x;
        if(draw_start_x < 0) draw_start_x = 0;
        int32_t draw_end_x = sprite_width / 2 + sprite_screen_x;
        if(draw_end_x >= w) draw_end_x = w - 1;

        for (int32_t column = draw_start_x; column < draw_end_x; column++) {
            int tex_x = (int) (256 * (column - (-sprite_width / 2 + sprite_screen_x)) * 
                                tex_size / sprite_width) / 256;

            if(trans_pos.y > 0 && column > 0 && column < w && trans_pos.y < z_buffer[column]) {
                for(int32_t y = draw_start_y; y < draw_end_y; y++) {
                    int32_t d = (y) * 256 - h * 128 + sprite_height * 128;
                    int32_t tex_y = ((d * tex_size) / sprite_height) / 256;
                    pixel_t color = 
                        get_texture(sprites[sprite_order[i]].tex_index)[tex_y][tex_x];
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
