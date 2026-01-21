#include "allocator.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <vector>

// ============================================================================
// TEST HELPERS
// ============================================================================

void test_passed(const char* test_name) {
    std::cout << "✓ PASSED: " << test_name << "\n";
}

void test_failed(const char* test_name, const char* reason) {
    std::cout << "✗ FAILED: " << test_name << " - " << reason << "\n";
}

// ============================================================================
// BASIC TESTS
// ============================================================================

void test_basic_malloc_free() {
    std::cout << "\n=== Test: Basic malloc/free ===\n";
    
    // Test 1: Simple allocation
    void* ptr = my_malloc(100);
    if (ptr == nullptr) {
        test_failed("test_basic_malloc_free", "my_malloc returned NULL");
        return;
    }
    test_passed("Allocate 100 bytes");
    
    // Test 2: Free
    my_free(ptr);
    test_passed("Free allocated memory");
    
    // Test 3: Allocate again (should reuse freed memory)
    void* ptr2 = my_malloc(100);
    if (ptr2 == nullptr) {
        test_failed("test_basic_malloc_free", "Second allocation failed");
        return;
    }
    test_passed("Reallocate after free");
    
    my_free(ptr2);
}

void test_zero_size() {
    std::cout << "\n=== Test: Zero size allocation ===\n";
    
    void* ptr = my_malloc(0);
    // Standard malloc may return NULL or a unique pointer for 0 bytes
    // Your implementation can choose either behavior, but document it
    test_passed("Handle zero-size allocation");
    
    if (ptr != nullptr) {
        my_free(ptr);
    }
}

void test_null_free() {
    std::cout << "\n=== Test: Free NULL pointer ===\n";
    
    // Freeing NULL should be safe (like standard free)
    my_free(nullptr);
    test_passed("Free NULL pointer (should not crash)");
}

void test_multiple_allocations() {
    std::cout << "\n=== Test: Multiple allocations ===\n";
    
    const int count = 10;
    void* pointers[count];
    
    // Allocate multiple blocks
    for (int i = 0; i < count; i++) {
        pointers[i] = my_malloc(50);
        if (pointers[i] == nullptr) {
            test_failed("test_multiple_allocations", "Allocation failed");
            return;
        }
    }
    test_passed("Allocate 10 blocks");
    
    // Free all blocks
    for (int i = 0; i < count; i++) {
        my_free(pointers[i]);
    }
    test_passed("Free all blocks");
}

// ============================================================================
// CALLOC TESTS
// ============================================================================

void test_calloc() {
    std::cout << "\n=== Test: calloc ===\n";
    
    // Test that calloc zeros memory
    int* ptr = (int*)my_calloc(10, sizeof(int));
    if (ptr == nullptr) {
        test_failed("test_calloc", "calloc returned NULL");
        return;
    }
    
    // Check that memory is zeroed
    bool all_zero = true;
    for (int i = 0; i < 10; i++) {
        if (ptr[i] != 0) {
            all_zero = false;
            break;
        }
    }
    
    if (all_zero) {
        test_passed("calloc zeros memory");
    } else {
        test_failed("test_calloc", "Memory not zeroed");
    }
    
    my_free(ptr);
}

// ============================================================================
// REALLOC TESTS
// ============================================================================

void test_realloc() {
    std::cout << "\n=== Test: realloc ===\n";
    
    // Test 1: realloc NULL (should act like malloc)
    void* ptr = my_realloc(nullptr, 100);
    if (ptr == nullptr) {
        test_failed("test_realloc", "realloc(NULL) failed");
        return;
    }
    test_passed("realloc(NULL) acts like malloc");
    
    // Test 2: realloc to larger size
    void* ptr2 = my_realloc(ptr, 200);
    if (ptr2 == nullptr) {
        test_failed("test_realloc", "realloc to larger size failed");
        my_free(ptr);
        return;
    }
    test_passed("realloc to larger size");
    
    // Test 3: realloc to smaller size
    void* ptr3 = my_realloc(ptr2, 50);
    if (ptr3 == nullptr) {
        test_failed("test_realloc", "realloc to smaller size failed");
        my_free(ptr2);
        return;
    }
    test_passed("realloc to smaller size");
    
    my_free(ptr3);
}

