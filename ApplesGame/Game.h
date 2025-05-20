#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "Constants.h"
#include "Math.h"
#include "Player.h"
#include "Apple.h"
#include "Rock.h"

namespace ApplesGame
{
	struct Game
	{
		Rectangle screenRect;
		

		Player player;
		Apple* apples=nullptr;
		int applesCount = 0;
		Rock rocks[NUM_ROCKS];

		// Global game data
		int score = 0;
		bool isGameFinished = false;
		bool isGamePaused = false;
		float timeSinceGameFinish = 0.f;
		bool isInMainMenu = true;
		bool isInSettingsMenu = false;

		// Resources
		sf::Texture playerTexture;
		sf::Texture appleTexture;
		sf::Texture rockTexture;
		sf::SoundBuffer eatAppleSoundBuffer;
		sf::SoundBuffer gameOverSoundBuffer;

		// Game objects
		sf::Sound eatAppleSound;
		sf::Sound gameOverSound;
		sf::RectangleShape background;

		// Texts and fonts
		sf::Font font;
		sf::Text scoreText;
		sf::Text pauseText;
		sf::Text controlsHintText;
		sf::Text gameOverText;

		//МЕНЮ
		 // Добавляем флаг меню
		sf::Text titleText;       // Текст "Apples Game"
		sf::Text startText;       // Текст "Начать игру (Enter)"
		sf::Text settingsText;  // Новая кнопка "Настройки"


	};

	void InitGame(Game& game);
	void UpdateGame(Game& game, float deltaTime);
	

	void HandleMainMenuInput(Game& game);
	void DrawSettingsMenu(Game& game, sf::RenderWindow& window);

	void CenterText(sf::Text& text);


	void StartPlayingState(Game& game);
	void UpdatePlayingState(Game& game, float deltaTime);

	void StartGameoverState(Game& game);
	void UpdateGameoverState(Game& game, float deltaTime);

	void DrawGame(Game& game, sf::RenderWindow& window);
	void DeinializeGame(Game& game);
}
