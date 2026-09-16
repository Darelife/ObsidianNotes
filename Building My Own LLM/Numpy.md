# Ndarray

Python List: `[Length, Items]`
`Items` $\rightarrow$ `[Pointer 1, Pointer 2,...]`
The final data blocks aren't stored contiguously. Just their pointers are, and the data is heterogenous.

Ndarray: Every item has the same type. The datablocks are stored contiguously. We can perform operations much more easily, for the entire vector, like, `arr*2`, instead of `[x * 2 for x in arr]`. Things are much more faster, as it's C-compiled

### Syntax

```py
x = np.array([...], dtype=np.int64)
print(x.shape) # dimensions (d0,d1,d2,...)
print(x.strides) # number of bytes to jump across the dimensions
```

dtype is optional

### View vs Copy
![[Pasted image 20260911182536.png]]
[Why NumPy Arrays Are So Fast (And How They Really Work)](https://aiadvances.org/why-numpy-arrays-are-so-fast-and-how-they-really-work-1e5b56c75f8b) 
[[Numpy View vs Copy]]
