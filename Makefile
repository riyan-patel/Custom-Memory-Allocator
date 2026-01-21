# Makefile for Custom Memory Allocator Project

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
# Use -O2 for performance testing, -O0 for debugging

# Directories
SRC_DIR = .
BUILD_DIR = build

# Source files
ALLOCATOR_SRC = $(SRC_DIR)/allocator.cpp
TEST_SRC = $(SRC_DIR)/test_allocator.cpp

# Object files
ALLOCATOR_OBJ = $(BUILD_DIR)/allocator.o
TEST_OBJ = $(BUILD_DIR)/test_allocator.o

# Executables
TEST_EXEC = $(BUILD_DIR)/test_allocator
BENCHMARK_EXEC = $(BUILD_DIR)/benchmark

# Default target
all: $(TEST_EXEC)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build allocator object file
$(ALLOCATOR_OBJ): $(ALLOCATOR_SRC) $(SRC_DIR)/allocator.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build test executable
$(TEST_EXEC): $(TEST_OBJ) $(ALLOCATOR_OBJ) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Build test object file
$(TEST_OBJ): $(TEST_SRC) $(SRC_DIR)/allocator.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
test: $(TEST_EXEC)
	@echo "Running test suite..."
	./$(TEST_EXEC)

# Run with valgrind (memory leak detection)
valgrind: $(TEST_EXEC)
	@echo "Running with valgrind..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_EXEC)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Rebuild everything
rebuild: clean all

# Debug build (with debug symbols and no optimization)
debug: CXXFLAGS += -DDEBUG -g3
debug: $(TEST_EXEC)

# Release build (optimized)
release: CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -DNDEBUG
release: clean $(TEST_EXEC)

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build test executable (default)"
	@echo "  test      - Build and run tests"
	@echo "  valgrind  - Run tests with valgrind (memory leak detection)"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized version"
	@echo "  clean     - Remove build files"
	@echo "  rebuild   - Clean and rebuild"
	@echo "  help      - Show this help message"

.PHONY: all test valgrind clean rebuild debug release help
