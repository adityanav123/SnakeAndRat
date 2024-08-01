#include "GameProp.hpp"
#include "sounds.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <iostream>
#include <string>
#include <vector>

// Global Declarations
SoundManager soundManager;

PlayerProperties player;
GameProperties Game;
Rat rat;
/**/

void initGame(sf::Vector2u gameWindowSize) {
  soundManager.addSound("background", "./assets/sounds/background.wav");
  soundManager.addSound("serpent", "./assets/sounds/serpant-sound.wav");
  soundManager.addSound("chew", "./assets/sounds/single-chew.wav");
  soundManager.addSound("death", "./assets/sounds/funny-death-1.wav");

  soundManager.setVolume("background", 35.f);
  soundManager.setVolume("serpent", 50.f);
  soundManager.loopIt("background");
  soundManager.loopIt("serpent");

  soundManager.playSound("background");
  soundManager.playSound("serpent");

  // Set the player's initial score
  player.setScore(1);

  // Set the game's initial properties
  Game.setSnakeSize(1);
  Game.addSnake({10, 10});
  Game.setIsBonusLevel(false);

  // Initialize the rat
  rat.posx = random() % (gameWindowSize.x - 50) + 10;
  rat.posy = random() % (gameWindowSize.y - 50) + 10;
  rat.radius = 10.f;
}

// check bonus level
void checkForBonusLevel() {
  if (player.getScore() % 5 == 0) {
    Game.setIsBonusLevel(true);
    rat.radius = 20.f;
  } else {
    Game.setIsBonusLevel(false);
    rat.radius = 10.f;
  }
}

void incrementScore() {
  player.incrementScore();
  checkForBonusLevel();
}

