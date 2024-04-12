#include "GamePlay.h"

namespace fs = std::filesystem;// Added namespace alias

void GamePlay::saveLeaderboard(const std::vector<PlayerScore>& leaderboard)
{
	std::ofstream file("leaderboard.txt");	//creates a file stream object to write data to the file
	if (file.is_open())						//if the file is open, the leaderboard data is written to the file using the << operator
	{
		for (const PlayerScore& playerScore : leaderboard)
		{
			file << playerScore.nickname << " " << playerScore.score << "\n";
		}
		file.close();
		//std::cout << "Leaderboard saved successfully.\n";
	}
	else
	{
		//std::cerr << "Failed to save leaderboard.\n";
	}
}

std::vector<PlayerScore> GamePlay::loadLeaderboard()
{
	std::vector<PlayerScore> leaderboard;
	if (fs::exists("leaderboard.txt"))		//filesystem usage - to check if the file "leaderboard.txt" exists
	{
		std::ifstream file("leaderboard.txt");
		if (file.is_open())
		{
			std::string line;
			while (std::getline(file, line))
			{
				std::istringstream iss(line);
				std::string nickname;
				int score;
				if (iss >> nickname >> score)
				{
					leaderboard.push_back({ nickname, score });
				}
			}
			file.close();
		}
		else 
		{
			//std::cerr << "Failed to load leaderboard.\n";
		}
	}
	else
	{
		//std::cout << "leaderboard file does not exist.\n";
	}
		
	std::sort(leaderboard.begin(), leaderboard.end(), [](const PlayerScore& a, const PlayerScore& b)
		{
			if (a.score != b.score)
			{
				return a.score > b.score;  // Sort by score in descending order
			}
			else
			{
				return a.nickname < b.nickname;  // If scores are equal, sort by nickname in ascending order
			}
		});

	if (leaderboard.size() > 10)
	{
		leaderboard.resize(10);  // Keep only the top 10 scores
	}
	return leaderboard;
}

void GamePlay::resetGame()
{
	currentLevel = 0;
	levelFinish = 0;
	gameFinish = 0;
	game_over = false;
	dogAlive = true;
	numberOfCollisionsWithBone = 0;
	numberOfCoinsCollected = 0;
	numberOfEnemiesKilled = 0;
	playerScore = 100;

	startTime = std::chrono::steady_clock::now();
	std::chrono::microseconds lag(0);
	previousTime = std::chrono::steady_clock::now();

	enemies.clear();
	dog.reset();
	convert_sketch(currentLevel, levelFinish, gameFinish, enemies, backgroundColor, map, dog);
}

