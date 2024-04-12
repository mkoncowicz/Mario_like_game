#include "Map.h"

Map::Map() :
	coin_animation(cellSize, "resources/images/coin.png", coinAnimationSpeed),
	question_block_animation(cellSize, "resources/images/mystery_block.png", mysteryBlockAnimSpeed)
{
	map_texture.loadFromFile("resources/images/map.png");
	cell_sprite.setTexture(map_texture);
}

unsigned short Map::get_map_sketch_height() const
{
	return map_sketch.getSize().y;
}

unsigned short Map::get_map_sketch_width() const
{
	return map_sketch.getSize().x;
}

unsigned short Map::get_map_width() const
{
	return myMap.size();
}

void Map::add_brick_particles(const unsigned short i_x, const unsigned short i_y)
{
	//adding brick particles
	brick_particles.push_back(Particle(i_x, i_y, -0.25f * brickParticleSpeed, -1.5f * brickParticleSpeed));
	brick_particles.push_back(Particle(i_x + 0.5f * cellSize, i_y, 0.25f * brickParticleSpeed, -1.5f * brickParticleSpeed));
	brick_particles.push_back(Particle(i_x, i_y + 0.5f * cellSize, -0.5f * brickParticleSpeed, -brickParticleSpeed));
	brick_particles.push_back(Particle(i_x + 0.5f * cellSize, i_y + 0.5f * cellSize, 0.5f * brickParticleSpeed, -brickParticleSpeed));
}

void Map::add_question_block_coin(const unsigned short i_x, const unsigned short i_y)
{
	question_block_coins.push_back(Particle(i_x, i_y, 0, coinJump));
}

