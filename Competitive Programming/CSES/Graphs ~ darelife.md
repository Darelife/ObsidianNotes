```table-of-contents
title: 
style: nestedList # TOC style (nestedList|nestedOrderedList|inlineFirstLevel)
minLevel: 0 # Include headings from the specified level
maxLevel: 0 # Include headings up to the specified level
include: 
exclude: 
includeLinks: true # Make headings clickable
hideWhenEmpty: false # Hide TOC if no headings are found
debugInConsole: false # Print debug info in Obsidian console
```

# Q1: Counting Rooms

> [!abstract] ### [Counting Rooms](https://cses.fi/problemset/task/1192) 
> **Goal:** Count the number of connected components (rooms) in a grid. **Constraints:** $n, m \le 1000$

Simply count the number of times you had to run DFS.

```cpp
void dfs(pair<int, int> u, pair<int, int> p, vector<vector<int>>& vis, vector<vector<int>>& a) {
  int dx[] = { -1, 1, 0, 0 };
  int dy[] = { 0, 0, -1, 1 };
  int x = u.first, y = u.second;
  vis[x][y] = 1;
  for (int i = 0; i < 4; i++) {
    if (x + dx[i] < 0 || x + dx[i] >= a.size()) continue;
    if (y + dy[i] < 0 || y + dy[i] >= a[0].size()) continue;
    if (x + dx[i] == p.first && y + dy[i] == p.second) continue;
    if (!vis[x + dx[i]][y + dy[i]] && a[x + dx[i]][y + dy[i]]) {
      dfs({ x + dx[i], y + dy[i] }, u, vis, a);
    }
  }
}

void solve() {
  int n, m;
  cin >> n >> m;
  vector<vector<int>> a(n, vector<int>(m));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      char c;
      cin >> c;
      if (c == '#') a[i][j] = 0;
      else a[i][j] = 1;
    }
  }

  int ans = 0;
  vector<vector<int>> vis(n, vector<int>(m));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m;j++) {
      if (!vis[i][j] && a[i][j]) {
        dfs({ i, j }, { -1, -1 }, vis, a);
        ans++;
      }
    }
  }
  cout << ans << endl;
}
```

# Q2: Labyrinth

> [!abstract] ### [Labyrinth](https://cses.fi/problemset/task/1193) 
> **Goal:** Find shortest path from A to B in a grid. 
> **Constraints:** $n, m \le 1000$ 
> **Idea:** BFS + path reconstruction.

1. Just run a simple BFS
2. Store parents
3. Iterate from the last point along the parents, to find the path

```cpp
void solve() {
  int n, m;
  cin >> n >> m;
  vector<vector<int>> a(n, vector<int>(m));
  char c;
  int sx, sy;
  int ex, ey;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      cin >> c;
      if (c == 'A' || c == 'B') {
        // a[i][j] = c - 'A' + 2;
        a[i][j] = -1;
        if (c == 'A') sx = i, sy = j;
        if (c == 'B') ex = i, ey = j;
        continue;
      }
      a[i][j] = (c == '#') ? 0 : 1;
    }
  }

  queue<vector<int>> q;
  vector<vector<int>> vis(n, vector<int>(m));
  vector<vector<char>> p(n, vector<char>(m));
  q.push({ sx, sy });
  int found = 0;
  while (!q.empty()) {
    if (found) break;
    int x = q.front()[0], y = q.front()[1];
    // vis[x][y] = 1;
    q.pop();
    int dx[] = { -1, 1, 0, 0 };
    int dy[] = { 0, 0, -1, 1 };
    for (int i = 0; i < 4; i++) {
      if (x + dx[i] < 0 || dx[i] + x >= n) continue;
      if (y + dy[i] < 0 || dy[i] + y >= m) continue;
      if (vis[x + dx[i]][y + dy[i]]) continue;
      if (i == 0) p[x + dx[i]][y + dy[i]] = 'U';
      else if (i == 1) p[x + dx[i]][y + dy[i]] = 'D';
      else if (i == 2) p[x + dx[i]][y + dy[i]] = 'L';
      else p[x + dx[i]][y + dy[i]] = 'R';

      if (!vis[x + dx[i]][y + dy[i]] && a[x + dx[i]][y + dy[i]] == 1) {
        vis[x + dx[i]][y + dy[i]] = 1;
        q.push({ x + dx[i], y + dy[i] });
      } else if (x + dx[i] == ex && y + dy[i] == ey) {
        found = 1;
      }
    }
  }

  if (!found) {
    cout << "NO" << endl;
    return;
  }

  cout << "YES" << endl;
  string ans;
  int tx = ex, ty = ey;
  while (tx != sx || ty != sy) {
    ans += p[tx][ty];
    if (p[tx][ty] == 'U') tx++;
    else if (p[tx][ty] == 'D') tx--;
    else if (p[tx][ty] == 'L') ty++;
    else ty--;
  }

  cout << ans.size() << endl;
  reverse(ans.begin(), ans.end());
  cout << ans << endl;
}
```

