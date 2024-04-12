#include "Character.h"

Character::Character() :
	dead(0),
	horizontal_speed(0),
	vertical_speed(0)
{
}

bool Character::get_dead() const
{
	return dead;
}

void Character::die()
{
}

void Character::update(const unsigned i_view_x)
{
}

void Character::draw(sf::RenderWindow& i_window) const
{
}

sf::FloatRect Character::get_hit_box() const
{
	return sf::FloatRect(x, y, cellSize, cellSize);
}
