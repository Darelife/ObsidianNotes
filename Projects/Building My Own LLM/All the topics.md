# Torch
## Randint
```python
randint = torch.randint(0, 10, (3, 4))
print(randint)
randint = torch.randint(-100, 100, (6,))
print(randint)
```

```output
tensor([[0, 8, 9, 4], [9, 3, 7, 0], [1, 1, 6, 4]]) tensor([-90, -75, 14, 21, -48, -57])
```

## Tensor
```python
tensor = torch.tensor([[0.1,1.2], [2.2, 3.1], [4.9, 5.2]])
```

```python
zeroes = torch.zeros(2, 3)
```

```python
ones = torch.ones(3,4)
```

```python
input = torch.empty(2,3)
# Has garbage values 
```

```python
arange = torch.arange(10)
```

```python
linspace = torch.linspace(0, 10, steps=5)
```

```python
logspace = torch.logspace(0, 1, steps=5)
print(logspace)
# 10^0, 10^0.25, 10^0.5, 10^0.75, 10^1

# import matplotlib.pyplot as plt
# we'll plot the points
# plt.plot(logspace.numpy())
# plt.show()
```

```python
eye = torch.eye(3)
# Diagonal matrix
```

```python
a = torch.ones(2,3)
emptyLike = torch.empty_like(a) # Creates a tensor of the same shape as 'a' but with uninitialized values
print(emptyLike)
```

## More Torch Functions Used Regularly

```python
probabilities = torch.tensor([0.1, 0.9])
samples = torch.multinomial(probabilities, num_samples=10, replacement=True)
print(samples) # tensor([1, 1, 0, 1, 1, 1, 1, 1, 1, 1])
```

```python
tensor = torch.tensor([1,2,3,4])
out = torch.cat((tensor, torch.tensor([5])), dim=0)
print(out) # tensor([1, 2, 3, 4, 5])
```

```python
out = torch.tril(torch.ones(5,5))
# Triangle Lower: Lower Triangle Matrix
# Makes the upper triangle (other than the diagonal) zero
print(out)
```

```python
out = torch.triu(torch.ones(5,5))
print(out)
```

```python
out = torch.zeros(5,5).masked_fill(
  torch.tril(torch.ones(5,5)) == 0,float('-inf')
)
# All the spots that are not false (not zero) become
# value (ie. -inf)
print(out)
print(torch.exp(out))
```

```python
input = torch.zeros(2,3,4)
out = input.transpose(0,2) # Swap 0, with 2
print(out.shape)
```

```python
tensor1 = torch.tensor([1,2,3])
tensor2 = torch.tensor([4,5,6])
tensor3 = torch.tensor([7,8,9])

stacked = torch.stack([tensor1, tensor2, tensor3])
print(stacked)
```

## Torch.nn

```python
import torch
import torch.nn as nn
import torch.nn.functional as F
```

### Linear

```python
sample = torch.tensor([10.,10.,10.])
linear = nn.Linear(3,3,bias=False)
# y = xW^T + b
# Sample has 3 features, and the linear layer has 3 output features
# so the weight matrix W will be of shape (3, 3).
# Since bias is set to False, there will be no bias term added to
# the output. The output of the linear layer will be computed as
# the dot product of the input sample and the weight matrix W.

# The values are initialized randomly from a uniform 
# distribution by default. The output will be a tensor of
# shape (3,) representing the transformed features after
# applying the linear transformation.
# U(-sqrt(k), sqrt(k)) where k = 1 / in_features = 1/3
print(linear.weight)
print(linear(sample))
```

```output
Parameter containing:
tensor([[-0.1923,  0.0323,  0.1064],
        [-0.3797, -0.3063,  0.1508],
        [ 0.1606,  0.5376,  0.0955]], requires_grad=True)
tensor([-0.5358, -5.3517,  7.9371], grad_fn=<SqueezeBackward4>)
```

### Softmax

```python
tensor1 = torch.tensor([1.0, 2.0, 3.0])
softmax = F.softmax(tensor1, dim=0)
print(softmax)

# Softmax(z) = exp(z) / sum(exp(z))
# It converts the input tensor into a probab distribution over
# the classes. The output values will be in the range (0, 1) and
# will sum to 1. The softmax function is often used in the final
# layer of a neural network for multi-class classification problems.
```

### Embedding

```python
vocabularySize = 1000
embeddingDim = 100
embedding = nn.Embedding(vocabularySize, embeddingDim)

inputIndices = torch.LongTensor([1, 5, 3, 2])
embeddedOutput = embedding(inputIndices)
print(embeddedOutput.shape)
print(embeddedOutput)
```

This is entirely random right now btw

## Dot Products & Matrix Multiplication

> [!info] Btw, in pytorch we use @ for matrix multiplication

> [!info] A quick reminder about n x m in matrices.
> n = number of rows, and m = number of columns

> [!info] Matrix Multiplication (Cross Product)
> We just do Output(i,j) = $\Sigma$ RowOf1(i) $\times$ ColumnOf2(j) 

```python
a = torch.tensor([[1,2], [3,4], [5,6]]) # 3x2 tensor
b = torch.tensor([[1,2,3], [4,5,6]]) # 2x3 tensor
result = torch.matmul(a, b) # 3x3 tensor
print(result)
result = torch.mm(a, b) # 3x3 tensor
print(result)
result = a @ b # 3x3 tensor
print(result)

# Matmul and @ are the same
# mm is only for 2D tensors / matrices
```