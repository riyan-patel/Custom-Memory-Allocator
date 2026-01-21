# Custom Memory Allocator Project

## 🎯 What is This Project? (Explained Like You're 10 Years Old)

Imagine you have a toy box, and you want to organize your toys better. Right now, when you ask for a toy (like asking for memory in a computer), someone just throws toys randomly into the box. When you're done playing, you throw it back in, but everything gets messy!

**A memory allocator is like a super-organized toy organizer:**

1. **Memory Pools** = Different boxes for different sized toys
   - Small toys go in the small box
   - Medium toys go in the medium box
   - Big toys go in the big box
   - This makes finding toys faster!

2. **Free Lists** = A list of empty spaces
   - When you put a toy back, you write down "Space #5 is empty now"
   - Next time you need that size, you check your list first
   - No need to search the whole box!

3. **Fragmentation Reduction** = Keeping things neat
   - Instead of having tiny empty spaces everywhere (like puzzle pieces scattered)
   - You combine small empty spaces into bigger ones
   - Like organizing puzzle pieces back into the box properly!

**Why is this cool?**
- Computers use `malloc()` and `free()` to get memory (like asking for toys)
- But the default way is slow and messy
- You're building a FASTER, SMARTER way to organize memory!
- It's like building your own super-organized toy system!

## 📚 What You'll Learn

- **Heap vs Stack**: Where memory lives in your computer
- **Pointer Arithmetic**: Math with memory addresses
- **Alignment**: Making sure things fit properly (like Tetris pieces)
- **False Sharing**: When things accidentally slow each other down
- **Cache Lines**: How your computer's brain remembers things
- **Undefined Behavior**: Tricky bugs that are hard to find!

## 🏗️ Project Structure

```
Custom Memory Allocator/
├── README.md              (This file!)
├── ASSIGNMENT.md          (Detailed instructions)
├── allocator.h            (Function declarations)
├── allocator.cpp          (Your implementation)
├── test_allocator.cpp     (Tests to verify your code)
└── Makefile               (Build instructions)
```

## 🚀 Quick Start

1. Read `ASSIGNMENT.md` for detailed instructions
2. Implement functions in `allocator.cpp`
3. Run `make` to build
4. Run `make test` to test your implementation
5. Compare performance with `make benchmark`

## 📝 Project Goals

- [ ] Implement basic `my_malloc()` and `my_free()`
- [ ] Create memory pools for different sizes
- [ ] Build free lists to track available memory
- [ ] Reduce fragmentation
- [ ] Test and benchmark your allocator
- [ ] (Stretch) Add thread-local allocation
- [ ] (Stretch) Visualize fragmentation

## 🎓 This is a College-Level Project!

This project demonstrates:
- Deep understanding of memory management
- Systems programming skills
- Performance optimization
- Debugging complex issues

**Recruiters notice this!** Very few students attempt custom allocators.

Good luck! 🚀
