## Modular Arithmetic
 
Modular arithmetic allows us to perform operations while keeping numbers bounded by a modulus \( m \).
 
### Basic Properties
 
$$
(a + b) \bmod m = (a \bmod m + b \bmod m) \bmod m
$$
 
$$
(a - b) \bmod m = (a \bmod m - b \bmod m + m) \bmod m
$$
 
$$
(a \cdot b) \bmod m = \big((a \bmod m) \cdot (b \bmod m)\big) \bmod m
$$
 
$$
a^b \bmod m = (a \bmod m)^b \bmod m
$$
---
 
## Binary Exponentiation
 
Binary exponentiation is used to efficiently compute
 
$$
x^n \bmod m
$$
 
### Idea
 
We represent the exponent \( n \) in binary.
Example: $5^{10} = 5^{1010_2} = 5^8 \cdot 5^2$
 
If we know the values of        $x^y\quad\text{for all powers of two}$
i.e., $x^1,\; x^2,\; x^4,\; \dots,\; x^{2^{\lfloor \log_2 n \rfloor}}$
then we can compute \( x^n \) by multiplying only the required terms.
 
### Time Complexity
 
- Naive exponentiation: \( O(n) \)
- Binary exponentiation:  
  $$\mathcal{O}(\log n)$$
 
---
 
### Recursive Binary Exponentiation
 
```cpp
long long binpow(long long a, long long b, long long m) {
	if (b == 0)
		return 1;
 
	long long res = binpow(a, b / 2, m);
	res = (res * res) % m;
 
	if (b % 2)
		res = (res * a) % m;
 
	return res;
}
```
 
```cpp
long long binpow(long long x, long long n, long long m) {
	x %= m;            // reduce base first
	long long res = 1;
 
	while (n > 0) {
		if (n & 1)
			res = (res * x) % m;
		x = (x * x) % m;
		n >>= 1;
	}
	return res;
}
 
```
 
 
## Modular Inverse
 
In modular arithmetic, **division is not directly defined**.  
To compute
 
$$
\frac{a}{b} \bmod m
$$
 
we instead compute
 
$$
a \cdot b^{-1} \bmod m
$$
 
where \( b^{-1} \) is the **modular inverse** of \( b \) modulo \( m \).
 
---
 
### Definition
 
A number \( b^{-1} \) is called the modular inverse of \( b \) modulo \( m \) if:
 
$$
b \cdot b^{-1} \equiv 1 \pmod m
$$
 
---
 
### When Does a Modular Inverse Exist?
 
A modular inverse of \( b \) modulo \( m \) exists **iff**:
 
$$
\gcd(b, m) = 1
$$
 
That is, \( b \) and \( m \) must be **coprime**.
 
---
 
## Fermat’s Little Theorem (Proof) (Read Later)
 
### Statement
 
If \( p \) is a prime number and \( \gcd(a, p) = 1 \), then:
 
$$
a^{p-1} \equiv 1 \pmod p
$$
 
---
 
### Proof
 
Consider the set of integers:
 
$$
S = \{1, 2, 3, \dots, p-1\}
$$
 
Since \( p \) is prime and \( gcd(a, p) = 1 \), multiplication by \( a \) permutes the elements of \( S \) modulo \( p \).
 
That is, the set:
 
$$
aS = \{a \cdot 1, a \cdot 2, \dots, a \cdot (p-1)\} \pmod p
$$
 
is just a rearrangement of \( S \).
 
Now take the product of all elements in both sets:
 
$$
(1 \cdot 2 \cdot \dots \cdot (p-1))
\equiv
(a \cdot 1)(a \cdot 2) \dots (a \cdot (p-1)) \pmod p
$$
 
Factor out \( a^{p-1} \):
 
$$
(1 \cdot 2 \cdot \dots \cdot (p-1))
\equiv
a^{p-1} \cdot (1 \cdot 2 \cdot \dots \cdot (p-1)) \pmod p
$$
 
Since none of the numbers \( 1, 2, $\dots$ , p-1 \) are divisible by \( p \), their product is invertible modulo \( p \).                                                                                                        
 
Cancelling it from both sides gives:
 
