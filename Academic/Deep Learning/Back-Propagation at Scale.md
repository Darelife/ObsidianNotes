- Training NN isn't hard cuz of the calculus, it's hard cuz of inefficient computation of gradients at scale.
- In the end, we just have to do DP on DAG.

- In tensorflow v1 (**Lazy Execution**), we generate the computational graph first, and then, the computation happens when we run it later (the forward pass)
- This can help in optimizing the code early on
- However, this isn't used now, and now, pytorch, and even tensorflow v2, generates the graph, during the forward pass.
- Errors show up only when the forward pass is done, instead of when the graph is created......but doesn't the same thing happen with pytorch?? like at runtime?? ||Yes..it happens during run time for both||

> [!info]
> In TensorFlow v1 (**Lazy Execution**), the entire computation graph is constructed symbolically first, and only when the graph is executed (forward pass) do errors appear. This delays feedback and can waste developer time (not extra model computation), because you may build a large graph before discovering a bug. 
> 
> In PyTorch (**Eager Execution**), the graph is constructed while executing the forward pass, so operations run immediately and many errors are caught at the exact line they occur, giving faster feedback and easier debugging.

**ChatGPT improved version**
> [!info]
> In TensorFlow v1 (**lazy execution**), the computation graph is constructed symbolically first, and only when the graph is executed do errors appear. This delays feedback and increases developer iteration time (not the cost of a single forward pass), because bugs may be discovered only after graph construction.
> 
> In PyTorch (**eager execution**), operations execute immediately while the graph is built, so many errors are raised at the exact Python line that caused them, enabling faster feedback and easier debugging.

Basically, if the error comes up while doing the forward pass, we can't point the error out properly, as the error will look like this

```lua
InvalidArgumentError: Matrix size-incompatible: In[0]: [32,128], In[1]: [64,256]
     [[node MatMul_17]]
```

## Forward Mode

In **Auto Differentiation**,
Suppose $y = w_{1}x_{1} + w_{2}x_{2}$, $\frac{dw_{1}}{dw_{1}} = 1, \frac{dw_{2}}{dw_{1}} = 0$, and  $\frac{dw_{1}}{dw_{2}} = 0, \frac{dw_{2}}{dw_{2}} = 1$ 

and Loss = y
Then, for each variable, like, $w_{1}$, we have to do 1 forward pass. Followed by another forward pass for $w_{2}$.

## Reverse Mode

There's another thing called **Reverse Mode Auto Differentiation**.

First we do the forward pass. We only calculate the values during this. We do NOT calculate the derivative. Then, we do the backward pass, to finally calculate the derivative.

Reverse mode starts from the loss. It doesn't start from the differentiation of the weights. It then, computes $\frac{dL}{dy}$. Then, it distributes it backwards to all the parameters at once: $\frac{dL}{dw_{1}}$, $\frac{dL}{dw_{2}}$, ...

> [!info] Backpropagation & Reverse Mode Auto Differentiation
> Here, back propagation refers to the entire thing, but basically backprop is basically just the chain rule part. The rest is reverse mode auto diff.
>  
> Pytorch's `.backward()` performs the reverse mode auto diff.

> [!additional Info] 
> Forward mode is still efficient when there are fewer inputs.
> There's also a mixed mode auto differentiation that's used in DL.

