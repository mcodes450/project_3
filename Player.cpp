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
 * ======================================================================
 */

#include "PlayerStream.hpp"

// Constructor: Initializes the stream with a sequence of Player objects matching the contents of the given vector.
VectorPlayerStream::VectorPlayerStream(const std::vector<Player>& players)
    : players_(players)
    , current_index_(0) // Start at the beginning of our simulated stream
{}

// Retrieves the next Player in the stream.
Player VectorPlayerStream::nextPlayer() {
    // Throws std::runtime_error If there are no more players remaining in the stream.
    if (current_index_ >= players_.size()) {
        throw std::runtime_error("No more players remaining in the stream.");
    }
    
    // Return the current player, then increment the tracker to yield the next player on subsequent calls.
    return players_[current_index_++];
}

// Returns the number of players remaining in the stream.
size_t VectorPlayerStream::remaining() const {
    // Simple arithmetic: total elements minus what we've already fetched
    return players_.size() - current_index_;
}