void bonusLevelBackground(sf::RenderWindow &window) {
  if (Game.getIsBonusLevel()) {
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
// Draw a settings icon in the top-left corner of the window
void drawSettingsIcon(sf::RenderWindow &window) {
  sf::RectangleShape settingsIcon(sf::Vector2f(30.f, 30.f));
  settingsIcon.setFillColor(sf::Color::Blue); // Placeholder color
  settingsIcon.setPosition(10.f, 10.f);
  window.draw(settingsIcon);
}

// gradient of shades of red, currently
// experimental
void drawGradientBackground(sf::RenderWindow &window) {
  if (Game.getIsBonusLevel()) {
    sf::Color startColor(255, 0, 0, 50);
    sf::Color endColor(255, 0, 0, 150);
    int rectCount = 10;

    float gradientHeight = static_cast<float>(window.getSize().y) / rectCount;
    for (int i = 0; i < rectCount; ++i) {
      sf::RectangleShape gradientRect(
          sf::Vector2f(window.getSize().x, gradientHeight));
      float yPos = i * gradientHeight;
      gradientRect.setPosition(0, yPos);

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
  for (const auto &segmentPos : Game.getSnakeBody()) {
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
  for (int i = 1; i < Game.getSnakeSize(); ++i) {
    auto snake = Game.getSnakeBody();
    if (snake[0].posx == snake[i].posx && snake[0].posy == snake[i].posy) {
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
  if (Game.onlyHeadLeft()) {
    auto &snakeHead = Game.getSnakeBody()[0];
    int new_posx =
        snakeHead.posx +
        ((Game.getCurrentMove() & Direction::RIGHT) == Direction::RIGHT
             ? stepCnt
             : 0) -
        ((Game.getCurrentMove() & Direction::LEFT) == Direction::LEFT ? stepCnt
                                                                      : 0);
    int new_posy =
        snakeHead.posy +
        ((Game.getCurrentMove() & Direction::DOWN) == Direction::DOWN ? stepCnt
                                                                      : 0) -
        ((Game.getCurrentMove() & Direction::UP) == Direction::UP ? stepCnt
                                                                  : 0);

    snakeHead.posx = checkPositionOutOfBounds(new_posx, gameWindowSize.x);
    snakeHead.posy = checkPositionOutOfBounds(new_posy, gameWindowSize.y);
  } else {
    auto &snakeBody = Game.getSnakeBody();
    for (int i = Game.getSnakeSize() - 2; i >= 0; --i) {
      snakeBody[i + 1] = snakeBody[i];
    }

    // update snake head
    int new_posx =
        snakeBody[0].posx +
        ((Game.getCurrentMove() & Direction::RIGHT) == Direction::RIGHT
             ? stepCnt
             : 0) -
        ((Game.getCurrentMove() & Direction::LEFT) == Direction::LEFT ? stepCnt
                                                                      : 0);
    int new_posy =
        snakeBody[0].posy +
        ((Game.getCurrentMove() & Direction::DOWN) == Direction::DOWN ? stepCnt
                                                                      : 0) -
        ((Game.getCurrentMove() & Direction::UP) == Direction::UP ? stepCnt
                                                                  : 0);

    snakeBody[0].posx = checkPositionOutOfBounds(new_posx, gameWindowSize.x);
    snakeBody[0].posy = checkPositionOutOfBounds(new_posy, gameWindowSize.y);
  }
}

void moveSnake(const sf::RenderWindow &window) {
  // control snake speed
  static sf::Clock clock;
  const int movementStepCnt = 25;

  if (clock.getElapsedTime().asMilliseconds() > Game.getSnakeSpeed()) {
    clock.restart();
    sf::Vector2u gameWindowSize = window.getSize();
    updateSnakePosition(gameWindowSize, movementStepCnt);
  }
}

void moveRat(sf::Vector2u gameWindowSize) {
  rat.posx = random() % (gameWindowSize.x - 50) + 10;
  rat.posy = random() % (gameWindowSize.y - 50) + 10;
}
// CHECK IF RAT EATEN
bool eatRat() {
  auto &snakeHead = Game.getSnakeBody()[0];
  return rat.canEat(snakeHead.posx, snakeHead.posy, 15.f);
}

void moveRat(sf::RenderWindow &window) {
  if (eatRat()) {
    incrementScore();
    moveRat(window.getSize());

    auto &snakeBody = Game.getSnakeBody();
    Snake newSnake{snakeBody[Game.getSnakeSize() - 1].posx + 2,
                   snakeBody[Game.getSnakeSize() - 1].posy + 2};

    Game.addSnake(newSnake);

    // make chewing sounds
    soundManager.playChewSound();
  }
}

// SNAKE MOVEMENT CHECK
void handleInput() {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
      Game.getPreviousMove() != Direction::DOWN) {
    Game.setCurrentMove(Direction::UP);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
             Game.getPreviousMove() != Direction::UP) {
    Game.setCurrentMove(Direction::DOWN);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
             Game.getPreviousMove() != Direction::RIGHT) {
    Game.setCurrentMove(Direction::LEFT);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
             Game.getPreviousMove() != Direction::LEFT) {
    Game.setCurrentMove(Direction::RIGHT);
  }
}

void runGame(sf::RenderWindow &window, sf::Font &font) {
  // Game Score
  sf::Text scoreText;
  scoreText.setFont(font);
  scoreText.setCharacterSize(30);
  scoreText.setFillColor(sf::Color::White);

  // main game loop
  while (window.isOpen()) {
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

    if (player.getGameState() == GameState::RUNNING) {
      if (checkSnakeCollision()) {
        soundManager.playDeathSound();
        player.setGameState(GameState::FINISH);
      }

      // movement
      handleInput();
      if (Game.getCurrentMove() == Direction::UP &&
          Game.getPreviousMove() != Direction::DOWN) {
        Game.setPreviousMove(Game.getCurrentMove());
      } else if (Game.getCurrentMove() == Direction::DOWN &&
                 Game.getPreviousMove() != Direction::UP) {
        Game.setPreviousMove(Game.getCurrentMove());
      } else if (Game.getCurrentMove() == Direction::LEFT &&
                 Game.getPreviousMove() != Direction::RIGHT) {
        Game.setPreviousMove(Game.getCurrentMove());
      } else if (Game.getCurrentMove() == Direction::RIGHT &&
                 Game.getPreviousMove() != Direction::LEFT) {
        Game.setPreviousMove(Game.getCurrentMove());
      }

      moveSnake(window);
      moveRat(window);
    }

    window.clear();
    // bonusLevelBackground(window);
    drawGradientBackground(window);
    // experimental

    if (player.getGameState() == GameState::FINISH) {
      sf::Text finishedText("Game FINSIHED; Your Score was : " +
                                std::to_string(player.getScore()) +
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
        player.setGameState(GameState::RUNNING);
        Game.getSnakeBody().clear();
        initGame(window.getSize());
      }
    }
    if (player.getGameState() == GameState::RUNNING) {
      // draw
      drawSnake(window, font);
      drawRat(window, font);

      // show score
      scoreText.setString("Score: " + std::to_string(player.getScore()));
      window.draw(scoreText);

      // bonus level message
      if (Game.getIsBonusLevel()) {
        sf::Text bonusText("Bonus Level!", font, 30);
        bonusText.setPosition(
            (window.getSize().x / 2) - (bonusText.getGlobalBounds().width / 2),
            window.getSize().y / 2 - bonusText.getGlobalBounds().height / 2);
        window.draw(bonusText);
      }
    }
    window.display();
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
  sf::RenderWindow window(sf::VideoMode(800, 800), "Snake And Rat",
                          sf::Style::Resize | sf::Style::Close);

  // fps limit
  // window.setFramerateLimit(60); // 60FPS

  initGame(window.getSize());
  runGame(window, font);

  return 0;
}
