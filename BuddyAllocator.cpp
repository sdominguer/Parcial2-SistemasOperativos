#include "BuddyAllocator.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

BuddyAllocator::BuddyAllocator(size_t totalSize, size_t minBlockSize)
    : totalSize(totalSize), minBlockSize(minBlockSize) {
    size_t power = 1;
    while ((minBlockSize << power) <= totalSize) ++power;
    levels = power + 1;

    basePtr = std::malloc(totalSize);
    if (!basePtr) {
        std::cerr << "Error al asignar memoria base.\n";
        std::exit(1);
    }

    freeLists.resize(levels, nullptr);
    freeLists.back() = reinterpret_cast<Block*>(basePtr);
    freeLists.back()->next = nullptr;
}

BuddyAllocator::~BuddyAllocator() {
    std::free(basePtr);
}

size_t BuddyAllocator::nextPowerOfTwo(size_t n) const {
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

int BuddyAllocator::getLevel(size_t size) const {
    size_t total = nextPowerOfTwo(size);
    int level = 0;
    while (getBlockSize(level) < total && level < levels) ++level;
    return level;
}

size_t BuddyAllocator::getBlockSize(int level) const {
    return minBlockSize << level;
}

size_t BuddyAllocator::log2(size_t x) const {
    size_t res = 0;
    while (x >>= 1) ++res;
    return res;
}

void BuddyAllocator::splitBlock(int level) {
    if (level + 1 >= levels || !freeLists[level + 1]) return;

    Block* block = freeLists[level + 1];
    freeLists[level + 1] = block->next;

    size_t halfSize = getBlockSize(level);
    Block* buddy = reinterpret_cast<Block*>(
        reinterpret_cast<char*>(block) + halfSize);

    block->next = buddy;
    buddy->next = freeLists[level];
    freeLists[level] = block;
}

void* BuddyAllocator::allocate(size_t size) {
    int level = getLevel(size);

    // Buscar nivel disponible o partir bloques
    int current = level;
    while (current < levels && !freeLists[current]) ++current;

    if (current == levels) return nullptr;

    while (current > level) {
        splitBlock(current - 1);
        --current;
    }

    Block* block = freeLists[level];
    freeLists[level] = block->next;

    void* ptr = static_cast<void*>(block);
    allocatedSizes[ptr] = level;
    return ptr;
}

void BuddyAllocator::deallocate(void* ptr) {
    if (!ptr || allocatedSizes.find(ptr) == allocatedSizes.end()) return;

    int level = allocatedSizes[ptr];
    allocatedSizes.erase(ptr);

    Block* block = static_cast<Block*>(ptr);
    block->next = freeLists[level];
    freeLists[level] = block;

    tryCoalesce(ptr, level);
}

void BuddyAllocator::tryCoalesce(void* ptr, int level) {
    if (level + 1 >= levels) return;

    size_t blockSize = getBlockSize(level);
    size_t offset = reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>(basePtr);
    size_t buddyOffset = offset ^ blockSize;
    void* buddyPtr = reinterpret_cast<char*>(basePtr) + buddyOffset;

    Block** current = &freeLists[level];
    Block* prev = nullptr;

    while (*current) {
        if (*current == buddyPtr) {
            if (prev)
                prev->next = (*current)->next;
            else
                freeLists[level] = (*current)->next;

            void* mergedPtr = (offset < buddyOffset) ? ptr : buddyPtr;
            deallocate(mergedPtr); // Reinsertar como bloque superior
            return;
        }
        prev = *current;
        current = &((*current)->next);
    }
}

size_t BuddyAllocator::getUsedMemory() const {
    size_t usedMemory = 0;
    for (const auto& pair : allocatedSizes) {
        usedMemory += getBlockSize(pair.second);
    }
    return usedMemory;
}

