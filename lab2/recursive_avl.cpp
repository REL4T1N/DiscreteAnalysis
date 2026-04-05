#include <iostream>
#include <string>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <new> 

class AVLTree {
private:
    struct TNode {
        std::string key;
        uint64_t value;
        uint8_t height;
        std::unique_ptr<TNode> left;
        std::unique_ptr<TNode> right;

        TNode(std::string k, uint64_t v) 
            : key(std::move(k)), value(v), height(1), left(nullptr), right(nullptr) {}
    };

    std::unique_ptr<TNode> root;

    static inline void ToLowerCase(std::string& word) {
        for (char& c : word) {
            if (c >= 'A' && c <= 'Z') {
                c += 32;
            }
        }
    }

    uint8_t GetHeight(const std::unique_ptr<TNode>& node) const {
        return node ? node->height : 0;
    }

    int GetBalance(const std::unique_ptr<TNode>& node) const {
        return node ? (int)GetHeight(node->right) - (int)GetHeight(node->left) : 0;
    }

    void UpdateHeight(const std::unique_ptr<TNode>& node) {
        if (node) {
            node->height = 1 + std::max(GetHeight(node->left), GetHeight(node->right));
        }
    }

    std::unique_ptr<TNode> RotateRight(std::unique_ptr<TNode> y) {
        std::unique_ptr<TNode> x = std::move(y->left);
        y->left = std::move(x->right);
        
        UpdateHeight(y);
        UpdateHeight(x);
        
        x->right = std::move(y);
        return x;
    }

    std::unique_ptr<TNode> RotateLeft(std::unique_ptr<TNode> x) {
        std::unique_ptr<TNode> y = std::move(x->right);
        x->right = std::move(y->left);
        
        UpdateHeight(x);
        UpdateHeight(y);
        
        y->left = std::move(x);
        return y;
    }

    std::unique_ptr<TNode> Balance(std::unique_ptr<TNode> node) {
        if (!node) return nullptr;

        UpdateHeight(node);
        int balance = GetBalance(node);

        if (balance == 2) {
            if (GetBalance(node->right) < 0) {
                node->right = RotateRight(std::move(node->right));
            }
            return RotateLeft(std::move(node));
        }
        if (balance == -2) {
            if (GetBalance(node->left) > 0) {
                node->left = RotateLeft(std::move(node->left));
            }
            return RotateRight(std::move(node));
        }

        return node;
    }

    std::unique_ptr<TNode> InsertImpl(std::unique_ptr<TNode> node, const std::string& key, uint64_t value, bool& added) {
        if (!node) {
            added = true;
            return std::make_unique<TNode>(key, value);
        }

        if (key < node->key) {
            node->left = InsertImpl(std::move(node->left), key, value, added);
        } else if (key > node->key) {
            node->right = InsertImpl(std::move(node->right), key, value, added);
        } else {
            added = false; 
            return node;
        }

        return Balance(std::move(node));
    }

    std::unique_ptr<TNode> RemoveImpl(std::unique_ptr<TNode> node, const std::string& key, bool& removed) {
        if (!node) {
            removed = false;
            return nullptr;
        }

        if (key < node->key) {
            node->left = RemoveImpl(std::move(node->left), key, removed);
        } else if (key > node->key) {
            node->right = RemoveImpl(std::move(node->right), key, removed);
        } else {
            removed = true;
            if (!node->left) return std::move(node->right);
            if (!node->right) return std::move(node->left);

            TNode* maxNode = node->left.get();
            while (maxNode->right) {
                maxNode = maxNode->right.get();
            }

            node->key = maxNode->key;
            node->value = maxNode->value;

            bool dummy = false;
            node->left = RemoveImpl(std::move(node->left), node->key, dummy);
        }

        return Balance(std::move(node));
    }

    TNode* SearchImpl(const std::unique_ptr<TNode>& node, const std::string& key) const {
        if (!node) return nullptr;

        if (key < node->key) return SearchImpl(node->left, key);
        if (key > node->key) return SearchImpl(node->right, key);
        
        return node.get();
    }


    void SavePreOrder(const std::unique_ptr<TNode>& node, std::ofstream& out) const {
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

    std::unique_ptr<TNode> LoadPreOrder(std::ifstream& in) {
        bool hasNode;
        if (!in.read(reinterpret_cast<char*>(&hasNode), sizeof(hasNode))) {
            return nullptr; 
        }

        if (!hasNode) return nullptr;

        uint16_t len;
        if (!in.read(reinterpret_cast<char*>(&len), sizeof(len))) throw std::runtime_error("Bad format");

        std::string key(len, ' ');
        if (!in.read(&key[0], len)) throw std::runtime_error("Bad format");

        uint64_t value;
        if (!in.read(reinterpret_cast<char*>(&value), sizeof(value))) throw std::runtime_error("Bad format");

        auto node = std::make_unique<TNode>(std::move(key), value);
        
        node->left = LoadPreOrder(in);
        node->right = LoadPreOrder(in);
        
        UpdateHeight(node);
        return node;
    }

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree() = default;

    bool Insert(std::string word, uint64_t value) {
        ToLowerCase(word);
        bool added = false;
        root = InsertImpl(std::move(root), word, value, added);
        return added;
    }

    bool Remove(std::string word) {
        ToLowerCase(word);
        bool removed = false;
        root = RemoveImpl(std::move(root), word, removed);
        return removed;
    }

    bool Search(std::string word, uint64_t& out_value) const {
        ToLowerCase(word);
        TNode* found = SearchImpl(root, word);
        if (found) {
            out_value = found->value;
            return true;
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
            std::unique_ptr<TNode> tempRoot = LoadPreOrder(in);
            
            root = std::move(tempRoot);
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