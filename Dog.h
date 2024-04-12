#pragma once
#include <SFML/Graphics.hpp>

#include <memory>
#include <chrono>
#include <cmath>
#include <ranges>
#include <algorithm>
#include <range/v3/action.hpp>

#include "Map.h"
#include "Character.h"
#include "Animation.h"
#include "Bone.h"

class Dog : public Character
{
public:
    
    Dog();
  
    bool get_dead() const;
   
    float get_vertical_speed() const;
   
    float get_x() const;

    void die(const bool i_instant_death);

    void draw_bone(const unsigned i_view_x, sf::RenderWindow& i_window);

    void reset();
   
    void set_position(const float i_x, const float i_y);
 
    void set_vertical_speed(const float i_value);
  
    void update(const unsigned i_view_x, Map& i_map);

    int numberOfCollisionsWithBone();

    int numberOfCoinsCollected();

    void draw(sf::RenderWindow& i_window);
  
    sf::FloatRect get_hit_box() const;

private:
    bool crouching;
    bool dead; 
    bool flipped; 
    bool onGround; 
    int bonesEaten;
    int coinsCollected;

    float enemyBounceSpeed; 
    float horizontal_speed; 
    float vertical_speed; 
    float x;
    float y; 

    unsigned char jump_timer;
    unsigned char powerup_state; 

    unsigned short death_timer; 
    unsigned short growth_timer; 
    unsigned short invincible_timer; 

    std::vector<Bone> bones; 

    sf::Texture texture; 
    sf::Sprite sprite;

    Animation big_walk_animation;
    Animation walk_animation;
};

