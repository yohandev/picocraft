#include <string.h>

#include "world.h"
#include "num.h"

inline u32 chunk_hash(ivec3 pos) {
    u32 hash = 0;
    // A few assumptions to make hashing better:
    // 1. Index needs to fit in 5 bits(32 slots)
    // 2. Chunks loaded will usually be close to each other,
    //    so their least significant position bits matter
    // 3. Chunk positions are aligned to CHUNK_SIZE, so their
    //    lower 4 position bits can be discarded
    hash ^= (pos.x >> 4) * 0x633767;
    hash ^= (pos.y >> 4) * 0x2634bf;
    hash ^= (pos.z >> 4) * 0x5aa6a3;

    return hash;
}

// Get the aligned position of the chunk that contains this block at `pos`
inline ivec3 chunk_pos(ivec3 pos) {
    // 0b[........|........|........|....][....]
    //          Truncate bits < CHUNK_SIZE ^
    return (ivec3){
        .x = pos.x & !(CHUNK_SIZE - 1),
        .y = pos.y & !(CHUNK_SIZE - 1),
        .z = pos.z & !(CHUNK_SIZE - 1),
    };
}

// Get the 1D index of a block in its chunks, given its world position
inline usize block_idx(ivec3 pos) {
    ivec3 rpos = {
        .x = pos.x & (CHUNK_SIZE - 1),
        .y = pos.y & (CHUNK_SIZE - 1),
        .z = pos.z & (CHUNK_SIZE - 1),
    };
    return rpos.z * CHUNK_AREA + rpos.y * CHUNK_SIZE + rpos.x;
}

void world_init(World* self) {
    memset(self->slots, SLOT_VACANT, CHUNK_SLOTS);
}

Chunk* world_load(World* self, ivec3 pos) {
    ivec3 cpos = chunk_pos(pos);
    usize slot;

    // TODO check already loaded
    
    // Find a free slot
    for (
        slot = 0;
        slot < CHUNK_SLOTS && self->slots[slot];
        slot++
    );
    // OOM
    if (slot >= CHUNK_SLOTS) return NULL;

    Chunk* c = &self->chunks[slot];
    // TODO load chunk from flash
    memset(&c->blocks, 0, CHUNK_VOLUME);
    c->pos = cpos;
    // Occupy slot in vector
    self->slots[slot] = true;
}

Chunk* world_get_chunk(World* self, ivec3 pos) {
    ivec3 cpos = chunk_pos(pos);

    for (usize i = 0; i < CHUNK_SLOTS; i++) {
        if (!self->slots[i]) continue;

        Chunk* c = &self->chunks[i];
        // Found!
        if (cmp(c->pos, ==, cpos)) return c;
    }
    return NULL;
}

Block* world_get(World* self, ivec3 pos) {
    return &world_get_chunk(self, pos)->blocks[block_idx(pos)];
}

Block* world_raycast(const World* self, vec3 a, vec3 d) {
    // DDA algorithm
    fixed steps = fxmax(fxabs(d.x), fxmax(fxabs(d.y), fxabs(d.z)));

    if (cmp(steps, ==, fixed(0))) {
        return;
    }
    d = div(d, steps);

    Block* b;
    ivec3 p;
    for (usize i = 0; i < fxfloor(steps); i++) {
        p = (ivec3){
            .x = fxfloor(a.x),
            .y = fxfloor(a.y),
            .z = fxfloor(a.z),
        };
        if (b = world_get(self, p)) {
            // TODO
        }
        a = add(a, d);
    }
}