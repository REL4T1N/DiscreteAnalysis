#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>

using namespace std;

void generate(string filename, int n, int type) {
    ofstream out(filename);
    if (type == 1) { // Random
        for (int i = 0; i < n; ++i) out << (char)('a' + rand() % 26);
    } else if (type == 2) { // Monotone
        for (int i = 0; i < n; ++i) out << 'a';
    } else if (type == 3) { // Killer (Worst case for naive)
        for (int i = 0; i < n - 1; ++i) out << 'a';
        out << 'b';
    }
    out << endl;
}

int main() {
    int n;
    cout << "Enter string length: ";
    cin >> n;
    generate("test_random.txt", n, 1);
    generate("test_monotone.txt", n, 2);
    generate("test_killer.txt", n, 3);
    return 0;
}