#pragma once

#include "Player.hpp"
#include "PlayerStream.hpp"

#include <iterator>
#include <unordered_map>
#include <vector>

struct RankingResult {
    std::vector<Player> top_;
    std::unordered_map<size_t, size_t> cutoffs_;
    double elapsed_;

    RankingResult(const std::vector<Player>& top = {}, const std::unordered_map<size_t, size_t>& cutoffs = {}, double elapsed = 0);
};

namespace Offline {
RankingResult quickSelectRank(std::vector<Player>& players);
RankingResult heapRank(std::vector<Player>& players);
};

namespace Online {
using PlayerIt = std::vector<Player>::iterator;

void replaceMin(PlayerIt first, PlayerIt last, Player& target);
RankingResult rankIncoming(PlayerStream& stream, const size_t& reporting_interval);
};
