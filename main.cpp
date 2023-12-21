#include "sounds.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

enum Direction {
  LEFT = 0b1,
  RIGHT = 0b10,
  UP = 0b100,
  DOWN = 0b1000,
};

enum GameState { RUNNING, FINISH, PAUSE };

struct Snake {
  int posx, posy;
};

struct Rat {
  int posx, posy;
  int radius;

  std::vector<float> centerCoord() {
    float centerX = posx + radius;
    float centerY = posy + radius;
    return {centerX, centerY};
  }

  // method to check if a point lies within a circle
  bool canEat(int x, int y) {
    float distance = std::sqrt(std::pow(x - posx, 2) + std::pow(y - posy, 2));
    float collisionThreshold = 20.f;
    return distance <= 20.f;
  }

} rat;

struct Player {
  int score;
  GameState gameState;

  bool gameOver() { return gameState == GameState::FINISH; }

} player;

// Game Properties
bool isBonusLevel = false;

// snake properties
int snakeSize = 1;
std::vector<Snake> snakeBody;
auto currentMove = Direction::UP;
Direction previousMove = Direction::UP;
static int snakeSpeed = 150;
//////////////////

// Extra Sounds
std::unique_ptr<SoundPlayer> chew =
    std::make_unique<SoundPlayer>("./assets/sounds/single-chew.wav");
std::unique_ptr<SoundPlayer> death =
    std::make_unique<SoundPlayer>("./assets/sounds/funny-death-1.wav");

inline void chewFood() {
  chew->setVolume(100.f);
  chew->play();
}

void unfortunate() {
  death->setVolume(110.f);
  death->play();
}

//////////////////

void initGame(sf::Vector2u gameWindowSize) {
  SoundPlayer *bgm = new SoundPlayer("./assets/sounds/background.wav");
  SoundPlayer *serpantSound =
      new SoundPlayer("./assets/sounds/serpant-sound.wav");

  // set bgm properties
  bgm->setVolume(35.f);
  serpantSound->setVolume(50.f);
  bgm->loopIt();
  serpantSound->loopIt();

  // play bgm
  bgm->play();
  serpantSound->play();

  // set initial position of snake and its food
  snakeBody.push_back({10, 10});
  rat.posx = random() % (gameWindowSize.x - 50) + 10;
  rat.posy = random() % (gameWindowSize.y - 50) + 10;
  rat.radius = 10.f;
}

// check bonus level
void checkForBonusLevel() {
  if (player.score % 5 == 0) {
    isBonusLevel = true;
    rat.radius = 20.f;
    // Change background or apply reddish gradient for bonus level
    // Example: window.clear(sf::Color(255, 100, 100)); // Reddish background
  } else {
    isBonusLevel = false;
    rat.radius = 10.f;
  }
}

void incrementScore() {
  player.score++;
  checkForBonusLevel();
}

void bonusLevelBackground(sf::RenderWindow &window) {
  if (isBonusLevel) {
    sf::RectangleShape redTint(
        sf::Vector2f(window.getSize().x, window.getSize().y));
    redTint.setFillColor(
        sf::Color(255, 0, 0, 100)); // Red color with some transparency
    window.draw(redTint);
  } else {
    window.clear(); // Clear the window without tint during normal levels
  }
}

// SETTINGS
void drawSettingsIcon(sf::RenderWindow &window) {
  // Draw a settings icon in the top-left corner of the window
  sf::RectangleShape settingsIcon(sf::Vector2f(30.f, 30.f));
  settingsIcon.setFillColor(sf::Color::Blue); // Placeholder color
  settingsIcon.setPosition(10.f, 10.f);
  window.draw(settingsIcon);
}

// gradient of red, currently - experimental
void drawGradientBackground(sf::RenderWindow &window) {
  if (isBonusLevel) {
    sf::Color startColor(255, 0, 0, 50); // Light red color
    sf::Color endColor(255, 0, 0, 150);  // Darker red color
    int rectCount = 10; // Number of rectangles for the gradient effect

    float gradientHeight = static_cast<float>(window.getSize().y) / rectCount;
    for (int i = 0; i < rectCount; ++i) {
      sf::RectangleShape gradientRect(
          sf::Vector2f(window.getSize().x, gradientHeight));
      float yPos = i * gradientHeight;
      gradientRect.setPosition(0, yPos);

      // Interpolate color between startColor and endColor based on rectangle
      // position
      float colorRatio = yPos / window.getSize().y;
      sf::Color blendedColor(
          static_cast<sf::Uint8>(startColor.r +
                                 colorRatio * (endColor.r - startColor.r)),
          static_cast<sf::Uint8>(startColor.g +
                                 colorRatio * (endColor.g - startColor.g)),
          static_cast<sf::Uint8>(startColor.b +
                                 colorRatio * (endColor.b - startColor.b)),
          static_cast<sf::Uint8>(startColor.a +
                                 colorRatio * (endColor.a - startColor.a)));
      gradientRect.setFillColor(blendedColor);

      window.draw(gradientRect);
    }
  } else {
    window.clear();
  }
}

