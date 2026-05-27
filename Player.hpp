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
    // Student Note: Added a virtual destructor to ensure proper cleanup of derived classes.
    virtual ~PlayerStream() = default;

    /**
     * @brief Retrieves the next Player in the stream, if possible.
     * @return The next Player object in the sequence.
     * @post Updates stream members so a subsequent call to
     *      nextPlayer() will yield the next Player in the to-be-read sequence.
     * @throws std::runtime_error, if there are no more players to fetch & nextPlayer() is called.
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
    // Student Design Choice: Using a vector to store the stream data and a size_t 
    // to track our current position. This allows O(1) access to the next player.
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
    * @return The next Player object in the sequence.
    * @throws std::runtime_error If there are no more players remaining in the stream.
    */
    Player nextPlayer() override;

    /**
     * @brief Returns the number of players remaining in the stream.
     * @return The count of players left to be read.
     */
    size_t remaining() const override; 
};
