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

		// Init apples
		int appleCount = NUM_APPLES;
		if (game.gameMode & MODE_50_APPLES)
		{
			appleCount = NUM_APPLES_EXTENDED;
		}
		
		

		game.applesCount = appleCount;
		game.apples = new Apple[game.applesCount];

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
		// �������� �� �����
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
		{
			game.isGamePaused = !game.isGamePaused; // ����������� ��������� �����
			sf::sleep(sf::milliseconds(200)); // ��������� ��������, ����� �������� ������������� ������������
		}

		// ���� ���� �� �����, �� ��������� ������� ������
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

		int appleCount;
		if (game.gameMode & MODE_50_APPLES)
		{
			appleCount = NUM_APPLES_EXTENDED;
		}
		else
		{
			appleCount = NUM_APPLES;
		}

		// Find player collisions with apples
		for (int i = 0; i < game.applesCount; ++i)
		{
			if (DoShapesCollide(GetPlayerCollider(game.player), GetAppleCollider(game.apples[i])))
			{
				++game.score;
				game.eatAppleSound.play();

				if (!(game.gameMode & MODE_NO_ACCELERATION))
				{
					SetPlayerSpeed(game.player, GetPlayerSpeed(game.player) + ACCELERATION);
					if (game.player.speed > MAX_SPEED)
						game.player.speed = MAX_SPEED;
				}
				if (game.gameMode & MODE_INFINITE_APPLES)
				{
					SetApplePosition(game.apples[i], GetRandomPositionInRectangle(game.screenRect));
				}
				else
				{
					//  ������� ������ �� ����� ��� �������� ��� "���������"
					SetApplePosition(game.apples[i], { -100, -100 }); // �������� ������
				}
				game.scoreText.setString("Score: " + std::to_string(game.score));
			}
		}

		int targetApples;
		if (game.gameMode & MODE_50_APPLES)
		{
			targetApples = NUM_APPLES_EXTENDED;
		}
		else
		{
			targetApples = NUM_APPLES;
		}
		if (!(game.gameMode & MODE_INFINITE_APPLES) && game.score >= targetApples)
		{
			game.isGameFinished = true;
			game.timeSinceGameFinish = 0.f;
		}


		/*if (game.isGameFinished && (game.gameMode & MODE_INFINITE_APPLES))

		{
			for (auto& rec : leaderboard)
			{
				if (rec.name == "Player" && game.score > rec.score)
				{
					rec.score = game.score;
				}
			}
			SortLeaderboard(leaderboard);
		}*/

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
		// ��������� ������� ������� ������
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			// ���������� ��� � �������� ����� ����

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

		// ������������� ����
		game.titleText.setFont(game.font);
		game.titleText.setString("Apples Game");
		game.titleText.setCharacterSize(48);
		game.titleText.setFillColor(sf::Color::White);
		game.titleText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f - 50.f);
		CenterText(game.titleText); // ���������� (������� ����)

		// ��������� ������ "������ ����"
		game.startText.setFont(game.font);
		game.startText.setString("Start Game (Enter)");
		game.startText.setCharacterSize(24);
		game.startText.setFillColor(sf::Color::White);
		game.startText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f + 50.f);
		CenterText(game.startText);

		//���������
		game.settingsText.setFont(game.font);
		game.settingsText.setString("Settings (S)");
		game.settingsText.setCharacterSize(24);
		game.settingsText.setFillColor(sf::Color::White);
		game.settingsText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f + 100.f);  // ���� ������ "Start Game"
		CenterText(game.settingsText);

		// Init game objects
		game.screenRect = { 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT };

		InitPlayer(game.player, game);

		// Init apples
		game.applesCount = 0;
		game.apples = new Apple[game.applesCount]; // �������� ������
		for (int i = 0; i < game.applesCount; ++i)
		{
			InitApple(game.apples[i], game);
			SetApplePosition(game.apples[i], GetRandomPositionInRectangle(game.screenRect));
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

		HandleGameModeMenuInput;
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
			game.isInMainMenu = false; // �������� ����
			
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			game.isInMainMenu = false;
			game.isInSettingsMenu = true;  // ��������� � ���� ��������
		}
	}

	

	void UpdateGame(Game& game, float deltaTime, sf::Event event)
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
			HandleGameModeMenuInput(game, event);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
			{
				game.isInSettingsMenu = false;
				game.isInMainMenu = true; // ������������ � ������� ����
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

	void HandleGameModeMenuInput(Game& game, sf::Event event)
	{
		if (game.isInSettingsMenu)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Num1: // ����������� ����� (20 ����� � ����������)
				game.gameMode = MODE_DEFAULT;
				break;

			case sf::Keyboard::Num2: // ����������� ������ � ����������
				game.gameMode = MODE_INFINITE_APPLES;
				break;

			case sf::Keyboard::Num3: // 20 ����� ��� ���������
				game.gameMode = MODE_NO_ACCELERATION;
				break;

			case sf::Keyboard::Num4: // ����������� ������ ��� ���������
				game.gameMode = MODE_INFINITE_APPLES | MODE_NO_ACCELERATION;
				break;

			case sf::Keyboard::Num5:
				game.gameMode = MODE_50_APPLES; // ������ 50 �����
				break;

			case sf::Keyboard::B:
				game.isInSettingsMenu = false;
				game.isInMainMenu = true;
				break;
			}
		}
	}
	

	void DrawSettingsMenu(Game& game, sf::RenderWindow& window)
	{
		window.clear();

		// ���������
		sf::Text settingsTitle;
		settingsTitle.setFont(game.font);
		settingsTitle.setString("Settings");
		settingsTitle.setCharacterSize(48);
		settingsTitle.setPosition(SCREEN_WIDTH / 2.f, 50.f);
		CenterText(settingsTitle);
		window.draw(settingsTitle);

		sf::Text title;
		title.setFont(game.font);
		title.setString("Select Game Mode");
		title.setCharacterSize(40);
		title.setFillColor(sf::Color::White);
		title.setPosition(SCREEN_WIDTH / 2.f, 100.f);
		CenterText(title);
		window.draw(title);

		sf::Text mode1;
		mode1.setFont(game.font);
		mode1.setString("1.20 apples with acceleration ");
		mode1.setCharacterSize(24);
		mode1.setPosition(SCREEN_WIDTH / 2.f, 200.f);
		CenterText(mode1);
		window.draw(mode1);

		sf::Text mode2;
		mode2.setFont(game.font);
		mode2.setString("2. Infinite apples with acceleration");
		mode2.setCharacterSize(24);
		mode2.setPosition(SCREEN_WIDTH / 2.f, 250.f);
		CenterText(mode2);
		window.draw(mode2);

		sf::Text mode3;
		mode3.setFont(game.font);
		mode3.setString("3. 20 apples without acceleration");
		mode3.setCharacterSize(24);
		mode3.setPosition(SCREEN_WIDTH / 2.f, 300.f);
		CenterText(mode3);
		window.draw(mode3);

		sf::Text mode4;
		mode4.setFont(game.font);
		mode4.setString("4. Infinite apples without acceleration");
		mode4.setCharacterSize(24);
		mode4.setPosition(SCREEN_WIDTH / 2.f, 350.f);
		CenterText(mode4);
		window.draw(mode4);

		sf::Text mode5;
		mode5.setFont(game.font);
		mode5.setString("5. 50 apples with acceleration");
		mode5.setCharacterSize(24);
		mode5.setPosition(SCREEN_WIDTH / 2.f, 400.f);
		CenterText(mode5);
		window.draw(mode5);


		// ������ "�����"
		sf::Text backText;
		backText.setFont(game.font);
		backText.setString("Back to Menu (B)");
		backText.setCharacterSize(24);
		backText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT - 450.f);
		CenterText(backText);
		window.draw(backText);
	}

	void DrawGame(Game& game, sf::RenderWindow& window)
	{

		if (game.isInMainMenu)
		{
			// ������ ������ ����
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
		delete[] game.apples;  // �����: �������, ���� ���� ���� ���������������
		game.apples = nullptr;
		game.applesCount = 0;
	}
}