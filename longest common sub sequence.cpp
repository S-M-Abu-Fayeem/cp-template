#include<bits/stdc++.h>
using namespace std;

const int N = 3030;
string a, b;
int dp[N][N];
int lcs(int i, int j) {
  if (i >= a.size() or j >= b.size()) return 0;
  if (dp[i][j] != -1) return dp[i][j];
  int ans = lcs(i + 1, j);
  ans = max(ans, lcs(i, j + 1));
  if (a[i] == b[j]) {
    ans = max(ans, lcs(i + 1, j + 1) + 1);
  }
  return dp[i][j] = ans;
}
void print(int i, int j) {
  if (i >= a.size() or j >= b.size()) return;
  if (a[i] == b[j]) {
    cout << a[i];
    print(i + 1, j + 1);
    return;
  }
  int x = lcs(i + 1, j);
  int y = lcs(i, j + 1);
  if (x >= y) {
    print(i + 1, j);
  }
  else {
    print(i, j + 1);
  }
}
int32_t main() {
  ios_base::sync_with_stdio(0);
  cin.tie(0);
  cin >> a >> b;
  memset(dp, -1, sizeof dp);
  // cout << lcs(0, 0) << '\n';
  print(0, 0);
  return 0;
}
