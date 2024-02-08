//
// Created by iansg on 2/6/2024.
//

#ifndef ADS_HW5_TRIE_H
#define ADS_HW5_TRIE_H

#include<string>
using namespace std;

class Trie {
    class Node {
        char val {};
        Node* next {};
        Node* child {};
        bool endOfWord {};
        friend Trie;
    public:
        Node() = default;
        explicit Node(char value) : val(value) {}
    };

    void _first_insert(Node* cur, const string& s, int i) {
        int n = (int)s.size();
        for (; i < n; ++i) {
            cur->child = new Node(s[i]);
            cur = cur->child;
        }
        cur->endOfWord = true;
    }

    void _insert(Node* cur, const string& s, int i) {
        if (i == (int)s.size()) {
            cur->endOfWord = true;
            return;
        }

        while (cur->next) {
            if (cur->val == s[i]) break;
            cur = cur->next;
        }
        if (cur->val != s[i]) {
            cur->next = new Node(s[i]);
            _first_insert(cur->next, s, ++i);
        }
        else {
            if (cur->child) _insert(cur->child, s, ++i);
            else _first_insert(cur, s, ++i);
        }
    }

    Node* _erase(Node* cur, const string& s, int i) {
        if (cur == nullptr) return cur;
        if (i == (int)s.size() - 1) {
            if (!cur->endOfWord) return cur;
            else if (cur->child) {
                cur->endOfWord = false;
                return cur;
            }
            else if (cur->next) {
                Node* nxt = cur->next;
                delete cur;
                return nxt;
            }
            else {
                delete cur;
                return nullptr;
            }
        }

        while (cur->next) {
            if (cur->val == s[i]) break;
            cur = cur->next;
        }
        if (cur->val != s[i] || !cur->child) return cur;

        cur->child = _erase(cur->child, s, ++i);

        if (cur->child || cur->endOfWord) return cur;
        else if (cur->next) {
            Node* nxt = cur->next;
            delete cur;
            return nxt;
        }
        else {
            delete cur;
            return nullptr;
        }
    }

    bool _search(Node* cur, const string& s, int i) {
        if (cur == nullptr) return false;

        while (cur->next) {
            if (cur->val == s[i]) break;
            cur = cur->next;
        }
        if (cur->val == s[i]) return (i == (int)s.size() - 1 && cur->endOfWord) || _search(cur->child, s, ++i);
        return false;
    }

    void _clear(Node* cur) {
        if (cur == nullptr) return;
        _clear(cur->child);
        _clear(cur->next);
        delete cur;
    }

    Node* root {};
public:
    Trie() = default;
    ~Trie() {
        clear();
    }
    void insert(const string& s) {
        if (s.empty()) return;
        if (root) _insert(root, s, 0);
        else {
            root = new Node(s[0]);
            _first_insert(root, s, 1);
        }
    }
    void erase(const string& s) {
        if (s.empty()) return;
        _erase(root, s, 0);
    }
    bool search(const string& s) {
        return !s.empty() && _search(root, s, 0);
    }
    void clear() { _clear(root); }
};

#endif //ADS_HW5_TRIE_H
