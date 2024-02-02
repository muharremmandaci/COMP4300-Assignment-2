// Assignment2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdio.h>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/System/Vector2.hpp>
#include "Game.h"

int main()
{
	Game* game = new Game("config.txt");

	game->Run();

	delete game;
	return 0;
}

