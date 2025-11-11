#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;
using ll = long long;

const int MAXN = 250007; // Maximum number of nodes
const int LOGN = 20;     // For binary lifting (log2(MAXN))
const ll INF = -1e18;    // Sentinel for root's "parent edge"

// --- Struct for Segment Tree Node ---
struct node {
    int mx;
    int mi;
    ll tot;
    int xr;
    int len;

    // Default constructor for initialization
    node(int _mx = -1, int _mi = 100000000, ll _tot = 0, int _xr = 0, int _len = 0)
        : mx(_mx), mi(_mi), tot(_tot), xr(_xr), len(_len) {}
};

// --- Identity Node for Merging ---
// This is the value that doesn't change the result of a merge
const node IDENTITY = node{-1, 100000000, 0, 0, 0};

// --- Global Merge Function (FIXED) ---
// This is the "combine" operation
node merge(const node& a, const node& b) {
    return node(
        max(a.mx, b.mx),
        min(a.mi, b.mi),
        a.tot + b.tot,
        a.xr ^ b.xr,
        a.len + b.len // <-- This was the missing 5th argument
    );
}

// --- Segment Tree (Simplified for Point Update, Range Query) ---
struct SegTree {
    vector<node> tree;
    int n;

    SegTree(int _n = 0) : n(_n) {
        // Initialize with the IDENTITY node, not INF
        tree.resize(4 * n, IDENTITY);
    }

    void build(const vector<ll>& a, int v, int tl, int tr) {
        if (tl == tr) {
            // Cast to int where needed by the node struct
            tree[v] = node{(int)a[tl], (int)a[tl], a[tl], (int)a[tl], 1};
        } else {
            int tm = (tl + tr) / 2;
            build(a, 2 * v, tl, tm);
            build(a, 2 * v + 1, tm + 1, tr);
            tree[v] = merge(tree[2 * v], tree[2 * v + 1]);
        }
    }

    // Point update function
    void update_point(int v, int tl, int tr, int pos, ll new_val) {
        if (tl == tr) {
            // This is the leaf
            tree[v] = node{(int)new_val, (int)new_val, new_val, (int)new_val, 1};
        } else {
            int tm = (tl + tr) / 2;
            if (pos <= tm)
                update_point(2 * v, tl, tm, pos, new_val);
            else
                update_point(2 * v + 1, tm + 1, tr, pos, new_val);
            
            // Re-combine children
            tree[v] = merge(tree[2 * v], tree[2 * v + 1]);
        }
    }

    node query(int v, int tl, int tr, int l, int r) {
        if (l > r) {
            // Return the identity node for an invalid range
            return IDENTITY;
        }
        
        // No push() needed

        if (l == tl && r == tr) {
            return tree[v];
        }
        
        int tm = (tl + tr) / 2;
        
        return merge(query(2 * v, tl, tm, l, min(r, tm)),
                     query(2 * v + 1, tm + 1, tr, max(l, tm + 1), r));
    }
    
    // --- Public Wrappers ---
    void build(const vector<ll>& a) { build(a, 1, 1, n); }
    // Wrapper for point update
    void update(int pos, ll new_val) { update_point(1, 1, n, pos, new_val); }
    // Wrapper for range query (returns a node, not ll)
    node query(int l, int r) { return query(1, 1, n, l, r); }
};

// --- Heavy-Light Decomposition ---
vector<pair<int, int>> adj2[MAXN];
int parent[MAXN], depth[MAXN], heavy[MAXN], head[MAXN], pos[MAXN];
int subtree_size[MAXN];
int cur_pos;
ll initial_val2[MAXN]; // Stores node values
vector<ll> arr;      
int up[MAXN][LOGN];  

// --- NEW: Prefix XOR array and processing ---
int pre[MAXN];

void pre_processsing() {
    pre[0] = 0;
    for (int i = 1; i < MAXN; i++) {
        // Correct prefix XOR calculation
        pre[i] = pre[i - 1] ^ i;
    }
}

// --- Reset function for multiple test cases ---
void reset_all(int n) {
    cur_pos = 0;
    arr.clear();
    arr.resize(n + 1);

    for (int i = 0; i <= n; ++i) {
        adj2[i].clear();
        heavy[i] = -1;
        // This gets overwritten by cin, but good to reset
        initial_val2[i] = 0; 
        for (int j = 0; j < LOGN; ++j) {
            up[i][j] = 0;
        }
    }
}