void Map::draw_map(const bool i_draw_background, const bool i_underground, const unsigned i_view_x, sf::RenderWindow& i_window)
{
	unsigned short map_end = ceil((screenWidth + i_view_x) / static_cast<float>(cellSize));
	unsigned short map_height = floor(map_sketch.getSize().y / 3.f);
	unsigned short map_start = floor(i_view_x / static_cast<float>(cellSize));

	//coin is drawn before the blocks because it should appear behind the question block
	if (!i_draw_background)
	{
		for (const Particle& question_block_coin : question_block_coins)
		{
			coin_animation.set_position(question_block_coin.x, question_block_coin.y);
			coin_animation.draw(i_window);
		}
	}

	for (unsigned short a = map_start; a < map_end; a++)
	{
		for (unsigned short b = 0; b < map_height; b++)
		{
			unsigned short sprite_x = 0;
			unsigned short sprite_y = 0;

			cell_sprite.setPosition(cellSize * a, cellSize * b);

			if (i_draw_background)
			{
				sf::Color pixel = map_sketch.getPixel(a, b + 2 * map_height);
				sf::Color pixel_down = sf::Color(0, 0, 0, 0);
				sf::Color pixel_left = sf::Color(0, 0, 0, 0);
				sf::Color pixel_right = sf::Color(0, 0, 0, 0);
				sf::Color pixel_up = sf::Color(0, 0, 0, 0);

				if (255 == pixel.a)
				{
					if (0 < a)
					{
						pixel_left = map_sketch.getPixel(a - 1, b + 2 * map_height);
					}

					if (0 < b)
					{
						pixel_up = map_sketch.getPixel(a, b + 2 * map_height - 1);
					}

					if (a < map_sketch.getSize().x - 1)
					{
						pixel_right = map_sketch.getPixel(1 + a, b + 2 * map_height);
					}

					if (b < map_height - 1)
					{
						pixel_down = map_sketch.getPixel(a, 1 + b + 2 * map_height);
					}

					if (sf::Color(255, 255, 255) == pixel)
					{
						sprite_x = 8;

						if (sf::Color(255, 255, 255) == pixel_up)
						{
							sprite_y = 1;
						}

						if (sf::Color(255, 255, 255) == pixel_left)
						{
							if (sf::Color(255, 255, 255) != pixel_right)
							{
								sprite_x = 9;
							}
						}
						else if (sf::Color(255, 255, 255) == pixel_right)
						{
							sprite_x = 7;
						}
					}
					else if (sf::Color(146, 219, 0) == pixel)
					{
						sprite_x = 5;
					}
					else if (sf::Color(146, 146, 0) == pixel)
					{
						sprite_x = 4;
					}
					else if (sf::Color(146, 182, 0) == pixel)
					{
						sprite_x = 6;
					}
					else if (sf::Color(0, 73, 0) == pixel)
					{
						sprite_y = 1;

						if (sf::Color(0, 109, 0) == pixel_left)
						{
							if (sf::Color(0, 109, 0) != pixel_right)
							{
								sprite_x = 2;
							}
						}
						else if (sf::Color(0, 109, 0) == pixel_right)
						{
							sprite_x = 1;
						}
					}
					else if (sf::Color(0, 109, 0) == pixel)
					{
						sprite_y = 1;

						if (sf::Color(0, 73, 0) == pixel_left)
						{
							sprite_x = 3;
						}
						else if (sf::Color(0, 73, 0) == pixel_right)
						{
							sprite_x = 5;
						}
						else
						{
							sprite_x = 4;
						}
					}
					else if (sf::Color(109, 255, 85) == pixel)
					{
						sprite_x = 12;

						if (sf::Color(109, 255, 85) == pixel_up)
						{
							sprite_y = 1;
						}
					}

					cell_sprite.setTextureRect(sf::IntRect(cellSize * sprite_x, cellSize * sprite_y, cellSize, cellSize));
					i_window.draw(cell_sprite);
				}
			}
			else if (Cell::Empty != myMap[a][b])
			{
				if (Cell::Coin == myMap[a][b])
				{
					coin_animation.set_position(cellSize * a, cellSize * b);
					coin_animation.draw(i_window);
				}
				else if (Cell::QuestionBlock == myMap[a][b])
				{
					question_block_animation.set_position(cellSize * a, cellSize * b);
					question_block_animation.draw(i_window);
				}
				else
				{
					//underground blocks have a different look - their texture 2 cells below the regular ones in the map texture
					sprite_y = 2 * i_underground;

					if (Cell::ActivatedQuestionBlock == myMap[a][b])
					{
						sprite_x = 6;
						sprite_y++;
					}
					else if (Cell::Pipe == myMap[a][b])
					{
						if (sf::Color(0, 182, 0) == map_sketch.getPixel(a, b))
						{
							sprite_y = 1;

							if (Cell::Pipe == myMap[a - 1][b])
							{
								sprite_x = 11;
							}
							else
							{
								sprite_x = 10;
							}
						}
						else if (sf::Color(0, 146, 0) == map_sketch.getPixel(a, b))
						{
							sprite_y = 0;

							if (sf::Color(0, 146, 0) == map_sketch.getPixel(a - 1, b))
							{
								sprite_x = 11;
							}
							else if (sf::Color(0, 146, 0) == map_sketch.getPixel(1 + a, b))
							{
								sprite_x = 10;
							}
							else
							{
								sprite_x = 10;

								if (sf::Color(0, 146, 0) == map_sketch.getPixel(a, b - 1))
								{
									sprite_y = 3;
								}
								else
								{
									sprite_y = 2;
								}
							}
						}
						else if (sf::Color(0, 219, 0) == map_sketch.getPixel(a, b))
						{
							if (sf::Color(0, 182, 0) == map_sketch.getPixel(1 + a, b))
							{
								sprite_x = 12;
							}
							else
							{
								sprite_x = 11;
							}

							if (sf::Color(0, 219, 0) == map_sketch.getPixel(a, b - 1))
							{
								sprite_y = 3;
							}
							else
							{
								sprite_y = 2;
							}
						}
					}
					else if (Cell::Wall == myMap[a][b])
					{
						if (sf::Color(0, 0, 0) == map_sketch.getPixel(a, b))
						{
							sprite_x = 2;
						}
						else
						{
							sprite_x = 3;
						}
					}
					cell_sprite.setTextureRect(sf::IntRect(cellSize * sprite_x, cellSize * sprite_y, cellSize, cellSize));
					i_window.draw(cell_sprite);
				}
			}
		}
	}

	//drawing brick particles
	if (!i_draw_background)
	{
		for (const Particle& brick_particle : brick_particles)
		{
			cell_sprite.setPosition(brick_particle.x, brick_particle.y);
			cell_sprite.setTextureRect(sf::IntRect(0.25f * cellSize, cellSize * (0.25f + 2 * i_underground), 0.5f * cellSize, 0.5f * cellSize));
			i_window.draw(cell_sprite);
		}
	}
}

void Map::set_map_cell(const unsigned short i_x, const unsigned short i_y, const Cell& i_cell)
{
	myMap[i_x][i_y] = i_cell;
}

