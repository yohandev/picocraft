/**
 * [block.h] A single tile in the world
 */
#pragma once

#include "num.h"

typedef struct {
    u8 type: 5;
    u8 state: 3;
} Block;

