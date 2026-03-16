#include <iostream>
#include <queue>
#include <memory>
#include <algorithm>
#include <climits> // INT_MIN и INT_MAX


struct Node {
    int value;
    Node* parent;
    Node* leftChild;
    Node* rightChild;

    Node(int v) : value(v), parent(nullptr), leftChild(nullptr), rightChild(nullptr) {}
};

class BST { // binary search tree
private:
    Node* root;

    Node* findNode(Node* node, int val) {
        if (!node || node->value == val) return node;
        
        if (node->value > val) return findNode(node->leftChild, val);
        return findNode(node->rightChild, val);
    }

    void destroyTree(Node* node) {
        if (node) {
            destroyTree(node->leftChild);
            destroyTree(node->rightChild);
            delete node;
        }
    }

    int height(Node* node) {
        if (!node) return 0;
        return 1 + std::max(height(node->leftChild), height(node->rightChild));
    }

    Node* getMinSubTree(Node* node) {
        if (!node) return nullptr;
        while (node->leftChild) {
            node = node->leftChild;
        }
        return node;
    }

    Node* getMaxSubTree(Node* node) {
        if (!node) return nullptr;
        while (node->rightChild) {
            node = node->rightChild;
        }
        return node;
    }

    Node* insertRecursive(Node* node, int val, Node* parent = nullptr) {
        if (!node) {
            Node* newNode = new Node(val);
            newNode->parent = parent;
            return newNode;
        }

        if (node->value > val) {
            node->leftChild = insertRecursive(node->leftChild, val, node);
        } else if (node->value < val) {
            node->rightChild = insertRecursive(node->rightChild, val, node);
        }

        return node;
    }

    void inorder(Node* node) {
        if (node) {
            inorder(node->leftChild);
            std::cout << node->value << " ";
            inorder(node->rightChild);
        }
    }

    void preorder(Node* node) {
        if (node) {
            std::cout << node->value << " ";
            preorder(node->leftChild);
            preorder(node->rightChild);
        }
    }

    void postorder(Node* node) {
        if (node) {
            postorder(node->leftChild);
            postorder(node->rightChild);
            std::cout << node->value << " ";
        }
    }

    bool isBST(Node* node, int minVal, int maxVal) {
        if (!node) return true;
        if (node->value <= minVal || node->value >= maxVal) return false;
        return isBST(node->leftChild, minVal, node->value) && isBST(node->rightChild, node->value, maxVal);
    }

    void updateParentLink(Node* parent, Node* oldChild, Node* newChild) {
        if (!parent) return;

        if (parent->leftChild == oldChild) {
            parent->leftChild = newChild;
        } else {
            parent->rightChild = newChild;
        }
    }

    int getValueWidth(int val) {
        if (val == 0) return 1;
        int width = 0;
        if (val < 0) width++;
        while (val != 0) {
            width++;
            val /= 10;
        }
        return width;
    }

public:
    BST() : root(nullptr) {}
    BST(int rootVal) : root(new Node(rootVal)) {}
    ~BST() {
        destroyTree(root);
    }

    void insert(int val) {
        root = insertRecursive(root, val);
    }

    Node* search(int val) {
        return findNode(root, val);
    }

    bool deleteSubTree(int val) {
        Node* target = findNode(root, val);
        if (!target) return false;

        if (target == root) {
            destroyTree(root);
            root = nullptr;
            return true;
        }

        updateParentLink(target->parent, target, nullptr);
        destroyTree(target);
        return true;
    }

    bool remove(int val) {
        Node* target = findNode(root, val);
        if (!target) return false;

        // 1. Лист
        if (!target->leftChild && !target->rightChild) {
            if (target == root) {
                delete root;
                root = nullptr;
            } else {
                updateParentLink(target->parent, target, nullptr);
                delete target;
            }
        }

        // 2. Есть один потомок
        else if (!target->leftChild || !target->rightChild) {
            Node* child = (target->leftChild) ? target->leftChild : target->rightChild;
            
            if (target == root) {
                root = child;
                child->parent = nullptr;
            } else {
                child->parent = target->parent;
                updateParentLink(target->parent, target, child);
            }
            delete target;
        }

        // 3. Два потомка
        else if (target->leftChild && target->rightChild) {
            Node* successor = getMinSubTree(target->rightChild);
            int successorVal = successor->value;

            remove(successor->value);
            target->value = successorVal;
        }

        return true;
    }

    void inorder() {
        std::cout << "Inorder traversal: ";
        inorder(root);
        std::cout << std::endl;
    }
    
    void preorder() {
        std::cout << "Preorder traversal: ";
        preorder(root);
        std::cout << std::endl;
    }
    
    void postorder() {
        std::cout << "Postorder traversal: ";
        postorder(root);
        std::cout << std::endl;
    }
    
