/*
You are given a rooted tree consisting of n nodes. The nodes are numbered 1,2,\ldots,n, and node 1 is the root. Each node has a value.
Your task is to process following types of queries:

change the value of node s to x
calculate the sum of values in the subtree of node s

Input
The first input line contains two integers n and q: the number of nodes and queries. The nodes are numbered 1,2,\ldots,n.
The next line has n integers v_1,v_2,\ldots,v_n: the value of each node.
Then there are n-1 lines describing the edges. Each line contans two integers a and b: there is an edge between nodes a and b.
Finally, there are q lines describing the queries. Each query is either of the form "1 s x" or "2 s".
Output
Print the answer to each query of type 2.
Constraints

1 \le n, q \le 2 \cdot 10^5
1 \le a,b, s \le n
1 \le v_i, x \le 10^9

Example
Input:
5 3
4 2 5 2 1
1 2
1 3
3 4
3 5
2 3
1 5 3
2 3

Output:
8
10
*/

#include<bits/stdc++.h>
 
using namespace std;
 
#define p_vec(a) for(int i = 0 ; i < a.size(); i++) { cout <<a[i] << " ";}  cout << endl;
#define sort(a) sort(a.begin(),a.end());
#define rsort(a) sort(a.rbegin(),a.end());
#define rep(tt,st,n) for(int tt = st ; tt < n; tt++)
#define per(tt,st,n) for(int tt = st; tt >= n; tt--)
#define lb(a,x) lower_bound(a.begin(),a.end(),x);
#define ub(a,x) upper_bound(a.begin(),a.end(),x);
#define all(a) a.begin(),a.end();
#define max_ele(a) *max_element(a.begin(),a.end());
#define min_ele(a) *min_element(a.begin(),a.end());
//data structure
#define vi vector<int> 
#define vvi vector<vi>
#define vll vector<ll>
#define vvll vector<vll>
#define pi pair<int,int>
#define pll pair<ll,ll>
#define vpi vector<pi>
#define vvpi vector<vpi>
#define ff first
#define ss second
#define pb(a) push_back(a);
#define int long long int
typedef long long ll;
typedef unsigned long long ull;
typedef long double lld;
 
//debug
#ifndef ONLINE_JUDGE
#define debug(x) cerr << #x <<" "; _print(x); cerr << endl;
#else
#define debug(x)
#endif
 
 
 
template <class T, class V> void _print(pair <T, V> p);
template <class T> void _print(vector <T> v);
template <class T> void _print(set <T> v);
template <class T, class V> void _print(map <T, V> v);
template <class T> void _print(multiset <T> v);
template <class T, class V> void _print(pair <T, V> p) {cerr << "{"; _print(p.ff); cerr << ","; _print(p.ss); cerr << "}";}
template <class T> void _print(vector <T> v) {cerr << "[ "; for (T i : v) {_print(i); cerr << " ";} cerr << "]";}
template <class T> void _print(set <T> v) {cerr << "[ "; for (T i : v) {_print(i); cerr << " ";} cerr << "]";}
template <class T> void _print(multiset <T> v) {cerr << "[ "; for (T i : v) {_print(i); cerr << " ";} cerr << "]";}
template <class T, class V> void _print(map <T, V> v) {cerr << "[ "; for (auto i : v) {_print(i); cerr << " ";} cerr << "]";}
 
 
 
//bit_manupulation
#define checkbit(x,n) (x&(1LL<<n))
#define setbit(x,n) (x=(x|(1LL<<n)))
#define resetbit(x,n) (x=(x&(~(1LL<<n))))
#define pow2(i) (1LL<<i)
 
#define fast() ios_base::sync_with_stdio(false); cin.tie(NULL);
 
template<typename T> void read(T&a){for(auto&e:a)cin>>e;}
template<typename T> void write(T a){for(auto e:a)cout<<e<<" ";cout<<endl;}
 
// ------------------------------------------------------------//
int n, q;
const int N = 4e5 + 9;
 
int timer = 0 ;
vi in_time(N,0);
vi out_time(N,0);
vvi g(N);
 
vi flat_tree(2*N);
 
void dfs(int u, int p) {
	timer++;
	in_time[u] = timer;
	flat_tree[timer] = u;
	//debug(timer)
 
	for(auto v : g[u]) {
		if(v != p) {
			dfs(v,u);
		}
	}
 
 
	timer++;
	out_time[u] = timer;
	flat_tree[timer] = u;
}
 
int a[N];
struct ST {
  int t[4 * N];
  static const int inf = 1e9;
  ST() {
    memset(t, 0, sizeof t);
  }
  void build(int n, int b, int e) {
    if (b == e) {
      t[n] = a[b];
      return;
    }
    int mid = (b + e) >> 1, l = n << 1, r = l | 1;
    build(l, b, mid);
    build(r, mid + 1, e);
    t[n] = (t[l] +t[r]); // change this
  }
  void upd(int n, int b, int e, int i, int x) {
    if (b > i || e < i) return;
    if (b == e && b == i) {
      t[n] = x; // update
      return;
    }
    int mid = (b + e) >> 1, l = n << 1, r = l | 1;
    upd(l, b, mid, i, x);
    upd(r, mid + 1, e, i, x);
    t[n] = (t[l] + t[r]); // change this
  }
  int query(int n, int b, int e, int i, int j) {
    if (b > j || e < i) return 0; // return appropriate value
    if (b >= i && e <= j) return t[n];
    int mid = (b + e) >> 1, l = n << 1, r = l | 1;
    return (query(l, b, mid, i, j) + query(r, mid + 1, e, i, j)); // change this
  }
}t;
 
vi tt(N);
 
void solve() {
	cin >> n >> q;
	rep(i,1,n + 1){
		cin >> tt[i];
	}
	rep(i,0,n-1) {
		int u, v;
		cin >>u >> v ;
		g[u].pb(v);
		g[v].pb(u);
		
		
	}
	
	dfs(1,-1);
	rep(i,1,n+1) {
		int u = i;
		a[in_time[u]] = tt[u];
		a[out_time[u]] = tt[u];
	}
	t.build(1,1,2*n);
	//rep(i,1,2*n + 1) {
	//	cout << a[i] << " " ;
	//}
	n = 2 * n;
	while(q--) {
		int ty; cin >> ty;
		if(ty == 1) {
			int u, x; cin >> u >> x;
			t.upd(1,1,n,in_time[u],x);
			t.upd(1,1,n,out_time[u],x);
		}
		else{
			int u; cin >> u;
			//debug(in_time[u])
			//debug(out_time[u])
			cout << t.query(1,1,n,in_time[u], out_time[u]) / 2 << endl;
		}
	}
}
 
 
 
 
int32_t main() {
	fast()
#ifndef ONLINE_JUDGE
	freopen("error.txt","w",stderr);
#endif
 
 
	int t = 1;
	for(int ca =  1; ca <= t ; ca++) {
		//cout << "Case #"<<ca<<": ";
		//debug(ca);
		//cout << ca << endl;
		solve();
		//cout << "\n"; 
	}
}
