#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class input_system
{
	bool holding_click;
	sf::Vector2<int> starting_click_position;
	sf::RenderWindow* renderer;
	double zoom_factor;
	int method;

public:
	input_system(sf::RenderWindow*);
	void handle_move(sf::Vector2<double> &, sf::Vector2<double> &);
	void handle_zoom(sf::Vector2<double>&, sf::Vector2<double>&);
	void handle_precision(int &);
	int handle_method();
};

