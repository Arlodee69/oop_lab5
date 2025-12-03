#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <memory_resource>
#include <vector>
#include <algorithm>
#include <stdexcept>

class customMemoryResource : public std::pmr::memory_resource {
private:
    static constexpr size_t BUFFER_SIZE = 1024;
    char memory_buffer[BUFFER_SIZE];

    struct Block {
        size_t offset;
        size_t size;
        bool is_free;
    };

    std::vector<Block> blocks;

public:

    void* do_allocate(size_t bytes, size_t alignment) override {
        if (bytes == 0) return nullptr;

        for (auto& block : blocks) {
            if (block.is_free && block.size >= bytes) {
                block.is_free = false;
                std::cout << "Переиспользование: смещение " << block.offset << ", размер " << bytes << " байт" << std::endl;
                return memory_buffer + block.offset;
            }
        }

        size_t alloc_offset = 0;
        for (const auto& block : blocks) {
            if (alloc_offset + bytes <= block.offset) {
                break;
            } else {
                alloc_offset = block.offset + block.size;
            }
        }

        if (alloc_offset + bytes > BUFFER_SIZE) {
            throw std::bad_alloc();
        }

        blocks.push_back({alloc_offset, bytes, false});
        std::sort(blocks.begin(), blocks.end(), [](const Block& a, const Block& b) {
            return a.offset < b.offset;
        });

        std::cout << "Выделение: смещение " << alloc_offset << ", размер " << bytes << " байт" << std::endl;
        return memory_buffer + alloc_offset;
    }

    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override {
        if (!ptr) return;

        size_t offset = static_cast<char*>(ptr) - memory_buffer;

        auto it = std::find_if(blocks.begin(), blocks.end(), [offset](const Block& b) {
            return b.offset == offset;
        });

        if (it == blocks.end()) {
            throw std::logic_error("Попытка освобождения не выделенного блока.");
        }

        it->is_free = true;
        std::cout << "Освобождение: адрес " << ptr << ", размер " << bytes << " байт" << std::endl;
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};

#endif