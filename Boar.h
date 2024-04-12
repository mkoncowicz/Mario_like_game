#pragma once
#include <cmath>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <ranges>
#include <algorithm>

#include "Animation.h"
#include "Enemy.h"

class Boar :
	public Enemy
{
public:
	Boar(const float i_x, const float i_y);

	bool get_dead(const bool i_deletion) const;

	void die(const unsigned char i_death_type);

	void draw(const unsigned i_view_x, sf::RenderWindow& i_window);

	void update(const unsigned i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const Map& i_map, Dog& i_dog);

private:
	bool check_collision;
	bool flipped;
	bool no_collision_dying;

	//0 - boar walk
	//1 - boar wait
	//2 - boar fast
	unsigned char state;
	unsigned short get_out_timer;

	sf::Sprite sprite;
	sf::Texture texture;

	Animation get_out_animation;
	Animation walk_animation;
};