int GamePlay::runGame()
{
	currentLevel = 0;
	levelFinish = 0;
	gameFinish = 0;

	game_over = false;
	dogAlive = true;
	restartGame = false;

	numberOfCollisionsWithBone = 0;
	numberOfCoinsCollected = 0;
	numberOfEnemiesKilled = 0;

	playerScore = 100;

	//to make the game framerate-independent
	std::chrono::microseconds lag(0);

	sf::RenderWindow window{ sf::VideoMode{ screen_resize* screenWidth, screen_resize* screenHeight }, "Go back home", sf::Style::Close };
	window.setPosition(sf::Vector2i(window.getPosition().x, window.getPosition().y - 45));

	sf::View view(sf::FloatRect(0, 0, screenWidth, screenHeight));

	sf::Event event;

	// Creating the welcome screen
	sf::Font font;
	if (!font.loadFromFile("font.ttf"))
	{
		std::cout << "Failed to load font file." << std::endl;
		return -1;
	}

	sf::Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("resources/images/background.png")) {
		std::cout << "Failed to load background image." << std::endl;
		return -1;
	}
	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setScale(screen_resize, screen_resize);

	Design design;
	
	sf::Text welcomeText = design.createText("Welcome to Go Back Home!", font, 36, sf::Color::Black, { screen_resize * screenWidth / 2 - welcomeText.getGlobalBounds().width /2 - 350, screen_resize * screenHeight / 2 - welcomeText.getGlobalBounds().height / 2 - 200 });
	sf::Text instructionsText = design.createText("Use the left and right arrow keys to move your dog.\n           Press the Z key or up arrow key to jump.\nPress down arrow key to crouch when your dog is big.\n\n", font, 24, sf::Color::Black, { screen_resize * screenWidth / 2 - welcomeText.getGlobalBounds().width / 2 - 100, screen_resize * screenHeight / 2 - welcomeText.getGlobalBounds().height / 2 - 50 });
	
	sf::RectangleShape playButton = design.createButton({ 240, 60 }, sf::Color::Black, { screen_resize * screenWidth / 2 - playButton.getSize().x / 2 - 120, screen_resize * screenHeight / 2 - playButton.getSize().y / 2 + 70 });
	sf::Text playButtonText = design.createText("PLAY", font, 30, sf::Color::White, { playButton.getPosition().x + playButton.getSize().x / 2 - playButtonText.getGlobalBounds().width / 2 - 60, playButton.getPosition().y + playButton.getSize().y / 2 - playButtonText.getGlobalBounds().height / 2 - 10 });
	
	sf::RectangleShape exitButton = design.createButton({ 240,60 }, sf::Color::Black, { screen_resize * screenWidth / 2 - exitButton.getSize().x / 2 - 120 , screen_resize * screenHeight / 2 - exitButton.getSize().y / 2 + 170 });
	sf::Text exitButtonText = design.createText("EXIT", font, 30, sf::Color::White, { exitButton.getPosition().x + exitButton.getSize().x / 2 - exitButtonText.getGlobalBounds().width / 2 - 60, exitButton.getPosition().y + exitButton.getSize().y / 2 - exitButtonText.getGlobalBounds().height / 2 - 10 });
	
	sf::RectangleShape leaderboardButton = design.createButton({ 380, 60 }, sf::Color::Black, { screen_resize * screenWidth / 2 - leaderboardButton.getSize().x / 2 - 190, screen_resize * screenHeight / 2 - leaderboardButton.getSize().y / 2 + 270 });
	sf::Text leaderboardButtonText = design.createText("LEADERBOARD", font, 30, sf::Color::White, { leaderboardButton.getPosition().x + leaderboardButton.getSize().x / 2 - leaderboardButton.getGlobalBounds().width / 2 + 15, leaderboardButton.getPosition().y + leaderboardButton.getSize().y / 2 - leaderboardButton.getGlobalBounds().height / 2 + 20 });

	sf::Text leaderboardTitle = design.createText("LEADERBOARD", font, 36, sf::Color::Red, { screen_resize * screenWidth / 2 - leaderboardTitle.getGlobalBounds().width / 2 - 200, screen_resize * screenHeight / 2 - leaderboardTitle.getGlobalBounds().height / 2 - 330 });

	sf::Text leaderboardText = design.createText("", font, 30, sf::Color::Black, { screen_resize * screenWidth / 2 - leaderboardText.getGlobalBounds().width / 2 - 130, screen_resize * screenHeight / 2 - leaderboardText.getGlobalBounds().height / 2 - 250 });
	
	sf::RectangleShape backButton = design.createButton({ 240,60 }, sf::Color::Black, { screen_resize * screenWidth / 2 - backButton.getSize().x / 2 - 120, screen_resize * screenHeight / 2 - backButton.getSize().y / 2 + 330 });
	sf::Text backButtonText = design.createText("BACK", font, 30, sf::Color::White, { backButton.getPosition().x + backButton.getSize().x / 2 - backButtonText.getGlobalBounds().width / 2 - 70, backButton.getPosition().y + backButton.getSize().y / 2 - backButtonText.getGlobalBounds().height / 2 - 10 });
	
	bool inWelcomeScreen = true;
	bool playButtonHighlighted = false;
	bool exitButtonHighlighted = false;
	bool backButtonHighlighted = false;
	bool leaderboardButtonHighlighted = false;
	bool inLeaderboardScreen = false;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				window.close();
				break;
			}
			case sf::Event::MouseMoved:
			{
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

				if (inWelcomeScreen)
				{
					if (playButton.getGlobalBounds().contains(mousePos))
					{
						playButton.setFillColor(sf::Color::White);
						playButtonText.setFillColor(sf::Color::Black);
						playButtonHighlighted = true;
					}
					else if (exitButton.getGlobalBounds().contains(mousePos))
					{
						exitButton.setFillColor(sf::Color::White);
						exitButtonText.setFillColor(sf::Color::Black);
						exitButtonHighlighted = true;
					}
					else if (leaderboardButton.getGlobalBounds().contains(mousePos))
					{
						leaderboardButton.setFillColor(sf::Color::White);
						leaderboardButtonText.setFillColor(sf::Color::Black);
						leaderboardButtonHighlighted = true;
					}
					else
					{
						playButton.setFillColor(sf::Color::Black);
						playButtonText.setFillColor(sf::Color::White);
						exitButton.setFillColor(sf::Color::Black);
						exitButtonText.setFillColor(sf::Color::White);
						leaderboardButton.setFillColor(sf::Color::Black);
						leaderboardButtonText.setFillColor(sf::Color::White);
						playButtonHighlighted = false;
						exitButtonHighlighted = false;
						leaderboardButtonHighlighted = false;
					}
				}
				else if (inLeaderboardScreen)
				{
					if (backButton.getGlobalBounds().contains(mousePos))
					{
						backButton.setFillColor(sf::Color::White);
						backButtonText.setFillColor(sf::Color::Black);
						backButtonHighlighted = true;
					}
					else
					{
						backButton.setFillColor(sf::Color::Black);
						backButtonText.setFillColor(sf::Color::White);
						backButtonHighlighted = false;
					}
				}
				break;
			}
			case sf::Event::MouseButtonPressed:
			{
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

				if (inWelcomeScreen)
				{
					if (playButton.getGlobalBounds().contains(mousePos))
					{
						// initializing the game
						convert_sketch(currentLevel, levelFinish, gameFinish, enemies, backgroundColor, map, dog);
						startTime = std::chrono::steady_clock::now();
						previousTime = std::chrono::steady_clock::now();
						goto GameLoop;
					}
					else if (leaderboardButton.getGlobalBounds().contains(mousePos))
					{
						inWelcomeScreen = false;
						inLeaderboardScreen = true;
					}
					else if (exitButton.getGlobalBounds().contains(mousePos))
					{
						window.close();
					}
				}
				else if (inLeaderboardScreen)
				{
					if (backButton.getGlobalBounds().contains(mousePos))
					{
						inLeaderboardScreen = false;
						inWelcomeScreen = true;
					}
				}
				break;
			}
			}
		}
		window.clear();
		window.draw(backgroundSprite);

		if (inWelcomeScreen)
		{
			window.draw(welcomeText);
			window.draw(instructionsText);
			window.draw(playButton);
			window.draw(playButtonText);
			window.draw(exitButton);
			window.draw(exitButtonText);
			window.draw(leaderboardButton);
			window.draw(leaderboardButtonText);

			if (playButtonHighlighted)
			{
				sf::RectangleShape playButtonHighlight = design.createHighlightedButton({ playButton.getSize().x + 10, playButton.getSize().y + 10 }, sf::Color::Transparent, 5, sf::Color::White, { playButton.getPosition().x - 5, playButton.getPosition().y - 5 });
				window.draw(playButtonHighlight);
			}
			else if (exitButtonHighlighted)
			{
				sf::RectangleShape exitButtonHighlight = design.createHighlightedButton({ exitButton.getSize().x + 10, exitButton.getSize().y + 10 }, sf::Color::Transparent, 5, sf::Color::White, { exitButton.getPosition().x - 5, exitButton.getPosition().y - 5 });
				window.draw(exitButtonHighlight);
			}
			else if (leaderboardButtonHighlighted)
			{
				sf::RectangleShape leaderboardButtonHighlighted = design.createHighlightedButton({ leaderboardButton.getSize().x + 10, leaderboardButton.getSize().y + 10 }, sf::Color::Transparent, 5, sf::Color::White, { leaderboardButton.getPosition().x - 5, leaderboardButton.getPosition().y - 5 });
				window.draw(leaderboardButtonHighlighted);
			}
		}
		else if (inLeaderboardScreen)
		{
			std::vector<PlayerScore> leaderboard = loadLeaderboard();

			std::sort(leaderboard.begin(), leaderboard.end(), [](const PlayerScore& a, const PlayerScore& b)
				{
					if (a.score != b.score)
					{
						return a.score > b.score;  // Sort by score in descending order
					}
					else
					{
						return a.nickname < b.nickname;  // If scores are equal, sort by nickname in ascending order
					}
				});

			std::ostringstream oss;
			for (const PlayerScore& playerScore : leaderboard) {
				oss << playerScore.nickname << " - " << playerScore.score << "\n";
			}
			leaderboardText.setString(oss.str());

			window.draw(leaderboardTitle);
			window.draw(leaderboardText);
			window.draw(backButton);
			window.draw(backButtonText);

			if (backButtonHighlighted)
			{
				sf::RectangleShape backButtonHighlighted = design.createHighlightedButton({ backButton.getSize().x + 10, backButton.getSize().y + 10 }, sf::Color::Transparent, 5, sf::Color::White, { backButton.getPosition().x - 5, backButton.getPosition().y - 5 });
				window.draw(backButtonHighlighted);
			}
		}
		window.display();
	}