# Q3: Building Roads

> [!abstract] ### [Building Roads](https://cses.fi/problemset/task/1666) 
> **Goal:** Connect all components with minimum roads. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** Count components using DFS, connect them.

1. Use DFS or DSU, to find the number of components
2. For each component, find 1 node
3. Connect all the other components (the node selected from them), to one of the component's nodes, or connect them in a chain. Your wish. Need components - 1 connections

```cpp
class DSU {
public:
  vector<int> parent;
  vector<int> size;
 
  DSU(int n) {
    parent.resize(n);
    size.resize(n, 1);
    for (int i = 0; i < n; i++) {
      parent[i] = i;
    }
  }
 
  int find(int x) {
    if (parent[x] != x) {
      parent[x] = find(parent[x]);
    }
    return parent[x];
  }
 
  void union_sets(int a, int b) {
    a = find(a);
    b = find(b);
    if (a != b) {
      if (size[a] < size[b]) swap(a, b);
      parent[b] = a;
      size[a] += size[b];
    }
  }
};
 
void solve() {
  int n, m;
  cin >> n >> m;
  DSU dsu(n);
  for (int i = 0; i < m; i++) {
    int a, b;
    cin >> a >> b;
    a--; b--;
    dsu.union_sets(a, b);
  }
  vector<int> par;
  for (int i = 0; i < n; i++) {
    if (dsu.parent[i] == i) par.pba(i);
  }
  cout << par.size() - 1 << endl;
  for (int i = 1; i < par.size(); i++) {
    cout << par[i - 1] + 1 << " " << par[i] + 1 << endl;
  }
}
```

# Q4: Message Route

> [!abstract] ### [Message Route](https://cses.fi/problemset/task/1667) 
> **Goal:** Find shortest path from node 1 to n. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** BFS in unweighted graph.

1. No need for dijkstra, as the weight of all is 1
2. Just run a BFS from the starting node, and as soon as you find the destination, stop (use parents like in labyrinth)

```cpp
void solve() {
  int n, m;
  cin >> n >> m;
  vector<vector<int>> adj(n);
  for (int i = 0; i < m; i++) {
    int a, b;
    cin >> a >> b;
    a--; b--;
    adj[a].pba(b);
    adj[b].pba(a);
  }
  queue<int> q;
  vector<int> dist(n, -1);
  vector<bool> vis(n, false);
  vector<int> par(n, -1);
  q.push(0);
  vis[0] = true;
  while (!q.empty()) {
    int u = q.front();
    q.pop();
    for (int v : adj[u]) {
      if (vis[v]) continue;
      vis[v] = true;
      dist[v] = dist[u] + 1;
      q.push(v);
      par[v] = u;
    }
  }
 
  vector<int> ans;
  int v = n - 1;
  while (par[v] != -1) {
    ans.pba(v + 1);
    v = par[v];
  }
  if (dist[n - 1] == -1) {
    cout << "IMPOSSIBLE" << endl;
    return;
  }
  cout << ans.size() + 1 << endl;
  cout << 1 << " ";
  for (int i = ans.size() - 1; i >= 0; i--) {
    cout << ans[i] << " ";
  }
  cout << endl;
}
```

# Q5: Building Teams

> [!abstract] ### [Building Teams](https://cses.fi/problemset/task/1668) 
> **Goal:** Check if graph is bipartite and assign teams. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** BFS/DFS coloring.

1. Try to colour the graph using 2 colours (0 and 1) using dfs (ig also bfs)
2. Assign opposite colours to adj nodes
3. If you ever find same coloured adjacent nodes, it's not possible

# Q6: Round Trip

> [!abstract] ### [Round Trip](https://cses.fi/problemset/task/1669) 
> **Goal:** Find any cycle in an undirected graph. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** DFS with parent tracking.

