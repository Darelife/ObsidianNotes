- something about perceptrons, and McCulloch Pitts neuron

# How To Learn?

> [!info] We can't initialize all of them with 0. Why?
> Cuz the gradients will always be 0

- rand weights
- learn from errors
## Hebbs Rule
(here we can start with zeroes btw)
we mainly just update the active inputs. If none, we update the bias
$$
w_{i,j} = w_{i,j} + \eta(y-\hat y)x
$$

## Perceptron Learning Rule
$$
\sigma(\Sigma w_i x_i + b) = \sigma(w^T x + b)
$$
if a model misclassifies a point, shift the decision boundary slightly toward the correct side. Hebbs won't work here, as the weights aren't binary. 
1. Initialize $w \leftarrow 0$
2. While there are misclassified points, pick a misclassified point, $x_n$ , and update$w \leftarrow w + y_n x_n$
