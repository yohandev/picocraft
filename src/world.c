#include <string.h>

#include "world.h"

#define LINEAR_IMPL

#define BUCKET_VACANT (CHUNK_SLOTS + 1)

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
    memset(self->bucket, BUCKET_VACANT, CHUNK_BUCKETS);
    memset(self->slots, false, CHUNK_SLOTS);
}

#ifndef LINEAR_IMPL
Chunk* world_load(World* self, ivec3 pos) {
    ivec3 cpos = chunk_pos(pos);
    usize bucket = chunk_hash(cpos);
    usize slot;

    // It should be impossible for this to loop forever, as
    // there are more buckets than slots
    for (
        bucket &= (CHUNK_BUCKETS - 1);
        self->bucket[bucket] != BUCKET_VACANT;
        bucket = (bucket + 1) % CHUNK_BUCKETS
    ) {
        Chunk* c = &self->chunks[self->bucket[bucket]];
        // Already loaded
        if (cmp(c->pos, ==, cpos)) return c;
    }
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
    // Occupy slot in hashmap
    self->slots[slot] = true;
    self->bucket[bucket] = slot;
}

Chunk* world_get_chunk(World* self, ivec3 pos) {
    ivec3 cpos = chunk_pos(pos);
    usize bucket = chunk_hash(cpos);

    for (
        bucket &= (CHUNK_BUCKETS - 1);
        self->bucket[bucket] != BUCKET_VACANT;
        bucket = (bucket + 1) % CHUNK_BUCKETS
    ) {
        Chunk* c = &self->chunks[self->bucket[bucket]];
        // Found!
        if (cmp(c->pos, ==, cpos)) return c;
    }
    // Not loaded
    return NULL;
}
#else
Chunk* world_load(World* self, ivec3 pos) {
    ivec3 cpos = chunk_pos(pos);
    usize slot;

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
#endif

Block* world_get(World* self, ivec3 pos) {
    return &world_get_chunk(self, pos)->blocks[block_idx(pos)];
}

Block* world_raycast(const World* self, vec3 from, vec3 dir) {

}