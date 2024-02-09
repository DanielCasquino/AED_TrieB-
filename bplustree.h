#include <iostream>
using namespace std;

// A BTree node
class BTreeNode
{
	int *keys;			  // An array of keys
	int t;				  // Minimum degree (defines the range for number of keys)
	BTreeNode **children; // An array of child pointers
	BTreeNode *parent;
	int keyNo;	 // Current number of keys
	bool isLeaf; // Is true when node is leaf. Otherwise false

public:
	BTreeNode(int _t, bool _leaf, BTreeNode *p); // Constructor

	// A function to traverse all nodes in a subtree rooted with this node
	void traverse();

	// A function to search a key in subtree rooted with this node.
	BTreeNode *search(int k); // returns NULL if k is not present.

	// A function that returns the index of the first key that is greater
	// or equal to k
	int findKey(int k);

	// A utility function to insert a new key in the subtree rooted with
	// this node. The assumption is, the node must be non-full when this
	// function is called
	void insertNonFull(int k);

	// A utility function to split the child y of this node. i is index
	// of y in child array C[]. The Child y must be full when this
	// function is called
	void splitChild(int i, BTreeNode *y);

	// A wrapper function to remove the key k in subtree rooted with
	// this node.
	void remove(int k);

	// A function to remove the key present in idx-th position in
	// this node which is a leaf
	void removeFromLeaf(int idx);

	// A function to remove the key present in idx-th position in
	// this node which is a non-leaf node
	void removeFromNonLeaf(int idx);

	// A function to get the predecessor of the key- where the key
	// is present in the idx-th position in the node
	int getPred(int idx);

	// A function to get the successor of the key- where the key
	// is present in the idx-th position in the node
	int getSucc(int idx);

	// A function to fill up the child node present in the idx-th
	// position in the C[] array if that child has less than t-1 keys
	void fill(int idx);

	// A function to borrow a key from the C[idx-1]-th node and place
	// it in C[idx]th node
	void borrowFromPrev(int idx);

	// A function to borrow a key from the C[idx+1]-th node and place it
	// in C[idx]th node
	void borrowFromNext(int idx);

	// A function to merge idx-th child of the node with (idx+1)th child of
	// the node
	void merge(int idx);

	// Make BTree friend of this so that we can access private members of
	// this class in BTree functions
	friend class BTree;
};

class BTree
{
	BTreeNode *root; // Pointer to root node
	int t;			 // Minimum degree
public:
	// Constructor (Initializes tree as empty)
	BTree(int _t)
	{
		root = NULL;
		t = _t;
	}

	void traverse()
	{
		if (root != NULL)
			root->traverse();
	}

	// function to search a key in this tree
	BTreeNode *search(int k)
	{
		return (root == NULL) ? NULL : root->search(k);
	}

	BTreeNode *getRoot()
	{
		return root;
	}

	// The main function that inserts a new key in this B-Tree
	void insert(int k);

	// The main function that removes a new key in this B-Tree
	void remove(int k);
};

BTreeNode::BTreeNode(int t1, bool leaf1, BTreeNode *p = NULL)
{
	// Copy the given minimum degree and leaf property
	t = t1;
	isLeaf = leaf1;

	// Allocate memory for maximum number of possible keys
	// and child pointers
	keys = new int[2 * t - 1];
	children = new BTreeNode *[2 * t];
	parent = p;

	// Initialize the number of keys as 0
	keyNo = 0;
}

// A utility function that returns the index of the first key that is
// greater than or equal to k
int BTreeNode::findKey(int k)
{
	int idx = 0;
	while (idx < keyNo && keys[idx] < k)
		++idx;
	return idx;
}

// A function to remove the key k from the sub-tree rooted with this node
void BTreeNode::remove(int k)
{
	cout << keys[0] << ' ';
	int i = 0;
	for (; i < keyNo; ++i)
		if (k <= keys[i])
			break;

	if (i < keyNo && keys[i] == k)
	{
		return (isLeaf) ? removeFromLeaf(i) : removeFromNonLeaf(i);
	}

	if (isLeaf)
		return;

	if (children[i]->keyNo < t - 1)
		fill(i);
	children[i]->remove(k);
}

