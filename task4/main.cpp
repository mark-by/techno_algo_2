#include <iostream>
#include <vector>
#include <cmath>

template<class T>
struct DefaultComparator {
    std::int8_t operator()(const T &l, const T &r) {
        if (l < r) return -1;
        if (l > r) return 1;
        return 0;
    }
};

template<class Key, class Comparator=DefaultComparator<Key>>
class AVLTree {
    struct Node {
        Node *left;
        Node *right;

        Key key;

        size_t weight;

        Node(const Key &key)
                : left(nullptr), right(nullptr),
                  key(key),
                  weight(1) {}
    };

public:
    AVLTree() : root(nullptr) {}

    ~AVLTree();

    size_t size() const;

    std::uint8_t height() const;

    size_t insert(const Key &key);

    size_t find(const Key &key);

    void pop(size_t index);

private:
    template<class Action>
    void _postOrder(Action action, Node *node);

    Node *balance(Node *node);

    std::uint8_t height(Node *node) const;

    std::size_t weight(Node *node) const;

    Node *popMin(Node *node, Node *& minNode);

    std::int8_t balanceFactor(Node *node);

    void setWeight(Node *node);

    Node *_insert(Node *node, const Key &key, size_t & indexCounter);

    Node *_pop(Node *node, size_t index, size_t &counter);

    Node *rotateRight(Node *node);

    Node *rotateLeft(Node *node);

    Node *root;
    Comparator comp;
};


void test(AVLTree<int> &tree);

int main() {
    AVLTree<int> tree;
    test(tree);
    return 0;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *
AVLTree<Key, Comparator>::_insert(AVLTree::Node *node, const Key &key, size_t & indexCounter) {
    if (!node) {
        return new Node(key);
    }
    if (comp(key, node->key) < 0) {
        indexCounter += node->left ? weight(node->left->right) + 1: 1;
        node->left = _insert(node->left, key, indexCounter);
    } else {
        indexCounter -= node->right ? weight(node->right->left) + 1: 0;
        node->right = _insert(node->right, key, indexCounter);
    }

    return balance(node);
}

template<class Key, class Comparator>
size_t AVLTree<Key, Comparator>::insert(const Key &key) {
    size_t currIndex = root ? weight(root->right) : 0;
    root = _insert(root, key, currIndex);
    return currIndex;
}

template<class Key, class Comparator>
void AVLTree<Key, Comparator>::pop(size_t index) {
    size_t currIndex = weight(root->right);
    root = _pop(root, index, currIndex);
}


template<class Key, class Comparator>
std::int8_t AVLTree<Key, Comparator>::balanceFactor(AVLTree<Key, Comparator>::Node *node) {
    return height(node->right) - height(node->left);
}

template<class Key, class Comparator>
std::uint8_t AVLTree<Key, Comparator>::height(AVLTree::Node *node) const {
    if (!node) {
        return 0;
    }
    return std::floor(std::log2(node->weight)) + 1;
}

template<class Key, class Comparator>
void AVLTree<Key, Comparator>::setWeight(AVLTree::Node *node) {
    if (!node) {
        return;
    }
    node->weight = weight(node->left) + weight(node->right) + 1;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *AVLTree<Key, Comparator>::balance(AVLTree::Node *node) {
    setWeight(node);

    std::int8_t bf = balanceFactor(node);

    if (bf == 2) {
        if (balanceFactor(node->right) < 0) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    } else if (bf == -2) {
        if (balanceFactor(node->left) > 0) {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }
    return node;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *AVLTree<Key, Comparator>::rotateRight(AVLTree::Node *node) {
    Node *temp = node->left;
    node->left = temp->right;
    temp->right = node;
    setWeight(node);
    setWeight(temp);
    return temp;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *AVLTree<Key, Comparator>::rotateLeft(AVLTree::Node *node) {
    Node *temp = node->right;
    node->right = temp->left;
    temp->left = node;
    setWeight(node);
    setWeight(temp);
    return temp;
}

template<class Key, class Comparator>
size_t AVLTree<Key, Comparator>::size() const {
    return weight(root);
}

template<class Key, class Comparator>
std::uint8_t AVLTree<Key, Comparator>::height() const {
    return height(root);
}

template<class Key, class Comparator>
size_t AVLTree<Key, Comparator>::find(const Key &key) {
    Node * curr = root;
    size_t index = weight(root->right);
    while(true) {
        if (!curr) {
            return 335;
        }
        std::int8_t compRes = comp(key, curr->key);
        if (compRes < 0) {
            curr = curr->left;
            index += weight(curr->right) + 1;
        } else if (compRes > 0) {
            curr = curr->right;
            index -= weight(curr->left) + 1;
        } else {
            return index;
        }
    }
}

template<class Key, class Comparator>
template<class Action>
void AVLTree<Key, Comparator>::_postOrder(Action action, AVLTree::Node *node) {
    if (!node) {
        return;
    }
    _postOrder(action, node->left);
    _postOrder(action, node->right);
    action(node);
}

template<class Key, class Comparator>
AVLTree<Key, Comparator>::~AVLTree() {
    _postOrder([](Node *node) {
        delete node;
    }, root);
}

template<class Key, class Comparator>
std::size_t AVLTree<Key, Comparator>::weight(AVLTree::Node *node) const {
    if (!node) {
        return 0;
    }
    return node->weight;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *AVLTree<Key, Comparator>::_pop(AVLTree::Node *node, size_t index, size_t &counter) {
    if (!node) {
        return nullptr;
    }

    if (counter == index) {
        Node *left = node->left;
        Node *right = node->right;
        delete node;

        if (!right) {
            return left;
        }

        Node *minNode = nullptr;
        right = popMin(right, minNode);
        minNode->right = right;
        minNode->left = left;

        return balance(minNode);
    } else if (index < counter) { // искомый индек меньше индекса текущего элемента, значит идем вправо (индекс со старших)
        counter -= node->right ? weight(node->right->left) + 1 : 0;
        node->right = _pop(node->right, index, counter);
    } else {
        counter += node->left ? weight(node->left->right) + 1 : 0;
        node->left = _pop(node->left, index, counter);
    };

    return balance(node);
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *AVLTree<Key, Comparator>::popMin(Node *node, Node *& minNode) {
    if (!node->left) {
        minNode = node;
        return node->right;
    }
    node->left = popMin(node->left, minNode);
    return balance(node);
}

void test(AVLTree<int> &tree) {
    int count;
    std::cin >> count;
    for (int i = 0; i < count; i++) {
        int command;
        int key;
        std::cin >> command >> key;
        switch (command) {
            case 1:
                std::cout << tree.insert(key) << std::endl;
                break;
            case 2:
                tree.pop(key);
                break;
        }
    }
}
