#pragma once
#include <SFML/Graphics.hpp>
#include <vector>


enum Direction {
    RIGHT = 0,
    UP = 1,
    DOWN = 2,
    LEFT = 3
};
class Animation {
private:
    std::vector<sf::IntRect> frames;
    std::map<Direction, std::vector<sf::IntRect>> framesPerDirection;
    float durationPerFrame;
    float elapsed;
    size_t currentFrame;

public:
    Animation(float duration = 0.1f)
        : durationPerFrame(duration), elapsed(0.f), currentFrame(0) {
    }

    void addFrame(sf::IntRect rect) {
        frames.push_back(rect);
    }

    void addDirectionalFrame(Direction dir, sf::IntRect rect)
    {
        framesPerDirection[dir].push_back(rect);
    }

    void update(float dt, Direction dir, sf::Sprite& sprite) {
        auto& dirFrames = framesPerDirection[dir];// identifing the frames associated with a particular direction, its a refrence to that particula vector
        if (dirFrames.empty()) return;

        elapsed += dt;
        if (elapsed >= durationPerFrame) {
            elapsed = 0.f;
            currentFrame = (currentFrame + 1) % dirFrames.size();
            sprite.setTextureRect(dirFrames[currentFrame]);
        }
    }

    void updateGhost(Direction dir, sf::Sprite& sprite)
    {
        if (frames.empty()) return;

        if (dir < frames.size())
        {
            sprite.setTextureRect(frames[dir]);
        }

    }

    void reset() {
        elapsed = 0.f;
        currentFrame = 0;
    }
};
