export module Global;

import <chrono>;
import <vector>;
import <array>;

export constexpr unsigned short screenHeight = 240;
export constexpr unsigned short screenWidth = 320;
export constexpr unsigned char cellSize = 16;
export constexpr float gravity = 0.25f;
export constexpr unsigned char screen_resize = 4;
export constexpr float maxVerticalSpeed = 8;

export constexpr unsigned char dogDeathDur = 32;
export constexpr unsigned char dogInvincibilityDur = 64;
export constexpr unsigned char dogBlinking = 4;
export constexpr float dogAcceleration = 0.25f;
export constexpr float dogWalkSpeed = 2;
export constexpr unsigned short dogAnimWalkSpeed = 4;
export constexpr float dogJumpSpeed = -4;
export constexpr unsigned char dogJumpTimer = 8;
export constexpr unsigned char dogGrowthDuration = 64;

export constexpr unsigned short dogCatcherDeathDur = 32;
export constexpr float dogCatcherSpeed = 1;
export constexpr unsigned short dogCatcherAnimWalkSpeed = 32;

export constexpr float boneSpeed = 1;
export constexpr unsigned short boneAnimSpeed = 4;

export constexpr unsigned short mysteryBlockAnimSpeed = 8;

export constexpr float boarWaitSpeed = 4;
export constexpr float boarSpeed = 1;
export constexpr unsigned short boarGetOutAnimSpeed = 4;
export constexpr unsigned short boarGetOutDuration = 512;
export constexpr unsigned short boarAnimWalkSpeed = 32;

export constexpr float brickParticleSpeed = 4;
export constexpr float coinJump = -6;
export constexpr unsigned short coinAnimationSpeed = 4;

export constexpr unsigned short entityUpdateArea = 64;

export constexpr std::chrono::microseconds frameDuration(16667);

//possible types of cells in a game map
export enum Cell
{
    ActivatedQuestionBlock,
    Brick,
    Coin,
    Empty,
    Pipe,
    QuestionBlock,
    Wall
};

//struct representing a particle in a particle system
export struct Particle
{
    float horizontal_speed;
    float vertical_speed;
    float x;
    float y;

    Particle(const float i_x, const float i_y, const float i_horizontal_speed = 0, const float i_vertical_speed = 0) :
        horizontal_speed(i_horizontal_speed),
        vertical_speed(i_vertical_speed),
        x(i_x),
        y(i_y)
    {

    }
};

//Typedef of a 2D vector representing the game map;  MyMap type is defined as a vector of arrays of Cell objects
//Each array represents a row
export typedef std::vector<std::array<Cell, screenHeight / cellSize>> MyMap;