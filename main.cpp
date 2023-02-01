#define __CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#include <iostream>
#include <vector>
#include "Map.h"

using std::cout;
using std::endl;
using lab::Map;
using lab::Pair;

void TestMap() {
    Map<std::string, int> data;
    data.emplace("key8", 8);
    data.emplace("key9", 9);
    data.emplace("key4", 4);
    data.emplace("key2", 2);
    data.emplace("key6", 6);
    data.emplace("key5", 5);
    data.emplace("key7", 7);
    cout << data.toJSON() << std::endl;
    data.erase("key1");
    data.erase("key4");
    cout << data.keys() << endl;
    for (auto elem : data) cout << elem << endl;
    data.clear();
    std::cout << "data is clear" << std::endl;
    for (auto elem : data) cout << elem << endl;
    data.emplace("key1", 1);
    data.emplace("key2", 2);
    cout << "cout data[3]: " << data[3] << endl;
    cout << data.toJSON();
}

int main() {
    TestMap();
    //const Map<std::string, int> dataConst{}

    _CrtDumpMemoryLeaks();
    return 0;
}