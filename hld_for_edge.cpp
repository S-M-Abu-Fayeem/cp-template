#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;
using ll = long long;

const int MAXN = 100005; // Maximum number of nodes
const int LOGN = 20;     // For binary lifting (log2(MAXN))

// --- START: PROBLEM-SPECIFIC ---
// This INF constant is tied to the sentinel value for a max query.
// For a sum query, you'd use 0. For min, use a large positive number.
const ll INF = -1e18; // Sentinel for max query
// --- END: PROBLEM-SPECIFIC ---


// --- Segment Tree (FIXED for Range Set Update, Range Max Query) ---
struct SegTree {
    vector<ll> tree, lazy;
    int n;

    // --- START: PROBLEM-SPECIFIC ---
    // These are the "identity" or "sentinel" values.
    // For "range set, range max":
    //   - tree nodes: -INF (so max() picks a real value)
    //   - lazy nodes: -INF (to mean "no lazy value pending")
    // For "range add, range sum":
    //   - tree nodes: 0
    //   - lazy nodes: 0 (to mean "add 0")
    SegTree(int _n = 0) : n(_n) {
        tree.resize(4 * n, INF); // Initialize with sentinel
        lazy.resize(4 * n, INF); // Use sentinel for "no lazy value"
    }
    // --- END: PROBLEM-SPECIFIC ---

    // Apply lazy value to node v
    void push(int v, int tl, int tr) {
        // --- START: PROBLEM-SPECIFIC ---
        // This is the logic for "pushing" a lazy value.
        
        // 1. Check for the lazy sentinel
        if (lazy[v] == INF) return; 
        
        // 2. Apply lazy value to the current node
        // For "range set":
        tree[v] = lazy[v]; 
        
        // For "range add":
        // tree[v] += lazy[v] * (tr - tl + 1);

        // 3. Propagate lazy value to children
        if (tl != tr) {
            // For "range set":
            lazy[2 * v] = lazy[v];
            lazy[2 * v + 1] = lazy[v];
            
            // For "range add":
            // lazy[2 * v] += lazy[v];
            // lazy[2 * v + 1] += lazy[v];
        }
        
        // 4. Clear the lazy sentinel
        lazy[v] = INF; 
        // --- END: PROBLEM-SPECIFIC ---
    }

    void build(const vector<ll>& a, int v, int tl, int tr) {
        if (tl == tr) {
            tree[v] = a[tl];
        } else {
            int tm = (tl + tr) / 2;
            build(a, 2 * v, tl, tm);
            build(a, 2 * v + 1, tm + 1, tr);
            
            // --- START: PROBLEM-SPECIFIC ---
            // This is the "combine" operation.
            // For "range max":
            tree[v] = max(tree[2 * v], tree[2 * v + 1]);
            
            // For "range sum":
            // tree[v] = tree[2 * v] + tree[2 * v + 1];
            // --- END: PROBLEM-SPECIFIC ---
        }
    }

    // 'new_val' is the value you are updating with
    void update(int v, int tl, int tr, int l, int r, ll new_val) {
        push(v, tl, tr); // Apply pending updates
        if (l > r) return;
        
        if (l == tl && r == tr) {
            // --- START: PROBLEM-SPECIFIC ---
            // This is how you apply an update to a node's lazy value.
            
            // For "range set":
            lazy[v] = new_val;
            
            // For "range add":
            // lazy[v] += new_val;
            // --- END: PROBLEM-SPECIFIC ---
            
            push(v, tl, tr); // Apply it immediately
        } else {
            int tm = (tl + tr) / 2;
            update(2 * v, tl, tm, l, min(r, tm), new_val);
            update(2 * v + 1, tm + 1, tr, max(l, tm + 1), r, new_val);
            
            // --- START: PROBLEM-SPECIFIC ---
            // This must be the same "combine" operation from build()
            tree[v] = max(tree[2 * v], tree[2 * v + 1]);
            // --- END: PROBLEM-SPECIFIC ---
        }
    }

    ll query(int v, int tl, int tr, int l, int r) {
        // --- START: PROBLEM-SPECIFIC ---
        // This is the sentinel value for a query on an invalid range.
        // It must be the identity for your combine operation.
        // For "range max":
        if (l > r) return INF; 
        
        // For "range sum":
        // if (l > r) return 0;
        // --- END: PROBLEM-SPECIFIC ---

        push(v, tl, tr); // Apply pending updates
        
        if (l == tl && r == tr) {
            return tree[v];
        }
        
        int tm = (tl + tr) / 2;
        
        // --- START: PROBLEM-SPECIFIC ---
        // This must be the same "combine" operation from build()
        return max(query(2 * v, tl, tm, l, min(r, tm)),
                   query(2 * v + 1, tm + 1, tr, max(l, tm + 1), r));
        // --- END: PROBLEM-SPECIFIC ---
    }
    
