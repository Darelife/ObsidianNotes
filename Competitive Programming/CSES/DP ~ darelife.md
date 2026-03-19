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

# Q1: Dice Combinations
> [!abstract] ### [Dice Combinations](https://cses.fi/problemset/task/1633)
> **Goal:** Count the number of ways to construct a sum $n$ by throwing a die one or more times.
> **Constraints:** $1 \le n \le 10^6$ | Time: 1.00s | Memory: 512MB

```cpp
void solve() {
  int n;
  cin >> n;
  vint dp(n + 1, 0);
  dp[0] = 1;
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= 6; j++) {
      if (i - j >= 0) {
        dp[i] = (dp[i] + dp[i - j]) % MOD;
      }
    }
  }
  cout << dp[n] << endl;
}
```

# Q2: Minimizing Coins
> [!abstract] ### [Minimizing Coins](https://cses.fi/problemset/task/1634)
> **Goal:** Find the minimum number of coins needed to produce a sum $x$.
> **Constraints:** $n \le 100$ coins, target sum $x \le 10^6$
> **Example:** Coins $\{1, 5, 7\}$ for sum $11 \implies 5+5+1$ (3 coins).

```cpp
void solve() {
  int v, n;
  cin >> v >> n;
  vint a(v);
  vcin(a, v);
  vint dp(n + 1, inf);
  dp[0] = 0;
  for (int i = 1; i <= n; i++) {
    for (int j = 0; j < v; j++) {
      if (i >= a[j]) {
        dp[i] = min(dp[i], dp[i - a[j]] + 1);
      }
    }
  }
  if (dp[n] == inf || dp[n] <= 0) {
    cout << -1 << endl;
    return;
  }
  cout << dp[n] << endl;
}
```

# Q3: Coin Combinations I
> [!abstract] ### [Coin Combinations I](https://cses.fi/problemset/task/1635) 
> **Goal:** Count the number of ways to produce a sum x using available coins (order matters). 
> **Constraints:** n≤100 coins, target sum x≤$10^6$ 
> **Example:** Coins {2,3,5} for sum 9⟹ 8 ways (e.g., 2+2+5,5+2+2,3+3+3, etc.)

```cpp
void solve() {
  int v, n;
  cin >> v >> n;
  vint coins(v);
  vcin(coins, v);
  vint dp(n + 1, 0);
  dp[0] = 1;
  for (int i = 1; i <= n; i++) {
    for (int j = 0; j < v; j++) {
      if (i - coins[j] >= 0) {
        dp[i] += dp[i - coins[j]];
        dp[i] %= 1000000007;
      }
    }
  }
  cout << dp[n] << endl;
}
```

# Q4: Coin Combinations II
> [!abstract] ### [Coin Combinations II](https://cses.fi/problemset/task/1636)
> **Goal:** Count the number of ways to produce a sum x using available coins (order **does not** matter).
> **Constraints:** n≤100 coins x≤$10^6$ target sum
> **Example:** Coins {2,3,5} for sum 9⟹3 ways. {2,2,5}, {2,2,2,3}, {3,3,3}.

Do it for each coin in order first now. That will ensure that you use all the coins in order. (exchange the loops)

```cpp
void solve() { 
  int n, x;
  cin >> n >> x;
  vint dp(x + 1, 0);
  vint coins(n);
  vcin(coins, n);
  dp[0] = 1;
  for (int i = 0; i < n; i++) {
    int coin = coins[i];
    for (int j = 0; j <= x; j++) {
      if (j - coin >= 0) {
        dp[j] = (dp[j] + dp[j - coin]) % MOD;
      }
    }
  }
  cout << dp[x] << endl;
}
```

# Q5: Removing Digits
> [!abstract] ### [Removing Digits](https://cses.fi/problemset/task/1637) 
> **Goal:** Given an integer nnn, remove one of its digits at each step. Find the **minimum number of steps** to make it 0. 
> **Constraints:** 1≤n≤1061 \le n \le 10^61≤n≤106 
> **Idea:** At each step, subtract any non-zero digit of the current number.

```cpp
void solve() {
  int n;
  cin >> n;
  vint dp(n + 1, 1e6 + 5);
  // dp[i] = minimum number of steps to reach 0 from i
  // dp[0] = 0 as we are already at 0
  // for all i from 1 to n, for each digit d, dp[i] = min(dp[i], dp[i - d] + 1)
  // return dp[n]
  dp[0] = 0;
  for (int i = 1; i <= n; i++) {
    for (char c : to_string(i)) {
      if (c - '0' <= i)
        dp[i] = min(dp[i], dp[i - (c - '0')] + 1);
    }
  }
  cout << dp[n] << endl;
}
```

