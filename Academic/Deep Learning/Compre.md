# Comprehensive Exam Notes: Deep Learning Architecture, Optimization, and Learning Paradigms

---

## Major Topic 1: Optimization Stability in Deep Networks

### Summary
Deep networks face fundamental optimization challenges due to vanishing/exploding gradients. The chain rule causes products of Jacobians, making gradient magnitudes scale exponentially with depth. This was addressed through three key innovations: proper initialization (Xavier/He), batch normalization, and residual connections. Each solved a different aspect of the instability problem.

### In-Depth

#### Vanishing/Exploding Gradients Problem
- **Chain rule effect**: ∂L/∂W₁ = ∏_{l=1}^L f'(z_l)W_l
- With sigmoid/tanh: f'(z) ∈ [0, 0.25], products become exponentially small → gradients vanish
- With large weights: products explode exponentially
- Depth amplifies numerical instabilities

#### Xavier/He Initialization (2010-2015)
- **Core principle**: Variance preservation - Var(a_l) ≈ Var(a_{l-1})
- **Derivation**: Var(a_l) ≈ n·Var(W)·Var(a_{l-1})
- **Solution**: n·Var(W) = 1 → Var(W) = 1/fan_in
- **He initialization for ReLU**: Var(W) = 2/fan_in (accounts for half activations being zero)
- **Impact**: Enabled training of moderately deep networks (~10-20 layers)

#### Batch Normalization (2015+)
- **Problem addressed**: Even with good initialization, distributions drift during training → layers saturate → gradients vanish
- **Solution**: Force mean=0, variance=1 per mini-batch during training
- **Effects**:
  - Smooths loss landscape (converts narrow curved valleys to circular valleys)
  - Improves conditioning
  - Allows higher learning rates
  - Dramatically speeds training
- **Not just regularization** - primarily an optimization technique

