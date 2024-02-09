#include <iostream>
#include "Trie.h"
#include "BTree.h"
#include <cassert>
using namespace std;

// TRIE
void test_insert_search() {
    Trie trie;
    trie.insert("hello");
    trie.insert("world");
    trie.insert("bye");
    assert(trie.search("nice") == false);
    trie.insert("nice");
    assert(trie.search("hello") == true);
    assert(trie.search("nice") == true);
}

void test_erase() {
    Trie trie;
    trie.insert("w");
    trie.insert("world");
    trie.insert("worm");
    assert(trie.search("worm") == true);
    trie.insert("wos");
    assert(trie.search("wos") == true);
    trie.erase("world");
    assert(trie.search("world") == false);
    assert(trie.search("worl") == false);
    assert(trie.search("worm") == true);
    assert(trie.search("w") == true);
    assert(trie.search("a") == false);
}

// BTree
void test() {
    BTree<char> bt (3);
    bt.insert('b');
    bt.insert('a');
    bt.insert('c');
    bt.insert('d');
    bt.insert('e');
    bt.insert('f');
    bt.erase('f');
    bt.erase('c');
    bt.traverse([](char k)->void{ cout << k << ' ';});
    assert(bt.search('a') == true);
    assert(bt.search('e') == true);
    assert(bt.search('f') == false);
    assert(bt.search('c') == false);
}

int main() {
//    test_insert_search();
//    test_erase();
    test();
    return 0;
}
