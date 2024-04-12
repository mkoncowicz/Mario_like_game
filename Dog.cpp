#include "Dog.h"

Dog::Dog() :
	Character(),
	crouching(0),
	dead(0),
	flipped(0),
	onGround(0),
	bonesEaten(0),
	coinsCollected(0),
	enemyBounceSpeed(0),
	horizontal_speed(0),
	vertical_speed(0),
	x(0),
	y(0),
	jump_timer(0),
	powerup_state(0),
	death_timer(dogDeathDur),
	growth_timer(0),
	invincible_timer(0),
	big_walk_animation(cellSize, "resources/images/big_dog_walk.png", dogAnimWalkSpeed),
	walk_animation(cellSize, "resources/images/dog_walk.png", dogAnimWalkSpeed)
{
	texture.loadFromFile("resources/images/dog_idle.png");
	sprite = sf::Sprite(texture);
}

bool Dog::get_dead() const
{
	return dead;
}

float Dog::get_vertical_speed() const
{
	return vertical_speed;
}

float Dog::get_x() const
{
	return x;
}

void Dog::die(const bool i_instant_death)
{
	//dog instantly dies and it doesn't matter if he's big or small
	if (i_instant_death)
	{
		dead = 1;

		if (0 == powerup_state)
		{
			texture.loadFromFile("resources/images/dog_dead.png");
		}
		else
		{
			texture.loadFromFile("resources/images/big_dog_dead.png");
		}
	}

	//dog dies, unless he's big.
	else if (0 == growth_timer && 0 == invincible_timer)
	{
		if (0 == powerup_state)
		{
			dead = 1;
			texture.loadFromFile("resources/images/dog_dead.png");
		}
		else
		{
			powerup_state = 0;
			invincible_timer = dogInvincibilityDur;

			if (!crouching)
			{
				y += cellSize;
			}
			else
			{
				crouching = 0;
			}
		}
	}
}

void Dog::draw_bone(const unsigned i_view_x, sf::RenderWindow& i_window)
{
	for (Bone& bone : bones)
	{
		bone.draw(i_view_x, i_window);
	}
}

void Dog::reset()
{
	crouching = 0;
	dead = 0;
	flipped = 0;
	onGround = 0;
	bonesEaten = 0;
	coinsCollected = 0;

	enemyBounceSpeed = 0;
	horizontal_speed = 0;
	vertical_speed = 0;
	x = 0;
	y = 0;

	jump_timer = 0;
	powerup_state = 0;

	death_timer = dogDeathDur;
	growth_timer = 0;
	invincible_timer = 0;

	bones.clear();

	texture.loadFromFile("resources/images/dog_idle.png");
	sprite.setTexture(texture);

	big_walk_animation.set_animation_speed(dogAnimWalkSpeed);
	big_walk_animation.set_flipped(0);

	walk_animation.set_animation_speed(dogAnimWalkSpeed);
	walk_animation.set_flipped(0);

}

void Dog::set_position(const float i_x, const float i_y)
{
	x = i_x;
	y = i_y;
}

void Dog::set_vertical_speed(const float i_value)
{
	enemyBounceSpeed = i_value;
}

int Dog::numberOfCollisionsWithBone()
{
	return bonesEaten;
}

int Dog::numberOfCoinsCollected()
{
	return coinsCollected;
}

