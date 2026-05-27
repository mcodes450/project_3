#pragma once
#include "Player.hpp"
#include <vector>
#include <stdexcept>

// Base abstract class interface for streaming players
class PlayerStream {
public:
    virtual ~PlayerStream() = default;
    
    /**
     * @brief Retrieves the next Player in the stream.
     * @return The next Player object in the sequence.
     */
    virtual Player nextPlayer() = 0;

    /**
     * @brief Returns the number of players remaining in the stream.
     * @return The count of players left to be read.
     */
    virtual size_t remaining() const = 0;
};

// Vector-backed implementation of the PlayerStream interface
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
     * @throws std::runtime_error If there are no more players remaining.
     */
    Player nextPlayer() override;

    /**
     * @brief Returns the number of players remaining in the stream.
     */
    size_t remaining() const override;
};
