#include<bits/stdc++.h>
#define int long long int
using namespace std;

const int N = 3e3 + 7;
int s(int , int);

int a[N];
int dp[N][N], dp2[N][N];

bool vis[N][N], vis2[N][N];

int f(int l, int r) {
	if( l > r) return 0;

	if(vis[l][r]) return dp[l][r];

	int left = a[l] + s(l + 1, r);
	int right = a[r] + s(l , r - 1);

	vis[l][r] = true;

	return dp[l][r] = max( left , right);
}

int s(int l, int r) {
	if(l > r) return 0;

	if(vis2[l][r]) return dp2[l][r];

	int left = -a[l] + f(l + 1, r);
	int right = -a[r] + f(l, r - 1);

	vis2[l][r] = true;
	return dp2[l][r] = min(left, right);
}

int32_t main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);

	int n; cin >> n; for(int i = 0 ; i < n; i++) cin >> a[i];
	cout << f(0, n - 1) <<endl;
}