1. Run DFS and keep track of parent of each node
2. If you visit a node that is already visited and not the parent, a cycle is found
3. Trace back using parents to print the cycle

# Q7: Monsters

> [!abstract] ### [Monsters](https://cses.fi/problemset/task/1194) 
> **Goal:** Escape grid before monsters reach you. 
> **Constraints:** $n, m \le 1000$ 
> **Idea:** Multi-source BFS (monsters) + BFS (player).

1. Run BFS from all the monsters (put all monsters in the queue first)
2. Time to get to next block = time to get to cur block + 1
3. Run another BFS from the player. If for any of the positions on the boundary, if your time < monsters time, you escape (also then trace the path using the parents)

# Q8: Shortest Routes I

> [!abstract] ### [Shortest Routes I](https://cses.fi/problemset/task/1671) 
> **Goal:** Find shortest distances from node 1. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** Dijkstra.

1. Regular PQ. Put (0,0) in it. (curDist, and node)
2. While it's not empty, get the top, and pop it. If curDist > dist[u], ignore
3. Go through all its connected vertices. If dist[v] > dist[u] + w, then, dist[v]=dist[u]+w, and push it in the PQ (dist[v], v)

# Q9: Shortest Routes II

> [!abstract] ### [Shortest Routes II](https://cses.fi/problemset/task/1672) 
> **Goal:** Answer shortest path queries between all pairs. 
> **Constraints:** $n \le 500$ 
> **Idea:** Floyd–Warshall.

1. For all the edges, dist(u,v) = min(dist(u,v), weight of (u,v))
2. dist(i,i) = 0
3. for all k (0, n), for all i (0, n), and for all j (0, n). If dist(i,k) < inf and dist(k, j) < inf, dist(i,j) = min(dist(i, j), dist(i,k) + dist(k,j))

# Q10: High Score

> [!abstract] ### [High Score](https://cses.fi/problemset/task/1673) 
> **Goal:** Maximize score path, detect positive cycles. 
> **Constraints:** $n \le 2500$ 
> **Idea:** Bellman-Ford with cycle detection.

1. Use Bellman-Ford to find maximum distance (invert weights or maximize directly)
2. Run n-1 relaxations, then check for cycles that can still improve the answer
3. If such a cycle can reach node n, answer is -1, otherwise print the distance

# Q11: Flight Discount

> [!abstract] ### [Flight Discount](https://cses.fi/problemset/task/1195) 
> **Goal:** Shortest path with one discounted edge. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** Dijkstra with state (used discount or not).

1. Run Dijkstra but keep 2 states: discount not used (0) and used (1)
2. PQ: Cost, Node, Flag. Keep a dist vector (node × flag (0,1))
3. From state 0, either go normally or use discount (w/2) and move to state 1
4. From state 1, you can only take edges normally
5. Store distances separately for both states and answer is dist(n-1,1)

```cpp
void solve() {
  int n, m;
  cin >> n >> m;
  vector<vector<pair<int, int>>> adj(n);
  for (int i = 0; i < m; i++) {
    int u, v, w;
    cin >> u >> v >> w;
    adj[u - 1].push_back({ v - 1, w });
  }
  vector<vector<int>> dist(n, vector<int>(2, LLONG_MAX));
  dist[0][0] = 0;
  dist[0][1] = 0;
  // cost, node, flag (if we've used the discount or not)
  priority_queue<
    pair<int, pair<int, int>>,
    vector<pair<int, pair<int, int>>>,
    greater<pair<int, pair<int, int>>>
  > pq;
  pq.push({ 0, { 0, 0 } });
  while (!pq.empty()) {
    auto [d, p] = pq.top();
    auto [u, f] = p;
    pq.pop();
    // if we have already found a better path to u
    if ((f == 1 && (dist[u][1] < d)) || (f == 0 && dist[u][0] < d)) continue;
    for (auto e : adj[u]) {
      auto [v, w] = e;
      if (f == 0) {
        // we aren't using the discount rn
        if (dist[v][0] > d + w) {
          dist[v][0] = d + w;
          pq.push({ dist[v][0], {v, 0} });
        }
        // lets use the discount now
        if (dist[v][1] > d + w / 2) {
          dist[v][1] = d + w / 2;
          pq.push({ dist[v][1], {v, 1} });
        }
      } else {
        if (dist[v][1] > d + w) {
          // we have already used the discount in the past
          dist[v][1] = d + w;
          pq.push({ dist[v][1], {v, 1} });
        }
      }
    }
  }
  cout << dist[n - 1][1] << endl;
}
```

