/*
 *======================================================================
 * STUDENT & PROJECT INFORMATION
 *======================================================================
 *
 * Student:       Mohammed Uddin
 * Project:       Project 3 - Ranking Up
 * Course:        CSCI 33500 - Software Analysis & Design III
 * Instructor:    Prof. I. Stamos
 * Semester:      Spring 2026
 *
 *======================================================================
 */

#include "Leaderboard.hpp"
#include <algorithm>
#include <chrono>
#include <functional>

RankingResult::RankingResult(const std::vector<Player>& top, const std::unordered_map<size_t, size_t>& cutoffs, double elapsed)
    : top_ { top }
    , cutoffs_ { cutoffs }
    , elapsed_ { elapsed }
{
}

namespace Offline {

RankingResult quickSelectRank(std::vector<Player>& players)
{
    // I am starting the high-resolution clock right here at the absolute entry point
    // of the function so that we capture the total runtime spent executing this algorithm.
    auto start = std::chrono::high_resolution_clock::now();

    // The assignment requires us to find the top 10 percent of players, rounding down.
    // Integer division by 10 naturally handles the truncation for us automatically.
    size_t top_count = players.size() / 10;

    // We must handle the edge case where the dataset is too small to yield a top 10 percent.
    // If top_count evaluates to zero, there is nothing to select or sort, so we can stop
    // early, compute the elapsed time, and return an empty result to avoid useless operations.
    if (top_count == 0) {
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        return RankingResult({}, {}, elapsed);
    }

    // To isolate the top elements, I need to find the exact pivot index where the partition boundary lies.
    // Subtracting the top_count from the total size gives us the starting index of our target upper tier.
    size_t target_index = players.size() - top_count;

    // Instead of sorting everything which takes O(N log N) time, I am using std::nth_element.
    // This executes a QuickSelect partitioning algorithm that rearranges the vector in O(N) average time,
    // ensuring that the element at target_index is exactly what it would be if the vector were fully sorted,
    // and that all elements to its right are greater than or equal to it.
    std::nth_element(players.begin(), players.begin() + target_index, players.end());

    // QuickSelect groups the largest elements at the end, but it does not guarantee they are ordered.
    // Since our leaderboard requires the final output to be strictly sorted from lowest to highest level,
    // I am running a standard sort exclusively on this small upper slice from target_index to the end.
    std::sort(players.begin() + target_index, players.end());

    // Now that the top 10 percent slice is isolated and sorted properly, I populate our output container
    // by constructing a new vector using the iterator range representing our validated leaderboard tier.
    std::vector<Player> top_players(players.begin() + target_index, players.end());

    // The algorithmic work is completely done, so I stop the timer immediately before any return steps.
    // I calculate the total duration in milliseconds by casting the time delta to a double precision value.
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    // The cutoffs map must remain empty for offline algorithms according to the specifications,
    // so I pass an empty map initializer along with our sorted players and timing metrics.
    return RankingResult(top_players, {}, elapsed);
}

RankingResult heapRank(std::vector<Player>& players)
{
    // Initializing our performance tracking timer to record the exact execution window of the heap approach.
    auto start = std::chrono::high_resolution_clock::now();

    // Computing the target size for our top 10 percent threshold using integer truncation division.
    size_t top_count = players.size() / 10;

    // Edge case safety check: if the input size is less than 10, top_count is 0, making ranking impossible.
    // We safely exit early and log the elapsed time up to this point to prevent processing failures downstream.
    if (top_count == 0) {
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        return RankingResult({}, {}, elapsed);
    }

    // To perform heapsort operations, we first convert the raw vector into a valid max-heap structure.
    // This runs std::make_heap in O(N) time, organizing the elements so that the maximum value is at index 0.
    std::make_heap(players.begin(), players.end());

    // We only need to extract the top_count largest elements, not fully sort the entire array.
    // I am looping exactly top_count times, calling std::pop_heap to move the largest remaining element
    // from the root to the end of the active heap range, and then shrinking the heap boundary by i elements.
    // This populates the back of our vector with the largest items in ascending order as the loop progresses.
    for (size_t i = 0; i < top_count; ++i) {
        std::pop_heap(players.begin(), players.end() - i);
    }

    // The largest elements have been sequentially placed at the back of the vector by our partial heap pop loop.
    // I extract this slice by building a new vector from the iterator range starting at the first extracted element.
    std::vector<Player> top_players(players.end() - top_count, players.end());

    // Stopping our execution clock right after extraction to make sure we do not log object return overhead.
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    return RankingResult(top_players, {}, elapsed);
}

} // namespace Offline

