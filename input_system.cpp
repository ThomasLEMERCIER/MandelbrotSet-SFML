#include "input_system.h"

input_system::input_system(sf::RenderWindow* renderer)
{
	this->holding_click = false;
	this->starting_click_position = { 0, 0 };
	this->renderer = renderer;
	this->zoom_factor = 0.2;
	this->method = 0;
}

void input_system::handle_move(sf::Vector2<double> &frac_tl, sf::Vector2<double> &frac_br)
{
	if (this->holding_click)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			sf::Vector2<int> offset = this->starting_click_position - sf::Mouse::getPosition(*this->renderer);
			sf::Vector2u size = this->renderer->getSize();
			double x_ratio = (frac_br.x - frac_tl.x) / (double)size.x;
			double y_ratio = (frac_br.y - frac_tl.y) / (double)size.y;
			sf::Vector2<double> translation = { offset.x * x_ratio, offset.y * y_ratio };
			frac_tl += translation;
			frac_br += translation;
			this->starting_click_position = sf::Mouse::getPosition(*this->renderer);
		}
		else
		{
			this->holding_click = false;
		}
	}
	else
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			this->holding_click = true;
			this->starting_click_position = sf::Mouse::getPosition(*this->renderer);
		}
	}
}

void input_system::handle_zoom(sf::Vector2<double>& frac_tl, sf::Vector2<double>& frac_br)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->renderer);
		sf::Vector2u size = this->renderer->getSize();

		double x_ratio = (frac_br.x - frac_tl.x) / (double)size.x;
		double y_ratio = (frac_br.y - frac_tl.y) / (double)size.y;

		sf::Vector2<double> zoom_point = { mouse_pos.x * x_ratio + frac_tl.x, mouse_pos.y * y_ratio + frac_tl.y};


		sf::Vector2<double> translation_tl = 0.5*(zoom_point - frac_tl);
		sf::Vector2<double> translation_br = 0.5*(zoom_point - frac_br);

		
		frac_tl += translation_tl*this->zoom_factor;
		frac_br += translation_br*this->zoom_factor;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->renderer);

		sf::Vector2<double> translation = 0.5 * (frac_br - frac_tl);
		frac_tl -= translation * this->zoom_factor;
		frac_br += translation * this->zoom_factor;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		frac_tl = { -2., 1. };
		frac_br = { 1., -1. };
	}
}

void input_system::handle_precision(int& iterations)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		iterations += 16;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && iterations > 16)
	{
		iterations -= 16;
	}
}

int input_system::handle_method()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0))
	{
		this->method = 0;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
	{
		this->method = 1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
	{
		this->method = 2;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3))
	{
		this->method = 3;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4))
	{
		this->method = 4;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5))
	{
		this->method = 5;
	}
	return this->method;
}
