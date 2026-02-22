#include <iostream>
#include <memory>
#include <queue>

struct Node{
    int key;
    Node* parent;
    Node* leftChildren;
    Node* rightChildren;

    Node(int k) : key(k), leftChildren(nullptr), rightChildren(nullptr), parent(nullptr) {}
};

class BinaryTree {
private:
    Node* root;

    void destroyTree(Node* node) {
        if (node) {
            destroyTree(node->leftChildren);
            destroyTree(node->rightChildren);
            delete node;
        }
    }

    void printInOrder(Node* node) {
        if (node) {
            printInOrder(node->leftChildren);
            std::cout << node->key << " ";
            printInOrder(node->leftChildren);
        }
    }

    Node* findNode(Node* node, int val) {
        if (!node || node->key == val) return node;

        Node* leftRes = findNode(node->leftChildren, val);
        if (leftRes) return leftRes;

        return findNode(node->rightChildren, val);
    }

    // высота дерева
    int height(Node* node) {
        if (!node) return 0;
        return 1 + std::max(height(node->leftChildren), height(node->rightChildren));
    }

public:
    BinaryTree() : root(nullptr) {}

    ~BinaryTree() {
        destroyTree(root);
    }

    void insert(int val) {
        Node* newNode = new Node(val);
        
        if (!root) {
            root = newNode;
            return;
        }

        std::queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            Node* cur = q.front();
            q.pop();

            if (!cur->leftChildren) {
                cur->leftChildren = newNode;
                newNode->parent = cur;
                return;
            } else {
                q.push(cur->leftChildren);
            }

            if (!cur->rightChildren) {
                cur->rightChildren = newNode;
                newNode->parent = cur;
                return;
            } else {
                q.push(cur->rightChildren);
            }
        }
    }

    Node* search(int val) {
        return findNode(root, val);
    }

    bool deleteSubTree(int val) {
        Node* target = search(val);
        if (!target) return false;

        if (target == root) {
            destroyTree(root);
            root = nullptr;
            return true;
        }

        if (target->parent) {
            if (target->parent->leftChildren == target) {
                target->parent->leftChildren = nullptr;
            } else {
                target->parent->rightChildren = nullptr;
            }
        }
        
        destroyTree(target);
        return true;
    }

    void inorder() {
        std::cout << "inorder traversal: ";
        printInOrder(root);
        std::cout << std::endl;
    }

    void preorder(Node* node) {
        if (!node) return;
        std::cout << node->key << " ";
        preorder(node->leftChildren);
        preorder(node->rightChildren);
    }

    void postorder(Node* node) {
        if (!node) return;
        postorder(node->leftChildren);
        postorder(node->rightChildren);
        std::cout << node->key << " ";
    }

    // обход в ширину
    void leveOrder() {
        if (!root) return;

        std::queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            Node* cur = q.front();
            q.pop();

            std::cout << cur->key << " ";

            if (cur->leftChildren) q.push(cur->leftChildren);
            if (cur->rightChildren) q.push(cur->rightChildren);
        }

        std::cout << std::endl;
    }

    int getHeight() {
        return height(root);
    }

    Node* getRoot() {return root;}
};










