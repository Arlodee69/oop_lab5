#ifndef CUSTOM_LIST_HPP
#define CUSTOM_LIST_HPP

#include <memory_resource>
#include <iterator>
#include <iostream>

template <typename T>
class CustomList {
private:
    struct Node {
        T value;
        Node* prev;
        Node* next;
    };

    std::pmr::polymorphic_allocator<Node> alloc;
    Node* head = nullptr;
    Node* tail = nullptr;
    size_t sz = 0;

public:
    class Iterator {
    private:
        Node* ptr;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;

        Iterator(Node* p) : ptr(p) {}

        reference operator*()  const { return ptr->value; }
        pointer   operator->() const { return &ptr->value; }

        Iterator& operator++() {
            if (ptr) ptr = ptr->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
    };
    explicit CustomList(std::pmr::memory_resource* mem_res) : alloc(mem_res)
    {}

    ~CustomList() {
        clear();
    }

    void push_back(const T& value) {
        Node* node = alloc.allocate(1);
        alloc.construct(node, Node{ value, tail, nullptr });

        if (!head) head = node;
        if (tail) tail->next = node;

        tail = node;
        sz++;
    }

    void push_front(const T& value) {
        Node* node = alloc.allocate(1);
        alloc.construct(node, Node{ value, nullptr, head });

        if (!tail) tail = node;
        if (head) head->prev = node;

        head = node;
        sz++;
    }

    size_t size() const { return sz; }

    void clear() {
        Node* cur = head;
        while (cur) {
            Node* nxt = cur->next;
            alloc.destroy(cur);
            alloc.deallocate(cur, 1);
            cur = nxt;
        }
        head = tail = nullptr;
        sz = 0;
    }
    Iterator begin() { return Iterator(head); }
    Iterator end()   { return Iterator(nullptr); }
};

#endif
