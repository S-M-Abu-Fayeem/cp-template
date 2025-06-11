#include <bits/stdc++.h>
using namespace std;

namespace reroot {
    const auto exclusive = [](const auto& a, const auto& base, const auto& merge_into, int vertex) {
        int n = static_cast<int>(a.size());
        using Aggregate = std::decay_t<decltype(base)>;
        vector<Aggregate> b(n, base);
        for (int bit = static_cast<int>(__lg(n)); bit >= 0; --bit) {
            for (int i = n - 1; i >= 0; --i) b[i] = b[i >> 1];
            int sz = n - (n & !bit);
            for (int i = 0; i < sz; ++i) {
                int index = (i >> bit) ^ 1;
                b[index] = merge_into(b[index], a[i], vertex, i);
            }
        }
        return b;
    };

    const auto rerooter = [](const auto& g, const auto& base, const auto& merge_into, const auto& finalize_merge) {
        int n = static_cast<int>(g.size());
        using Aggregate = std::decay_t<decltype(base(0))>;
        using Value = std::decay_t<decltype(finalize_merge(base(0), 0, 0))>;
        vector<Value> root_dp(n), dp(n);
        vector<vector<Value>> edge_dp(n), redge_dp(n);
        vector<int> bfs, parent(n, -1);
        bfs.reserve(n);
        bfs.push_back(0);
        parent[0] = -1;

        for (int i = 0; i < n; ++i) {
            int u = bfs[i];
            for (auto v : g[u]) {
                if (parent[u] == v) continue;
                parent[v] = u;
                bfs.push_back(v);
            }
        }

        for (int i = n - 1; i >= 0; --i) {
            int u = bfs[i];
            int p_edge_index = -1;
            Aggregate aggregate = base(u);
            for (int edge_index = 0; edge_index < static_cast<int>(g[u].size()); ++edge_index) {
                int v = g[u][edge_index];
                if (parent[u] == v) {
                    p_edge_index = edge_index;
                    continue;
                }
                aggregate = merge_into(aggregate, dp[v], u, edge_index);
            }
            dp[u] = finalize_merge(aggregate, u, p_edge_index);
        }

        for (auto u : bfs) {
            if (parent[u] != -1)
                dp[parent[u]] = dp[u];

            edge_dp[u].reserve(g[u].size());
            for (auto v : g[u]) edge_dp[u].push_back(dp[v]);

            auto dp_exclusive = exclusive(edge_dp[u], base(u), merge_into, u);
            redge_dp[u].reserve(g[u].size());
            for (int i = 0; i < static_cast<int>(dp_exclusive.size()); ++i)
                redge_dp[u].push_back(finalize_merge(dp_exclusive[i], u, i));

            root_dp[u] = finalize_merge(n > 1 ? merge_into(dp_exclusive[0], edge_dp[u][0], u, 0) : base(u), u, -1);

            for (int i = 0; i < static_cast<int>(g[u].size()); ++i) {
                dp[g[u][i]] = redge_dp[u][i];
            }
        }

        return make_tuple(move(root_dp), move(edge_dp), move(redge_dp));
    };
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t = 1;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<long long> values(n);
        for (int i = 0; i < n; i++) cin >> values[i];
        vector<vector<int>> g(n);
        for (int i = 0; i < n - 1; ++i) {
            int u, v;
            cin >> u >> v;
            --u; --v;
            g[u].push_back(v);
            g[v].push_back(u);
        }

        using Aggregate = tuple<long long, long long, long long , long long >;
        using Value = tuple<long long, long long, long long>;

        auto base = [&](int vertex) -> Aggregate {
            return {0LL, 1LL, 0LL, 0LL};
        };

        auto merge_into = [&](Aggregate vertex_dp, Value neighbor_dp, int vertex, int edge_index) -> Aggregate {
            auto &[ans, subtree_size, child_vals, child_sizes] = vertex_dp;
            ans += get<0>(neighbor_dp);
            subtree_size += get<1>(neighbor_dp);
            ans+=get<1>(neighbor_dp)*(values[vertex]^(get<2>(neighbor_dp)));
            child_sizes+=(get<1>(neighbor_dp));
            //child_sizes.push_back(get<1>(neighbor_dp));
            return vertex_dp;
        };

        auto finalize_merge = [&](Aggregate vertex_dp, int vertex, int edge_index) -> Value {
            auto &[ans, subtree_size, child_vals, child_sizes] = vertex_dp;
            long long total = ans;
            
            return {total, subtree_size, values[vertex]};
        };

        auto [reroot_result, edge_dp, redge_dp] = reroot::rerooter(g, base, merge_into, finalize_merge);

        for (int i = 0; i < n; ++i) {
            cout << get<0>(reroot_result[i]) << " ";
        }
        cout << "\n";
    }
    return 0;
}
