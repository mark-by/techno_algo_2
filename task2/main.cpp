#include <iostream>
#include <vector>
#include <stack>

template<class T>
struct DefaultComparator {
    bool operator()(T &l, T &r) {
        return l < r;
    }
};

template<class Key, class Comparator = DefaultComparator<Key>>
class Tree {
    struct Node {
        Key key;
        Node *left;
        Node *right;

        Node() : key(0), left(nullptr), right(nullptr) {}

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

    void print();

private:
    Node *root;
    Comparator comp;

    template<class Action>
    void _postOrder(Node *node, Action action);

    template<class Action>
    void _inOrder(Node *node, Action action);
};

void input(Tree<long> &tree);

int main() {
    Tree<long> tree;
    input(tree);
    tree.print();
    return 0;
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
void Tree<Key, Comparator>::_inOrder(Tree::Node *node, Action action) {
    std::stack<Node *> history;
    while (!history.empty() || node) {
        if (node) {
            history.push(node);
            node = node->left;
        } else {
            node = history.top();
            history.pop();
            action(node);
            node = node->right;
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

template<class Key, class Comparator>
void Tree<Key, Comparator>::print()  {
    _inOrder(root, [](Node * node) {
        std::cout << node->key << " ";
    });
    std::cout << std::endl;
}

void input(Tree<long> &tree) {
    size_t size;
    std::cin >> size;
    size_t idx = 0;
    long number;
    while (idx < size) {
        std::cin >> number;
        tree.add(number);
        idx++;
    }
}
