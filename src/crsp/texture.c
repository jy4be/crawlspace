#include "crsp.h"

u32 CRSP_texture_size;

struct texture_LL_S {
    pixel_t** data;
    struct texture_LL_S* next;
};

struct texture_LL_S* CRSP_texture_head = NULL;

u32 add_texture(char* path) {
    struct texture_LL_S* end = CRSP_texture_head;
    u32 index;

    while(end) {
        end = end->next;
        index++;
    }
//TODO
    end = (path);

    return index;
}

pixel_t** get_texture(u32 index) {
    struct texture_LL_S* end = CRSP_texture_head;
    for(u32 i = 0; i < index; i++)
        end = end->next;

    return end->data;
}