// DRAW THE SNAKE
void drawSnake(sf::RenderWindow &window, sf::Font &font) {
  int idx = 0;
  for (const auto &segmentPos : snakeBody) {
    sf::RectangleShape body(sf::Vector2f(20.f, 20.f));
    body.setFillColor(sf::Color::Green);

    // insert text
    sf::Text text((idx == 0) ? "H" : "B", font, 15);
    text.setFillColor(sf::Color::Black);
    text.setStyle(sf::Text::Bold);

    body.setPosition(static_cast<float>(segmentPos.posx),
                     static_cast<float>(segmentPos.posy));
    window.draw(body);

    text.setPosition(static_cast<float>(segmentPos.posx) + 5,
                     static_cast<float>(segmentPos.posy) + 2);
    window.draw(text);
    ++idx;
  }
}

// DRAW THE RAT
void drawRat(sf::RenderWindow &window, sf::Font &font) {
  sf::CircleShape ratCircle(rat.radius);
  ratCircle.setFillColor(sf::Color::Red); // Set rat color

  ratCircle.setPosition(static_cast<float>(rat.posx),
                        static_cast<float>(rat.posy));
  window.draw(ratCircle);

  // Create text "R"
  sf::Text text("R", font, 15);
  text.setFillColor(sf::Color::White);
  text.setStyle(sf::Text::Bold);

  // Get the global bounding rectangle of the rat circle
  sf::FloatRect circleBounds = ratCircle.getGlobalBounds();

  // Position the text at the center of the circle
  text.setPosition(circleBounds.left + circleBounds.width / 2.0f -
                       text.getLocalBounds().width / 2.0f,
                   circleBounds.top + circleBounds.height / 2.0f -
                       text.getLocalBounds().height / 2.0f);
  window.draw(text);
}

// check self collision
bool checkSnakeCollision() {
  for (int i = 1; i < snakeSize; ++i) {
    if (snakeBody[0].posx == snakeBody[i].posx &&
        snakeBody[0].posy == snakeBody[i].posy) {
      return true;
    }
  }
  return false;
}

// SNAKE MOVEMENT METHODS
int checkPositionOutOfBounds(int position, int boundary) {
  if (position < 0) {
    return position + boundary; // Wrap-around on underflow
  } else if (position >= boundary) {
    return position - boundary; // Wrap-around on overflow
  }
  return position; // No change if within bounds
}

void updateSnakePosition(sf::Vector2u gameWindowSize, int stepCnt) {
  if (snakeSize == 1) {
    int new_posx = snakeBody[0].posx +
                   (currentMove & Direction::RIGHT ? stepCnt : 0) -
                   (currentMove & Direction::LEFT ? stepCnt : 0);
    int new_posy = snakeBody[0].posy +
                   (currentMove & Direction::DOWN ? stepCnt : 0) -
                   (currentMove & Direction::UP ? stepCnt : 0);

    snakeBody[0].posx = checkPositionOutOfBounds(new_posx, gameWindowSize.x);
    snakeBody[0].posy = checkPositionOutOfBounds(new_posy, gameWindowSize.y);
  } else {
    for (int i = snakeSize - 2; i >= 0; --i) {
      snakeBody[i + 1] = snakeBody[i];
    }
    // Update head position with overflow/underflow checks
    int new_posx = snakeBody[0].posx +
                   (currentMove & Direction::RIGHT ? stepCnt : 0) -
                   (currentMove & Direction::LEFT ? stepCnt : 0);
    int new_posy = snakeBody[0].posy +
                   (currentMove & Direction::DOWN ? stepCnt : 0) -
                   (currentMove & Direction::UP ? stepCnt : 0);

    snakeBody[0].posx = checkPositionOutOfBounds(new_posx, gameWindowSize.x);
    snakeBody[0].posy = checkPositionOutOfBounds(new_posy, gameWindowSize.y);
  }
}

void moveSnake(const sf::RenderWindow &window) {
  // control snake speed
  static sf::Clock clock;
  const int movementStepCnt = 25;

  if (clock.getElapsedTime().asMilliseconds() > snakeSpeed) {
    clock.restart();

    sf::Vector2u gameWindowSize = window.getSize();

    updateSnakePosition(gameWindowSize, movementStepCnt);
  }
}
/////////////////////

