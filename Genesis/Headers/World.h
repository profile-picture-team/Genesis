
#ifndef _world_h_
#define _world_h_

#include "Log.h"

#include <cstdint>
#include <string>
#include <iostream>
#include <queue>
//#include <condition_variable>
//#include <mutex>
//#include <thread>
#include <SFML/Graphics.hpp>

class World
{
  public:
	
	struct Cell {
		uint8_t		bot_f		= 0;
		uint8_t		mineral_f	= 0;
		
		uint16_t	energy		= 500;
		uint16_t	mineral		= 0;
		uint8_t		*genome		= 0;
		uint8_t		incommand	= 0;
		uint8_t		turn		= 0;
		
		uint8_t		assimilate	= 0;
		uint8_t		predator	= 0;
		uint8_t		synthesis	= 0;
	};
	
  private:
	
	const uint16_t	limitEnergy			= 1000;
	const uint16_t	limitMineral		= 1000;
	const sf::Color ASSIMILATE			= sf::Color(0, 255, 0, 0);
	const sf::Color PREDATOR			= sf::Color(255, 0, 0, 0);
	const sf::Color SYNTHESIS			= sf::Color(0, 0, 255, 0);
	const sf::Color MIN_ENERGY			= sf::Color::Yellow;
	const sf::Color MAX_ENERGY			= sf::Color::Red;
	const sf::Color ColorThickness		= sf::Color(150, 150, 150);
	
	const uint8_t	COMMAND_ASSIMILATE	= 0;
	const uint8_t	COMMAND_PREDATOR	= 0;
	const uint8_t	COMMAND_SYNTHESIS	= 0;
	
	uint16_t							SideCell;
	uint16_t							WidthField, HeightField;
	uint8_t								DisplayMode;
	Cell								*field;
	sf::RenderWindow					window;
	std::string							name;
	std::queue<sf::Vector2<uint16_t>>	QueueOfActive;
	
	inline Cell &Field(uint16_t x, uint16_t y) { return field[x + y * WidthField]; }
	inline void DeleteBot(Cell &cell) { 
		delete cell.genome; cell.genome = 0;
		cell.bot_f = 0; cell.mineral_f	= 1;
		cell.energy		= cell.incommand	= cell.turn			=
		cell.assimilate	= cell.predator		= cell.synthesis	= 0;
	}
	inline void UpdateCell(uint16_t x, uint16_t y) {
		Cell &activeCell = Field(x, y);
		
		sf::RectangleShape delete_cell(sf::Vector2f(SideCell, SideCell));
		delete_cell.setOutlineThickness(-0.5);
		delete_cell.setOutlineColor(ColorThickness);
		delete_cell.setFillColor(sf::Color::White);
		delete_cell.setPosition(sf::Vector2f(x * SideCell + 0.5, y * SideCell + 0.5));	
		window.draw(delete_cell);
		
		if (DisplayMode == 0) {
			if (activeCell.bot_f)			{
				sf::RectangleShape cell(sf::Vector2f(SideCell, SideCell));
				sf::Color fill(0, 0, 0);
				cell.setOutlineThickness(-0.5);
				cell.setOutlineColor(ColorThickness);
				cell.setPosition(sf::Vector2f(x * SideCell + 0.5, y * SideCell + 0.5));
				
				fill += ASSIMILATE;
				fill.a = (float)activeCell.assimilate / (activeCell.assimilate + activeCell.predator + activeCell.synthesis) * 255;
				cell.setFillColor(fill);
				window.draw(cell);
				fill -= ASSIMILATE;
				
				fill += PREDATOR;
				fill.a = (float)activeCell.predator / (activeCell.assimilate + activeCell.predator + activeCell.synthesis) * 255;
				cell.setFillColor(fill);
				window.draw(cell);
				fill -= PREDATOR;
				
				fill += SYNTHESIS;
				fill.a = (float)activeCell.synthesis / (activeCell.assimilate + activeCell.predator + activeCell.synthesis) * 255;
				cell.setFillColor(fill);
				window.draw(cell);
				fill -= SYNTHESIS;
			}
			else if (activeCell.mineral_f)	{
				sf::RectangleShape cell(sf::Vector2f(SideCell * 0.8f, SideCell * 0.8f));
				cell.setOutlineThickness(-0.5);
				cell.setOutlineColor(sf::Color(100, 100, 100));
				cell.setFillColor(sf::Color::White);
				cell.setPosition(sf::Vector2f((x + 0.1f) * SideCell + 0.5, (y + 0.1f) * SideCell + 0.5));
			window.draw(cell);
			}
		}
		else if (DisplayMode == 1) {
			if (activeCell.bot_f) {
				sf::RectangleShape cell(sf::Vector2f(SideCell, SideCell));
				cell.setOutlineThickness(-0.5);
				cell.setOutlineColor(ColorThickness);
				cell.setFillColor(MIN_ENERGY);
				cell.setPosition(sf::Vector2f(x * SideCell + 0.5, y * SideCell + 0.5));
				window.draw(cell);
				
				sf::Color color(MAX_ENERGY);
				color.a = (float)activeCell.energy / limitEnergy * 255;
				cell.setFillColor(color);
				window.draw(cell);
			}
			else if (activeCell.mineral_f) {
				sf::RectangleShape cell(sf::Vector2f(SideCell, SideCell));
				sf::Color fill(153, 50, 204);
				fill.a = (float)activeCell.mineral / (limitEnergy / 2) * 255;
				
				cell.setOutlineThickness(-0.5);
				cell.setOutlineColor(ColorThickness);
				cell.setFillColor(fill);
				cell.setPosition(sf::Vector2f(x * SideCell + 0.5, y * SideCell + 0.5));
				window.draw(cell);
			}
		}
	}
	inline void DrawBoards() {
		sf::VertexArray lines(sf::Lines, 2);
		lines[0].color = lines[1].color = sf::Color::Black;
		
		lines[0].position = sf::Vector2f(1, 0);
		lines[1].position = sf::Vector2f(1, window.getSize().y - 1);
		window.draw(lines);
		
		lines[0].position = sf::Vector2f(window.getSize().x, window.getSize().y - 1);
		window.draw(lines);
		
		lines[1].position = sf::Vector2f(window.getSize().x, 0);
		window.draw(lines);
		
		lines[0].position = sf::Vector2f(1, 0);
		window.draw(lines);
	}
	