    Node* getRoot() {
        return root;
    }

    int getMin() {
        Node* minNode = getMinSubTree(root);
        return minNode ? minNode->value : -1; // -1 = пустое дерево
    }
    
    int getMax() {
        Node* maxNode = getMaxSubTree(root);
        return maxNode ? maxNode->value : -1;
    }
    
    int getHeight() {
        return height(root);
    }

    bool isEmpty() {
        return root == nullptr;
    }
    
    void clear() {
        destroyTree(root);
        root = nullptr;
    }
    
    bool isValidBST() {
        return isBST(root, INT_MIN, INT_MAX);
    }

    // за вайбокжено
    void printFormattedLevelOrder() {
        if (!root) {
            std::cout << "\nДерево пустое. Рисовать нечего\n";
            return;
        }
        
        std::cout << "\nДерево:\n";
        
        // Вычисляем высоту для форматирования
        int h = height(root);
        
        std::queue<Node*> q;
        q.push(root);
        
        int level = 0;
        int maxWidth = getValueWidth(getMax()) + 2; // ширина ячейки
        
        while (!q.empty() && level < h) {
            int levelSize = q.size();
            
            // Вычисляем начальный отступ для этого уровня
            int indent = (1 << (h - level - 1)) * maxWidth / 2;
            for (int i = 0; i < indent; i++) {
                std::cout << " ";
            }
            
            std::queue<Node*> nextLevel;
            
            for (int i = 0; i < levelSize; i++) {
                Node* current = q.front();
                q.pop();
                
                // Печатаем значение узла
                if (current) {
                    std::cout << current->value;
                    
                    // Добавляем пробелы до следующего узла
                    int spaces = (1 << (h - level)) * maxWidth - getValueWidth(current->value);
                    for (int j = 0; j < spaces; j++) {
                        std::cout << " ";
                    }
                    
                    // Добавляем детей в очередь для следующего уровня
                    nextLevel.push(current->leftChild);
                    nextLevel.push(current->rightChild);
                } else {
                    // Если узел пустой, печатаем пробелы
                    int spaces = (1 << (h - level)) * maxWidth;
                    for (int j = 0; j < spaces; j++) {
                        std::cout << " ";
                    }
                    
                    nextLevel.push(nullptr);
                    nextLevel.push(nullptr);
                }
            }
            
            std::cout << std::endl;
            
            // Переходим к следующему уровню
            q = nextLevel;
            level++;
        }
    }
};

