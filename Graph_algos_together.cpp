/*
 ============================================================================
   ICPC GRAPH ALGORITHMS TEMPLATE  (single file, C++17)
 ============================================================================
   HOW TO USE
   - Every algorithm is self-contained: copy only what you need.
   - Nodes are 0-indexed (0..n-1). If your problem is 1-indexed, just create
     the structure with size n+1 and use nodes 1..n.
   - main() at the bottom shows a small working example of every algorithm.
   - Compile:  g++ -std=c++17 -O2 graph_templates.cpp -o g

   TABLE OF CONTENTS                                   COMPLEXITY
   ---------------------------------------------------------------------------
    1. BFS (shortest path, unweighted) + path        O(V + E)
    2. Grid BFS (4 directions)                        O(R * C)
    3. DFS (iterative, no stack overflow)             O(V + E)
    4. 0-1 BFS (edge weights 0 or 1)                  O(V + E)
    5. DSU / Union-Find                               ~O(1) per op
    6. Topological Sort (Kahn) + cycle detection      O(V + E)
    7. Bipartite check (2-coloring)                   O(V + E)
    8. Dijkstra (non-negative weights) + path         O(E log V)
    9. Bellman-Ford (negative weights / cycles)       O(V * E)
   10. Floyd-Warshall (all pairs)                     O(V^3)
   11. Kruskal MST                                    O(E log E)
   12. Prim MST                                       O(E log V)
   13. LCA (binary lifting) + k-th ancestor + dist    O(N log N) build, O(log N) query
   14. Bridges & Articulation Points                  O(V + E)
   15. SCC (Kosaraju) + Condensation DAG              O(V + E)
   16. 2-SAT                                          O(V + E)
   17. Max Flow (Dinic) + Min Cut                     O(V^2 E), very fast in practice
   18. Min Cost Max Flow (SPFA)                       O(F * V * E)
   19. Bipartite Matching (Hopcroft-Karp)             O(E sqrt V)
   20. Hungarian (min-cost assignment)                O(n^2 m)
   21. Euler Path / Circuit (Hierholzer)              O(V + E)
   22. Tree Diameter                                  O(N)
   23. Heavy-Light Decomposition (path queries)       O(log^2 N) per query
   24. Centroid Decomposition (skeleton)              O(N log N)
 ============================================================================
*/
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = 1e18;   // "infinity" for distances (safe to add small weights)


// ============================================================================
// 1. BFS — shortest path in an UNWEIGHTED graph
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Finds the minimum number of edges from one node to all others.
// COMMON PROBLEM: Shortest number of moves/hops, e.g. 'minimum flights from city A to B',
//                 'fewest knight moves to a square', multi-source 'distance to nearest fire'.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// dist[v] = number of edges from src to v (-1 if unreachable).
// par[v]  = previous node on a shortest path (used to rebuild the path).
// Multi-source BFS: push ALL sources with dist 0 at the start.
// ============================================================================
struct BFS {
    vector<int> dist, par;
    void run(int src, const vector<vector<int>>& adj) {
        int n = adj.size();
        dist.assign(n, -1); par.assign(n, -1);
        queue<int> q;
        dist[src] = 0; q.push(src);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u])
                if (dist[v] == -1) {          // not visited yet
                    dist[v] = dist[u] + 1;
                    par[v] = u;
                    q.push(v);
                }
        }
    }
    // path from src to t (empty if unreachable)
    vector<int> path(int t) {
        vector<int> p;
        if (dist[t] == -1) return p;
        for (int v = t; v != -1; v = par[v]) p.push_back(v);
        reverse(p.begin(), p.end());
        return p;
    }
};


// ============================================================================
// 2. GRID BFS — shortest path on a 2D grid ('#' = wall, '.' = free)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : BFS where cells of a 2D map are the nodes and moves are the edges.
// COMMON PROBLEM: Maze: 'shortest path from S to E avoiding #',
//                 'count rooms' (flood fill), 'monsters + player escape' (multi-source BFS).
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// For 8 directions use dx = {-1,-1,-1,0,0,1,1,1}, dy = {-1,0,1,-1,1,-1,0,1}.
// ============================================================================
const int dx[4] = {-1, 1, 0, 0};
const int dy[4] = {0, 0, -1, 1};

vector<vector<int>> gridBFS(const vector<string>& grid, int sr, int sc) {
    int R = grid.size(), C = grid[0].size();
    vector<vector<int>> d(R, vector<int>(C, -1));
    queue<pair<int,int>> q;
    d[sr][sc] = 0; q.push({sr, sc});
    while (!q.empty()) {
        auto [r, c] = q.front(); q.pop();
        for (int k = 0; k < 4; k++) {
            int nr = r + dx[k], nc = c + dy[k];
            if (nr < 0 || nr >= R || nc < 0 || nc >= C) continue; // outside
            if (grid[nr][nc] == '#' || d[nr][nc] != -1) continue;  // wall / seen
            d[nr][nc] = d[r][c] + 1;
            q.push({nr, nc});
        }
    }
    return d;
}


// ============================================================================
// 3. DFS — iterative (recursive DFS can overflow the stack for n ~ 1e6)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Visits everything reachable from a node; groups nodes into components.
// COMMON PROBLEM: 'How many connected islands / friend groups?', 'is v reachable from u?',
//                 'minimum roads to add so all cities connect' = components - 1.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Returns nodes in the order they were visited. Also counts components.
// ============================================================================
vector<int> dfsOrder(int src, const vector<vector<int>>& adj, vector<bool>& vis) {
    vector<int> order, st = {src};
    while (!st.empty()) {
        int u = st.back(); st.pop_back();
        if (vis[u]) continue;
        vis[u] = true;
        order.push_back(u);
        for (int v : adj[u]) if (!vis[v]) st.push_back(v);
    }
    return order;
}

int countComponents(const vector<vector<int>>& adj) {
    int n = adj.size(), comps = 0;
    vector<bool> vis(n, false);
    for (int i = 0; i < n; i++)
        if (!vis[i]) { dfsOrder(i, adj, vis); comps++; }
    return comps;
}


