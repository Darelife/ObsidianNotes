# DSTN Milestone 4 — Predictive Storage Caching & Tiered Data Migration

> **Course:** Data Storage Technologies and Networks **Team:** Satvik Agrawal & Prakhar Bhandari

---

## Table of Contents

1. [[#1. Introduction & Motivation]]
2. [[#2. Project Goals]]
3. [[#3. Background & Literature Survey]]
4. [[#4. System Design & Architecture]]
5. [[#5. Eviction Policies]]
6. [[#6. XGBoost Model Training]]
7. [[#7. Experiments & Results]]
8. [[#8. Implementation Details]]
9. [[#9. Key Learnings]]
10. [[#10. Future Work]]
11. [[#11. Conclusion]]

---

## 1. Introduction & Motivation

Modern GPU-accelerated training workloads face a critical bottleneck known as the **I/O Wall**: GPUs can process data far faster than traditional storage systems can deliver it. This leads to GPU idle time (I/O stalls), wasted compute cycles, and slower model convergence.

The core problem has three dimensions:

- **Compute-Data Mismatch** — Modern GPUs consume data faster than storage can provide it, causing I/O stalls where the GPU literally sits doing nothing.
- **Dataset Scale** — Training datasets (TBs to PBs) are too large to fit entirely in fast GPU memory.
- **Inefficient Caching** — Traditional policies like LRU (Least Recently Used) fail because AI access patterns appear random or lack temporal locality, especially with standard dataset shuffling between epochs.

### Why LRU Fails for AI Training

LRU assumes that recently accessed data is likely to be accessed again soon — this is true for databases, file systems, and web caches. But in AI training, the dataset is **shuffled at every epoch**. Every image is accessed exactly once per epoch in a completely random order. LRU has nothing to exploit.

### The Core Insight

Unlike general-purpose caching, AI training data carries a strong semantic signal: the **training loss**. Loss tells you how hard the model is finding each sample right now. Hard samples (high loss) should stay cached in fast memory. Easy samples (low loss) — the ones the model has already learned — can be safely evicted.

This project builds, implements, and evaluates caching policies that exploit this signal.

---

## 2. Project Goals

By the end of the semester, the goal was to deliver a data-driven caching policy that:

- Is evaluated on **real traces** from AI training workloads (not synthetic data)
- Runs inside a **3-tier simulator** faithfully emulating HBM, DRAM, and NVMe latencies
- **Outperforms LRU** in cache hit rate and I/O latency for the same memory budget

### Why a Trace-Driven Simulator?

Directly modifying GPU memory systems (CUDA kernels, vLLM internals) carries high engineering risk for a semester project — complex C++ memory management, tricky integration, opaque debugging.

Instead:

1. Collect real access traces from PyTorch training (timestamp, block ID, per-sample loss)
2. Feed those traces into a Python simulator that models the 3-tier cache
3. Compare all policies in a fully controlled, reproducible environment

---

## 3. Background & Literature Survey

### 3.1 The Memory Hierarchy

The system targets a single GPU node with three memory tiers. The latency differences between tiers are enormous:

|Memory Tier|Role|Measured Latency|Source|
|---|---|---|---|
|**Tier 0 — GPU HBM**|Target cache state (fastest)|~330 ns (~466 cycles)|Wei et al., arXiv:2402.13499|
|**Tier 1 — Host DRAM**|Intermediate staging|~1.2 µs (1,200 ns)|Sano et al., Kioxia CXL Paper|
|**Tier 2 — NVMe SSD**|Deep storage / miss path|~15–100+ µs (15,000+ ns)|NVIDIA GDS Config Guide|

Coldest data lives on NVMe. The goal is to keep the most valuable blocks in HBM or at least DRAM so the GPU never stalls on slow NVMe reads.

### 3.2 Related Work

#### LMCache

Demonstrates that tiering KV-cache between GPU, CPU, and SSD dramatically improves LLM inference. However, its eviction policies are mostly rule-based — it optimizes _how_ data moves between tiers but not _what_ to move using learned signals.

#### SHADE (Importance-Aware Caching)

Introduces the crucial idea of using **sample difficulty (training loss) as an importance score** for caching. It keeps hard examples in fast memory to accelerate convergence. This project builds directly on this concept and extends it with ML-based prediction.

#### HVAC

Uses local NVMe storage as a cache layer to remove I/O bottlenecks in large-scale distributed deep learning. Focuses on multi-node file system scenarios rather than a single GPU node's HBM/DRAM/NVMe hierarchy — which is our target.

> **Takeaway:** LMCache proves tiered caching matters. SHADE proves that importance scores are powerful eviction signals. HVAC reinforces that smart caching can eliminate I/O bottlenecks at scale. Our contribution is combining ML-based prediction with importance scoring inside a unified 3-tier simulator.

---

## 4. System Design & Architecture

### 4.1 Trace Collection

Rather than guessing with synthetic data, the system records exactly what happens when training ResNet18 on CIFAR-10/CIFAR-100. Every time the GPU requests a sample, a trace record is logged:

```json
{
  "time_ns":  1718000000000,
  "block_id": "IMG_42",
  "example_id": 42,
  "loss": 1.87
}
```

|Field|What It Captures|
|---|---|
|`time_ns`|Nanosecond timestamp (used for recency)|
|`block_id`|String ID for the image (`IMG_<idx>`)|
|`example_id`|Dataset index|
|`loss`|Per-sample cross-entropy loss from last forward pass|

The dataset is shuffled at the image level at the start of every epoch, meaning temporal access order is fully randomized. This is exactly where LRU fails — there is no locality to exploit.

### 4.2 The 3-Tier Simulator

The `TieredSimulator` class models the memory hierarchy. On every data access:

1. Check which tier currently holds the requested block (HBM → DRAM → NVMe)
2. Record a hit (HBM or DRAM) or a miss (NVMe fetch)
3. If the block is not in HBM, promote it — evicting one block from HBM down to DRAM (cascading if DRAM is also full, demoting its victim to NVMe)
4. Accumulate I/O latency based on which tier the block came from

**Tier capacities in simulation:**

|Tier|Capacity|
|---|---|
|HBM|1,000 blocks|
|DRAM|5,000 blocks|
|NVMe|Unlimited|

With a dataset of 20,000 images, HBM holds only **5% of the dataset** at once — creating genuine cache competition where the eviction policy makes a real measurable difference.

### 4.3 Data Migration Flow

When a request arrives for a block currently on NVMe:

```
1. Locate block → found on NVMe (miss, 15,000 ns latency)
2. HBM full? → evict lowest-priority block from HBM → move to DRAM
3. DRAM full? → evict lowest-priority block from DRAM → move to NVMe
4. Place requested block into HBM
5. Update all policy state trackers
```

> **Note:** There is no prefetching in the current system. All decisions are reactive (made after an access miss). Predictive prefetching is identified as future work.

---

## 5. Eviction Policies

### 5.1 LRU — Baseline

Least Recently Used evicts whichever block was accessed least recently. Works great for workloads with strong temporal locality. Fails for shuffled training data because every epoch presents all samples in a completely new random order — recency tells you nothing about future importance.

### 5.2 SHADE — Importance-Aware Heuristic

Inspired by the SHADE paper. The importance score for each block is set to its most recent training loss. When eviction is needed, the block with the **lowest loss** (the easiest sample the model has already learned well) is removed.

> **Intuition:** If the model classifies an image correctly with high confidence (low loss), keeping it in fast memory wastes space. A hard image (high loss) that the model struggles with will benefit more from fast retrieval.

### 5.3 XGBoost Predictive — Version 1 (Recency / Frequency / Loss)

The first XGBoost model was trained on traces from ResNet18 on Tiny ImageNet. Features were inspired by classical caching policies:

- **Recency** — how long since this image was last accessed
- **Frequency** — how many times this image has been accessed
- **Loss** — raw per-sample training loss

The model predicted a binary label: _will this block be accessed again soon?_ At eviction time, the block with the lowest predicted `p_hot` score is evicted.

**Result:** Moderate improvement over LRU but fell short of SHADE. In a fully shuffled training loop, recency and frequency carry almost no predictive signal — every image is accessed once per epoch in random order, so all blocks have similar recency and frequency statistics.

### 5.4 XGBoost Predictive — Version 2 (Training-Aware Loss Features)

After diagnosing why Version 1 underperformed, the feature set was completely redesigned around **loss dynamics**, dropping recency and frequency entirely.

#### Feature 1: EMA Loss

Instead of raw loss, track a smoothed loss using an exponential moving average (α = 0.3):

```
EMA[x] = (0.3 × loss_current) + (0.7 × EMA_previous)
```

This captures the **stable difficulty** of a sample over time, filtering out noise from any single forward pass.

#### Feature 2: Loss Trend

The difference between the current loss and the previous loss for that block:

```
trend = loss_current − loss_previous
```

- Positive trend → model is getting _worse_ on this sample (it's getting harder)
- Negative trend → model is learning it well

#### Feature 3: Time Since High Loss

Track the timestamp of the last time a sample triggered a loss above the threshold (1.0):

```python
if loss > 1.0:
    last_high_loss_time[x] = current_timestamp

time_since_high = current_time - last_high_loss_time[x]

# If a sample NEVER had high loss → set time gap to 1,000,000,000,000
# (massive penalty → prime eviction candidate)
```

Samples that have never been hard get a massive time gap, making them prime candidates for eviction. Samples that struggled recently stay cached.

> **Why these features work:** In shuffled training, temporal access patterns are meaningless — but loss dynamics are not. A sample that consistently has high loss across epochs is genuinely hard and benefits from staying in fast memory. EMA smooths noise, Trend captures learning dynamics, and Time Since High Loss provides a recency signal anchored to training difficulty rather than access order.

### 5.5 Contextual Bandit — Set Dueling Policy

The Contextual Bandit wraps both SHADE and XGBoost as competing experts using a **set dueling tournament**:

- **10% of cache sets** are designated leader sets: 5% test Policy A (SHADE), 5% test Policy B (XGBoost)
- **Remaining 90%** follow whoever is currently winning
- A **penalty** is recorded whenever an evicted block is accessed again within 50,000 steps
- The policy with fewer penalties controls the majority sets

This allows the system to adapt online to whichever policy is performing better. In practice, the Bandit's performance closely tracks the better of its two underlying experts.

---

## 6. XGBoost Model Training

### 6.1 Training Data

The XGBoost model (`fresh_xgb_model.json`) is trained on traces collected from ResNet18 on Tiny ImageNet. The trace file contains one record per sample access with timestamp, block ID, and loss value.

### 6.2 Feature Engineering (`m4_train.py`)

```python
# 1. EMA Loss (alpha = 0.3)
df["ema_loss"] = df.groupby("block_id")["loss"].transform(
    lambda x: x.ewm(alpha=0.3, adjust=False).mean()
)

# 2. Loss Trend
df["prev_loss"]   = df.groupby("block_id")["loss"].shift(1)
df["loss_trend"]  = (df["loss"] - df["prev_loss"]).fillna(0.0)

# 3. Time Since High Loss
HIGH_LOSS_THRESHOLD = 1.0
df["last_high_loss_time"] = df["time_ns"].where(df["loss"] > HIGH_LOSS_THRESHOLD)
df["last_high_loss_time"] = df.groupby("block_id")["last_high_loss_time"].ffill()
df["time_since_high_loss"] = (df["time_ns"] - df["last_high_loss_time"]).fillna(1e12)
```

### 6.3 Label & Training

The label is binary: does this block have a next access recorded in the trace? This frames the problem as a **hotness prediction** task.

```python
df["target"] = (
    df.groupby("block_id")["time_ns"].shift(-1).notnull()
).astype(int)

xgb_model = xgb.XGBClassifier(n_estimators=100, max_depth=6)
xgb_model.fit(X, y)
xgb_model.save_model("fresh_xgb_model.json")
```

### 6.4 Inference in the Simulator

During eviction, `PredictivePolicy` builds a feature matrix for all current HBM residents and evicts the block with the lowest `p_hot`:

```python
features = np.array([ema_losses, loss_trends, times_since_high], dtype=np.float32).T

dmatrix  = xgb.DMatrix(
    features,
    feature_names=["ema_loss", "loss_trend", "time_since_high_loss"]
)
hot_probs = model.predict(dmatrix)

evict = current_blocks[int(np.argmin(hot_probs))]  # coldest block leaves
```

---

## 7. Experiments & Results

All experiments use ResNet18 trained on CIFAR-10 or CIFAR-100, batch size 128, 3 epochs, dataset shuffled between epochs.

### Experiment 1 — Small Scale: 5,000 Images, CIFAR-10

With 5,000 images and HBM capacity of 1,000 (20% of dataset), all non-LRU policies achieve 100% overall hit rate by epoch 3 — data fits within HBM + DRAM combined. Confirms the policies work but doesn't stress-test them.

|Policy|HBM Hit Rate|Overall Hit Rate|Latency (ms)|
|---|---|---|---|
|LRU (Baseline)|0.00%|0.00%|75.0|
|SHADE|19.94%|100.00%|5.1|
|**XGBoost v1** (Recency/Freq)|6.90%|100.00%|5.7|
|Contextual Bandit|19.20%|100.00%|5.2|

> XGBoost v1 shows lower HBM hit rate than SHADE — it over-relies on recency/frequency signals that carry no meaning in shuffled training.

### Experiment 2 — Stress Test: 20,000 Images, XGBoost v1 Features

Scaling to 20,000 images forces genuine cache competition. HBM holds only 5% of the dataset.

|Policy|HBM Hit Rate|Overall Hit Rate|Latency (ms)|
|---|---|---|---|
|LRU (Baseline)|0.00%|0.00%|300.0|
|SHADE|4.99%|29.98%|216.4|
|**XGBoost v1** (Recency/Freq)|3.02%|22.80%|236.5|
|Contextual Bandit|4.86%|29.55%|217.6|

LRU is catastrophically bad — 300ms vs ~216ms for the best policies. XGBoost v1 lags SHADE by 7 percentage points. The hypothesis: recency and frequency are useless features here.

### Experiment 3 — Redesigned Features: 20,000 Images, XGBoost v2, CIFAR-10

After switching to EMA Loss, Loss Trend, and Time Since High Loss, XGBoost matches SHADE:

|Policy|HBM Hit Rate|Overall Hit Rate|Latency (ms)|
|---|---|---|---|
|LRU (Baseline)|0.12%|4.93%|286.4|
|SHADE|4.99%|29.98%|216.4|
|**XGBoost v2** (Loss Features)|**5.00%**|**30.00%**|**216.3**|
|Contextual Bandit|4.96%|29.98%|216.4|

XGBoost v2 achieves essentially identical performance to SHADE (within 0.01%) while being a fully trained ML model — not a hardcoded heuristic. Latency drops from 286ms (LRU) to 216ms, a **24.5% reduction**.

### Experiment 4 — Generalization: 20,000 Images, XGBoost v2, CIFAR-100

The XGBoost model was trained on Tiny ImageNet traces and tested **without retraining** on CIFAR-100 — a completely different dataset with 100 classes.

|Policy|HBM Hit Rate|Overall Hit Rate|Latency (ms)|
|---|---|---|---|
|LRU (Baseline)|0.15%|4.75%|286.9|
|SHADE|4.97%|30.00%|216.3|
|**XGBoost v2** (Loss Features)|**5.00%**|**29.99%**|**216.4**|
|Contextual Bandit|4.50%|29.99%|216.4|

Full performance maintained on CIFAR-100 with zero retraining. Loss dynamics generalize across datasets because they capture universal training behavior, not dataset-specific access patterns.

---

## 8. Implementation Details

### 8.1 Key Classes

|Class|Responsibility|
|---|---|
|`EvictionPolicy`|Abstract base with `access()` and `evict()`|
|`LRUPolicy`|`OrderedDict`-based recency tracking|
|`ImportancePolicy`|Per-block importance score map; evict minimum|
|`PredictivePolicy`|EMA/trend/high-loss maps + XGBoost inference|
|`SetDuelingPolicy`|Tournament wrapper around two expert policies|
|`StorageTier`|Block list + capacity + latency constant|
|`TieredSimulator`|Orchestrates all tiers; locate / promote / demote|
|`CachedTrainingDataset`|PyTorch `Dataset` wrapper; intercepts `__getitem__`|

### 8.2 Integration with PyTorch Training

`CachedTrainingDataset` wraps CIFAR and intercepts every data access:

```python
def __getitem__(self, idx):
    block_id = f"IMG_{idx}"
    record = {
        "time_ns":  time.time_ns(),
        "block_id": block_id,
        "loss":     self.loss_history.get(idx, 2.5),  # default = high loss
    }
    with self._lock:
        self.sim.process_trace(record)

    image, label = self.dataset[idx]
    return idx, image, label, current_tier

def update_loss(self, idx: int, loss: float):
    self.loss_history[idx] = loss
```

After each batch, per-sample losses are written back via `update_loss()`, creating a feedback loop between the training signal and the caching policy.

### 8.3 Tier Latencies (Hardcoded in Simulation)

```python
LATENCY = {
    "HBM":  330,    # nanoseconds
    "DRAM": 1200,
    "NVMe": 15000,
}
```

I/O cost is accumulated per image per batch and summed into a total millisecond latency per epoch.

---

## 9. Key Learnings

### Feature Engineering Is Everything

The biggest performance gap was not between ML and non-ML policies — it was between two versions of the same ML policy using different features. XGBoost v1 (recency/frequency/loss) underperformed SHADE. XGBoost v2 (EMA loss/trend/time-since-high-loss) matched it exactly. Domain knowledge about what makes a good signal in your specific problem context is the decisive factor.

### Shuffling Kills Recency Signals

Standard dataset shuffling completely destroys temporal locality. Any policy relying on access recency or frequency in shuffled training degrades toward LRU-level performance. Loss dynamics survive shuffling because they're tied to semantics (what the model finds hard), not access order.

### Importance Awareness Is a Powerful Prior

SHADE's simple heuristic of keeping high-loss samples performs remarkably well. It effectively acts as a near-perfect oracle for "which samples are still being learned," and an ML model has to at least match this to justify its complexity. The lesson: understand your baseline before trying to beat it.

### Generalization Was Surprisingly Strong

XGBoost trained on Tiny ImageNet traces generalized to CIFAR-100 with no performance degradation. EMA Loss, Loss Trend, and Time Since High Loss capture **universal training dynamics** that apply across datasets and model architectures.

### The Bandit Adapts but Doesn't Exceed

The Contextual Bandit performs at the level of its best underlying expert. It never simultaneously outperforms both experts, but it provides safety — it won't collapse to LRU-level performance if one expert degrades.

### Trace-Driven Simulation Is Effective

The trace-driven approach gave full observability, fast iteration, and reproducible experiments without the engineering risk of modifying real GPU systems. The simulation latencies are grounded in published hardware benchmarks, keeping results meaningful.

---

## 10. Future Work

### Predictive Prefetching

The current system is purely reactive. Prefetching would load data into HBM _before_ the GPU requests it. Possible approaches:

- Use XGBoost `p_hot` scores to proactively promote blocks
- Sequence models (RNN / Transformer) trained on access history
- Bandit-based prefetch admission control

Main risk: incorrect prefetches waste bandwidth and displace useful cached blocks. Requires accurate future-access prediction, which is harder than eviction prediction.

### Multi-Node Extension

The current system targets a single GPU node. Multi-node training would require distributed cache coordination, shared importance metadata across workers, and network-aware migration policies.

### Online Model Retraining

The XGBoost model is trained offline on a fixed trace. An online variant could retrain or fine-tune periodically as new loss data accumulates, adapting to training dynamics that change across stages (early training has high loss everywhere; later training has sparser hard samples).

### Richer Feature Sets

Candidate additions:

- Gradient norms (not just loss)
- Curriculum learning signals
- Per-class difficulty clustering
- Cross-epoch access pattern history

---

## 11. Conclusion

This project demonstrates that learning-based caching policies can significantly outperform classical approaches for AI training workloads.

**Key results at a glance:**

|Policy|vs. LRU|Key Insight|
|---|---|---|
|LRU|Baseline (catastrophic)|Zero hit rate in shuffled training|
|SHADE|**−28% latency**|Loss as importance = powerful heuristic|
|XGBoost v1|Partial improvement|Wrong features — recency/freq are useless here|
|XGBoost v2|**Matches SHADE exactly**|Right features — loss dynamics generalize|
|Contextual Bandit|Tracks best expert|Robust, adaptive, but doesn't exceed experts|

The broader lesson: effective caching for AI workloads requires **domain-aware features**. AI training produces rich semantic signals (per-sample loss) that are far more predictive of future access importance than recency or frequency alone. The path forward is to exploit these signals more deeply through richer models, online adaptation, and proactive prefetching.