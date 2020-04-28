#include <iostream>
#include <vector>
#include <cmath>

template<class Key>
struct DefaultComparator {
    std::int8_t operator()(const Key &l, const Key &r) {
        if (l < r) return -1;
        if (l > r) return 1;
        return 0;
    }
};

template<class Key, class Comparator=DefaultComparator<Key>>
class AVLTree {
    struct Node {
        Key key;
        size_t weight;

        Node *left;
        Node *right;

        explicit Node(const Key &key) :
                key(key), weight(1),
                left(nullptr), right(nullptr) {}
    };

public:
    AVLTree() : root(nullptr) {}

    void print();

    size_t push(const Key &key, bool reverse = false);

    void pop(size_t index, bool reverse = false);

    size_t size();

private:
    size_t weight(Node *node);

    void setWeight(Node * node);

    std::uint8_t height(Node *node);

    Node *_push(Node *node, const Key &key, size_t &indexCounter);

    Node *_pop(Node *node, size_t index, size_t &indexCounter, bool reverse=false);

    Node * popMin(Node * node);

    Node *balance(Node *node);

    Node * rotateRight(Node * node);

    Node * rotateLeft(Node * node);

    std::int8_t balanceFactor(Node *node);

    Comparator comp;
    Node *root;
};


void test(AVLTree<int> &tree);

int main() {
    AVLTree<int> tree;
//    for (int i = 0; i < 100000; i++) {
//        tree.push(i);
//    }
    test(tree);
    return 0;
}

void test(AVLTree<int> &tree) {
    size_t amount;
    std::cin >> amount;
    for (size_t i = 0; i < amount; i++) {
        int command;
        int key;
        std::cin >> command >> key;
        switch (command) {
            case 1:
                std::cout << tree.push(key, true) << std::endl;
                break;
            case 2:
                tree.pop(key);
                break;
        }
    }
}


template<class Key, class Comparator>
size_t AVLTree<Key, Comparator>::push(const Key &key, bool reverse) {
    size_t currIndex = root ? weight(root->left) : 0;
    root = _push(root, key, currIndex);
    return reverse ? size() - 1 - currIndex : currIndex;
}

template<class Key, class Comparator>
void AVLTree<Key, Comparator>::pop(size_t index, bool reverse) {
    if (!root) {
        return;
    }
    size_t currIndex = reverse ? weight(root->right) : weight(root->left);
    root = _pop(root, index, currIndex, reverse);
}

template<class Key, class Comparator>
size_t AVLTree<Key, Comparator>::size() {
    return weight(root);
}

template<class Key, class Comparator>
size_t AVLTree<Key, Comparator>::weight(Node *node) {
    if (!node) {
        return 0;
    }

    return node->weight;
}

template<class Key, class Comparator>
void AVLTree<Key, Comparator>::setWeight(Node *node) {
    if (!node) {
        return;
    }

    node->weight = weight(node->left) + weight(node->right) + 1;
}

template<class Key, class Comparator>
std::uint8_t AVLTree<Key, Comparator>::height(Node *node) {
    if (!node) {
        return 0;
    }

    return std::floor(std::log2(node->weight)) + 1;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *
AVLTree<Key, Comparator>::_push(Node *node, const Key &key, size_t &indexCounter) {
    if (!node) {
        return new Node(key);
    }

    std::int8_t compRes = comp(key, node->key);
    if (compRes < 0) {
        indexCounter -= node->left ? weight(node->left->right) + 1 : 0;
        node->left = _push(node->left, key, indexCounter);
    } else {
        indexCounter += node->right ? weight(node->right->left) + 1 : 1;
        node->right = _push(node->right, key, indexCounter);
    }

    return balance(node);
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *
AVLTree<Key, Comparator>::_pop(Node *node, size_t index, size_t &indexCounter, bool reverse) {
    if (!node) {
        return nullptr;
    }

    if (index < indexCounter) {
        if (node->left) {
            indexCounter += (weight(node->left->right) + 1) * (reverse ? 1 : -1);
        }
        node->left = _pop(node->left, index, indexCounter, reverse);
    } else if (index > indexCounter) {
        if (node->right){
            indexCounter += (weight(node->right->left) + 1) * (reverse ? -1 : 1);
        }
        node->right = _pop(node->right, index, indexCounter, reverse);
    } else {
        Node * left = node->left;
        Node * right = node->right;
        delete node;

        if (!right) {
            return left;
        }

        Node * minNode = popMin(right);
        minNode->left = left;

        return balance(minNode);
    }
    return balance(node);
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node * AVLTree<Key, Comparator>::popMin(Node * node) {
    Node * curr = node;
    while (curr->left) {
        curr = curr->left;
    }
    node->left =  curr->right;
    curr->right = balance(node);
    return balance(curr);
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node * AVLTree<Key, Comparator>::balance(Node * node) {
    setWeight(node);

    std::int8_t factor = balanceFactor(node);

    if (factor == 2) {
        if (balanceFactor(node->right) < 0) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    }
    if (factor == -2) {
        if (balanceFactor(node->left) > 0) {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }
    return node;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node * AVLTree<Key, Comparator>::rotateRight(Node *node) {
    Node *left = node->left;
    node->left = left->right;
    left->right = node;
    setWeight(node);
    setWeight(left);
    return left;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node * AVLTree<Key, Comparator>::rotateLeft(Node *node) {
    Node *right = node->right;
    node->right = right->left;
    right->left = node;
    setWeight(node);
    setWeight(right);
    return right;
}

template<class Key, class Comparator>
std::int8_t AVLTree<Key, Comparator>::balanceFactor(Node *node) {
    if (!node) {
        return 0;
    }
    return height(node->right) - height(node->left);
}