    void build(const vector<ll>& a) { build(a, 1, 1, n); }
    void update(int l, int r, ll new_val) { update(1, 1, n, l, r, new_val); }
    ll query(int l, int r) { return query(1, 1, n, l, r); }
};

// --- Heavy-Light Decomposition ---
vector<pair<int,int>> adj2[MAXN];
int parent[MAXN], depth[MAXN], heavy[MAXN], head[MAXN], pos[MAXN];
int subtree_size[MAXN];
int cur_pos;
ll initial_val2[MAXN]; // Stores edge weight from parent
vector<ll> arr;      
int up[MAXN][LOGN]; 

// --- NEW: Reset function for multiple test cases ---
void reset_all(int n) {
    cur_pos = 0;
    arr.clear();
    arr.resize(n + 1);

    for (int i = 0; i <= n; ++i) {
        adj2[i].clear();
        heavy[i] = -1;
        // --- START: PROBLEM-SPECIFIC ---
        // Must match the query sentinel (INF for max, 0 for sum)
        initial_val2[i] = INF; 
        // --- END: PROBLEM-SPECIFIC ---
        for (int j = 0; j < LOGN; ++j) {
            up[i][j] = 0;
        }
    }
}

// 1. DFS for sizes, depth, parent, AND binary lifting
void dfs_sz(int u, int p, int value) {
    subtree_size[u] = 1;
    parent[u] = p;
    initial_val2[u] = value; // Store incoming edge weight
    depth[u] = (u == p) ? 0 : depth[p] + 1;
    heavy[u] = -1; 
    
    up[u][0] = p;
    for (int i = 1; i < LOGN; ++i) {
        if(up[u][i-1] != 0) 
           up[u][i] = up[up[u][i-1]][i-1];
        else
           up[u][i] = 0; 
    }

    int max_c_size = 0;
    for (auto& edge : adj2[u]) {
        int v = edge.first;
        int w = edge.second;
        if (v == p) continue;
        dfs_sz(v, u, w);
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
    arr[cur_pos] = initial_val2[u]; // Use initial_val2

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

// --- Path Operations (FIXED for Edges) ---

// Helper function to query from node 'u' up to (but not including) 'p'
ll query_chain(SegTree& st, int u, int p) {
    // --- START: PROBLEM-SPECIFIC ---
    // Must match the query sentinel
    ll res = INF; 
    // --- END: PROBLEM-SPECIFIC ---
    
    while (depth[p] < depth[u]) {
        int top = head[u];
        if (depth[top] <= depth[p]) {
            int diff = depth[u] - depth[p];
            top = get_kth_ancestor(u, diff - 1);
        }
        
        // --- START: PROBLEM-SPECIFIC ---
        // Must match the combine operation
        res = max(res, st.query(pos[top], pos[u]));
        // --- END: PROBLEM-SPECIFIC ---
        u = parent[top];
    }
    return res;
}

// New path query function
ll path_query(SegTree& st, int u, int v) {
    int lc = lca(u, v);
    ll res_u = query_chain(st, u, lc);
    ll res_v = query_chain(st, v, lc);
    
    // --- START: PROBLEM-SPECIFIC ---
    // Must match the combine operation
    return max(res_u, res_v);
    // --- END: PROBLEM-SPECIFIC ---
}


void solve() {
    int n;
    cin >> n;

    reset_all(n);

    vector<pair<int,pair<int,int>> > ed;
    
    /*for (int i = 1; i <= n; ++i) {
        ll node_val_unused;
        cin >> node_val_unused;
    }*/
    
    for (int i = 0; i < n - 1; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        adj2[u].push_back({v,w});
        adj2[v].push_back({u,w});
        ed.push_back(make_pair(u,make_pair(v,w)));
    }

    // Initialize HLD
    // --- START: PROBLEM-SPECIFIC ---
    // Root's incoming edge value must be the sentinel
    dfs_sz(1, 1, INF); 
    // --- END: PROBLEM-SPECIFIC ---
    dfs_hld(1, 1);   

    SegTree st(n);
    st.build(arr);

    while(true) {
        string s; cin >> s;
        if(s == "DONE") {
            break;
        }
        if(s == "QUERY") {
            int u, v; cin >> u >> v;
            cout << path_query(st,u,v) << "\n";
        }
        else{ // This is "CHANGE"
            int u_idx, w; cin >> u_idx >> w; u_idx--; // This is edge index
            auto itr = ed[u_idx];
            int u = itr.first;
            int v = itr.second.first;
            
            if(parent[u] == v) {
                swap(u,v);
            }
            // v is the child node
            
            st.update(pos[v], pos[v], w);
        }
    }
}

// --- Main Function ---
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int t; cin >> t;
    while(t--) {
        solve();
        if(t > 0) cout << "\n"; // Optional: for formatting
    }
    
    return 0;
}
