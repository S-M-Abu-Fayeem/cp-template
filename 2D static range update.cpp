#include <bits/stdc++.h>
using namespace std;

const int N = 1e3 + 9;
long long d[N][N], a[N][N];

int main() {
    int n, m, q; cin >> n >> m >> q;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            cin >> a[i][j];
        }
    }
    while (q--) {
        int x1, y1, x2, y2, x; cin >> x1 >> y1 >> x2 >> y2 >> x;
        d[x1][y1] += x;
        d[x2 + 1][y1] -= x;
        d[x1][y2 + 1] -= x;
        d[x2 + 1][y2 + 1] += x;
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            d[i][j] += d[i - 1][j] + d[i][j - 1] - d[i - 1][j - 1];
        }
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            a[i][j] += d[i][j];
        }
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            cout << a[i][j] << " ";
        }
        cout << endl;
    }
}