#### Residual Connections (ResNet, 2015)
- **Degradation problem**: Networks deeper than ~20-30 layers had worse training error (not just test error)
- **Key insight**: Depth wasn't the problem; optimization was
- **Residual block**: H(x) = x + F(x)
- **Gradient flow**: dL/dx = dL/dH·(1 + F'(x))
- **Why it works**: Even if F'(x) = 0.01, gradient still flows through identity term
- **Comparison**: Without residual: gradient multiplier = (0.9)^50 ≈ 0.005; With residual: 1 + (0.9)^50 ≈ 1.005
- **Implicit bias**: Networks start near identity, learn small residual corrections → low complexity solutions

#### Dying ReLU Problem
- ReLU has no upper saturation but can die on negative side
- If neuron always receives negative inputs, gradient becomes zero forever
- Solutions: Leaky ReLU, ELU, proper initialization

### Questions

1. **Q**: Calculate the gradient multiplier for a 100-layer network where each layer's derivative is 0.8, with and without residual connections. Compare magnitudes.

2. **Q**: Why did ReLU help with vanishing gradients compared to sigmoid?

3. **Q**: You observe early layers not learning and gradients near zero. Is this more likely due to (a) bad initialization, (b) bad data, or (c) too much regularization? Justify.

4. **Q**: What is the relationship between fan-in and activation variance in Xavier initialization?

5. **Q**: Why is initialization described as a "temporary fix"? What happens during training that degrades initialization benefits?

---

## Major Topic 2: Architecture as Inductive Bias

### Summary
Neural network architectures encode assumptions about data structure (inductive bias). This determines what patterns are easy/hard to learn, which solutions are reachable/preferred, and how well models generalize. Different data types require different architectural symmetries: translation invariance for images, permutation equivariance for graphs, etc. Architecture is not just about representational capacity—it shapes optimization dynamics and generalization.

### In-Depth

#### Inductive Bias Principles
- **Definition**: Assumptions built into architecture about data structure
- **Functions**:
  - Reduces sample complexity
  - Improves generalization
  - Determines which solutions are reachable, preferred, and generalize
- **Tradeoff**: Strong bias helps in low-data regimes; weak bias scales better with large data

#### Data Type ↔ Architecture Mapping

| Data Type | Key Symmetry | Good Architecture |
|-----------|--------------|-------------------|
| Images (2D grid) | Translation | CNN, ViT |
| Sequences/Text | Order (no permutation invariance) | Transformer, RNN |
| Graphs (molecules) | Node permutation | GNN |
| Sets/Point clouds | Permutation invariance | Deep Sets, PointNet |
| 3D objects | Rotation, translation | SE(3)-equivariant nets |
| Time series | Temporal continuity | SSM, RNN variants |
| Audio | Local + temporal | CNN + Transformer hybrid |

#### Symmetry Concepts
- **Invariance**: f(Tx) = f(x) — output doesn't change under transformation
- **Equivariance**: f(Tx) = Tf(x) — output transforms consistently with input
- **Parameter sharing**: Same function applied across space (CNN), tokens (Transformer), or nodes (GNN)

#### CNN Translation Properties
- Convolution layers: translation **equivariant** (output shifts when input shifts)
- Pooling layers: approximately translation **invariant**
- Overall CNN: not purely invariant or equivariant; combines both

#### Molecular Property Prediction Case Study
- **Transformer approach**: Treats molecule as sequence (arbitrary ordering)
  - 10k molecules: ~65% accuracy
  - 1M molecules: ~80% accuracy
- **GNN approach**: Respects permutation symmetry
  - 10k molecules: ~80-85% accuracy
  - 1M molecules: ~88-90% accuracy
- **Conclusion**: Built-in symmetry is far more data-efficient

#### 3D Object Recognition Case Study
- Standard Transformer: No rotation invariance → needs dataset with all rotations → poor generalization
- Equivariant model: Encodes rotation symmetry → works with far less data

#### ViT vs CNN Experiments (Dosovitskiy et al.)
- **Finding**: ViT performs worse than CNNs on small datasets
- **But**: When pre-trained on massive datasets (JFT-300M), ViT matches or outperforms CNNs
- **Conclusion**: Data scale can compensate for weaker inductive bias

#### MLP-Mixer Findings (Tolstikhin et al.)
- All-MLP architecture achieves competitive performance with ViT and CNNs
- **Key insight**: Critical factor is ability to model global relationships, not specific mechanism (attention vs. MLP)
- Improves more rapidly with data than ResNets or even ViT

### Questions

1. **Q**: Is a CNN translation invariant or equivariant? Explain for different layer types.

2. **Q**: You have a point cloud classification task. What symmetries are present? Why would an MLP fail? Suggest a suitable architecture.

3. **Q**: Which statements are TRUE?
   - A. Invariance implies equivariance
   - B. Equivariance preserves structure of transformations
   - C. CNNs are translation invariant at all layers
   - D. Parameter sharing enforces symmetry

4. **Q**: Which tasks require invariance vs equivariance? (Optical flow, object detection, graph classification, image segmentation, image classification)

5. **Q**: Explain why Transformers outperform CNNs at large scale but not always in low-data regimes.

6. **Q**: For function f(x)=x² with transformation T(x)=-x, is f invariant, equivariant, or neither? What about f(x)=2x?

---

## Major Topic 3: Representation Learning and Hierarchy

### Summary
Neural networks learn layered representations through progressive abstraction. Shallow layers learn simple features (edges, textures); deep layers learn abstract concepts (objects, semantics). Architecture defines what representations are formed and how information is composed—shaping the feature hierarchy that emerges during learning.

### In-Depth

#### Hierarchical Representation Learning
- **CNN progression**: edges → textures → object parts → objects → scenes
- **Transformer progression**: tokens → local context → phrase-level meaning → sentence semantics
- **GNN progression**: local neighborhoods → motif patterns → global graph structure

#### Depth vs Capacity Distinction
- Depth ≠ just increased capacity
- Depth = progressive abstraction and compositionality
- Each layer builds upon and refines representations from previous layers

#### Feature Engineering Evolution
- **Traditional pipeline**: Handcrafted features → learning (features independent of task)
- **Modern DL pipeline**: End-to-end learning from raw data
- **Key shift**: We now engineer architectures that decide what features can emerge

#### Structured vs Unstructured Data
- Structured/tabular data: Often directly fed into classical ML
- Success of DL primarily in learning from unstructured data (images, text, audio)
- Representation learning = automatically discovering useful features/patterns

### Questions

1. **Q**: How does the hierarchical representation in CNNs differ from that in Transformers?

2. **Q**: Why is depth important beyond just increasing model capacity?

3. **Q**: What does it mean that we now "engineer architectures that decide what features can emerge" rather than engineering features directly?

---

## Major Topic 4: Scaling Laws and Modern AI

### Summary
Scaling laws describe predictable improvements in model performance by increasing three core components: model parameters, training data, and compute. Scaling has become the primary strategy for SOTA AI. Success requires architectures that are stable enough to optimize at scale—Transformers succeeded because they combine stable optimization (residuals + normalization), global information flow (attention), and massive parallelization.

### In-Depth

#### Scaling Law Components
1. **Number of model parameters** (size)
2. **Volume of training data**
3. **Computational power**

#### What Makes Architectures Scalable?
- **Parallelism**: How many computations can be executed simultaneously
  - Transformers: Fully parallel within and across layers (using pipeline parallelism)
  - RNNs: Limited parallelism (sequential by nature)
  - CNNs: Parallel within layer, sequential across layers
- **Optimization stability**: Residuals + normalization prevent gradient issues
- **Hardware efficiency**: Matrix operations favored by GPUs
- **Modular & homogeneous design**: Same block repeated → easier optimization and distribution

#### Architecture Success Timeline
- **Phase 1 (pre-2010)**: Shallow networks, optimization unstable
- **Phase 2 (2010-2015)**: Xavier, ReLU, BatchNorm → deep CNNs trainable
- **Phase 3 (2015)**: ResNet → ultra-deep networks
- **Phase 4 (2017+)**: Transformers + residuals + LayerNorm
- **Phase 5 (2020+)**: Scaling laws → LLM explosion

#### Why Transformers Dominate Scaling
- Fully parallel computation
- Stable gradients (LayerNorm + residuals)
- Uniform architecture (same block repeated)
- Hardware alignment with GPU matrix operations
- Global receptive field from layer 1 (unlike CNNs requiring depth for global context)

#### Critical Insight
> "The last decade of AI progress was not just about bigger models — it was about designing architectures that make optimization stable enough to scale."

### Questions

1. **Q**: Compare Transformers, RNNs, and CNNs on: parallelism, optimization stability, and architectural uniformity.

2. **Q**: Why did CNNs "lose" to Transformers? Did they fail to scale?

3. **Q**: What factors determine whether an architecture scales well?

4. **Q**: Explain the relationship between stable optimization and the ability to scale models.

---

## Major Topic 5: Self-Supervised Learning (SSL)

### Summary
SSL enables learning from raw, unlabeled data by predicting naturally-occurring signals rather than human annotations. This removes the labeling bottleneck that constrained supervised learning. SSL has been wildly successful in NLP (language models) and is now transforming vision and multimodal learning. Key approaches include contrastive learning (SimCLR, MoCo), masked autoencoders (MAE), and multimodal learning (CLIP).

### In-Depth

#### The Labeling Problem
- **ImageNet scale**: 14M labeled images, 49,000 workers, 2.5 years, ~$40k for 1M images
- **Limitations of supervised learning**:
  - Financial wall: Specialized labeling (medical imaging) costs 3-5× more
  - Time: 80% of project time on data wrangling
  - Scale: Internet generates billions of items daily; humans can't keep up

#### SSL Framework: Pretext → Transfer
1. **Pretrain** on pretext task (no labels needed)
2. **Transfer** encoder to downstream tasks (linear classifiers, KNN, fine-tuning)

#### Pretext Task Categories

**Generative** (predict part of input):
- Autoencoders (sparse, denoising, masked)
- Autoregressive models
- Colorization, Inpainting

**Discriminative** (predict something about input):
- Context prediction, Rotation prediction
- Clustering (DeepCluster)
- Contrastive learning

**Multimodal** (use additional signals):
- Video frames, Sound, 3D, Language

#### Key SSL Methods for Vision

| Method | Year | Core Idea | Performance |
|--------|------|-----------|-------------|
| Context Prediction | 2015 | Predict relative position of two patches | ~30% on Places205 |
| Rotation Prediction | 2018 | 4-way classification of rotation angle | ~32% |
| DeepCluster | 2018 | Iterative clustering + pseudo-labeling | ~33% |
| SimCLR | 2020 | Contrastive learning with augmentations | ~70%+ |
| MoCo | 2020 | Momentum encoder + large dictionary | ~70%+ |
| MAE | 2021 | Masked autoencoding with ViT | ~70%+ |
| CLIP | 2021 | Image-text contrastive | Zero-shot |
| DINO | 2021 | Self-distillation with ViT | Unsupervised segmentation |

#### Contrastive Learning (SimCLR/MoCo)
- **Core idea**: Different augmentations of same image should have similar features; different images should have dissimilar features
- **Loss**: InfoNCE (cross-entropy over 2N-1 other views)
- **Similarity**: sᵢⱼ = φ(xᵢ)ᵀφ(xⱼ) / (‖φ(xᵢ)‖·‖φ(xⱼ)‖)
- **Temperature τ** controls sharpness of distribution

#### Masked Autoencoders (MAE)
- **Approach**: Divide image into patches, discard most (e.g., 75%), reconstruct missing patches
- **Why it works**: Forces model to learn semantic understanding, not just low-level statistics
- **Recent dominance**: Dethroned contrastive methods (as of 2021-2022)

#### Multimodal SSL: CLIP
- **Training**: Contrastive loss between image and matching caption
- **Key capability**: Zero-shot classification (classify without fine-tuning)
- **Enables**: DALL-E, Stable Diffusion (bridges vision and language)
- **Prompt engineering**: "a photo of {label}" performs better than just the label

#### DINO (Self-distillation with ViT)
- **Emergent abilities**:
  - Unsupervised object segmentation (attention maps outline objects perfectly)
  - Zero-shot k-NN classification
  - Semantic layout awareness

#### SSL for NLP (Historical Success)
- RNN/Transformer language models trained on raw text
- Scale works better for language due to:
  1. Semantic density (few words give rich information)
  2. Universality (language describes any concept)
  3. Scalability (non-experts can caption; web-scale collection)

#### Evaluation of SSL Methods
- Pretext task performance
- Representation quality (linear evaluation, clustering, t-SNE)
- Transfer learning / downstream task performance
- Robustness and generalization
- Computational efficiency

### Questions

1. **Q**: Why was supervised learning unsustainable for scaling to internet-scale data? List three reasons.

2. **Q**: Calculate the approximate cost to label 1M images at 10 seconds per image with $15/hour wages. What real-world factors would increase this cost?

3. **Q**: Explain the difference between unsupervised learning and self-supervised learning.

4. **Q**: In contrastive learning with a batch of N images (2N augmentations total), if (xᵢ, xⱼ) is a positive pair, how many negatives does the model compare against?

5. **Q**: What are the limitations of evaluating SSL methods on ImageNet? Why is this problematic given the motivation for SSL?

6. **Q**: Why has SSL been more successful in NLP than in vision historically? What changed?

7. **Q**: What is CLIP's "superpower"? How does prompt engineering affect its performance?

---

## Major Topic 6: Generative Modeling

### Summary
Generative models learn the underlying data distribution and can generate new samples. The key principle is that "understanding = ability to generate." Including autoencoders, GANs, diffusion models, and autoregressive models. Diffusion models power modern image generation (DALL-E, Stable Diffusion, Midjourney) with simple objectives (denoising) + massive scale.

### In-Depth

#### Autoencoders
- **Architecture**: Encoder φ, Decoder ψ, bottleneck h ∈ ℝᴴ (H < D)
- **Loss**: L(x) = ‖x - x̂‖₂² (reconstruction error)
- **Sparse autoencoder**: Add sparsity penalty L = ‖x - x̂‖₂² + λ‖h‖₁
- **Limitation**: H < D is only thing forcing nontrivial representations

#### Diffusion Models
- **Forward process**: Gradually add noise to data
- **Reverse denoising process**: Learn to denoise step by step
- **Key insight**: Simple objective (denoising) + scale → powerful generation
- **Modern applications**: DALL-E, Stable Diffusion, Sora

#### Autoregressive Models
- **Core idea**: Predict next token given previous tokens
- **Application**: LLMs (GPT series)
- **Key**: "Complex behavior emerges from simple training + large scale"

### Questions

1. **Q**: How does a sparse autoencoder force meaningful representations beyond just bottleneck dimension?

2. **Q**: What does "understanding = ability to generate" mean in the context of generative modeling?

3. **Q**: Compare the objectives of diffusion models and autoregressive models. What common principle do they share?

---

## Major Topic 7: Key Review Questions Compilation

### Architecture-Optimization Questions

1. **Q**: Why do gradients vanish/explode in deep networks? Explain using chain rule intuition.

2. **Q**: What role does fan-in play in determining activation variance?

3. **Q**: Why does poor initialization make optimization difficult even before training begins?

4. **Q**: What problem does BatchNorm aim to solve beyond initialization?

5. **Q**: Why does normalization improve optimization beyond just stabilizing variance?

6. **Q**: What is the degradation problem ResNet solved? Why was it surprising?

7. **Q**: In a residual block H(x) = x + F(x), if F'(x) = 0.01, compute the gradient multiplier. Compare with non-residual case.

8. **Q**: For a 50-layer network with per-layer derivative 0.9, compare gradient multiplier with and without residual connections.

### Inductive Bias and Symmetry Questions

9. **Q**: A model trained on upright images gets 90% accuracy, but 50% when images are rotated. What symmetry is missing? How would you fix it?

10. **Q**: Why does a standard Transformer struggle with graph data when treating input as an ordered sequence?

11. **Q**: Consider an input image of size 224×224 with a 3×3 kernel. Without parameter sharing, how many parameters needed? With convolution? Compute reduction factor.

### Scaling Questions

12. **Q**: Do Transformers need depth like CNNs do for global receptive fields? Explain.

13. **Q**: What does "Architecture is not just expressivity" mean? What else does architecture determine?

14. **Q**: Explain the statement: "Architecture is implicit regularization via optimization."

### SSL Evaluation Questions

15. **Q**: How would you evaluate whether an SSL method has learned good representations?

16. **Q**: What is the difference between a pretext task and a downstream task in SSL?

17. **Q**: Which is NOT a valid SSL pretext task: (a) predicting rotation, (b) predicting next word, (c) predicting class labels, (d) predicting missing image patches?

---

## Quick Reference: Key Equations and Values

| Concept | Equation |
|---------|----------|
| Gradient product | ∂L/∂W₁ = ∏ f'(z_l)W_l |
| Xavier init | Var(W) = 1/fan_in |
| He init (ReLU) | Var(W) = 2/fan_in |
| Residual forward | H(x) = x + F(x) |
| Residual gradient | dL/dx = dL/dH·(1 + F'(x)) |
| Autoencoder loss | L = ‖x - x̂‖₂² |
| Sparse AE loss | L = ‖x - x̂‖₂² + λ‖h‖₁ |
| Contrastive similarity | sᵢⱼ = φ(xᵢ)ᵀφ(xⱼ) / (‖φ(xᵢ)‖·‖φ(xⱼ)‖) |
| InfoNCE loss | Lᵢ = -log(exp(sᵢⱼ/τ) / Σₖ exp(sᵢₖ/τ)) |

---

## Final Tips for Comprehensive Exam

1. **Reading material emphasis**: The instructor explicitly stated slides are not exhaustive. Focus on referenced sections from:
   - Goodfellow et al., *Deep Learning Book* (T1)
   - Zhang et al., *Dive into Deep Learning* (T2)
   - ViT paper (Dosovitskiy) - Sections 4 and 3
   - MLP-Mixer paper - Introduction, Section 3
   - DINO paper (Emerging Properties in Self-Supervised Vision Transformers)

2. **MCQ preparation**: Expect significant number of very short answer/MCQ questions testing:
   - Identifying correct initialization formulas
   - Recognizing symmetry types
   - Matching architectures to data types
   - Understanding scaling law components

3. **Descriptive questions likely on**:
   - Residual connections and gradient flow
   - Contrastive learning mechanics
   - Inductive bias vs scale tradeoff
   - Why Transformers succeeded where previous architectures struggled

4. **Cheat sheet strategy** (open-book component):
   - Key equations (optimization, initialization, contrastive loss)
   - Timeline of breakthroughs (2010-2025)
   - Architecture-data type mapping table
   - SSL method comparison table

5. **Post-midterm weightage**: Greater weight on post-midterm topics (SSL, scaling laws, modern architectures, inductive bias)