# Q12: Cycle Finding

> [!abstract] ### [Cycle Finding](https://cses.fi/problemset/task/1197) 
> **Goal:** Detect and print a negative cycle.
> **Constraints:** $n \le 2500$ 
> **Idea:** Bellman-Ford.

```cpp
void solve() {
  int n, m;
  cin >> n >> m;
  vector<tuple<int, int, int>> edges;
  for (int i = 0; i < m; i++) {
    int a, b, c;
    cin >> a >> b >> c;
    edges.push_back({ a, b, c });
  }
  vector<int> dist(n + 1, 0);
  vector<int> parent(n + 1, -1);
  int x = -1;
  for (int i = 0; i < n; i++) {
    x = -1;
    for (auto [a, b, c] : edges) {
      if (dist[b] > dist[a] + c) {
        dist[b] = dist[a] + c;
        parent[b] = a;
        x = b;
      }
    }
  }
  if (x == -1) {
    cout << "NO" << endl;
  } else {
    cout << "YES" << endl;
    // find cycle
    int y = x;
    for (int i = 0; i < n; i++) y = parent[y];
    vector<int> cycle;
    for (int cur = y; ; cur = parent[cur]) {
      cycle.push_back(cur);
      if (cur == y && cycle.size() > 1) break;
    }
    reverse(cycle.begin(), cycle.end());
    for (int v : cycle) cout << v << " ";
    cout << endl;
  }
}
```

# Q13: Flight Routes

> [!abstract] ### [Flight Routes](https://cses.fi/problemset/task/1196) 
> **Goal:** Find k shortest paths. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** Modified Dijkstra (store k distances).

1. Use Dijkstra but store up to k distances for each node
2. Instead of one dist array, keep a list (or max-heap) of k shortest distances per node
3. Continue pushing states into PQ even if node is visited before (up to k times)
4. The k smallest distances to node n are the answer

```cpp
void solve() {
  int n, m, k;
  cin >> n >> m >> k;
  vector<vector<pair<int, int>>> adj(n);
  for (int i = 0; i < m; i++) {
    int u, v, w;
    cin >> u >> v >> w;
    adj[u - 1].push_back({ v - 1, w });
  }
  // cost, node
  priority_queue<
    pair<int, int>,
    vector<pair<int, int>>,
    greater<pair<int, int>>
  > pq;
  // for each node, a pq to get the best k
  vector<priority_queue<int>> best(n);
  pq.push({ 0, 0 });
  best[0].push(0);
  while (!pq.empty()) {
    auto [cost, node] = pq.top();
    pq.pop();
    if (best[node].size() == k && cost > best[node].top()) continue;
    for (auto it : adj[node]) {
      if (best[it.first].size() < k) {
        best[it.first].push(cost + it.second);
        pq.push({ cost + it.second, it.first });
      } else if (best[it.first].top() > cost + it.second) {
        best[it.first].pop();
        best[it.first].push({ cost + it.second });
        pq.push({ cost + it.second, it.first });
      }
    }
  }
  vector<int> ans;
  for (int i = 0; i < k; i++) {
    ans.push_back(best[n - 1].top());
    best[n - 1].pop();
  }
  for (int i = k - 1; i >= 0; i--) {
    cout << ans[i] << " ";
  }
  cout << endl;
}
```

# Q14: Round Trip II

> [!abstract] ### [Round Trip II](https://cses.fi/problemset/task/1678) 
> **Goal:** Find a cycle in a directed graph. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** DFS with recursion stack.

