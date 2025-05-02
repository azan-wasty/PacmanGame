#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
using namespace std;
using namespace sf;

class Entity {
protected:
    Sprite sprite;
    Texture texture;
    Vector2f position;
    float speed;

public:
    
	Entity(float x, float y, float spd) : position(x, y), speed(spd) {
		sprite.setPosition(position);
	}

    virtual ~Entity() {}

    
    virtual void LoadTexture(const string& filename) {
        if (texture.loadFromFile(filename)) {
            sprite.setTexture(texture);
        }
    }

    // Shared behavior (non-virtual)
    void SetPosition(float x, float y) {
        position = Vector2f(x, y);
        sprite.setPosition(position);
    }
    
    void Move(float offsetX, float offsetY) {
        position += Vector2f(offsetX, offsetY);
        sprite.setPosition(position);
    }

    void Draw(RenderWindow& window) {
        window.draw(sprite);
    }

    FloatRect GetBounds() const {
        return sprite.getGlobalBounds();
    }

    Vector2f GetPosition() const {
        return position;
    }

    // Abstract behavior for derived classes
    virtual void Update() = 0;        // Must override
    virtual void Reset() = 0;         // Must override
};
