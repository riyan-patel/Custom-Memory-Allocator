#include "allocator.h"
#include <cstdlib>   // for size_t
#include <cstring>   // for memset, memcpy
#include <iostream>  // for debugging
#include <unistd.h>  // for sbrk, mmap (Unix systems)
#include <sys/mman.h> // for mmap, munmap

// ============================================================================
// GLOBAL STATE
// ============================================================================

// Memory pools for different size classes
static MemoryPool small_pool;
static MemoryPool medium_pool;
static MemoryPool large_pool;
static MemoryPool xlarge_pool;  // For blocks > LARGE_BLOCK_MAX

// Track if allocator is initialized
static bool allocator_initialized = false;

// ============================================================================
// INITIALIZATION & CLEANUP
// ============================================================================

void allocator_init() {
    // TODO: Implement this function
    // 1. Initialize all memory pools
    // 2. Allocate memory for each pool using mmap() or sbrk()
    // 3. Set up initial free lists
    // 4. Mark allocator as initialized
    
    if (allocator_initialized) {
        return;  // Already initialized
    }
    
    // Example structure (you need to fill this in):
    // init_pool(&small_pool, SMALL_POOL_SIZE, SMALL_BLOCK_MAX);
    // init_pool(&medium_pool, MEDIUM_POOL_SIZE, MEDIUM_BLOCK_MAX);
    // init_pool(&large_pool, LARGE_POOL_SIZE, LARGE_BLOCK_MAX);
    // init_pool(&xlarge_pool, LARGE_POOL_SIZE, SIZE_MAX);  // No max for xlarge
    
    allocator_initialized = true;
    std::cout << "Allocator initialized\n";
}