```cpp
int dfs(int u, int p, vector<vector<int>>& adj, vector<int>& vis, vector<int>& path) {
  vis[u] = 1;
  for (auto v : adj[u]) {
    path.pba(v);
    if (!vis[v]) {
      if (dfs(v, u, adj, vis, path)) {
        return 1;
      }
    } else if (vis[v] == 1) return 1;
    path.pop_back();
  }
  vis[u] = 2;
  return 0;
}

void solve() {
  int n, m;
  cin >> n >> m;
  vector<vector<int>> adj(n);
  for (int i = 0; i < m; i++) {
    int a, b;
    cin >> a >> b;
    a--; b--;
    adj[a].pba(b);
  }
  vint path;
  vint vis(n, 0);
  for (int i = 0; i < n; i++) {
    path.pba(i);
    if (!vis[i] && dfs(i, -1, adj, vis, path)) {
      reverse(path.begin(), path.end());
      int len = 1;
      for (int i = 1; i < path.size(); i++) {
        if (path[i] != path[0]) len++;
        else break;
      }
      cout << len + 1 << endl;
      vector<int> ans(path.begin(), path.begin() + len + 1);
      reverse(ans.begin(), ans.end());
      for (int i = 0; i <= len; i++) {
        cout << ans[i] + 1 << " ";
      }
      return;
    }
    path.pop_back();
  }
  cout << "IMPOSSIBLE";
}
```

# Q15: Course Schedule

> [!abstract] ### [Course Schedule](https://cses.fi/problemset/task/1679) 
> **Goal:** Find topological ordering. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** Kahn's algorithm / DFS topo sort.

```cpp
void solve() {
  int n, m;
  cin >> n >> m;
  vector<vector<int>> adj(n);
  vector<int> indeg(n, 0);
  for (int i = 0; i < m; i++) {
    int a, b;
    cin >> a >> b;
    a--; b--;
    adj[a].pba(b);
    indeg[b]++;
  }
  queue<int> q;
  int len = 0;
  for (int i = 0; i < n; i++) {
    if (indeg[i] == 0) q.push(i);
  }
  vector<int> ans;
  while (!q.empty()) {
    int u = q.front();
    q.pop();
    len++;
    ans.pba(u);
    for (auto v : adj[u]) {
      indeg[v]--;
      if (indeg[v] == 0) {
        q.push(v);
      }
    }
  }
  if (len != n) {
    cout << "IMPOSSIBLE" << endl;
    return;
  }
  vpin(ans);
}
```

# Q16: Longest Flight Route

> [!abstract] ### [Longest Flight Route](https://cses.fi/problemset/task/1680) 
> **Goal:** Longest path in DAG from 1 to n. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** DP on DAG + topo order.

```cpp
void solve() {
  int n, m;
  cin >> n >> m;
  vector<vector<int>> adj(n);
  vector<int> indeg(n, 0);
  for (int i = 0; i < m; i++) {
    int a, b;
    cin >> a >> b;
    a--; b--;
    adj[a].pba(b);
    indeg[b]++;
  }
  queue<int> q;
  for (int i = 0; i < n; i++) {
    if (indeg[i] == 0) q.push(i);
  }
  vector<int> topo;
  while (!q.empty()) {
    int u = q.front(); q.pop();
    topo.push_back(u);
    for (int v : adj[u]) {
      indeg[v]--;
      if (indeg[v] == 0) q.push(v);
    }
  }
  vector<int> dist(n, -1);
  vector<int> par(n, -1);
  dist[0] = 1;
  for (int u : topo) {
    if (dist[u] == -1) continue;
    for (int v : adj[u]) {
      if (dist[u] + 1 > dist[v]) {
        dist[v] = dist[u] + 1;
        par[v] = u;
      }
    }
  }
  if (dist[n - 1] == -1) {
    cout << "IMPOSSIBLE" << endl;
    return;
  }
  vector<int> path;
  int t = n - 1;
  while (t != -1) {
    path.push_back(t + 1);
    t = par[t];
  }
  reverse(path.begin(), path.end());
  cout << path.size() << endl;
  for (int x : path) cout << x << " ";
  cout << endl;
}
```

# Q17: Game Routes

> [!abstract] ### [Game Routes](https://cses.fi/problemset/task/1681) 
> **Goal:** Count number of paths in DAG. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** DP on DAG.

```cpp
void solve() {
  int n, m;
  cin >> n >> m;
  vector<vector<int>> adj(n);
  vector<int> indeg(n);
  for (int i = 0; i < m; i++) {
    int a, b;
    cin >> a >> b;
    a--; b--;
    adj[a].pba(b);
    indeg[b]++;
  }
  vector<int> topo;
  queue<int> q;
  for (int i = 0; i < n; i++) {
    if (indeg[i] == 0) q.push(i);
  }
  while (!q.empty()) {
    int u = q.front();
    topo.pba(u);
    q.pop();
    for (auto v : adj[u]) {
      if (--indeg[v] == 0) {
        q.push(v);
      }
    }
  }
  vector<int> dp(n);
  dp[0] = 1;
  for (int u : topo) {
    for (int v : adj[u]) {
      dp[v] = (dp[v] + dp[u]) % MOD;
    }
  }
  cout << dp[n - 1] << endl;
}
```

