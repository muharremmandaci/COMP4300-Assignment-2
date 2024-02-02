#include "Game.h"
#include <fstream>
#include <sstream>
#include <iostream>


Game::Game(const std::string& configFile)
{
	Init(configFile);
}

void Game::Run()
{

	while (Window.isOpen()) {

		Gui.Update(DeltaClock, EntityManager.GetEntityMap());

		SpawnInterval   = Gui.GetSpawnInterval();
		ManuelSpawnFlag = Gui.GetManuelSpawnFlag();

		sUserInput();

		if (IsRunning) {
			GameFrameCount++;

			ScoreText.setString("Score : " + std::to_string(GameScore));
			EntityManager.Update();

			if (Gui.IsLifeSpanActive) {
				sLifeSpan();
			}
			if (Gui.IsSpawnerActive) {
				sSpawner();
			}
			if (Gui.IsMovementActive) {
				sMovement();
			}
			if (Gui.IsSpecialShootActive) {
				sSpecialShoot();
			}
			if (Gui.IsCollisionActive) {
				sCollision();
			}
				sRender();
		}
	}

}

void Game::Init(const std::string& configFile)
{
	GameFrameCount = 0;
	IsRunning = true;
	
	//Read in the config file 
	std::fstream input{ configFile };

	if (!input.is_open())
	{
		std::cerr << "Failed to open: " << configFile << '\n';
		exit(-1);
	}

	std::string identifier;

	while (input >> identifier) {
		if (identifier == "Window")
		{
			int width;
			input >> width;

			int height;
			input >> height;

			int fps;
			input >> fps;

			int fullscreen;
			input >> fullscreen;

			if (fullscreen == 1) {
				Window.create(sf::VideoMode(width, height), "Geometry Wars", sf::Style::Close);
			}
			else {
				auto fullScreenModes = sf::VideoMode::getFullscreenModes();
				Window.create(fullScreenModes[0], "Geometry Wars", sf::Style::Fullscreen);
			}
			Window.setFramerateLimit(fps);
		}
		else if (identifier == "Font") {
			std::string filepath;
			input >> filepath;
			if (!GameFont.loadFromFile(filepath))
			{
				std::cerr << "Failed to load font. Filepath: " << filepath;
			}

			ScoreText.setFont(GameFont);
			CooldownText.setFont(GameFont);

			int fontSize;
			input >> fontSize;
			ScoreText.setCharacterSize(fontSize);
			CooldownText.setCharacterSize(fontSize);

			sf::Vector3<sf::Uint16> textColor;
			input >> textColor.x;
			input >> textColor.y;
			input >> textColor.z;
			//ScoreText.setFillColor(sf::Color(RGB.r, RGB.g, RGB.b));
			ScoreText.setFillColor(sf::Color(textColor.x, textColor.y, textColor.z));
			CooldownText.setFillColor(sf::Color(textColor.x, textColor.y, textColor.z));
		}
		else if (identifier == "Player") {
			input >> PlayerSpecs.ShapeRadius;
			input >> PlayerSpecs.CollisionRadius;
			input >> PlayerSpecs.Speed;

			sf::Vector3<sf::Uint16> color;

			input >> color.x;
			input >> color.y;
			input >> color.z;
			PlayerSpecs.FillColor.r = color.x;
			PlayerSpecs.FillColor.g = color.y;
			PlayerSpecs.FillColor.b = color.z;

			input >> color.x;
			input >> color.y;
			input >> color.z;
			PlayerSpecs.OutlineColor.r = color.x;
			PlayerSpecs.OutlineColor.g = color.y;
			PlayerSpecs.OutlineColor.b = color.z;

			input >> PlayerSpecs.OutlineThickness;
			input >> PlayerSpecs.ShapeVertices;
		}
		else if (identifier == "Enemy") {
			input >> EnemySpecs.ShapeRadius;
			input >> EnemySpecs.CollisionRadius;
			input >> EnemySpecs.Speed.Min;
			input >> EnemySpecs.Speed.Max;

			sf::Vector3<sf::Uint16> color;
			input >> color.x;
			input >> color.y;
			input >> color.z;
			EnemySpecs.OutlineColor.r = color.x;
			EnemySpecs.OutlineColor.g = color.y;
			EnemySpecs.OutlineColor.b = color.z;

			input >> EnemySpecs.OutlineThickness;
			input >> EnemySpecs.ShapeVertices.Min;
			input >> EnemySpecs.ShapeVertices.Max;
			input >> EnemySpecs.LifeSpan;
			input >> EnemySpecs.SpawnInterval;
		}
		else if (identifier == "Bullet") {
			input >> BulletSpecs.ShapeRadius;
			input >> BulletSpecs.CollisionRadius;
			input >> BulletSpecs.Speed;
			sf::Vector3<sf::Uint16> color;

			input >> color.x;
			input >> color.y;
			input >> color.z;
			BulletSpecs.FillColor.r = color.x;
			BulletSpecs.FillColor.g = color.y;
			BulletSpecs.FillColor.b = color.z;

			input >> color.x;
			input >> color.y;
			input >> color.z;
			BulletSpecs.OutlineColor.r = color.x;
			BulletSpecs.OutlineColor.g = color.y;
			BulletSpecs.OutlineColor.b = color.z;

			input >> BulletSpecs.OutlineThickness;
			input >> BulletSpecs.ShapeVertices;
			input >> BulletSpecs.LifeSpan;
		}

		ScoreText.setPosition(0, 0);
		ScoreText.setString(std::to_string(0));

		CooldownText.setPosition(0, Window.getSize().y - CooldownText.getCharacterSize());
		CooldownText.setString(std::to_string(0));

	}
	SpawnPlayer();
	SpawnInterval = EnemySpecs.SpawnInterval;
	Gui.Init(&Window, SpawnInterval);
}

