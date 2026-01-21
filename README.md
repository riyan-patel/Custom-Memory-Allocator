# Custom Memory Allocator

A high-performance custom memory allocator implementation in C++ that replaces the standard `malloc()` and `free()` functions. This project demonstrates advanced systems programming concepts including memory pool management, free list optimization, and fragmentation reduction strategies.

## Overview

This allocator implements a custom dynamic memory management system with the following key features:

### Core Components

1. **Memory Pools**
   - Segregated size classes for efficient allocation
   - Separate pools for small (8-64 bytes), medium (65-256 bytes), large (257-1024 bytes), and extra-large (>1024 bytes) allocations
   - Reduces search time and improves cache locality

2. **Free List Management**
   - Maintains linked lists of available memory blocks per size class
   - Enables O(1) allocation for common sizes
   - Eliminates the need for linear search through memory

3. **Fragmentation Reduction**
   - Block coalescing to merge adjacent free blocks
   - Reduces external fragmentation
   - Implements both first-fit and best-fit allocation strategies

## Technical Concepts

This project covers advanced systems programming topics:

- **Heap Management**: Understanding the heap memory model and its trade-offs
- **Pointer Arithmetic**: Low-level memory address manipulation
- **Memory Alignment**: Ensuring proper data alignment for performance and correctness
- **False Sharing**: Cache line contention in multi-threaded environments
- **Cache Locality**: Optimizing memory access patterns
- **Undefined Behavior**: Identifying and avoiding memory-related undefined behavior

## Project Structure

```
Custom-Memory-Allocator/
├── README.md              Project overview and documentation
├── ASSIGNMENT.md          Detailed implementation requirements
├── QUICKSTART.md          Step-by-step implementation guide
├── allocator.h            Header file with API and data structures
├── allocator.cpp          Core allocator implementation
├── test_allocator.cpp     Comprehensive test suite
└── Makefile               Build system and test runner
```

## Building and Testing

### Prerequisites
- C++17 compatible compiler (GCC or Clang)
- Make build system
- (Optional) Valgrind for memory leak detection

### Build Commands

```bash
# Build the project
make

# Run test suite
make test

# Run with memory leak detection
make valgrind

# Build optimized release version
make release

# Clean build artifacts
make clean
```

## Implementation Status

### Core Features
- [x] Project structure and framework
- [ ] Basic `my_malloc()` and `my_free()` implementation
- [ ] Memory pool initialization and management
- [ ] Free list data structure
- [ ] Block coalescing algorithm
- [ ] Alignment handling
- [ ] `my_calloc()` and `my_realloc()` support

### Advanced Features (Stretch Goals)
- [ ] Thread-local allocation pools
- [ ] Performance benchmarking vs. standard malloc
- [ ] Fragmentation visualization tools
- [ ] Memory leak detection and reporting

## Academic and Professional Value

This project demonstrates proficiency in:

- **Systems Programming**: Low-level memory management and system calls
- **Algorithm Design**: Efficient data structures for memory allocation
- **Performance Optimization**: Cache-aware design and allocation strategies
- **Debugging Skills**: Identifying and resolving memory-related issues
- **Software Engineering**: Well-structured, testable, and maintainable code

Custom memory allocators are rarely implemented by students, making this project particularly noteworthy for technical interviews and portfolio reviews.

## Documentation

- See `ASSIGNMENT.md` for detailed implementation requirements and grading rubric
- See `QUICKSTART.md` for a step-by-step implementation guide
- Code comments in `allocator.h` and `allocator.cpp` provide implementation guidance

## License

This project is part of an academic assignment and is intended for educational purposes.

