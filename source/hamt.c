#include "gir_internals.h"

static inline bool is_subtrie(hamt_node_t* node) {
    return (node->base_or_value & 1u);
}

static inline void set_subtrie(hamt_node_t* node, hamt_node_t* value) {
    node->base_or_value = ((uintptr_t)value | 1);
}

static inline hamt_node_t* get_subtrie(hamt_node_t* node) {
    return ((hamt_node_t*)((node->base_or_value | 1) ^ 1));
}

static inline void set_value(hamt_node_t* node, void* value) {
    node->base_or_value = (uintptr_t)value;
}

static inline void* get_value(hamt_node_t* node) {
    return ((void*)node->base_or_value);
}

/*****************************************************************************/

static inline uint32_t hash_key(uint8_t* key, size_t len) {
    uint32_t a=31415u, b=27183u, hash;
    for (hash=0; len > 0; key++, len--, a*=b)
        hash = (a * hash) + *key;
    return hash;
}

static inline uint32_t rehash_key(uint8_t* key, size_t len, int level) {
    uint32_t a=31415u, b=27183u, hash;
    for (hash=0; len > 0; key++, len--, a*=b)
        hash = (a * hash * level) + *key;
    return hash;
}

static inline uint32_t get_hash(hamt_t* trie, void* entry) {
    uint8_t* addr;
    size_t len;
    trie->getkey(entry, &addr, &len);
    uint32_t hash = hash_key(addr, len);
    return hash;
}

static inline uint32_t get_rehash(hamt_t* trie, void* entry, int level) {
    uint8_t* addr;
    size_t len;
    trie->getkey(entry, &addr, &len);
    return rehash_key(addr, len, level);
}

/*****************************************************************************/

static inline uint64_t popcount(uint64_t bits) {
#ifdef __GNUC__
    return __builtin_popcount(bits);
#else
    const uint64_t m1  = 0x5555555555555555;
    const uint64_t m2  = 0x3333333333333333;
    const uint64_t m4  = 0x0f0f0f0f0f0f0f0f;
    const uint64_t h01 = 0x0101010101010101;
    bits -= (bits >> 1) & m1;
    bits  = (bits & m2) + ((bits >> 2) & m2);
    bits  = (bits + (bits >> 4)) & m4;
    return  (bits * h01) >> 56;
#endif
}

static inline uint32_t get_index(hamt_ctx_t* ctx) {
    uint32_t mask = ~((~0ul) << ctx->part);
    uint32_t lower_bits = ctx->current->bitmap_or_key & mask;
    uint32_t count = popcount(lower_bits);
    return count;
}

static inline uint32_t get_size(hamt_ctx_t* ctx) {
    return popcount(ctx->current->bitmap_or_key);
}

/*****************************************************************************/

static inline void init_context(hamt_ctx_t* ctx, hamt_t* trie, void* entry) {
    ctx->hash        = get_hash(trie, entry);
    ctx->part        = (ctx->hash & 0x1F);
    ctx->bits        = 0;
    ctx->level       = 0;
    ctx->grandparent = NULL;
    ctx->parent      = NULL;
    ctx->current     = &trie->root[ctx->part];
}

static inline void add_to_subtrie(hamt_ctx_t* ctx, void* key)
{
    uint32_t size;
    uint32_t index;
    hamt_node_t* newnodes;
    /* Set the bit in the bitmap */
    ctx->current->bitmap_or_key |= (1 << ctx->part);
    /* Count bits in the bitmap to determine new size */
    size = get_size(ctx);
    newnodes = malloc(size * sizeof(hamt_node_t));
    /* Copy the old data to the new array leaving a gap for the new entry */
    index = get_index(ctx);
    memcpy(newnodes, get_subtrie(ctx->current), index * sizeof(hamt_node_t));
    memcpy(&newnodes[index+1], &(get_subtrie(ctx->current))[index], (size-index-1) * sizeof(hamt_node_t));
    free(get_subtrie(ctx->current));
    /* Setup the new node */
    newnodes[index].bitmap_or_key = ctx->hash;
    set_value(&newnodes[index], key);
    set_subtrie(ctx->current, newnodes);
}

