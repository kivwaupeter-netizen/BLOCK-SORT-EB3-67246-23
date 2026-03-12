# Block Sort — C++ Implementation

## Author

| | |
|---|---|
| **Name** | Peter Kivwau Mbithi |
| **Registration Number** | EB3/67246/23 |
| **Language** | C++ (C++17) |
| **Algorithm** | Block Sort (Block Merge Sort) |

---

## How to Compile and Run

```bash
g++ -O2 -std=c++17 -o block_sort block_sort.cpp
./block_sort
```

The program runs three sections automatically:
1. A step-by-step walkthrough on a small list
2. A performance benchmark across all required input sizes
3. An interactive prompt where you enter your own list and choose ascending or descending order

---

## Algorithm Description

Block Sort is a comparison-based sorting algorithm that combines Insertion Sort and a bottom-up Merge Sort. It runs in **O(n log n)** time in all cases.

The algorithm works in two phases:

**Phase 1 — Block subdivision and insertion sort**
The array is divided into blocks of size approximately √n (the square root of the total number of elements). Each block is sorted independently using Insertion Sort. Insertion Sort is chosen here because it performs very efficiently on short, contiguous sequences and operates entirely in place with no extra memory.

**Phase 2 — Bottom-up merge**
Once all blocks are individually sorted, the algorithm merges adjacent pairs of sorted blocks. After the first merge pass, sorted runs are twice the block size. Each subsequent pass doubles the run length again. This continues until one fully sorted run covers the entire array — exactly like the merge phase of a standard Merge Sort, but starting from already-sorted chunks instead of single elements.

If descending order is requested, the array is reversed in O(n) after sorting.

The key advantage of Block Sort over a plain Merge Sort is that pre-sorting blocks with Insertion Sort reduces the total number of comparisons during merging, and by choosing a block size of √n the two phases are balanced optimally.

---

## Step-by-Step Example

**Input:** `[38, 27, 43, 3, 9, 82, 10]`  
n = 7, block size = ⌊√7⌋ = 2

### Phase 1 — Insertion-sort each block

| Block indices | Before | After |
|---|---|---|
| [0 .. 1] | 38  27 | **27  38** |
| [2 .. 3] | 43   3 | **3   43** |
| [4 .. 5] |  9  82 | **9   82** |
| [6 .. 6] | 10     | **10**     |

Array after Phase 1: `[27, 38, 3, 43, 9, 82, 10]`

### Phase 2 — Bottom-up merge passes

**Pass 1 (merge window = 4):**

- Merge [0..1] with [2..3] → `[3, 27, 38, 43, 9, 82, 10]`
- Merge [4..5] with [6..6] → `[3, 27, 38, 43, 9, 10, 82]`

**Pass 2 (merge window = 8):**

- Merge [0..3] with [4..6] → `[3, 9, 10, 27, 38, 43, 82]`

**Final result:** `[3, 9, 10, 27, 38, 43, 82]` ✓

---

## Comparisons and Swaps

Every time two elements are compared inside `insertionSort` or `merge`, the global `comparisons` counter is incremented. Every time an element is shifted during insertion sort, or written back to the array during a merge, the global `swaps` counter is incremented. Both counters are reset to zero before each timed run so that results per input size are independent.

---

## Time Complexity Analysis

| Case | Scenario | Complexity |
|---|---|---|
| **Best case** | Array is already sorted. Each block passes through insertion sort in O(blockSize) comparisons rather than O(blockSize²). The merge passes still scan every element each round, contributing O(n log n). | **O(n log n)** |
| **Average case** | Random input. Insertion sort on each of the n/√n blocks costs O(√n · √n) = O(n) total across Phase 1. There are O(log n) merge passes each touching all n elements, giving O(n log n) for Phase 2. The dominant term is O(n log n). | **O(n log n)** |
| **Worst case** | Reverse-sorted input. Insertion sort degrades to O(blockSize²) per block, but since blockSize = √n, the total cost across all blocks is O((√n)² · n/√n) = O(n · √n). However, the merge phase remains O(n log n), which grows faster than O(n√n) for large n, making O(n log n) the overall worst-case bound. | **O(n log n)** |

---

## Space Complexity

| Component | Space used | Reason |
|---|---|---|
| Insertion sort (Phase 1) | O(1) | Fully in-place; only one integer variable `key` is needed |
| Merge buffer (Phase 2) | O(n) | A temporary vector is allocated to hold the merged result of each pair of runs; in the worst pass this is the full array |
| **Overall** | **O(n)** | Dominated by the merge buffer |

In a fully in-place Block Sort implementation the merge buffer can be reduced to O(√n) by using one of the sorted blocks as a scratch area. This implementation uses a simpler O(n) auxiliary buffer for clarity and correctness.

---

## Experiment Results

All experiments used randomly generated integer arrays with values in the range [−1 000 000, 1 000 000]. Each run used a fixed random seed so results are reproducible. Timing was measured using `std::chrono::high_resolution_clock`.

```
Size        Comparisons       Swaps         Time (ms)    Sorted?
------------------------------------------------------------------------
1           0                 0             0.000        YES
2           1                 0             0.000        YES
3           3                 2             0.000        YES
4           5                 3             0.000        YES
5           6                 8             0.000        YES
10          25                18            0.001        YES
250         2,179             1,997         0.016        YES
999         13,806            13,092        0.082        YES
9,999       323,161           313,719       1.473        YES
89,786      7,550,749         7,462,909     24.759       YES
789,300     183,330,895       182,548,181   582.563      YES
1,780,000   614,210,378       612,440,788   1,871.389    YES
```

### Discussion

**Growth rate of comparisons confirms O(n log n):**  
From n = 999 to n = 9,999 (a 10× increase), comparisons grew from 13,806 to 323,161 — a factor of approximately 23. The theoretical O(n log n) factor for a 10× increase in n is 10 × log₂(10) ≈ 10 × 3.32 = 33. The measured factor is somewhat lower because the insertion-sort phase is very efficient on small blocks and reduces the work the merge phase needs to do, keeping constant factors small.

**Swaps track closely with comparisons:**  
Swaps and comparisons are nearly equal because the merge step writes each element into a temporary buffer and then copies it back, counting a swap on every write that differs from the current value. This means swaps reflect data movement rather than element exchanges in the traditional sense.

**Timing is consistent with O(n log n):**  
From n = 9,999 (1.473 ms) to n = 89,786 (24.759 ms), the input grew by roughly 9× while time grew by roughly 17×. From n = 89,786 to n = 789,300 (an 8.8× increase), time grew by 23.5×. These ratios are consistent with O(n log n) growth across all tested sizes.

**Very small inputs (n ≤ 10):**  
Block size equals 1 or 2, so the algorithm reduces almost entirely to insertion sort on tiny segments. Comparisons and swaps are negligible and timing is below the resolution of the clock.

**Large inputs (n = 1,780,000):**  
The algorithm correctly sorted 1.78 million elements in under 2 seconds on a single core, demonstrating that the O(n log n) implementation scales to production-relevant sizes.

---

## Repository Contents

```
block_sort.cpp    Full C++ source code
README.md         This file
```

---

## Submission

```
EB3/67246/23 – Peter Kivwau Mbithi – https://github.com/YOUR_USERNAME/block-sort
```