void allocator_cleanup() {
    // TODO: Implement this function
    // 1. Check for memory leaks (unfreed blocks)
    // 2. Unmap/deallocate all pools
    // 3. Mark allocator as uninitialized
    
    if (!allocator_initialized) {
        return;
    }
    
    // TODO: Free all pools using munmap()
    // TODO: Print leak report if any blocks are still allocated
    
    allocator_initialized = false;
    std::cout << "Allocator cleaned up\n";
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

size_t align_size(size_t size) {
    // TODO: Implement alignment
    // Round up size to nearest multiple of ALIGNMENT
    // Example: if ALIGNMENT is 8, then:
    //   7 -> 8
    //   8 -> 8
    //   9 -> 16
    //   15 -> 16
    
    // Hint: Use (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1)
    
    return size;  // Placeholder - implement this!
}

BlockHeader* get_header(void* ptr) {
    // TODO: Implement this
    // Given a user pointer, return the block header
    // The header is stored BEFORE the user pointer
    
    // Hint: Cast ptr to char*, subtract sizeof(BlockHeader)
    // Then cast back to BlockHeader*
    
    if (ptr == nullptr) {
        return nullptr;
    }
    
    // Placeholder - implement this!
    return nullptr;
}

void* get_user_ptr(BlockHeader* header) {
    // TODO: Implement this
    // Given a block header, return the user pointer
    // The user pointer is stored AFTER the header
    
    // Hint: Cast header to char*, add sizeof(BlockHeader)
    // Then cast back to void*
    
    if (header == nullptr) {
        return nullptr;
    }
    
    // Placeholder - implement this!
    return nullptr;
}

MemoryPool* select_pool(size_t size) {
    // TODO: Implement pool selection
    // Return the appropriate pool based on size
    
    if (size <= SMALL_BLOCK_MAX) {
        return &small_pool;
    } else if (size <= MEDIUM_BLOCK_MAX) {
        return &medium_pool;
    } else if (size <= LARGE_BLOCK_MAX) {
        return &large_pool;
    } else {
        return &xlarge_pool;
    }
}

// ============================================================================
// POOL MANAGEMENT
// ============================================================================

void init_pool(MemoryPool* pool, size_t pool_size, size_t max_block_size) {
    // TODO: Implement pool initialization
    // 1. Allocate memory for the pool using mmap()
    //    Example: pool->pool_start = mmap(NULL, pool_size, PROT_READ|PROT_WRITE, 
    //                                     MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    // 2. Set pool size
    // 3. Create initial free block covering entire pool
    // 4. Initialize free list with this block
    // 5. Set up statistics
    
    if (pool == nullptr) {
        return;
    }
    
    // TODO: Allocate memory using mmap()
    // TODO: Create initial block header
    // TODO: Add to free list
    
    std::cout << "Pool initialized: size=" << pool_size << "\n";
}

void* allocate_from_pool(MemoryPool* pool, size_t size) {
    // TODO: Implement allocation from a pool
    // 1. Align the requested size
    // 2. Find a suitable free block (use first-fit or best-fit)
    // 3. If block is larger than needed, split it
    // 4. Mark block as allocated
    // 5. Remove from free list
    // 6. Return user pointer
    
    if (pool == nullptr) {
        return nullptr;
    }
    
    size_t aligned_size = align_size(size);
    
    // TODO: Find a free block
    // TODO: Split if necessary
    // TODO: Mark as allocated
    // TODO: Remove from free list
    
    return nullptr;  // Placeholder
}

void free_to_pool(MemoryPool* pool, BlockHeader* header) {
    // TODO: Implement freeing to a pool
    // 1. Mark block as free
    // 2. Coalesce with adjacent free blocks
    // 3. Add to free list
    // 4. Update statistics
    
    if (pool == nullptr || header == nullptr) {
        return;
    }
    
    // TODO: Mark as free
    // TODO: Coalesce adjacent blocks
    // TODO: Add to free list
}

// ============================================================================
// FREE LIST MANAGEMENT
// ============================================================================

void add_to_free_list(MemoryPool* pool, BlockHeader* header) {
    // TODO: Implement adding to free list
    // Add header to the front of the free list
    
    if (pool == nullptr || header == nullptr) {
        return;
    }
    
    // TODO: Insert at head of free list
    // Hint: header->next_free = pool->free_list;
    //       pool->free_list = header;
}

void remove_from_free_list(MemoryPool* pool, BlockHeader* header) {
    // TODO: Implement removing from free list
    // Handle three cases:
    // 1. Header is at the head
    // 2. Header is in the middle
    // 3. Header is at the tail
    
    if (pool == nullptr || header == nullptr) {
        return;
    }
    
    // TODO: Remove from linked list
}

BlockHeader* coalesce_blocks(MemoryPool* pool, BlockHeader* header) {
    // TODO: Implement block coalescing
    // 1. Check if previous block is free (if it exists)
    // 2. Check if next block is free (if it exists)
    // 3. Merge adjacent free blocks
    // 4. Update sizes and pointers
    
    if (pool == nullptr || header == nullptr) {
        return header;
    }
    
    // TODO: Find previous block (tricky - you need to track this)
    // TODO: Find next block (easier - header + header->size)
    // TODO: Merge if adjacent blocks are free
    
    return header;  // Return coalesced block
}

// ============================================================================
// ALLOCATION STRATEGIES
// ============================================================================

BlockHeader* find_first_fit(MemoryPool* pool, size_t size) {
    // TODO: Implement first-fit strategy
    // Search free list for first block that's large enough
    
    if (pool == nullptr) {
        return nullptr;
    }
    
    // TODO: Iterate through free list
    // TODO: Return first block with size >= requested size
    
    return nullptr;
}

BlockHeader* find_best_fit(MemoryPool* pool, size_t size) {
    // TODO: Implement best-fit strategy
    // Search free list for smallest block that's large enough
    
    if (pool == nullptr) {
        return nullptr;
    }
    
    // TODO: Iterate through free list
    // TODO: Track the best (smallest suitable) block
    // TODO: Return best block
    
    return nullptr;
}

BlockHeader* split_block(BlockHeader* header, size_t size) {
    // TODO: Implement block splitting
    // If block is much larger than needed, split it
    // Return the block that will be used
    // Add the remainder to free list
    
    if (header == nullptr) {
        return nullptr;
    }
    
    // TODO: Check if block is large enough to split
    // (e.g., if remainder would be >= minimum block size)
    // TODO: Create new header for remainder
    // TODO: Update sizes
    // TODO: Return original header (now with correct size)
    
    return header;
}

// ============================================================================
// PUBLIC API
// ============================================================================

void* my_malloc(size_t size) {
    // TODO: Implement main allocation function
    // 1. Initialize allocator if needed
    // 2. Handle zero-size requests
    // 3. Select appropriate pool
    // 4. Allocate from pool
    // 5. Return pointer
    
    if (!allocator_initialized) {
        allocator_init();
    }
    
    if (size == 0) {
        return nullptr;  // or return a valid pointer to 0 bytes
    }
    
    // TODO: Select pool and allocate
    MemoryPool* pool = select_pool(size);
    return allocate_from_pool(pool, size);
}

void my_free(void* ptr) {
    // TODO: Implement main free function
    // 1. Handle NULL pointer
    // 2. Get block header
    // 3. Find which pool this block belongs to
    // 4. Free to pool
    
    if (ptr == nullptr) {
        return;  // Freeing NULL is safe (like standard free)
    }
    
    // TODO: Get header
    // TODO: Find pool (you might need to track this in header)
    // TODO: Free to pool
}

void* my_calloc(size_t num, size_t size) {
    // TODO: Implement calloc
    // 1. Calculate total size
    // 2. Check for overflow
    // 3. Allocate using my_malloc
    // 4. Zero the memory using memset
    
    size_t total_size = num * size;
    
    // Check for overflow
    if (num != 0 && total_size / num != size) {
        return nullptr;  // Overflow
    }
    
    void* ptr = my_malloc(total_size);
    if (ptr != nullptr) {
        std::memset(ptr, 0, total_size);
    }
    
    return ptr;
}

void* my_realloc(void* ptr, size_t size) {
    // TODO: Implement realloc
    // 1. Handle NULL ptr (like malloc)
    // 2. Handle zero size (like free)
    // 3. Get old block header
    // 4. If new size <= old size, maybe shrink (optional)
    // 5. If new size > old size:
    //    a. Try to expand in place (if next block is free)
    //    b. Otherwise, allocate new block and copy data
    // 6. Free old block if moved
    
    if (ptr == nullptr) {
        return my_malloc(size);
    }
    
    if (size == 0) {
        my_free(ptr);
        return nullptr;
    }
    
    // TODO: Get old header and size
    // TODO: Try to expand or allocate new block
    // TODO: Copy data if moved
    
    return nullptr;  // Placeholder
}

// ============================================================================
// STATISTICS & DEBUGGING
// ============================================================================

void print_allocator_stats() {
    // TODO: Print statistics for all pools
    // - Total allocated
    // - Total free
    // - Number of free blocks
    // - Fragmentation metrics
    
    std::cout << "=== Allocator Statistics ===\n";
    // TODO: Print stats for each pool
}

size_t get_allocated_bytes() {
    // TODO: Sum allocated bytes across all pools
    return 0;
}

size_t get_free_bytes() {
    // TODO: Sum free bytes across all pools
    return 0;
}

size_t get_free_block_count() {
    // TODO: Count free blocks across all pools
    return 0;
}

bool validate_allocator() {
    // TODO: Validate allocator integrity
    // Check:
    // - All headers are valid
    // - Free list is consistent
    // - No double-free
    // - No memory corruption
    
    return true;  // Placeholder
}
