#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <ranges>
#include <algorithm>

#include "Map.h"
#include "Character.h"
#include "Animation.h"

class Bone : public Character
{
public:
	
	Bone(const float i_x, const float i_y);
	
	bool get_dead() const;
	
	void draw(const unsigned i_view_x, sf::RenderWindow& i_window);
	
	void die(const bool i_value);
	
	void update(const unsigned i_view_x, const Map& i_map);

	sf::FloatRect get_hit_box() const;

private:

	bool dead;
	char horizontalDirection;
	float verticalSpeed;
	float x;
	float y;
	unsigned int start; 

	sf::Sprite sprite;
	sf::Texture texture;
};