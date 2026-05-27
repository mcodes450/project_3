#include "Leaderboard.hpp"
#include <algorithm>
#include <chrono>

// Constructor for RankingResult with top players, cutoffs, and elapsed time.
RankingResult::RankingResult(const std::vector<Player>& top, const std::unordered_map<size_t, size_t>& cutoffs, double elapsed)
    : top_{ top }
    , cutoffs_{ cutoffs }
    , elapsed_{ elapsed }
{}

namespace Offline {

    // Helper student function to handle the top 10% calculation uniformly.
    // Calculate the top 10% of players by taking 10% of the total size and rounding down to the nearest integer.
    inline size_t getTopTenCount(size_t total_size) {
        return total_size / 10; // Integer division naturally rounds down for us
    }

    // =========================================================================
    // Part A: Heapsort approach
    // =========================================================================
    RankingResult heapRank(std::vector<Player>& players) {
        // Start tracking time for the selection/sorting operation.
        auto start_time = std::chrono::high_resolution_clock::now();

        size_t n = players.size();
        size_t k = getTopTenCount(n); // Target size of the leaderboard

        std::vector<Player> top_players;

        if (k > 0 && n > 0) {
            // We want to be as efficient as possible, so let's make sure we perform all operations in-place.
            // We will instead use heap operations from the <algorithm> library, which lets us directly manipulate the underlying vector.
            
            // Step 1: Turn the entire input vector into a Max-Heap. 
            // This lets us efficiently pull out the largest level elements iteratively.
            std::make_heap(players.begin(), players.end());

            // Step 2: Our algorithm will leverage the same idea, but we will stop after we have sorted the top 10% of players.
            // We run an early-stopping loops exactly 'k' times.
            for (size_t i = 0; i < k; ++i) {
                // pop_heap swaps the maximum element at index 0 to the back of our active heap range
                // and restores the heap property for the remaining elements.
                std::pop_heap(players.begin(), players.end() - i);
            }

            // Step 3: Collect the results. Because pop_heap moves items sequentially to the end, 
            // the top k elements are automatically sitting at the back of the vector [n - k, n) 
            // sorted in ascending order (lowest level of the top 10% up to the absolute highest).
            top_players.assign(players.end() - k, players.end());
        }

        // Stop timing after all sorting manipulations finish.
        auto end_time = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end_time - start_time).count();

        // cutoffs_ parameter must be empty for all Offline algorithms.
        return RankingResult(top_players, {}, duration);
    }

    // =========================================================================
    // Part B: Quicksort approach
    // =========================================================================
    
    // Student Thought Process: Standard Lomuto partitioning scheme.
    // Partitioning around a pivot means everything is less than the pivot on one side, and greater than the pivot on the other.
    size_t partition(std::vector<Player>& players, size_t left, size_t right) {
        Player pivot = players[right]; // Choose rightmost element as pivot
        size_t i = left;

        for (size_t j = left; j < right; ++j) {
            // Sorting in ascending order based on player level
            if (players[j] < pivot) {
                std::swap(players[i], players[j]);
                i++;
            }
        }
        std::swap(players[i], players[right]); // Put the pivot in its final sorted home
        return i; // Return where the pivot landed
    }

    // Student Thought Process: We will create a quickselect/quicksort hybrid.
    // Depending whether it lies in the top 10%, recurse on the partitions with either quickselect or quicksort.
    void quickSelectSortHelper(std::vector<Player>& players, size_t left, size_t right, size_t target_index) {
        if (left >= right) return;

        size_t pivot_index = partition(players, left, right);

        // Case 1: The pivot landed exactly on our threshold boundary line.
        if (pivot_index == target_index) {
            // If we find a pivot in the top 10%, one of those sides are guaranteed to have entirely players in the top 10%.
            // Everything from pivot_index onwards belongs in the top 10%. We just need to ensure that specific right side is sorted.
            std::sort(players.begin() + pivot_index, players.begin() + right + 1);
        }
        // Case 2: Pivot is to the right of our target line.
        else if (pivot_index > target_index) {
            // This means everything to the right of the pivot is safely inside the top 10%. 
            // We sort that guaranteed right chunk immediately using std::sort.
            std::sort(players.begin() + pivot_index, players.begin() + right + 1);
            
            // However, the boundary split line itself is still somewhere inside the left side,
            // so we continue running Quickselect on the left partition.
            if (pivot_index > 0) {
                quickSelectSortHelper(players, left, pivot_index - 1, target_index);
            }
        }
        // Case 3: Pivot is to the left of our target line.
        else {
            // Elements to the left of the pivot are completely outside the top 10%, so we can completely ignore them.
            // The boundary line and the players we want are strictly in the right partition.
            // We recurse with Quickselect on the right partition to find where the top 10% begins.
            quickSelectSortHelper(players, pivot_index + 1, right, target_index);
        }
    }

    RankingResult quickSelectRank(std::vector<Player>& players) {
        auto start_time = std::chrono::high_resolution_clock::now();

        size_t n = players.size();
        size_t k = getTopTenCount(n);
        std::vector<Player> top_players;

        if (k > 0 && n > 0) {
            // The index where our top 10% elements begin when sorted ascending is (n - k)
            size_t target_index = n - k;
            quickSelectSortHelper(players, 0, n - 1, target_index);

            // Copy out the identified and sorted top elements from the back of the modified array.
            top_players.assign(players.begin() + target_index, players.end());
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end_time - start_time).count();

        // Post: The order of the parameter vector is modified.
        return RankingResult(top_players, {}, duration);
    }
}

