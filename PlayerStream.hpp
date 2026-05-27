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
    virtual Player nextPlayer() = 0;
    virtual size_t remaining() const = 0;
    virtual ~PlayerStream() = default;
};

/**
 * @brief The interface for a PlayerStream created using the contents of a vector.
 */
class VectorPlayerStream : public PlayerStream {
private:
    std::vector<Player> players_;
    size_t cursor_;

public:
    /**
     * @brief Constructs a VectorPlayerStream from a vector of Players.
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
