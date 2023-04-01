#include "crsp.h"

u32 CRSP_sprites_amount = 0;
sprite_t* CRSP_sprites[MAX_SPRITES] = {0};

u32 add_sprite(sprite_t sprite_data){
    sprite_t* sprite = malloc(sizeof(sprite_t));
    *sprite = sprite_data;

    int index = 0;
    while (CRSP_sprites[index]) index++;

    CRSP_sprites[index] = sprite;

    CRSP_sprites_amount++;

    return index;
}

void remove_sprite(u32 index) {
    free(CRSP_sprites[index]);
    CRSP_sprites[index] = NULL;

    CRSP_sprites_amount--;
}

static inline int sprite_cmp(const void* a, const void* b) {
    sprite_t* e1 = (sprite_t*) a;
    sprite_t* e2 = (sprite_t*) b;

    if (e1->dist > e2->dist) return 1;
    if (e2->dist > e1->dist) return -1;
    else return 0;
}

void sort_sprites(sprite_t* sprites[MAX_SPRITES], uint32_t amount) {
    qsort(sprites, amount, sizeof(sprite_t*), sprite_cmp);
}