# Q6: Grid Paths I
> [!abstract] ### [Grid Paths I](https://cses.fi/problemset/task/1638)
> **Goal:** Count the number of ways to move from the top-left to bottom-right of a grid with obstacles.
> **Constraints:** n ≤ 1000
> **Idea:** Classic grid DP; dp[i][j] = ways from top or left if cell is not blocked.

```cpp
void solve() {
  int n;
  cin >> n;
  vector<string> grid(n);
  forr(i, n) cin >> grid[i];
  vector<vector<int>> dp(n, vector<int>(n, 0));
  dp[0][0] = grid[0][0] == '.' ? 1 : 0;
  if (grid[0][0] == '*') {
    cout << 0 << endl;
    return;
  }
  forr(i, n) {
    forr(j, n) {
      if (grid[i][j] == '*') {
        dp[i][j] = 0;
        continue;
      }
      if (i > 0) {
        dp[i][j] += dp[i - 1][j];
      }
      if (j > 0) {
        dp[i][j] += dp[i][j - 1];
      }
      dp[i][j] %= 1000000007;
    }
  }
  cout << dp[n - 1][n - 1] << endl;
}
```

# Q7: Book Shop
> [!abstract] ### [Book Shop](https://cses.fi/problemset/task/1158)
> **Goal:** Maximize total pages with a limited budget.
> **Constraints:** n ≤ 1000, x ≤ 10^5
> **Idea:** 0/1 Knapsack DP.

```cpp
void solve() {
  int n, x;
  cin >> n >> x;
  vint cost(n), pages(n);
  vcin(cost, n);
  vcin(pages, n);
  vint dp(x + 1, 0);
  // dp[i] = max pages that can be bought with i money
  for (int i = 0; i < n; i++) {
    for (int j = x; j >= cost[i]; j--) {
      dp[j] = max(dp[j], dp[j - cost[i]] + pages[i]);
    }
  }
  cout << dp[x] << endl;
}
```

# Q8: Array Description
> [!abstract] ### [Array Description](https://cses.fi/problemset/task/1746)
> **Goal:** Count arrays satisfying adjacent difference ≤ 1 with some fixed values.
> **Constraints:** n ≤ 10^5, values ≤ 100
> **Idea:** DP over index and value.

```cpp
void solve() {
  int n, m;
  cin >> n >> m;
  vint a(n);
  vcin(a, n);
  vector<vint> dp(n, vint(m + 1, 0));
  if (a[0] == 0) {
    for (int j = 1; j <= m; j++) {
      dp[0][j] = 1;
    }
  } else dp[0][a[0]] = 1;
  for (int i = 1; i < n; i++) {
    if (a[i]) {
      int j = a[i];
      dp[i][j] = (dp[i - 1][j]) % MOD;
      if (j > 1) dp[i][j] = (dp[i - 1][j - 1] + dp[i][j]) % MOD;
      if (j < m) dp[i][j] = (dp[i - 1][j + 1] + dp[i][j]) % MOD;
    } else {
      for (int j = 1; j <= m; j++) {
        dp[i][j] = (dp[i - 1][j]) % MOD;
        if (j > 1) dp[i][j] = (dp[i - 1][j - 1] + dp[i][j]) % MOD;
        if (j < m) dp[i][j] = (dp[i - 1][j + 1] + dp[i][j]) % MOD;
      }
    }
  }
  int ans = 0;
  for (int i = 1; i <= m; i++) {
    ans = (ans + dp[n - 1][i]) % MOD;
  }
  cout << ans << endl;
}
```

# Q9: Counting Towers
> [!abstract] ### [Counting Towers](https://cses.fi/problemset/task/2413)
> **Goal:** Count ways to build towers using 2×n blocks.
> **Constraints:** n ≤ 10^6
> **Idea:** DP with state transitions for full and partial fillings.

dp(i,0) → Number of ways to build from column i to n when the current column is fully filled (aligned)

dp(i,1) → Number of ways when the current column is split/misaligned

```cpp
int const MOD = 1e9 + 7;
vector<vint> dp(1e6 + 1, vint(2));
void solve() {
  int n;
  cin >> n;
  dp[n][0] = 1;
  dp[n][1] = 1;
  for (int i = n - 1; i >= 0; i--) {
    dp[i][0] = (2 * dp[i + 1][0] + dp[i + 1][1]) % MOD;
    dp[i][1] = (dp[i + 1][0] + 4 * dp[i + 1][1]) % MOD;
  }
  cout << (dp[1][0] + dp[1][1]) % MOD << endl;
}
```

