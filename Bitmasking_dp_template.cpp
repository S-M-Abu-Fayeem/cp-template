/*
 ============================================================================
   BITMASK DP TEMPLATE — learned by solving one problem
 ============================================================================

   PROBLEM: Elevator Rides  (CSES 1653, https://cses.fi/problemset/task/1653)
   ----------------------------------------------------------------------------
   There are n people who want to reach the top floor of a building.
   The elevator can carry at most x total weight in one ride.
   Person i has weight w[i].
   What is the MINIMUM number of rides needed to take everyone up?

   Input
     First line : n x          (1 <= n <= 20, 1 <= x <= 1e9)
     Second line: w1 w2 ... wn (1 <= wi <= x)
   Output
     One integer: the minimum number of rides.

   Example
     Input:
       4 10
       4 8 6 1
     Output:
       2
     (Ride 1: 4 + 6 = 10, Ride 2: 8 + 1 = 9)

 ============================================================================
   SOLUTION IDEA
 ============================================================================
   Why bitmask?  n <= 20 is the BIG HINT. 2^20 ~ 1e6 subsets, so we can
   afford a DP over every subset of people.

   A "mask" is an integer whose bits say which people are already up:
       bit i = 1  ->  person i has already ridden
       mask = 0b0101 -> persons 0 and 2 are up

   STATE
     dp[mask] = { rides, last }
       rides = minimum number of rides used to bring up exactly the set "mask"
       last  = weight in the CURRENT (last) ride, as small as possible
               (a lighter last ride leaves more room for the next person)

   Comparing two states: fewer rides is better; if equal, smaller "last".
   std::pair compares exactly like that, so we just use min().

   TRANSITION  (add person i who is NOT in mask yet)
     if last + w[i] <= x   ->  same ride:  {rides, last + w[i]}
     else                  ->  new ride :  {rides + 1, w[i]}
     dp[mask | (1<<i)] = min(dp[mask | (1<<i)], that)

   BASE
     dp[0] = {1, 0}   // "1 ride started, 0 weight inside" (avoids a special case)

   ANSWER
     dp[(1<<n) - 1].first     // all bits set = everybody is up

   ORDER
     mask | (1<<i) is always BIGGER than mask, so a simple loop
     mask = 0 .. 2^n - 1 processes every state before it is used.

   COMPLEXITY: O(2^n * n) time, O(2^n) memory  ->  ~2e7 ops for n = 20.
 ============================================================================
*/
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// ============================================================================
// PART 1: BIT OPERATIONS CHEAT SHEET (use these in any bitmask problem)
// ============================================================================
//   (mask >> i) & 1        -> is bit i set?  (is item i in the set?)
//   mask | (1 << i)        -> add item i
//   mask & ~(1 << i)       -> remove item i
//   mask ^ (1 << i)        -> toggle item i
//   (1 << n) - 1           -> full set {0, 1, ..., n-1}
//   __builtin_popcount(m)  -> number of items in the set  (popcountll for long long)
//   __builtin_ctz(m)       -> index of the lowest set bit (m != 0)
//   m & -m                 -> value of the lowest set bit
//   m & (m - 1)            -> remove the lowest set bit
//   (a & b) == a           -> a is a subset of b
//   Use 1LL << i when i can be >= 31.

// ============================================================================
// PART 2: SOLUTION OF THE PROBLEM
// ============================================================================
int solveElevator(int n, ll x, const vector<ll>& w) {
    int FULL = (1 << n) - 1;
    // dp[mask] = {rides, weight of the last ride}
    vector<pair<int, ll>> dp(1 << n, {INT_MAX, 0});
    dp[0] = {1, 0};                               // base case

    for (int mask = 0; mask <= FULL; mask++) {
        if (dp[mask].first == INT_MAX) continue;  // unreachable state (never happens here)
        auto [rides, last] = dp[mask];
        for (int i = 0; i < n; i++) {
            if (mask >> i & 1) continue;          // person i is already up
            pair<int, ll> nxt;
            if (last + w[i] <= x) nxt = {rides, last + w[i]};   // fits in current ride
            else                  nxt = {rides + 1, w[i]};      // start a new ride
            int nmask = mask | (1 << i);
            dp[nmask] = min(dp[nmask], nxt);
        }
    }
    return dp[FULL].first;
}

