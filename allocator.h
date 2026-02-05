#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>  // for size_t
#include <cstdint>  // for uintptr_t

// ============================================================================
// CONSTANTS
// ============================================================================

// Alignment requirement (8 bytes is standard for most systems)
#define ALIGNMENT 8

// Size classes for memory pools
#define SMALL_BLOCK_MAX   64
#define MEDIUM_BLOCK_MAX  256
#define LARGE_BLOCK_MAX   1024

// Initial pool sizes (you can adjust these)
#define SMALL_POOL_SIZE   (64 * 1024)   // 64 KB
#define MEDIUM_POOL_SIZE  (256 * 1024)  // 256 KB
#define LARGE_POOL_SIZE   (1024 * 1024) // 1 MB

// ============================================================================
// BLOCK HEADER STRUCTURE
// ============================================================================

/**
 * Block header structure
 * This is stored before each allocated block
 * 
 * TODO: You need to design this structure!
 * Consider:
 * - Size of the block
 * - Whether it's free or allocated
 * - Pointer to next free block (for free list)
 * - Maybe a magic number for debugging?
 */
struct BlockHeader {
    // TODO: Add your fields here
    // Example:
    // size_t size;
    // bool is_free;
    // BlockHeader* next_free;
    // uint32_t magic;  // For debugging (e.g., 0xDEADBEEF)
    size_t size;
    bool is_free;
    BlockHeader* next_free;

};

// ============================================================================
// MEMORY POOL STRUCTURE
// ============================================================================

/**
 * Represents a memory pool for a specific size class
 * 
 * TODO: Design this structure
 * Consider:
 * - Pointer to the memory pool
 * - Size of the pool
 * - Free list head
 * - Statistics (allocated, free, etc.)
 */
struct MemoryPool {
    // TODO: Add your fields here
    // Example:
    // void* pool_start;
    // size_t pool_size;
    // BlockHeader* free_list;
    // size_t allocated_bytes;
    // size_t free_bytes;
    void* pool_start;
    size_t pool_size;
    BlockHeader* free_list;


    // for statistics
    size_t allocated_bytes;
    size_t free_bytes;
};

// ============================================================================
// PUBLIC API - These functions replace malloc/free
// ============================================================================

/**
 * Allocate memory (replaces malloc)
 * 
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
void* my_malloc(size_t size);

/**
 * Free memory (replaces free)
 * 
 * @param ptr Pointer to memory to free
 */
void my_free(void* ptr);

/**
 * Allocate and zero memory (replaces calloc)
 * 
 * @param num Number of elements
 * @param size Size of each element
 * @return Pointer to allocated memory, or NULL on failure
 */
void* my_calloc(size_t num, size_t size);

/**
 * Reallocate memory (replaces realloc)
 * 
 * @param ptr Pointer to existing memory
 * @param size New size
 * @return Pointer to reallocated memory, or NULL on failure
 */
void* my_realloc(void* ptr, size_t size);

// ============================================================================
// INTERNAL FUNCTIONS - Helper functions you'll implement
// ============================================================================

/**
 * Initialize the allocator
 * Call this once at program start
 */
void allocator_init();

/**
 * Cleanup the allocator
 * Call this at program end
 */
void allocator_cleanup();

/**
 * Align a size to the required alignment
 * 
 * @param size Size to align
 * @return Aligned size
 */
size_t align_size(size_t size);

/**
 * Get the header from a user pointer
 * 
 * @param ptr User pointer
 * @return Pointer to block header
 */
BlockHeader* get_header(void* ptr);

/**
 * Get user pointer from header
 * 
 * @param header Block header
 * @return User pointer
 */
void* get_user_ptr(BlockHeader* header);

/**
 * Find the appropriate pool for a given size
 * 
 * @param size Requested size
 * @return Pointer to appropriate MemoryPool
 */
MemoryPool* select_pool(size_t size);

/**
 * Initialize a memory pool
 * 
 * @param pool Pool to initialize
 * @param size Size of the pool
 * @param max_block_size Maximum block size for this pool
 */
void init_pool(MemoryPool* pool, size_t pool_size, size_t max_block_size);

/**
 * Allocate from a specific pool
 * 
 * @param pool Pool to allocate from
 * @param size Size to allocate
 * @return Pointer to allocated memory, or NULL
 */
void* allocate_from_pool(MemoryPool* pool, size_t size);

/**
 * Free a block back to its pool
 * 
 * @param pool Pool to free to
 * @param header Block header to free
 */
void free_to_pool(MemoryPool* pool, BlockHeader* header);

/**
 * Add a block to the free list
 * 
 * @param pool Pool containing the block
 * @param header Block to add
 */
void add_to_free_list(MemoryPool* pool, BlockHeader* header);

/**
 * Remove a block from the free list
 * 
 * @param pool Pool containing the block
 * @param header Block to remove
 */
void remove_from_free_list(MemoryPool* pool, BlockHeader* header);

/**
 * Coalesce adjacent free blocks
 * 
 * @param pool Pool to coalesce in
 * @param header Block to start coalescing from
 * @return Pointer to coalesced block header
 */
BlockHeader* coalesce_blocks(MemoryPool* pool, BlockHeader* header);

/**
 * Find a free block using first-fit strategy
 * 
 * @param pool Pool to search
 * @param size Required size
 * @return Pointer to suitable block, or NULL
 */
BlockHeader* find_first_fit(MemoryPool* pool, size_t size);

/**
 * Find a free block using best-fit strategy
 * 
 * @param pool Pool to search
 * @param size Required size
 * @return Pointer to suitable block, or NULL
 */
BlockHeader* find_best_fit(MemoryPool* pool, size_t size);

/**
 * Split a block if it's too large
 * 
 * @param header Block to split
 * @param size Size needed
 * @return Pointer to the block that will be used
 */
BlockHeader* split_block(BlockHeader* header, size_t size);

// ============================================================================
// STATISTICS & DEBUGGING
// ============================================================================

/**
 * Print allocator statistics
 */
void print_allocator_stats();

/**
 * Get total allocated memory
 */
size_t get_allocated_bytes();

/**
 * Get total free memory
 */
size_t get_free_bytes();

/**
 * Get number of free blocks
 */
size_t get_free_block_count();

/**
 * Validate allocator integrity (for debugging)
 * Returns true if valid, false if corruption detected
 */
bool validate_allocator();

#endif // ALLOCATOR_H
