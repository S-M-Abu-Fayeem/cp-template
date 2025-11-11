#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
using ll = long long;

const int MAXN = 200005; // Maximum number of nodes

// --- Segment Tree with Lazy Propagation ---
// This struct supports:
// 1. add(l, r, val): Add 'val' to all elements in range [l, r]
// 2. query(l, r): Get the sum of elements in range [l, r]
struct SegTree {
    vector<ll> tree, lazy;
    int n;

    SegTree(int _n = 0) : n(_n) {
        tree.resize(4 * n);
        lazy.resize(4 * n, 0);
    }

    void push(int v, int tl, int tr) {
        if (lazy[v] == 0) return;
        tree[v] = lazy[v] * (tr - tl + 1);
        if (tl != tr) {
            //update this;
            lazy[2 * v] = lazy[v];
            lazy[2 * v + 1] = lazy[v];
        }
        lazy[v] = 0;
    }

    void build(const vector<ll>& a, int v, int tl, int tr) {
        if (tl == tr) {
            tree[v] = a[tl];
        } else {
            int tm = (tl + tr) / 2;
            build(a, 2 * v, tl, tm);
            build(a, 2 * v + 1, tm + 1, tr);
            tree[v] = tree[2 * v] + tree[2 * v + 1];
        }
    }

    void update(int v, int tl, int tr, int l, int r, ll add) {
        push(v, tl, tr);
        if (l > r) return;
        if (l == tl && r == tr) {
            lazy[v] = add; //modify this
            push(v, tl, tr);
        } else {
            int tm = (tl + tr) / 2;
            update(2 * v, tl, tm, l, min(r, tm), add);
            update(2 * v + 1, tm + 1, tr, max(l, tm + 1), r, add);
            tree[v] = tree[2 * v] + tree[2 * v + 1];
        }
    }

    ll query(int v, int tl, int tr, int l, int r) {
        if (l > r) return 0;
        push(v, tl, tr);
        if (l == tl && r == tr) {
            return tree[v];
        }
        int tm = (tl + tr) / 2;
        return query(2 * v, tl, tm, l, min(r, tm)) +
               query(2 * v + 1, tm + 1, tr, max(l, tm + 1), r);
    }
    
    // Public helper functions
    void build(const vector<ll>& a) { build(a, 1, 1, n); }
    void update(int l, int r, ll add) { update(1, 1, n, l, r, add); }
    ll query(int l, int r) { return query(1, 1, n, l, r); }
};

// --- Heavy-Light Decomposition ---
vector<int> adj[MAXN];
int parent[MAXN], depth[MAXN], heavy[MAXN], head[MAXN], pos[MAXN];
int subtree_size[MAXN];
int cur_pos; // Current position in the flattened array
ll initial_val[MAXN]; // Initial values of the nodes
vector<ll> arr;      // Flattened array for segment tree

// 1. DFS to calculate subtree sizes, depths, and parents
void dfs_sz(int u, int p) {
    subtree_size[u] = 1;
    parent[u] = p;
    depth[u] = (u == p) ? 0 : depth[p] + 1;
    heavy[u] = -1; // -1 indicates no heavy child
    int max_c_size = 0;

    for (int v : adj[u]) {
        if (v == p) continue;
        dfs_sz(v, u);
        subtree_size[u] += subtree_size[v];
        if (subtree_size[v] > max_c_size) {
            max_c_size = subtree_size[v];
            heavy[u] = v;
        }
    }
}

// 2. DFS to build the heavy chains and assign positions
void dfs_hld(int u, int h) {
    head[u] = h; // 'h' is the head of the current chain
    pos[u] = ++cur_pos;
    arr[cur_pos] = initial_val[u]; // Set value in flattened array

    // Process the heavy child first to keep the chain contiguous
    if (heavy[u] != -1) {
        dfs_hld(heavy[u], h);
    }

    // Process light children, each starting a new chain
    for (int v : adj[u]) {
        if (v != parent[u] && v != heavy[u]) {
            dfs_hld(v, v); // 'v' becomes the head of its new chain
        }
    }
}

// --- Path Operations ---

// Helper function to apply updates on the HLD path
void process_path_update(SegTree& st, int u, int v, ll val) {
    while (head[u] != head[v]) {
        // Ensure 'u' is on the deeper chain
        if (depth[head[u]] < depth[head[v]]) {
            swap(u, v);
        }
        // Update the segment from the head of u's chain to u
        st.update(pos[head[u]], pos[u], val);
        // Move u to the parent of its chain's head
        u = parent[head[u]];
    }

    // Now u and v are on the same chain
    if (depth[u] > depth[v]) {
        swap(u, v);
    }
    // Update the final segment [pos[u], pos[v]]
    // (Note: This includes the LCA. If you want to exclude it,
    //  you might use pos[u]+1 if it's for edge values)
    st.update(pos[u], pos[v], val);
}

// Helper function to apply queries on the HLD path
ll process_path_query(SegTree& st, int u, int v) {
    ll res = 0;
    while (head[u] != head[v]) {
        if (depth[head[u]] < depth[head[v]]) {
            swap(u, v);
        }
        res += st.query(pos[head[u]], pos[u]);
        u = parent[head[u]];
    }

    if (depth[u] > depth[v]) {
        swap(u, v);
    }
    // Query the final segment [pos[u], pos[v]]
    res += st.query(pos[u], pos[v]);
    return res;
}

// --- Main Function ---
int main() {
    // Example Usage
    int n, q;
    cin >> n >> q;

    // Read initial node values
    for (int i = 1; i <= n; ++i) {
        cin >> initial_val[i];
    }

    // Read edges
    for (int i = 0; i < n - 1; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // Initialize HLD
    cur_pos = 0;
    arr.resize(n + 1); // 1-based indexing for pos
    dfs_sz(1, 1);    // Root the tree at 1
    dfs_hld(1, 1);   // Start HLD from root 1, which is its own head

    // Initialize Segment Tree
    SegTree st(n);
    st.build(arr);

    // Process queries
    for (int i = 0; i < q; ++i) {
        int type;
        cin >> type;
        if (type == 1) {
            // Type 1: Update path (u, v) by adding 'val'
            int u, v;
            ll val;
            cin >> u >> val;
            //range update
            process_path_update(st, u, u, val);
        } else {
            // Type 2: Query path sum (u, v)
            int u, v;
            cin >> u ;
            cout << process_path_query(st, 1, u) << endl;
        }
    }

    return 0;
}
