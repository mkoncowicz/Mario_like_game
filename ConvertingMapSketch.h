#pragma once
#include <chrono>

#include "DogCatcher.h"
#include "Boar.h"

void convert_sketch(const unsigned char i_current_level, unsigned short& i_level_finish, unsigned short& i_game_end, std::vector<std::shared_ptr<Enemy>>& i_enemies, sf::Color& i_background_color, Map& i_map, Dog& i_dog);