namespace Online {

void replaceMin(PlayerIt first, PlayerIt last, Player& target)
{
    // If the iterator range is completely empty, there is nothing to replace, so we return immediately.
    if (first == last) return;

    // To optimize performance, we overwrite the root element (the minimum value) directly with our target player.
    // This avoids deleting and reinserting elements, but it temporarily breaks the min-heap property at the root.
    *first = std::move(target);

    // I need to manually perform a heapify-down operation to sink the new root element to its correct position.
    // I am calculating the size of our heap range and initializing a tracking index variable to zero.
    size_t index = 0;
    size_t count = std::distance(first, last);

    while (true) {
        // Standard binary tree array formulas to calculate the indices of the left and right children.
        size_t left_child = 2 * index + 1;
        size_t right_child = 2 * index + 2;
        size_t smallest = index;

        // I compare the parent with its left child to see if the min-heap ordering property is violated.
        // If the left child exists and has a lower level than our current smallest node, I track its index.
        if (left_child < count && *(first + left_child) < *(first + smallest)) {
            smallest = left_child;
        }
        // I repeat the evaluation for the right child, checking if it is even smaller than our active minimum selection.
        if (right_child < count && *(first + right_child) < *(first + smallest)) {
            smallest = right_child;
        }

        // If the smallest node is no longer our current index, a structural violation exists.
        // I swap the elements to sink the target player down one level, and update our index to continue tracking.
        // If no child is smaller, the min-heap property is successfully restored, and we break out of the loop.
        if (smallest != index) {
            std::swap(*(first + index), *(first + smallest));
            index = smallest;
        } else {
            break;
        }
    }
}

RankingResult rankIncoming(PlayerStream& stream, const size_t& reporting_interval)
{
    // Setting up our local variables: top_heap will house our leaderboard elements,
    // cutoffs will track our milestone values, and total_elapsed_ms will sum up our processing execution windows.
    std::vector<Player> top_heap;
    std::unordered_map<size_t, size_t> cutoffs;
    double total_elapsed_ms = 0.0;
    size_t processed_count = 0;

    // We must read every single player provided by the stream until it runs out of elements.
    while (stream.remaining() > 0) {
        // Important: fetching the next player from the stream is an external I/O dependency.
        // To keep our benchmarking accurate, I call nextPlayer() before starting our timing clock.
        Player p = stream.nextPlayer();

        // Starting our high-resolution stopwatch to measure only the leaderboard insertion and filtering logic.
        auto start = std::chrono::high_resolution_clock::now();
        processed_count++;

        // Phase 1: filling the leaderboard buffer until we reach our designated reporting_interval capacity.
        if (top_heap.size() < reporting_interval) {
            top_heap.push_back(p);
            // The exact moment our buffer hits capacity, we convert it into a min-heap.
            // I pass std::greater<Player>() so that the player with the lowest level is kept at the root index 0.
            if (top_heap.size() == reporting_interval) {
                std::make_heap(top_heap.begin(), top_heap.end(), std::greater<Player>());
            }
        } else {
            // Phase 2: the buffer is full, so we stream-filter incoming entries against the active minimum.
            // If the incoming player's level is greater than the root of our min-heap, they qualify for the leaderboard.
            // We invoke our custom replaceMin function to swap out the old minimum and sink the new value into place.
            if (p.level_ > top_heap.front().level_) {
                replaceMin(top_heap.begin(), top_heap.end(), p);
            }
        }

        // Milestone Tracking: check if the number of processed players matches a reporting interval milestone.
        // If it does, we record the current cutoff requirement, which is simply the root level of our min-heap.
        if (processed_count % reporting_interval == 0) {
            if (!top_heap.empty()) {
                cutoffs[processed_count] = top_heap.front().level_;
            }
        }

        // Stopping our timer for the current streaming step and adding the duration to our cumulative runtime tally.
        auto end = std::chrono::high_resolution_clock::now();
        total_elapsed_ms += std::chrono::duration<double, std::milli>(end - start).count();
    }

    // Terminal Logging: the prompt specifies that we must log a final cutoff entry after processing all players,
    // regardless of whether the final count falls on an exact multiple of the reporting interval.
    auto final_start = std::chrono::high_resolution_clock::now();
    if (processed_count > 0 && !top_heap.empty()) {
        // If the stream ended before filling our initial buffer, we find the minimum element manually;
        // otherwise, the root of our min-heap safely represents the definitive leaderboard cutoff boundary.
        if (top_heap.size() < reporting_interval) {
            cutoffs[processed_count] = std::min_element(top_heap.begin(), top_heap.end())->level_;
        } else {
            cutoffs[processed_count] = top_heap.front().level_;
        }
    }

    // Final sorting stage: our internal heap array tracks items using min-heap structure for streaming efficiency,
    // but the final RankingResult specification dictates that the top_ vector must be returned in strict ascending order.
    // I run a final sort pass across the collection to guarantee the required ordering layout.
    std::sort(top_heap.begin(), top_heap.end());

    // Measuring the final sorting and terminal cutoff steps and appending it to our total time counter.
    auto final_end = std::chrono::high_resolution_clock::now();
    total_elapsed_ms += std::chrono::duration<double, std::milli>(final_end - final_start).count();

    return RankingResult(top_heap, cutoffs, total_elapsed_ms);
}

} // namespace Online
