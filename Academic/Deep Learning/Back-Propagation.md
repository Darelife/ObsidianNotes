[[Lecture 4,5,6,7,8 Neural Network Models]]
[[Deep Learning.canvas]]

# Lecture 9: Backpropagation

## The Need for Backpropagation in MLPs

_(Notes on why the Perceptron rule fails for hidden layers, non-convex loss surfaces, and the difficulty of assigning "blame" to hidden weights)_

## Perceptron Learning vs. Backpropagation

_(Notes comparing sample-by-sample stochastic updates vs. batch processing, and adjusting decision boundaries vs. average error optimization)_

## The Mechanics of Backpropagation

_(Notes on the chain rule, calculating local derivatives symbolically, and updating weights using the gradient and learning rate)_

## Handling Non-Differentiable Operations

_(Notes on how Deep Learning handles discrete decisions and corner cases using subgradients, ReLU, and Softmax instead of argmax)_

## Numerical Stability Issues

_(Notes on vanishing and exploding gradients, and what backprop does NOT do—e.g., finding minima or deciding step sizes)_

# Lecture 10, 11 & 12: Automatic Differentiation

## The Scaling Challenge for Gradients

_(Notes on the redundancy and complexity of calculating millions of partial derivatives for Deep Neural Networks)_

## Four Methods of Differentiation

_(Notes comparing the pros/cons of Analytical, Symbolic (expression swell), Numerical (perturbation, approximation, scaling poorly), and Automatic differentiation)_

## Mechanics of Automatic Differentiation (Autodiff)

_(Notes on breaking functions into elementary operations, the forward pass (storing intermediate values), and the backward pass (accumulating local derivatives))_

## Implementation in Frameworks (PyTorch)

_(Notes on building computational graphs on the fly and using `requires_grad=True` and `loss.backward()`)_

[[Back-Propagation at Scale]]