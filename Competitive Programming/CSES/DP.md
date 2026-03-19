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

# Q1
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

# Q2
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

# Q3
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

# Q4
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

# Q5
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

# Q6
> [!abstract] ### [Grid Paths I](https://cses.fi/problemset/task/1638)
> **Goal:** Count the number of ways to move from the top-left to bottom-right of a grid with obstacles.
> **Constraints:** n ≤ 1000
> **Idea:** Classic grid DP; dp[i][j] = ways from top or left if cell is not blocked.

# Q7
> [!abstract] ### [Book Shop](https://cses.fi/problemset/task/1158)
> **Goal:** Maximize total pages with a limited budget.
> **Constraints:** n ≤ 1000, x ≤ 10^5
> **Idea:** 0/1 Knapsack DP.

# Q8
> [!abstract] ### [Array Description](https://cses.fi/problemset/task/1746)
> **Goal:** Count arrays satisfying adjacent difference ≤ 1 with some fixed values.
> **Constraints:** n ≤ 10^5, values ≤ 100
> **Idea:** DP over index and value.

# Q9
> [!abstract] ### [Counting Towers](https://cses.fi/problemset/task/2413)
> **Goal:** Count ways to build towers using 2×n blocks.
> **Constraints:** n ≤ 10^6
> **Idea:** DP with state transitions for full and partial fillings.

# Q10
> [!abstract] ### [Edit Distance](https://cses.fi/problemset/task/1639)
> **Goal:** Find minimum operations to convert one string to another.
> **Constraints:** length ≤ 5000
> **Idea:** Classic DP with insert, delete, replace.

# Q11
> [!abstract] ### [Longest Common Subsequence](https://cses.fi/problemset/task/1143)
> **Goal:** Find the length of the longest common subsequence of two strings.
> **Constraints:** length ≤ 1000
> **Idea:** Standard 2D DP.

# Q12
> [!abstract] ### [Rectangle Cutting](https://cses.fi/problemset/task/1744)
> **Goal:** Minimum cuts to divide rectangle into squares.
> **Constraints:** a, b ≤ 500
> **Idea:** DP over dimensions, try all cuts.

# Q13
> [!abstract] ### [Minimal Grid Path](https://cses.fi/problemset/task/1638)
> **Goal:** Find minimum cost path in a grid.
> **Constraints:** n ≤ 1000
> **Idea:** DP or Dijkstra on grid.

# Q14
> [!abstract] ### [Money Sums](https://cses.fi/problemset/task/1745)
> **Goal:** Find all possible sums from given coins.
> **Constraints:** n ≤ 100, sum ≤ 10^5
> **Idea:** Subset sum DP.

# Q15
> [!abstract] ### [Removal Game](https://cses.fi/problemset/task/1097)
> **Goal:** Maximize score assuming optimal play from both ends.
> **Constraints:** n ≤ 5000
> **Idea:** DP on intervals.

# Q16
> [!abstract] ### [Two Sets II](https://cses.fi/problemset/task/1093)
> **Goal:** Count ways to divide numbers 1..n into two sets with equal sum.
> **Constraints:** n ≤ 500
> **Idea:** Subset sum DP with symmetry handling.

# Q17
> [!abstract] ### [Mountain Range](https://cses.fi/problemset/task/1748)
> **Goal:** Count valid mountain sequences.
> **Constraints:** n ≤ 10^6
> **Idea:** Catalan-like DP.

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