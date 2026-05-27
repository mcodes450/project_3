#pragma once

#include "Player.hpp"
#include "PlayerStream.hpp"

#include <iterator>
#include <unordered_map>
#include <vector>

// Student Note: This struct is the "container" for our results. It separates the 
// top players from the performance metrics and threshold data used in the project.
struct RankingResult {
    std::vector<Player> top_;      // The actual leaderboard
    std::unordered_map<size_t, size_t> cutoffs_; // Milestone data for online algorithms[cite: 3]
    double elapsed_;               // Execution time in milliseconds[cite: 3]

    RankingResult(const std::vector<Player>& top = {}, const std::unordered_map<size_t, size_t>& cutoffs = {}, double elapsed = 0);
};

namespace Offline {
/**
 * @brief Uses a mixture of quickselect/quicksort to select and sort the top 10% 
 *        of players with O(log N) memory.
 * @param players A reference to the vector of Player objects to be ranked.
 */
RankingResult quickSelectRank(std::vector<Player>& players);

/**
 * @brief Uses an early-stopping version of heapsort to select and sort the top 10% 
 *        of players in-place.
 * @param players A reference to the vector of Player objects to be ranked.
 */
RankingResult heapRank(std::vector<Player>& players);
};

namespace Online {
using PlayerIt = std::vector<Player>::iterator;

/**
 * @brief A helper method that replaces the minimum element in a min-heap with a 
 *        target value & preserves the heap.
 * Student Note: This is our optimized "swap and percolate" logic to keep the 
 * heap size constant during streaming.
 */
void replaceMin(PlayerIt first, PlayerIt last, Player& target);

/**
 * @brief Exhausts a stream of Players until none are left.
 * Student Note: This handles the "Online" part of the project where we don't 
 * have all data at once.
 */
RankingResult rankIncoming(PlayerStream& stream, const size_t& reporting_interval);
};
