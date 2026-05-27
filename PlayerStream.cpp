#include "PlayerStream.hpp"

// Constructor initializes the internal player vector and sets the index to the beginning.
VectorPlayerStream::VectorPlayerStream(const std::vector<Player>& players)
    : players_(players)
    , current_index_(0) // Start the stream at the first element of the vector
{}

// Retrieves the next player from the stream if available.
Player VectorPlayerStream::nextPlayer() {
    // Check if we have already processed all elements in our simulated stream vector.
    if (current_index_ >= players_.size()) {
        throw std::runtime_error("No more players remaining in the stream.");
    }
    
    // Return the player at the current position and increment the index for subsequent requests.
    return players_[current_index_++];
}

// Calculates how many players are still waiting to be read from the vector stream.
size_t VectorPlayerStream::remaining() const {
    // The difference between the total size and our current position gives the remaining count.
    return players_.size() - current_index_;
}