// ============================================================================
// 4. 0-1 BFS — shortest path when every weight is 0 or 1
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Shortest path when edges cost only 0 or 1; faster than Dijkstra.
// COMMON PROBLEM: 'Minimum walls to break to reach the exit' (free cell = 0, wall = 1),
//                 'minimum edge reversals to go from 1 to n' (original = 0, reversed = 1).
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Like Dijkstra but with a deque: weight-0 edges go to the FRONT,
// weight-1 edges go to the BACK. adj[u] = {(v, w)} with w in {0,1}.
// ============================================================================
vector<int> zeroOneBFS(int src, const vector<vector<pair<int,int>>>& adj) {
    int n = adj.size();
    vector<int> d(n, INT_MAX);
    deque<int> dq;
    d[src] = 0; dq.push_back(src);
    while (!dq.empty()) {
        int u = dq.front(); dq.pop_front();
        for (auto [v, w] : adj[u])
            if (d[u] + w < d[v]) {
                d[v] = d[u] + w;
                if (w == 0) dq.push_front(v); else dq.push_back(v);
            }
    }
    return d;
}


// ============================================================================
// 5. DSU (Disjoint Set Union / Union-Find)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Tracks which items are in the same group and merges groups fast.
// COMMON PROBLEM: Online queries 'connect a b' / 'are a and b connected?',
//                 cycle detection in undirected graph, Kruskal MST, 'size of largest group'.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// find(x)    : representative of x's set (path compression)
// unite(a,b) : merge sets, returns false if already in same set (=> cycle!)
// size(x)    : size of x's set
// ============================================================================
struct DSU {
    vector<int> p, sz;
    DSU(int n) : p(n), sz(n, 1) { iota(p.begin(), p.end(), 0); }
    int find(int x) {
        while (x != p[x]) x = p[x] = p[p[x]];   // path halving
        return x;
    }
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) swap(a, b);           // union by size
        p[b] = a; sz[a] += sz[b];
        return true;
    }
    bool same(int a, int b) { return find(a) == find(b); }
    int size(int x) { return sz[find(x)]; }
};


// ============================================================================
// 6. TOPOLOGICAL SORT (Kahn's algorithm) — DIRECTED graph
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Orders nodes of a DAG so every edge goes from earlier to later.
// COMMON PROBLEM: 'Course schedule: order courses so prerequisites come first (or IMPOSSIBLE)',
//                 task scheduling, DP on DAG (longest path / count paths), dependency cycle check.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Repeatedly take a node with in-degree 0.
// If the result has fewer than n nodes => the graph has a CYCLE.
// For the lexicographically smallest order, replace queue with
// priority_queue<int, vector<int>, greater<int>>.
// ============================================================================
vector<int> topoSort(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<int> indeg(n, 0), order;
    for (int u = 0; u < n; u++) for (int v : adj[u]) indeg[v]++;
    queue<int> q;
    for (int i = 0; i < n; i++) if (indeg[i] == 0) q.push(i);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : adj[u]) if (--indeg[v] == 0) q.push(v);
    }
    if ((int)order.size() != n) return {};      // cycle exists
    return order;
}
// Directed cycle check:   topoSort(adj).empty()  (for n > 0)
// Undirected cycle check: use DSU — if unite(u,v) returns false, there's a cycle.


// ============================================================================
// 7. BIPARTITE CHECK — can we color nodes with 2 colors so every edge
//    connects different colors? (= graph has no odd cycle)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Checks if nodes split into 2 groups with edges only across groups.
// COMMON PROBLEM: 'Split students into 2 teams so no two friends share a team',
//                 'does the graph have an odd cycle?', first step before bipartite matching.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// color[v] in {0,1} on success.
// ============================================================================
bool isBipartite(const vector<vector<int>>& adj, vector<int>& color) {
    int n = adj.size();
    color.assign(n, -1);
    for (int s = 0; s < n; s++) {
        if (color[s] != -1) continue;
        queue<int> q; q.push(s); color[s] = 0;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (color[v] == -1) { color[v] = color[u] ^ 1; q.push(v); }
                else if (color[v] == color[u]) return false;   // odd cycle
            }
        }
    }
    return true;
}


// ============================================================================
// 8. DIJKSTRA — single-source shortest path, NON-NEGATIVE weights
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Shortest distance from one source with NON-NEGATIVE edge weights.
// COMMON PROBLEM: 'Cheapest route from city 1 to every city',
//                 state-graph Dijkstra: (node, coupon_used) for 'one flight at half price'.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// adj[u] = {(v, w)}. dist[v] = INF if unreachable.
// ============================================================================
struct Dijkstra {
    vector<ll> dist;
    vector<int> par;
    void run(int src, const vector<vector<pair<int,ll>>>& adj) {
        int n = adj.size();
        dist.assign(n, INF); par.assign(n, -1);
        // min-heap of (distance, node)
        priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<>> pq;
        dist[src] = 0; pq.push({0, src});
        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d != dist[u]) continue;           // outdated entry, skip
            for (auto [v, w] : adj[u])
                if (dist[u] + w < dist[v]) {      // relax edge
                    dist[v] = dist[u] + w;
                    par[v] = u;
                    pq.push({dist[v], v});
                }
        }
    }
    vector<int> path(int t) {
        vector<int> p;
        if (dist[t] == INF) return p;
        for (int v = t; v != -1; v = par[v]) p.push_back(v);
        reverse(p.begin(), p.end());
        return p;
    }
};


// ============================================================================
// 9. BELLMAN-FORD — shortest paths with NEGATIVE weights
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Shortest paths with negative weights; detects negative cycles.
// COMMON PROBLEM: 'Is there a currency arbitrage loop?', 'can the score grow infinitely?'
//                 (negate weights, check -INF reaching n), systems of difference constraints.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Relax all edges n-1 times. If something still improves afterwards,
// there's a negative cycle. We then mark every node affected by a negative
// cycle with -INF (its distance can be made arbitrarily small).
// Returns true if NO negative cycle is reachable from src.
// ============================================================================
struct Edge { int u, v; ll w; };

bool bellmanFord(int n, int src, const vector<Edge>& edges, vector<ll>& dist) {
    dist.assign(n, INF);
    dist[src] = 0;
    for (int i = 0; i < n - 1; i++) {
        bool changed = false;
        for (auto& e : edges)
            if (dist[e.u] != INF && dist[e.u] + e.w < dist[e.v]) {
                dist[e.v] = dist[e.u] + e.w;
                changed = true;
            }
        if (!changed) break;                       // early exit
    }
    bool ok = true;
    // propagate -INF through everything reachable from a negative cycle
    for (int i = 0; i < n; i++)
        for (auto& e : edges) {
            if (dist[e.u] == INF) continue;
            if (dist[e.u] == -INF || dist[e.u] + e.w < dist[e.v]) {
                if (dist[e.v] != -INF) ok = false;
                dist[e.v] = -INF;
            }
        }
    return ok;
}


