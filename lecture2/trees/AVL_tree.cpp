#include <iostream>
#include <algorithm>
#include <memory>
#include <queue>

struct Node {
    int value;
    int balance;
    std::weak_ptr<Node> parent;
    std::shared_ptr<Node> leftChild;
    std::shared_ptr<Node> rightChild;

    Node(int v) : value(v), balance(0), parent(), leftChild(nullptr), rightChild(nullptr) {}
};

class AVL {
private:
    std::shared_ptr<Node> root;

    std::shared_ptr<Node> findNode(std::shared_ptr<Node> node, int val) {
        if (!node) return nullptr;

        if (node->value == val) return node;
        else if (node->value > val) return findNode(node->leftChild, val);
        else return findNode(node->rightChild, val);
    }

    std::shared_ptr<Node> getMinSubTree(std::shared_ptr<Node> node) {
        if (!node) return nullptr;
        while (node->leftChild) {
            node = node->leftChild;
        }
        return node;
    }

    std::shared_ptr<Node> getMaxSubTree(std::shared_ptr<Node> node) {
        if (!node) return nullptr;
        while (node->rightChild) {
            node = node->rightChild;
        }
        return node;
    }

    int getMin() {
        auto minNode = getMinSubTree(root);
        return minNode ? minNode->value : 0;
    }

    int getMax() {
        auto maxNode = getMaxSubTree(root);
        return maxNode ? maxNode->value : 0;
    }


    /*
    Высота узла: количество рёбер от узла до самого глубокого листа
    Лист имеет высоту 0, пустое поддерево имеет высоту -1
    */
    int height(std::shared_ptr<Node> node) {
        if (!node) return -1;
        return 1 + std::max(height(node->leftChild), height(node->rightChild));
    } 

    /*
    Глубина узла: количество рёбер от корня до узла
    Корень имеет глубину 0
    */
    int depth(std::shared_ptr<Node> node) {
        if (!node) return -1;
        if (!node->parent.lock()) return 0; // метод lock() у waek_ptr временно превращает указатель в shared_ptr
        return 1 + depth(node->parent.lock());
    }

    std::shared_ptr<Node> rotateRight(std::shared_ptr<Node> oldParent) {
        std::shared_ptr<Node> newParent = oldParent->leftChild;
        std::shared_ptr<Node> subTree = newParent->rightChild;

        newParent->rightChild = oldParent;
        oldParent->leftChild = subTree;

        newParent->parent = oldParent->parent;
        oldParent->parent = newParent;
        if (subTree) subTree->parent = oldParent;

        // обновление balance
        if (newParent->balance >= 0) {
            oldParent->balance = oldParent->balance - newParent->balance - 1;
        } else {
            oldParent->balance = oldParent->balance - 1;
        }

        if (oldParent->balance <= 0) {
            newParent->balance = newParent->balance + oldParent->balance - 1;
        } else {
            newParent->balance = newParent->balance - 1;
        }

        return newParent;
    }

    std::shared_ptr<Node> rotateLeft(std::shared_ptr<Node> oldParent) {
        std::shared_ptr<Node> newParent = oldParent->rightChild;
        std::shared_ptr<Node> subTree = newParent->leftChild;

        newParent->leftChild = oldParent;
        oldParent->rightChild = subTree;

        newParent->parent = oldParent->parent;
        oldParent->parent = newParent;
        if (subTree) subTree->parent = oldParent;

        // обновление balance
        if (newParent->balance <= 0) {
            oldParent->balance = oldParent->balance - newParent->balance + 1;
        } else {
            oldParent->balance = oldParent->balance + 1;
        }

        if (oldParent->balance >= 0) {
            newParent->balance = newParent->balance + oldParent->balance + 1;
        } else {
            newParent->balance = newParent->balance + 1;
        }

        return newParent;
    }

    std::shared_ptr<Node> rotateLeftRight(std::shared_ptr<Node> node) {
        node->leftChild = rotateLeft(node->leftChild);
        if (node->leftChild) node->leftChild->parent = node;
        return rotateRight(node);
    }

