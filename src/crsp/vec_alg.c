#include "crsp.h"
#include <math.h>

v2f_t v2f_rotate(v2f_t v, double angle){
    return (v2f_t) {
        .x = v.x * cos(-angle) - v.y * sin(-angle),
        .y = v.x * sin(-angle) + v.y * cos(-angle)
    };
}
