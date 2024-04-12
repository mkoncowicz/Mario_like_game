#include <SFML/Graphics.hpp>

#include "ConvertingMapSketch.h"

//converts a level sketch into a playable map
void convert_sketch(const unsigned char i_current_level, unsigned short& i_level_finish, unsigned short& i_game_end, std::vector<std::shared_ptr<Enemy>>& i_enemies, sf::Color& i_background_color, Map& i_map, Dog& i_dog)
{
	unsigned short map_height;

	i_map.update_map_sketch(i_current_level);
	i_map.set_map_size(i_map.get_map_sketch_width());

	//dividing the height by 3 as the sketch stores the level as 3 layers: blocks, entities, and background tiles.
	map_height = floor(i_map.get_map_sketch_height() / 3.f);

	i_background_color = i_map.get_map_sketch_pixel(0, i_map.get_map_sketch_height() - 1);

	for (unsigned short a = 0; a < i_map.get_map_sketch_width(); a++)
	{
		for (unsigned short b = 0; b < 2 * map_height; b++)
		{
			sf::Color pixel = i_map.get_map_sketch_pixel(a, b);

			if (b < map_height)
			{
				if (sf::Color(182, 73, 0) == pixel)
				{
					i_map.set_map_cell(a, b, Cell::Brick);
				}
				else if (sf::Color(255, 255, 0) == pixel)
				{
					i_map.set_map_cell(a, b, Cell::Coin);
				}
				else if (sf::Color(0, 146, 0) == pixel || sf::Color(0, 182, 0) == pixel || sf::Color(0, 219, 0) == pixel)
				{
					i_map.set_map_cell(a, b, Cell::Pipe);
				}
				else if (sf::Color(255, 73, 85) == pixel || sf::Color(255, 146, 85) == pixel)
				{
					i_map.set_map_cell(a, b, Cell::QuestionBlock);
				}
				else if (sf::Color(0, 0, 0) == pixel || sf::Color(146, 73, 0) == pixel)
				{
					i_map.set_map_cell(a, b, Cell::Wall);
				}
				else
				{
					i_map.set_map_cell(a, b, Cell::Empty);

					if (sf::Color(0, 255, 255) == pixel)
					{
						i_level_finish = a;
					}

					if (sf::Color(255, 0, 255) == pixel)
					{
						i_game_end = a;
					}
				}
			}
			else
			{
				if (sf::Color(255, 0, 0) == pixel)
				{
					i_dog.set_position(cellSize * a, cellSize * (b - map_height));
				}
				else if (sf::Color(182, 73, 0) == pixel)
				{
					i_enemies.push_back(std::make_shared<DogCatcher>(cellSize * a, cellSize * (b - map_height)));
				}
				else if (sf::Color(0, 219, 0) == pixel)
				{
					i_enemies.push_back(std::make_shared<Boar>(cellSize * a, cellSize * (b - map_height)));
				}
			}
		}
	}
}