void Game::SpawnPlayer()
{
	//This goes slightly against the EntityManagers paradigm
	Player = EntityManager.AddEntity("Player");

	sf::Vector2f middleOfWindow{ Window.getSize().x / 2.0f, Window.getSize().y / 2.0f };

	Player->cTransform = std::make_shared<CTransform>(middleOfWindow, sf::Vector2f(0.0f, 0.0f), 0.0f);
	Player->cShape = std::make_shared<CShape>(PlayerSpecs.ShapeRadius, PlayerSpecs.ShapeVertices,
		PlayerSpecs.FillColor, PlayerSpecs.OutlineColor, PlayerSpecs.OutlineThickness);
	Player->cCollision = std::make_shared<CCollision>(PlayerSpecs.CollisionRadius);
	Player->cInput = std::make_shared<CInput>();
	Player->cSpecialShoot = std::make_shared<CSpecialShoot>(40, 60 * 10);
}

void Game::SpawnEnemy()
{
	std::srand((unsigned)std::time(NULL) * GameFrameCount);

	auto enemy = EntityManager.AddEntity("Enemy");

	auto radius = EnemySpecs.ShapeRadius;

	float minX = 0 + radius + 5;
	float maxX = Window.getSize().x - radius - 5;

	float minY = 0 + radius + 5;
	float maxY = Window.getSize().y - radius - 5;

	auto shapeX = minX + (std::rand() % (int)(1 + maxX - minX));
	auto shapeY = minY + (std::rand() % (int)(1 + maxY - minY));

	sf::Vector2f shapePos{ shapeX, shapeY };

	float angle = 0 + (std::rand() % (1 + (360 - 0)));
	angle = angle * 3.14f / 180.0f;

	auto minSpeed = EnemySpecs.Speed.Min;
	auto maxSpeed = EnemySpecs.Speed.Max;
	
	auto speed = minSpeed + (std::rand() % (1 + int(maxSpeed - minSpeed)));
	// To prevent low speed
	auto signSpeed = 1;
	if (speed != 0) {
		signSpeed = speed / std::fabs(speed);
	}
	
	speed = speed + 0.5f * signSpeed;

	auto velX = std::cosf(angle) * speed;
	auto velY = std::sinf(angle) * speed;


	sf::Vector2f shapeVel{ velX,  velY };

	enemy->cTransform = std::make_shared<CTransform>(shapePos, shapeVel, angle);
	enemy->cCollision = std::make_shared<CCollision>(EnemySpecs.CollisionRadius);
	enemy->cLifeSpan = std::make_shared<CLifeSpan>(EnemySpecs.LifeSpan);

	auto minVertices = EnemySpecs.ShapeVertices.Min;
	auto maxVertices = EnemySpecs.ShapeVertices.Max;
	auto vertices = minVertices + (std::rand() % (1 + maxVertices - minVertices));

	uint8_t minColor = 0;
	uint8_t maxColor = 255;
	uint8_t r = minColor + (std::rand() % (1 + maxColor - minColor));
	uint8_t g = minColor + (std::rand() % (1 + maxColor - minColor));
	uint8_t b = minColor + (std::rand() % (1 + maxColor - minColor));

	auto fillColor = sf::Color(r, g, b);

	enemy->cShape = std::make_shared<CShape>(EnemySpecs.ShapeRadius, vertices, fillColor,
		EnemySpecs.OutlineColor, EnemySpecs.OutlineThickness);

	enemy->cScore = std::make_shared<CScore>(vertices * 100);

	std::cout << "x : " << shapeX << ", y : " << shapeY << ", angle : " << angle << std::endl;
}