void Map::set_map_size(const unsigned short i_new_size)
{
	myMap.clear();
	myMap.resize(i_new_size);
}

void Map::update_map_sketch(const unsigned char i_current_level)
{
	map_sketch.loadFromFile("resources/images/level_sketch" +std::to_string(static_cast<unsigned short>(i_current_level)) + ".png");
}

void Map::update()
{
	for (Particle& question_block_coin : question_block_coins)
	{
		question_block_coin.vertical_speed += gravity;
		question_block_coin.y += question_block_coin.vertical_speed;
	}

	for (Particle& brick_particle : brick_particles)
	{
		brick_particle.vertical_speed += gravity;
		brick_particle.x += brick_particle.horizontal_speed;
		brick_particle.y += brick_particle.vertical_speed;
	}

	std::erase_if(brick_particles, [screenHeight = screenHeight](const Particle& i_brick_particle)
		{
			return screenHeight <= i_brick_particle.y;
		});

	std::erase_if(question_block_coins, [](const Particle& i_question_block_coin)
		{
			return 0 <= i_question_block_coin.vertical_speed;
		});
	
	coin_animation.update();
	question_block_animation.update();
}

//calculating collisions for the given hitbox and list of cells
std::vector<unsigned char> Map::map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hitbox) const
{
	std::vector<unsigned char> output;

	auto rows = std::views::iota(
		static_cast<short>(std::floor(i_hitbox.top / cellSize)),
		static_cast<short>(std::floor((std::ceil(i_hitbox.height + i_hitbox.top) - 1) / cellSize) + 1)
	);

	auto cols = std::views::iota(
		static_cast<short>(std::floor(i_hitbox.left / cellSize)),
		static_cast<short>(std::floor((std::ceil(i_hitbox.left + i_hitbox.width) - 1) / cellSize) + 1)
	);

	for (auto a : rows)
	{
		output.push_back(0);

		auto values = cols | std::views::filter([&](short b) {
			return (b >= 0 && b < myMap.size()) && (a >= 0 && a < myMap[0].size());
			}) | std::views::transform([&](short b) {
				return i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), myMap[b][a])
					? static_cast<unsigned char>(pow(2, std::floor((std::ceil(i_hitbox.left + i_hitbox.width) - 1) / cellSize) - b))
					: 0;
				});

			output[a - static_cast<short>(std::floor(i_hitbox.top / cellSize))] = ranges::accumulate(values, 0);

			if (std::ranges::all_of(cols, [&](short b) { return b < 0 || b >= myMap.size() || a < 0 || a >= myMap[0].size(); }) &&
				i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), Cell::Wall))
			{
				output[a - static_cast<short>(std::floor(i_hitbox.top / cellSize))] += static_cast<unsigned char>(pow(2, std::floor((std::ceil(i_hitbox.left + i_hitbox.width) - 1) / cellSize) - cols.back() - 1));
			}
	}
	return output;
}

std::vector<unsigned char> Map::map_collision(const std::vector<Cell>& i_check_cells, std::vector<sf::Vector2i>& i_collision_cells, const sf::FloatRect& i_hitbox) const
{
	std::vector<unsigned char> output;

	i_collision_cells.clear();

	for (short a = floor(i_hitbox.top / cellSize); a <= floor((ceil(i_hitbox.height + i_hitbox.top) - 1) / cellSize); a++)
	{
		output.push_back(0);

		for (short b = floor(i_hitbox.left / cellSize); b <= floor((ceil(i_hitbox.left + i_hitbox.width) - 1) / cellSize); b++)
		{
			if (0 <= b && b < myMap.size())
			{
				if (0 <= a && a < myMap[0].size())
				{
					if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), myMap[b][a]))
					{
						i_collision_cells.push_back(sf::Vector2i(b, a));

						output[a - floor(i_hitbox.top / cellSize)] += pow(2, floor((ceil(i_hitbox.left + i_hitbox.width) - 1) / cellSize) - b);
					}
				}
			}
			else if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), Cell::Wall))
			{
				output[a - floor(i_hitbox.top / cellSize)] += pow(2, floor((ceil(i_hitbox.left + i_hitbox.width) - 1) / cellSize) - b);
			}
		}
	}
	return output;
}

sf::Color Map::get_map_sketch_pixel(const unsigned short i_x, const unsigned short i_y) const
{
	return map_sketch.getPixel(i_x, i_y);
}
