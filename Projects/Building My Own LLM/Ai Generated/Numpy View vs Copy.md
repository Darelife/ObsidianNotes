This image explains the difference between a NumPy **view** and a **copy**, using transpose.

Suppose:

```python
x = np.arange(12).reshape(4, 3)
```

So `x` looks like:

```text
0   1   2
3   4   5
6   7   8
9  10  11
```

## 1. How NumPy stores `x`

Although `x` is 2D, memory is physically one-dimensional:

```text
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
```

The array stores metadata describing how to interpret this memory:

```text
shape   = (4, 3)
strides = (24, 8)
```

Assuming each number occupies 8 bytes:

- Moving one column right means moving `8` bytes.
    
- Moving one row down means moving `24` bytes: skip three numbers.
    

The address of `x[i, j]` is conceptually:

```python
base_address + i * 24 + j * 8
```

## 2. `x.T`: transpose as a view

```python
y = x.T
```

Now the shape becomes `(3, 4)`:

```text
0   3   6   9
1   4   7  10
2   5   8  11
```

But NumPy has **not moved or copied any numbers**. Both `x` and `y` use the same underlying memory:

```text
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
```

It simply changes the strides:

```text
shape   = (3, 4)
strides = (8, 24)
```

Now:

- Moving down one row moves `8` bytes.
    
- Moving right one column moves `24` bytes.
    

So the address of `y[i, j]` becomes:

```python
base_address + i * 8 + j * 24
```

This is why transposing is extremely cheap: NumPy only creates new metadata. It does not rearrange the data.

Since the memory is shared, modifying the transpose can modify `x`:

```python
y[0, 1] = 100

print(x)
# [[  0   1   2]
#  [100   4   5]
#  [  6   7   8]
#  [  9  10  11]]
```

## 3. `x.T.copy()`: an independent array

```python
z = x.T.copy()
```

This time NumPy allocates new memory and physically stores the elements in the transposed order:

```text
0, 3, 6, 9, 1, 4, 7, 10, 2, 5, 8, 11
```

Therefore:

```text
shape   = (3, 4)
strides = (32, 8)
```

- Moving right goes to the next number: `8` bytes.
    
- Moving down skips four numbers: `32` bytes.
    

Because it owns separate memory, changing `z` does not affect `x`.

```python
z[0, 1] = 100
# x remains unchanged
```

## The three flags

- `C_CONTIGUOUS`: elements are laid out row-by-row in memory.
    
- `F_CONTIGUOUS`: elements are laid out column-by-column in memory.
    
- `OWNDATA`: the array owns its underlying memory instead of borrowing it from another array.
    

So:

|Array|Memory|Strides|C-contiguous|Owns data|
|---|---|--:|--:|--:|
|`x`|Original|`(24, 8)`|Yes|Yes|
|`x.T`|Shared with `x`|`(8, 24)`|No|No|
|`x.T.copy()`|New memory|`(32, 8)`|Yes|Yes|

The main idea is:

> A view changes how NumPy **walks through existing memory**. A copy creates and fills **new memory**.