GameLoop:
	while (window.isOpen())
	{
		std::chrono::microseconds deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previousTime);
		lag += deltaTime;
		previousTime += deltaTime;

		while (frameDuration <= lag)
		{
			unsigned view_x;
			lag -= frameDuration;

			while (window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					window.close();
					break;
				}
				case sf::Event::KeyPressed:
				{
					switch (event.key.code)
					{
					case sf::Keyboard::Enter:
					{
						enemies.clear();
						dog.reset();
						convert_sketch(currentLevel, levelFinish, gameFinish, enemies, backgroundColor, map, dog);
					}
					}
					switch (event.key.code)
					{
					case sf::Keyboard::Escape:
					{
						window.close();
						break;
					}
					}
				}
				}
			}
			//if dog goes beyond the finish, we move on to the next level			
			if (cellSize * levelFinish <= dog.get_x())
			{					
				currentLevel++;
				enemies.clear();
				dog.reset();
				convert_sketch(currentLevel, levelFinish, gameFinish, enemies, backgroundColor, map, dog);
			}
			//if dog reaches end of game, bonus points
			if (cellSize * gameFinish == dog.get_x())
			{
				playerScore += 100;
				game_over = true;
			}
			//keeping dog at the center of the view
			view_x = std::clamp<int>(round(dog.get_x()) - 0.5f * (screenWidth - cellSize), 0, cellSize * map.get_map_width() - screenWidth);

			map.update();
			dog.update(view_x, map);

			for (unsigned short a = 0; a < enemies.size(); a++)
			{
				enemies[a]->update(view_x, enemies, map, dog);
			}

			for (unsigned short a = 0; a < enemies.size(); a++)
			{
				if (1 == enemies[a]->get_dead(1))
				{
					numberOfEnemiesKilled++;
					enemies.erase(a + enemies.begin());
					a--;
				}
			}

			if (frameDuration > lag)
			{
				view.reset(sf::FloatRect(view_x, 0, screenWidth, screenHeight));

				window.setView(view);
				window.clear(backgroundColor);

				//if the background color is sf::Color(0, 0, 85), the level is underground
				map.draw_map(1, sf::Color(0, 0, 85) == backgroundColor, view_x, window);
				dog.draw_bone(view_x, window);
				map.draw_map(0, sf::Color(0, 0, 85) == backgroundColor, view_x, window);

				for (unsigned short a = 0; a < enemies.size(); a++)
				{
					enemies[a]->draw(view_x, window);
				}

				dog.draw(window);

				if (dogAlive)
				{
					std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
					std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
					int seconds = static_cast<int>(elapsedSeconds.count());

					secondsVector.push_back(seconds);

					sf::Text timerText(std::to_string(seconds) + "s", font, 12);
					timerText.setFillColor(sf::Color::White);
					timerText.setPosition(view.getCenter().x + view.getSize().x / 2 - timerText.getGlobalBounds().width - 10,
						view.getCenter().y - view.getSize().y / 2 + 10);

					window.draw(timerText);

					if (secondsVector.size() > 1 && secondsVector[secondsVector.size() - 1] > secondsVector[secondsVector.size() - 2])
					{
						playerScore -= 1;
					}

					numberOfCollisionsWithBone = dog.numberOfCollisionsWithBone();
					numebrOfCollisionsVector.push_back(numberOfCollisionsWithBone);
					if (numebrOfCollisionsVector.size() > 1 && numebrOfCollisionsVector[numebrOfCollisionsVector.size() - 1] > numebrOfCollisionsVector[numebrOfCollisionsVector.size() - 2])
					{
						playerScore += 20;
					}

					numberOfCoinsCollected = dog.numberOfCoinsCollected();
					numberOfCoinsVector.push_back(numberOfCoinsCollected);
					if (numberOfCoinsVector.size() > 1 && numberOfCoinsVector[numberOfCoinsVector.size() - 1] > numberOfCoinsVector[numberOfCoinsVector.size() - 2])
					{
						playerScore += 10;
					}

					numeberOfEnemiesVector.push_back(numberOfEnemiesKilled);
					if (numeberOfEnemiesVector.size() > 1 && numeberOfEnemiesVector[numeberOfEnemiesVector.size() - 1] > numeberOfEnemiesVector[numeberOfEnemiesVector.size() - 2])
					{
						playerScore += 10;
					}

					sf::Text scoreText(std::to_string(playerScore), font, 12);
					scoreText.setFillColor(sf::Color::White);
					scoreText.setPosition(view.getCenter().x - view.getSize().x / 2 + 10,
						view.getCenter().y - view.getSize().y / 2 + 10);

					scoreText.setString(std::to_string(playerScore));

					window.draw(scoreText);
				}
				window.display();
			}

			if (dog.get_dead())
			{
				game_over = true;
				dogAlive = false;
			}
		}
		if (game_over)
		{
			sf::RenderWindow nicknameWindow{ sf::VideoMode{ 800, 600 }, "Enter Nickname", sf::Style::Close };
			nicknameWindow.setPosition(sf::Vector2i(nicknameWindow.getPosition().x, nicknameWindow.getPosition().y - 45));
			
			sf::Text nicknameWindowText = design.createText("Enter your nickname:", font, 24, sf::Color::Black, { 100,200 });
			sf::RectangleShape inputBox = design.createHighlightedButton({ 400, 40 }, sf::Color::White, 2, sf::Color::Black, { 100,250 });
			sf::Text hintText = design.createText("Only small letters are allowed. \n\nAfter entering your nickname press \nENTER to save your nickname to the leaderboard \nand go to the Ending screen.", font, 16, sf::Color::Black, { 100,300 });

			std::string nickname;
			bool validNickname = false;

			while (nicknameWindow.isOpen())
			{
				sf::Event event;
				while (nicknameWindow.pollEvent(event))
				{
					if (event.type == sf::Event::Closed)
					{
						nicknameWindow.close();
					}
					else if (event.type == sf::Event::TextEntered)
					{
						if (event.text.unicode < 128) 
						{
							char c = static_cast<char>(event.text.unicode);
							if (std::regex_match(std::string(1, c), std::regex("[a-z]"))) // to check if the character (converted from event.text.unicode) matches the regular expression pattern "[a-z]"
							{
								nickname += c;
							}
						}
					}
					else if (event.type == sf::Event::KeyPressed) 
					{
						if (event.key.code == sf::Keyboard::Enter) 
						{
							validNickname = std::regex_match(nickname, std::regex("[a-z]+")); //to check if the nickname string matches the regular expression pattern "[a-z]+" - nickname should contain one or more lowercase letters from 'a' to 'z'
							std::vector<PlayerScore> leaderboard = loadLeaderboard();
							leaderboard.push_back({ nickname, playerScore });
							std::sort(leaderboard.begin(), leaderboard.end(), [](const PlayerScore& a, const PlayerScore& b)
								{
									if (a.score != b.score)
									{
										return a.score > b.score;  // Sort by score in descending order
									}
									else
									{
										return a.nickname < b.nickname;  // If scores are equal, sort by nickname in ascending order
									}
								});
							saveLeaderboard(leaderboard);
							nicknameWindow.close();
						}
					}
				}
				nicknameWindow.clear(sf::Color::White);
				nicknameWindow.draw(nicknameWindowText);
				nicknameWindow.draw(inputBox);
				nicknameWindow.draw(hintText);

				sf::Text inputText(nickname, font, 16);
				inputText.setFillColor(sf::Color::Black);
				inputText.setPosition(110, 255);
				nicknameWindow.draw(inputText);

				nicknameWindow.display();
			}
			bool inEndingScreen = true;

			sf::RenderWindow endingWindow(sf::VideoMode(screen_resize * screenWidth, screen_resize * screenHeight), "Game Over", sf::Style::Close);
			endingWindow.setPosition(sf::Vector2i(window.getPosition().x, window.getPosition().y));

			sf::Text gameOverText = design.createText("GAME OVER", font, 40, sf::Color::Red, { screen_resize * screenWidth / 2 - gameOverText.getGlobalBounds().width / 2 - 200, (screen_resize * screenHeight / 2 - gameOverText.getGlobalBounds().height / 2) - 250 });
			sf::Text scoreLabel = design.createText("Final Score:", font, 30, sf::Color::Black, { screen_resize * screenWidth / 2 - scoreLabel.getGlobalBounds().width / 2 - 250, screen_resize * screenHeight / 2 - scoreLabel.getGlobalBounds().height / 2 - 90 });
			sf::Text scoreText = design.createText(std::to_string(playerScore), font, 30, sf::Color::Black, { screen_resize * screenWidth / 2 - scoreText.getGlobalBounds().width / 2 + 100, screen_resize * screenHeight / 2 - scoreText.getGlobalBounds().height / 2 - 90 });
			sf::Text timeLabel = design.createText("Time:", font, 30, sf::Color::Black, {screen_resize * screenWidth / 2 - timeLabel.getGlobalBounds().width / 2 - 80, screen_resize * screenHeight / 2 - timeLabel.getGlobalBounds().height / 2 - 20});
			
			int finalTime = secondsVector.empty() ? 0 : secondsVector.back();

			sf::Text timeText = design.createText(std::to_string(finalTime) + "s", font, 30, sf::Color::Black, { screen_resize * screenWidth / 2 - timeText.getGlobalBounds().width / 2 + 100, screen_resize * screenHeight / 2 - timeText.getGlobalBounds().height / 2 - 20 });
			sf::Text nicknameLabel = design.createText("Nickname:", font, 30, sf::Color::Black, { screen_resize * screenWidth / 2 - nicknameLabel.getGlobalBounds().width / 2 - 200, screen_resize * screenHeight / 2 - nicknameLabel.getGlobalBounds().height / 2 + 50 });
			sf::Text nicknameText = design.createText("", font, 30, sf::Color::Black, { screen_resize * screenWidth / 2 - nicknameText.getGlobalBounds().width / 2 + 100, screen_resize * screenHeight / 2 - nicknameText.getGlobalBounds().height / 2 + 50 });
			
			nicknameText.setString(nickname);

			sf::RectangleShape playAgainButton = design.createButton({ 340,60 }, sf::Color::Black, { screen_resize * screenWidth / 2 - playAgainButton.getSize().x / 2 - 170, screen_resize * screenHeight / 2 - playAgainButton.getSize().y / 2 + 170 });
			sf::Text playAgainButtonText = design.createText("PLAY AGAIN", font, 30, sf::Color::Black, { playAgainButton.getPosition().x + playAgainButton.getSize().x / 2 - playAgainButtonText.getGlobalBounds().width / 2 - 155, playAgainButton.getPosition().y + playAgainButton.getSize().y / 2 - playAgainButtonText.getGlobalBounds().height / 2 - 10 });
			
			sf::RectangleShape exitEndButton = design.createButton({ 240,60 }, sf::Color::Black, { screen_resize * screenWidth / 2 - exitEndButton.getSize().x / 2 - 120, screen_resize * screenHeight / 2 - exitEndButton.getSize().y / 2 + 370 });
			sf::Text exitEndButtonText = design.createText("EXIT", font, 30, sf::Color::Black, { exitEndButton.getPosition().x + exitEndButton.getSize().x / 2 - exitEndButtonText.getGlobalBounds().width / 2 - 60, exitEndButton.getPosition().y + exitEndButton.getSize().y / 2 - exitEndButtonText.getGlobalBounds().height / 2 - 10 });
			
			bool playAgainButtonHighlighted = false;
			bool exitEndButtonHighlighted = false;
								
			while (endingWindow.isOpen())
			{
				sf::Event endingEvent;
				while (endingWindow.pollEvent(endingEvent))
				{
					switch (endingEvent.type)
					{
					case sf::Event::Closed:
					{
						endingWindow.close();
						window.close();
						break;
					}
					case sf::Event::MouseMoved:
					{
						sf::Vector2f mousePos = endingWindow.mapPixelToCoords(sf::Vector2i(endingEvent.mouseMove.x, endingEvent.mouseMove.y));

						if (inEndingScreen)
						{
							if (leaderboardButton.getGlobalBounds().contains(mousePos))
							{
								leaderboardButton.setFillColor(sf::Color::White);
								leaderboardButtonText.setFillColor(sf::Color::Black);
								leaderboardButtonHighlighted = true;
							}
							else if (playAgainButton.getGlobalBounds().contains(mousePos))
							{
								playAgainButton.setFillColor(sf::Color::White);
								playAgainButtonText.setFillColor(sf::Color::Black);
								playAgainButtonHighlighted = true;
							}
							else if (exitEndButton.getGlobalBounds().contains(mousePos))
							{
								exitEndButton.setFillColor(sf::Color::White);
								exitEndButtonText.setFillColor(sf::Color::Black);
								exitEndButtonHighlighted = true;
							}
							else
							{
								leaderboardButton.setFillColor(sf::Color::Black);
								playAgainButton.setFillColor(sf::Color::Black);
								exitEndButton.setFillColor(sf::Color::Black);
								leaderboardButtonText.setFillColor(sf::Color::White);
								playAgainButtonText.setFillColor(sf::Color::White);
								exitEndButtonText.setFillColor(sf::Color::White);
								leaderboardButtonHighlighted = false;
								playAgainButtonHighlighted = false;
								exitEndButtonHighlighted = false;
							}
						}
						if (inLeaderboardScreen)
						{
							if (backButton.getGlobalBounds().contains(mousePos))
							{
								backButton.setFillColor(sf::Color::White);
								backButtonText.setFillColor(sf::Color::Black);
								backButtonHighlighted = true;
							}
							else
							{
								backButton.setFillColor(sf::Color::Black);
								backButtonText.setFillColor(sf::Color::White);
								backButtonHighlighted = false;
							}
						}
						break;
					}
					case sf::Event::MouseButtonPressed:
					{
						sf::Vector2f mousePos = endingWindow.mapPixelToCoords(sf::Vector2i(endingEvent.mouseButton.x, endingEvent.mouseButton.y));

						if (inEndingScreen)
						{
							if (playAgainButton.getGlobalBounds().contains(mousePos))
							{
								inEndingScreen = false;
								inWelcomeScreen = true;
								resetGame();
								goto GameLoop;
							}
							else if (leaderboardButton.getGlobalBounds().contains(mousePos))
							{
								inEndingScreen = false;
								inLeaderboardScreen = true;
							}
							else if (exitEndButton.getGlobalBounds().contains(mousePos))
							{
								endingWindow.close();
								window.close();
								break;
							}
						}
						else if (inLeaderboardScreen)
						{
							if (backButton.getGlobalBounds().contains(mousePos))
							{
								inLeaderboardScreen = false;
								inEndingScreen = true;
							}
						}
						break;
					}
					}
				}
				endingWindow.clear();
				endingWindow.draw(backgroundSprite);

				if (inEndingScreen)
				{
					endingWindow.draw(gameOverText);
					endingWindow.draw(scoreLabel);
					endingWindow.draw(scoreText);
					endingWindow.draw(timeLabel);
					endingWindow.draw(timeText);
					endingWindow.draw(nicknameLabel);
					endingWindow.draw(nicknameText);
					endingWindow.draw(leaderboardButton);
					endingWindow.draw(leaderboardButtonText);
					endingWindow.draw(playAgainButton);
					endingWindow.draw(playAgainButtonText);
					endingWindow.draw(exitEndButton);
					endingWindow.draw(exitEndButtonText);

					if (leaderboardButtonHighlighted)
					{
						sf::RectangleShape leaderboardButtonHighlighted = design.createHighlightedButton({ leaderboardButton.getSize().x + 10, leaderboardButton.getSize().y + 10 }, sf::Color::Transparent, 5, sf::Color::White, { leaderboardButton.getPosition().x - 5, leaderboardButton.getPosition().y - 5 });
						endingWindow.draw(leaderboardButtonHighlighted);
					}
					else if (playAgainButtonHighlighted)
					{
						sf::RectangleShape playAgainButtonHighlighted = design.createHighlightedButton({ playAgainButton.getSize().x + 10, playAgainButton.getSize().y + 10 }, sf::Color::Transparent, 5, sf::Color::White, { playAgainButton.getPosition().x - 5, playAgainButton.getPosition().y - 5 });
						endingWindow.draw(playAgainButtonHighlighted);
					}
					else if (exitEndButtonHighlighted)
					{
						sf::RectangleShape exitEndButtonHighlighted = design.createHighlightedButton({ exitEndButton.getSize().x + 10, exitEndButton.getSize().y + 10 }, sf::Color::Transparent, 5, sf::Color::White, { exitEndButton.getPosition().x - 5, exitEndButton.getPosition().y - 5 });
						endingWindow.draw(exitEndButtonHighlighted);
					}
				}
				else if (inLeaderboardScreen)
				{
					std::vector<PlayerScore> leaderboard = loadLeaderboard();
					std::sort(leaderboard.begin(), leaderboard.end(), [](const PlayerScore& a, const PlayerScore& b)
						{
							if (a.score != b.score)
							{
								return a.score > b.score;  // Sort by score in descending order
							}
							else
							{
								return a.nickname < b.nickname;  // If scores are equal, sort by nickname in ascending order
							}
						});

					std::ostringstream oss;
					for (const PlayerScore& playerScore : leaderboard) {
						oss << playerScore.nickname << " - " << playerScore.score << "\n";
					}
					leaderboardText.setString(oss.str());

					endingWindow.draw(leaderboardTitle);
					endingWindow.draw(leaderboardText);
					endingWindow.draw(backButton);
					endingWindow.draw(backButtonText);

					if (backButtonHighlighted)
					{
						sf::RectangleShape backButtonHighlighted = design.createHighlightedButton({ backButton.getSize().x + 10, backButton.getSize().y + 10 }, sf::Color::Transparent, 5, sf::Color::White, { backButton.getPosition().x - 5, backButton.getPosition().y - 5 });
						endingWindow.draw(backButtonHighlighted);
					}
				}
				endingWindow.display();
			}
		}
	}
	return 0;
}