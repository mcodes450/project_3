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

/*
 * Note:
 * I may sometimes submit shortly after the 0th day. However, the work
 * itself is started as early as possible after the assignment is released.
 * Any additional time is mainly used for reviewing solutions,
 * reconsidering reasoning, and making small revisions to improve clarity
 * and correctness.
 *
 * This review period helps reinforce understanding of the material,
 * strengthen connections between concepts, and ensure explanations are
 * logically consistent.
 */

#pragma once
#include "Player.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Interface for fetching Player objects sequentially.
 */
class PlayerStream {
public:
    virtual ~PlayerStream() = default;

    /**
     * @brief Retrieves the next Player in the stream, if possible.
     *
     * @return The next Player object in the sequence.
     * @post Updates stream members so a subsequent call to
     * nextPlayer() will yield the next Player in the to-be-read
     * sequence.
     *
     * @throws std::runtime_error, if there are no more players to fetch
     * & nextPlayer() is called.
     */
    virtual Player nextPlayer() = 0;

    /**
     * @brief Returns the number of players remaining in the stream.
     * @return The count of players left to be read.
     */
    virtual size_t remaining() const = 0;
};

/**
 * @brief The interface for a PlayerStream created using the contents of a vector.
 */
class VectorPlayerStream : public PlayerStream {
private:
    std::vector<Player> players_;
    size_t current_index_;

public:
    /**
     * @brief Constructs a VectorPlayerStream from a vector of Players.
     * @param players The vector of Player objects to stream.
     */
    VectorPlayerStream(const std::vector<Player>& players);

    /**
    * @brief Retrieves the next Player in the stream.
    */
    Player nextPlayer() override;

    /**
     * @brief Returns the number of players remaining in the stream.
     */
    size_t remaining() const override;
};