# Q18: Investigation

> [!abstract] ### [Investigation](https://cses.fi/problemset/task/1202) 
> **Goal:** Shortest path + count ways + min/max edges. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** Dijkstra + extra state tracking.

```cpp
v<v<pair<int, int>>> adj;
vector<int> dist, ways, minn, maxx, vis;

void dijkstra(int u) {
  priority_queue<
    pair<int, int>,
    vector<pair<int, int>>,
    greater<>
  > pq;
  dist[u] = 0;
  minn[u] = 0;
  maxx[u] = 0;
  // dist, u
  pq.push({ 0, u });
  ways[u] = 1;
  while (!pq.empty()) {
    auto [d, u] = pq.top();
    pq.pop();
    if (d > dist[u]) continue;
    for (auto [v, dd] : adj[u]) {
      int t = dd + dist[u];
      if (t == dist[v]) {
        ways[v] = (ways[v] + ways[u]) % MOD;
        minn[v] = min(minn[v], minn[u] + 1);
        maxx[v] = max(maxx[v], maxx[u] + 1);
      } else if (t < dist[v]) {
        ways[v] = ways[u];
        minn[v] = minn[u] + 1;
        maxx[v] = maxx[u] + 1;
        dist[v] = t;
        pq.push({ t, v });
      }
    }
  }
}

void solve() {
  int n, m;
  cin >> n >> m;
  adj.resize(n);
  dist.resize(n, inf);
  ways.resize(n);
  minn.resize(n, inf);
  maxx.resize(n);
  vis.resize(n);
  for (int i = 0; i < m; i++) {
    int a, b, c;
    cin >> a >> b >> c;
    a--; b--;
    adj[a].pba({ b, c });
  }
  dijkstra(0);
  cout << dist[n - 1] << " " << ways[n - 1] << " " <<
    minn[n - 1] << " " << maxx[n - 1] << endl;
}
```

# Q19: Planets Queries I

> [!abstract] ### [Planets Queries I](https://cses.fi/problemset/task/1750) 
> **Goal:** Find k-th successor in functional graph. 
> **Constraints:** $n \le 2 \times 10^5$ 
> **Idea:** Binary lifting.

# Q20: Planets Queries II

> [!abstract] ### [Planets Queries II](https://cses.fi/problemset/task/1160) 
> **Goal:** Distance queries in functional graph. 
> **Constraints:** $n \le 2 \times 10^5$ 
> **Idea:** Binary lifting + cycle handling.

# Q21: Planets Cycles

> [!abstract] ### [Planets Cycles](https://cses.fi/problemset/task/1751) 
> **Goal:** Find cycle length for each node. 
> **Constraints:** $n \le 2 \times 10^5$ 
> **Idea:** Functional graph + DFS.

```cpp
int steps = 0;
vector<int> a, vis, cnt;
queue<int> path;

void dfs(int u) {
  path.push(u);
  if (vis[u]) {
    steps += cnt[u];
    return;
  }
  steps++;
  vis[u] = 1;
  dfs(a[u]);
}

void solve() {
  int n;
  cin >> n;
  a.resize(n);
  vcin(a, n);
  for (int& i : a) i--;
  vis.resize(n);
  cnt.resize(n);
  for (int i = 0; i < n; i++) {
    if (!vis[i]) {
      steps = 0;
      dfs(i);
      int flag = 1;
      while (!path.empty()) {
        if (path.front() == path.back()) flag = 0;
        cnt[path.front()] = steps;
        steps -= flag;
        path.pop();
      }
    }
  }
  vpin(cnt);
}
```

# Q22: Road Reparation

> [!abstract] ### [Road Reparation](https://cses.fi/problemset/task/1675) 
> **Goal:** Find MST cost. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** Kruskal / Prim.

# Q23: Road Construction

> [!abstract] ### [Road Construction](https://cses.fi/problemset/task/1676) 
> **Goal:** Track components after adding edges. 
> **Constraints:** $n \le 10^5$ 
> **Idea:** DSU.

