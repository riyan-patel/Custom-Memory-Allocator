# Assignment: Custom Memory Allocator

## 📋 Overview

You will implement a custom memory allocator that replaces the standard `malloc()` and `free()` functions. This project will deepen your understanding of memory management, pointer arithmetic, and system-level programming.

## 🎯 Learning Objectives

By completing this assignment, you will:
1. Understand how memory allocation works at a low level
2. Implement memory pools and free lists
3. Learn about memory alignment and fragmentation
4. Gain experience with pointer arithmetic and memory manipulation
5. Practice debugging memory-related issues

## 📦 Part 1: Basic Allocator (Required)

### Task 1.1: Basic Structure
**Files to modify:** `allocator.cpp`

Implement the basic structure:
- `my_malloc(size_t size)` - Allocate memory
- `my_free(void* ptr)` - Free memory
- `my_calloc(size_t num, size_t size)` - Allocate and zero memory
- `my_realloc(void* ptr, size_t size)` - Resize memory

**Requirements:**
- Use a single large memory pool (e.g., 1MB)
- Track allocated blocks with a simple header structure
- Handle alignment (8-byte alignment recommended)

**Hint:** Start with a simple linked list of free blocks.

### Task 1.2: Block Header Structure
Design a header structure that stores:
- Size of the block
- Whether the block is free or allocated
- Pointer to next free block (if free)

**Questions to consider:**
- How do you find the header from a user pointer?
- How do you handle alignment requirements?

## 📦 Part 2: Memory Pools (Required)

### Task 2.1: Multiple Pools
**Files to modify:** `allocator.cpp`

Create separate memory pools for different size classes:
- Small blocks: 8-64 bytes
- Medium blocks: 65-256 bytes
- Large blocks: 257-1024 bytes
- Extra large: >1024 bytes

**Implementation steps:**
1. Create separate pools for each size class
2. Route allocation requests to the appropriate pool
3. Update `my_malloc()` to select the right pool

### Task 2.2: Pool Management
Implement functions to:
- Initialize each pool
- Allocate from the correct pool
- Return blocks to the correct pool on free

## 📦 Part 3: Free Lists (Required)

### Task 3.1: Free List Implementation
**Files to modify:** `allocator.cpp`

Instead of searching through all blocks, maintain free lists:
- Each size class has its own free list
- When a block is freed, add it to the appropriate free list
- When allocating, check the free list first

**Data structure options:**
- Singly linked list
- Doubly linked list (easier to merge adjacent blocks)

### Task 3.2: Coalescing
Implement block coalescing:
- When freeing a block, check if adjacent blocks are also free
- Merge adjacent free blocks into one larger block
- This reduces fragmentation!

**Challenge:** How do you find adjacent blocks?

## 📦 Part 4: Fragmentation Reduction (Required)

### Task 4.1: Best Fit vs First Fit
Implement allocation strategies:
- **First Fit**: Use the first block that's large enough
- **Best Fit**: Use the smallest block that's large enough
- Compare both strategies

### Task 4.2: Fragmentation Metrics
Add functions to track:
- Total allocated memory
- Total free memory
- Number of free blocks
- Largest free block size

## 📦 Part 5: Testing & Validation (Required)

### Task 5.1: Unit Tests
**Files to modify:** `test_allocator.cpp`

Write tests for:
- Basic allocation and deallocation
- Edge cases (0 bytes, very large allocations)
- Multiple allocations
- Free list correctness
- Coalescing behavior

### Task 5.2: Memory Leak Detection
Implement a simple leak detector:
- Track all allocated blocks
- On program exit, report any unfreed blocks
- Report total leaks

## 🚀 Part 6: Stretch Goals (Optional)

### Stretch Goal 1: Thread-Local Allocation
- Implement thread-local storage for allocations
- Reduce contention in multi-threaded programs
- Use thread-local storage (TLS) or thread-specific data

### Stretch Goal 2: Performance Benchmarking
- Compare your allocator vs standard `malloc()`
- Measure:
  - Allocation speed
  - Deallocation speed
  - Memory overhead
  - Fragmentation

### Stretch Goal 3: Fragmentation Visualization
- Create a visual representation of memory layout
- Show allocated vs free blocks
- Display fragmentation over time

## 📝 Implementation Guidelines

### Code Style
- Use clear, descriptive variable names
- Add comments explaining complex logic
- Follow consistent formatting
- Handle edge cases gracefully

### Error Handling
- Return `NULL` on allocation failure
- Validate input parameters
- Add assertions for debugging

### Testing Strategy
1. Start with simple cases (allocate, free, allocate again)
2. Test edge cases (0 bytes, NULL pointers)
3. Test stress cases (many allocations)
4. Test fragmentation scenarios
5. Compare with standard malloc

## 🔍 Debugging Tips

1. **Use a debugger**: `gdb` or `lldb` are your friends
2. **Add logging**: Print when blocks are allocated/freed
3. **Check alignment**: Use `%p` to print pointers, check if aligned
4. **Watch for undefined behavior**: 
   - Don't access freed memory
   - Don't double-free
   - Don't free memory you didn't allocate

## 📊 Grading Rubric

| Component | Points | Description |
|-----------|--------|-------------|
| Basic Allocator | 25 | malloc/free work correctly |
| Memory Pools | 25 | Multiple pools implemented |
| Free Lists | 20 | Free lists work correctly |
| Coalescing | 15 | Adjacent blocks merge |
| Testing | 10 | Comprehensive tests |
| Code Quality | 5 | Clean, commented code |
| Stretch Goals | +10 | Each stretch goal |

## 📚 Resources

- "The C Programming Language" - Memory Management chapter
- "Operating Systems: Three Easy Pieces" - Memory Management
- Linux `malloc()` implementation (glibc)
- `jemalloc` documentation

## ⚠️ Important Notes

- **Don't use `malloc()` in your allocator!** Use `mmap()` or `sbrk()` to get raw memory
- **Handle alignment**: Most systems require 8-byte alignment
- **Be careful with pointer arithmetic**: One mistake can corrupt memory
- **Test thoroughly**: Memory bugs are hard to find!

## 🎓 Submission Requirements

1. All source code files
2. Test results/output
3. Brief report explaining:
   - Your design decisions
   - Performance characteristics
   - Challenges faced
   - Future improvements

Good luck! This is a challenging but rewarding project. 🚀
