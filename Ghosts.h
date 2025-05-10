#pragma once
#include "entity.h"
#include "animation.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>


    class Ghost : public Entity {
    private:
        sf::Texture texture;
        Animation animation;
        Direction currentDirection;
        sf::Vector2f initialPosition;
        int frameWidth, frameHeight;

    public:
        // Updated constructor to accept a map of frame indexes for each direction
        Ghost(const std::string& spriteSheetPath,
            int frameCount, int frameWidth, int frameHeight,
            float x, float y, float speed, float scale,
            const std::map<Direction, int>& frameIndexes)
            : Entity(x, y, speed),
            animation(0.1f),
            currentDirection(LEFT),
            initialPosition(x, y+20),
            frameWidth(frameWidth),
            frameHeight(frameHeight)
        {
            if (!texture.loadFromFile(spriteSheetPath)) {
                std::cerr << "Failed to load ghost texture: " << spriteSheetPath << std::endl;
            }

            sprite.setTexture(texture);
           
            sprite.setScale(scale, scale);
            sprite.setPosition(position);

            // Add frames per direction based on the passed frameIndexes
            for (const auto& pair : frameIndexes) {
                Direction dir = pair.first;
                int frameIndex = pair.second;

                // Add the frame for the specified direction and frame index
                animation.addDirectionalFrame(
                    dir,
                    sf::IntRect(frameIndex * frameWidth, static_cast<int>(dir) * frameHeight, frameWidth, frameHeight)
                );
            }
        }
    


    void Move(Direction dir)  {
        currentDirection = dir;

        switch (dir) {
        case LEFT:  position.x -= speed; break;
        case RIGHT: position.x += speed; break;
        case UP:    position.y -= speed; break;
        case DOWN:  position.y += speed; break;
        }

        sprite.setPosition(position);
    }

    // Call this every frame with deltaTime
    void Update(float deltaTime) {
        animation.update(deltaTime, currentDirection, sprite);
    }

    void Reset() {
        position = initialPosition;
        sprite.setPosition(position);
        animation.reset();
    }

    sf::Vector2f GetPosition() const {
        return position;
    }

    sf::Sprite getSprite() const {
        return sprite;
    }

    bool GhostCollision(const sf::Vector2f& pacmanPosition) const {
        return sprite.getGlobalBounds().intersects(sf::FloatRect(pacmanPosition, sf::Vector2f(40, 40)));
    }
};