    std::shared_ptr<Node> rotateRightLeft(std::shared_ptr<Node> node) {
        node->rightChild = rotateRight(node->rightChild);
        if (node->rightChild) node->rightChild->parent = node;
        return rotateLeft(node);
    }

    std::shared_ptr<Node> balanceNode(std::shared_ptr<Node> node) {
        if (!node) return nullptr;

        if (node->balance == 2) {
            if (node->leftChild->balance >= 0) {
                return rotateRight(node);
            } else {
                return rotateLeftRight(node);
            }
        } else if (node->balance == -2) {
            if (node->rightChild->balance <= 0) {
                return rotateLeft(node);
            } else {
                return rotateRightLeft(node);
            }
        }
        return node;
    }

    std::shared_ptr<Node> insertNode(std::shared_ptr<Node> node, int val, bool& heightIncreased) {
        if (!node) {
            heightIncreased = true;
            return std::make_shared<Node>(val);
        }

        if (node->value > val) {
            auto newLeftChild = insertNode(node->leftChild, val, heightIncreased);

            if (newLeftChild != node->leftChild) {
                node->leftChild = newLeftChild;
                if (newLeftChild) {
                    newLeftChild->parent = node;
                }
            }

            if (heightIncreased) {
                node->balance++;
                if (node->balance == 0) {
                    heightIncreased = false; // было -1 стало 0 => высота не изменилась
                } else if (node->balance == 1) {
                    heightIncreased = true; // было 0 стало 1 => высота изменилась
                } else if (node->balance == 2) {
                    auto balanced = balanceNode(node); // было 1 стало 2 => высота изменилась
                    heightIncreased = false;
                    return balanced;
                }
            }

            return node;

        } else if (node->value < val) {
            auto newRightChild = insertNode(node->rightChild, val, heightIncreased);

            if (newRightChild != node->rightChild) {
                node->rightChild = newRightChild;
                if (newRightChild) {
                    newRightChild->parent = node;
                }
            }

            if (heightIncreased) {
                node->balance--; 
                if (node->balance == 0) {
                    heightIncreased = false; // было 1 стало 0
                } else if (node->balance == -1) {
                    heightIncreased = true; // было 0 стало -1
                } else if (node->balance == -2) {
                    auto balanced = balanceNode(node); // было -1 стало -2
                    heightIncreased = false;
                    return balanced;
                }
            }

            return node;

        } else {
            heightIncreased = false;
            return node;
        }
    }

    std::shared_ptr<Node> removeNode(std::shared_ptr<Node> node, int val) {
        if (!node) return nullptr;
        // здесь нужна логика удаления и балансировки
        return node;
    }

    int getValueWidth(int val) {
        if (val == 0) return 1;
        int width = 0;
        if (val < 0) ++width; // схавали минус типо
        while (val != 0) {
            ++width;
            val /= 10;
        }
        return width;
    }

    void printTree() {
        if (!root) {
            std::cout << "Дерево пустое. Рисовать нечего\n";
            return;
        }

        std::cout << "\nДерево:\n";

        int h = treeHeight();
        std::queue<std::shared_ptr<Node>> q;
        q.push(root);

        int level = 0, maxWidth = getValueWidth(getMax()) + 2;

        while (!q.empty() && level <= h) {
            int levelSize = q.size();
            
            // начальный отступ для каждой строки
            // int indent = (1 << (h - level - 1)) * maxWidth / 2;
            /*
            Поскольку цикл while идет при level <= h, на последней итерации level будет равен h.
            Получится вычисление 1 << -1 (сдвиг битов на отрицательное число). 
            В C++ это Undefined Behavior, что бэд практик.
            */
            int indent = 0;
            if (h - level - 1 >= 0) {
                indent = (1 << (h - level - 1)) * maxWidth / 2;
            }
            for (int i = 0; i < indent; ++i) {
                std::cout << " ";
            }


            std::queue<std::shared_ptr<Node>> nextLevel;
            for (int i = 0; i < levelSize; ++i) {
                auto cur = q.front();
                q.pop();

                if (cur) {
                    std::cout << cur->value << "(" << cur->balance << ")";
                    // дырка между значениями в строке
                    int spaces = (1 << (h - level)) * maxWidth - getValueWidth(cur->value);
                    for (int j = 0; j < spaces; ++j) {
                        std::cout << " ";
                    }

                    nextLevel.push(cur->leftChild);
                    nextLevel.push(cur->rightChild);
                } else {
                    // просто дырка до возможно следующего
                    int spaces = (1 << (h - level)) * maxWidth;
                    for (int j = 0; j < spaces; j++) {
                        std::cout << " ";
                    }
                    
                    nextLevel.push(nullptr);
                    nextLevel.push(nullptr);
                }
            }
            std::cout << std::endl;
            q = nextLevel;
            ++level;
        }
    }



public:
    AVL() : root(nullptr) {}
    AVL(int val) : root(std::make_shared<Node>(val)) {}
    ~AVL() {}

