#include "PlayerStream.hpp"

VectorPlayerStream::VectorPlayerStream(const std::vector<Player>& players)
    : players_(players)
    , cursor_(0)
{}

Player VectorPlayerStream::nextPlayer() {
    // Throws a runtime error if client tries to fetch past the stream bounds
    if (cursor_ >= players_.size()) {
        throw std::runtime_error("No more players remaining in the stream.");
    }
    return players_[cursor_++];
}

size_t VectorPlayerStream::remaining() const {
    return players_.size() - cursor_;
}
