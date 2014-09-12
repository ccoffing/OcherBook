#include "clc/data/List.h"


TEST_CASE("constructor")
{
    PRINT_PROGRESS;
    clc::List l;
    CHECK(l.isEmpty());
    CHECK(0U == l.size());
}

TEST_CASE("addIndex")
{
    PRINT_PROGRESS;
    clc::List l;
    l.add((void*)123);
    l.add((void*)789);
    l.addAt((void*)456, 1);

    CHECK((void*)123 == l.get(0));
    CHECK((void*)456 == l.get(1));
    CHECK((void*)789 == l.get(2));

}

TEST_CASE("hasItem")
{
    PRINT_PROGRESS;
    clc::List l;
    CHECK(! l.hasItem((void*)0));
    l.add((void*)0);
    CHECK(l.hasItem((void*)0));
    CHECK(! l.hasItem((void*)123));
    l.add((void*)123);
    CHECK(l.hasItem((void*)123));
}

TEST_CASE("clear")
{
    PRINT_PROGRESS;
    clc::List l;
    CHECK(l.isEmpty());
    l.add((void*)123);
    CHECK(! l.isEmpty());
    l.clear();
    CHECK(l.isEmpty());
}

TEST_CASE("mixed")
{
    PRINT_PROGRESS;
    clc::List l;

    l.add((void*)123);
    CHECK(! l.isEmpty());
    CHECK(1U == l.size());
    CHECK((void*)123 == l.get(0));

    l.add((void*)456);
    CHECK(2U == l.size());
    CHECK((void*)123 == l.get(0));
    CHECK((void*)456 == l.get(1));

    l.replaceItem(1, (void*)789);
    CHECK((void*)123 == l.get(0));
    CHECK((void*)789 == l.get(1));

    l.removeAt(0U);
    CHECK(1U == l.size());
    CHECK((void*)789 == l.get(0));

    l.clear();
    CHECK(l.isEmpty());
}

// TODO:
// void* remove()
// void* remove(unsigned int index)
// bool remove(void* o)
// void set(unsigned int index, void* o)
