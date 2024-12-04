#include<bits/stdc++.h>
using namespace std;

const int N = 2e5 + 9, M = 2e5 + 9;

struct ST {
  int t[4 * M];
  static const int inf = 1e9;
  ST() {
    memset(t, 0, sizeof t);
  }
  void upd(int n, int b, int e, int i, int x) {
    if (b > i || e < i) return;
    if (b == e && b == i) {
      t[n] = max(t[n], x);
      return;
    }
    int mid = (b + e) >> 1, l = n << 1, r = l | 1;
    upd(l, b, mid, i, x);
    upd(r, mid + 1, e, i, x);
    t[n] = max(t[l], t[r]);
  }
  int query(int n, int b, int e, int i, int j) {
    if (b > j || e < i) return -inf;
    if (b >= i && e <= j) return t[n];
    int mid = (b + e) >> 1, l = n << 1, r = l | 1;
    int L = query(l, b, mid, i, j);
    int R = query(r, mid + 1, e, i, j);
    return max(L, R);
  }
}t;
int a[N], dp[N];
int32_t main() {
  ios_base::sync_with_stdio(0);
  cin.tie(0);
  int n; cin >> n;
  set<int> se;
  for (int i = 1; i <= n; i++) {
    cin >> a[i];
    se.insert(a[i]);
  }
  map<int, int> mp;
  int id = 0;
  for (auto x: se) {
    mp[x] = ++id;
  }
  for (int i = 1; i <= n; i++) {
    a[i] = mp[a[i]]; // coordinate compression, now all a[i] are 1 <= a[i] <= n which is cute
  }
  for (int i = 1; i <= n; i++) {
    dp[i] = 1;
    // int mx = 0;
    // for (int j = 1; j < i; j++) {
    //   if (a[j] < a[i]) {
    //     mx = max(mx, dp[j]);
    //   }
    // }
    // optimized the loop
    if (a[i] != 1) {
      int mx = t.query(1, 1, M, 1, a[i] - 1);
      mx++;
      dp[i] = max(dp[i], mx);
    }
    t.upd(1, 1, M, a[i], dp[i]);
  }
  int ans = 0;
  for (int i = 1; i <= n; i++) {
    ans = max(ans, dp[i]);
  }
  cout << ans << '\n';
  return 0;
}