# Q10: Edit Distance
> [!abstract] ### [Edit Distance](https://cses.fi/problemset/task/1639)
> **Goal:** Find minimum operations to convert one string to another.
> **Constraints:** length ≤ 5000
> **Idea:** Classic DP with insert, delete, replace.

3 cases...insert, delete, and replace
for the 2 strings, (m,n) length.

if i == 0, then, you have to insert j times...so like, dp(i,j) = j if (i == 0)
if j == 0, then, the same thing but for i.
if the last char is same, then, you don't have to do anything
otherwise, it's 1 + min cost for insert or deletion or replace like thing

dp(i,j) = min operations to convert a[0..i-1] to b[0..j-1]


```cpp
void solve() {
  string a, b;
  cin >> a >> b;
  int n = a.size(), m = b.size();
  vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));
  dp[0][0] = 0;
  // dp[i][j] = min operations to convert a[0..i-1] to b[0..j-1]
  for (int i = 1; i <= n; i++) dp[i][0] = i;
  for (int i = 1; i <= m; i++) dp[0][i] = i;
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= m; j++) {
      if (a[i - 1] == b[j - 1]) dp[i][j] = dp[i - 1][j - 1];
      else dp[i][j] = 1 + min(dp[i- 1][j - 1], min(dp[i - 1][j], dp[i][j - 1]));
    }
  }
  cout << dp[n][m] << endl;
}
```

# Q11: LCS
> [!abstract] ### [Longest Common Subsequence](https://cses.fi/problemset/task/1143)
> **Goal:** Find the length of the longest common subsequence of two strings.
> **Constraints:** length ≤ 1000
> **Idea:** Standard 2D DP.

dp(i, j) = max len of common subsequence
like, then, 
if a[i] == b[j], then, dp(i, j) = 1 + dp(i-1, j-1)
else dp(i, j) = max(dp(i-1, j), dp(i, j-1))

```cpp
void solve() {
  int n, m;
  cin >> n >> m;
 
  vint a(n), b(m);
  vcin(a, n); vcin(b, m);
 
  vector<vector<pair<int, vector<int>>>> dp(n + 1, vector<pair<int, vector<int>>>(m + 1, { 0, {} }));
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= m; j++) {
      if (a[i - 1] == b[j - 1]) {
        dp[i][j].first = dp[i - 1][j - 1].first + 1;
        dp[i][j].second = dp[i - 1][j - 1].second;
        // dp[i][j].second.push_back(a[i - 1]); removed this to decrease the space usage... gonna calculate this in the end
      } else {
        // dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
        if (dp[i - 1][j].first > dp[i][j - 1].first) {
          dp[i][j] = dp[i - 1][j];
        } else {
          dp[i][j] = dp[i][j - 1];
        }
      }
    }
  }
 
  cout << dp[n][m].first << endl;
  // vpin(dp[n][m].second);
  stack<int> s;
  int i = n, j = m;
  while (i && j) {
    if (a[i - 1] == b[j - 1]) {
      i--, j--;
      s.push(a[i]);
    } else if (dp[i - 1][j] > dp[i][j - 1]) {
      i--;
    } else {
      j--;
    }
  }
  while (!s.empty()) {
    cout << s.top() << " ";
    s.pop();
  }
  cout << endl;
}
```

# Q12: Rectangle Cutting
> [!abstract] ### [Rectangle Cutting](https://cses.fi/problemset/task/1744)
> **Goal:** Minimum cuts to divide rectangle into squares.
> **Constraints:** a, b ≤ 500
> **Idea:** DP over dimensions, try all cuts.

dp(i, j) = min cuts for a rectangle sized $i \times j$
Then, if (i < j) dp(i, j) = 1 + dp(i, j-i)
else if (i > j) dp(i, j) = 1 + dp(i-j, j)
else dp(i, j) = 0

this was my idea, but apparently, we've to iterate for all the possible cuts using a loop both along the width and height.

