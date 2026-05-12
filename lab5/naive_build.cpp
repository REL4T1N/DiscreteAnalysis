#include <iostream>
#include <string>

using namespace std;

int main() {
    string s;
    if (!(cin >> s)) return 0;
    int n = s.length();
    string d = s + s;
    int best = 0;

    for (int i = 1; i < n; ++i) {
        // Сравниваем посимвольно, не создавая новые строки
        for (int j = 0; j < n; ++j) {
            if (d[i + j] < d[best + j]) {
                best = i;
                break;
            } else if (d[i + j] > d[best + j]) {
                break;
            }
        }
    }
    cout << d.substr(best, n) << endl;
    return 0;
}