// 1. DFS for sizes, depth, parent, AND binary lifting
void dfs_sz(int u, int p) {
    subtree_size[u] = 1;
    parent[u] = p;
    depth[u] = (u == p) ? 0 : depth[p] + 1;
    heavy[u] = -1;  
    
    up[u][0] = p;
    for (int i = 1; i < LOGN; ++i) {
        if (up[u][i - 1] != 0) 
            up[u][i] = up[up[u][i - 1]][i - 1];
        else
            up[u][i] = 0; 
    }

    int max_c_size = 0;
    for (auto& edge : adj2[u]) {
        int v = edge.first;
        if (v == p) continue;
        dfs_sz(v, u);
        subtree_size[u] += subtree_size[v];
        if (subtree_size[v] > max_c_size) {
            max_c_size = subtree_size[v];
            heavy[u] = v;
        }
    }
}

// 2. DFS for HLD chains
void dfs_hld(int u, int h) {
    head[u] = h;
    pos[u] = ++cur_pos;
    arr[cur_pos] = initial_val2[u]; // Use node's value

    if (heavy[u] != -1) {
        dfs_hld(heavy[u], h);
    }
    
    for (auto& edge : adj2[u]) {
        int v = edge.first;
        if (v != parent[u] && v != heavy[u]) {
            dfs_hld(v, v);
        }
    }
}

// --- Binary Lifting LCA Functions ---
int get_kth_ancestor(int u, int k) {
    for (int i = 0; i < LOGN; ++i) {
        if (k & (1 << i)) {
            u = up[u][i];
        }
    }
    return u;
}

int lca(int u, int v) {
    if (depth[u] < depth[v]) swap(u, v);
    
    u = get_kth_ancestor(u, depth[u] - depth[v]);
    
    if (u == v) return u;
    
    for (int i = LOGN - 1; i >= 0; --i) {
        if (up[u][i] != up[v][i]) {
            u = up[u][i];
            v = up[v][i];
        }
    }
    return up[u][0];
}

// --- Path Operations ---

// Helper function to query from node 'u' up to (but not including) 'p'
node query_chain(SegTree& st, int u, int p) {
    // Start with the identity
    node res = IDENTITY; 
    
    while (depth[p] < depth[u]) {
        int top = head[u];
        if (depth[top] <= depth[p]) {
            int diff = depth[u] - depth[p];
            top = get_kth_ancestor(u, diff - 1);
        }
        
        // Query the segment tree for the chain
        res = merge(res, st.query(pos[top], pos[u]));
        u = parent[top];
    }
    return res;
}

// New path query function (FIXED)
bool path_query(SegTree& st, int u, int v) {
    int lc = lca(u, v);
    
    // Query path u -> lca
    node res_u = query_chain(st, u, lc);
    // Query path v -> lca
    node res_v = query_chain(st, v, lc);
    
    // Combine path results
    node itr = merge(res_u, res_v);
    
    // --- Manually add the LCA's value ---
    // (We use initial_val2[] because it's guaranteed to be up-to-date)
    itr.mx = max(itr.mx, (int)initial_val2[lc]);
    itr.mi = min(itr.mi, (int)initial_val2[lc]); // <-- Fixed missing semicolon
    itr.tot += initial_val2[lc];
    itr.xr ^= (int)initial_val2[lc];
    itr.len++;
    // ---
    
    ll tot = 1LL * (itr.len) * (itr.len + 1) / 2;
    int xxxr = pre[itr.len];
    
    // --- Check for permutation ---
    // (Fixed logic: == instead of ^)
    if (xxxr == itr.xr && tot == itr.tot && itr.mi == 1 && itr.mx == itr.len) {
        return true;
    }
    
    return false;
}


void solve() {
    int n, q;
    cin >> n >> q;

    reset_all(n);

    for (int i = 1; i <= n; ++i) {
        cin >> initial_val2[i]; // Read node values
    }
    
    for (int i = 0; i < n - 1; ++i) {
        int u, v;
        cin >> u >> v;
        // Edge weight is irrelevant for node-based HLD
        adj2[u].push_back({v, 0});
        adj2[v].push_back({u, 0});
    }

    // Initialize HLD
    dfs_sz(1, 1); // Pass 1 as its own parent (root)
    dfs_hld(1, 1);   

    SegTree st(n);
    st.build(arr); // Build segtree from HLD-flattened array

    for (int i = 0; i < q; i++) {
        int type; cin >> type;
        if (type == 1) {
            int u, v; cin >> u >> v;
            if (path_query(st, u, v)) {
                cout << "Yes\n";
            } else {
                cout << "No\n";
            }
        } else {
            int u, w; cin >> u >> w;
            // This is a point update
            st.update(pos[u], w); 
            
            // --- CRITICAL FIX ---
            // Update the array used for manual LCA checks
            initial_val2[u] = w;
        }
    }
}

// --- Main Function ---
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // --- CRITICAL FIX ---
    // Call the pre-processing function once
    pre_processsing();

    int t; cin >> t;
    while (t--) {
        solve();
        // The problem this is from (CSES) doesn't need this, 
        // but it's fine.
        // if(t > 0) cout << "\n"; 
    }
    
    return 0;
}