    std::shared_ptr<Node> search(int val) {
        return findNode(root, val);
    }

    void insert(int val) {
        bool heightIncreased = false;
        root = insertNode(root, val, heightIncreased);
        if (root) {
            root->parent.reset();
        }
    }

    void remove(int val) {
        root = removeNode(root, val);
    }

    std::shared_ptr<Node> getRoot() {
        return root;
    }

    int treeHeight() {
        return height(root);
    }

    int nodeHeight(std::shared_ptr<Node> node) {
        return height(node);
    }

    int nodeDepth(std::shared_ptr<Node> node) {
        return depth(node);
    }

    void print() {
        printTree();
    }

    bool isEmpty() {
        return root == nullptr;
    }
};

/*
Что мне нужно в AVL дереве:
1) поиск элемента
2) вставка элемента
3) удаление элемента

Что нужно для поиска элемента:
рекурсивно запускать фукнцию поиска, если число которое ищу меньше, 
то налево, если больше, то на право.
Если в одной из веток был найден результат - возвращаем его.
Если результата не было найдено при полном обходе дерева в глубину - возвращаем nullptr

Что нужно для вставки:
Нужно проверить, что такого значения в дереве нет через findNode
Потом нужно рекурсивно или обходом в ширину найти лист, потомком которого станет встраиваемый элемент
Дальше мы его вставляем и по алгоритму от макарова меняем счётчик balance.
Если получилось, что всё норм проверим функций isAVL, что дерево точно AVL
Если получилось, что нужно балансировать - обратимся к методу балансировки balanceTree
После балансировки опять проверим, что дерево отбалансировалось методом isAVL
Если всё гуд - элемент вставлен

Что нужно для алгоритма балансировки:
1) посмотреть нормально, как балансируется AVL дерево
2) вариант с счётчиком высоты при балансировке - не очень мне подходит, потому что
    - преподаватель сказал, что нужно иметь счётчик balance, который изменяется от -2 до 2 включительно. Если мы пришли к предку из левого потомка, до увеличиваем счётчик на 1, если из правого до уменьшаем на 1. Пока разница этих счётчиков по модулю меньше 1, то балансировать ничего не нужно.
    - если мы вставили элемент и в родителе стало 1 или -1, то это значит, что раньше это был лист, ничего не делаем - дерево сбалансировано
    - если мы вставили элемент и в родителе стало 0, то это значит, что раньше у родителя был 1 потомок, теперь их стало 2
    - если мы вставили элемент и в родителе стало -2 или 2, то мы довесили жирного потомка к листу, который тоже был единственным потомком - нужно балансировать
3) опять же, посмотреть, как понять и куда крутить дерево для балансировки при такой реализации. Типо вот мы поняли, что нужно крутить, отрицательное/положительное число должны дать намёк куда и в какую сторону крутить
4) нарисовать примеры от руки, чтобы было понятнее

Где-то тут в плане удаление...
*/


