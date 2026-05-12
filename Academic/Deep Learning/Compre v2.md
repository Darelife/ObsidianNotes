# 📚 Complete Deep Learning Study Guide: Architecture, Optimization & Self-Supervised Learning

This guide synthesizes lecture slides with key research papers to give you comprehensive, exam-ready knowledge.

---

## Part 1: Architecture & Optimization

### 1.1 The Initial Problem: Vanishing/Exploding Gradients

Before 2012, training deep networks was notoriously difficult. The culprit was the chain rule.

**The Mathematical Problem**:
When you compute gradients for early layers (like $W_1$), you end up multiplying many Jacobian matrices and derivatives:

$$\frac{\partial L}{\partial W_1} = \prod_{l=1}^L f'(z_l)W_l$$

- With sigmoid or tanh activation functions, $f'(z)$ is at most 0.25
- Multiply 50 such numbers: $0.25^{50} = \text{effectively zero}$ → gradients **vanish**
- If weights $W_l$ are large, the product **explodes**

**Why This Matters**: Your network stops learning. The front layers never update because the gradient signal never reaches them.

**What This Looks Like**:
- Early layers not learning (weights stay near initialization)
- Gradients near zero when you check them
- Loss plateaus without decreasing

### 1.2 Xavier/He Initialization (The First Fix)

The insight: **Stabilize the variance of activations** at initialization so signals neither blow up nor vanish.

**Variance Propagation Derivation**:

For a linear layer with weights $W$ and input $a_{l-1}$:
$$a_l = W a_{l-1}$$

The variance (assuming zero-mean and independence) is:
$$\text{Var}(a_l) \approx n \cdot \text{Var}(W) \cdot \text{Var}(a_{l-1})$$

where $n$ is the **fan-in** (number of inputs to the layer).

**Goal**: $\text{Var}(a_l) \approx \text{Var}(a_{l-1})$

**Solution**: $n \cdot \text{Var}(W) = 1$, so $\text{Var}(W) = \frac{1}{n}$

| Initialization | Formula | When to Use |
|---|---|---|
| Xavier (Glorot) | $\text{Var}(W) = \frac{1}{\text{fan}_{\text{in}}}$ | tanh, sigmoid, linear |
| He (Kaiming) | $\text{Var}(W) = \frac{2}{\text{fan}_{\text{in}}}$ | ReLU (half activations are zero) |

**Why These Work**: They ensure that at initialization, the signal propagates through all layers without shrinking or exploding.

**The Limitation**: Initialization only stabilizes the **first step**. As training progresses, weights change, distributions drift, and the variance relationships break down.

### 1.3 Batch Normalization (Keeping Stability During Training)

**The Problem Even With Good Initialization**:
- Weights change during training
- Input distributions to each layer shift (covariate shift)
- Later layers receive constantly changing inputs
- Layers can still saturate, but now during training

**The BatchNorm Solution**:
For each mini-batch $B = \{x_1, \dots, x_m\}$, compute:

**1. Mean**: $\mu_B = \frac{1}{m}\sum_{i=1}^m x_i$
**2. Variance**: $\sigma_B^2 = \frac{1}{m}\sum_{i=1}^m (x_i - \mu_B)^2$
**3. Normalize**: $\hat{x}_i = \frac{x_i - \mu_B}{\sqrt{\sigma_B^2 + \epsilon}}$
**4. Scale and Shift**: $y_i = \gamma\hat{x}_i + \beta$

Where $\gamma$ and $\beta$ are **learnable parameters** (the network can undo normalization if needed).

**What BatchNorm Does to the Loss Landscape**:
- **Without BN**: Loss surface has narrow curved valleys. Gradient descent zig-zags wildly.
- **With BN**: Loss surface becomes more circular (better conditioning). Gradients point more directly toward the minimum.