void Game::SpawnBullet(const float angle)
{
	auto bullet = EntityManager.AddEntity("Bullet");
	bullet->cShape = std::make_shared<CShape>(BulletSpecs.ShapeRadius, BulletSpecs.ShapeVertices,
		BulletSpecs.FillColor, BulletSpecs.OutlineColor, BulletSpecs.OutlineThickness);
	bullet->cCollision = std::make_shared<CCollision>(BulletSpecs.CollisionRadius);
	bullet->cLifeSpan = std::make_shared<CLifeSpan>(BulletSpecs.LifeSpan);

	auto velX = std::cosf(angle) * BulletSpecs.Speed;
	auto velY = std::sinf(angle) * BulletSpecs.Speed;
	sf::Vector2f shapeVel{ velX,  velY };
	bullet->cTransform = std::make_shared<CTransform>(Player->cTransform->Position, shapeVel, angle);

}

void Game::SpawnSmallEnemy(std::shared_ptr<EntityCls> deadEnemy)
{
	if (deadEnemy->GetTag() == "Enemy") {
		auto count = deadEnemy->cShape->Shape.getPointCount();
		for (size_t i = 0; i < count; i++)
		{
			auto smallEnemy = EntityManager.AddEntity("SmallEnemy");

			auto angle = (360.0f / count) * i * 3.14f / 180.0f;
			auto speed = std::sqrtf(std::powf(deadEnemy->cTransform->Velocity.x, 2) + std::powf(deadEnemy->cTransform->Velocity.y, 2));
			auto velX = std::cosf(angle) * speed;
			auto velY = -1 * std::sinf(angle) * speed;

			smallEnemy->cTransform = std::make_shared<CTransform>(deadEnemy->cTransform->Position, sf::Vector2f(velX, velY), angle);
			smallEnemy->cCollision = std::make_shared<CCollision>(deadEnemy->cCollision->Radius / 3.0f);
			smallEnemy->cLifeSpan = std::make_shared<CLifeSpan>(deadEnemy->cLifeSpan->TotalLife / 2.0f);

			smallEnemy->cShape = std::make_shared<CShape>(deadEnemy->cShape->Shape.getRadius() / 3.0f,
				deadEnemy->cShape->Shape.getPointCount(), deadEnemy->cShape->Shape.getFillColor(),
				deadEnemy->cShape->Shape.getOutlineColor(), deadEnemy->cShape->Shape.getOutlineThickness());

			smallEnemy->cScore = std::make_shared<CScore>(deadEnemy->cShape->Shape.getPointCount() * 100 * 2);
		}
	}

}

void Game::sMovement()
{
	if (Player->cInput->Down) {
		Player->cTransform->Velocity.y = PlayerSpecs.Speed;
	}
	else if (Player->cInput->Up) {
		Player->cTransform->Velocity.y = -PlayerSpecs.Speed;
	}
	else {
		Player->cTransform->Velocity.y = 0;
	}

	if (Player->cInput->Right) {
		Player->cTransform->Velocity.x = PlayerSpecs.Speed;
	}
	else if (Player->cInput->Left) {
		Player->cTransform->Velocity.x = -PlayerSpecs.Speed;
	}
	else {
		Player->cTransform->Velocity.x = 0;
	}

	for (auto& e : EntityManager.GetEntities()) {
		if (e->cTransform != nullptr)
		{
			e->cTransform->Position.x += e->cTransform->Velocity.x;
			e->cTransform->Position.y += e->cTransform->Velocity.y;

		}
	}
}

