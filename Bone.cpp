#include "Bone.h"

Bone::Bone(const float i_x, const float i_y) :
	Character(),
	dead(0),
	horizontalDirection(1),
	verticalSpeed(0),
	x(i_x),
	y(i_y),
	start(i_y)
{
	texture.loadFromFile("resources/images/bone.png");
}

bool Bone::get_dead() const
{
	return dead;
}

void Bone::draw(const unsigned i_view_x, sf::RenderWindow& i_window)
{
	if (-cellSize < round(y) && round(x) > static_cast<int>(i_view_x) - cellSize && round(x) < screenWidth + i_view_x && round(y) < screenHeight)
	{
		sprite.setPosition(round(x), round(y));
		sprite.setTexture(texture);
		i_window.draw(sprite);
	}
}

void Bone::die(const bool i_value)
{
	dead = i_value;
}

//i_view_x - x position of the current view.
//i_map - constant reference to the map object.
void Bone::update(const unsigned i_view_x, const Map& i_map)
{
	if (!dead)
	{
		if (-cellSize < y && x >= static_cast<int>(i_view_x) - cellSize - entityUpdateArea && x < entityUpdateArea + screenWidth + i_view_x && y < screenHeight)
		{
			if (y > start - cellSize)
			{
				y -= boneSpeed;

				if (y <= start - cellSize)
				{
					y = start - cellSize;
					start = 2 * screenHeight;
				}
			}
			else
			{
				auto collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall },
					sf::FloatRect(x, verticalSpeed + y, cellSize, cellSize));

				if (std::ranges::any_of(collision, [](const unsigned char i_value) { return i_value != 0; }))
				{
					if (0 > verticalSpeed)
					{
						y = cellSize * (1 + floor((verticalSpeed + y) / cellSize));
					}
					else
					{
						y = cellSize * (ceil((verticalSpeed + y) / cellSize) - 1);
					}

					verticalSpeed = 0;
				}
				else
				{
					y += verticalSpeed;
				}

				sf::FloatRect horizontal_hit_box = { boneSpeed * horizontalDirection + x, y, cellSize, cellSize };

				collision = i_map.map_collision({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, horizontal_hit_box);

				if (std::ranges::any_of(collision, [](const unsigned char i_value) { return i_value != 0; }))
				{
					if (0 < horizontalDirection)
					{
						x = cellSize * (ceil((boneSpeed * horizontalDirection + x) / cellSize) - 1);
					}
					else
					{
						x = cellSize * (1 + floor((boneSpeed * horizontalDirection + x) / cellSize));
					}

					horizontalDirection *= -1;
				}
				else
				{
					x += boneSpeed * horizontalDirection;
				}

				verticalSpeed = std::min(gravity + verticalSpeed, maxVerticalSpeed);
			}
		}

		if (screenHeight <= y)
		{
			dead = 1;
		}
	}
}

sf::FloatRect Bone::get_hit_box() const
{
	return sf::FloatRect(x, y, cellSize, cellSize);
}