// ============================================================================
// PART 3: OTHER BITMASK DP PATTERNS (reusable templates)
// ============================================================================

// ---- 3a. Travelling Salesman / Hamiltonian path -----------------------------
// dp[mask][v] = min cost of a path that visits exactly the nodes in mask
//               and ends at v.  Start at node 0, return to 0.
// Same shape counts Hamiltonian paths (CSES 1690 Hamiltonian Flights):
// replace min with += (mod 1e9+7).
// Complexity: O(2^n * n^2), n <= ~20.
ll tsp(int n, const vector<vector<ll>>& cost) {
    const ll INF = 1e18;
    vector<vector<ll>> dp(1 << n, vector<ll>(n, INF));
    dp[1][0] = 0;                                  // only node 0 visited, standing at 0
    for (int mask = 1; mask < (1 << n); mask++)
        for (int v = 0; v < n; v++) {
            if (dp[mask][v] == INF || !(mask >> v & 1)) continue;
            for (int u = 0; u < n; u++) {
                if (mask >> u & 1) continue;       // already visited
                int nm = mask | (1 << u);
                dp[nm][u] = min(dp[nm][u], dp[mask][v] + cost[v][u]);
            }
        }
    ll best = INF;
    for (int v = 1; v < n; v++)                    // close the cycle back to 0
        best = min(best, dp[(1 << n) - 1][v] + cost[v][0]);
    return n == 1 ? 0 : best;
}

// ---- 3b. Iterate over all SUBMASKS of a mask --------------------------------
// Total over all masks = O(3^n)  (fine for n <= ~15).
// Typical use: split a set into groups, dp[mask] = min over sub of dp[mask^sub] + cost[sub].
void submaskExample(int mask) {
    for (int sub = mask; sub > 0; sub = (sub - 1) & mask) {
        // sub is a non-empty submask of mask
        // the rest is mask ^ sub
    }
}

// ---- 3c. Sum over subsets (SOS DP) ------------------------------------------
// f[mask] = sum of a[sub] for every sub that is a subset of mask.  O(2^n * n).
// (CSES 1654 SOS Bit Problem uses this.)
vector<ll> sosDP(int n, vector<ll> f) {
    for (int i = 0; i < n; i++)
        for (int mask = 0; mask < (1 << n); mask++)
            if (mask >> i & 1) f[mask] += f[mask ^ (1 << i)];
    return f;
}

// ---- 3d. Brute force over all subsets ---------------------------------------
// Check every subset of n <= 20 items (e.g. "pick items with total exactly S").
bool subsetSumExists(const vector<ll>& a, ll S) {
    int n = a.size();
    for (int mask = 0; mask < (1 << n); mask++) {
        ll sum = 0;
        for (int i = 0; i < n; i++) if (mask >> i & 1) sum += a[i];
        if (sum == S) return true;
    }
    return false;
}

// ============================================================================
// MAIN: reads the CSES input and prints the answer.
// Run with no input file to see the demos:  ./a < /dev/null
// ============================================================================
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n; ll x;
    if (cin >> n >> x) {                           // CSES judge input
        vector<ll> w(n);
        for (auto& v : w) cin >> v;
        cout << solveElevator(n, x, w) << "\n";
        return 0;
    }

    // ---- demos (when no input is given) ----
    cout << "Elevator example: " << solveElevator(4, 10, {4, 8, 6, 1}) << " (expected 2)\n";
    vector<vector<ll>> c = {{0, 10, 15, 20}, {10, 0, 35, 25}, {15, 35, 0, 30}, {20, 25, 30, 0}};
    cout << "TSP example: " << tsp(4, c) << " (expected 80)\n";
    auto f = sosDP(2, {1, 2, 3, 4});
    cout << "SOS f[3] = " << f[3] << " (expected 10)\n";
    cout << "Subset sum 9 in {3,34,4,12,5,2}: " << subsetSumExists({3, 34, 4, 12, 5, 2}, 9) << " (expected 1)\n";
    return 0;
}
