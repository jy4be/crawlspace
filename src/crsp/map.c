#include "crsp.h"

map_t CRSP_map;
void set_map(u32 width, u32 height, tile_t *tiles) {
    CRSP_map.width = width;
    CRSP_map.height = height;
    CRSP_map.tiles = tiles;
}