```cpp
void solve() {
  int a, b;
  cin >> a >> b;
  // vector<vint> dp(a + 1, vint(b + 1, 0));
  vector<vint> dp(a + 1, vint(b + 1, inf));
  for (int i = 1; i <= a; i++) {
    for (int j = 1; j <= b; j++) {
      // dp[i][j] = 1;
      // if (i > j) dp[i][j] += dp[i - j][j];
      // else dp[i][j] += dp[i][j - i];
      if (i == j) dp[i][j] = 0;
      else {
        for (int k = 1; k < i; k++) {
          dp[i][j] = min(dp[i][j], dp[k][j] + dp[i - k][j] + 1);
        }
        for (int k = 1; k < j; k++) {
          dp[i][j] = min(dp[i][j], dp[i][k] + dp[i][j - k] + 1);
        }
      }
    }
  }
  cout << dp[a][b] << endl;
}
```

# Q13: Minimal Grid Path
> [!abstract] ### [Minimal Grid Path](https://cses.fi/problemset/task/1638)
> **Goal:** Find minimum cost path in a grid.
> **Constraints:** n ≤ 1000
> **Idea:** DP on grid (actually sort of greedy)

just iterating on every possible position that I can be, after `i` steps....via `next` and `curPos`

```cpp
void solve() {
  int n;
  cin >> n;
  vector<string> grid(n);
  for (int i = 0; i < n; i++) {
    cin >> grid[i];
  }
  string ans = "";
  ans += grid[0][0];
 
  vector<pair<int, int>> curPos;
  curPos.pba({ 0, 0 });
 
  int dx[2] = { 0, 1 };
  int dy[2] = { 1, 0 };
 
  vector<vector<bool>> visited(n, vector<bool>(n, false));
  visited[0][0] = true;
 
 
 
  for (int i = 1; i < 2 * n - 1; i++) {
    vector<pair<int, int>> next;
    char minn = 'Z';
 
    for (auto [x, y] : curPos) {
      for (int dir = 0; dir < 2; ++dir) {
        int nx = x + dx[dir];
        int ny = y + dy[dir];
 
        if (nx < n && ny < n && !visited[nx][ny]) {
          visited[nx][ny] = true;
          char c = grid[nx][ny];
          if (c < minn) {
            minn = c;
            next.clear();
            next.pba({ nx, ny });
          } else if (c == minn) {
            next.pba({ nx, ny });
          }
        }
      }
    }
    ans += minn;
    curPos = next;
  }
  cout << ans << endl;
}
```

# Q14: Money Sums
> [!abstract] ### [Money Sums](https://cses.fi/problemset/task/1745)
> **Goal:** Find all possible sums from given coins.
> **Constraints:** n ≤ 100, sum ≤ 10^5
> **Idea:** Subset sum DP.

dp(i,j) = true if we can make sum j using first i coins
dp(i,j) = dp(i - 1,j) || dp(i - 1,j - a[i])

literally just like the ways question....just that here we've to ensure that we use each coin only once.

```cpp
 
void solve() {
  int n;
  cin >> n;
  vint a(n);
  vcin(a, n);
  vector<vector<bool>> dp(n + 1, vector<bool>(100001, 0));
  dp[0][0] = 1;
 
  for (int i = 1; i <= n; i++) {
    for (int j = 0; j <= 100000; j++) {
      dp[i][j] = dp[i - 1][j]; 
      if (j - a[i - 1] >= 0) {
        dp[i][j] = dp[i][j] || dp[i - 1][j - a[i - 1]];
        // If we're using coin i, then, we're only considering the dp's till coin i-1...and not the past of the things that we've processed for coin i till now. Hence, here we're ensuring that we use each coin only once
      }
    }
  }
  cout << accumulate(dp[n].begin(), dp[n].end(), 0) - 1 << endl;
  for (int i = 1; i <= 100000; i++) {
    if (dp[n][i]) {
      cout << i << " ";
    }
  }
  cout << endl;
}
```

# Q15: Removal Game
> [!abstract] ### [Removal Game](https://cses.fi/problemset/task/1097)
> **Goal:** Maximize score assuming optimal play from both ends.
> **Constraints:** n ≤ 5000
> **Idea:** DP on intervals.

I believe for the states, we just need the left and right boundaries. Thought of the turn number or something, but in the end, no matter the user, we just want the final ans assuming any particular user, with a given `l` and `r`.

dp(l, r) = max(dp(l+1,r), dp(l, r-1))
dp(i,i) = $a_{i}$

So like, the appropriate way to go ahead with this intuition, is to make the value stored in the DP the difference between the 2 players, instead of the max we can get from that range or something of that sort.

dp(l, r) = player1 - player2 for that range

if we pick a[l], then, from the opponents perspective, the best diff would be dp(l+1,r)...So, from our side, it would be -dp(l+1,r)...and since we gain a[l]...the difference between the values = a[l] - dp(l+1,r). Same thing for a[r]

