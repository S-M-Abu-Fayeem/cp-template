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
 
typedef long long ll;
typedef unsigned long long ull;
typedef long double lld;
 
//debug
#ifndef ONLINE_JUDGE
#define debug(x) cerr << #x <<" "; _print(x); cerr << endl;
#else
#define debug(x)
#endif
 
void _print(ll t) {cerr << t;}
void _print(int t) {cerr << t;}
void _print(string t) {cerr << t;}
void _print(char t) {cerr << t;}
void _print(lld t) {cerr << t;}
void _print(double t) {cerr << t;}
void _print(ull t) {cerr << t;}
 
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
const ll INF = 1e18;
int n;
const int N =  1e3+7;
vector<vector<ll> >  capacity(N, vector<ll>(N,0));
vector<vector<ll> >  capacity2(N, vector<ll>(N,0));
vector<vector<int> >  is_edge(N, vector<int>(N,0));
vector<vector<int> > adj(N);
 
ll bfs(int s, int t, vector<int> &parent) {
	fill(parent.begin(),parent.end(),-1);
	parent[s] = -2;
	queue< pair<int,ll> > q;
	q.push({s,INF});
	while(!q.empty()) {
		int cur = q.front().first;
		ll flow = q.front().second;
		q.pop();
 
		for(int next : adj[cur]) {
			if( parent[next] == -1 && capacity[cur][next]) {
				parent[next] = cur;
				ll new_flow = min(flow,capacity[cur][next]);
				if(next == t) {
					return new_flow;
				}
				q.push({next,new_flow});
			}
		}
 	}
 	return 0;
}
 
 
ll maxflow(int s, int t) {
	ll flow = 0 ;
	vector<int> parent(n);
	ll new_flow = 0;
	//int yo = 10;
	while(new_flow = bfs(s,t,parent)) {
		debug(new_flow)
		flow+=new_flow;
		int cur = t;
		while(cur!=s) {
			debug(cur);
			int prev = parent[cur];
			capacity[prev][cur] -= new_flow;
			capacity[cur][prev] += new_flow;
			cur = prev;
		}
		debug(" ");
		//cout << cur << " " << s << endl;
		//debug(parent);
		//debug(capacity);
		//break;
 
	}
	return flow;
}
 
void find_min_cut_vertices(int s) {
	vector<int> parent(n);
	fill(parent.begin(),parent.end(),-1);
	parent[s] = -2;
	queue<int> q; q.push(s);
	vector<int> s_c; // contains the vertices that are reachable from source
	while(!q.empty()) {
		int cur = q.front();
		s_c.push_back(cur);
		debug(cur);
		q.pop();
		for(int next: adj[cur]) {
			debug(next);
			debug(capacity[cur][next]);
			if(parent[next]==-1 && capacity[cur][next] > 0) {
				parent[next] = cur;
				q.push(next);
			}
		}
	}
	debug(s_c);
	vector<int> t_c;
	for(int i = 1 ; i <= n; i++) {
		if(find(s_c.begin(),s_c.end(),i) == s_c.end()) {
			t_c.push_back(i);
		}
	}
	debug(t_c);
	debug(s_c);
	for(auto x : s_c) {
		for(auto y : t_c) {
 
			if(is_edge[x][y]==1){
				cout << x << " " << y << "\n";
			}
		}
	}
 
}
 
void solve() {
	int m; cin >>n >> m;n++;
	while(m--) {
		int u, v; ll w; cin >> u >> v;;
		capacity[u][v] += 1;
		capacity[v][u]+= 1;
		capacity2[u][v] += 1;
		capacity2[v][u]+= 1;
		adj[u].push_back(v);
		adj[v].push_back(u);
		is_edge[u][v] = 1;
		is_edge[v][u] = 1;
	}
	//debug(adj);
	cout << maxflow(1,n-1) << "\n";
	debug(capacity);
	find_min_cut_vertices(1);
}
 
 
 
 
int32_t main() {
	//fast()
#ifndef ONLINE_JUDGE
	freopen("error.txt","w",stderr);
#endif
 
 
	int t; t = 1;
	for(int ca =  1; ca <= t ; ca++) {
		//cout << "Case #"<<ca<<": ";
		//debug(ca);
		//cout << ca << endl;
		solve();
		//cout << "\n"; 
	}
}
