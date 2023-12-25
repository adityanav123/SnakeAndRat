#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <iostream>
#include <memory>
#include <unordered_map>

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

class SoundManager {
private:
  std::unordered_map<std::string, std::unique_ptr<SoundPlayer>> soundPlayers;

public:
  SoundManager() {}

  void addSound(const std::string &soundName, const std::string &filePath) {
    soundPlayers[soundName] = std::make_unique<SoundPlayer>(filePath);
  }

  void playSound(const std::string &soundName) {
    auto it = soundPlayers.find(soundName);
    if (it != soundPlayers.end()) {
      it->second->play();
    }
  }

  void setVolume(const std::string &soundName, float volume) {
    auto it = soundPlayers.find(soundName);
    if (it != soundPlayers.end()) {
      it->second->setVolume(volume);
    }
  }

  void loopIt(const std::string &soundName) {
    auto it = soundPlayers.find(soundName);
    if (it != soundPlayers.end()) {
      it->second->loopIt();
    }
  }

  void stopSound(const std::string &soundName) {
    auto it = soundPlayers.find(soundName);
    if (it != soundPlayers.end()) {
      it->second->stop();
    }
  }

  // play chewing and death sounds
  void playChewSound() { playSound("chew"); }

  void playDeathSound() { playSound("death"); }

  // Other sound management methods (will add later)
};

// for other files usage
extern SoundManager soundManager;