$$
a^{p-1} \equiv 1 \pmod p
$$
 
## Modular Inverse Using Fermat’s Little Theorem
 
If \( m \) is **prime**, then: $b^{m-1} \equiv 1 \pmod m$
 
Multiplying both sides by \( b^{-1} \): $b^{m-2} \equiv b^{-1} \pmod m$
 
### Formula
 
$$
b^{-1} \bmod m = b^{m-2} \bmod m
$$
 
This can be computed efficiently using **binary exponentiation**.
 
---
 
### C++ Implementation (using binpow)
 
```cpp
long long modinv(long long b, long long m) {
	return binpow(b, m - 2, m);
}
```
 
# Primes
 
Every positive integer has a **unique prime factorization**, meaning it can be written as a product of prime numbers in exactly one way (up to ordering):
 
$$n = p_1^{a_1} \, p_2^{a_2} \cdots p_k^{a_k}$$
> [!info] for $n = p_1^{a_1} \, p_2^{a_2} \cdots p_k^{a_k}$, number of divisors = (a+1) \* (a2+1) ... 
 
where \( p_i \) are **distinct prime numbers** and \( a_i \) are **positive integers**.
 
```cpp
vector<int> factor(int n) {
	vector<int> ret;
	for (int i = 2; i * i <= n; i++) {
		while (n % i == 0) {
			ret.push_back(i);
			n /= i;
		}
	}
	if (n > 1) { ret.push_back(n); }
	return ret;
}
```
 
This algorithm runs in \( $\mathcal{O}(\sqrt{n})$ \) time, because the `for` loop checks divisibility for at most \( $\sqrt{n}$ \) values.
 
Even though there is a `while` loop inside the `for` loop, dividing \( n \) by \( i \) quickly reduces the value of \( n \). As a result, the upper bound of the outer loop decreases, meaning the `for` loop runs fewer iterations in practice, which actually speeds up the code.
 
 
# Sieve 
 
```cpp
vector<bool> is_prime(n + 1, true);
is_prime[0] = is_prime[1] = false;
 
for (int i = 2; i * i <= n; i++) {
	if (is_prime[i]) {
		for (int j = i * i; j <= n; j += i) {
			is_prime[j] = false;
		}
	}
}
```
 
 
# Applications of the Sieve of Eratosthenes
 
## 1. Prime Checking in $O(1)$
 
After building the sieve up to $N$, we can check if any number $x \le N$ is prime in constant time using a boolean array.
 
---
 
## 2. Finding the Smallest Prime Factor (SPF)
 
Instead of only marking composites, we store the **smallest prime factor** for every number.
 
Let:
$$
\text{spf}[x] = \text{smallest prime that divides } x
$$
 
This enables fast factor-based computations.
 
### SPF Sieve (Core Precomputation)
 
```cpp
const int N = 1e6;
int spf[N + 1];
bool is_prime[N + 1];
 
void sieve() {
	for (int i = 1; i <= N; i++) {
		spf[i] = i;
		is_prime[i] = true;
	}
	is_prime[0] = is_prime[1] = false;
 
	for (int i = 2; i * i <= N; i++) {
		if (is_prime[i]) {
			for (int j = i * i; j <= N; j += i) {
				if (is_prime[j]) {
					is_prime[j] = false;
					spf[j] = i;
				}
			}
		}
	}
}
````
 
---
 
## 3. Fast Prime Factorization
 
Using the SPF array, any number $n$ can be factorized as:  
$$  
n = p_1 \times p_2 \times \dots \times p_k  
$$
 
by repeatedly dividing by $\text{spf}[n]$.
 
### Prime Factorization Code
 
```cpp
vector<int> factorize(int n) {
	vector<int> factors;
	while (n > 1) {
		factors.push_back(spf[n]);
		n /= spf[n];
	}
	return factors;
}
```
 
**Time Complexity:** $O(\log n)$
 
---
 
## 4. Computing Number of Divisors
 
If the prime factorization of $n$ is:  
$$  
n = p_1^{a_1} p_2^{a_2} \dots p_k^{a_k}  
$$
 
then the number of divisors is:  
$$  
(a_1 + 1)(a_2 + 1)\dots(a_k + 1)  
$$
 
 
# GCD (Greatest Common Divisor)
 
## Euclidean Algorithm
 
To compute the GCD of two non-negative integers, we use the **Euclidean Algorithm**:
 
$$
\gcd(a, b) =
\begin{cases}
a & \text{if } b = 0 \\
\gcd(b, a \bmod b) & \text{if } b \neq 0
\end{cases}
$$
 
---
 
## Proof of the Euclidean Formula
 
We prove that:
$$
\gcd(a, b) = \gcd(b, a \bmod b)
$$
 
### Proof
 
Let:
$$
a = bq + r \quad \text{where } r = a \bmod b
$$
 
Now:
- Any common divisor of $a$ and $b$ also divides $r = a - bq$
- Any common divisor of $b$ and $r$ also divides $a = bq + r$
 
Thus, the set of common divisors of $(a, b)$ and $(b, r)$ is the same.
 
Therefore:
$$
\gcd(a, b) = \gcd(b, a \bmod b)
$$
 
This proves the correctness of the Euclidean Algorithm.
 
---
 
## Recursive Implementation (C++)
 
```cpp
int gcd(int a, int b) {
	if (b == 0) return a;
	return gcd(b, a % b);
}
````
 
