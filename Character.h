#pragma once
#include <SFML/Graphics.hpp>

#include <memory>
#include <vector>

import Global;
class Character 
{
public:
	Character();

	virtual bool get_dead() const;

	virtual void die();

	virtual void update(const unsigned i_view_x);

	virtual void draw(sf::RenderWindow& i_window) const;

	sf::FloatRect get_hit_box() const;

protected:
	bool dead;
	float horizontal_speed;
	float vertical_speed;
	float x;
	float y;
	sf::Sprite sprite;

};

