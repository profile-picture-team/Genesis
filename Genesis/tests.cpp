#include "Headers/World.h"
//#include "Headers/SaveFile.h"
#include "Headers/Log.h"

#include <SFML/Graphics.hpp>
#include <thread>
#include <iomanip>
#include <iostream>
#include <stdint.h>
using namespace std;

uint8_t *CreatGen(uint8_t);

int main()
{
	cin.get();
	GeneralLog("***START_FUNCTION_MAIN***");
	
	World world(20, 20, 10, "World");
	
	World::Cell cell1;
	cell1.bot_f = 1;
	cell1.energy = 500;
	cell1.genome = CreatGen(3);
	
	
	world.setDisplayMode(1);
	
	world.resetCell(sf::Vector2<uint16_t>(10, 10), cell1);
	
	while (1) {
		cout << "Press to ENTER";
		cin.get(); 
		world.simulateStep(); 
	}
	
	cin.get();
	//string world_name = "world";
	//uint32_t width = 50, height = 70;
	//World *world = new World(width, height, 12, world_name);
	//cin.get();
	//delete world;
	
	GeneralLog("***STOP_FUNCTION_MAIN***");
	
	return 0;
}

uint8_t *CreatGen(uint8_t c) {
	uint8_t *gen = new uint8_t[256];
	for (int i = 0; i < 256; i++) gen[i] = c;
	return gen;
}
