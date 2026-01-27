# E. Product Queries

> [!question] We have an array of size $n$. There are n questions. In the $i$-th question, we need to tell the minimum number of elements (repetitions are allowed) that we can multiply, to get $i$

> [!info] Test Case 1: 3 2 2 3 7 3 6 7
> For i = 1, it's not possible, so -1
> For i = 2, just take a[1] (0 based) : so, 1
> For i = 3, it's 1
> For i = 4, it's 2
> For i = 5, it's again not possible, so -1
> For i = 6, there's a 6. So, 1
> For i = 7, it's 1
> For i = 8, We can multiply 2 with itself 3 times...so, 3

Now, this question is kinda confusing...I'm thinking of storing all the factors of all the numbers, but there's no way the rest of the logic can work after this. nvm, just saw DP as one of the tags, and I'm thinking of a 1d approach rn, involving the factors. The obvious base case, could just be to have the $a_i$th element's answer stored as 1. $dp[i] = 1$ for all i from 0 to n-1.

Wait though, we need it for the values, and not the indices, cuz, later for all the numbers larger, we'll check for the dp[value]. So, yeah, we can do that. 

Then, with our pre calculated values of the factors. Hmm, im still not confident about the factors part. Ig we could just go through the multiples somehow. Technically im thinking of doing it just like sieve. So, I don't think it would matter. So, lite.

```cpp
void solve() {
  int n;
  cin >> n;
  vector<int> a(n);
  vcin(a,n);

  vector<int> dp(n+1, INT_MAX);
  for (int i:a) dp[i] = 1;

  for (int i = 0; i<n; i++) {
    for (int j = 1; j<=n; j++) {
      if (n < a[i]*j) break;
      dp[a[i]*j] = min(dp[a[i]*j], 1 + dp[j]);
    }
  }

  for (int i = 1; i<=n; i++) {
    if (dp[i] == INT_MAX) cout << -1 << " ";
    else cout << dp[i] << " ";
  }
  cout << endl;
}
```

# F. Pizza Delivery

> [!question] We are on a 2D grid (X, Y) plane. We have a starting point, A. Then, we can move up and down anywhere in a column (Y axis), but on the X axis, we can either stay where we are, or move right. We have a bunch of points that we have to visit on the 2D grid, followed by an ending point, B, which is again on the right of those points. We need to find the minimum time to cover all those points.

Initially, I assumed that we could try some basic greedy approach. That like, the point where we must start in the current column (top or bottom...as we anyways have to cover the entire column), depends on the next column. But then, the point for the next column, depends on the column next to it. The solution can't be about starting from the last point, as it would end up being the same question literally...just with the twist that instead of moving right, we move left, A becomes the ending point, and B becomes the start. So, then, it could be like, a decision tree thing....which means...recursive. Which directly got me to think about DP. 

Now, in DP, I believe that it's just about which point we start from. TOP or BOTTOM. So, for each node, we just need to store that. And then, we can keep moving in the order. For the current column, we would need the column to the right, and so on. So, we can just use the dp, to find the time it would take to reach the end, from that point.

$dp[N][2]$ sized array basically. Where `dp[COLUMN = i][TOP=1, BOTTOM=0]` = time it would take to reach the end, if we started from the ith column, and in that, we started from TOP (if j = 1) or BOTTOM (if j = 0).

Time to reach from `i`, starting from the bottom is the minimum of the time it would take to reach from `i+1`'s top + dist between `i+1`'s TOP and `i`'s TOP (as we end in `i`'s top, cuz we start from `i`'s bottom), and the time to reach from `i+1`'s bottom + dist between `i+1`'s bottom and `i`'s top.

$$
dp_{{i,0}} = |top_{i}-bottom_{i}| + min(dp_{i+1,1} + 1+|top_{i+1}-top_{i}| \ , \ dp_{i+1,0} + 1+|bottom_{i+1}-top_{i}|)
$$
$$
dp_{{i,1}} = |top_{i}-bottom_{i}| + min(dp_{i+1,1} + 1+|top_{i+1}-bottom_{i}| \ , \ dp_{i+1,0} + 1+|bottom_{i+1}-bottom_{i}|)
$$

In fact, we can even space optimise it. But im just planning on doing it using memoization.

```cpp
vector<vector<int>> dp;
int DP(int i, int j, int &n, vector<pair<int,int>>& top, vector<pair<int,int>>& bottom) {
  // if (i == n) {
  // }
  if (dp[i][j] != -1) return dp[i][j];
  if (i == n-1) {
    dp[i][j] = 0;
    return 0;
  }

  if (j == 0) {
    int ans = top[i].first-bottom[i].first
      + min(DP(i+1,1,n,top,bottom) + top[i+1].second-top[i].second
      + abs(top[i+1].first-top[i].first), 
      
      DP(i+1,0,n,top,bottom) + top[i+1].second-top[i].second
      + abs(bottom[i+1].first-top[i].first));
    dp[i][j] = ans;
    return ans;
  }
  int ans = top[i].first-bottom[i].first
    + min(DP(i+1,1,n,top,bottom) + top[i+1].second-top[i].second
    + abs(top[i+1].first-bottom[i].first), 
    
    DP(i+1,0,n,top,bottom) + top[i+1].second-top[i].second
    + abs(bottom[i+1].first-bottom[i].first));
  dp[i][j] = ans;
  return ans;
}

void solve() {
  int n, ax, ay, bx, by;
  cin >> n >> ax >> ay >> bx >> by;
  vector<int> x(n), y(n);
  vcin(x, n);
  vcin(y, n);

  map<int,vector<int>> pos;
  for (int i = 0; i<n; i++) {
    pos[x[i]].pba(y[i]);
  }

  vector<pair<int,int>> top, bottom;

  for (auto[x, v]:pos) {
    int t = *max_element(v.begin(), v.end());
    int b = *min_element(v.begin(), v.end());
    top.pba({t, x});
    bottom.pba({b, x});
  }
  top.pba({by,bx});
  bottom.pba({by,bx});

  n = top.size();
  dp.assign(n, vector<int>(2, -1));
  int ans1 = DP(0,1,n,top,bottom);
  int ans2 = DP(0,0,n,top,bottom);

  ans2 += abs(ax-bottom[0].second);
  ans2 += abs(ay-bottom[0].first);

  ans1 += abs(ax-top[0].second);
  ans1 += abs(ay-top[0].first);

  cout << min(ans1, ans2) << endl;
}
```