// MOVE RAT////////////////////////
void genRatPos(sf::Vector2u gameWindowSize) {
  rat.posx = random() % (gameWindowSize.x - 50) + 10;
  rat.posy = random() % (gameWindowSize.y - 50) + 10;
}
// CHECK IF RAT EATEN
bool snakeAteRat() { return rat.canEat(snakeBody[0].posx, snakeBody[0].posy); }

void checkAndMoveRat(sf::RenderWindow &window) {
  if (snakeAteRat()) {
    incrementScore();
    // find new coordinates for rat and display
    genRatPos(window.getSize());

    // add a new snake
    snakeBody.push_back(
        {snakeBody[snakeSize - 1].posx + 2, snakeBody[snakeSize - 1].posy + 2});

    // make chewing sounds
    chewFood();
  }
}

////////////////////

// SNAKE MOVEMENT CHECK
void handleInput() {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
      previousMove != Direction::DOWN) {
    currentMove = Direction::UP;
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
             previousMove != Direction::UP) {
    currentMove = Direction::DOWN;
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
             previousMove != Direction::RIGHT) {
    currentMove = Direction::LEFT;
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
             previousMove != Direction::LEFT) {
    currentMove = Direction::RIGHT;
  }
}

int main() {
  // font init
  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/shadows-into-light.ttf")) {
    std::cerr << "Could not load font file" << std::endl;
    return -1;
  }
  // Window creation
  sf::RenderWindow window(sf::VideoMode(500, 300), "Snake And Rat",
                          sf::Style::Resize | sf::Style::Close);

  // can also put fps limit
  //   window.setFramerateLimit(60); // 60FPS

  initGame(window.getSize());

  // Game Score
  sf::Text scoreText;
  scoreText.setFont(font);
  scoreText.setCharacterSize(30);
  scoreText.setFillColor(sf::Color::White);

  // main game loop
  while (window.isOpen()) {
    snakeSize = snakeBody.size();
    sf::Event event;
    // Update the position of the score text
    scoreText.setPosition(window.getSize().x - 130, 20);

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::Resized) {
        // Handle window resize event
        sf::FloatRect visibleArea(0.f, 0.f,
                                  static_cast<float>(event.size.width),
                                  static_cast<float>(event.size.height));
        window.setView(sf::View(visibleArea));
      }
    }

    if (player.gameState == GameState::RUNNING) {
      if (checkSnakeCollision()) {
        unfortunate();
        player.gameState = GameState::FINISH;
      }

      // movement
      handleInput();

      // Check if the new direction is valid based on previous direction
      if (currentMove == Direction::UP && previousMove != Direction::DOWN) {
        previousMove = currentMove;
      } else if (currentMove == Direction::DOWN &&
                 previousMove != Direction::UP) {
        previousMove = currentMove;
      } else if (currentMove == Direction::LEFT &&
                 previousMove != Direction::RIGHT) {
        previousMove = currentMove;
      } else if (currentMove == Direction::RIGHT &&
                 previousMove != Direction::LEFT) {
        previousMove = currentMove;
      }

      // move the snake
      moveSnake(window);
      // check if rat is eaten
      checkAndMoveRat(window);
    }

    window.clear();
    // bonusLevelBackground(window);
    drawGradientBackground(window); // experimental

    if (player.gameState == GameState::FINISH) {
      sf::Text finishedText(
          "Game FINSIHED; Your Score was : " + std::to_string(player.score) +
              "\nPress "
              "'R' to restart",
          font, 30);
      finishedText.setStyle(sf::Text::Style::Bold);
      finishedText.setPosition(
          window.getSize().x / 2 - finishedText.getGlobalBounds().width / 2,
          window.getSize().y / 2 - finishedText.getGlobalBounds().height / 2);
      window.draw(finishedText);

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        // Reset the game on any other key press
        player.gameState = GameState::RUNNING;
        snakeBody.clear();          // Clear snake segments
        initGame(window.getSize()); // Initialize the game again
      }
    }
    if (player.gameState == GameState::RUNNING) {
      // settings
      //   drawSettingsIcon(window);

      // draw
      drawSnake(window, font);
      drawRat(window, font);

      // show score
      scoreText.setString("Score: " + std::to_string(player.score));
      window.draw(scoreText);

      // bonus level message
      if (isBonusLevel) {
        sf::Text bonusText("Bonus Level!", font, 30);
        bonusText.setPosition(
            (window.getSize().x / 2) - (bonusText.getGlobalBounds().width / 2),
            window.getSize().y / 2 - bonusText.getGlobalBounds().height / 2);
        window.draw(bonusText);
      }
    }
    window.display();
  }

  return 0;
}
