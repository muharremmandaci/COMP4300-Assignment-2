#pragma once
#include <SFML/Graphics.hpp>

class CTransform {
public:
	sf::Vector2f Position = {0.0f, 0.0f };
	sf::Vector2f Velocity = {0.0f, 0.0f };
	float Angle = 0.0f;

	CTransform(const sf::Vector2f & position, const sf::Vector2f & velocity, const float & angle)
		: Position(position), Velocity(velocity), Angle(angle) {}
};

class CShape {
public:
	sf::CircleShape Shape;

	CShape(const float  radius, const size_t pointCount, const sf::Color& fillColor, const sf::Color& outlineColor, const float thickness) {
		Shape = sf::CircleShape(radius, pointCount);
		Shape.setFillColor(fillColor);
		Shape.setOutlineColor(outlineColor);
		Shape.setOutlineThickness(thickness);
		Shape.setOrigin(radius, radius);
	}
};

class CScore {
public:
	int Score = 0;

	CScore(const int score)
		:Score(score) {}
};

class CCollision {
public:
	float Radius = 0;

	CCollision(const float radius)
		:Radius(radius) {}
};

class CLifeSpan {
public:
	int TotalLife;
	int RemainingLife;

	CLifeSpan(const int life)
		:TotalLife(life), RemainingLife(life) {}
};

class CSpecialShoot {
public:
	int BulletAmount;
	int Cooldown;
	int RemainingCooldown;
	CSpecialShoot(int bulletAmount, int cooldown)
		:BulletAmount(bulletAmount), Cooldown(cooldown), RemainingCooldown(0){}
};

class CInput {
public:
	bool Up;
	bool Down;
	bool Left;
	bool Right;
	bool Shoot;
	bool SpecialShoot;
	sf::Vector2f MousePos;

	CInput()
		:Up(false), Down(false) , Left(false) , Right(false), Shoot(false), SpecialShoot(false){
		MousePos = sf::Vector2f(0, 0);
	}
};