void Game::sRender()
{
	Window.clear();

	for (auto& e : EntityManager.GetEntities()) {
		if (e->cShape != nullptr)
		{
			e->cTransform->Angle += 1;

			e->cShape->Shape.setPosition(e->cTransform->Position);
			e->cShape->Shape.setRotation(e->cTransform->Angle);
			Window.draw(e->cShape->Shape);
		}
	}
	Window.draw(ScoreText);
	Window.draw(CooldownText);
	Gui.Render();
	Window.display();
}

void Game::sSpawner()
{
	if ((GameFrameCount % SpawnInterval) == 0) {
		SpawnEnemy();
	}

	if (ManuelSpawnFlag) {
		SpawnEnemy();
	}

	if (Player->cInput->Shoot) {
		Player->cInput->Shoot = false;
		auto playerX = Player->cTransform->Position.x;
		auto playerY = Player->cTransform->Position.y;

		auto diffX = Player->cInput->MousePos.x - playerX;
		auto diffY = Player->cInput->MousePos.y - playerY;

		auto angle = std::atan2f(diffY, diffX);

		SpawnBullet(angle);

	}

}

void Game::sCollision()
{
	auto windowX = Window.getSize().x;
	auto windowY = Window.getSize().y;

	auto entities = EntityManager.GetEntities();
	auto enemies = EntityManager.GetEntities("Enemy");
	auto bullets = EntityManager.GetEntities("Bullet");
	auto smallEnemies = EntityManager.GetEntities("SmallEnemy");

	if ((Player->cTransform->Position.x - Player->cCollision->Radius) < 0) {
		Player->cTransform->Position.x = Player->cCollision->Radius;
	}
	else if ((Player->cTransform->Position.x + Player->cCollision->Radius) > windowX) {
		Player->cTransform->Position.x = windowX - Player->cCollision->Radius;
	}
	else if ((Player->cTransform->Position.y - Player->cCollision->Radius) < 0) {
		Player->cTransform->Position.y += Player->cCollision->Radius;
	}
	else if ((Player->cTransform->Position.y + Player->cCollision->Radius) > windowY) {
		Player->cTransform->Position.y -= Player->cCollision->Radius;
	}

	// check player - enemy collision


	for (auto& enemy : enemies) {
		if (enemy->cCollision != nullptr) {
			auto diffX = Player->cTransform->Position.x - enemy->cTransform->Position.x;
			auto diffY = Player->cTransform->Position.y - enemy->cTransform->Position.y;
			auto radiusEnemy = enemy->cCollision->Radius;
			auto radiusPlayer = Player->cCollision->Radius;

			if (((diffX * diffX) + (diffY * diffY)) <= std::pow((radiusPlayer + radiusEnemy), 2)) {
				enemy->Destroy();
				Player->Destroy();
				SpawnPlayer();
				GameScore = 0;
			}
		}
	}

	for (auto& enemy : smallEnemies) {
		if (enemy->cCollision != nullptr) {
			auto diffX = Player->cTransform->Position.x - enemy->cTransform->Position.x;
			auto diffY = Player->cTransform->Position.y - enemy->cTransform->Position.y;
			auto radiusEnemy = enemy->cCollision->Radius;
			auto radiusPlayer = Player->cCollision->Radius;

			if (((diffX * diffX) + (diffY * diffY)) <= std::pow((radiusPlayer + radiusEnemy), 2)) {
				enemy->Destroy();
				Player->Destroy();
				SpawnPlayer();
				GameScore = 0;
			}
		}
	}

	// check window bound collision
	for (auto& e : entities) {
		if (e->cCollision != nullptr) {
			auto pos = e->cTransform->Position;
			auto radius = e->cCollision->Radius;

			if ((pos.x - radius) < 0 || (pos.x + radius) > windowX) {
				e->cTransform->Velocity.x *= -1;
			}

			if ((pos.y - radius) < 0 || (pos.y + radius) > windowY) {
				e->cTransform->Velocity.y *= -1;
			}
		}
	}

	// check bullet-enemy collision


	for (auto& bullet : bullets) {
		auto radiusBullet = bullet->cCollision->Radius;
		for (auto& enemy : enemies) {
			auto diffX = bullet->cTransform->Position.x - enemy->cTransform->Position.x;
			auto diffY = bullet->cTransform->Position.y - enemy->cTransform->Position.y;
			auto radiusEnemy = enemy->cCollision->Radius;

			if (((diffX * diffX) + (diffY * diffY)) <= std::pow((radiusBullet + radiusEnemy), 2)) {
				enemy->Destroy();
				SpawnSmallEnemy(enemy);
				bullet->Destroy();
				GameScore += enemy->cScore->Score;
			}
		}
		for (auto& enemy : smallEnemies) {
			auto diffX = bullet->cTransform->Position.x - enemy->cTransform->Position.x;
			auto diffY = bullet->cTransform->Position.y - enemy->cTransform->Position.y;
			auto radiusEnemy = enemy->cCollision->Radius;

			if (((diffX * diffX) + (diffY * diffY)) <= std::pow((radiusBullet + radiusEnemy), 2)) {
				enemy->Destroy();
				bullet->Destroy();
				GameScore += enemy->cScore->Score;
			}
		}
	}
}