//i_view_x - The x position of the user's view; 
//i_map - reference to the current Map object
void Dog::update(const unsigned i_view_x, Map& i_map)
{
	//making dog bounce after updating all the enemies to prevent a bug
	if (0 != enemyBounceSpeed)
	{
		vertical_speed = enemyBounceSpeed;
		enemyBounceSpeed = 0;
	}
	
	std::ranges::for_each(bones, [i_view_x, &i_map](Bone& bone) {	//to iterate over the elements in the bones container and update each Bone object using the lambda function
		bone.update(i_view_x, i_map);
		});
	
	if (!dead)
	{
		bool moving = false;

		//the map_collision function returns a vector of numbers;
		//each number is a binary representation of the collisions in a single row. 
		//That vector is stored in this vector.
		std::vector<unsigned char> collision;

		//map_collision function can return coordinates of cells intersecting the hitbox
		std::vector<sf::Vector2i> cells;

		sf::FloatRect hit_box = get_hit_box();

		onGround = false;

		if (!crouching)
		{
			if (0 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
				1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				moving = true;
				horizontal_speed = std::max(horizontal_speed - dogAcceleration, -dogWalkSpeed);
			}

			if (0 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
				1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				moving = true;
				horizontal_speed = std::min(dogAcceleration + horizontal_speed, dogWalkSpeed);
			}
		}

		if (!moving)
		{
			if (0 < horizontal_speed)
			{
				horizontal_speed = std::max<float>(0, horizontal_speed - dogAcceleration);
			}
			else if (0 > horizontal_speed)
			{
				horizontal_speed = std::min<float>(0, dogAcceleration + horizontal_speed);
			}
		}
		if (0 < powerup_state)
		{
			if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::C) || 1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				if (!crouching)
				{
					crouching = 1;
					y += cellSize;
				}
			}
			else if (crouching)
			{
				hit_box.height += cellSize;
				hit_box.top -= cellSize;

				//making sure we can stand up without hitting anything.
				collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);

				if (std::ranges::all_of(collision, [](const unsigned char i_value)
					{
						return 0 == i_value;
					}))
				{
					crouching = 0;
					y -= cellSize;
				}
				else
				{
					collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);

					//if we stand up and hit brick - we'll destroy them.
					if (std::ranges::all_of(collision, [](const unsigned char i_value)
						{
							return 0 == i_value;
						}))
					{
						crouching = 0;
						y -= cellSize;
						i_map.map_collision({ Cell::Brick }, cells, hit_box);

						for (const sf::Vector2i& cell : cells)
						{
							i_map.set_map_cell(cell.x, cell.y, Cell::Empty);
							i_map.add_brick_particles(cellSize * cell.x, cellSize * cell.y);
						}
					}
				}
			}
		}

		hit_box = get_hit_box();
		hit_box.left += horizontal_speed;
		collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);

		if (std::ranges::all_of(collision, [](const unsigned char i_value) { return i_value == 0; }) == 0) 
		{
			moving = false;
			x = (horizontal_speed > 0) ? cellSize * (ceil((horizontal_speed + x) / cellSize) - 1) :
				cellSize * (1 + floor((horizontal_speed + x) / cellSize));
			horizontal_speed = 0;
		}
		else
		{
			x += horizontal_speed;
		}

		hit_box = get_hit_box();
		hit_box.top++;
		collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);

		if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || 1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		{
			if (0 == vertical_speed && 0 == std::ranges::all_of(collision, [](const unsigned char i_value) { return i_value == 0; }))
			{
				vertical_speed = dogJumpSpeed;
				jump_timer = dogJumpTimer;
			}
			//The longer the jump button is pressed, the higher dog jumps
			else if (0 < jump_timer) 
			{
				vertical_speed = dogJumpSpeed;
				jump_timer--;
			}
			else
			{
				vertical_speed = std::min(gravity + vertical_speed, maxVerticalSpeed);
			}
		}
		else
		{
			vertical_speed = std::min(gravity + vertical_speed, maxVerticalSpeed);
			jump_timer = 0;
		}

		hit_box = get_hit_box();
		hit_box.top += vertical_speed;
		collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);

		if (std::ranges::all_of(collision, [](const unsigned char i_value) { return i_value == 0; }) == 0)
		{
			if (0 > vertical_speed)
			{
				//destroying bricks
				if (!crouching && 0 < powerup_state)
				{
					i_map.map_collision({ Cell::Brick }, cells, hit_box);

					for (const sf::Vector2i& cell : cells)
					{
						i_map.set_map_cell(cell.x, cell.y, Cell::Empty);
						i_map.add_brick_particles(cellSize * cell.x, cellSize * cell.y);
					}
				}

				i_map.map_collision({ Cell::QuestionBlock }, cells, hit_box);

				//activating question blocks
				for (const sf::Vector2i& cell : cells)
				{
					i_map.set_map_cell(cell.x, cell.y, Cell::ActivatedQuestionBlock);

					//It can be either a bone or a coin, depending on the color of the pixel in the sketch
					if (sf::Color(255, 73, 85) == i_map.get_map_sketch_pixel(cell.x, cell.y))
					{
						bones.push_back(Bone(cellSize * cell.x, cellSize * cell.y));
					}
					else
					{
						i_map.add_question_block_coin(cellSize * cell.x, cellSize * cell.y);
						coinsCollected++; 
					}
				}

				y = cellSize * (1 + floor((vertical_speed + y) / cellSize));
			}
			else if (0 < vertical_speed)
			{
				y = cellSize * (ceil((vertical_speed + y) / cellSize) - 1);
			}
			jump_timer = 0;
			vertical_speed = 0;
		}
		else
		{
			y += vertical_speed;
		}

		if (0 == horizontal_speed)
		{
			if (moving)
			{
				flipped = 1 - flipped;
			}
		}
		else if (0 < horizontal_speed)
		{
			flipped = 0;
		}
		else if (0 > horizontal_speed)
		{
			flipped = 1;
		}

		hit_box = get_hit_box();
		hit_box.top++;
		collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);

		if (std::ranges::all_of(collision, [](const unsigned char i_value) { return i_value == 0; }) == 0)
		{
			onGround = 1;
		}

			for (Bone& bone : bones)
			{
				//bone eating and becoming big
				if (1 == get_hit_box().intersects(bone.get_hit_box()))
				{
					bone.die(1);
					if (0 == powerup_state)
					{
						powerup_state = 1;
						growth_timer = dogGrowthDuration;
						y -= cellSize;
					}
					bonesEaten++;
				}
			}
			
			if (0 < invincible_timer)
			{
				invincible_timer--;
			}

			hit_box = get_hit_box();
			i_map.map_collision({ Cell::Coin }, cells, hit_box);

			//Collecting coins
			for (const sf::Vector2i& cell : cells)
			{
				i_map.set_map_cell(cell.x, cell.y, Cell::Empty);
				coinsCollected++;
			}

			if (0 < growth_timer)
			{
				growth_timer--;
			}

			if (y >= screenHeight - get_hit_box().height)
			{
				die(1);
			}

			if (0 == powerup_state)
			{
				walk_animation.set_animation_speed(dogCatcherAnimWalkSpeed * dogWalkSpeed / abs(horizontal_speed));
				walk_animation.update();
			}
			else
			{
				big_walk_animation.set_animation_speed(dogCatcherAnimWalkSpeed * dogWalkSpeed / abs(horizontal_speed));
				big_walk_animation.update();
			}
	}
	else
	{
		if (0 == death_timer)
		{
			vertical_speed = std::min(gravity + vertical_speed, maxVerticalSpeed);
			y += vertical_speed;
		}
		else if (1 == death_timer)
		{
			vertical_speed = dogJumpSpeed;
		}
		death_timer = std::max(0, death_timer - 1);
	}
	// remove elements from the bones container that satisfy a condition defined by the lambda function
	// The resulting modified container is assigned back to the bones container using the |= operator
	bones |= ranges::actions::remove_if([](const Bone& i_bone)	
		{
			return 1 == i_bone.get_dead();
		});
}