static inline void build_subtrie(hamt_t* trie, hamt_ctx_t* ctx, void* key)
{
    uint32_t hash = ctx->current->bitmap_or_key;
    while (true) {
        uint32_t part;
        /* Descend down the trie to the nodes that dont exist yet */
        ctx->bits += 5;
        if (ctx->bits > 30) {
            ctx->hash = get_rehash(trie, key, ctx->level);
            hash = get_rehash(trie, (void*)(ctx->current->base_or_value), ctx->level);
            ctx->bits = 0;
        }
        ctx->part = (ctx->hash >> ctx->bits) & 0x1F;
        part = (hash >> ctx->bits) & 0x1F;
        /* If the bits are equal, build a single entry node and keep going */
        if (part == ctx->part) {
            hamt_node_t* newnode = (hamt_node_t*)calloc(1,sizeof(hamt_node_t));
            newnode->bitmap_or_key = hash;
            newnode->base_or_value = ctx->current->base_or_value;
            ctx->current->bitmap_or_key = 1 << ctx->part;
            set_subtrie(ctx->current, newnode);
            ctx->current = newnode;
            ctx->level++;
        /* Otherwise they differ, so build a two-entry node */
        } else {
            hamt_node_t* newnodes = (hamt_node_t*)calloc(2,sizeof(hamt_node_t));
            /* copy nodes to new subtrie in order */
            if (part < ctx->part) {
                newnodes[0].bitmap_or_key = hash;
                newnodes[0].base_or_value = ctx->current->base_or_value;
                newnodes[1].bitmap_or_key = ctx->hash;
                set_value(&newnodes[1], key);
            } else {
                newnodes[0].bitmap_or_key = ctx->hash;
                set_value(&newnodes[0], key);
                newnodes[1].bitmap_or_key = hash;
                newnodes[1].base_or_value = ctx->current->base_or_value;
            }
            /* Update the bitmap for the subtrie */
            ctx->current->bitmap_or_key = (1UL << ctx->part) | (1UL << part);
            set_subtrie(ctx->current, newnodes);
            break;
        }
    }
}

/*****************************************************************************/

void hamt_init(hamt_t* trie, hamt_keyfn_t getkey, hamt_delfn_t delfn, hamt_cmpfn_t cmpfn) {
    trie->root     = calloc(32, sizeof(hamt_node_t));
    trie->getkey   = getkey;
    trie->delentry = delfn;
    trie->cmpentry = cmpfn;
}

static void* find_entry(hamt_t* trie, hamt_ctx_t* ctx, void* key) {
    void* entry = NULL;

    /* If the slot is not empty, search the trie */
    if (0 != ctx->current->base_or_value) {
        while (true) {
            /* If we reached a leaf, check to see if we have a match */
            if (!is_subtrie(ctx->current)) {
                if ((ctx->current->bitmap_or_key == ctx->hash) &&
                    (0 == trie->cmpentry(key, get_value(ctx->current)))) {
                    entry = get_value(ctx->current);
                }
                break;
            }
            /* Otherwise, descend further down the tree */
            ctx->bits += 5;
            if (ctx->bits > 30) {
                ctx->hash = get_rehash(trie, key, ctx->level);
                ctx->bits = 0;
            }
            ctx->part = (ctx->hash >> ctx->bits) & 0x1F;
            /* If the trie is empty, bail */
            if (0 == (ctx->current->bitmap_or_key & (1 << ctx->part)))
                break;
            /* Otherwise, descend to the subtrie */
            ctx->grandparent = ctx->parent;
            ctx->parent      = ctx->current;
            ctx->current     = &((get_subtrie(ctx->current))[get_index(ctx)]);
            ctx->level++;
        }
    }
    return entry;
}

void hamt_insert(hamt_t* trie, void* key) {
    hamt_ctx_t ctx;
    void* entry;
    init_context(&ctx, trie, key);

    /* If the root table entry is empty, then fill it in and bail */
    if (0 == ctx.current->base_or_value) {
        ctx.current->bitmap_or_key = ctx.hash;
        set_value(ctx.current, key);
    } else {
        entry = find_entry(trie, &ctx, key);
        /* If we found an entry, replace it */
        if (entry != NULL) {
            trie->delentry(get_value(ctx.current));
            set_value(ctx.current, entry);
        /* If we ended on a subtrie, add the value to the node */
        } else if (is_subtrie(ctx.current)) {
            add_to_subtrie(&ctx, key);
        /* If we ended on a value, build the tree out until the keys differ */
        } else {
            build_subtrie(trie, &ctx, key);
        }
    }
}

void* hamt_lookup(hamt_t* trie, void* key) {
    hamt_ctx_t ctx;
    init_context(&ctx, trie, key);
    return find_entry(trie, &ctx, key);
}

