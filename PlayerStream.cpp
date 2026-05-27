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

#include "PlayerStream.hpp"

// The constructor sets up our streaming interface by copying the vector input.
// I am tracking the current position using a dedicated size_t index variable,
// initializing it to zero so that the stream always begins reading from the
// very first element of the underlying container.
VectorPlayerStream::VectorPlayerStream(const std::vector<Player>& players)
    : players_(players)
    , current_index_(0)
{
}

// This function acts as an iterator that advances through our player records.
// First, I need a guard clause to check if our tracking index has hit or exceeded
// the total size of the vector, which would mean there are no more players to read.
// If it is out of bounds, I throw a runtime_error immediately to prevent unsafe memory access;
// otherwise, I return the current player and post-increment the index to advance the stream.
Player VectorPlayerStream::nextPlayer()
{
    if (current_index_ >= players_.size()) {
        throw std::runtime_error("No more players remaining in the stream.");
    }
    return players_[current_index_++];
}

// To determine how many elements are left unread, I am using simple subtraction.
// By subtracting our current position index from the overall size of the vector,
// we get an instantaneous, constant-time count of the remaining objects,
// which helps the main loops safely evaluate termination conditions.
size_t VectorPlayerStream::remaining() const
{
    return players_.size() - current_index_;
}

/*
 * This project was completed for learning and educational purposes.
 * External resources or assistance may have been consulted to better
 * understand concepts and improve implementation.
 * The work submitted represents my own learning process and effort.
 */