```cpp
class DisjointSet {
public:
  vector<int> parent, size;
  int maxSize;
  DisjointSet(int n) {
    parent.resize(n + 1, 1);
    size.resize(n + 1, 1);
    maxSize = 1;
    for (int i = 0; i <= n; i++) {
      parent[i] = i;
    }
  }
  int findUPar(int x) {
    if (x != parent[x]) {
      parent[x] = findUPar(parent[x]);
    }
    return parent[x];
  }
  bool UnionBySize(int x, int y) {
    int rootX = parent[x];
    int rootY = parent[y];
    if (rootX == rootY) return false;
    if (size[rootX] < size[rootY]) {
      swap(rootX, rootY);
    }
    parent[rootY] = rootX;
    size[rootX] += size[rootY];
    maxSize = max(maxSize, size[rootX]);
    return true;
  }
};

void solve() {
  int n, m;
  cin >> n >> m;
  vector<vector<int>> edges;
  for (int i = 0; i < m; i++) {
    int u, v;
    cin >> u >> v;
    edges.push_back({ u, v });
  }
  int connected = n;
  DisjointSet ds(n);
  for (int i = 0; i < m; i++) {
    int u = edges[i][0];
    int v = edges[i][1];
    if (ds.findUPar(u) != ds.findUPar(v)) {
      connected--;
      bool flag = ds.UnionBySize(u, v);
    }
    cout << connected << " " << ds.maxSize << endl;
  }
}
```

> [!abstract] ### [Flight Routes Check](https://cses.fi/problemset/task/1682)  
> **Goal:** Check strong connectivity  
> **Constraints:** $n \le 10^5$  
> **Idea:** Kosaraju / SCC

> [!abstract] ### [Planets and Kingdoms](https://cses.fi/problemset/task/1683)  
> **Goal:** Find SCCs  
> **Constraints:** $n \le 10^5$  
> **Idea:** Kosaraju / Tarjan

> [!abstract] ### [Giant Pizza](https://cses.fi/problemset/task/1684)  
> **Goal:** Solve 2-SAT problem  
> **Constraints:** $n \le 10^5$  
> **Idea:** Implication graph + SCC

> [!abstract] ### [Coin Collector](https://cses.fi/problemset/task/1686)  
> **Goal:** Max coins collectable in graph  
> **Constraints:** $n \le 10^5$  
> **Idea:** SCC + DP on DAG

> [!abstract] ### [Mail Delivery](https://cses.fi/problemset/task/1691)  
> **Goal:** Find Eulerian path in undirected graph  
> **Constraints:** $n \le 10^5$  
> **Idea:** Hierholzer's algorithm

> [!abstract] ### [De Bruijn Sequence](https://cses.fi/problemset/task/1692)  
> **Goal:** Construct binary De Bruijn sequence  
> **Constraints:** $n \le 15$  
> **Idea:** Eulerian cycle in de Bruijn graph

> [!abstract] ### [Teleporters Path](https://cses.fi/problemset/task/1693)  
> **Goal:** Find Eulerian path in directed graph  
> **Constraints:** $n \le 10^5$  
> **Idea:** Hierholzer's algorithm

> [!abstract] ### [Hamiltonian Flights](https://cses.fi/problemset/task/1690)  
> **Goal:** Count Hamiltonian paths from 1 to n  
> **Constraints:** $n \le 20$  
> **Idea:** Bitmask DP

> [!abstract] ### [Knight's Tour](https://cses.fi/problemset/task/1689)  
> **Goal:** Visit all squares exactly once  
> **Constraints:** $8 \times 8$ board  
> **Idea:** Backtracking + heuristics

> [!abstract] ### [Download Speed](https://cses.fi/problemset/task/1694)  
> **Goal:** Find max flow  
> **Constraints:** $n \le 500$  
> **Idea:** Dinic / Edmonds-Karp

> [!abstract] ### [Police Chase](https://cses.fi/problemset/task/1695)  
> **Goal:** Find min cut edges  
> **Constraints:** $n \le 500$  
> **Idea:** Max flow + min cut

> [!abstract] ### [School Dance](https://cses.fi/problemset/task/1696)  
> **Goal:** Maximum bipartite matching  
> **Constraints:** $n \le 500$  
> **Idea:** Hopcroft–Karp / flow

> [!abstract] ### [Distinct Routes](https://cses.fi/problemset/task/1711)  
> **Goal:** Find k edge-disjoint paths  
> **Constraints:** $n \le 500$  
> **Idea:** Max flow + path reconstruction