#include "PlayerStream.hpp"

VectorPlayerStream::VectorPlayerStream(const std::vector<Player>& players)
    : players_(players)
    , current_index_(0)
{
}

Player VectorPlayerStream::nextPlayer()
{
    if (current_index_ >= players_.size()) {
        throw std::runtime_error("No more players remaining in the stream.");
    }
    return players_[current_index_++];
}

size_t VectorPlayerStream::remaining() const
{
    return players_.size() - current_index_;
}
