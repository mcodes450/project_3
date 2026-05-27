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
    auto start = std::chrono::high_resolution_clock::now();

    // Compute top 10 percent of items rounding down
    size_t top_count = players.size() / 10;

    if (top_count == 0) {
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        return RankingResult({}, {}, elapsed);
    }

    size_t target_index = players.size() - top_count;

    // Use quickselect partitioning to isolate the top 10 percent largest players to the end
    std::nth_element(players.begin(), players.begin() + target_index, players.end());

    // Sort the isolated upper partition in strict ascending sequence
    std::sort(players.begin() + target_index, players.end());

    std::vector<Player> top_players(players.begin() + target_index, players.end());

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    return RankingResult(top_players, {}, elapsed);
}

RankingResult heapRank(std::vector<Player>& players)
{
    auto start = std::chrono::high_resolution_clock::now();

    size_t top_count = players.size() / 10;

    if (top_count == 0) {
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        return RankingResult({}, {}, elapsed);
    }

    // Construct an initial max heap across the container slice
    std::make_heap(players.begin(), players.end());

    // Execute early stopping heapsort to pop only the necessary top items
    for (size_t i = 0; i < top_count; ++i) {
        std::pop_heap(players.begin(), players.end() - i);
    }

    std::vector<Player> top_players(players.end() - top_count, players.end());

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    return RankingResult(top_players, {}, elapsed);
}

} // namespace Offline

namespace Online {

void replaceMin(PlayerIt first, PlayerIt last, Player& target)
{
    if (first == last) return;

    // Overwrite the root min node position directly with the replacement value
    *first = std::move(target);

    // Track array indices manually to filter elements down the binary layout structure
    size_t index = 0;
    size_t count = std::distance(first, last);

    while (true) {
        size_t left_child = 2 * index + 1;
        size_t right_child = 2 * index + 2;
        size_t smallest = index;

        // Evaluate level values to track structural shifts within a min heap context
        if (left_child < count && *(first + left_child) < *(first + smallest)) {
            smallest = left_child;
        }
        if (right_child < count && *(first + right_child) < *(first + smallest)) {
            smallest = right_child;
        }

        // Keep sinking the elements down until order assertions hold true
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
    std::vector<Player> top_heap;
    std::unordered_map<size_t, size_t> cutoffs;
    double total_elapsed_ms = 0.0;
    size_t processed_count = 0;

    while (stream.remaining() > 0) {
        // Exclude next player retrieval streaming costs from performance duration logs
        Player p = stream.nextPlayer();

        auto start = std::chrono::high_resolution_clock::now();
        processed_count++;

        if (top_heap.size() < reporting_interval) {
            top_heap.push_back(p);
            if (top_heap.size() == reporting_interval) {
                // Initialize internal storage buffer sequence as a min heap layout layout
                std::make_heap(top_heap.begin(), top_heap.end(), std::greater<Player>());
            }
        } else {
            // Conditionally filter incoming data stream against the active leaderboard minimum bound
            if (p.level_ > top_heap.front().level_) {
                replaceMin(top_heap.begin(), top_heap.end(), p);
            }
        }

        // Record active progress limits matching exact interval intervals
        if (processed_count % reporting_interval == 0) {
            if (!top_heap.empty()) {
                cutoffs[processed_count] = top_heap.front().level_;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        total_elapsed_ms += std::chrono::duration<double, std::milli>(end - start).count();
    }

    // Save final status information logging boundaries immediately upon completing all reads
    auto final_start = std::chrono::high_resolution_clock::now();
    if (processed_count > 0 && !top_heap.empty()) {
        if (top_heap.size() < reporting_interval) {
            cutoffs[processed_count] = std::min_element(top_heap.begin(), top_heap.end())->level_;
        } else {
            cutoffs[processed_count] = top_heap.front().level_;
        }
    }

    // Ensure terminal collection results match final tracking sequence sorting specifications
    std::sort(top_heap.begin(), top_heap.end());

    auto final_end = std::chrono::high_resolution_clock::now();
    total_elapsed_ms += std::chrono::duration<double, std::milli>(final_end - final_start).count();

    return RankingResult(top_heap, cutoffs, total_elapsed_ms);
}

} // namespace Online