// A function to remove the idx-th key from this node - which is a leaf node
void BTreeNode::removeFromLeaf(int idx)
{

	// Move all the keys after the idx-th pos one place backward
	for (int i = idx + 1; i < keyNo; ++i)
		keys[i - 1] = keys[i];

	// Reduce the count of keys
	keyNo--;

	return;
}

// A function to remove the idx-th key from this node - which is a non-leaf node
void BTreeNode::removeFromNonLeaf(int idx)
{

	int k = keys[idx];

	// If the child that precedes k (C[idx]) has atleast t keys,
	// find the predecessor 'pred' of k in the subtree rooted at
	// C[idx]. Replace k by pred. Recursively delete pred
	// in C[idx]
	if (children[idx]->keyNo >= t)
	{
		int pred = getPred(idx);
		keys[idx] = pred;
		children[idx]->remove(pred);
	}

	// If the child C[idx] has less that t keys, examine C[idx+1].
	// If C[idx+1] has atleast t keys, find the successor 'succ' of k in
	// the subtree rooted at C[idx+1]
	// Replace k by succ
	// Recursively delete succ in C[idx+1]
	else if (children[idx + 1]->keyNo >= t)
	{
		int succ = getSucc(idx);
		keys[idx] = succ;
		children[idx + 1]->remove(succ);
	}

	// If both C[idx] and C[idx+1] has less that t keys,merge k and all of C[idx+1]
	// into C[idx]
	// Now C[idx] contains 2t-1 keys
	// Free C[idx+1] and recursively delete k from C[idx]
	else
	{
		merge(idx);
		children[idx]->remove(k);
	}
	return;
}

// A function to get predecessor of keys[idx]
int BTreeNode::getPred(int idx)
{
	// Keep moving to the right most node until we reach a leaf
	BTreeNode *cur = children[idx];
	while (!cur->isLeaf)
		cur = cur->children[cur->keyNo];

	// Return the last key of the leaf
	return cur->keys[cur->keyNo - 1];
}

int BTreeNode::getSucc(int idx)
{

	// Keep moving the left most node starting from C[idx+1] until we reach a leaf
	BTreeNode *cur = children[idx + 1];
	while (!cur->isLeaf)
		cur = cur->children[0];

	// Return the first key of the leaf
	return cur->keys[0];
}

// A function to fill child C[idx] which has less than t-1 keys
void BTreeNode::fill(int idx)
{

	// If the previous child(C[idx-1]) has more than t-1 keys, borrow a key
	// from that child
	if (idx != 0 && children[idx - 1]->keyNo >= t)
		borrowFromPrev(idx);

	// If the next child(C[idx+1]) has more than t-1 keys, borrow a key
	// from that child
	else if (idx != keyNo && children[idx + 1]->keyNo >= t)
		borrowFromNext(idx);

	// Merge C[idx] with its sibling
	// If C[idx] is the last child, merge it with its previous sibling
	// Otherwise merge it with its next sibling
	else
	{
		if (idx != keyNo)
			merge(idx);
		else
			merge(idx - 1);
	}
	return;
}

// A function to borrow a key from C[idx-1] and insert it
// into C[idx]
void BTreeNode::borrowFromPrev(int idx)
{

	BTreeNode *child = children[idx];
	BTreeNode *sibling = children[idx - 1];

	// The last key from C[idx-1] goes up to the parent and key[idx-1]
	// from parent is inserted as the first key in C[idx]. Thus, the loses
	// sibling one key and child gains one key

	// Moving all key in C[idx] one step ahead
	for (int i = child->keyNo - 1; i >= 0; --i)
		child->keys[i + 1] = child->keys[i];

	// If C[idx] is not a leaf, move all its child pointers one step ahead
	if (!child->isLeaf)
	{
		for (int i = child->keyNo; i >= 0; --i)
			child->children[i + 1] = child->children[i];
	}

	// Setting child's first key equal to keys[idx-1] from the current node
	child->keys[0] = keys[idx - 1];

	// Moving sibling's last child as C[idx]'s first child
	if (!child->isLeaf)
		child->children[0] = sibling->children[sibling->keyNo];

	// Moving the key from the sibling to the parent
	// This reduces the number of keys in the sibling
	keys[idx - 1] = sibling->keys[sibling->keyNo - 1];

	child->keyNo += 1;
	sibling->keyNo -= 1;

	return;
}

