#include "Leaderboard.hpp"
#include <chrono>
#include <algorithm>

RankingResult::RankingResult(const std::vector<Player>& top, const std::unordered_map<size_t, size_t>& cutoffs, double elapsed)
    : top_ { top }
    , cutoffs_ { cutoffs }
    , elapsed_ { elapsed }
{
}

namespace Offline {

/**
 * @brief Classical Lomuto-style partition technique that uses the middle 
 * element as a pivot to avoid worst-case O(N^2) behavior on sorted inputs.
 */
size_t partition(std::vector<Player>& players, int low, int high) {
    int mid = low + (high - low) / 2;
    std::swap(players[mid], players[high]);
    Player pivot = players[high];
    
    int i = low;
    for (int j = low; j < high; ++j) {
        if (players[j] < pivot) {
            std::swap(players[i], players[j]);
            i++;
        }
    }
    std::swap(players[i], players[high]);
    return i;
}

/**
 * @brief Hybrid helper that switches strategy dynamically. If a pivot falls within 
 * the top 10% bounds, the right half is fully sorted via quicksort, while 
 * the left side continues selection pruning via quickselect.
 */
void quickQuark(std::vector<Player>& players, int low, int high, int target_idx) {
    if (low >= high) return;
    
    int pivot_idx = static_cast<int>(partition(players, low, high));
    
    if (target_idx == -1) {
        // Pure quicksort branch
        quickQuark(players, low, pivot_idx - 1, -1);
        quickQuark(players, pivot_idx + 1, high, -1);
    } else {
        if (pivot_idx < target_idx) {
            // Target cutoff index lies ahead; skip sorting left side completely
            quickQuark(players, pivot_idx + 1, high, target_idx);
        } else {
            // Pivot is inside the top 10%. The right slice is guaranteed to be 
            // part of the top 10%, meaning it requires standard quicksort.
            quickQuark(players, pivot_idx + 1, high, -1);
            if (pivot_idx > target_idx) {
                // Left slice still overlaps with target_idx; keep running quickselect
                quickQuark(players, low, pivot_idx - 1, target_idx);
            }
        }
    }
}

RankingResult quickSelectRank(std::vector<Player>& players) {
    auto start = std::chrono::high_resolution_clock::now();
    
    size_t n = players.size();
    size_t k = n / 10; // Round down to nearest integer
    int target_idx = static_cast<int>(n - k);
    
    if (k > 0) {
        quickQuark(players, 0, static_cast<int>(n - 1), target_idx);
    }
    
    std::vector<Player> top;
    if (k > 0) {
        top.assign(players.begin() + target_idx, players.end());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    return RankingResult(top, {}, elapsed_ms);
}

RankingResult heapRank(std::vector<Player>& players) {
    auto start = std::chrono::high_resolution_clock::now();
    
    size_t n = players.size();
    size_t k = n / 10; // Round down to nearest integer
    
    if (k > 0) {
        // Build initial max-heap structure in O(N)
        std::make_heap(players.begin(), players.end());
        
        // Successive pops extract largest elements in reverse order,
        // which leaves the top elements sorted in ascending order at the end of the vector.
        for (size_t i = 0; i < k; ++i) {
            std::pop_heap(players.begin(), players.end() - i);
        }
    }
    
    std::vector<Player> top;
    if (k > 0) {
        top.assign(players.end() - k, players.end());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    return RankingResult(top, {}, elapsed_ms);
}

} // namespace Offline

namespace Online {

void replaceMin(PlayerIt first, PlayerIt last, Player& target) {
    size_t size = std::distance(first, last);
    if (size == 0) return;
    
    // Override the current minimum element located at the root
    *first = std::move(target);
    
    // Percolate down using zero-based indexing to restore the min-heap status
    size_t parent = 0;
    while (true) {
        size_t leftChild = 2 * parent + 1;
        size_t rightChild = leftChild + 1;
        if (leftChild >= size) break; // Reached leaf level
        
        size_t minChild = leftChild;
        if (rightChild < size && *(first + rightChild) < *(first + leftChild)) {
            minChild = rightChild;
        }
        
        if (*(first + minChild) < *(first + parent)) {
            std::swap(*(first + parent), *(first + minChild));
            parent = minChild;
        } else {
            break; // Valid heap positioning met
        }
    }
}

RankingResult rankIncoming(PlayerStream& stream, const size_t& reporting_interval) {
    double elapsed_ms = 0;
    std::vector<Player> heap;
    std::unordered_map<size_t, size_t> cutoffs;
    size_t processed_count = 0;
    
    while (stream.remaining() > 0) {
        // Time taken to extract elements from the stream is excluded from the tracker
        Player p = stream.nextPlayer();
        
        auto start = std::chrono::high_resolution_clock::now();
        processed_count++;
        
        if (heap.size() < reporting_interval) {
            heap.push_back(p);
            if (heap.size() == reporting_interval) {
                // Initialize min-heap with std::greater once memory buffer fills up
                std::make_heap(heap.begin(), heap.end(), std::greater<Player>());
            }
        } else {
            // Replace root element if candidate level is greater than current threshold minimum
            if (p > heap.front()) {
                replaceMin(heap.begin(), heap.end(), p);
            }
        }
        
        // Capture milestone snapshots
        if (processed_count % reporting_interval == 0) {
            if (heap.size() == reporting_interval) {
                cutoffs[processed_count] = heap.front().level_;
            } else {
                auto min_it = std::min_element(heap.begin(), heap.end());
                cutoffs[processed_count] = (min_it != heap.end()) ? min_it->level_ : 1;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        elapsed_ms += std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    // Final transformation: sort leaderboard from lowest to highest level
    auto start_sort = std::chrono::high_resolution_clock::now();
    if (!heap.empty()) {
        std::sort(heap.begin(), heap.end());
    }
    
    // Log absolute final cutoff state regardless of reporting interval intervals
    if (processed_count > 0) {
        if (!heap.empty()) {
            cutoffs[processed_count] = heap.front().level_;
        }
    }
    
    auto end_sort = std::chrono::high_resolution_clock::now();
    elapsed_ms += std::chrono::duration<double, std::milli>(end_sort - start_sort).count();
    
    return RankingResult(heap, cutoffs, elapsed_ms);
}

} // namespace Online