// ============================================================================
// 10. FLOYD-WARSHALL — all-pairs shortest paths (n <= ~500)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Shortest distance between EVERY pair of nodes (small n).
// COMMON PROBLEM: 'Answer q queries: shortest path a->b' with n <= 500,
//                 transitive closure ('can i reach j?'), minimum-weight cycle in a graph.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// d[i][j] = edge weight (INF if no edge), d[i][i] = 0. Modified in place.
// Negative cycle exists if some d[i][i] < 0 afterwards.
// ============================================================================
void floydWarshall(vector<vector<ll>>& d) {
    int n = d.size();
    for (int k = 0; k < n; k++)           // k = intermediate node (MUST be outer)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (d[i][k] < INF && d[k][j] < INF)
                    d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
}


// ============================================================================
// 11. KRUSKAL — Minimum Spanning Tree
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Minimum Spanning Tree: connect all nodes with minimum total edge cost.
// COMMON PROBLEM: 'Minimum cost to build roads so every city is connected',
//                 max spanning tree, 'minimize the largest edge on a path' (bottleneck).
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Sort edges by weight, add an edge if it joins two different components.
// Returns total weight, or -1 if the graph is not connected.
// (For MAXIMUM spanning tree: sort in decreasing order.)
// ============================================================================
ll kruskal(int n, vector<Edge> edges, vector<Edge>* mstEdges = nullptr) {
    sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) { return a.w < b.w; });
    DSU dsu(n);
    ll total = 0; int used = 0;
    for (auto& e : edges)
        if (dsu.unite(e.u, e.v)) {
            total += e.w; used++;
            if (mstEdges) mstEdges->push_back(e);
        }
    return used == n - 1 ? total : -1;
}


// ============================================================================
// 12. PRIM — Minimum Spanning Tree (good for dense graphs / adjacency lists)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Same MST answer as Kruskal, but grows one tree from a start node.
// COMMON PROBLEM: Dense graphs: 'connect all n points with minimum cable length',
//                 use when the graph is given as an adjacency list or is complete.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Grow the tree from node 0, always taking the cheapest edge leaving it.
// Returns total weight, or -1 if not connected.
// ============================================================================
ll prim(const vector<vector<pair<int,ll>>>& adj) {
    int n = adj.size(), cnt = 0;
    vector<bool> inTree(n, false);
    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<>> pq;
    pq.push({0, 0});
    ll total = 0;
    while (!pq.empty()) {
        auto [w, u] = pq.top(); pq.pop();
        if (inTree[u]) continue;
        inTree[u] = true; total += w; cnt++;
        for (auto [v, c] : adj[u]) if (!inTree[v]) pq.push({c, v});
    }
    return cnt == n ? total : -1;
}


// ============================================================================
// 13. LCA — Lowest Common Ancestor with BINARY LIFTING (on a TREE)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Finds the deepest common ancestor of two tree nodes; fast distance queries.
// COMMON PROBLEM: 'q queries: distance between nodes a and b in a tree',
//                 'k-th boss of an employee' (k-th ancestor), path max with extra tables.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// up[j][v] = 2^j-th ancestor of v.
// lca(a,b)      : lowest common ancestor
// kth(v,k)      : k-th ancestor of v
// dist(a,b)     : weighted distance between a and b
// edgesBetween  : number of edges between a and b
// adj[u] = {(v, w)}. For an unweighted tree use w = 1.
// ============================================================================
struct LCA {
    int n, LOG;
    vector<vector<int>> up;
    vector<int> depth;
    vector<ll> distRoot;          // weighted distance from root
    LCA(const vector<vector<pair<int,ll>>>& adj, int root = 0) {
        n = adj.size(); LOG = 1;
        while ((1 << LOG) <= n) LOG++;
        up.assign(LOG, vector<int>(n, root));
        depth.assign(n, 0); distRoot.assign(n, 0);
        // iterative DFS to fill parent (up[0]), depth, distRoot
        vector<int> st = {root};
        vector<bool> vis(n, false); vis[root] = true;
        while (!st.empty()) {
            int u = st.back(); st.pop_back();
            for (auto [v, w] : adj[u])
                if (!vis[v]) {
                    vis[v] = true;
                    up[0][v] = u;
                    depth[v] = depth[u] + 1;
                    distRoot[v] = distRoot[u] + w;
                    st.push_back(v);
                }
        }
        // 2^j-th ancestor = 2^(j-1)-th ancestor of the 2^(j-1)-th ancestor
        for (int j = 1; j < LOG; j++)
            for (int v = 0; v < n; v++)
                up[j][v] = up[j - 1][up[j - 1][v]];
    }
    int kth(int v, int k) {               // returns root if k > depth[v]
        for (int j = 0; j < LOG; j++) if (k >> j & 1) v = up[j][v];
        return v;
    }
    int lca(int a, int b) {
        if (depth[a] < depth[b]) swap(a, b);
        a = kth(a, depth[a] - depth[b]);  // bring a up to b's level
        if (a == b) return a;
        for (int j = LOG - 1; j >= 0; j--)  // jump both while ancestors differ
            if (up[j][a] != up[j][b]) { a = up[j][a]; b = up[j][b]; }
        return up[0][a];
    }
    ll dist(int a, int b) { return distRoot[a] + distRoot[b] - 2 * distRoot[lca(a, b)]; }
    int edgesBetween(int a, int b) { return depth[a] + depth[b] - 2 * depth[lca(a, b)]; }
};


