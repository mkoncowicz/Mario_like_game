#include "Design.h"

Design::Design()
{
}

sf::Text Design::createText(const std::string& content, sf::Font& i_font, unsigned int characterSize, const sf::Color& i_color, const sf::Vector2f& position)
{
	sf::Text text(content, i_font, characterSize);
	text.setFillColor(i_color);
	text.setPosition(position);
	return text;
}

sf::RectangleShape Design::createButton(const sf::Vector2f& size, const sf::Color& i_color, const sf::Vector2f& position)
{
	sf::RectangleShape button(size);
	button.setFillColor(i_color);
	button.setPosition(position);
	return button;
}

sf::RectangleShape Design::createHighlightedButton(const sf::Vector2f& size, const sf::Color& i_color, unsigned int i_thickness, const sf::Color& i_outlineColor, const sf::Vector2f& position)
{
	sf::RectangleShape highlightedButton(size);
	highlightedButton.setFillColor(i_color);
	highlightedButton.setOutlineThickness(i_thickness);
	highlightedButton.setOutlineColor(i_outlineColor);
	highlightedButton.setPosition(position);
	return highlightedButton;
}


