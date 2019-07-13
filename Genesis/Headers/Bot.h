
#ifndef _bot_h_
#define _bot_h_

#include <stdint.h>
#include <ctime>
#include <cstdlib>
#include <string>
using namespace std;

class World;

class Bot
{
  private:
	uint8_t genome[256];
	uint8_t i_mutation;
	uint8_t i_command;
	uint8_t direction;
	uint16_t coord[2];
	uint16_t minerals;

	Bot() {}

  public:
	Bot(uint8_t *genome, uint16_t x, uint8_t y, bool mutation)
	{
		uint8_t i = 255;
		do
		{
			i++;
			this->genome[i] = genome[i];
		} while (i < 255);
		i_mutation = 0;
		i_command = 0;
		direction = 0;
		coord[0] = x;
		coord[1] = y;
		minerals = 0;

		if (mutation)
		{
			srand(clock());
			this->genome[rand() % 256] = rand() % 256;
		}
	}

	uint8_t GiveCommand() { return genome[i_command++]; }
	bool CheckMutation()
	{
		++i_mutation %= 4;
		if (i_mutation == 0)
			return 1;
		return 0;
	}

	friend int SaveGenome(Bot *sgen, string name);
	friend int SaveWorld(World *sworld, string name);
	friend World *LoadWorld(string name);
	friend void WriteBot(Bot *b);
	
	friend class ListBot;
	friend class World;
	
	friend int main();
};
#endif
