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
#define INF 1000000000
int N = 2e5 + 7, M;
const int NN = 7;
vector< vector<pair<int,int> > >adj(NN);

int dist[NN][2]; // change the sates a - > b , a - > c

struct State{
	int node ,cur_available_amount, cost;
	State() {}
    State(int a, int b, int _c) { node = a; cur_available_amount = b; cost = _c; }
    bool operator>(const State &s) const { return cost > s.cost; }
} ;


int dijkstra() {
	for(int i = 0 ; i < N; i++) {
		for(int j = 0 ; j < 2 ; j++) {// no of cur_available_amount
			dist[i][j] = INF;
		}
	}

	priority_queue< State,vector<State>,greater<State> > pq;

	pq.push(State(0,0,0));// start point
	dist[0][0] = 0;

	while(!pq.empty()) {

		State s = pq.top(); pq.pop();
		debug(s.node)
		debug(s.cur_available_amount);
		debug(dist[s.node][s.cur_available_amount]);
		if(dist[s.node][s.cur_available_amount] < s.cost) {
			continue;
		}


		for(auto next_node : adj[s.node]) {
			//iterate through all state

			int v = next_node.first;
			//cout << v << endl;
			int required_amount = next_node.second;

			if(dist[v][s.cur_available_amount^1] > dist[s.node][s.cur_available_amount] + required_amount) {

				dist[v][s.cur_available_amount^1] = dist[s.node][s.cur_available_amount] + required_amount;

				//change state here
				pq.push(State(v, s.cur_available_amount ^ 1 , dist[v][s.cur_available_amount^1]));
			}
		}
	}

	// return the required answer from dist;
	
	debug(dist[N - 1][0])
	return dist[N-1][0];
}


void solve() {
	cin >> N >> M;

	for(int i = 0 ; i < M ; i++) {
		int u, v, w; 
		cin >> u >> v >> w;
		u--; v--;
		
		adj[u].push_back({v,w});
		adj[v].push_back({u,w});
	}
	debug(adj)

	int res = dijkstra();

	cout << (res == 1e9 ? -1 : res);
}




int32_t main() {
	//fast()
#ifndef ONLINE_JUDGE
	freopen("error.txt","w",stderr);
#endif


	int t=1;
	for(int ca =  1; ca <= t ; ca++) {
		//cout << "Case #"<<ca<<": ";
		//debug(ca);
		//cout << ca << endl;
		solve();
		//cout << "\n"; 
	}
}
