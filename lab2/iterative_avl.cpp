#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <stdexcept>

class AVLTree {
private:
    struct TNode {
        std::string key;
        uint64_t value;
        uint8_t height;
        TNode* left;
        TNode* right;

        TNode(std::string k, uint64_t v) 
            : key(std::move(k)), value(v), height(1), left(nullptr), right(nullptr) {}
    };

    TNode* root;

    static inline void ToLowerCase(std::string& word) {
        for (char& c : word) {
            if (c >= 'A' && c <= 'Z') c += 32;
        }
    }

    inline uint8_t GetHeight(TNode* node) const {
        return node ? node->height : 0;
    }

    inline int GetBalance(TNode* node) const {
        return node ? (int)GetHeight(node->right) - (int)GetHeight(node->left) : 0;
    }

    inline void UpdateHeight(TNode* node) {
        if (node) {
            node->height = 1 + std::max(GetHeight(node->left), GetHeight(node->right));
        }
    }

    TNode* RotateRight(TNode* y) {
        TNode* x = y->left;
        y->left = x->right;
        x->right = y;
        UpdateHeight(y);
        UpdateHeight(x);
        return x;
    }

    TNode* RotateLeft(TNode* x) {
        TNode* y = x->right;
        x->right = y->left;
        y->left = x;
        UpdateHeight(x);
        UpdateHeight(y);
        return y;
    }

    void BalancePath(const std::vector<TNode**>& path) {
        for (int i = (int)path.size() - 1; i >= 0; --i) {
            TNode** nodePtr = path[i];
            TNode* node = *nodePtr;
            
            UpdateHeight(node);
            int balance = GetBalance(node);

            if (balance == 2) {
                if (GetBalance(node->right) < 0) {
                    node->right = RotateRight(node->right);
                }
                *nodePtr = RotateLeft(node);
            } else if (balance == -2) {
                if (GetBalance(node->left) > 0) {
                    node->left = RotateLeft(node->left);
                }
                *nodePtr = RotateRight(node);
            }
        }
    }

    void DestroyTree(TNode* node) {
        if (node) {
            DestroyTree(node->left);
            DestroyTree(node->right);
            delete node;
        }
    }

    void SavePreOrder(TNode* node, std::ofstream& out) const {
        bool hasNode = (node != nullptr);
        out.write(reinterpret_cast<const char*>(&hasNode), sizeof(hasNode));

        if (hasNode) {
            uint16_t len = node->key.size();
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            out.write(node->key.c_str(), len);
            out.write(reinterpret_cast<const char*>(&node->value), sizeof(node->value));

            SavePreOrder(node->left, out);
            SavePreOrder(node->right, out);
        }
    }

    TNode* LoadPreOrder(std::ifstream& in) {
        bool hasNode;
        if (!in.read(reinterpret_cast<char*>(&hasNode), sizeof(hasNode))) return nullptr;
        if (!hasNode) return nullptr;

        uint16_t len;
        if (!in.read(reinterpret_cast<char*>(&len), sizeof(len))) throw std::runtime_error("Bad format");

        std::string key(len, ' ');
        if (!in.read(&key[0], len)) throw std::runtime_error("Bad format");

        uint64_t value;
        if (!in.read(reinterpret_cast<char*>(&value), sizeof(value))) throw std::runtime_error("Bad format");

        TNode* node = new TNode(std::move(key), value);
        node->left = LoadPreOrder(in);
        node->right = LoadPreOrder(in);
        UpdateHeight(node);
        return node;
    }

public:
    AVLTree() : root(nullptr) {}
    
    ~AVLTree() {
        DestroyTree(root);
    }

    bool Insert(std::string word, uint64_t value) {
        ToLowerCase(word);
        
        TNode** curr = &root;
        std::vector<TNode**> path;
        path.reserve(64); 

        while (*curr) {
            path.push_back(curr);
            if (word < (*curr)->key) {
                curr = &((*curr)->left);
            } else if (word > (*curr)->key) {
                curr = &((*curr)->right);
            } else {
                return false; 
            }
        }

        *curr = new TNode(std::move(word), value);
        path.push_back(curr); 

        BalancePath(path);
        return true;
    }

    bool Remove(std::string word) {
        ToLowerCase(word);

        TNode** curr = &root;
        std::vector<TNode**> path;
        path.reserve(64);

        while (*curr && (*curr)->key != word) {
            path.push_back(curr);
            if (word < (*curr)->key) curr = &((*curr)->left);
            else curr = &((*curr)->right);
        }

        if (!*curr) return false; 

        TNode* toDelete = *curr;

        if (toDelete->left && toDelete->right) {
            path.push_back(curr);
            TNode** pred = &(toDelete->left);
            while ((*pred)->right) {
                path.push_back(pred);
                pred = &((*pred)->right);
            }

            toDelete->key = std::move((*pred)->key);
            toDelete->value = (*pred)->value;

            curr = pred;
            toDelete = *curr;
        }

        *curr = toDelete->left ? toDelete->left : toDelete->right;
        delete toDelete;

        BalancePath(path);
        return true;
    }

    bool Search(std::string word, uint64_t& out_value) const {
        ToLowerCase(word);
        
        TNode* curr = root;
        while (curr) {
            if (word < curr->key) curr = curr->left;
            else if (word > curr->key) curr = curr->right;
            else {
                out_value = curr->value;
                return true;
            }
        }
        return false;
    }

    bool Save(const std::string& filepath) const {
        std::ofstream out(filepath, std::ios::binary);
        if (!out.is_open()) return false;
        SavePreOrder(root, out);
        return out.good();
    }

    bool Load(const std::string& filepath) {
        std::ifstream in(filepath, std::ios::binary);
        if (!in.is_open()) return false;

        try {
            TNode* tempRoot = LoadPreOrder(in);
            DestroyTree(root);
            root = tempRoot;
            return true;
        } catch (...) {
            return false;
        }
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    AVLTree tree;
    std::string cmd;

    while (std::cin >> cmd) {
        try {
            if (cmd == "+") {
                std::string word;
                uint64_t value;
                std::cin >> word >> value;
                
                if (tree.Insert(word, value)) {
                    std::cout << "OK\n";
                } else {
                    std::cout << "Exist\n";
                }
            } 
            else if (cmd == "-") {
                std::string word;
                std::cin >> word;
                
                if (tree.Remove(word)) {
                    std::cout << "OK\n";
                } else {
                    std::cout << "NoSuchWord\n";
                }
            } 
            else if (cmd == "!") {
                std::string action, path;
                std::cin >> action >> path;
                
                if (action == "Save") {
                    if (tree.Save(path)) {
                        std::cout << "OK\n";
                    } else {
                        std::cout << "ERROR: Failed to save file (check permissions or path)\n";
                    }
                } 
                else if (action == "Load") {
                    if (tree.Load(path)) {
                        std::cout << "OK\n";
                    } else {
                        std::cout << "ERROR: Failed to load file (bad format or not found)\n";
                    }
                }
            } 
            else {
                uint64_t value;
                if (tree.Search(cmd, value)) {
                    std::cout << "OK: " << value << "\n";
                } else {
                    std::cout << "NoSuchWord\n";
                }
            }
        } 
        catch (const std::bad_alloc& e) {
            std::cout << "ERROR: Out of memory\n";
        } 
        catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << "\n";
        }
    }

    return 0;
}