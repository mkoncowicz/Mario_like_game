#pragma once
#include <SFML/Graphics.hpp>

#include <array>

#include "Map.h"
#include "Character.h"
#include "Dog.h"

class Enemy : public std::enable_shared_from_this<Enemy>, public Character
{
public:
	Enemy(const float i_x, const float i_y);
	
	virtual bool get_dead(const bool i_deletion) const;
	
	virtual void die(const unsigned char i_death_type);
	
	virtual void update(const unsigned i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const Map& i_map, Dog& i_dog) = 0;
	
	virtual void draw(const unsigned i_view_x, sf::RenderWindow& i_window) = 0;
	
	sf::FloatRect get_hit_box() const;

protected:
	
	bool dead; 
	float horizontal_speed;
	float vertical_speed;
	float x; 
	float y; 
};

