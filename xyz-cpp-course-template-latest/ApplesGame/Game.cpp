#include "Game.h"
#include <cassert>
#include <SFML/Graphics.hpp>

namespace ApplesGame
{
	void StartPlayingState(Game& game)
	{
		SetPlayerPosition(game.player, { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });
		SetPlayerSpeed(game.player, INITIAL_SPEED);
		SetPlayerDirection(game.player, PlayerDirection::Right);

			
			if (game.apples != nullptr) 
			{
				delete[] game.apples;
				game.apples = nullptr;
			}

			game.applesCount = 20 + rand() % 30;
			game.apples = new Apple[game.applesCount];

		// Init apples
		for (int i = 0; i < game.applesCount; ++i)
		{
			InitApple(game.apples[i], game);
			SetApplePosition(game.apples[i], GetRandomPositionInRectangle(game.screenRect));
		}

		// Init rocks
		for (int i = 0; i < NUM_ROCKS; ++i)
		{
			SetRockPosition(game.rocks[i], GetRandomPositionInRectangle(game.screenRect));
		}

		game.score = 0;
		game.isGameFinished = false;
		game.timeSinceGameFinish = 0;

		game.scoreText.setString("Score: " + std::to_string(game.score));
		
	}

	void UpdatePlayingState(Game& game, float deltaTime)
	{
		// Проверка на паузу
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
		{
			game.isGamePaused = !game.isGamePaused; // Переключаем состояние паузы
			sf::sleep(sf::milliseconds(200)); // Добавляем задержку, чтобы избежать множественных срабатываний
		}

		// Если игра на паузе, не обновляем игровую логику
		if (game.isGamePaused)
		{
			return;
		}

		// Handle input
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			SetPlayerDirection(game.player, PlayerDirection::Right);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			SetPlayerDirection(game.player, PlayerDirection::Up);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			SetPlayerDirection(game.player, PlayerDirection::Left);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			SetPlayerDirection(game.player, PlayerDirection::Down);
		}

		UpdatePlayer(game.player, deltaTime);

		// Find player collisions with apples
		for (int i = 0; i < game.applesCount; ++i)
		{
			if (DoShapesCollide(GetPlayerCollider(game.player), GetAppleCollider(game.apples[i])))
			{
				SetApplePosition(game.apples[i], GetRandomPositionInRectangle(game.screenRect));
				++game.score;
				SetPlayerSpeed(game.player, GetPlayerSpeed(game.player) + ACCELERATION);
				game.eatAppleSound.play();
				game.scoreText.setString("Score: " + std::to_string(game.score));
			}
		}

		// Find player collisions with rocks
		for (int i = 0; i < NUM_ROCKS; ++i)
		{
			if (DoShapesCollide(GetPlayerCollider(game.player), GetRockCollider(game.rocks[i])))
			{
				StartGameoverState(game);
			}
		}