void Dog::draw(sf::RenderWindow& i_window)
{
	//when dog is invincible his sprite blinks
	if (0 == invincible_timer / dogBlinking % 2)
	{
		bool draw_sprite = 1;
		//When dog is growing, his sprite will switch between being big and small
		bool draw_big = 0 == growth_timer / dogBlinking % 2;

		sprite.setPosition(round(x), round(y));

		if (!dead)
		{
			if (0 < powerup_state)
			{
				if (crouching)
				{
					if (!draw_big)
					{
						texture.loadFromFile("resources/images/dog_idle.png");
					}
					else
					{
						texture.loadFromFile("resources/images/big_dog_croutch.png");
					}
				}
				else if (!onGround)
				{
					if (!draw_big)
					{
						sprite.setPosition(round(x), cellSize + round(y));
						texture.loadFromFile("resources/images/dog_jump.png");
					}
					else
					{
						texture.loadFromFile("resources/images/big_dog_jump.png");
					}
				}
				else
				{
					if (0 == horizontal_speed)
					{
						if (!draw_big)
						{
							sprite.setPosition(round(x), cellSize + round(y));
							texture.loadFromFile("resources/images/dog_idle.png");
						}
						else
						{
							texture.loadFromFile("resources/images/big_dog_idle.png");
						}
					}
					else if ((0 < horizontal_speed && 0 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
							1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) ||
							(0 > horizontal_speed && 0 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
							1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
						 {
							 if (!draw_big)
							 {
								sprite.setPosition(round(x), cellSize + round(y));
								texture.loadFromFile("resources/images/dog_idle.png");
							 }
							 else
							 {
							 	texture.loadFromFile("resources/images/big_dog_idle.png");
							 }
						 }
					else
					{
						draw_sprite = 0;

						if (!draw_big)
						{
							walk_animation.set_flipped(flipped);
							walk_animation.set_position(round(x), cellSize + round(y));
							walk_animation.draw(i_window);
						}
						else
						{
							big_walk_animation.set_flipped(flipped);
							big_walk_animation.set_position(round(x), round(y));
							big_walk_animation.draw(i_window);
						}
					}
				}
			}
			else if (!onGround)
			{
				texture.loadFromFile("resources/images/dog_jump.png");
			}
			else
			{
				if (0 == horizontal_speed)
				{
					texture.loadFromFile("resources/images/dog_idle.png");
				}
				else if ((0 < horizontal_speed && 0 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
						1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) ||
						(0 > horizontal_speed && 0 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
						1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))

					 {
						 texture.loadFromFile("resources/images/dog_idle.png");
					 }
				else
				{
					draw_sprite = 0;

					walk_animation.set_flipped(flipped);
					walk_animation.set_position(round(x), round(y));
					walk_animation.draw(i_window);
				}
			}
		}

		if (draw_sprite)
		{
			if (!flipped)
			{
				sprite.setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
			}
			else
			{
				sprite.setTextureRect(sf::IntRect(texture.getSize().x, 0, -static_cast<int>(texture.getSize().x), texture.getSize().y));
			}
			i_window.draw(sprite);
		}
	}
}

sf::FloatRect Dog::get_hit_box() const
{
	// hitbox will be small if dog is small or crouching
	if (1 == crouching || 0 == powerup_state)
	{
		return sf::FloatRect(x, y, cellSize, cellSize);
	}
	else
	{
		return sf::FloatRect(x, y, cellSize, 2 * cellSize);
	}
}
