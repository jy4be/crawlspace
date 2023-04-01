#include <stdio.h>
#include "crsp.h"
#include <errno.h>
#include <SDL2/SDL_image.h>
#include <math.h>

u32 CRSP_texture_size;

struct texture_LL_S {
    pixel_t* data;
    struct texture_LL_S* next;
};

struct texture_LL_S* CRSP_texture_head = NULL;

u32 add_texture(char* path) {
    struct texture_LL_S* end = CRSP_texture_head;
    u32 index = 0;

    while(end) {
        end = end->next;
        index++;
    }

    return index;
}

u32 add_dev_texture(u8 dev_index) {
        pixel_t *data = malloc(CRSP_texture_size * CRSP_texture_size * 3);

   for( uint32_t tex_coord = 0; tex_coord <CRSP_texture_size* CRSP_texture_size; tex_coord++){
        uint32_t grad256 = ((256 * tex_coord)  / (CRSP_texture_size * CRSP_texture_size));
        uint32_t circle =
            ((CRSP_texture_size / 2 - tex_coord / CRSP_texture_size) * (CRSP_texture_size / 2 - tex_coord / CRSP_texture_size)) +
            ((CRSP_texture_size / 2 - tex_coord % CRSP_texture_size) * (CRSP_texture_size / 2 - tex_coord % CRSP_texture_size));

        switch(dev_index) {
        case 0:
            data[tex_coord] = 
            circle < CRSP_texture_size * 4  ?
                (pixel_t) {.r = 255, .g = 0, .b = 0} :
                (pixel_t) {0};
            break;
        case 1:
            data[tex_coord] =
            (tex_coord / CRSP_texture_size) -
            (tex_coord % CRSP_texture_size) == 0 ?
                (pixel_t) {.r = 255, .g = 0, .b = 0} :
                (pixel_t) {0};
            break;
        case 2:
            data[tex_coord] =
            (pixel_t) {.r = grad256, .g = grad256, .b = grad256};
            break;
        case 3:
            data[tex_coord] =
            (((tex_coord / CRSP_texture_size) % 16 && (tex_coord % CRSP_texture_size) % 16)) ?
                (pixel_t) {.r = 255, .g = 0, .b = 0} :
                (pixel_t) {0};
            break;
        case 4:
            data[tex_coord] =
            abs((int)(tex_coord % CRSP_texture_size) - (int)CRSP_texture_size/2) < CRSP_texture_size/4 ?
                (pixel_t) {.r = 0, .g = 128 + tex_coord % 64, .b = 128} :
                (pixel_t) {0};
            break;
        }
    }

    
    struct texture_LL_S* texture = malloc(sizeof(struct texture_LL_S));
    texture->data = data;
    texture->next = NULL;

    if (!CRSP_texture_head){
        CRSP_texture_head = texture;
        return 0;
    }

    struct texture_LL_S* end = CRSP_texture_head;

    u32 index = 1;

    while(end->next) {
        end = end->next;
        index++;
    }
    end->next = texture;

    return index;
}

pixel_t* get_texture(u32 index) {
    struct texture_LL_S* end = CRSP_texture_head;
    for(u32 i = 0; i < index; i++){
        end = end->next;
    }
    //printf("%d\n", end->data[0].r);

    return end->data;
}

void destroy_textures(){
    struct texture_LL_S* next = CRSP_texture_head;
    struct texture_LL_S* old;

    while(next) {
        old = next;
        next = next->next;
        free(old->data);
        free(old);
    }
}