**Benefits**:
- Allows much higher learning rates
- Accelerates training dramatically (often 10x)
- Provides mild regularization (since each mini-batch's statistics differ)

**Key Distinction**: BatchNorm is primarily an **optimization technique**, not a regularization technique (though it has side benefits).

### 1.4 Dying ReLU and Its Solutions

Despite being "non-saturating," ReLU has one deadly failure mode:

**The Problem**: If a neuron's weights get updated so it always receives negative inputs, its output is always 0 → gradient is 0 → neuron "dies" forever.

**Solutions**:
- **Leaky ReLU**: $f(x) = \max(0.01x, x)$ (small slope for negatives)
- **ELU**: Exponential for negatives
- **Proper initialization** (He initialization helps prevent dead neurons)

### 1.5 Residual Connections (ResNet) - The Breakthrough

**The Degradation Problem** (2015):
Researchers found that networks beyond ~20-30 layers became **harder** to train—not just in test accuracy, but **training error got worse** with more layers. This was surprising: a deeper model should be able to match a shallower model by learning identity mappings for the extra layers.

**The Residual Solution**:
Instead of learning $H(x)$ directly, learn $H(x) = x + F(x)$ where $F(x)$ is a residual.

**Why This Works for Gradients**:
Backward pass through a residual block:
$$\frac{dL}{dx} = \frac{dL}{dH} \cdot (1 + F'(x))$$

The "1" from the identity connection provides a **gradient highway**. Even if $F'(x)$ is tiny (say 0.01), the gradient can still flow through the identity path without vanishing.

**Numerical Comparison**:
For a 50-layer network where each layer's derivative = 0.9:

**Without residuals**: Gradient multiplier = $0.9^{50} \approx 0.005$ (vanishing)

**With residuals**: Gradient multiplier = $1 + 0.9^{50} \approx 1.005$ (stable!)

**The Implicit Bias**:
Residual networks naturally start near the identity function and learn small corrections. This biases them toward low-complexity solutions—an elegant form of automatic regularization built into the architecture.

### 1.6 Key Review Problems (Test Yourself)

**1. Gradient Calculation**:
A 100-layer network with per-layer derivative = 0.8. Compare gradient multipliers:
- Without residuals: $0.8^{100} \approx 2 \times 10^{-10}$
- With residuals: $1 + 0.8^{100} \approx 1$
- **Conclusion**: Residual connections completely solve the vanishing gradient problem.

**2. Why did ReLU help?**
- Sigmoid derivative max = 0.25; ReLU derivative = 1 for positive inputs
- Product of many 1's stays at 1; product of many 0.25's goes to 0
- **Note**: ReLU is not a complete solution—dead neurons remain a risk.

**3. Diagnosing the Problem**:
You observe early layers not learning, gradients near zero. Likely cause:
- **(a) bad initialization** ✓ (most likely)
- (b) bad data (would affect all layers)
- (c) too much regularization (would also affect later layers)

**4. Xavier Initialization Role**:
Fan-in $n$ determines how much variance is multiplied at each layer. Setting $\text{Var}(W) = 1/n$ keeps variance constant, preventing exponential growth or decay of signals.

**5. Why "Temporary Fix"?**
Initialization only ensures good signal propagation at step 1. During training:
- Weights change → variance relationships break
- Distributions drift (covariate shift)
- BatchNorm was needed to continuously maintain stability.

---

## Part 2: Architecture as Inductive Bias

### 2.1 Core Concept

**Inductive bias** = assumptions built into your architecture about what the data looks like.

This determines:
- Which patterns are easy vs. hard to learn
- How much data you need
- Which solutions the network prefers

### 2.2 CNN Inductive Bias

**Key Assumptions** (built into convolutions):
1. **Locality**: Nearby pixels are related (small kernels)
2. **Translation Equivariance**: $f(\text{shift}(x)) = \text{shift}(f(x))$

**Parameter Sharing**: Same filter applied everywhere, dramatically reducing parameters.

**Effect**: CNNs are very data-efficient but may miss long-range dependencies.

### 2.3 Vision Transformer (ViT) - The Alternative

**How ViT Works**:
1. Split image into 16×16 patches (tokens)
2. Linearly project each patch (patch embedding)
3. Add positional embeddings
4. Process through standard Transformer encoder

**The Key Finding** (Dosovitskiy et al., 2020):
- On small datasets (ImageNet only): ViT **underperforms** ResNet
- On large datasets (JFT-300M, 300M images): ViT **matches or exceeds** ResNet

**What This Means**: "Large scale training trumps inductive bias." Transformers have weaker inductive bias but scale better.

### 2.4 MLP-Mixer - The Surprising Result

**The Architecture** (Tolstikhin et al., 2021):
- No convolutions, no attention—just MLPs!
- **Token-mixing MLP**: Mixes information across patches (per channel)
- **Channel-mixing MLP**: Mixes information across channels (per patch)

**The Insight**: What matters is the ability to model **global relationships**, not the specific mechanism (attention vs. convolution vs. MLP).

**Performance**: With large-scale pre-training, MLP-Mixer is competitive with both CNNs and Transformers.

### 2.5 GNNs and Symmetry

**The Symmetry Principle**:
- **Invariance**: $f(Tx) = f(x)$ (output unchanged)
- **Equivariance**: $f(Tx) = Tf(x)$ (output transforms consistently)

**Example: Molecular Property Prediction**
- **Transformer approach**: Serializes molecule arbitrarily → ~65% accuracy with 10k molecules
- **GNN approach**: Respects permutation symmetry → ~80-85% accuracy with same data

**The Lesson**: If you build the symmetry into the architecture, you need far less data. If not, the model must learn it from scratch (requiring massive scale).

### 2.6 Key Review Problems

**1. CNN: Invariant or Equivariant?**
- Convolution: **Equivariant** (output shifts with input)
- Pooling: **Approximately invariant**
- Overall CNN: Neither pure—combines both properties

**2. A model trained on upright images gets 90% accuracy, but 50% when images are rotated.**
- Missing symmetry: Rotation invariance
- Fixes: Rotate data during training, build rotation-equivariant architecture, use specialized layers

**3. For $f(x) = x^2$ with $T(x) = -x$**:
$f(T(x)) = (-x)^2 = x^2 = f(x)$ → **Invariant**

**4. For $f(x) = 2x$ with $T(x) = -x$**:
$f(T(x)) = -2x = T(f(x))$ → **Equivariant**

---

## Part 3: Self-Supervised Learning (SSL)

### 3.1 The Labeling Problem

**Why Supervised Learning Doesn't Scale**:
- Labeling 1M images ≈ $40,000+
- Labeling 1B images ≈ $40M+
- Specialized tasks (medical imaging) cost 3-5× more

**ImageNet Story**: 14M images, 49,000 workers, 2.5 years

### 3.2 Core SSL Framework: Pretext → Transfer

**Step 1: Pretext Task** (no labels needed)
Train a network to predict something about the data itself

**Step 2: Transfer**
Use the encoder for real tasks (linear classifier, fine-tuning, k-NN)

### 3.3 Major SSL Approaches

| Method | Core Idea | Key Result |
|--------|-----------|------------|
| **SimCLR** | Contrastive learning: push positive pairs together, negatives apart | 76.5% linear on ImageNet |
| **MoCo** | Momentum encoder for large dictionary | On par with SimCLR |
| **MAE** | Mask 75% of patches, reconstruct pixels | 87.8% fine-tune on ImageNet-1K |
| **DINO** | Self-distillation with no labels | 80.1% linear, emergent segmentation |
| **CLIP** | Image-text contrastive (400M pairs) | Zero-shot 76.2% on ImageNet |

### 3.4 Contrastive Learning (SimCLR/MoCo)

**How It Works**:
1. Take batch of N images
2. Create 2 augmentations of each (2N total)
3. Define similarity: $s_{i,j} = \frac{\phi(x_i)^T \phi(x_j)}{\|\phi(x_i)\|\|\phi(x_j)\|}$ (cosine similarity)
4. Loss for positive pair $i,j$ (same original image):
   $$L_i = -\log \frac{\exp(s_{i,j}/\tau)}{\sum_{k=1}^{2N} \exp(s_{i,k}/\tau)}$$

**Intuition**: Cross-entropy over "which of the other 2N-1 images came from the same source?"

### 3.5 Masked Autoencoders (MAE)

**The Insight**: Why do masked autoencoders work for vision when they're already huge in NLP?

**Analysis** (He et al., 2021):
1. **Information density**: Language is dense; images have spatial redundancy
2. **Solution**: Mask 75% of patches (none of this 15% masking like BERT!)
3. **Asymmetric design**: Lightweight decoder (heavy encoder sees only visible patches)

**Why MAE Works So Well**:
- Forces holistic understanding (can't just interpolate from neighbors)
- Huge efficiency gains (75% masking = 4× speedup)
- Learn representations that transfer beautifully to detection and segmentation

### 3.6 DINO: Self-Distillation with No Labels

**The Framework**:
- Student network: trained via gradient descent
- Teacher network: momentum average of student (exponential moving average)
- Both networks see different augmented views of the same image
- Student predicts teacher's output distribution

**Emergent Properties** (only with ViT + DINO):
1. **Semantic segmentation**: Attention maps perfectly outline objects (Fig 1 in paper)
2. **Excellent k-NN features**: 78.3% top-1 on ImageNet with small ViT
3. **No supervised data needed**

**Why This Matters**: These properties don't emerge with:
- Supervised ViTs
- ConvNets (even with same self-supervised methods)

### 3.7 CLIP: Vision-Language Learning

**The Scale**: 400 million (image, text) pairs from the internet

**The Training Objective**: Contrastive loss between image and text encoder outputs
- For each image in batch: which of the N captions matches?
- For each caption: which of the N images matches?

**Zero-Shot Classification**:
1. Create text prompts: for each class label, feed "a photo of a {label}"
2. Encode all prompts with text encoder
3. Encode test image with vision encoder
4. Predict class with highest similarity

**Performance**: Matches original ResNet-50 on ImageNet **without ever seeing an ImageNet training example**.

**The Superpower**: CLIP can classify into any set of categories you describe in natural language—no fixed label set!

### 3.8 Comprehensive SSL Review Questions

**1. Problem solving**: You have 100,000 unlabeled medical images and 1,000 labeled ones. What SSL approach would you use?
- **Semi-supervised learning** with contrastive or masked autoencoder pre-training on all 100,000, then fine-tune on the 1,000 labeled.

**2. Why does SSL work better for NLP historically?**
- Semantic density (few words = rich information)
- Universality (language describes any concept)
- Scalability (non-experts can caption easily)

**3. CLIP vs. standard SSL**: What unique capability does CLIP have?
- Zero-shot transfer to **new categories not seen during training**
- Natural language interface for specifying visual concepts

**4. If your MAE model is reconstructing low-level details but not learning semantic features, what might be wrong?**
- Masking ratio too low (needs ~75% for vision)
- Decoder might be too powerful (needs to be light to force semantic encoding)

---

## Part 4: Scaling Laws and Modern AI

### 4.1 The Three Scaling Axes

1. **Model parameters** (size)
2. **Training data** (volume)
3. **Compute** (FLOPs)

**Key Insight**: These must scale **together**. Mismatch leads to suboptimal performance.

### 4.2 What Makes Architectures Scalable?

| Property | Why It Matters |
|----------|----------------|
| **Parallelism** | More GPUs = faster training |
| **Optimization stability** | Can use large batches, high LRs |
| **Hardware alignment** | Matrix ops (GPUs favorite) |
| **Homogeneous design** | Same block repeated = simpler to optimize and distribute |

**Why Transformers Win**:
- Fully parallel computation
- Stable gradients (LayerNorm + residuals)
- Uniform architecture (easy stacking)
- Matrix multiplication heavy (GPU-friendly)

### 4.3 The Evolution Timeline

| Phase | Years | Key Development |
|-------|-------|-----------------|
| 1 | pre-2010 | Shallow networks only |
| 2 | 2010-2015 | Xavier, ReLU, BatchNorm → deep CNNs trainable |
| 3 | 2015 | ResNet → ultra-deep (152 layers!) |
| 4 | 2017+ | Transformers + residuals + LayerNorm |
| 5 | 2020+ | Scaling laws → LLM explosion |

### 4.4 The Core Thesis

> "The last decade of AI progress was not just about bigger models — it was about designing architectures that make optimization stable enough to scale."

**Implication**: Without residuals, normalization, and good initialization, scaling would fail as models would simply be untrainable.

### 4.5 Review Questions

**1. Do Transformers need depth like CNNs?**
- No! Transformers have **global receptive field from layer 1** (attention sees everything)
- Depth still helps for building hierarchical representations
- But not required for basic function like in CNNs

**2. Why does scaling work for Transformers but not earlier architectures?**
- Earlier models (RNNs, early CNNs) had optimization instabilities that worsened with scale
- Transformers had residuals (gradient highway) and normalization from the start

---

## Exam Preparation Summary

### Must-Know Equations
| Concept | Formula |
|---------|---------|
| Xavier init | $\text{Var}(W) = 1/\text{fan}_{\text{in}}$ |
| He init (ReLU) | $\text{Var}(W) = 2/\text{fan}_{\text{in}}$ |
| Residual gradient | $dL/dx = dL/dH \cdot (1 + F'(x))$ |
| Contrastive loss (InfoNCE) | $L_i = -\log(\exp(s_{i,j}/\tau)/\sum_k \exp(s_{i,k}/\tau))$ |

### Key Paper Takeaways
- **ViT** (Dosovitskiy): Large scale trumps inductive bias
- **MLP-Mixer** (Tolstikhin): Global relationships, not specific mechanisms, matter
- **DINO** (Caron): Self-supervision gives ViTs emergent segmentation properties
- **CLIP** (Radford): 400M image-text pairs enable zero-shot classification
- **MAE** (He): Mask 75% with asymmetric encoder-decoder

### For the Cheat Sheet (Open-Book Section)
1. All key equations
2. Timeline (2010-2025)
3. Architecture-data type mapping table
4. SSL method comparison table (SimCLR, MoCo, MAE, DINO, CLIP)
5. Key paper authors and 1-sentence contributions

Good luck with your preparation! The key insight to remember: **Architecture determines optimization, optimization determines what's learnable, and learnability determines what scales.**