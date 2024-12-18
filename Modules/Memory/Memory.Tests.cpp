// Author: Jake Rieger
// Created: 12/16/2024.
//

#include "PoolAllocator.hpp"
#include "ArenaAllocator.hpp"
#include "UniquePtr.hpp"
#include "SharedPtr.hpp"
#include "WeakPtr.hpp"

#include <iostream>
#include <catch2/catch_test_macros.hpp>

using namespace x::Memory;

struct alignas(16) PoolObject {
    int id;
    int x, y;
    PoolObject(int id, int x, int y) : id(id), x(x), y(y) {}
};

struct ArenaObject {
    int x, y, z;
    ArenaObject(int x, int y, int z) : x(x), y(y), z(z) {}
};

TEST_CASE("Pool Allocator", "[Memory]") {
    constexpr size_t poolSize = 10;
    PoolAllocator pool(sizeof(PoolObject), poolSize);
    REQUIRE(pool.getCapacity() == poolSize);
    REQUIRE(pool.getObjectSize() == sizeof(PoolObject));

    PoolObject* objects[poolSize];
    for (int i = 0; i < poolSize; ++i) {
        void* memory = pool.allocate();
        objects[i]   = new (memory) PoolObject(i, i + 1, i + 1);
    }

    for (auto& object : objects) {
        REQUIRE(object != nullptr);
        REQUIRE(sizeof(*object) == sizeof(PoolObject));
        object->~PoolObject();
        pool.deallocate(object);
    }
}

TEST_CASE("Arena Allocator", "[Memory]") {
    constexpr size_t arenaSize = 1024;  // 1Kb memory block
    ArenaAllocator arena(arenaSize);
    REQUIRE(arena.getTotalSize() == arenaSize);

    void* p1 = arena.allocate(sizeof(ArenaObject));
    REQUIRE(p1 != nullptr);
    REQUIRE(arena.getOffset() == sizeof(ArenaObject));

    auto* s1 = new (p1) ArenaObject(1, 2, 3);
    REQUIRE(s1 != nullptr);
    REQUIRE(s1->x == 1);
    REQUIRE(s1->y == 2);
    REQUIRE(s1->z == 3);

    void* p2 = arena.allocate(128);
    REQUIRE(p2 != nullptr);
    auto* s2 = new (p2) char[128];
    REQUIRE(s2 != nullptr);

    void* p3 = arena.allocate(256, 16);
    REQUIRE(p3 != nullptr);
    auto* s3 = new (p3) uint16_t[256];
    REQUIRE(s3 != nullptr);

    arena.reset();
    REQUIRE(arena.getOffset() == 0);
}

TEST_CASE("Unique Pointer", "[Memory]") {
    auto arenaObj = makeUnique<ArenaObject>(1, 2, 3);
    REQUIRE(arenaObj.get() != nullptr);
    REQUIRE(arenaObj->x == 1);
    REQUIRE(arenaObj->y == 2);
    REQUIRE(arenaObj->z == 3);
    arenaObj.reset();
    REQUIRE(arenaObj.get() == nullptr);
}

TEST_CASE("Shared Pointer", "[Memory]") {
    auto obj1 = makeShared<ArenaObject>(1, 2, 3);
    REQUIRE(obj1.get() != nullptr);
    REQUIRE(obj1->x == 1);
    REQUIRE(obj1->y == 2);
    REQUIRE(obj1->z == 3);
    {
        SharedPtr<ArenaObject> obj2(obj1);
        REQUIRE(obj1.useCount() == 2);
    }
    REQUIRE(obj1.useCount() == 1);
}

TEST_CASE("Weak Pointer", "[Memory]") {
    auto sp1                 = makeShared<ArenaObject>(1, 2, 3);
    WeakPtr<ArenaObject> wp1 = sp1;
    {
        SharedPtr<ArenaObject> sp2 = wp1.lock();
        REQUIRE(sp2.get() != nullptr);
        REQUIRE(sp1.useCount() == 2);
    }
    REQUIRE(sp1.useCount() == 1);
}