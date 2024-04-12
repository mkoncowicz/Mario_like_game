#pragma once
#include <SFML/Graphics.hpp>

#include <cmath>
#include <chrono>
#include <ranges>
#include <algorithm>

#include "Animation.h"
#include "Enemy.h"

class DogCatcher :
    public Enemy
{
public:
    DogCatcher( const float i_x, const float i_y);

    bool get_dead(const bool i_deletion) const;

    void die(const unsigned char i_death_type);

    void draw(const unsigned i_view_x, sf::RenderWindow& i_window);
    
    void update(const unsigned i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const Map& i_map, Dog& i_dog);

private:
    bool no_collision_dying;
    unsigned short death_timer;

    sf::Sprite sprite;
    sf::Texture texture;

    Animation walk_animation;
};

