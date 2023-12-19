#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <iostream>

struct SoundPlayer {
  std::string fileName;
  sf::SoundBuffer buffer;
  sf::Sound sound;

  SoundPlayer(std::string _pathAndFileName) : fileName(_pathAndFileName) {
    if (!buffer.loadFromFile(_pathAndFileName)) {
      std::cout << "error loading from file!" << std::endl;
    }
  }

  void play() {
    sound.setBuffer(buffer);
    sound.play();
  }

  void stop() { sound.stop(); }

  void loopIt() { sound.setLoop(true); }

  void pause() { sound.pause(); }

  void setVolume(float qty) { sound.setVolume(qty); }
};
