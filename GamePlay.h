#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <string>
#include <regex>
#include <filesystem>

#include "Boar.h"
#include "ConvertingMapSketch.h"
#include "DogCatcher.h"
#include "Design.h"

struct PlayerScore
{
	std::string nickname;
	int score;
};

class GamePlay
{
public:
	int runGame();

	void resetGame();
	
	void saveLeaderboard(const std::vector<PlayerScore>& leaderboard);
	
	std::vector<PlayerScore> loadLeaderboard();

private:
	unsigned char currentLevel;
	unsigned short levelFinish;
	unsigned short gameFinish;

	bool game_over;
	bool dogAlive;
	bool restartGame;

	int numberOfCollisionsWithBone;
	int numberOfCoinsCollected;
	int numberOfEnemiesKilled;
	int playerScore;

	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point previousTime;

	std::vector<std::shared_ptr<Enemy>> enemies; //to store all enemies in one vector
	std::vector<int> secondsVector;
	std::vector<int> numebrOfCollisionsVector;
	std::vector<int> numberOfCoinsVector;
	std::vector<int> numeberOfEnemiesVector;

	sf::Color backgroundColor = sf::Color(0, 219, 255);

	Map map;
	Dog dog;
};