/*int main() {
    // Создадим дерево вручную для теста поиска
    AVL tree(50);  // корень 50
    tree.print();
    
    // TODO: потом заменим на нормальную вставку
    // Пока вручную создадим структуру:
    //        50(0)
    //       /    \
    //     30(0)  70(0)
    //    /   \   /  \
    //  20(0)40(0)60(0)80(0)
    
    // Получаем доступ к корню (для ручного создания)
    auto root = tree.getRoot();
    
    // Создаем левое поддерево
    root->leftChild = std::make_shared<Node>(30);
    root->leftChild->parent = root;
    
    root->leftChild->leftChild = std::make_shared<Node>(20);
    root->leftChild->leftChild->parent = root->leftChild;
    tree.print();
    
    root->leftChild->rightChild = std::make_shared<Node>(40);
    root->leftChild->rightChild->parent = root->leftChild;
    
    // Создаем правое поддерево
    root->rightChild = std::make_shared<Node>(70);
    root->rightChild->parent = root;
    tree.print();
    
    root->rightChild->leftChild = std::make_shared<Node>(60);
    root->rightChild->leftChild->parent = root->rightChild;
    
    root->rightChild->rightChild = std::make_shared<Node>(80);
    root->rightChild->rightChild->parent = root->rightChild;
    
    // Выводим дерево
    tree.print();
    
    // Тестируем поиск
    std::cout << "=== Тест поиска ===\n";
    
    int testValues[] = {50, 30, 70, 20, 40, 60, 80, 100, 25};
    std::cout << "Высота дерева: " << tree.treeHeight() << std::endl;
    
    for (int val : testValues) {
        auto found = tree.search(val);
        if (found) {
            std::cout << "Найден: " << val;
            if (auto parent = found->parent.lock()) {
                std::cout << " (родитель: " << parent->value << ");";
            }
            std::cout << " Высота: " << tree.nodeHeight(found) << ";";
            std::cout << " Глубина: " << tree.nodeDepth(found);
            std::cout << std::endl;
        } else {
            std::cout << "Не найден: " << val << std::endl;
        }
    }
    
    return 0;
}
*/

/*
int main() {
    std::cout << "\n=== ТЕСТ 1: Правый поворот (LL случай) ===\n";
    AVL tree(50);  // корень 50
    
    // TODO: потом заменим на нормальную вставку
    // Пока вручную создадим структуру:
    //        50(2)
    //       /    
    //     30(1)
    //    /     
    //  20(0)
    
    // Получаем доступ к корню (для ручного создания)
    auto root = tree.getRoot();
    
    // Создаем левое поддерево
    root->leftChild = std::make_shared<Node>(30);
    root->leftChild->parent = root;
    root->balance++;
    tree.print();
    
    root->leftChild->leftChild = std::make_shared<Node>(20);
    root->leftChild->leftChild->parent = root->leftChild;
    root->leftChild->balance++;
    root->balance++;
    tree.print();

    root->rightChild = std::make_shared<Node>(60);
    root->rightChild->parent = root;
    root->balance--;
    tree.print();

    root->leftChild->leftChild->leftChild = std::make_shared<Node>(10);
    root->leftChild->leftChild->leftChild->parent = root->leftChild->leftChild;
    root->leftChild->leftChild->balance++;
    root->leftChild->balance++;
    tree.print();
    
    std::cout << "Дерево до правого поворота:";
    tree.print();
    
    std::shared_ptr<Node> oldNode = root->leftChild;  // 30
    std::shared_ptr<Node> parent = oldNode->parent.lock();  // 50 (ЗАПОМИНАЕМ!)
    std::shared_ptr<Node> newNode = tree.rotateRight(oldNode);  // 20
    
    // Обновляем ссылку у родителя (50)
    if (parent) {
        parent->leftChild = newNode;
        newNode->parent = parent;
    }
    
    std::cout << "Дерево после правого поворота:";
    tree.print();

}
*/

int main() {
    AVL tree;

    std::vector<int> nums = {3, 14, 15, 9, 2, 6, 5, 35};
    std::cout << "В AVL дерево будут добавлены: ";
    for (int num : nums) {
        std::cout << num << " ";
    }
    std::cout << "\n";

    for (int num : nums) {
        std::cout << "\nВставляем: " << num;
        tree.insert(num);
        tree.print();
    }


}