void Game::sUserInput()
{
	sf::Event event;

	while (Window.pollEvent(event))
	{
		Gui.ProcessEvent(event);
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape) {
				Window.close();
			}

			if (event.key.code == sf::Keyboard::W) {
				Player->cInput->Up = true;
			}
			if (event.key.code == sf::Keyboard::A) {
				Player->cInput->Left = true;
			}
			if (event.key.code == sf::Keyboard::S) {
				Player->cInput->Down = true;
			}
			if (event.key.code == sf::Keyboard::D) {
				Player->cInput->Right = true;
			}
			if (event.key.code == sf::Keyboard::P) {
				IsRunning = !IsRunning;
			}

		}

		if (event.type == sf::Event::MouseButtonPressed && !Gui.GetAnyItemHovered()) {
			if (IsRunning) {

				if (event.mouseButton.button == sf::Mouse::Left) {
					auto mouseX = event.mouseButton.x;
					auto mouseY = event.mouseButton.y;


					Player->cInput->Shoot = true;
					Player->cInput->MousePos = sf::Vector2f(mouseX, mouseY);
				}
				if (event.mouseButton.button == sf::Mouse::Right) {
					Player->cInput->SpecialShoot = true;
				}
			}
		}

		if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::W) {
				Player->cInput->Up = false;
			}
			if (event.key.code == sf::Keyboard::A) {
				Player->cInput->Left = false;
			}
			if (event.key.code == sf::Keyboard::S) {
				Player->cInput->Down = false;
			}
			if (event.key.code == sf::Keyboard::D) {
				Player->cInput->Right = false;
			}
		}

	}
}

void Game::sLifeSpan()
{
	auto entities = EntityManager.GetEntities();
	for (auto entity : entities) {
		if (entity->cLifeSpan != nullptr) {
			entity->cLifeSpan->RemainingLife -= 1;
			auto fillColor = entity->cShape->Shape.getFillColor();
			auto outlineColor = entity->cShape->Shape.getOutlineColor();

			uint8_t alpha = ((float)entity->cLifeSpan->RemainingLife / entity->cLifeSpan->TotalLife) * 255;
			fillColor.a = alpha;
			outlineColor.a = alpha;
			entity->cShape->Shape.setFillColor(fillColor);
			entity->cShape->Shape.setOutlineColor(outlineColor);

			if (entity->cLifeSpan->RemainingLife <= 0) {
				entity->Destroy();
			}
		}
	}
}

void Game::sSpecialShoot()
{
	if (Player->cInput->SpecialShoot) {
		if (Player->cSpecialShoot->RemainingCooldown == 0) {
			// spawn many bullet
			for (size_t i = 0; i < Player->cSpecialShoot->BulletAmount; i++)
			{
				float angle = (360.0f / Player->cSpecialShoot->BulletAmount) * i;
				SpawnBullet(angle);
			}

			// set up cooldown
			Player->cSpecialShoot->RemainingCooldown = Player->cSpecialShoot->Cooldown;
		}
		Player->cInput->SpecialShoot = false;
	}

	if (Player->cSpecialShoot->RemainingCooldown > 0) {
		Player->cSpecialShoot->RemainingCooldown--;
	}

	CooldownText.setString("Cooldown : " + std::to_string(Player->cSpecialShoot->RemainingCooldown / 60.0f));

}
