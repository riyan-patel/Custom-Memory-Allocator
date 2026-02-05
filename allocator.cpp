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
    // Initialize all memory pools
    // This sets up separate pools for different size classes
    
    if (allocator_initialized) {
        return;  // Already initialized
    }
    
    // Initialize each pool with its size and max block size
    init_pool(&small_pool, SMALL_POOL_SIZE, SMALL_BLOCK_MAX);
    init_pool(&medium_pool, MEDIUM_POOL_SIZE, MEDIUM_BLOCK_MAX);
    init_pool(&large_pool, LARGE_POOL_SIZE, LARGE_BLOCK_MAX);
    init_pool(&xlarge_pool, LARGE_POOL_SIZE, SIZE_MAX);  // No max for xlarge
    
    allocator_initialized = true;
    std::cout << "Allocator initialized\n";
}

void allocator_cleanup() {
    // Cleanup the allocator and check for memory leaks
    // This should be called at program end
    
    if (!allocator_initialized) {
        return;
    }
    
    // Step 1: Check for memory leaks (unfreed blocks)
    // Walk through all pools and count allocated blocks
    size_t total_allocated = 0;
    size_t leak_count = 0;
    
    MemoryPool* pools[] = {&small_pool, &medium_pool, &large_pool, &xlarge_pool};
    
    for (int i = 0; i < 4; i++) {
        MemoryPool* pool = pools[i];
        
        if (pool->pool_start == nullptr) {
            continue;  // Pool not initialized
        }
        
        // Walk through the entire pool looking for allocated blocks
        uintptr_t pool_start = (uintptr_t)pool->pool_start;
        uintptr_t pool_end = pool_start + pool->pool_size;
        uintptr_t current = pool_start;
        
        while (current < pool_end) {
            BlockHeader* header = (BlockHeader*)current;
            
            // Check if this is a valid block (not past pool end)
            if (current + header->size > pool_end) {
                break;  // Invalid block, stop scanning
            }
            
            // Check if block is allocated (not free)
            if (!header->is_free) {
                total_allocated += header->size;
                leak_count++;
            }
            
            // Move to next block
            current += header->size;
        }
    }
    
    // Print leak report
    if (leak_count > 0) {
        std::cout << "⚠️  MEMORY LEAK DETECTED!\n";
        std::cout << "   Leaked blocks: " << leak_count << "\n";
        std::cout << "   Total leaked: " << total_allocated << " bytes\n";
    } else {
        std::cout << "✓ No memory leaks detected\n";
    }
    
    // Step 2: Unmap/deallocate all pools using munmap()
    if (small_pool.pool_start != nullptr && small_pool.pool_start != MAP_FAILED) {
        munmap(small_pool.pool_start, small_pool.pool_size);
        small_pool.pool_start = nullptr;
    }
    
    if (medium_pool.pool_start != nullptr && medium_pool.pool_start != MAP_FAILED) {
        munmap(medium_pool.pool_start, medium_pool.pool_size);
        medium_pool.pool_start = nullptr;
    }
    
    if (large_pool.pool_start != nullptr && large_pool.pool_start != MAP_FAILED) {
        munmap(large_pool.pool_start, large_pool.pool_size);
        large_pool.pool_start = nullptr;
    }
    
    if (xlarge_pool.pool_start != nullptr && xlarge_pool.pool_start != MAP_FAILED) {
        munmap(xlarge_pool.pool_start, xlarge_pool.pool_size);
        xlarge_pool.pool_start = nullptr;
    }
    
    // Step 3: Mark allocator as uninitialized
    allocator_initialized = false;
    std::cout << "Allocator cleaned up\n";
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

size_t align_size(size_t size) {
    // Round up size to nearest multiple of ALIGNMENT
    // Formula: (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1)
    // This adds (ALIGNMENT - 1) to size, then clears the lower bits
    // Example: if ALIGNMENT is 8, then:
    //   7 -> 8,  8 -> 8,  9 -> 16,  15 -> 16
    
    if (size == 0) {
        return ALIGNMENT;  // Minimum aligned size
    }
    
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

BlockHeader* get_header(void* ptr) {
    // Given a user pointer, return the block header
    // The header is stored BEFORE the user pointer
    
    if (ptr == nullptr) {
        return nullptr;
    }
    
    // Cast to char* for byte arithmetic, subtract header size, cast back to BlockHeader*
    return (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
}

void* get_user_ptr(BlockHeader* header) {
    // Given a block header, return the user pointer
    // The user pointer is stored AFTER the header
    
    if (header == nullptr) {
        return nullptr;
    }
    
    // Cast to char* for byte arithmetic, add header size, cast back to void*
    return (void*)((char*)header + sizeof(BlockHeader));
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

void init_pool(MemoryPool* pool, size_t pool_size, size_t /* max_block_size */) {
    // Initialize a memory pool by allocating memory and creating initial free block
    
    if (pool == nullptr) {
        return;
    }
    
    // Step 1: Allocate memory from the operating system using mmap()
    // mmap() asks the OS for a chunk of memory
    pool->pool_start = mmap(NULL, pool_size, PROT_READ | PROT_WRITE, 
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    // Check if allocation failed
    if (pool->pool_start == MAP_FAILED) {
        std::cerr << "Failed to allocate pool memory\n";
        return;
    }
    
    // Step 2: Store the pool size
    pool->pool_size = pool_size;
    
    // Step 3: Create initial free block covering the entire pool
    // The first block header goes at the start of the pool
    BlockHeader* initial_block = (BlockHeader*)pool->pool_start;
    
    // Set up the initial block:
    // - Size is the entire pool (we'll use it all as one big free block)
    // - It's free (available for allocation)
    // - No next free block yet (it's the only one)
    initial_block->size = pool_size;
    initial_block->is_free = true;
    initial_block->next_free = nullptr;
    
    // Step 4: Initialize free list - point to this initial block
    pool->free_list = initial_block;
    
    // Step 5: Initialize statistics
    pool->allocated_bytes = 0;
    pool->free_bytes = pool_size;
    
    std::cout << "Pool initialized: size=" << pool_size << "\n";
}

void* allocate_from_pool(MemoryPool* pool, size_t size) {
    // Allocate memory from a specific pool
    // This is the core allocation function
    
    if (pool == nullptr) {
        return nullptr;
    }
    
    // Step 1: Align the requested size to 8-byte boundary
    size_t aligned_size = align_size(size);
    
    // Calculate total size needed: user data + header, then align
    size_t total_size_needed = aligned_size + sizeof(BlockHeader);
    total_size_needed = align_size(total_size_needed);  // Align the total
    
    // Step 2: Find a suitable free block using first-fit strategy
    BlockHeader* block = find_first_fit(pool, total_size_needed);
    
    if (block == nullptr) {
        // No suitable block found
        return nullptr;
    }
    
    // Step 3: Remove the block from the free list (we're about to use it)
    remove_from_free_list(pool, block);
    
    // Step 4: Check if we can split the block (if it's much larger than needed)
    size_t original_size = block->size;
    size_t min_split_size = total_size_needed + sizeof(BlockHeader) + ALIGNMENT;
    
    if (original_size >= min_split_size) {
        // Block is large enough to split - create a remainder block
        // Update current block to use only what we need
        block->size = total_size_needed;
        
        // Create a new free block from the remainder
        BlockHeader* remainder = (BlockHeader*)((char*)block + total_size_needed);
        remainder->size = original_size - total_size_needed;
        remainder->is_free = true;
        remainder->next_free = nullptr;
        
        // Add remainder to free list
        add_to_free_list(pool, remainder);
        
        // Update statistics for the remainder
        pool->free_bytes += remainder->size;
    } else {
        // Use the whole block (too small to split efficiently)
        // block->size stays as original_size
    }
    
    // Step 5: Mark the block as allocated
    block->is_free = false;
    block->next_free = nullptr;  // Not in free list anymore
    
    // Step 6: Update statistics
    pool->allocated_bytes += block->size;
    pool->free_bytes -= block->size;
    
    // Step 7: Return the user pointer (after the header)
    return get_user_ptr(block);
}

void free_to_pool(MemoryPool* pool, BlockHeader* header) {
    // Free a block back to its pool
    // This makes the memory available for future allocations
    
    if (pool == nullptr || header == nullptr) {
        return;
    }
    
    // Step 1: Update statistics BEFORE marking as free
    // (We need to know it was allocated to update stats correctly)
    if (!header->is_free) {  // Only update if it was actually allocated
        pool->allocated_bytes -= header->size;
        pool->free_bytes += header->size;
    }
    
    // Step 2: Mark block as free
    header->is_free = true;
    header->next_free = nullptr;  // Will be set when added to free list
    
    // Step 3: Add to free list (makes it available for allocation)
    add_to_free_list(pool, header);
    
    // Note: Coalescing (merging adjacent free blocks) is optional
    // and can be added later for better fragmentation reduction
}

// ============================================================================
// FREE LIST MANAGEMENT
// ============================================================================

void add_to_free_list(MemoryPool* pool, BlockHeader* header) {
    // Add a block to the front of the free list
    // This makes it quickly available for the next allocation
    
    if (pool == nullptr || header == nullptr) {
        return;
    }
    
    // Make sure the block is marked as free
    header->is_free = true;
    
    // Insert at the head of the free list
    // The new block points to whatever was first
    header->next_free = pool->free_list;
    
    // Update the pool's free_list to point to this new block
    pool->free_list = header;
}

void remove_from_free_list(MemoryPool* pool, BlockHeader* header) {
    // Remove a block from the free list
    // This happens when we're about to allocate it
    
    if (pool == nullptr || header == nullptr) {
        return;
    }
    
    // Case 1: Block is at the head of the free list
    if (pool->free_list == header) {
        // Just move the head to the next block
        pool->free_list = header->next_free;
        header->next_free = nullptr;
        return;
    }
    
    // Case 2 & 3: Block is in the middle or at the tail
    // We need to find the previous block and update its next pointer
    BlockHeader* current = pool->free_list;
    
    // Walk through the list to find the block before 'header'
    while (current != nullptr && current->next_free != header) {
        current = current->next_free;
    }
    
    // If we found the previous block, update its next pointer
    if (current != nullptr) {
        // Skip over 'header' by pointing to whatever header was pointing to
        current->next_free = header->next_free;
        header->next_free = nullptr;  // Clear the link
    }
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
    // Search the free list for the first block that's large enough
    // "First fit" means we take the first block we find that fits
    
    if (pool == nullptr) {
        return nullptr;
    }
    
    // Start at the head of the free list
    BlockHeader* current = pool->free_list;
    
    // Walk through the free list
    while (current != nullptr) {
        // Check if this block is free and large enough
        if (current->is_free && current->size >= size) {
            // Found a suitable block!
            return current;
        }
        
        // Move to the next free block
        current = current->next_free;
    }
    
    // No suitable block found
    return nullptr;
}

BlockHeader* find_best_fit(MemoryPool* pool, size_t /* size */) {
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

BlockHeader* split_block(BlockHeader* header, size_t /* size */) {
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
    // Main free function - frees memory allocated by my_malloc()
    
    if (ptr == nullptr) {
        return;  // Freeing NULL is safe (like standard free)
    }
    
    // Step 1: Get the block header from the user pointer
    BlockHeader* header = get_header(ptr);
    
    if (header == nullptr) {
        return;  // Invalid pointer
    }
    
    // Step 2: Find which pool this block belongs to
    // We check if the block's address is within each pool's range
    MemoryPool* pool = nullptr;
    
    uintptr_t block_addr = (uintptr_t)header;
    
    // Check small pool
    if (small_pool.pool_start != nullptr) {
        uintptr_t pool_start = (uintptr_t)small_pool.pool_start;
        uintptr_t pool_end = pool_start + small_pool.pool_size;
        if (block_addr >= pool_start && block_addr < pool_end) {
            pool = &small_pool;
        }
    }
    
    // Check medium pool
    if (pool == nullptr && medium_pool.pool_start != nullptr) {
        uintptr_t pool_start = (uintptr_t)medium_pool.pool_start;
        uintptr_t pool_end = pool_start + medium_pool.pool_size;
        if (block_addr >= pool_start && block_addr < pool_end) {
            pool = &medium_pool;
        }
    }
    
    // Check large pool
    if (pool == nullptr && large_pool.pool_start != nullptr) {
        uintptr_t pool_start = (uintptr_t)large_pool.pool_start;
        uintptr_t pool_end = pool_start + large_pool.pool_size;
        if (block_addr >= pool_start && block_addr < pool_end) {
            pool = &large_pool;
        }
    }
    
    // Check xlarge pool
    if (pool == nullptr && xlarge_pool.pool_start != nullptr) {
        uintptr_t pool_start = (uintptr_t)xlarge_pool.pool_start;
        uintptr_t pool_end = pool_start + xlarge_pool.pool_size;
        if (block_addr >= pool_start && block_addr < pool_end) {
            pool = &xlarge_pool;
        }
    }
    
    // Step 3: Free to the appropriate pool
    if (pool != nullptr) {
        free_to_pool(pool, header);
    } else {
        // Block doesn't belong to any pool - invalid pointer or corruption
        std::cerr << "Warning: Attempted to free invalid pointer\n";
    }
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
    // Reallocate memory - resize an existing allocation
    
    // Handle NULL ptr (like malloc)
    if (ptr == nullptr) {
        return my_malloc(size);
    }
    
    // Handle zero size (like free)
    if (size == 0) {
        my_free(ptr);
        return nullptr;
    }
    
    // Get old block header
    BlockHeader* old_header = get_header(ptr);
    if (old_header == nullptr) {
        return nullptr;  // Invalid pointer
    }
    
    // Calculate sizes
    size_t aligned_new_size = align_size(size);
    size_t new_total_size = aligned_new_size + sizeof(BlockHeader);
    new_total_size = align_size(new_total_size);
    
    size_t old_total_size = old_header->size;
    size_t old_user_size = old_total_size - sizeof(BlockHeader);
    
    // If new size is smaller or equal, we can just use the existing block
    // (We could shrink it, but for simplicity, we'll just keep the same size)
    if (new_total_size <= old_total_size) {
        return ptr;  // Can use existing block
    }
    
    // New size is larger - need to allocate new block and copy data
    void* new_ptr = my_malloc(size);
    if (new_ptr == nullptr) {
        return nullptr;  // Allocation failed
    }
    
    // Copy old data to new location
    // Copy the minimum of old and new sizes
    size_t copy_size = (old_user_size < aligned_new_size) ? old_user_size : aligned_new_size;
    std::memcpy(new_ptr, ptr, copy_size);
    
    // Free the old block
    my_free(ptr);
    
    return new_ptr;
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