int main() {
    std::cout << "=== Тест 1: Создание пустого дерева ===\n";
    BST tree1;

    tree1.printFormattedLevelOrder();

    std::cout << "Дерево пустое: " << (tree1.isEmpty() ? "Да" : "Нет") << std::endl;
    std::cout << "Min: " << tree1.getMin() << std::endl;
    std::cout << "Max: " << tree1.getMax() << std::endl;
    std::cout << "Height: " << tree1.getHeight() << std::endl;
    std::cout << "Is valid BST: " << (tree1.isValidBST() ? "Да" : "Нет") << std::endl;


    std::cout << "\n=== Тест 2: Вставка элементов ===\n";
    BST tree2;
    std::cout << "Вставляем: 50, 30, 70, 20, 40, 60, 80, 35, 45\n";
    tree2.insert(50);
    tree2.insert(30);
    tree2.insert(70);
    tree2.insert(20);
    tree2.insert(40);
    tree2.insert(60);
    tree2.insert(80);
    tree2.insert(35);
    tree2.insert(45);

    tree2.printFormattedLevelOrder();

    tree2.inorder();
    tree2.preorder();
    tree2.postorder();
    
    std::cout << "Min: " << tree2.getMin() << std::endl;
    std::cout << "Max: " << tree2.getMax() << std::endl;
    std::cout << "Height: " << tree2.getHeight() << std::endl;
    std::cout << "Is valid BST: " << (tree2.isValidBST() ? "Да" : "Нет") << std::endl;


    std::cout << "\n=== Тест 3: Поиск элементов ===\n";
    int searchValues[] = {20, 35, 50, 100};
    for (int val : searchValues) {
        Node* found = tree2.search(val);
        std::cout << "Поиск " << val << ": " << (found ? "Найден" : "Не найден");
        if (found && found->parent) {
            std::cout << " (родитель: " << found->parent->value << ")";
        }
        std::cout << std::endl;
    }

    std::cout << "\n=== Тест 4: Удаление листьев ===\n";
    std::cout << "Удаляем 20 (лист):\n";
    tree2.remove(20);
    tree2.printFormattedLevelOrder();

    std::cout << "Height: " << tree2.getHeight() << std::endl;
    
    std::cout << "\nУдаляем 45 (лист):\n";
    tree2.remove(45);
    tree2.printFormattedLevelOrder();
    std::cout << "Height: " << tree2.getHeight() << std::endl;


    std::cout << "\n=== Тест 5: Удаление узлов с одним ребенком ===\n";
    BST tree3;
    std::cout << "Создаем дерево: 50, 30, 20, 40, 35\n";
    tree3.insert(50);
    tree3.insert(30);
    tree3.insert(20);
    tree3.insert(40);
    tree3.insert(35);
    tree3.printFormattedLevelOrder();
    
    std::cout << "\nУдаляем 40 (имеет одного ребенка - 35):\n";
    tree3.remove(40);
    tree3.printFormattedLevelOrder();


    std::cout << "\n=== Тест 6: Удаление узлов с двумя детьми ===\n";
    BST tree4;
    std::cout << "Создаем дерево: 50, 30, 70, 20, 40, 60, 80\n";
    tree4.insert(50);
    tree4.insert(30);
    tree4.insert(70);
    tree4.insert(20);
    tree4.insert(40);
    tree4.insert(60);
    tree4.insert(80);
    tree4.printFormattedLevelOrder();

    std::cout << "\nУдаляем 30 (имеет двух детей - 20 и 40):\n";
    tree4.remove(30);
    tree4.printFormattedLevelOrder();
    
    std::cout << "\nУдаляем 50 (корень с двумя детьми):\n";
    tree4.remove(50);
    tree4.printFormattedLevelOrder();


    std::cout << "\n=== Тест 7: Удаление несуществующего элемента ===\n";
    bool result = tree4.remove(999);
    std::cout << "Результат удаления 999: " << (result ? "Успех" : "Не найден") << std::endl;


    std::cout << "\n=== Тест 8: Удаление поддеревьев ===\n";
    BST tree5;
    std::cout << "Создаем дерево: 50, 30, 70, 20, 40, 60, 80, 35, 45, 65, 75\n";
    tree5.insert(50);
    tree5.insert(30);
    tree5.insert(70);
    tree5.insert(20);
    tree5.insert(40);
    tree5.insert(60);
    tree5.insert(80);
    tree5.insert(35);
    tree5.insert(45);
    tree5.insert(65);
    tree5.insert(75);
    tree5.printFormattedLevelOrder();
    
    std::cout << "\nУдаляем поддерево с корнем 70:\n";
    tree5.deleteSubTree(70);
    tree5.printFormattedLevelOrder();
    std::cout << "Height: " << tree5.getHeight() << std::endl;
    

    std::cout << "\n=== Тест 9: Очистка дерева ===\n";
    std::cout << "Дерево до очистки: ";
    tree5.printFormattedLevelOrder();
    tree5.clear();
    std::cout << "Дерево после очистки: ";
    tree5.printFormattedLevelOrder();
    std::cout << "Is empty: " << (tree5.isEmpty() ? "Да" : "Нет") << std::endl;


    std::cout << "\n=== Тест 10: Вставка дубликатов ===\n";
    BST tree6;
    tree6.insert(50);
    tree6.insert(50);  // дубликат
    tree6.insert(30);
    tree6.insert(30);  // дубликат
    tree6.printFormattedLevelOrder();
    std::cout << "Дубликаты не должны быть\n";
    

    std::cout << "\n=== Тест 11: Проверка isValidBST на сломанном дереве ===\n";
    BST tree7;
    tree7.insert(50);
    tree7.insert(30);
    tree7.insert(70);
    tree7.printFormattedLevelOrder();
    std::cout << "До поломки дерева:\n";
    std::cout << "Is valid BST: " << (tree7.isValidBST() ? "Да" : "Нет") << std::endl;
    
    // Ломаем дерево вручную
    Node* root = tree7.getRoot();
    root->leftChild->value = 60;  // 60 < 50? Нет! Дерево сломано
    tree7.printFormattedLevelOrder();
    
    std::cout << "После поломки дерева (левый ребенок стал 60):\n";
    std::cout << "Is valid BST: " << (tree7.isValidBST() ? "Да" : "Нет") << std::endl;

    
    std::cout << "\n=== Тест 12: Правый потомок с длинной левой цепочкой ===\n";
    BST tree11;
    std::cout << "Создаем дерево: 50, 40, 60, 55, 53, 57, 70\n";
    tree11.insert(50);
    tree11.insert(40);
    tree11.insert(60);
    tree11.insert(55);
    tree11.insert(53);
    tree11.insert(57);
    tree11.insert(70);
    tree11.printFormattedLevelOrder();
    
    std::cout << "\nУдаляем 50 (successor должен быть 53):\n";
    tree11.remove(50);
    tree11.printFormattedLevelOrder();
    
    return 0;
}