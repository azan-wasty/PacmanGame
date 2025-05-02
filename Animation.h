#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
using namespace std;
using namespace sf;
enum Direction {
    RIGHT = 0,
    UP = 1,
    DOWN = 2,
    LEFT = 3
};
class Animation {
private:
     vector< IntRect> frames;
     map<Direction,  vector< IntRect>> framesPerDirection;
    float durationPerFrame;
    float elapsed;
    size_t currentFrame;

public:
    Animation(float duration = 0.1f)
        : durationPerFrame(duration), elapsed(0.f), currentFrame(0) {
    }

    void addFrame( IntRect rect) {
        frames.push_back(rect);
    }

    void addDirectionalFrame(Direction dir,  IntRect rect)
    {
        framesPerDirection[dir].push_back(rect);
    }

    void update(float dt, Direction dir,  Sprite& sprite) {
        auto& dirFrames = framesPerDirection[dir];// identifing the frames associated with a particular direction, its a refrence to that particula vector
        if (dirFrames.empty()) return;

        elapsed += dt;
        if (elapsed >= durationPerFrame) {
            elapsed = 0.f;
            currentFrame = (currentFrame + 1) % dirFrames.size();
            sprite.setTextureRect(dirFrames[currentFrame]);
        }
    }

    void updatetele(Direction dir,  Sprite& sprite)
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