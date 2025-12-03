#include <gtest/gtest.h>
#include "../include/header.hpp"
#include "../include/list.hpp"

class TestMemoryResource : public customMemoryResource {
public:
    int allocCount = 0;
    int deallocCount = 0;

protected:
    void* do_allocate(size_t bytes, size_t alignment) override {
        allocCount++;
        return customMemoryResource::do_allocate(bytes, alignment);
    }

    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override {
        deallocCount++;
        customMemoryResource::do_deallocate(ptr, bytes, alignment);
    }
};

//Тесты контейнера (Forward Iterator)

TEST(CustomListTest, HandlesEmptyList) {
    TestMemoryResource mem;
    CustomList<int> lst(&mem);

    EXPECT_EQ(lst.size(), 0);
    EXPECT_EQ(lst.begin(), lst.end());

    for (auto it = lst.begin(); it != lst.end(); ++it) {
        FAIL() << "Empty list must not iterate";
    }
}

TEST(CustomListTest, PushAndIterateForward) {
    TestMemoryResource mem;
    CustomList<int> lst(&mem);

    lst.push_back(1);
    lst.push_back(2);
    lst.push_back(3);

    std::vector<int> v;
    for (auto it = lst.begin(); it != lst.end(); ++it)
        v.push_back(*it);

    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}

TEST(CustomListTest, PushFrontIterateForward) {
    TestMemoryResource mem;
    CustomList<int> lst(&mem);

    lst.push_front(30);
    lst.push_front(20);
    lst.push_front(10);

    std::vector<int> result;
    for (auto it = lst.begin(); it != lst.end(); ++it)
        result.push_back(*it);

    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 10);
    EXPECT_EQ(result[1], 20);
    EXPECT_EQ(result[2], 30);
}

TEST(CustomListTest, SizeCheck) {
    TestMemoryResource mem;
    CustomList<int> lst(&mem);

    lst.push_back(5);
    lst.push_back(10);

    EXPECT_EQ(lst.size(), 2);
}

TEST(CustomListTest, IsForwardIterator) {
    TestMemoryResource mem;
    CustomList<int> lst(&mem);

    lst.push_back(1);
    lst.push_back(2);
    lst.push_back(3);

    auto it1 = lst.begin();
    auto it2 = it1;
    EXPECT_EQ(*it1, *it2);

    ++it1;
    EXPECT_NE(*it1, *it2);

    while (it1 != lst.end()) ++it1;
    EXPECT_EQ(it1, lst.end());
}
struct ComplexData {
    int id;
    double value;
};

TEST(CustomListTest, WorkWithComplexType) {
    TestMemoryResource mem;
    CustomList<ComplexData> lst(&mem);

    lst.push_back({1, 1.5});
    lst.push_back({2, 2.5});

    auto it = lst.begin();
    EXPECT_EQ(it->id, 1);
    EXPECT_DOUBLE_EQ(it->value, 1.5);

    ++it;
    EXPECT_EQ(it->id, 2);
    EXPECT_DOUBLE_EQ(it->value, 2.5);
}

TEST(CustomListTest, ListDestructorReleasesMemory) {
    TestMemoryResource mem;

    {
        CustomList<int> lst(&mem);
        lst.push_back(10);
        lst.push_back(20);
        lst.push_back(30);

        EXPECT_EQ(mem.allocCount, 3);
        EXPECT_EQ(mem.deallocCount, 0);
    }

    EXPECT_EQ(mem.deallocCount, 3);
}

//Тесты Memory Resource работы

TEST(MemoryResourceTest, AllocateDeallocateCounts) {
    TestMemoryResource mem;
    {
        CustomList<int> lst(&mem);

        lst.push_back(1);
        lst.push_back(2);
        lst.push_back(3);

        EXPECT_EQ(mem.allocCount, 3);
        EXPECT_EQ(mem.deallocCount, 0);
    }
    EXPECT_EQ(mem.deallocCount, 3);
}

TEST(MemoryResourceTest, ReuseFreedBlocks) {
    TestMemoryResource mem;

    {
        CustomList<int> lst(&mem);
        lst.push_back(10);
        lst.push_back(20);

        EXPECT_EQ(mem.allocCount, 2);
    }

    EXPECT_EQ(mem.deallocCount, 2);

    {
        CustomList<int> lst2(&mem);
        lst2.push_back(5);
        lst2.push_back(6);

        EXPECT_EQ(mem.allocCount, 4);
    }
}

TEST(CustomListTest, PolymorphicAllocatorWorks) {
    TestMemoryResource mem;
    std::pmr::polymorphic_allocator<int> alloc(&mem);

    int* p = alloc.allocate(5);
    EXPECT_NE(p, nullptr);

    alloc.deallocate(p, 5);
    EXPECT_EQ(mem.allocCount, 1);
    EXPECT_EQ(mem.deallocCount, 1);
}

TEST(CustomMemoryResourceTest, CleansUpOnDestruction) {
    auto* mr = new TestMemoryResource();

    {
        CustomList<int> lst(mr);
        lst.push_back(10);
        lst.push_back(20);
    }

    EXPECT_EQ(mr->allocCount, 2);
    EXPECT_EQ(mr->deallocCount, 2);

    delete mr;
}
