# McCulloch Pitts Neuron 1943

- There was no learning in this. It was just computation. 
- It just showed that neural like units can implement logical reasoning.
- Networks of neurons can implement any logical curcuit
- There were inputs, weights, -> weighted sum, a step function, and then an output.
- They we connected by "fixed" weights.
- Reasoning can be expressed as computation, and the brain could be viewed as a computing system.


We could implement basic, C = A, C = A & B, C = A | B, and C = A & ~B easily using this

for `and`, activation = 1.5, and for `or`, activation = 0.5

# Rosenblatt's Perceptron 1958

This involved learning from data using error correction.

> Intelligence doesn't have to be programmed, it can be **learned**

A **perceptron** is the simplest neural network unit. It takes multiple inputs, combines them linearly, and passes the result through an activation function.

$$
y = \sigma\!\left(\sum_i w_i x_i + b \right)
$$

## What Each Term Means

### Inputs
$$
\mathbf{x} = [x_1, x_2, \dots, x_n]
$$
These are the input features.

Example:
- \(x_1\) = pixel intensity
- \(x_2\) = edge detected 
- \(x_3\) = bias input, etc.

---

### Weights
$$
\mathbf{w} = [w_1, w_2, \dots, w_n]
$$

Each input has a weight that represents **how important that input is**.

- Larger \(w_i\) → input \(x_i\) matters more  
- Negative \(w_i\) → input discourages activation  

---

### Bias
$$b$$

The bias shifts the decision boundary.

Without bias:
- The separating line/plane must pass through the origin

With bias:
- The line/plane can move freely

---

### Linear Combination (Score)

$$
z = \sum_i w_i x_i + b
$$

This is just a weighted sum of inputs.  
Geometrically, this defines a **decision boundary**:

$$
\mathbf{w}^T \mathbf{x} + b = 0
$$

This is:
- a line in 2D 
- a plane in 3D 
- a hyperplane in higher dimensions

---

### Activation Function

$$
y = \sigma(z)
$$

For a classic perceptron,
\($\sigma$\) is a **step function**:

$$
\sigma(z) =
\begin{cases}
1 & \text{if } z \ge 0 \\
0 & \text{if } z < 0
\end{cases}
$$

So the output is **binary: 0 or 1**.

---

## Intuition

The perceptron is asking:

> “Is the weighted sum of inputs big enough to cross a threshold?”

If yes → output 1 
If no → output 0 

---

## Example

Let:

$$
\mathbf{x} = [1, 0], \quad \mathbf{w} = [2, -1], \quad b = -0.5
$$$$
z = (2 \cdot 1) + (-1 \cdot 0) - 0.5 = 1.5
$$
$$
y = \sigma(1.5) = 1
$$

---

## Key Takeaways

- The perceptron computes a **linear score**
- Then applies a **threshold**
- This creates a **linear decision boundary**
- It can solve problems like AND / OR  
- It **cannot solve XOR** (not linearly separable)

---

## Visualization Tool

Interactive demo:
https://vinizinho.net/projects/perceptron-viz/


## Learning the OR Gate using Rosenblatt’s Perceptron (Hebbian & Error-Correction Rules)

The OR gate can be implemented using Rosenblatt’s perceptron by learning the weights ((w_1, w_2)) and bias (b) automatically instead of setting them manually. 
Starting from initial values (e.g., (w_1 = 0, w_2 = 0, b = 0)), the perceptron processes each training example ((x_1, x_2, y)) and computes:

$$
y' = f(w_1 x_1 + w_2 x_2 + b) 
$$

If the output (y') is incorrect, the parameters are updated using the perceptron learning rule (a simplified Hebbian / error-correction rule):

$$  
w_i \leftarrow w_i + \eta (y - y') x_i, \quad b \leftarrow b + \eta (y - y')  $$

This shifts the decision boundary slightly toward the correct side whenever a point is misclassified.
After iterating over the OR truth table, the perceptron converges to a valid solution such as:

$$ 
w_1 = 1, ; w_2 = 1, ; b = -1  
$$

which correctly classifies all OR inputs.



## Perceptron Convergence Algo

1. Choose a training case
2. If the predicted output matches the output label, do nothing.
3. If the perceptron predicts a 0, and it should have predicted a 1, add the input vector to the weight vector.
4. If the perceptron predicts a 1, and it should have predicted a 0, subtract the input vector from the weight vector.

We need a **smooth measure** of how wrong the model is. Not binary (correct/incorrect), but quantitative. 
It should be
1. smooth objective
2. optimizable
3. works for all models

> [!question] Why can't we initialize all the weights with 0? Why is it random?
> If it's 0, all the networks neurons become clones of each other, and never learn diff features (symmetry).
> 
> Neuron 1's output = neuron 2's output. Gradients during backprop = same. Update = same. so, it learns just 1 feature basically, and will all become identical.