#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

import Global;

class Design
{
public:

	Design();

	sf::Text createText(const std::string& content, sf::Font& i_font, unsigned int characterSize, const sf::Color& i_color, const sf::Vector2f& position);

	sf::RectangleShape createButton(const sf::Vector2f& size, const sf::Color& i_color, const sf::Vector2f& position);

	sf::RectangleShape createHighlightedButton(const sf::Vector2f& zise, const sf::Color& i_color, unsigned int i_thickness, const sf::Color& i_outlineColor, const sf::Vector2f& position);
};