// A function to borrow a key from the C[idx+1] and place
// it in C[idx]
void BTreeNode::borrowFromNext(int idx)
{

	BTreeNode *child = children[idx];
	BTreeNode *sibling = children[idx + 1];

	// keys[idx] is inserted as the last key in C[idx]
	child->keys[(child->keyNo)] = keys[idx];

	// Sibling's first child is inserted as the last child
	// into C[idx]
	if (!(child->isLeaf))
		child->children[(child->keyNo) + 1] = sibling->children[0];

	// The first key from sibling is inserted into keys[idx]
	keys[idx] = sibling->keys[0];

	// Moving all keys in sibling one step behind
	for (int i = 1; i < sibling->keyNo; ++i)
		sibling->keys[i - 1] = sibling->keys[i];

	// Moving the child pointers one step behind
	if (!sibling->isLeaf)
	{
		for (int i = 1; i <= sibling->keyNo; ++i)
			sibling->children[i - 1] = sibling->children[i];
	}

	// Increasing and decreasing the key count of C[idx] and C[idx+1]
	// respectively
	child->keyNo += 1;
	sibling->keyNo -= 1;

	return;
}

// A function to merge C[idx] with C[idx+1]
// C[idx+1] is freed after merging
void BTreeNode::merge(int idx)
{
	BTreeNode *child = children[idx];
	BTreeNode *sibling = children[idx + 1];

	// Pulling a key from the current node and inserting it into (t-1)th
	// position of C[idx]
	child->keys[t - 1] = keys[idx];

	// Copying the keys from C[idx+1] to C[idx] at the end
	for (int i = 0; i < sibling->keyNo; ++i)
		child->keys[i + t] = sibling->keys[i];

	// Copying the child pointers from C[idx+1] to C[idx]
	if (!child->isLeaf)
	{
		for (int i = 0; i <= sibling->keyNo; ++i)
			child->children[i + t] = sibling->children[i];
	}

	// Moving all keys after idx in the current node one step before -
	// to fill the gap created by moving keys[idx] to C[idx]
	for (int i = idx + 1; i < keyNo; ++i)
		keys[i - 1] = keys[i];

	// Moving the child pointers after (idx+1) in the current node one
	// step before
	for (int i = idx + 2; i <= keyNo; ++i)
		children[i - 1] = children[i];

	// Updating the key count of child and the current node
	child->keyNo += sibling->keyNo + 1;
	keyNo--;

	// Freeing the memory occupied by sibling
	delete (sibling);
	return;
}

// The main function that inserts a new key in this B-Tree
void BTree::insert(int k)
{
	if (root == NULL)
		root = new BTreeNode(t, true);
	else if (root->keyNo == (t << 1) - 1)
	{
		auto *s = new BTreeNode(t, false);

		s->children[0] = root;
		root->parent = s;
		root = s;
		s->splitChild(0, s->children[0]);
	}
	root->insertNonFull(k);
}

// A utility function to insert a new key in this node
// The assumption is, the node must be non-full when this
// function is called
void BTreeNode::insertNonFull(int k)
{
	// Initialize index as index of rightmost element
	int i = keyNo - 1;

	// If this is a leaf node
	if (isLeaf == true)
	{
		// The following loop does two things
		// a) Finds the location of new key to be inserted
		// b) Moves all greater keys to one place ahead
		while (i >= 0 && keys[i] > k)
		{
			keys[i + 1] = keys[i];
			i--;
		}

		// Insert the new key at found location
		keys[i + 1] = k;
		keyNo = keyNo + 1;
	}
	else // If this node is not leaf
	{
		// Find the child which is going to have the new key
		while (i >= 0 && keys[i] > k)
			i--;

		// See if the found child is full
		if (children[i + 1]->keyNo == 2 * t - 1)
		{
			// If the child is full, then split it
			splitChild(i + 1, children[i + 1]);

			// After split, the middle key of C[i] goes up and
			// C[i] is splitted into two. See which of the two
			// is going to have the new key
			if (keys[i + 1] < k)
				i++;
		}
		children[i + 1]->insertNonFull(k);
	}
}

