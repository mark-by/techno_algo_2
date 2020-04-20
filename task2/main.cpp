#include <iostream>
#include <vector>

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
    Tree() : root(nullprt) {};

    void add(Key &key);

    void inOrderPrint();

private:
    Node *root;
    Comparator comp;
};

void test(Tree<long> &tree);

int main() {
    return 0;
}

template<class Key, class Comparator>
void Tree<Key, Comparator>::add(Key &key) {
    if (!root) {
        root = new Node(key);
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
void Tree<Key, Comparator>::inOrderPrint() {

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
    tree.inOrderPrint();
}
