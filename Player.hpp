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
#include <string>

struct Player {
    std::string name_;
    size_t level_;
    size_t id_;
    
    /**
    * @brief Constructs a Player with the given identifier.
    * @param name A const. string reference to be the player name
    * @param level The current level of the Player
    */
    Player(const std::string& name="NONE", const size_t& level = 1);

    /**
     * @brief Defines convenience comparators for Players, 
     * defining notions of equality & ordering on their level.
     */
    bool operator<(const Player& rhs) const;
    bool operator==(const Player& rhs) const;
    bool operator>(const Player& rhs) const;
};
