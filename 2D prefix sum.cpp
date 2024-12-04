#include <bits/stdc++.h>
using namespace std;

const int N = 1e3 + 9;
long long pref[N][N];
int a[N][N];

int main() {
    int n, m; cin >> n >> m;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            cin >> a[i][j];
        }
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            pref[i][j] = pref[i - 1][j] + pref[i][j - 1] - pref[i - 1][j - 1] + a[i][j];
        }
    }

    int q; cin >> q;
    while (q--) {
        int x1, y1, x2, y2; cin >> x1 >> y1 >> x2 >> y2;
        // long long ans = 0;
        // for (int i = x1; i <= x2; i++) {
        //   for (int j = y1; j <= y2; j++) {
        //     ans += a[i][j];
        //   }
        // }
        long long ans = pref[x2][y2] - pref[x1 - 1][y2] - pref[x2][y1 - 1] + pref[x1 - 1][y1 - 1];
        cout << ans << endl;
    }
}
