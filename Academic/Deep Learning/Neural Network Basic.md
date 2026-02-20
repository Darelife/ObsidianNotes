[[Lecture 4,5,6,7,8 Neural Network Models]]
[[Deep Learning.canvas]]

# Lecture 4: Neural Network Models

## McCulloch-Pitts Neuron (1943)

_(Notes on binary activation, fixed weights, and logical computations like AND/OR/NOT)_

- In 1943, this model established neural like, networks, and could perform logical reasoning
- No training algorithm yet (fixed weights)
- Output $h_{w}(x) = step(x^{T}w)$
- basically, the sum of all the $X \times W$'s. X = input, and W = weight
- And then, the step function fires only if the weighted sum reaches the designated threshold.
- The neurons are binary (0 or 1 only)
- For `and`, T = 1.5 (w1 = 1, w2 = 1). For `or`, T = 0.5 (w1 = 1, w2 = 1).
- We've a slight update. Some of the weights could be `-inf`. So basically we've 2 types of signals
	- Inhibitory (weight = -inf): Always output 1, unless one of the inhibitory inputs isn't 0....($X \times W$) (here -inf * 0 is 0)
	- Excitatory : Output 1 if the threshold is cleared
	- $$Output = \begin{cases} 1 & \text{if } \sum (\text{excitatory inputs}) \ge T \text{ AND } \sum (\text{inhibitory inputs}) = 0 \\ 0 & \text{otherwise} \end{cases}$$
- For `not`, T = 0, but the input is inhibitory (w = -inf). So if 0 comes through, it will allow it, but if 1 comes through, it will be -inf, which will be below the threshold, making it output 0.

## Rosenblatt’s Perceptron (1958)

_(Notes on learning from data, threshold activation, and error correction)_

- Unlike the M-P neuron, the Perceptron can learn its own weights and bias from data.
- Weights ($w$) are no longer fixed at $1$ or $-\infty$. They can be any real number (positive or negative).
- Instead of a fixed threshold $T$, we use a bias term $b$. This effectively moves the threshold to the other side of the equation: $z = \sum (x_i \cdot w_i) + b$
- Activation Function: Still uses a Step Function(Heaviside step function).
    - $$h_{w,b}(x) = \begin{cases} 1 & \text{if } \sum_{i=1}^{n} w_ix_i + b \ge 0 \\ 0 & \text{otherwise} \end{cases}$$
- If the Perceptron makes a mistake, it updates its weights using the formula:
    - $w_{new} = w_{old} + \Delta w$
    - $\Delta w = \eta \cdot (target - prediction) \cdot x$
    - _(Where $\eta$ is the learning rate, a small value like 0.1)_

## Decision Boundaries and Linear Separability

_(Notes on why perceptrons can only solve linearly separable problems)_

## Multi-Layer Perceptrons (MLPs)

_(Notes on stacking neurons to create non-linear decision boundaries)_

# Lecture 5: Perceptron Learning Rule

## Learning from Errors and Weight Updates

_(Notes on Hebb's rule and shifting decision boundaries)_

## Perceptron Convergence Theorem

_(Notes on the theoretical guarantees for linearly separable data)_

## Limitations of the Perceptron Rule

_(Notes on what happens when data isn't linearly separable and the need for smooth, quantitative error measures)_

## Weight Initialization Strategies and Pitfalls

_(Notes on zero initialization, He initialization, vanishing/exploding gradients, and the symmetry problem)_

# Lecture 6 & 7: Neural Network Training

## Empirical Risk and Continuous Loss

_(Notes on transitioning from binary "mistakes" to measurable "loss" averages)_

## Perceptron Learning vs. Loss-Based Training

_(Notes comparing stochastic/sample-by-sample updates to batch processing and gradient-based optimization for deep networks)_

# Lecture 8: Optimization for Deep Neural Networks

## Linear Regression and Loss Minimization

_(Notes on squared error loss and expressing target values as weighted sums)_

## Analytic (Closed-Form) Solutions

_(Notes on solving via derivatives and why this deterministic approach is often impossible in Deep Learning)_

## Iterative Optimization Methods

_(Notes on Gradient Descent, grid search, step size, and finding the direction of the minimum)_

## Common Loss Functions

_(Notes on choosing loss functions based on the task, e.g., squared/absolute errors for regression vs. cross-entropy/hinge loss for classification)_
