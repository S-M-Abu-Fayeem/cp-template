#include<bits/stdc++.h>
using namespace std;

const int N = 1e3 + 7;
string s;

int rec(int l, int r, vector<vector<int> > &dp) {
	if(l == r) return 1;
	if(l > r) return 0;

	if(dp[l][r] != -1) return dp[l][r];
	int ans = 0;
	if(s[l] == s[r]) {
		//cout << l << " " << r << endl;
		ans = max(ans, 2 + rec( l + 1, r - 1, dp));
	}
	else{
		ans = max( rec(l + 1, r ,dp ) , rec(l, r - 1, dp));
	}
	return dp[l][r] = ans ;
}
int32_t main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	int t; cin >> t;
	cin.ignore();
	while(t--) {
		getline(cin, s);
		int n = s.size();
		vector< vector<int> > dp(n, vector<int> (n, -1));
		cout << rec(0, n - 1, dp) << "\n";
	}
}
