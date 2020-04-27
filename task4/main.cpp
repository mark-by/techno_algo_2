#include <iostream>
#include <vector>

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

        std::uint8_t height;

        Node(const Key &key)
                : left(nullptr), right(nullptr),
                  key(key),
                  height(1) {}
    };

public:
    AVLTree() : root(nullptr), _size(0) {}

    ~AVLTree();

    size_t size() const;

    std::uint8_t height() const;

    int insert(const Key &key);

    size_t find(const Key &key);

    void pop(size_t index);

private:
    template<class Action>
    void _postOrder(Action action, Node *node);

    Node *_find(Node *node, const Key &key, size_t &counter, bool &founded);

    Node *balance(Node *node);

    std::uint8_t height(Node *node);

    Node *popMin(Node *node);

    std::int8_t balanceFactor(Node *node);

    void setHeight(Node *node);

    Node *_insert(Node *node, const Key &key);

    Node *_pop(Node *node, size_t index, size_t &counter);

    Node *rotateRight(Node *node);

    Node *rotateLeft(Node *node);

    Node *root;
    size_t _size;
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
AVLTree<Key, Comparator>::_insert(AVLTree::Node *node, const Key &key) {
    if (!node) {
        _size++;
        return new Node(key);
    }
    if (comp(key, node->key) < 0) {
        node->left = _insert(node->left, key);
    } else {
        node->right = _insert(node->right, key);
    }

    return balance(node);
}

template<class Key, class Comparator>
int AVLTree<Key, Comparator>::insert(const Key &key) {
    root = _insert(root, key);
    return 0;
}

template<class Key, class Comparator>
void AVLTree<Key, Comparator>::pop(size_t index) {
    size_t counter = 0;
    root = _pop(root, index, counter);
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *AVLTree<Key, Comparator>::_pop(Node *node, size_t index, size_t &counter) {
    if (!node) {
        return nullptr;
    }
    node->right = _pop(node->right, index, counter); // сначала node->right потому что индексация с старшего элемента
    if (counter > index) {
        return node;
    }
    if (counter == index) {
        counter++;
        _size--;

        Node *left = node->left;
        Node *right = node->right;
        delete node;

        if (!right) {
            return left;
        }

        Node *minNode = popMin(right);
        minNode->left = left;

        return balance(minNode);
    } else {
        counter++;
        node->left = _pop(node->left, index, counter);
    }
    return node;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *AVLTree<Key, Comparator>::popMin(AVLTree::Node *node) {
    Node *curr = node;
    while (curr->left) {
        curr = curr->left;
    }
    node->left = curr->right;
    curr->right = balance(node);
    return balance(curr);
}

template<class Key, class Comparator>
std::int8_t AVLTree<Key, Comparator>::balanceFactor(AVLTree<Key, Comparator>::Node *node) {
    return height(node->left) - height(node->right);
}

template<class Key, class Comparator>
std::uint8_t AVLTree<Key, Comparator>::height(AVLTree::Node *node) {
    if (!node) {
        return 0;
    }
    return node->height;
}

template<class Key, class Comparator>
void AVLTree<Key, Comparator>::setHeight(AVLTree::Node *node) {
    if (!node) {
        return;
    }
    node->height = std::max(height(node->left), height(node->right)) + 1;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *AVLTree<Key, Comparator>::balance(AVLTree::Node *node) {
    setHeight(node);

    std::int8_t bf = balanceFactor(node);
    // bf == 2 => левое поддерево больше правого
    // bf == -2 => правое поддерево больше левого

    if (bf == -2) {
        if (balanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    } else if (bf == 2) {
        if (balanceFactor(node->left) < 0) {
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
    setHeight(node);
    setHeight(temp);
    return temp;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *AVLTree<Key, Comparator>::rotateLeft(AVLTree::Node *node) {
    Node *temp = node->right;
    node->right = temp->left;
    temp->left = node;
    setHeight(node);
    setHeight(temp);
    return temp;
}

template<class Key, class Comparator>
size_t AVLTree<Key, Comparator>::size() const {
    return _size;
}

template<class Key, class Comparator>
std::uint8_t AVLTree<Key, Comparator>::height() const {
    return height(root);
}

template<class Key, class Comparator>
size_t AVLTree<Key, Comparator>::find(const Key &key) {
    size_t counter = 0;
    bool founded = false;
    _find(root, key, counter, founded);
    return counter;
}

template<class Key, class Comparator>
typename AVLTree<Key, Comparator>::Node *
AVLTree<Key, Comparator>::_find(AVLTree::Node *node, const Key &key, size_t &counter, bool &founded) {
    if (!node) {
        return node;
    }
    _find(node->right, key, counter, founded); // сначала спускаемся в правое поддерево, потому что индексация с старшего элемента
    if (founded) {
        return nullptr;
    }
    if (node->key == key) {
        founded = true;
        return node;
    } else {
        counter++;
        _find(node->left, key, counter, founded);
    }
    return node;
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

void test(AVLTree<int> &tree) {
    int count;
    std::cin >> count;
    for (int i = 0; i < count; i++) {
        int command;
        int key;
        std::cin >> command >> key;
        switch (command) {
            case 1:
                tree.insert(key);
                std::cout << tree.find(key) << std::endl;
                break;
            case 2:
                tree.pop(key);
                break;
        }
    }
}