		// Check screen borders collision
		if (!DoShapesCollide(GetPlayerCollider(game.player), game.screenRect))
		{
			StartGameoverState(game);
		}
	}

	void StartGameoverState(Game& game)
	{
		game.score = 0;
		game.isGameFinished = true;
		game.timeSinceGameFinish = 0.f;
		game.gameOverSound.play();
	}

	void UpdateGameoverState(Game& game, float deltaTime)
	{
		// Проверяем нажатие клавиши Пробел
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			// Сбрасываем фон и начинаем новую игру
	
			StartPlayingState(game);
		}
		
	}

	void InitGame(Game& game)
	{
		// Load resources
		assert(game.playerTexture.loadFromFile(RESOURCES_PATH + "\\Player.png"));
		assert(game.appleTexture.loadFromFile(RESOURCES_PATH + "\\Apple.png"));
		assert(game.rockTexture.loadFromFile(RESOURCES_PATH + "\\Rock.png"));
		assert(game.eatAppleSoundBuffer.loadFromFile(RESOURCES_PATH + "\\AppleEat.wav"));
		assert(game.gameOverSoundBuffer.loadFromFile(RESOURCES_PATH + "\\Death.wav"));
		assert(game.font.loadFromFile(RESOURCES_PATH + "\\Fonts\\Roboto-Regular.ttf"));


		// ИНИЦИАЛИЗАЦИЯ МЕНЮ
		game.titleText.setFont(game.font);
		game.titleText.setString("Apples Game");
		game.titleText.setCharacterSize(48);
		game.titleText.setFillColor(sf::Color::White);
		game.titleText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f - 50.f);
		CenterText(game.titleText); // Центрируем (функция ниже)

		// Настройка текста "Начать игру"
		game.startText.setFont(game.font);
		game.startText.setString("Start Game (Enter)");
		game.startText.setCharacterSize(24);
		game.startText.setFillColor(sf::Color::White);
		game.startText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f + 50.f);
		CenterText(game.startText);

		//настройки
		game.settingsText.setFont(game.font);
		game.settingsText.setString("Settings (S)");
		game.settingsText.setCharacterSize(24);
		game.settingsText.setFillColor(sf::Color::White);
		game.settingsText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f + 100.f);  // Ниже кнопки "Start Game"
		CenterText(game.settingsText);


		// Init game objects
		game.screenRect = { 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT };

		InitPlayer(game.player, game);

		// Init apples
		for (int i = 0; i < game.applesCount; ++i)
		{
			InitApple(game.apples[i], game);
		}

		// Init rocks
		for (int i = 0; i < NUM_ROCKS; ++i)
		{
			InitRock(game.rocks[i], game);
		}

		// Init background
		game.background.setSize(sf::Vector2f(game.screenRect.size.x, game.screenRect.size.y));
		game.background.setFillColor(sf::Color::Black);
		game.background.setPosition(0.f, 0.f);

		// Init sounds
		game.eatAppleSound.setBuffer(game.eatAppleSoundBuffer);
		game.gameOverSound.setBuffer(game.gameOverSoundBuffer);

		// Init texts
		game.scoreText.setFont(game.font);
		game.scoreText.setCharacterSize(20);
		game.scoreText.setFillColor(sf::Color::White);
		game.scoreText.setString("Score: " + std::to_string(game.score));
		game.scoreText.setPosition(10.f, 25.f);

		game.pauseText.setFont(game.font);
		game.pauseText.setCharacterSize(48);
		game.pauseText.setFillColor(sf::Color::White);
		game.pauseText.setString("\t\tPAUSED\nPress P to restart");
		game.pauseText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f - 50.f);
		CenterText(game.pauseText);

		game.controlsHintText.setFont(game.font);
		game.controlsHintText.setCharacterSize(20);
		game.controlsHintText.setFillColor(sf::Color::White);
		game.controlsHintText.setString("Press P for Pause, ESC to exit");
		game.controlsHintText.setPosition(10.f, 5.f);

		game.gameOverText.setFont(game.font);
		game.gameOverText.setCharacterSize(48);
		game.gameOverText.setFillColor(sf::Color::White);
		game.gameOverText.setString("\t\t\t\tGame Over\nPress SPACE to restart game");
		game.gameOverText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f - 50.f);
		CenterText(game.gameOverText);



		StartPlayingState(game);
	}


	void CenterText(sf::Text& text)
	{
		sf::FloatRect bounds = text.getLocalBounds();
		text.setOrigin(bounds.width / 2, bounds.height / 2);
	}

	void HandleMainMenuInput(Game& game)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
		{
			game.isInMainMenu = false;  // Начинаем игру
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			game.isInSettingsMenu = true;  // Переходим в меню настроек
		}
	}

	void UpdateGame(Game& game, float deltaTime)
	{

		if (game.isInMainMenu)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				game.isInMainMenu = false; //start game
				StartPlayingState(game);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				game.isInMainMenu = false;
				game.isInSettingsMenu = true;
			}
		}
		else if (game.isInSettingsMenu)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
			{
				game.isInSettingsMenu = false;
				game.isInMainMenu = true; // Возвращаемся в главное меню
			}
		}
		else
		{
			// Update game state
			if (!game.isGameFinished)
			{
				UpdatePlayingState(game, deltaTime);
			}
			else
			{
				UpdateGameoverState(game, deltaTime);
			}
		}
		
	}


	void DrawSettingsMenu(Game& game, sf::RenderWindow& window)
	{
		window.clear();

		// Заголовок
		sf::Text settingsTitle;
		settingsTitle.setFont(game.font);
		settingsTitle.setString("Settings");
		settingsTitle.setCharacterSize(48);
		settingsTitle.setPosition(SCREEN_WIDTH / 2.f, 100.f);
		CenterText(settingsTitle);
		window.draw(settingsTitle);

		// Кнопка "Назад"
		sf::Text backText;
		backText.setFont(game.font);
		backText.setString("Back to Menu (B)");
		backText.setCharacterSize(24);
		backText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT - 100.f);
		CenterText(backText);
		window.draw(backText);
	}

	void DrawGame(Game& game, sf::RenderWindow& window)
	{

		if (game.isInMainMenu)
		{
			// Рисуем только меню
			window.draw(game.titleText);
			window.draw(game.startText);
			window.draw(game.settingsText);
		}
		else if (game.isInSettingsMenu)
		{
			DrawSettingsMenu(game, window);
		}
		else
		{
			// Draw game objects
			DrawPlayer(game.player, window);
			for (int i = 0; i < game.applesCount; ++i)
			{
				DrawApple(game.apples[i], window);
			}

			for (int i = 0; i < NUM_ROCKS; ++i)
			{
				DrawRock(game.rocks[i], window);
			}

			// Draw texts
			if (!game.isGameFinished)
			{
				window.draw(game.controlsHintText);
				window.draw(game.scoreText);


				if (game.isGamePaused)
				{
					window.draw(game.pauseText);

				}
			}
			else
			{
				window.draw(game.gameOverText);
				window.draw(game.scoreText);
			}
		}
	}

	void DeinializeGame(Game& game)
	{
		delete[] game.apples;  // Важно: очищаем, даже если игра перезапускалась
		game.apples = nullptr;
		game.applesCount = 0;
	}
}
