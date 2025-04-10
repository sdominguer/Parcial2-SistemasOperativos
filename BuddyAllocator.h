#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <cstddef>
#include <vector>
#include <unordered_map>

class BuddyAllocator {
public:

    BuddyAllocator(size_t totalSize, size_t minBlockSize = 128);
    ~BuddyAllocator();

    void* allocate(size_t size);
    void deallocate(void* ptr);

    size_t getUsedMemory() const;

private:
    struct Block {
        Block* next;
    };

    size_t totalSize;
    size_t minBlockSize;
    size_t levels;           // Número de niveles de bloques (log2)

    void* basePtr;           // Puntero a la memoria principal
    std::vector<Block*> freeLists;
    std::unordered_map<void*, size_t> allocatedSizes; // Mapa para liberar

    int getLevel(size_t size) const;
    size_t getBlockSize(int level) const;
    size_t nextPowerOfTwo(size_t n) const;
    void splitBlock(int level);
    void tryCoalesce(void* ptr, int level);
    size_t log2(size_t x) const;
};

#endif
