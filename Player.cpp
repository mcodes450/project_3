#include "Player.hpp"

Player::Player(const std::string& name, const size_t& level)
    : name_ { name }
    , level_ { level }
    , id_ { 0 }
{
}

bool Player::operator<(const Player& rhs) const
{
    return level_ < rhs.level_;
}

bool Player::operator==(const Player& rhs) const
{
    return level_ == rhs.level_;
}

bool Player::operator>(const Player& rhs) const
{
    return level_ > rhs.level_;
}