// ============================================================================
// 14. BRIDGES & ARTICULATION POINTS — UNDIRECTED graph
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Bridge = edge whose removal disconnects the graph; AP = such a vertex.
// COMMON PROBLEM: 'Find all critical connections (network breaks if removed)',
//                 'which routers are single points of failure?', 2-edge-connected components.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// tin[u] = discovery time, low[u] = smallest tin reachable from u's subtree
// using at most one back edge.
//   Edge (u,v) is a BRIDGE      if low[v] >  tin[u]
//   u is an ARTICULATION POINT  if low[v] >= tin[u] (non-root),
//                               or root with > 1 DFS child.
// Edge ids are used so MULTIPLE EDGES are handled correctly.
// (Recursive: fine for n up to ~2e5; for bigger, increase stack size.)
// ============================================================================
struct BridgesAP {
    int n, m = 0, timer = 0;
    vector<vector<pair<int,int>>> adj;    // (neighbor, edge id)
    vector<int> tin, low;
    vector<bool> isAP;
    vector<pair<int,int>> bridges;
    BridgesAP(int n) : n(n), adj(n) {}
    void addEdge(int u, int v) {
        adj[u].push_back({v, m});
        adj[v].push_back({u, m});
        m++;
    }
    void dfs(int u, int parentEdge) {
        tin[u] = low[u] = timer++;
        int children = 0;
        for (auto [v, id] : adj[u]) {
            if (id == parentEdge) continue;              // don't go back on same edge
            if (tin[v] != -1) {                          // back edge
                low[u] = min(low[u], tin[v]);
            } else {                                     // tree edge
                dfs(v, id);
                low[u] = min(low[u], low[v]);
                if (low[v] > tin[u]) bridges.push_back({u, v});
                if (low[v] >= tin[u] && parentEdge != -1) isAP[u] = true;
                children++;
            }
        }
        if (parentEdge == -1 && children > 1) isAP[u] = true;
    }
    void run() {
        tin.assign(n, -1); low.assign(n, -1); isAP.assign(n, false);
        bridges.clear(); timer = 0;
        for (int i = 0; i < n; i++) if (tin[i] == -1) dfs(i, -1);
    }
};


// ============================================================================
// 15. SCC — Strongly Connected Components (Kosaraju) — DIRECTED graph
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Groups nodes where every node can reach every other (directed graph).
// COMMON PROBLEM: 'Can every city reach every other? If not, print a failing pair',
//                 'planets & kingdoms', compress cycles then DP on the DAG (max coins).
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Step 1: DFS on graph, record nodes by finish time.
// Step 2: DFS on REVERSED graph in decreasing finish time; each tree = 1 SCC.
// comp[v] = SCC id. IDs are in TOPOLOGICAL ORDER of the condensation DAG
// (comp 0 is a "source" component).
// ============================================================================
struct SCC {
    int n, cnt = 0;
    vector<vector<int>> g, rg;
    vector<int> comp, order;
    vector<bool> vis;
    SCC(int n) : n(n), g(n), rg(n) {}
    void addEdge(int u, int v) { g[u].push_back(v); rg[v].push_back(u); }
    void dfs1(int u) {
        vis[u] = true;
        for (int v : g[u]) if (!vis[v]) dfs1(v);
        order.push_back(u);                       // post-order (finish time)
    }
    void dfs2(int u, int c) {
        comp[u] = c;
        for (int v : rg[u]) if (comp[v] == -1) dfs2(v, c);
    }
    int run() {
        vis.assign(n, false); comp.assign(n, -1); order.clear();
        for (int i = 0; i < n; i++) if (!vis[i]) dfs1(i);
        reverse(order.begin(), order.end());
        cnt = 0;
        for (int u : order) if (comp[u] == -1) dfs2(u, cnt++);
        return cnt;
    }
    // DAG where each SCC is one node (call after run())
    vector<vector<int>> condensation() {
        vector<vector<int>> dag(cnt);
        for (int u = 0; u < n; u++)
            for (int v : g[u])
                if (comp[u] != comp[v]) dag[comp[u]].push_back(comp[v]);
        for (auto& x : dag) { sort(x.begin(), x.end()); x.erase(unique(x.begin(), x.end()), x.end()); }
        return dag;
    }
};


// ============================================================================
// 16. 2-SAT — boolean variables x0..x(n-1), clauses of form (A OR B)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Solves boolean formulas where every clause has exactly 2 literals.
// COMMON PROBLEM: 'Each person wants topping A or not B — satisfy everyone?',
//                 'pick one of 2 options per item with pairwise conflicts'.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Literal "x = true" is node 2x, "x = false" is node 2x+1.
// Clause (A or B) gives implications: (!A -> B) and (!B -> A).
// Unsatisfiable iff x and !x are in the same SCC.
// Assignment: x = true if comp[x_true] comes AFTER comp[x_false] in topo order.
// ============================================================================
struct TwoSAT {
    int n;
    SCC scc;
    vector<bool> value;
    TwoSAT(int n) : n(n), scc(2 * n) {}
    int lit(int x, bool val) { return 2 * x + (val ? 0 : 1); }
    // (x == vx) OR (y == vy)
    void addOr(int x, bool vx, int y, bool vy) {
        int a = lit(x, vx), b = lit(y, vy);
        scc.addEdge(a ^ 1, b);    // !a -> b
        scc.addEdge(b ^ 1, a);    // !b -> a
    }
    void forceValue(int x, bool vx) { addOr(x, vx, x, vx); }              // x must be vx
    void addImplies(int x, bool vx, int y, bool vy) { addOr(x, !vx, y, vy); } // (x==vx) -> (y==vy)
    void addXor(int x, int y) { addOr(x, true, y, true); addOr(x, false, y, false); } // exactly one true
    void addAtMostOne(int x, int y) { addOr(x, false, y, false); }         // not both true
    bool solve() {
        scc.run();
        value.assign(n, false);
        for (int i = 0; i < n; i++) {
            if (scc.comp[2 * i] == scc.comp[2 * i + 1]) return false;
            value[i] = scc.comp[2 * i] > scc.comp[2 * i + 1];
        }
        return true;
    }
};


