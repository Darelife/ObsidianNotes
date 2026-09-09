# C. 101
Just ensure that there's a start, and an end, after that, make all the remaining -1's, 0

```cpp
void solve() {
  int n;
  cin >> n;
  vector<int> a(n);
  vcin(a, n);

  for (int i = 0; i < n; i++) {
    if (a[i] == 1) break;
    if (a[i] == -1) {
      a[i] = 1;
      break;
    }
  }
  for (int i = n - 1; i >= 0; i--) {
    if (a[i] == 1) break;
    if (a[i] == -1) {
      a[i] = 1;
      break;
    }
  }

  for (int i = 0; i < n; i++) {
    if (a[i] == -1) a[i] = 0;
  }
  vpin(a);
}
```

# D. MEX Multiset
for mex(A) + mex(b) + mex(c) >= 2\*mex(a) (assuming mex(a) >= mex(b) >= mex(c))
assume that we just focus on having 2 of them high, and ignore the 3rd one, we can still do it
so, store the counts of all the elements, and only put them in A, or B. go number wise, like, 1,2,3,...
for all cnt[i] >= 2 for it to work, otherwise, not possible, also break, if cnt[i] = 0, it's possible. cuz, `i` will be the mex in that case.

nvm, if cnt[i] = 1, then, we can just put it in C, as long as it's not 0.

```cpp
void solve() {
  int n;
  cin >> n;
  vector<int> a(n);
  vcin(a, n);
  map<int, int> cnt;
  for (int i = 0; i < n; i++) cnt[a[i]]++;

  for (int i = 0; i <= n / 2 + 2; i++) {
    if (cnt[i] == 1 && i == 0) {
      cout << "NO" << endl;
      return;
    } else if (cnt[i] == 0) {
      break;
    }
  }
  cout << "YES" << endl;

  string ans = "";
  map<int, int> put;
  for (int i = 0; i < n; i++) {
    if (cnt[a[i]] == 1) ans += 'C';
    else if (put[a[i]]) ans += 'B';
    else {
      ans += 'A';
      put[a[i]] = 1;
    }
  }
  cout << ans << endl;

}
```

# E. Treasure Map Destruction (Constructive Version)

We could run a BFS from the elements that we know the value of, and then...
oh wait, if we run a BFS from the -1 elements, and then validate each of them? (check if treasure could be there)?? nvm the previous approach was better ig, cuz we won't know whether the value in another node could be cuz of it or not.
faah, we can just ban specific ranges, that can't work out, like, via a difference array or something.
in the end, any element not in the banned range, can have treasure.

```cpp
void solve() {
  int n;
  cin >> n;
  vector<int> a(n);
  vcin(a, n);
  vector<int> b(n + 1);
  for (int i = 0; i < n; i++) {
    if (a[i] > 0) {
      b[max(0ll, i - a[i] + 1)]++;
      b[min(n, i + a[i])]--;
    }
  }
  for (int i = 1; i <= n; i++) {
    b[i] += b[i - 1];
  }
  string ans = "";
  for (int i = 0; i < n; i++) ans += '1';
  for (int i = 0; i < n; i++) {
    if (b[i] > 0) ans[i] = '0';
  }
  int cnt = 0;
  for (int i = 0; i < n; i++) cnt += (ans[i] == '1');
  if (cnt == 0) {
    cout << -1 << endl;
    return;
  }
  for (int i = 0; i < n; i++) {
    if (a[i] >= 0) {
      if (i - a[i] >= 0 && ans[i - a[i]] == '1') continue;
      if (i + a[i] < n && ans[i + a[i]] == '1') continue;
      cout << -1 << endl;
      return;
    }
  }
  cout << ans << endl;
}
```