---
 
## Iterative Implementation (C++)
 
```cpp
int gcd(int a, int b) {
	while (b != 0) {
		a %= b;
		swap(a, b);
	}
	return a;
}
```
 
---
 
## Important GCD Properties and Formulas
 
### 1. Symmetry
 
$$  
\gcd(a, b) = \gcd(b, a)  
$$
 
---
 
### 2. GCD with Zero
 
$$  
\gcd(a, 0) = a, \quad \gcd(0, 0) = 0  
$$
 
---
 
### 3. Scaling Property
 
$$  
\gcd(ka, kb) = k \cdot \gcd(a, b)  
$$
 
for any integer $k > 0$.
 
---
 
### 4. Subtraction Property
 
$$  
\gcd(a, b) = \gcd(a - b, b)  
$$
 
(valid when $a \ge b$)
 
This is a direct consequence of the Euclidean Algorithm.
 
---
 
### 5. Relation with LCM
 
For any positive integers $a$ and $b$:  
$$  
\gcd(a, b) \cdot \operatorname{lcm}(a, b) = a \cdot b  
$$
 
---
 
### 6. GCD of Multiple Numbers
 
$$  
\gcd(a, b, c) = \gcd(\gcd(a, b), c)  
$$
 
---
 
### 7. Coprime Numbers
 
Two numbers $a$ and $b$ are **coprime** if:  
$$  
\gcd(a, b) = 1  
$$
 
---
 
## Time Complexity
 
The Euclidean Algorithm runs in:  
$$  
O(\log \min(a, b))  
$$
 
 
 
 
# Combinatorics
 
Credits USACO : Please check it out...it's amazing
 
```cpp
const int MAXN = 1e6;
 
long long fac[MAXN + 1];
long long inv[MAXN + 1];
 
/** @return x^n modulo m in O(log p) time. */
long long exp(long long x, long long n, long long m) {
	x %= m;  // note: m * m must be less than 2^63 to avoid ll overflow
	long long res = 1;
	while (n > 0) {
		if (n % 2 == 1) { res = res * x % m; }
		x = x * x % m;
		n /= 2;
	}
	return res;
}
 
/** Precomputes n! from 0 to MAXN. */
void factorial(long long p) {
	fac[0] = 1;
	for (int i = 1; i <= MAXN; i++) { fac[i] = fac[i - 1] * i % p; }
}
 
/**
 * Precomputes all modular inverse factorials
 * from 0 to MAXN in O(n + log p) time
 */
void inverses(long long p) {
	inv[MAXN] = exp(fac[MAXN], p - 2, p);
	for (int i = MAXN; i >= 1; i--) { inv[i - 1] = inv[i] * i % p; }
}
 
/** @return nCr mod p */
long long choose(long long n, long long r, long long p) {
	return fac[n] * inv[r] % p * inv[n - r] % p;
}
```~