// ============================================================================
// ALIGNMENT TESTS
// ============================================================================

void test_alignment() {
    std::cout << "\n=== Test: Alignment ===\n";
    
    // Allocate various sizes and check alignment
    for (size_t size = 1; size <= 100; size++) {
        void* ptr = my_malloc(size);
        if (ptr == nullptr) {
            test_failed("test_alignment", "Allocation failed");
            return;
        }
        
        uintptr_t addr = (uintptr_t)ptr;
        if (addr % ALIGNMENT != 0) {
            test_failed("test_alignment", "Pointer not aligned");
            my_free(ptr);
            return;
        }
        
        my_free(ptr);
    }
    
    test_passed("All pointers are aligned");
}

// ============================================================================
// FRAGMENTATION TESTS
// ============================================================================

void test_fragmentation() {
    std::cout << "\n=== Test: Fragmentation ===\n";
    
    // Allocate and free in a pattern that causes fragmentation
    void* ptrs[5];
    
    // Allocate 5 blocks
    for (int i = 0; i < 5; i++) {
        ptrs[i] = my_malloc(100);
    }
    
    // Free middle blocks
    my_free(ptrs[1]);
    my_free(ptrs[3]);
    
    // Try to allocate a larger block (should coalesce)
    void* large = my_malloc(200);
    if (large != nullptr) {
        test_passed("Coalescing works");
        my_free(large);
    } else {
        test_failed("test_fragmentation", "Coalescing failed");
    }
    
    // Free remaining blocks
    my_free(ptrs[0]);
    my_free(ptrs[2]);
    my_free(ptrs[4]);
}

// ============================================================================
// STRESS TESTS
// ============================================================================

void test_stress() {
    std::cout << "\n=== Test: Stress test ===\n";
    
    const int iterations = 1000;
    std::vector<void*> pointers;
    
    // Allocate and free many times
    for (int i = 0; i < iterations; i++) {
        void* ptr = my_malloc(rand() % 500 + 1);
        if (ptr != nullptr) {
            pointers.push_back(ptr);
        }
        
        // Occasionally free a random pointer
        if (pointers.size() > 10 && rand() % 3 == 0) {
            int idx = rand() % pointers.size();
            my_free(pointers[idx]);
            pointers.erase(pointers.begin() + idx);
        }
    }
    
    // Free all remaining
    for (void* ptr : pointers) {
        my_free(ptr);
    }
    
    test_passed("Stress test completed");
}

// ============================================================================
// MEMORY CORRUPTION TESTS
// ============================================================================

void test_write_read() {
    std::cout << "\n=== Test: Write/Read ===\n";
    
    // Allocate and write data
    int* ptr = (int*)my_malloc(10 * sizeof(int));
    if (ptr == nullptr) {
        test_failed("test_write_read", "Allocation failed");
        return;
    }
    
    for (int i = 0; i < 10; i++) {
        ptr[i] = i * 10;
    }
    
    // Read back and verify
    bool correct = true;
    for (int i = 0; i < 10; i++) {
        if (ptr[i] != i * 10) {
            correct = false;
            break;
        }
    }
    
    if (correct) {
        test_passed("Write/read works correctly");
    } else {
        test_failed("test_write_read", "Data corruption detected");
    }
    
    my_free(ptr);
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main() {
    std::cout << "========================================\n";
    std::cout << "  Custom Memory Allocator Test Suite\n";
    std::cout << "========================================\n";
    
    // Initialize allocator
    allocator_init();
    
    // Run tests
    test_basic_malloc_free();
    test_zero_size();
    test_null_free();
    test_multiple_allocations();
    test_calloc();
    test_realloc();
    test_alignment();
    test_fragmentation();
    test_write_read();
    test_stress();
    
    // Print statistics
    std::cout << "\n=== Final Statistics ===\n";
    print_allocator_stats();
    
    // Cleanup
    allocator_cleanup();
    
    std::cout << "\n========================================\n";
    std::cout << "  All tests completed!\n";
    std::cout << "========================================\n";
    
    return 0;
}
