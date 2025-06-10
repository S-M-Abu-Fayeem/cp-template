// rerooting template taken from kartik arora





#include<bits/stdc++.h>
#define ll long long
#define pb push_back
#define fr(a,b) for(int i = a; i < b; i++)
#define rep(i,a,b) for(int i = a; i < b; i++)
#define mod 1000000007
#define inf (1LL<<60)
#define all(x) (x).begin(), (x).end()
#define prDouble(x) cout << fixed << setprecision(10) << x
#define triplet pair<ll,pair<ll,ll>>
#define fast_io ios_base::sync_with_stdio(false);cin.tie(NULL)
using namespace std;
 

ll t,n,m,x,i,j,k,q;
vector<int> tree[200001];
int depth[200001];
int ans[200001];
int d[200001];

void reset() {
    for(int i = 1;  i <= n; i++) {
        tree[i].clear();
        depth[i] = 0;
        ans[i] = 0;
        d[i] = 0;
    }
}
void eval_depth(int src, int par)
{
    int src_depth = 0;
    for(int child : tree[src])
    {
        if(child != par)
        {
            eval_depth(child, src);
            src_depth = max(src_depth, 1 + depth[child]);
        }
    }
    depth[src] = src_depth;
}

void solve(int src, int par, int par_ans)
{
    vector<int> prefixMax, suffixMax;
    // get depth of child nodes.
    for(int child : tree[src])
    {
        if(child != par)
        {
            prefixMax.push_back(depth[child]);
            suffixMax.push_back(depth[child]);
        }
    }
    // build prefix and suffix
    for(int i = 1; i < prefixMax.size(); i++)
        prefixMax[i] = max(prefixMax[i], prefixMax[i-1]);
    for(int i = (int)suffixMax.size() - 2; i >= 0; i--)
        suffixMax[i] = max(suffixMax[i], suffixMax[i+1]);
 
    //eval ans for every child c_no
    int c_no = 0;
    for(int child : tree[src])
    {
        if(child != par)
        {
            int op1 = (c_no == 0) ? INT_MIN : prefixMax[c_no - 1];
            int op2 = (c_no == (int)suffixMax.size() - 1) ? INT_MIN : suffixMax[c_no + 1];
            int partial_ans = 1 + max(par_ans, max(op1,op2));
 
            solve(child, src, partial_ans);
            c_no++;
        }
    }
 
    //eval ans for src
    ans[src] = 1 + max(par_ans, (prefixMax.empty() ? -1 : prefixMax.back()));
}



void dfs(int u, int p){
    d[u] = d[p] + 1;
    for(auto v : tree[u]) {
        if(v!=p) {
            dfs(v,u);
        }
    }
}
int main() {
   fast_io;
   
   //cin >> t;
   cin >> t;
   while(t--)
   {
        int val, val2;
        cin >> n >> val >> val2;
        fr(0,n-1)
        {
            int u,v;
            cin >> u >> v;
            tree[u].push_back(v);
            tree[v].push_back(u);
        }
        eval_depth(1, 0);
        solve(1, 0, -1);
        ll best = 0;
        dfs(1,0);
        fr(1,n+1){
            best = max(best, 1ll*(ans[i]) * val - 1ll*(d[i] - 1) * (val2) );
        }
        cout << best << "\n";
        reset();
   }
   return 0;
}
