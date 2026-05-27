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
