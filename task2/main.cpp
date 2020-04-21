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
    enum NodeType {
        leftDone, rootWas, rightDone
    };
    enum OrderType {
        inOrderType, postOrderType, preOrderType
    };

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
        _order(passNode, delNode, postOrderType);
    }

    void add(Key &key);

    template<class Action>
    void inOrder(Action action);

    template<class Action>
    void postOrder(Action action);

    template<class Action>
    void preOrder(Action action);

private:
    Node *root;
    Comparator comp;

    static Key getKeyNode(Node *node) {
        return node->key;
    }

    static void delNode(Node * node) {
        delete node;
    }

    static Node * passNode(Node * node) {
        return node;
    }

    template<class NodeAction, class Action>
    void _order(NodeAction nodeAction, Action action, std::uint8_t orderType);
};

void test(Tree<long> &tree);

int main() {
    Tree<long> tree;
    test(tree);
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
template<class NodeAction, class Action>
void Tree<Key, Comparator>::_order(NodeAction nodeAction, Action action, std::uint8_t orderType) {
    std::stack<Node *> history;
    std::stack<std::uint8_t> description;
    history.push(root);
    description.push(rootWas);
    while (!history.empty()) {
        switch (description.top()) {
            case rootWas:
                if (orderType == preOrderType) { action(nodeAction(history.top())); }
                if (history.top()->left) {
                    history.push(history.top()->left);
                    description.push(rootWas);
                } else {
                    description.top() = leftDone;
                }
                break;
            case leftDone:
                if (orderType == inOrderType) { action(nodeAction(history.top())); }
                if (history.top()->right) {
                    history.push(history.top()->right);
                    description.push(rootWas);
                } else {
                    description.top() = rightDone;
                }
                break;
            case rightDone:
                if (orderType == postOrderType) { action(nodeAction(history.top())); }
                history.pop();
                description.pop();
                if (description.empty()) {
                    break;
                }
                if (description.top() == rootWas) {
                    description.top() = leftDone;
                } else {
                    description.top() = rightDone;
                }
                break;
        }
    }
}

template<class Key, class Comparator>
template<class Action>
void Tree<Key, Comparator>::postOrder(Action action) {
    _order(getKeyNode, action, postOrderType);
}

template<class Key, class Comparator>
template<class Action>
void Tree<Key, Comparator>::inOrder(Action action) {
    _order(getKeyNode, action, inOrderType);
}

template<class Key, class Comparator>
template<class Action>
void Tree<Key, Comparator>::preOrder(Action action) {
    _order(getKeyNode, action, preOrderType);
}

void print(long key) {
    std::cout << key << " ";
}

void test(Tree<long> &tree) {
    size_t size;
    std::cin >> size;
    size_t idx = 0;
    long number;
    while (idx < size) {
        std::cin >> number;
        tree.add(number);
        idx++;
    }
    tree.inOrder(print);
}