Since dp(i,i) = a[i], we'll have to make the initial loop on the length of the range, as we'll iterate over it.

```cpp
void solve() {
    int n;
    cin >> n;
    vint a(n);
    vcin(a,n);
    
    vector<vector<int>> dp(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        dp[i][i] = a[i];
    }
    
    for (int len = 2; len <= n; len++) {
        for (int l = 0; l + len - 1 < n; l++) {
            int r = l + len - 1;
            
            dp[l][r] = max(
                a[l] - dp[l+1][r],
                a[r] - dp[l][r-1]
            );
        }
    }
    
    int total = accumulate(a.begin(), a.end(), 0LL);
    int ans = (total + dp[0][n-1]) / 2;
    cout << ans << endl;
}
```


# Q16: Two Sets II
> [!abstract] ### [Two Sets II](https://cses.fi/problemset/task/1093)
> **Goal:** Count ways to divide numbers 1..n into two sets with equal sum.
> **Constraints:** n ≤ 500
> **Idea:** Subset sum DP with symmetry handling.

Basically now, we'll have to store the ways to get sum/2...just like regular subset sum only...

2d dp. one val for the index range that we'll consider, and the other for the target. We'll store the number of ways, instead of a bool for whether its possible or not.

So, that's dp(i,j). Now, 
dp(i, j) = dp(i-1, j) + dp(i-1, j-a[i])...just realized that we don't have any array...so, a[i] = i in this q. Hence, just replace that.
also yeah, dp(0,0) = 1 so that later on we can keep adding to it.

```cpp
void solve() {
  int n;
  cin >> n;
  int x = n * (n + 1) / 2;
  if (x % 2 != 0) {
    cout << 0 << endl;
    return;
  }
  x /= 2;
  vector<vector<int>> dp(n + 1, vector<int>(x + 1, 0));
  dp[0][0] = 1;
  for (int i = 1; i <= n; i++) {
    for (int j = 0; j <= x; j++) {
      dp[i][j] = (j - i >= 0) ? dp[i - 1][j] + dp[i - 1][j - i] : dp[i - 1][j];
      dp[i][j] = (dp[i][j]) % MOD;
    }
  }
  debug(dp);
  cout << dp[n - 1][x];
}
 
```

# Q17: Mountain Range
> [!abstract] ### [Mountain Range](https://cses.fi/problemset/task/1748)
> **Goal:** Count valid mountain sequences.
> **Constraints:** n ≤ 10^6
> **Idea:** Catalan-like DP.

I've no idea how to solve this question. I've tried it like, 3-4 times, and I've even read the solution.

Ek baar bas try karunga khudse karne ka though firse....nahi hua, toh lite!!! FUCK THIS QUESTION!!!

Okay...so, i just read a solution for it. They made a Binary Tree basically. And in that, they ensured that the largest element is the root. For each side from the root, find the largest element for that segment...and keep going. 
Then, ans = depth of the tree + 1. Nvm, ig this is also wrong. Lite.

# Q18
> [!abstract] ### [Increasing Subsequence](https://cses.fi/problemset/task/1145)
> **Goal:** Find length of longest increasing subsequence.
> **Constraints:** n ≤ 2×10^5
> **Idea:** Greedy + binary search.

# Q19
> [!abstract] ### [Projects](https://cses.fi/problemset/task/1140)
> **Goal:** Maximize reward from non-overlapping projects.
> **Constraints:** n ≤ 2×10^5
> **Idea:** Weighted interval scheduling + DP + binary search.

# Q20
> [!abstract] ### [Elevator Rides](https://cses.fi/problemset/task/1653)
> **Goal:** Minimize number of rides given weight constraints.
> **Constraints:** n ≤ 20
> **Idea:** Bitmask DP.

# Q21
> [!abstract] ### [Counting Tilings](https://cses.fi/problemset/task/2181)
> **Goal:** Count ways to tile a grid.
> **Constraints:** n ≤ 1000
> **Idea:** DP with bitmasking / profile DP.

# Q22
> [!abstract] ### [Counting Numbers](https://cses.fi/problemset/task/2220)
> **Goal:** Count numbers in range without adjacent equal digits.
> **Constraints:** up to 10^18
> **Idea:** Digit DP.

# Q23
> [!abstract] ### [Increasing Subsequence II](https://cses.fi/problemset/task/1748)
> **Goal:** Count number of increasing subsequences.
> **Constraints:** n ≤ 2×10^5
> **Idea:** DP + Fenwick Tree / Segment Tree.