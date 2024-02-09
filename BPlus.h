//
// Created by iansg on 2/7/2024.
//

#ifndef ADS_BTREE_BTREE_H
#define ADS_BTREE_BTREE_H

#include <iostream>
#include <functional>
using namespace std;

template<typename T>
class BPlus {
    class Node {
        int n {}; // number of keys
        T** key {}; // array of keys
        Node** c {}; // array of pointers to children
        bool leaf {}; // boolean, true if it is a leaf
        friend BPlus;

    public:
        Node() : leaf(true) {}
        explicit Node(int t) {
            int m = (t<<1); // order of tree
            n = 0;
            leaf = true;

            key = new T*[m - 1];
            for (int i = 1; i < m; ++i)
                key[i - 1] = nullptr;

            c = new Node*[m];
            for (int i = 0; i < m; ++i)
                c[i] = nullptr;
        }
        ~Node() {
            for (int i = 0; i < (n<<1) - 1; ++i)
                delete key[i];
            delete[] key;

            for (int i = 0; i < (n<<1); ++i)
                delete c[i];
            delete[] c;
        }
    };

    void create_root(T k) {
        root = new Node(t);
        root->n = 1;
        root->key[0] = new T(k);
    }

    void split_root() {
        Node* s = new Node(t);
        s->leaf = false;

        s->c[0] = root;
        root = s;
        split_children(s, 0);
    }

    void split_children(Node* x, int i) {
        for (int j = x->n; j > i; --j) // shift keys right
            x->key[j] = x->key[j - 1];

        x->n += 1; // increase key count

        for (int j = x->n; j > i + 1; --j) // shift children right
            x->c[j] = x->c[j - 1];

        int m = t - 1; // median of child's keys' array
        Node* y = x->c[i];

        Node* z = new Node(t);
        for (int j = 0; j < m; ++j) // assign to z, y's greatest keys
            z->key[j] = y->key[j + t];

        for (int j = 0; j <= m; ++j) // assign to z, y's greatest children
            z->c[j] = y->c[j + t];

        x->key[i] = y->key[m]; // insert middle key in x
        x->c[i + 1] = z; // insert child in x

        z->n = t - 1;
        y->n = t - 1;

        z->leaf = y->leaf;
    }

    void insert_non_full(Node* x, T k) {
        int i = 0;
        for (; i < x->n; ++i)
            if (k <= *x->key[i]) break;

        if (i < x->n && *x->key[i] == k) return;
        if (x->leaf) {
            for (int j = i + 1; j <= x->n; ++j)
                x->key[j] = x->key[j - 1];
            x->key[i] = new T(k);
            x->n += 1;
            return;
        }

        if (x->c[i]->n == (t<<1) - 1) {
            split_children(x, i);
            return insert_non_full(x, k);
        }
        return insert_non_full(x->c[i], k);
    }

    T* predecessor(Node* x) {
        if (x->leaf) return x->key[x->n - 1];
        return predecessor(x->c[x->n]);
    }

    T* successor(Node* x) {
        if (x->leaf) return x->key[0];
        return successor(x->c[0]);
    }

    void erase_1(Node* x, int i) {
        for (int j = i + 1; j < x->n; ++j) // shift keys left
            x->key[j - 1] = x->key[j];

        x->n -= 1; // decrease key count
    }

    void erase_2a(Node* x, int i) {
        x->key[i] = predecessor(x->c[i]);
        erase(x->c[i], *x->key[i]);
    }

    void erase_2b(Node* x, int i) {
        x->key[i] = successor(x->c[i + 1]);
        erase(x->c[i + 1], *x->key[i]);
    }

    void erase_2c(Node* x, int i) {
        Node* y = x->c[i];
        Node* z = x->c[i + 1];

        y->n = (t<<1) - 1;
        y->key[t - 1] = x->key[i];
        for (int j = 0; j < t - 1; ++j) // merge keys in y
            y->key[j + t] = z->key[j];
        for (int j = 0; j < t; ++j) // merge children in y
            y->c[j + t] = z->c[j];

        for (int j = i + 1; j < x->n; ++j) // shift left keys in x
            x->key[j - 1] = x->key[j];
        for (int j = i + 1; j < x->n; ++j) // shift left children in x
            x->c[j] = x->c[j + 1];

        x->c[x->n] = nullptr;
        x->n -= 1; // reduce x's key count

        z->n = 0;
        delete z;

        erase(x->c[i], *y->key[t - 1]);
    }

    void erase_2(Node* x, int i) {
        if (i > 0 && x->c[i - 1]->n >= t) return erase_2a(x, i);
        else if (i < (t<<1) - 2 && x->c[i + 1]->n >= t) return erase_2b(x, i);
        else erase_2c(x, i);
    }

