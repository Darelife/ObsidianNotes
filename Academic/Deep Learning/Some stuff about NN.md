# UAT (Deep Learning Book 6.4.1) 

2 Layer NNs are sufficient to solve problems of any difficulty (Like, $R^{N}\rightarrow R^{M}$). So Why do we need depth? (Universal Approximation Theorem (**UAT**))

> [!info] 3 Fundamental qs of DL
> 1. Approximation
> 2. Optimization
> 3. Generalization

UAT says that it's possible, but not efficient
- UAT says that the NN should be sufficiently wide. How much is "Sufficient"??


Why are shallow networks inefficient?
- Shallow networks approximate complex functions by patching togehter many local regions, while deep networks reuse intermediate computations hierarchically. 
- Eg: Deep = reusability. That's why foundation models work. Like, why can you fine tune chatgpt to make a chat bot for your company?


|Model|Regions it can create|
|---|---|
|1 wide layer|grows with neurons|
|L layers|grows **exponentially with depth**|

# When GPT works, why do we care about UAT?
- It gives a baseline guarantee. The limitation isn't "Can the model represent the function?" NNs are expressive enough
- Separates 2 questions
	- Expressivity (Representation exists) (We don't have a problem with this)
	- Trainability + generalization  (We have problems in this)

> [!info] Page 195 (The Deep Learning Book)
> In summary, a feedforward network with a single layer is suﬃcient to represent any function, but the layer may be infeasibly large and may fail to learn and generalize correctly. In many circumstances, using deeper models can reduce the number of units required to represent the desired function and can reduce the amount of generalization error.

Generalization = If my model will work on inputs that it hasn't been trained on.


# More on model capacity (beyond UAT)
- Inductive Bias : Belongs to the architecture (shallow, deep, cnn, etc). It makes learning realistic with finite data.

As model capacity increases, training error decreases, while the test error first decreases, and then increases.
But then, n double descent...test risk goes up, and then, starts going down again.....(if it's over-parameterized....if the data is very less, but we're using a massive neural network)