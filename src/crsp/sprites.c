#include "crsp.h"

sprite_handle_t* CRSP_sprite_head = NULL;

struct Sprite_Handle_S {
    sprite_t sprite;
    struct Sprite_Handle_S *next;
};


sprite_handle_t* add_sprite(sprite_t sprite_data){
    sprite_handle_t* handle = malloc(sizeof(sprite_handle_t));
    handle->sprite = sprite_data;
    handle->next = NULL;

    sprite_handle_t* end = CRSP_sprite_head;

    while(end) end = end->next;
    end = handle;

    return end;
}

void remove_sprite(sprite_handle_t* handle) {
    sprite_handle_t* prior = CRSP_sprite_head;

    while(prior->next != handle) prior = prior->next;

    prior->next = handle->next;
    free(handle);
}

void set_sprite(sprite_handle_t* handle, sprite_t sprite) {
    handle->sprite = sprite;
}

sprite_t get_sprite(sprite_handle_t* handle) {
    return handle->sprite;
}