    void erase_3a(Node* x, int i, T k) {
        Node* y = x->c[i];
        if (i > 0 && x->c[i - 1]->n >= t) {
            for (int j = 0; j < y->n; ++j) // shift all keys right
                y->key[j + 1] = y->key[j];
            for (int j = 0; j <= y->n; ++j) // shift all children right
                y->c[i + 1] = y->c[i];
            y->n += 1; // update y's key count

            Node* z = x->c[i - 1]; // left child (from whom a key & child are borrowed)
            y->key[0] = x->key[i - 1]; // add key to y
            y->c[0] = z->c[z->n]; // add child to y

            z->n -= 1;
            x->key[i - 1] = z->key[z->n];
            z->c[z->n] = nullptr;

            erase(x->c[i], k);
        }
        else {
            Node* z = x->c[i + 1]; // right child (from whom a key & child are borrowed)

            y->key[y->n] = x->key[i]; // add key to y
            y->n += 1;
            y->c[y->n] = z->c[0]; // add child to y

            x->key[i] = z->key[0];
            for (int j = 1; j < z->n; ++j)
                z->key[j - 1] = z->key[j];
            for (int j = 1; j <= z->n; ++j)
                z->c[j - 1] = z->c[j];

            z->c[z->n] = nullptr;
            z->n -= 1;

            erase(x->c[i], k);
        }
    }

    void erase_3b(Node* x, int i, T k) {
        if (i > 0) {
            Node* y = x->c[i - 1];
            y->key[y->n] = x->key[i - 1]; // add median key to left child
            y->n += 1;

            Node* z = x->c[i];
            for (int j = 0; j < z->n; ++j) // pass keys to left child
                y->key[j + y->n] = z->key[j];
            y->n += 1;
            for (int j = 0; j <= z->n; ++j) // pass children to left child
                y->c[j + y->n] = z->c[j];
            y->n = (t<<1) - 1;

            for (int j = i; j < x->n; ++j) // shift x's keys left
                x->key[j - 1] = x->key[j];
            for (int j = i + 1; j <= x->n; ++j) // shift x's children left
                x->c[j - 1] = x->c[j];
            x->n -= 1;

            z->n = 0;
            delete z;

            erase(x->c[i - 1], k);
        }
        else {
            Node* y = x->c[i];
            y->key[y->n] = x->key[i]; // add median key to child
            y->n += 1;

            Node* z = x->c[i + 1];
            for (int j = 0; j < z->n; ++j) // receive keys from right child
                y->key[j + y->n] = z->key[j];
            y->n += 1;
            for (int j = 0; j <= z->n; ++j) // receive children from right child
                y->c[j + y->n] = z->c[j];
            y->n = (t<<1) - 1;

            for (int j = i + 1; j < x->n; ++j) // shift x's keys left
                x->key[j - 1] = x->key[j];
            for (int j = i + 2; j <= x->n; ++j) // shift x's children left
                x->c[j - 1] = x->c[j];
            x->n -= 1;

            z->n = 0;
            delete z;

            erase(x->c[i], k);
        }
    }

    void erase_3(Node* x, int i, T k) {
        if ((i > 0 && x->c[i - 1]->n >= t) || (i < (t<<1) - 2 && x->c[i + 1]->n >= t)) return erase_3a(x, i, k);
        return erase_3b(x, i, k);
    }

    void erase(Node* x, T k) {
        int i = 0;
        for (; i < x->n; ++i)
            if (k <= *x->key[i]) break;

        if (i < x->n && *x->key[i] == k) {
            if (x->leaf) return erase_1(x, i);
            return erase_2(x, i);
        }

        if (x->leaf) return;

        if (x->c[i]->n >= t) return erase(x->c[i], k);
        return erase_3(x, i, k);
    }

    bool search(Node* x, T k) {
        int i = 0;
        for (; i < x->n; ++i)
            if (k <= *x->key[i]) break;

        if (i < x->n && *x->key[i] == k) return true;
        if (x->leaf) return false;
        return search(x->c[i], k);
    }

    void traverse(Node* x, function<void(T)> process) {
        if (x == nullptr) return;

        int i = 0;
        for (; i < x->n; ++i) {
            traverse(x->c[i], process);
            process(*x->key[i]);
        }
        if (!x->leaf) traverse(x->c[i], process);
    }

    int t {};
    Node* root {};

public:
    explicit BPlus(int t) : t(t) {}
    ~BPlus() { clear(); }
    void clear() {}
    void insert(T k) {
        if (root == nullptr) return create_root(k);
        if (root->n == (t<<1) - 1) split_root();
        insert_non_full(root, k);
    }
    void erase(T k) {
        if (root == nullptr) return;
        erase(root, k);
        if (root->n == 0) {
            Node* temp = root;
            root = (root->leaf ? nullptr : root->c[0]);
            delete temp;
        }
    }
    bool search(T k) {
        if (root == nullptr) return false;
        return search(root, k);
    }
    void traverse(function<void(T)> process) {
        if (root == nullptr) return;
        return traverse(root, process);
    }
};

#endif //ADS_BTREE_BTREE_H
