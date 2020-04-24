#include <iostream>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>

template<class T>
struct DefaultComparator {
    bool operator()(T &l, T &r) {
        return l < r;
    }
};

class ITree {
protected:
    template<class Node>
    int _maxWidth(Node *&root) {
        if (!root) { return 0; }
        std::queue<Node *> nodes;
        nodes.push(root);
        int currentLayerWidth = 1; // 1 потому что root
        int nextLayerWidth = 0;
        int maxWidth = 0;
        while (!nodes.empty()) {
            for (int i = 0; i < currentLayerWidth; i++) {
                Node *curr = nodes.front();
                nodes.pop();
                if (curr->left) {
                    nodes.push(curr->left);
                    nextLayerWidth++;
                }
                if (curr->right) {
                    nodes.push(curr->right);
                    nextLayerWidth++;
                }
            }
            if (nextLayerWidth > maxWidth) {
                maxWidth = nextLayerWidth;
            }
            currentLayerWidth = nextLayerWidth;
            nextLayerWidth = 0;
        }
        return std::max(currentLayerWidth,
                        maxWidth); // если в дереве только root, то максимальным будет currLayerWidth == 1
    }
};

template<class Key, class Comparator = DefaultComparator<Key>>
class Tree : public ITree {
    struct Node {
        Key key;
        Node *left;
        Node *right;

        Node() : left(nullptr), right(nullptr) {}

        explicit Node(Key &key) : key(key), left(nullptr), right(nullptr) {}
    };

public:
    Tree() : root(nullptr) {};
    ~Tree() {
        _postOrder(root, [](Node * node) {
            delete node;
        });
    }

    void add(Key &key);

    int maxWidth() {
        return _maxWidth(root);
    };

private:
    template<class Action>
    void _postOrder(Node * node, Action action);

    Node *root;
    Comparator comp;
};


template<class Key, class Comparator = DefaultComparator<Key>>
class Treap : public ITree {
    struct Node {
        Key key;
        size_t priority{};
        Node *left;
        Node *right;

        Node() : left(nullptr), right(nullptr) {}

        Node(Key &key, size_t priority)
                : key(key),
                  priority(priority),
                  left(nullptr),
                  right(nullptr) {}
    };

public:
    Treap() : root(nullptr) {}
    ~Treap() {
        _postOrder(root, [](Node * node) {
            delete node;
        });
    }
    void add(Key &key, size_t priority);

    int maxWidth() {
        return _maxWidth(root);
    };

private:
    void split(Node *node, Key &key, Node *&left, Node *&right);

    void _add(Node *&node, Key &key, size_t priority);

    template<class Action>
    void _postOrder(Node *& node, Action action);

    Comparator comp;
    Node *root;
};

void input(Treap<long> &treap, Tree<long> &tree);

int test(Treap<long> &treap, Tree<long> &tree);

int main() {
    Treap<long> treap;
    Tree<long> tree;
    input(treap, tree);
    std::cout << test(treap, tree) << std::endl;
    return 0;
}

void input(Treap<long> &treap, Tree<long> &tree) {
    int count;
    std::cin >> count;
    long key;
    size_t priority;
    for (int i = 0; i < count; i++) {
        std::cin >> key >> priority;
        treap.add(key, priority);
        tree.add(key);
    }
}

int test(Treap<long> &treap, Tree<long> &tree) {
    return treap.maxWidth() - tree.maxWidth();
}

template<class Key, class Comparator>
void Treap<Key, Comparator>::add(Key &key, size_t priority) {
    _add(root, key, priority);
}


template<class Key, class Comparator>
void Treap<Key, Comparator>::_add(Treap::Node *&node, Key &key, size_t priority) {
    if (!node) {
        node = new Node(key, priority);
    } else if (node->priority < priority) {
        Node *newNode = new Node(key, priority);
        split(node, key, newNode->left, newNode->right);
        node = newNode;
    } else {
        if (comp(key, node->key)) {
            _add(node->left, key, priority);
        } else {
            _add(node->right, key, priority);
        }
    }
}

template<class Key, class Comparator>
void Treap<Key, Comparator>::split(Treap::Node *node, Key &key, Treap::Node *&left, Treap::Node *&right) {
    if (!node) {
        left = nullptr;
        right = nullptr;
    } else if (!comp(key, node->key)) {
        split(node->right, key, node->right, right);
        left = node;
    } else {
        split(node->left, key, left, node->left);
        right = node;
    }
}


template<class Key, class Comparator>
template<class Action>
void Treap<Key, Comparator>::_postOrder(Treap::Node *&node, Action action) {
    if(!node) {
        return;
    }
    _postOrder(node->left, action);
    _postOrder(node->right, action);
    action(node);
}

template<class Key, class Comparator>
void Tree<Key, Comparator>::add(Key &key) {
    if (!root) {
        root = new Node(key);
        return;
    }
    Node *curr = root;
    while (curr) {
        if (!comp(key, curr->key)) {
            if (curr->right) {
                curr = curr->right;
            } else {
                curr->right = new Node(key);
                break;
            }
        } else {
            if (curr->left) {
                curr = curr->left;
            } else {
                curr->left = new Node(key);
                break;
            }
        }
    }
}

template<class Key, class Comparator>
template<class Action>
void Tree<Key, Comparator>::_postOrder(Tree::Node * node, Action action) {
    std::stack<Node *> history;
    Node * lastNode = nullptr;
    while (!history.empty() || node) {
        if (node) {
            history.push(node);
            node = node->left;
        } else {
            Node * top = history.top();
            if (top->right && lastNode != top->right) {
                node = top->right;
            } else {
                action(top);
                lastNode = history.top();
                history.pop();
            }
        }
    }
}
