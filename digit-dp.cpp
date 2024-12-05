#include<bits/stdc++.h>
#define int long long int
using namespace std;
vector<int> digits = {0,1,2,3,4,5,6,7,8,9};\
int dp[21][4][2];
int rec(string &s,int slot, int left , bool tight) {
    if(slot == 0) return 1;
    if(dp[slot][left][tight]!= -1) return dp[slot][left][tight];
    int ub = tight ? s[(int) s.size() - slot] - '0' : 9;
    int ans = 0 ;
    if(left > 0) {
        for(int i = 1 ; i <= ub ; i++) {
            ans+= rec(s, slot - 1, left - 1 , (i == ub) & tight );
        }
    }
    ans+=rec(s, slot - 1, left , (0 == ub) & tight);
    return dp[slot][left][tight]=ans ;
}
int32_t main(){
    int t ; cin >> t;
    while(t--){
        int l , r ; cin >> l >> r;
        l--;
        string s1 = to_string(l);
        string s2 = to_string(r);
        memset(dp, -1 ,sizeof dp);
        int ans1 = rec(s1, (int)s1.size(), 3, 1);
        memset(dp, -1 ,sizeof dp);
        int ans2 = rec(s2, (int)s2.size(), 3 , 1);
       // cout << s1 << " " << ans1 << endl << s2 << " " << ans2 << endl;
        cout << ans2 - ans1 << "\n";
    }
}