// A utility function to split the child y of this node
// Note that y must be full when this function is called
void BTreeNode::splitChild(int i, BTreeNode *y)
{
	// Create a new node which is going to store (t-1) keys
	// of y
	BTreeNode *z = new BTreeNode(y->t, y->isLeaf, y->parent);
	z->keyNo = t - 1;

	// Copy the last (t-1) keys of y to z
	for (int j = 0; j < t - 1; j++)
		z->keys[j] = y->keys[j + t];

	// Copy the last t children of y to z
	if (y->isLeaf == false)
	{
		for (int j = 0; j < t; j++)
			z->children[j] = y->children[j + t];
	}

	// Reduce the number of keys in y
	y->keyNo = t - 1;

	// Since this node is going to have a new child,
	// create space of new child
	for (int j = keyNo; j >= i + 1; j--)
		children[j + 1] = children[j];

	// Link the new child to this node
	children[i + 1] = z;

	// A key of y will move to this node. Find location of
	// new key and move all greater keys one space ahead
	for (int j = keyNo - 1; j >= i; j--)
		keys[j + 1] = keys[j];

	// Copy the middle key of y to this node
	keys[i] = y->keys[t - 1];

	// Increment count of keys in this node
	keyNo = keyNo + 1;
}

// Function to traverse all nodes in a subtree rooted with this node
void BTreeNode::traverse()
{
	int i = 0;
	for (; i < keyNo; ++i)
	{
		if (!isLeaf)
			children[i]->traverse();
		cout << " " << keys[i];
	}
	if (isLeaf)
		return;
	children[i]->traverse();
}

// Function to search key k in subtree rooted with this node
BTreeNode *BTreeNode::search(int k)
{
	int i = 0;
	for (; i < keyNo; ++i)
		if (k <= keys[i])
			break;

	if (keys[i] == k)
		return this;

	if (isLeaf)
		return NULL;

	return children[i]->search(k);
}

void BTree::remove(int k)
{
	if (root == NULL)
		return;
	root->remove(k);
}

// Driver program to test above functions
void bPlusTreeTest()
{
	BTree t(3); // A B-Tree with minimum degree 3

	t.insert(1);
	t.insert(3);
	t.insert(7);
	t.insert(10);
	t.insert(11);
	t.insert(13);
	t.insert(14);
	t.insert(15);
	t.insert(18);
	t.insert(16);
	t.insert(19);
	t.insert(24);
	t.insert(25);
	t.insert(26);
	t.insert(21);
	t.insert(4);
	t.insert(5);
	t.insert(20);
	t.insert(22);
	t.insert(2);
	t.insert(17);
	t.insert(12);
	t.insert(6);

	cout << "Traversal of tree constructed is\n";
	t.traverse();
	cout << endl;

	t.remove(6);
	cout << "Traversal of tree after removing 6\n";
	t.traverse();
	cout << endl;

	t.remove(13);
	cout << "Traversal of tree after removing 13\n";
	t.traverse();
	cout << endl;

	t.remove(7);
	cout << "Traversal of tree after removing 7\n";
	t.traverse();
	cout << endl;

	t.remove(4);
	cout << "Traversal of tree after removing 4\n";
	t.traverse();
	cout << endl;

	t.remove(2);
	cout << "Traversal of tree after removing 2\n";
	t.traverse();
	cout << endl;

	t.remove(16);
	cout << "Traversal of tree after removing 16\n";
	t.traverse();
	cout << endl;
}
