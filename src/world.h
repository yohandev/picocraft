/**
 * [world.h] World and chunks in Picocraft.
 */
#pragma once

#include "num.h"
#include "block.h"

// Size(in blocks) of a chunk
#define CHUNK_SIZE 16
#define CHUNK_VOLUME (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

// Size of chunk slot allocator
#define MAX_CHUNKS 32

// A cubic segment of a Picocraft `World`
typedef struct {
    Block blocks[CHUNK_VOLUME]; // Blocks in the chunks
    ivec3 pos;                  // World position of the chunk's corner
} Chunk;

typedef struct {
    Chunk chunks[MAX_CHUNKS];   // Heap for `Chunk`s
    enum {
        SLOT_UNINIT,            // Free slot
        SLOT_OCCUPIED,          // Initialized and in-use
        SLOT_STALE,             // Initialized but free for use
    } slots[MAX_CHUNKS];        // Tracks state of slots in `chunks`
} World;

// Get the aligned position of the chunk that contains this block at `pos`
inline ivec3 chunk_pos(ivec3 pos) {
    // 0b[........|........|........|.....][...]
    //           Truncate bits < CHUNK_SIZE ^
    return (ivec3){
        .x = pos.x & !(CHUNK_SIZE - 1),
        .y = pos.y & !(CHUNK_SIZE - 1),
        .z = pos.z & !(CHUNK_SIZE - 1),
    };
}

// Loads the chunk that contains `pos`, possibly returning `NULL` if OOM
Chunk* world_load(World* self, ivec3 pos);
// Gets the block at world position `pos`, returning `NULL` if not loaded
Block* world_get(const World* self, ivec3 pos);
// Shoots a ray in the world, returning `NULL` if it hit nothing or went
// past loaded chunks
Block* world_raycast(const World* self, vec3 from, vec3 dir);