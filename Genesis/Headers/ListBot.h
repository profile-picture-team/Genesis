
#ifndef _listbot_h_
#define _listbot_h_

#include "Bot.h"

#include <stdint.h>
#include <string>
using namespace std;

class World;

class ListBot
{
  private:
	struct Element
	{
		Bot *data;
		Element *next;
	};

	Element *FirstEll;
	Element *LastEll;

	Element *ActiveEll;
	Element *PreviousEll;

	void NewBot(Bot *nbot)
	{
		Element *newEll = new Element;
		newEll->next = FirstEll;
		LastEll->next = newEll;
		LastEll = newEll;
		newEll->data = nbot;
	}

  public:
	ListBot(uint8_t *genome, uint16_t x, uint16_t y)
	{
		Element *newEll = new Element;
		FirstEll = newEll;
		LastEll = newEll;
		ActiveEll = newEll;
		PreviousEll = newEll;
		LastEll->next = newEll;

		newEll->data = new Bot(genome, x, y, 0);
	}

	ListBot(Bot *bot)
	{
		Element *newEll = new Element;
		FirstEll = newEll;
		LastEll = newEll;
		ActiveEll = newEll;
		PreviousEll = newEll;
		LastEll->next = newEll;

		newEll->data = bot;
	}

	~ListBot()
	{
		Element *nextEll = FirstEll->next;
		FirstEll->next = NULL;
		FirstEll = nextEll;
		nextEll = nextEll->next;
		while (FirstEll)
		{
			nextEll = FirstEll->next;
			delete FirstEll->data;
			delete FirstEll;
			FirstEll = nextEll;
		}
	}

	bool NewBot(uint16_t x, uint16_t y)
	{
		Element *newEll = NULL;
		newEll = new Element;
		newEll->next = FirstEll;
		LastEll->next = newEll;
		LastEll = newEll;
		newEll->data = new Bot(ActiveEll->data->genome, x, y, ActiveEll->data->CheckMutation());
		if (newEll == 0)
			return 0;
		return 1;
	}

	void DeleteBot()
	{
		PreviousEll->next = ActiveEll->next;
		delete ActiveEll->data;
		delete ActiveEll;
		ActiveEll = PreviousEll;
	}

	void NextBot()
	{
		PreviousEll = ActiveEll;
		ActiveEll = ActiveEll->next;
	}

	Bot *GiveBot() { return ActiveEll->data; }

	friend int SaveWorld(World *sworld, string name);
	friend World *LoadWorld(string name);
};
#endif