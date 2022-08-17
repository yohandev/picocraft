/**
 * [world.h] World and chunks in Picocraft.
 */
#pragma once

#include "num.h"
#include "block.h"

// Size(in blocks) of a chunk
#define CHUNK_SIZE 16
#define CHUNK_AREA (CHUNK_SIZE * CHUNK_SIZE)
#define CHUNK_VOLUME (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

// Size of chunk slot allocator
#define CHUNK_SLOTS 32

// A cubic segment of a Picocraft `World`
typedef struct {
    Block blocks[CHUNK_VOLUME]; // Blocks in the chunks
    ivec3 pos;                  // World position of the chunk's corner
} Chunk;

typedef struct {
    Chunk chunks[CHUNK_SLOTS];  // Heap for maybe-uninit `Chunk`s
    enum {
        SLOT_VACANT,            // Matching index in `chunks` is uninit
        SLOT_OCCUPIED,          // Matching index in `chunks` is in-use
    } slots[CHUNK_SLOTS];
} World;

void world_init(World* self);

// Loads the chunk that contains `pos`, possibly returning `NULL` if OOM
Chunk* world_load(World* self, ivec3 pos);
// Gets the block at world position `pos`, returning `NULL` if not loaded
Block* world_get(World* self, ivec3 pos);
// Get the chunk that contains the world position `pos`, `NULL` if not loaded
Chunk* world_get_chunk(World* self, ivec3 pos);
// Shoots a ray in the world, returning `NULL` if it hit nothing or went
// past loaded chunks
Block* world_raycast(const World* self, vec3 from, vec3 dir);