// ============================================================================
// 17. MAX FLOW — DINIC
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Maximum amount of 'stuff' you can push from source s to sink t.
// COMMON PROBLEM: 'Max download speed from server to computer', 'max edge-disjoint paths',
//                 min cut: 'cheapest edges to remove to separate s and t', project selection.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// addEdge(u, v, cap) adds a directed edge. For an UNDIRECTED edge use
// addEdge(u, v, cap, cap).
// Edges are stored in pairs: edge id and id^1 is its reverse edge.
// After maxflow:
//   - flow on edge id = (original cap) - e[id].cap
//   - minCut(): nodes reachable from s in the residual graph (S-side).
//     Edges going from S-side to T-side form the minimum cut.
// ============================================================================
struct Dinic {
    struct E { int to; ll cap; };
    int n;
    vector<E> e;
    vector<vector<int>> g;
    vector<int> level, it;
    Dinic(int n) : n(n), g(n) {}
    int addEdge(int u, int v, ll cap, ll revCap = 0) {
        g[u].push_back(e.size()); e.push_back({v, cap});
        g[v].push_back(e.size()); e.push_back({u, revCap});
        return e.size() - 2;                      // id of forward edge
    }
    bool bfs(int s, int t) {                      // build level graph
        level.assign(n, -1);
        queue<int> q; level[s] = 0; q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int id : g[u])
                if (e[id].cap > 0 && level[e[id].to] == -1) {
                    level[e[id].to] = level[u] + 1;
                    q.push(e[id].to);
                }
        }
        return level[t] != -1;
    }
    ll dfs(int u, int t, ll f) {                  // send blocking flow
        if (u == t) return f;
        for (int& i = it[u]; i < (int)g[u].size(); i++) {
            int id = g[u][i];
            int v = e[id].to;
            if (e[id].cap > 0 && level[v] == level[u] + 1) {
                ll d = dfs(v, t, min(f, e[id].cap));
                if (d > 0) {
                    e[id].cap -= d;
                    e[id ^ 1].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }
    ll maxflow(int s, int t) {
        ll flow = 0;
        while (bfs(s, t)) {
            it.assign(n, 0);
            while (ll f = dfs(s, t, INF)) flow += f;
        }
        return flow;
    }
    vector<bool> minCut(int s) {                  // call after maxflow
        bfs(s, s);
        vector<bool> side(n);
        for (int i = 0; i < n; i++) side[i] = level[i] != -1;
        return side;
    }
};


// ============================================================================
// 18. MIN COST MAX FLOW — SPFA (Bellman-Ford with queue)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Max flow, but picks the cheapest one among all max flows.
// COMMON PROBLEM: 'Ship k units of goods with minimum transport cost',
//                 'assign tasks to workers with costs and capacities'.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Sends as much flow as possible (up to maxF) with minimum total cost.
// Works with negative costs as long as there are no negative cycles.
// run(s, t) returns {flow, cost}.
// ============================================================================
struct MCMF {
    struct E { int to; ll cap, cost; };
    int n;
    vector<E> e;
    vector<vector<int>> g;
    vector<ll> dist;
    vector<int> parEdge;
    vector<bool> inq;
    MCMF(int n) : n(n), g(n) {}
    void addEdge(int u, int v, ll cap, ll cost) {
        g[u].push_back(e.size()); e.push_back({v, cap, cost});
        g[v].push_back(e.size()); e.push_back({u, 0, -cost});
    }
    bool spfa(int s, int t) {                     // cheapest augmenting path
        dist.assign(n, INF); parEdge.assign(n, -1); inq.assign(n, false);
        queue<int> q; dist[s] = 0; q.push(s); inq[s] = true;
        while (!q.empty()) {
            int u = q.front(); q.pop(); inq[u] = false;
            for (int id : g[u]) {
                auto& ed = e[id];
                if (ed.cap > 0 && dist[u] + ed.cost < dist[ed.to]) {
                    dist[ed.to] = dist[u] + ed.cost;
                    parEdge[ed.to] = id;
                    if (!inq[ed.to]) { inq[ed.to] = true; q.push(ed.to); }
                }
            }
        }
        return dist[t] < INF;
    }
    pair<ll,ll> run(int s, int t, ll maxF = INF) {
        ll flow = 0, cost = 0;
        while (flow < maxF && spfa(s, t)) {
            ll f = maxF - flow;                    // bottleneck on the path
            for (int v = t; v != s; v = e[parEdge[v] ^ 1].to) f = min(f, e[parEdge[v]].cap);
            for (int v = t; v != s; v = e[parEdge[v] ^ 1].to) {
                e[parEdge[v]].cap -= f;
                e[parEdge[v] ^ 1].cap += f;
            }
            flow += f; cost += f * dist[t];
        }
        return {flow, cost};
    }
};


// ============================================================================
// 19. BIPARTITE MATCHING — HOPCROFT-KARP
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Maximum number of pairs between left and right, no node used twice.
// COMMON PROBLEM: 'Max boy-girl dance pairs', 'assign jobs to qualified workers',
//                 min vertex cover on bipartite graph, min path cover in DAG (n - matching).
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Left side: 0..nL-1, right side: 0..nR-1. addEdge(l, r).
// matchL[l] = right partner (-1 if none), matchR[r] = left partner.
// Useful facts (Konig): in bipartite graphs
//   min vertex cover = max matching
//   max independent set = V - max matching
//   min path cover in DAG = n - max matching (split each node into in/out)
// ============================================================================
struct HopcroftKarp {
    int nL, nR;
    vector<vector<int>> g;
    vector<int> matchL, matchR, dist;
    HopcroftKarp(int nL, int nR) : nL(nL), nR(nR), g(nL) {}
    void addEdge(int l, int r) { g[l].push_back(r); }
    bool bfs() {                         // layer free left nodes
        queue<int> q;
        dist.assign(nL, -1);
        for (int l = 0; l < nL; l++) if (matchL[l] == -1) { dist[l] = 0; q.push(l); }
        bool found = false;
        while (!q.empty()) {
            int l = q.front(); q.pop();
            for (int r : g[l]) {
                int l2 = matchR[r];
                if (l2 == -1) found = true;           // free right node reachable
                else if (dist[l2] == -1) { dist[l2] = dist[l] + 1; q.push(l2); }
            }
        }
        return found;
    }
    bool dfs(int l) {                    // find augmenting path
        for (int r : g[l]) {
            int l2 = matchR[r];
            if (l2 == -1 || (dist[l2] == dist[l] + 1 && dfs(l2))) {
                matchL[l] = r; matchR[r] = l;
                return true;
            }
        }
        dist[l] = -1;
        return false;
    }
    int maxMatching() {
        matchL.assign(nL, -1); matchR.assign(nR, -1);
        int res = 0;
        while (bfs())
            for (int l = 0; l < nL; l++)
                if (matchL[l] == -1 && dfs(l)) res++;
        return res;
    }
};


// ============================================================================
// 20. HUNGARIAN ALGORITHM — minimum cost assignment
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Perfect assignment of n workers to m jobs minimizing total cost.
// COMMON PROBLEM: 'n workers, n jobs, cost matrix: minimum total cost',
//                 'maximum total profit assignment' (negate costs), n up to ~500.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// cost[i][j] = cost of assigning worker i to job j (n workers, m jobs, n <= m).
// Returns {min total cost, assign} where assign[i] = job of worker i.
// For MAXIMUM: negate all costs, then negate the answer.
// ============================================================================
pair<ll, vector<int>> hungarian(const vector<vector<ll>>& cost) {
    int n = cost.size(), m = cost[0].size();
    vector<ll> u(n + 1, 0), v(m + 1, 0);           // potentials
    vector<int> p(m + 1, 0), way(m + 1, 0);        // p[j] = row matched to column j
    for (int i = 1; i <= n; i++) {
        p[0] = i;
        int j0 = 0;
        vector<ll> minv(m + 1, INF);
        vector<bool> used(m + 1, false);
        do {
            used[j0] = true;
            int i0 = p[j0], j1 = 0;
            ll delta = INF;
            for (int j = 1; j <= m; j++)
                if (!used[j]) {
                    ll cur = cost[i0 - 1][j - 1] - u[i0] - v[j];
                    if (cur < minv[j]) { minv[j] = cur; way[j] = j0; }
                    if (minv[j] < delta) { delta = minv[j]; j1 = j; }
                }
            for (int j = 0; j <= m; j++)
                if (used[j]) { u[p[j]] += delta; v[j] -= delta; }
                else minv[j] -= delta;
            j0 = j1;
        } while (p[j0] != 0);
        do { int j1 = way[j0]; p[j0] = p[j1]; j0 = j1; } while (j0);
    }
    vector<int> assign(n, -1);
    for (int j = 1; j <= m; j++) if (p[j]) assign[p[j] - 1] = j - 1;
    return {-v[0], assign};
}


// ============================================================================
// 21. EULER PATH / CIRCUIT — HIERHOLZER (uses every EDGE exactly once)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Walks through every EDGE exactly once (path or closed circuit).
// COMMON PROBLEM: 'Mail delivery: use every street once and return home',
//                 'rebuild string from all k-length substrings' (De Bruijn), 'teleporters path'.
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Conditions:
//   Undirected: 0 odd-degree nodes (circuit) or exactly 2 (path).
//   Directed  : all in == out (circuit) or one node out-in=1 (start)
//               and one node in-out=1 (end).
//   All edges must be in one connected component.
// find() returns the sequence of vertices (m+1 of them), or {} if impossible.
// ============================================================================
struct Euler {
    int n, m = 0;
    bool directed;
    vector<vector<pair<int,int>>> g;   // (to, edge id)
    vector<int> indeg, outdeg;
    Euler(int n, bool directed) : n(n), directed(directed), g(n), indeg(n, 0), outdeg(n, 0) {}
    void addEdge(int u, int v) {
        g[u].push_back({v, m});
        if (!directed) g[v].push_back({u, m});
        outdeg[u]++; indeg[v]++;
        m++;
    }
    vector<int> find() {
        int start = -1;
        if (directed) {
            int plus = 0, minus = 0;
            for (int v = 0; v < n; v++) {
                int d = outdeg[v] - indeg[v];
                if (d == 1) { plus++; start = v; }
                else if (d == -1) minus++;
                else if (d != 0) return {};
            }
            if (plus > 1 || plus != minus) return {};
        } else {
            int odd = 0;
            for (int v = 0; v < n; v++)
                if ((indeg[v] + outdeg[v]) % 2) { odd++; start = v; }
            if (odd != 0 && odd != 2) return {};
        }
        if (start == -1)                       // circuit: start anywhere with an edge
            for (int v = 0; v < n; v++) if (!g[v].empty()) { start = v; break; }
        if (start == -1) return {};            // no edges at all

        vector<int> ptr(n, 0), path, st = {start};
        vector<bool> used(m, false);
        while (!st.empty()) {
            int u = st.back();
            while (ptr[u] < (int)g[u].size() && used[g[u][ptr[u]].second]) ptr[u]++;
            if (ptr[u] == (int)g[u].size()) {  // dead end -> add to answer
                path.push_back(u); st.pop_back();
            } else {
                auto [v, id] = g[u][ptr[u]];
                used[id] = true;
                st.push_back(v);
            }
        }
        if ((int)path.size() != m + 1) return {};   // graph not connected
        reverse(path.begin(), path.end());
        return path;
    }
};


// ============================================================================
// 22. TREE DIAMETER — longest path in a tree
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Length of the longest path between any two nodes in a tree.
// COMMON PROBLEM: 'Longest cable between two computers in a tree network',
//                 'for each node, farthest node distance' = max(dist from A, dist from B).
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Trick: from any node, the farthest node A is one end of the diameter.
// The farthest node from A is the other end B.
// ============================================================================
vector<ll> treeDistances(int src, const vector<vector<pair<int,ll>>>& adj) {
    int n = adj.size();
    vector<ll> d(n, -1);
    vector<int> st = {src};
    d[src] = 0;
    while (!st.empty()) {
        int u = st.back(); st.pop_back();
        for (auto [v, w] : adj[u]) if (d[v] == -1) { d[v] = d[u] + w; st.push_back(v); }
    }
    return d;
}

// returns {diameter length, endpoint A, endpoint B}
tuple<ll,int,int> treeDiameter(const vector<vector<pair<int,ll>>>& adj) {
    auto d0 = treeDistances(0, adj);
    int A = max_element(d0.begin(), d0.end()) - d0.begin();
    auto dA = treeDistances(A, adj);
    int B = max_element(dA.begin(), dA.end()) - dA.begin();
    return {dA[B], A, B};
}


// ============================================================================
// 23. HEAVY-LIGHT DECOMPOSITION — path queries/updates on a tree
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Breaks a tree into chains so path queries become O(log) segment-tree ranges.
// COMMON PROBLEM: 'Change value of node v; query sum/max on path a->b',
//                 'path queries with point updates' (CSES Path Queries II).
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Splits the tree into chains so any path crosses O(log n) chains.
// Each chain occupies a contiguous range in the segment tree.
// Here: values on NODES, point update, path SUM query.
// (Change SegTree's operation to max/min/etc. as needed.
//  For values on EDGES: store edge (par[v], v) at node v, and skip the
//  LCA node in the last query: st.query(pos[a] + 1, pos[b]).)
// ============================================================================
struct SegTree {                     // iterative segment tree, sum
    int n;
    vector<ll> t;
    SegTree(int n) : n(n), t(2 * n, 0) {}
    void update(int p, ll val) {     // set position p = val
        for (t[p += n] = val; p > 1; p >>= 1) t[p >> 1] = t[p] + t[p ^ 1];
    }
    ll query(int l, int r) {         // sum on [l, r] inclusive
        ll res = 0;
        for (l += n, r += n + 1; l < r; l >>= 1, r >>= 1) {
            if (l & 1) res += t[l++];
            if (r & 1) res += t[--r];
        }
        return res;
    }
};

struct HLD {
    int n, cur = 0;
    vector<int> par, dep, heavy, head, pos, sz;
    SegTree st;
    HLD(const vector<vector<int>>& adj, int root = 0)
        : n(adj.size()), par(n, -1), dep(n, 0), heavy(n, -1),
          head(n), pos(n), sz(n, 1), st(n) {
        // BFS order (no recursion)
        vector<int> order = {root};
        for (int i = 0; i < (int)order.size(); i++) {
            int u = order[i];
            for (int v : adj[u]) if (v != par[u]) { par[v] = u; dep[v] = dep[u] + 1; order.push_back(v); }
        }
        // subtree sizes, bottom-up
        for (int i = (int)order.size() - 1; i > 0; i--) sz[par[order[i]]] += sz[order[i]];
        // heavy child = child with the biggest subtree
        for (int u : order)
            for (int v : adj[u])
                if (v != par[u] && (heavy[u] == -1 || sz[v] > sz[heavy[u]])) heavy[u] = v;
        // walk every chain from its top, giving consecutive positions
        for (int u : order)
            if (par[u] == -1 || heavy[par[u]] != u)
                for (int v = u; v != -1; v = heavy[v]) { head[v] = u; pos[v] = cur++; }
    }
    void update(int v, ll val) { st.update(pos[v], val); }
    ll queryPath(int a, int b) {
        ll res = 0;
        // climb from the deeper chain head until both are on the same chain
        for (; head[a] != head[b]; b = par[head[b]]) {
            if (dep[head[a]] > dep[head[b]]) swap(a, b);
            res += st.query(pos[head[b]], pos[b]);
        }
        if (dep[a] > dep[b]) swap(a, b);
        res += st.query(pos[a], pos[b]);
        return res;
    }
    int lca(int a, int b) {
        for (; head[a] != head[b]; b = par[head[b]])
            if (dep[head[a]] > dep[head[b]]) swap(a, b);
        return dep[a] < dep[b] ? a : b;
    }
};


// ============================================================================
// 24. CENTROID DECOMPOSITION (skeleton)
// ----------------------------------------------------------------------------
// WHAT IT DOES  : Recursively splits a tree at centroids; every path goes through some centroid.
// COMMON PROBLEM: 'Count pairs of nodes at distance exactly k',
//                 'nearest red node with paint updates' (Xenia and Tree).
// ----------------------------------------------------------------------------
// HOW IT WORKS / NOTES:
// Centroid = node whose removal leaves pieces of size <= n/2.
// Recursively removing centroids gives a "centroid tree" of depth O(log n).
// Every path in the original tree passes through the centroid of some
// level, so: "count/handle all paths through c" at each centroid c.
// cpar[v] = parent of v in the centroid tree (-1 for the top).
// Put your problem logic where it says  >>> PROCESS <<<.
// ============================================================================
struct CentroidDecomp {
    vector<vector<int>> adj;
    vector<int> sz, cpar;
    vector<bool> removed;
    CentroidDecomp(const vector<vector<int>>& a)
        : adj(a), sz(a.size()), cpar(a.size(), -1), removed(a.size(), false) {
        build(0, -1);
    }
    int calcSize(int u, int p) {
        sz[u] = 1;
        for (int v : adj[u]) if (v != p && !removed[v]) sz[u] += calcSize(v, u);
        return sz[u];
    }
    int findCentroid(int u, int p, int total) {
        for (int v : adj[u])
            if (v != p && !removed[v] && sz[v] * 2 > total) return findCentroid(v, u, total);
        return u;
    }
    void build(int u, int p) {
        int total = calcSize(u, -1);
        int c = findCentroid(u, -1, total);
        cpar[c] = p;
        removed[c] = true;
        // >>> PROCESS all paths passing through centroid c here <<<
        for (int v : adj[c]) if (!removed[v]) build(v, c);
    }
};


// ============================================================================
//                               DEMO / TESTS
// ============================================================================
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // ---------- Unweighted undirected graph ----------
    //  0 - 1 - 2 - 3      4 - 5
    int n = 6;
    vector<vector<int>> adj(n);
    auto addU = [&](int u, int v) { adj[u].push_back(v); adj[v].push_back(u); };
    addU(0, 1); addU(1, 2); addU(2, 3); addU(4, 5);

    BFS bfs; bfs.run(0, adj);
    cout << "[1] BFS dist 0->3 = " << bfs.dist[3] << ", path:";
    for (int v : bfs.path(3)) cout << ' ' << v;
    cout << "\n";

    vector<string> grid = {"..#", ".##", "..."};
    cout << "[2] Grid BFS (0,0)->(2,2) = " << gridBFS(grid, 0, 0)[2][2] << "\n";

    cout << "[3] Components = " << countComponents(adj) << "\n";

    vector<vector<pair<int,int>>> g01(3);
    g01[0].push_back({1, 1}); g01[0].push_back({2, 1}); g01[2].push_back({1, 0});
    cout << "[4] 0-1 BFS dist 0->1 = " << zeroOneBFS(0, g01)[1] << "\n";

    DSU dsu(5); dsu.unite(0, 1); dsu.unite(1, 2);
    cout << "[5] DSU same(0,2)=" << dsu.same(0, 2) << " size(0)=" << dsu.size(0) << "\n";

    // ---------- Directed graph for topo sort ----------
    vector<vector<int>> dag(4);
    dag[0] = {1, 2}; dag[1] = {3}; dag[2] = {3};
    cout << "[6] Topo order:";
    for (int v : topoSort(dag)) cout << ' ' << v;
    cout << "\n";

    vector<int> color;
    cout << "[7] Bipartite = " << isBipartite(adj, color) << "\n";

    // ---------- Weighted graph ----------
    vector<vector<pair<int,ll>>> wadj(4);
    auto addW = [&](int u, int v, ll w) { wadj[u].push_back({v, w}); wadj[v].push_back({u, w}); };
    addW(0, 1, 4); addW(0, 2, 1); addW(2, 1, 2); addW(1, 3, 5);
    Dijkstra dj; dj.run(0, wadj);
    cout << "[8] Dijkstra 0->3 = " << dj.dist[3] << ", path:";
    for (int v : dj.path(3)) cout << ' ' << v;
    cout << "\n";

    vector<Edge> edges = {{0, 1, 4}, {0, 2, 1}, {2, 1, -2}, {1, 3, 5}};
    vector<ll> bf;
    bool ok = bellmanFord(4, 0, edges, bf);
    cout << "[9] Bellman-Ford ok=" << ok << " dist 0->3 = " << bf[3] << "\n";

    vector<vector<ll>> fw(4, vector<ll>(4, INF));
    for (int i = 0; i < 4; i++) fw[i][i] = 0;
    for (auto& e : edges) fw[e.u][e.v] = min(fw[e.u][e.v], e.w);
    floydWarshall(fw);
    cout << "[10] Floyd 0->3 = " << fw[0][3] << "\n";

    vector<Edge> und = {{0, 1, 4}, {0, 2, 1}, {2, 1, 2}, {1, 3, 5}};
    cout << "[11] Kruskal MST = " << kruskal(4, und) << "\n";
    cout << "[12] Prim MST = " << prim(wadj) << "\n";

    // ---------- Tree ----------
    //        0
    //       / \    (tree)
    //      1   2
    //     / \   \  (tree)
    //    3   4   5
    vector<vector<pair<int,ll>>> tree(6);
    vector<vector<int>> tadj(6);
    auto addT = [&](int u, int v) {
        tree[u].push_back({v, 1}); tree[v].push_back({u, 1});
        tadj[u].push_back(v); tadj[v].push_back(u);
    };
    addT(0, 1); addT(0, 2); addT(1, 3); addT(1, 4); addT(2, 5);
    LCA lca(tree, 0);
    cout << "[13] LCA(3,4)=" << lca.lca(3, 4) << " LCA(3,5)=" << lca.lca(3, 5)
         << " dist(3,5)=" << lca.dist(3, 5) << "\n";

    // ---------- Bridges & APs ----------
    //  0 - 1 - 2      (1-2-3 is a triangle; 0-1 is a bridge)
    //       \ /
    //        3
    BridgesAP bap(4);
    bap.addEdge(0, 1); bap.addEdge(1, 2); bap.addEdge(2, 3); bap.addEdge(3, 1);
    bap.run();
    cout << "[14] Bridges:";
    for (auto [u, v] : bap.bridges) cout << " (" << u << "," << v << ")";
    cout << "  APs:";
    for (int i = 0; i < 4; i++) if (bap.isAP[i]) cout << ' ' << i;
    cout << "\n";

    // ---------- SCC ----------
    SCC scc(5);
    scc.addEdge(0, 1); scc.addEdge(1, 2); scc.addEdge(2, 0); scc.addEdge(2, 3); scc.addEdge(3, 4);
    cout << "[15] SCC count = " << scc.run() << "\n";

    // ---------- 2-SAT ----------
    // (x0 OR x1) AND (NOT x0 OR x1) AND (NOT x1 OR NOT x2)  => x1 = true, x2 = false
    TwoSAT ts(3);
    ts.addOr(0, true, 1, true);
    ts.addOr(0, false, 1, true);
    ts.addOr(1, false, 2, false);
    bool sat = ts.solve();
    cout << "[16] 2-SAT sat=" << sat << " values:";
    for (int i = 0; i < 3; i++) cout << ' ' << ts.value[i];
    cout << "\n";

    // ---------- Max flow ----------
    Dinic din(4);
    din.addEdge(0, 1, 3); din.addEdge(0, 2, 2); din.addEdge(1, 2, 5);
    din.addEdge(1, 3, 2); din.addEdge(2, 3, 3);
    cout << "[17] Max flow = " << din.maxflow(0, 3) << "\n";

    // ---------- Min cost max flow ----------
    MCMF mc(4);
    mc.addEdge(0, 1, 2, 1); mc.addEdge(0, 2, 1, 2);
    mc.addEdge(1, 3, 1, 3); mc.addEdge(1, 2, 1, 1); mc.addEdge(2, 3, 2, 1);
    auto [fl, co] = mc.run(0, 3);
    cout << "[18] MCMF flow=" << fl << " cost=" << co << "\n";

    // ---------- Bipartite matching ----------
    HopcroftKarp hk(3, 3);
    hk.addEdge(0, 0); hk.addEdge(0, 1); hk.addEdge(1, 0); hk.addEdge(2, 2);
    cout << "[19] Max matching = " << hk.maxMatching() << "\n";

    // ---------- Hungarian ----------
    vector<vector<ll>> cst = {{4, 1, 3}, {2, 0, 5}, {3, 2, 2}};
    auto [hc, asg] = hungarian(cst);
    cout << "[20] Hungarian cost=" << hc << " assign:";
    for (int j : asg) cout << ' ' << j;
    cout << "\n";

    // ---------- Euler ----------
    Euler eu(4, false);
    eu.addEdge(0, 1); eu.addEdge(1, 2); eu.addEdge(2, 0); eu.addEdge(0, 3);
    cout << "[21] Euler path:";
    for (int v : eu.find()) cout << ' ' << v;
    cout << "\n";

    auto [diam, A, B] = treeDiameter(tree);
    cout << "[22] Tree diameter = " << diam << " (" << A << " .. " << B << ")\n";

    // ---------- HLD ----------
    HLD hld(tadj, 0);
    for (int v = 0; v < 6; v++) hld.update(v, v + 1);   // value of node v = v+1
    cout << "[23] HLD path sum 3->5 = " << hld.queryPath(3, 5)
         << "  lca(4,3)=" << hld.lca(4, 3) << "\n";

    CentroidDecomp cd(tadj);
    cout << "[24] Centroid tree parent:";
    for (int v = 0; v < 6; v++) cout << ' ' << cd.cpar[v];
    cout << "\n";
    return 0;
}
