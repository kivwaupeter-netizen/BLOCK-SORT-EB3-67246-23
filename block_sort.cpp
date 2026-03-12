#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <string>
#include <sstream>

long long comparisons = 0;
long long swaps       = 0;

void resetCounters() { comparisons = 0; swaps = 0; }

void insertionSort(std::vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
        int key = arr[i];
        int j   = i - 1;
        while (j >= left) {
            ++comparisons;
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                ++swaps;
                --j;
            } else {
                break;
            }
        }
        arr[j + 1] = key;
    }
}

void merge(std::vector<int>& arr, int left, int mid, int right) {
    std::vector<int> temp;
    temp.reserve(right - left + 1);

    int i = left, j = mid + 1;
    while (i <= mid && j <= right) {
        ++comparisons;
        if (arr[i] <= arr[j]) {
            temp.push_back(arr[i++]);
        } else {
            temp.push_back(arr[j++]);
        }
    }
    while (i <= mid)   temp.push_back(arr[i++]);
    while (j <= right) temp.push_back(arr[j++]);

    for (int k = 0; k < (int)temp.size(); ++k) {
        if (arr[left + k] != temp[k]) ++swaps;
        arr[left + k] = temp[k];
    }
}

void blockSort(std::vector<int>& arr, bool ascending = true) {
    int n = (int)arr.size();
    if (n <= 1) return;

    int blockSize = std::max(1, (int)std::sqrt((double)n));

    for (int start = 0; start < n; start += blockSize) {
        int end = std::min(start + blockSize - 1, n - 1);
        insertionSort(arr, start, end);
    }

    for (int size = blockSize; size < n; size *= 2) {
        for (int left = 0; left < n - size; left += 2 * size) {
            int mid   = left + size - 1;
            int right = std::min(left + 2 * size - 1, n - 1);
            merge(arr, left, mid, right);
        }
    }

    if (!ascending) {
        int lo = 0, hi = n - 1;
        while (lo < hi) {
            std::swap(arr[lo++], arr[hi--]);
            ++swaps;
        }
    }
}

std::vector<int> randomList(int n, int seed = 42) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(-1'000'000, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

void printList(const std::vector<int>& v, int maxShow = 20) {
    int n = (int)v.size();
    if (n <= maxShow) {
        std::cout << "[ ";
        for (int x : v) std::cout << x << " ";
        std::cout << "]";
    } else {
        std::cout << "[ ";
        for (int i = 0; i < 5; ++i) std::cout << v[i] << " ";
        std::cout << "... ";
        for (int i = n - 5; i < n; ++i) std::cout << v[i] << " ";
        std::cout << "]";
    }
    std::cout << "\n";
}

bool isSorted(const std::vector<int>& v, bool ascending = true) {
    for (int i = 1; i < (int)v.size(); ++i) {
        if (ascending  && v[i - 1] > v[i]) return false;
        if (!ascending && v[i - 1] < v[i]) return false;
    }
    return true;
}

void stepByStepDemo() {
    std::cout << "\n";
    std::cout << "==========================================================\n";
    std::cout << "  STEP-BY-STEP DEMO  (small list)\n";
    std::cout << "==========================================================\n";

    std::vector<int> demo = {38, 27, 43, 3, 9, 82, 10};
    int n         = (int)demo.size();
    int blockSize = std::max(1, (int)std::sqrt((double)n));

    std::cout << "Original list : ";
    printList(demo);
    std::cout << "n = " << n << ",  block size = sqrt(" << n << ") = " << blockSize << "\n\n";

    std::cout << "--- Phase 1: Insertion-sort each block ---\n";
    for (int start = 0; start < n; start += blockSize) {
        int end = std::min(start + blockSize - 1, n - 1);
        std::cout << "  Block [" << start << ".." << end << "] before: [ ";
        for (int i = start; i <= end; ++i) std::cout << demo[i] << " ";
        std::cout << "]\n";
        insertionSort(demo, start, end);
        std::cout << "  Block [" << start << ".." << end << "] after : [ ";
        for (int i = start; i <= end; ++i) std::cout << demo[i] << " ";
        std::cout << "]\n\n";
    }
    std::cout << "After Phase 1 : ";
    printList(demo);

    std::cout << "\n--- Phase 2: Bottom-up merge passes ---\n";
    int pass = 1;
    for (int size = blockSize; size < n; size *= 2) {
        std::cout << "  Pass " << pass++ << " (merge window = " << 2 * size << "):\n";
        for (int left = 0; left < n - size; left += 2 * size) {
            int mid   = left + size - 1;
            int right = std::min(left + 2 * size - 1, n - 1);
            std::cout << "    Merge [" << left << ".." << mid << "] with ["
                      << (mid + 1) << ".." << right << "]\n";
            merge(demo, left, mid, right);
            std::cout << "    Result: ";
            printList(demo);
        }
    }
    std::cout << "\nFinal sorted  : ";
    printList(demo);
    std::cout << "Correctly sorted? " << (isSorted(demo) ? "YES" : "NO") << "\n";
}

void runExperiments() {
    std::vector<int> sizes = {1, 2, 3, 4, 5, 10, 250, 999, 9999,
                               89786, 789300, 1780000};

    std::cout << "\n";
    std::cout << "==========================================================\n";
    std::cout << "  PERFORMANCE EXPERIMENTS\n";
    std::cout << "==========================================================\n";
    std::cout << std::left
              << std::setw(12) << "Size"
              << std::setw(18) << "Comparisons"
              << std::setw(14) << "Swaps"
              << std::setw(16) << "Time (ms)"
              << "Sorted?\n";
    std::cout << std::string(72, '-') << "\n";

    for (int sz : sizes) {
        auto arr = randomList(sz, 2024);
        resetCounters();

        auto t1 = std::chrono::high_resolution_clock::now();
        blockSort(arr);
        auto t2 = std::chrono::high_resolution_clock::now();

        double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
        bool ok   = isSorted(arr);

        std::cout << std::left
                  << std::setw(12) << sz
                  << std::setw(18) << comparisons
                  << std::setw(14) << swaps
                  << std::setw(16) << std::fixed << std::setprecision(3) << ms
                  << (ok ? "YES" : "NO") << "\n";
    }
}

void interactiveSort() {
    std::cout << "\n==========================================================\n";
    std::cout << "  INTERACTIVE MODE\n";
    std::cout << "==========================================================\n";
    std::cout << "Enter integers separated by spaces (then press Enter):\n> ";

    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::vector<int> arr;
    int x;
    while (iss >> x) arr.push_back(x);

    if (arr.empty()) { std::cout << "No numbers entered.\n"; return; }

    std::cout << "Order? [A]scending / [D]escending: ";
    char ch;
    std::cin >> ch;
    bool asc = (ch != 'D' && ch != 'd');

    resetCounters();
    auto t1 = std::chrono::high_resolution_clock::now();
    blockSort(arr, asc);
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    std::cout << "\nSorted list   : ";
    printList(arr);
    std::cout << "Comparisons   : " << comparisons << "\n";
    std::cout << "Swaps         : " << swaps       << "\n";
    std::cout << "Time          : " << std::fixed << std::setprecision(3) << ms << " ms\n";
    std::cout << "Correctly sorted? " << (isSorted(arr, asc) ? "YES" : "NO") << "\n";
}

int main() {
    std::cout << "##########################################################\n";
    std::cout << "##           BLOCK SORT  —  C++ Implementation          ##\n";
    std::cout << "##########################################################\n";

    stepByStepDemo();
    runExperiments();
    std::cout << "\n";
    interactiveSort();

    return 0;
}
