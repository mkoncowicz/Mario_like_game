#include "DogCatcher.h"

DogCatcher::DogCatcher(const float i_x, const float i_y) :
	Enemy(i_x, i_y),
	no_collision_dying(0),
	death_timer(dogCatcherDeathDur),
	walk_animation(cellSize, "resources/images/dog_catcher_walk.png", dogCatcherAnimWalkSpeed)
{
	horizontal_speed = -dogCatcherSpeed;
	texture.loadFromFile("resources/images/dog_catcher_dead1.png");
}

bool DogCatcher::get_dead(const bool i_deletion) const
{
	if (i_deletion)
	{
		return dead;
	}
	else
	{
		return dead || no_collision_dying || dogCatcherDeathDur > death_timer;
	}
}

void DogCatcher::die(const unsigned char i_death_type)
{
	switch (i_death_type)
	{
	case 0:
	{
		dead = 1;
		break;
	}
	case 1:
	{
		//dog catcher is squished by dog
		if (!no_collision_dying)
		{
			death_timer--;
		}
		break;
	}
	case 2:
	{
		if (dogCatcherDeathDur == death_timer)
		{
			//dog catcher dies from boar sliding
			no_collision_dying = 1;
			vertical_speed = 0.5f * dogJumpSpeed;
			texture.loadFromFile("resources/images/dog_catcher_dead1.png");
		}
	}
	}
}

void DogCatcher::draw(const unsigned i_view_x, sf::RenderWindow& i_window)
{
	//making sure that dog catcher is not drawn outside the view
	if (-cellSize < round(y) && round(x) > static_cast<int>(i_view_x) - cellSize && round(x) < screenWidth + i_view_x && round(y) < screenHeight)
	{
		if (1 == no_collision_dying || dogCatcherDeathDur > death_timer)
		{
			sprite.setPosition(round(x), round(y));
			sprite.setTexture(texture);
			i_window.draw(sprite);
		}
		else
		{
			walk_animation.set_position(round(x), round(y));
			walk_animation.draw(i_window);
		}
	}
}

void DogCatcher::update(const unsigned i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const Map& i_map, Dog& i_dog)
{
	//making sure that dog catcher is not updated outside the view
	if (-cellSize < y && x >= static_cast<int>(i_view_x) - cellSize - entityUpdateArea && x < entityUpdateArea + screenWidth + i_view_x && y < screenHeight)
	{
		std::vector<unsigned char> collision;

		sf::FloatRect hit_box = get_hit_box();

		vertical_speed = std::min(gravity + vertical_speed, maxVerticalSpeed);

		hit_box.top += vertical_speed;
		collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);

		if (!no_collision_dying)
		{
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

				//making sure that when dog catcher falls on another enemy, they don't intersect
				if (!get_dead(0))
				{
					for (unsigned short a = 0; a < i_enemies.size(); a++)
					{
						if (shared_from_this() != i_enemies[a] && 0 == i_enemies[a]->get_dead(0) && 1 == hit_box.intersects(i_enemies[a]->get_hit_box()))
						{
							changed = 1;

							if (0 > vertical_speed)
							{
								y = i_enemies[a]->get_hit_box().height + i_enemies[a]->get_hit_box().top;
							}
							else
							{
								y = i_enemies[a]->get_hit_box().top - cellSize;
							}

							vertical_speed = 0;
							break;
						}
					}
				}
				if (!changed)
				{
					y += vertical_speed;
				}
			}

			if (!get_dead(0))
			{
				hit_box = get_hit_box();
				hit_box.left += horizontal_speed;
				collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);
				
				if (std::ranges::all_of(collision, [](const unsigned char i_value) { return i_value == 0; }) == 0)
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

					//changing direction when colliding with another enemy
					for (unsigned short a = 0; a < i_enemies.size(); a++)
					{
						if (shared_from_this() != i_enemies[a] && 0 == i_enemies[a]->get_dead(0) && 1 == hit_box.intersects(i_enemies[a]->get_hit_box()))
						{
							changed = 1;
							horizontal_speed *= -1;
							break;
						}
					}
					if (!changed)
					{
						x += horizontal_speed;
					}
				}

				if (!i_dog.get_dead() && 1 == get_hit_box().intersects(i_dog.get_hit_box()))
				{
					if (0 < i_dog.get_vertical_speed())
					{
						die(1);
						i_dog.set_vertical_speed(0.5f * dogJumpSpeed);
					}
					else
					{
						i_dog.die(0);
					}
				}
				walk_animation.update();
			}
			else if (dogCatcherDeathDur > death_timer)
			{
				if (0 < death_timer)
				{
					death_timer--;
				}
				else
				{
					die(0);
				}
			}
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
