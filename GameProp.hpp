#pragma once
#include <cmath>
#include <utility>
#include <vector>

enum class Direction : int {
  LEFT = 0b1,
  RIGHT = 0b10,
  UP = 0b100,
  DOWN = 0b1000,
};

inline Direction operator|(Direction lhs, Direction rhs) {
  return static_cast<Direction>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

inline Direction operator&(Direction lhs, Direction rhs) {
  return static_cast<Direction>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

enum class GameState { RUNNING, FINISH, PAUSE };

struct Snake {
  int posx, posy;
};

struct Rat {
  int posx, posy;
  int radius;

  std::pair<float, float> centerCoord() const {
    float centerX = posx + radius;
    float centerY = posy + radius;
    return {centerX, centerY};
  }

  // updated can eat
  bool canEat(int x, int y, float collisionThreshold) const {
    float circleCenterX = posx + radius;
    float circleCenterY = posy + radius;
    float distanceSq =
        std::pow(x - circleCenterX, 2) + std::pow(y - circleCenterY, 2);

    // debug
    // std::cout << "distance between snake head and rat = "
    //           << std::sqrt(distanceSq) << "\t radius = " << radius <<
    //           std::endl;

    float radiusSq = std::pow(radius + collisionThreshold, 2);
    return distanceSq <= radiusSq;
  }
};

// Game Properties class :
class GameProperties {
public:
  GameProperties()
      : isBonusLevel(false), snakeSize(1), currentMove(Direction::UP),
        previousMove(Direction::UP), snakeSpeed(150) {}

  // Getter and Setter methods for isBonusLevel
  bool getIsBonusLevel() const { return isBonusLevel; }

  void setIsBonusLevel(bool bonusLevel) { isBonusLevel = bonusLevel; }

  // Getter and Setter methods for snakeSize
  int getSnakeSize() const { return snakeBody.size(); }

  void setSnakeSize(int size) { snakeSize = size; }

  // Getter and Setter methods for snakeBody
  std::vector<Snake> &getSnakeBody() { return snakeBody; }

  const std::vector<Snake> &getImmutableSnake() const { return snakeBody; }

  void addSnake(const Snake &snake) { snakeBody.emplace_back(snake); };

  bool onlyHeadLeft() { return getSnakeSize() == 1; }

  // Getter and Setter methods for currentMove
  Direction getCurrentMove() const { return currentMove; }

  void setCurrentMove(Direction move) { currentMove = move; }

  // Getter and Setter methods for previousMove
  Direction getPreviousMove() const { return previousMove; }

  void setPreviousMove(Direction move) { previousMove = move; }

  // Getter and Setter methods for snakeSpeed
  int getSnakeSpeed() const { return snakeSpeed; }

  void setSnakeSpeed(int speed) { snakeSpeed = speed; }

private:
  bool isBonusLevel;
  int snakeSize;
  std::vector<Snake> snakeBody;
  Direction currentMove;
  Direction previousMove;
  int snakeSpeed;
};

// Player Class
class PlayerProperties {
public:
  PlayerProperties() : score(0), gameState(GameState::RUNNING) {}

  // Getter and Setter methods for score
  int getScore() const { return score; }

  void setScore(int newScore) { score = newScore; }

  // Getter and Setter methods for gameState
  GameState getGameState() const { return gameState; }

  void setGameState(GameState newState) { gameState = newState; }

  // Method to check if the game is over
  bool isGameOver() const { return gameState == GameState::FINISH; }

  // increment score
  void incrementScore() { this->score++; }

private:
  int score;
  GameState gameState;
};