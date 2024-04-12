
#include "Boar.h"

Boar::Boar(const float i_x, const float i_y) :
	Enemy(i_x, i_y),
	check_collision(1),
	flipped(1),
	no_collision_dying(0),
	state(0),
	get_out_timer(boarGetOutDuration),
	get_out_animation(cellSize, "resources/images/boar_get_out.png", boarGetOutAnimSpeed),
	walk_animation(cellSize, "resources/images/boar_walk.png", boarAnimWalkSpeed)
{
	horizontal_speed = -boarSpeed;
	texture.loadFromFile("resources/images/boar_wait.png");
}

bool Boar::get_dead(const bool i_deletion) const
{
	if (i_deletion)
	{
		return dead;
	}
	else
	{
		return dead || no_collision_dying;
	}
}

void Boar::die(const unsigned char i_death_type)
{
	switch (i_death_type)
	{
	case 0:
	{
		dead = 1; //instant death
		break;
	}
	case 2:
	{
		no_collision_dying = 1;
		vertical_speed = 0.5f * dogJumpSpeed;
		texture.loadFromFile("resources/images/boar_dead.png");
	}
	}
}

void Boar::draw(const unsigned i_view_x, sf::RenderWindow& i_window)
{
	if (-cellSize < round(y) && round(x) > static_cast<int>(i_view_x) - cellSize && round(x) < screenWidth + i_view_x && round(y) < screenHeight)
	{
		if (0 < state || 1 == no_collision_dying)
		{
			if (0 == get_dead(0) && 1 == state && get_out_timer <= 0.25f * boarGetOutDuration)
			{
				get_out_animation.set_position(round(x), round(y));
				get_out_animation.draw(i_window);
			}
			else
			{
				sprite.setPosition(round(x), round(y));
				sprite.setTexture(texture);
				i_window.draw(sprite);
			}
		}
		else
		{
			walk_animation.set_flipped(flipped);
			walk_animation.set_position(round(x), round(y));
			walk_animation.draw(i_window);
		}
	}
}

void Boar::update(const unsigned i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const Map& i_map, Dog& i_dog)
{
	if (-cellSize < y && x >= static_cast<int>(i_view_x) - cellSize - entityUpdateArea && x < entityUpdateArea + screenWidth + i_view_x && y < screenHeight)
	{
		vertical_speed = std::min(gravity + vertical_speed, maxVerticalSpeed);

		if (0 == get_dead(0))
		{
			std::vector<unsigned char> collision;
			
			sf::FloatRect hit_box = get_hit_box();

			hit_box.top += vertical_speed;
			collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);

			if (!std::ranges::all_of(collision, [](const unsigned char i_value) { return i_value == 0; }))
			{
				if (0 > vertical_speed)
				{
					y = cellSize * (1 + floor((vertical_speed + y) / cellSize));
				}
				else
				{
					y = cellSize * (ceil((vertical_speed + y) / cellSize) - 1);
				}

				vertical_speed = 0;
			}
			else
			{
				bool changed = 0;

				for (unsigned short a = 0; a < i_enemies.size(); a++)
				{
					if (shared_from_this() != i_enemies[a] && 0 == i_enemies[a]->get_dead(0) && 1 == hit_box.intersects(i_enemies[a]->get_hit_box()))
					{
						changed = 1;

						if (0 > vertical_speed)
						{
							y = i_enemies[a]->get_hit_box().top + i_enemies[a]->get_hit_box().height;
						}
						else
						{
							y = i_enemies[a]->get_hit_box().top - cellSize;
						}

						vertical_speed = 0;
						break;
					}
				}
				if (!changed)
				{
					y += vertical_speed;
				}
			}

			hit_box = get_hit_box();
			hit_box.left += horizontal_speed;

			collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);

			if (0 == no_collision_dying && 0 == std::ranges::all_of(collision, [](const unsigned char i_value)
				{
					return 0 == i_value;
				}))
			{
				if (0 < horizontal_speed)
				{
					x = cellSize * (ceil((horizontal_speed + x) / cellSize) - 1);
				}
				else
				{
					x = cellSize * (1 + floor((horizontal_speed + x) / cellSize));
				}

				horizontal_speed *= -1;
			}
			else
			{
				bool changed = false;

				for (unsigned short a = 0; a < i_enemies.size(); a++)
				{
					if (shared_from_this() != i_enemies[a] && 0 == i_enemies[a]->get_dead(0) && 1 == hit_box.intersects(i_enemies[a]->get_hit_box()))
					{
						if (0 == state)
						{
							changed = true;
							horizontal_speed *= -1;
						}
						else if (2 == state)
						{
							i_enemies[a]->die(2);
						}
						break;
					}
				}

				if (!changed)
				{
					x += horizontal_speed;
				}
			}

			if (0 == i_dog.get_dead() && 1 == get_hit_box().intersects(i_dog.get_hit_box()))
			{
				if (check_collision)
				{
					if (0 == state)
					{
						if (0 < i_dog.get_vertical_speed()) //turning into boar wait
						{
							check_collision = 0; //checking collisions once we collide with dog
							horizontal_speed = 0;
							state = 1;
							get_out_timer = boarGetOutDuration;
							i_dog.set_vertical_speed(0.5f * dogJumpSpeed);
						}
						else
						{
							i_dog.die(0);
						}
					}
					else if (1 == state) //sliding
					{
						check_collision = 0;
						state = 2;

						if (x < i_dog.get_x()) //direction depending on side of collision
						{
							horizontal_speed = -boarWaitSpeed;
						}
						else
						{
							horizontal_speed = boarWaitSpeed;
						}
						if (0 < i_dog.get_vertical_speed())
						{
							i_dog.set_vertical_speed(0.5f * dogJumpSpeed);
						}
					}
					else
					{
						if (0 < i_dog.get_vertical_speed()) //stop sliding
						{
							check_collision = 0;
							horizontal_speed = 0;
							state = 1;
							get_out_timer = boarGetOutDuration;
							i_dog.set_vertical_speed(0.5f * dogJumpSpeed);
						}
						else if ((0 < horizontal_speed && x < i_dog.get_x()) ||
							(0 > horizontal_speed && x > i_dog.get_x()))
						{
							i_dog.die(0);
						}
					}
				}
			}
			else
			{
				check_collision = 1; //checking collision again when dog stops intersecting with boar
			}
			if (0 < horizontal_speed)
			{
				flipped = 0;
			}
			else if (0 > horizontal_speed)
			{
				flipped = 1;
			}
			if (1 == state)
			{
				get_out_timer--;

				if (0 == get_out_timer)
				{
					state = 0;

					if (!flipped)
					{
						horizontal_speed = boarSpeed;
					}
					else
					{
						horizontal_speed = -boarSpeed;
					}
				}
				else
				{
					get_out_animation.update();
				}
			}
			walk_animation.update();
		}
		else
		{
			y += vertical_speed;
		}
	}
	if (screenHeight <= y)
	{
		die(0);
	}
}
