#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    string s;
    if (!(cin >> s)) return 0;
    int n = s.length();
    string d = s + s;
    int i = 0, j = 1, k = 0;
    while (i < n && j < n && k < n) {
        int diff = d[i+k] - d[j+k];
        if (diff == 0) k++;
        else {
            if (diff > 0) i += k + 1;
            else j += k + 1;
            if (i == j) j++;
            k = 0;
        }
    }
    cout << d.substr(min(i, j), n) << "\n";
    return 0;
}