  public:
	World(uint16_t WidthField_cell, uint16_t HeightField_cell, uint16_t SideCell_px, std::string name) {
		WidthField = WidthField_cell;
		HeightField = HeightField_cell;
		SideCell = SideCell_px;
		this->name = name;
		DisplayMode = 0;
		field = new Cell[WidthField * HeightField];
		
		window.create(sf::VideoMode(WidthField * SideCell + 1, HeightField * SideCell + 1), name, sf::Style::Titlebar);
		
		for (uint32_t x = 0; x < WidthField; x++)
			for (uint32_t y = 0; y < HeightField; y++)
				UpdateCell(x ,y);
		DrawBoards();
		
		window.display();
	}
	~World() {
		window.close();
	}
	
	void setDisplayMode(uint8_t mode) {
		DisplayMode = mode;
		for (uint32_t x = 0; x < WidthField; x++)
			for (uint32_t y = 0; y < HeightField; y++)
				UpdateCell(x, y);
		window.display();
	}
	bool resetCell(sf::Vector2<uint16_t> position, Cell &cell) { 
		if (position.x > WidthField || position.y > HeightField) return 0;
		Field(position.x, position.y) = cell;
		if (cell.bot_f)QueueOfActive.push(position);
		cell.assimilate = cell.predator = cell.synthesis = 0;
		for (int i = 0; i < 256; i++)
			if (cell.genome[i] == COMMAND_ASSIMILATE)		cell.assimilate++;
			else if (cell.genome[i] == COMMAND_PREDATOR)	cell.predator++;
			else if (cell.genome[i] == COMMAND_SYNTHESIS)	cell.synthesis++;
		UpdateCell(position.x, position.y);
		DrawBoards();
		window.display();
		return 1;
	}
	void simulateStep() {
		
		if (QueueOfActive.empty()) return;
		sf::Vector2<uint16_t> position = QueueOfActive.front();
		cout << position.x << " " << position.y << endl;
		cout << "test 195\n";	
		QueueOfActive.pop();
		while (!Field(position.x, position.y).bot_f && !QueueOfActive.empty()) { position = QueueOfActive.front(); QueueOfActive.pop(); }
		cout << "test 198\n";
		Cell &cell = Field(position.x, position.y);
		cout << "test 200\n";
		
		if (position.x > WidthField || position.y > HeightField) return;
		cout << "test 210\n";
		
		for (int i = 0; i < 1; i++) {
			cout << "test 213\n";
			uint8_t c;
			if (cell.genome) {
			cout << "Error 215\n";
			c = cell.genome[cell.incommand]; }
			cout << "test 217\n";
				 if (			c  <   8) { //move
				uint16_t x1 = position.x, y1 = position.y;
				uint16_t x2 = position.x, y2 = position.y;
				c = (c + 8 - cell.turn) % 8;
				
					 if (c == 1 || c == 2 || c == 3) {if (x2 < WidthField - 1)	x2++; else DeleteBot(cell);}
				else if (c == 5 || c == 6 || c == 7) {if (x2 > 0)				x2--; else DeleteBot(cell);}
				
					 if (c == 3 || c == 4 || c == 5) {if (y2 < HeightField - 1)	y2++; else DeleteBot(cell);}
				else if (c == 7 || c == 0 || c == 1) {if (y2 > 0)				y2--; else DeleteBot(cell);}
				
				if (cell.bot_f) {
					Cell a = Field(x2, y2);
					Cell b = Field(x1, y1);
					Field(x1, y1) = a;
					Field(x2, y2) = b;
					UpdateCell(x2, y2);
				}
				
				UpdateCell(x1, y1);
				DrawBoards();
				
				position = sf::Vector2<uint16_t>(x2, y2);
				
			} else cout << "Error 241\n";
			//else if (007 < c && c  < 016) { //rotate
				//cell.turn = (c + cell.turn) % 8;
			//}
			//else if (015 < c && c  < 024) {}
			//else if (023 < c && c  < 032) {}
			//else if (031 < c && c  < 040) {}
			//else if (			c == 040) {}
			//else if (			c == 041) {}
			//else if (			c == 042) {}
			//else if (042 < c && c  < 171) {}
		}
		
		if (cell.bot_f || 1) QueueOfActive.push(position);
		window.display();
	}
	
	//friend int SaveWorld(World *sworld, string name);
	friend int main();
};

#endif