namespace Online {

    // =========================================================================
    // Part B: Replace Min
    // =========================================================================
    // replaceMin() is a modified percolateDown() routine that replaces the top (ie. overrides the minimum value) of
    // a min-heap with a new element, then percolates it down to maintain the heap-property.
    void replaceMin(PlayerIt first, PlayerIt last, Player& target) {
        size_t heap_size = std::distance(first, last);
        if (heap_size == 0) return;

        // In our implementation, the first element of the heap (ie. the start parameter) is a valid element of the heap... considered the root.
        // Overwrite the root directly with our target player.
        *first = std::move(target);

        size_t parent = 0;
        size_t child = 2 * parent + 1; // Formula for left child index in a 0-indexed vector array
        Player root_val = *first;

        // Percolate down implementation adapted from textbook, adjusting for 0-based indexing.
        while (child < heap_size) {
            // Find the smaller of the two children to see who we might swap with
            if (child + 1 < heap_size && *(first + (child + 1)) < *(first + child)) {
                child++; // Switch index to right child if it has a smaller level
            }

            // If the parent value is already smaller or equal to the smallest child, min-heap structure is sound!
            if (root_val < *(first + child) || root_val == *(first + child)) {
                break;
            }

            // Otherwise, shift the smaller child up into the parent's slot
            *(first + parent) = std::move(*(first + child));
            parent = child;          // Move down the tree
            child = 2 * parent + 1;  // Re-calculate new left child index
        }
        // Finally drop the saved original root value into its mathematically correct, stable position
        *(first + parent) = std::move(root_val);
    }

    // =========================================================================
    // Part C: Continuous Stream
    // =========================================================================
    RankingResult rankIncoming(PlayerStream& stream, const size_t& reporting_interval) {
        auto start_time = std::chrono::high_resolution_clock::now();
        double elapsed_ms = 0.0;

        std::vector<Player> heap_vec;
        std::unordered_map<size_t, size_t> cutoffs;
        size_t processed_count = 0;

        heap_vec.reserve(reporting_interval);

        // Our online algorithm will read in the contents of a PlayerStream until there are no players left.
        while (stream.remaining() > 0) {
            // Fetch next item. Time spent fetching is excluded per instructions.
            Player p = stream.nextPlayer();

            // Resume timing processing mechanics
            auto loop_start = std::chrono::high_resolution_clock::now();
            processed_count++;

            // While we read, we will maintain a min-heap of size reporting_interval.
            if (heap_vec.size() < reporting_interval) {
                heap_vec.push_back(p);
                // Once we have exactly filled the buffer size, structure it into a min-heap layout
                if (heap_vec.size() == reporting_interval) {
                    // Instead, use a vector, the STL heap operations, & replaceMin()
                    std::make_heap(heap_vec.begin(), heap_vec.end(), std::greater<Player>());
                }
            } else {
                // If the incoming player has a higher level than the smallest element on our leaderboard (the root), 
                // they deserve a spot on the board.
                if (p > heap_vec.front()) {
                    replaceMin(heap_vec.begin(), heap_vec.end(), p); // Swap them into the heap
                }
            }

            // Record the Player level after reading every <reporting_interval> players representing the minimum level required...
            if (processed_count % reporting_interval == 0) {
                // The minimum player level on a min-heap is guaranteed to be at the root front index.
                cutoffs[processed_count] = heap_vec.front().level_;
            }

            auto loop_end = std::chrono::high_resolution_clock::now();
            elapsed_ms += std::chrono::duration<double, std::milli>(loop_end - loop_start).count();
        }

        // Finalize timing tracking for sorting mechanics
        auto finish_start = std::chrono::high_resolution_clock::now();

        // ...including the minimum level after ALL players have been read, regardless of being a multiple of the reporting interval
        if (!heap_vec.empty()) {
            cutoffs[processed_count] = heap_vec.front().level_;
        }

        // After we've exhausted all the players, we will return the players in the heap in sorted order.
        // sorted (least to greatest) order
        // Since it's currently configured as a min-heap, sort_heap with std::greater sorts it ascending.
        std::sort_heap(heap_vec.begin(), heap_vec.end(), std::greater<Player>());

        auto finish_end = std::chrono::high_resolution_clock::now();
        elapsed_ms += std::chrono::duration<double, std::milli>(finish_end - finish_start).count();

        // Post: All elements of the stream are read until there are none remaining.
        return RankingResult(heap_vec, cutoffs, elapsed_ms);
    }
}
