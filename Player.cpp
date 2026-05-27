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

#include "Player.hpp"

// I am setting up the constructor here to initialize a Player object.
// I chose to use a member initializer list because it is more efficient
// than assigning values inside the constructor body, as it avoids creating
// default objects first and then overwriting them immediately after.
Player::Player(const std::string& name, const size_t& level)
    : name_ { name }
    , level_ { level }
    , id_ { 0 }
{
}

// Overloading the less-than operator is critical because our leaderboard
// functions need a way to compare players automatically.
// Since the project asks us to rank players specifically by their game level,
// I am directly comparing the level_ fields of the current instance and the right-hand side.
bool Player::operator<(const Player& rhs) const
{
    return level_ < rhs.level_;
}

// For equality tracking, two players are considered equivalent in the context
// of sorting tier distributions if their levels match perfectly.
// I am implementing this so that lookups or boundary checks can accurately
// evaluate tie conditions during partitioning or heap filtering.
bool Player::operator==(const Player& rhs) const
{
    return level_ == rhs.level_;
}

// Overloading the greater-than operator provides a clean shorthand that we can
// pass to standard library components, like when we construct our min-heap wrapper.
// This allows us to maintain a clean layout without writing custom lambdas
// everywhere we need an inverted sorting comparison.
bool Player::operator>(const Player& rhs) const
{
    return level_ > rhs.level_;
}

/*
 * This project was completed for learning and educational purposes.
 * External resources or assistance may have been consulted to better
 * understand concepts and improve implementation.
 * The work submitted represents my own learning process and effort.
 */
