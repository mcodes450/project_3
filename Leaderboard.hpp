#pragma once

// Header guards prevent redefinition bugs if this file is included across multiple translation units.
#include "Player.hpp"
#include "PlayerStream.hpp"

// Standard library data containers required to manage the leaderboard memory allocations and tracking metrics.
#include <iterator>
#include <unordered_map>
#include <vector>

// This structure bundles all required grading metrics and sorting outputs into a single return object.
struct RankingResult {
    // Holds the final isolated slice of top-performing players sorted in strict ascending order.
    std::vector<Player> top_;

    // Maps streaming progress milestones to the baseline level qualification required at that exact moment.
    std::unordered_map<size_t, size_t> cutoffs_;

    // Tracks pure computational execution time in milliseconds, excluding external data stream latency.
    double elapsed_;

    // The constructor uses default arguments to handle empty datasets or early edge-case exits safely.
    RankingResult(const std::vector<Player>& top = {}, const std::unordered_map<size_t, size_t>& cutoffs = {}, double elapsed = 0);
};

// Encloses batch-processing algorithms that require full access to the entire dataset right at launch.
namespace Offline {
    // Rearranges the vector in place using an average linear time QuickSelect routine to isolate the upper tier.
    RankingResult quickSelectRank(std::vector<Player>& players);

    // Organizes data into a max heap structure and extracts the highest elements sequentially in logarithmic time.
    RankingResult heapRank(std::vector<Player>& players);
};

// Encloses streaming operations optimized for processing incoming data sequentially point by point.
namespace Online {
    // Type alias shorthand to keep iterator parameters clean and readable throughout the helper functions.
    using PlayerIt = std::vector<Player>::iterator;

    // Overwrites the min-heap root and runs a heapify-down operation to maintain tree properties efficiently.
    void replaceMin(PlayerIt first, PlayerIt last, Player& target);

    // Reads continuously from a live stream, dynamically updating the buffer and tracking milestone boundaries.
    RankingResult rankIncoming(PlayerStream& stream, const size_t& reporting_interval);
};
