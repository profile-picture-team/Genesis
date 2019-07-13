
#ifndef _savefile_h_
#define _savefile_h_

#include "Bot.h"
#include "World.h"
#include "ListBot.h"
#include "ConsoleCommand.h"
#include "Log.h"

#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

int SaveGenome(Bot *sgen, string name)
{
	/** 
	Функция сохраняет геном бота
	переданного по ссылке в файл %name%.gen,
	но не изменяет его на выходе
	!!!ВНИМАНИЕ!!!
	Если геном с таким именем уде существует,
	то он перезапишиися!
	**/
	string path = "Genomes/" + name + ".gen";
	GeneralLog("Save genome: " + path);
	system((mkdir_r + "Genomes").c_str());
	ofstream fout(path);

	if (!fout.is_open())
	{
		GeneralLog("Error: not save genome: " + path);
		return 1;
	}

	uint8_t *gen = sgen->genome;

	for (uint16_t i = 0; i < 256; i++)
		fout << *(gen + i);

	fout.close();

	return 0;
}

int SaveWorld(World *sworld, string name)
{
	/** 
	Функция сохраняет содержимое объекта 
	переданного по ссылке в папку %name%,
	но не изменяет его на выходе
	!!!ВНИМАНИЕ!!!
	Если мир с таким именем уде существует,
	то он перезапишиися!
	**/
	string path = "Worlds/" + name + "/" + name + ".struct";
	GeneralLog("Save world: " + path);
	system((mkdir_r + "Worlds/" + name + "/Bots").c_str());
	ofstream fout;

	fout.open(path);
	if (!fout.is_open())
	{
		GeneralLog("Error: Save world: " + path);
		return 1;
	}
	/*Вывод параметров мира*/
	fout.write((char *)&sworld->WidthField, 2);
	fout.write((char *)&sworld->HeightField, 2);
	fout.write((char *)&sworld->SideCell, 2);
	/*Вывод структуры поля мира*/
	uint32_t size = sworld->HeightField * sworld->WidthField;
	for (uint32_t i = 0; i < size; i += 4)
	{
		uint8_t outbyte = 0;
		for (uint32_t j = 0; (j < size) && (j < 4); j++)
			outbyte += (sworld->field[j + i] << (j * 2));
		fout.write((char *)&outbyte, 1);
	}
	/*Вывод структуры распределения енергии на поле*/
	for (uint32_t i = 0; i < size; i += 2)
	{
		uint8_t outbyte[3] = {0, 0, 0};

		outbyte[0] = (sworld->energy[i]);
		outbyte[1] = (sworld->energy[i] >> 8);
		if (i + 1 < size)
		{
			outbyte[1] += (sworld->energy[i + 1] << 4);
			outbyte[2] = (sworld->energy[i + 1] >> 4);
		}
		
		fout.write((char *)&outbyte[0], 1);
		fout.write((char *)&outbyte[1], 1);
		fout.write((char *)&outbyte[2], 1);
	}
	fout.close();

	/*Вывод списка ботов*/
	path = "Worlds/" + name + "/Bots/0.list";
	GeneralLog("Save world: " + path);
	fout.open(path);
	if (!fout.is_open())
	{
		GeneralLog("Error: Save world: " + path);
		return 2;
	}
	ListBot::Element *terminator = sworld->listBot->ActiveEll;
	ListBot::Element *curEll = terminator;
	do
	{
		Bot *sbot = curEll->data;

		fout << (uint8_t)(((sbot->direction % 8) << 2) + ((sbot->i_mutation % 4) << 5));
		fout.write((char *)&sbot->coord[0], 2);
		fout.write((char *)&sbot->coord[1], 2);
		fout.write((char *)&sbot->minerals, 2);
		fout.write((char *)&sbot->i_command, 1);
		for (int i = 0; i < 256; i++)
			fout << sbot->genome[i];

		curEll = curEll->next;
	} while (curEll != terminator);
	fout << (char)2;
	fout.close();

	return 0;
}

uint8_t *LoadGenome(string name)
{
	/**
	Функция загружает из файла %name%.gen геном
	и возращает его в виде указателя на первый элемент
	Если файл %name%.gen не существует возращает NULL
	!!!ВНИМАНИЕ!!!
	Функция не чистит выделенную память под геном!
	Возможна утечка памяти!
	**/
	ifstream fin;
	uint8_t *gen = new uint8_t[256];
	fin.open("Genomes/" + name + ".gen");
	if (!fin.is_open())
		return NULL;
	for (int i = 0; i < 256; i++)
		gen[i] = fin.get();
	return gen;
}

World *LoadWorld(string name)
{
	/**
	Функция загружает мир из директории %name%
	и возращает его в виде указателя.
	!!!ВНИМАНИЕ!!!
	Функция не чистит выделенную память под мир!
	Возможна утечка памяти!
	**/
	string path = "Worlds/" + name + "/" + name + ".struct"; 
	GeneralLog("Load world: " + path);
	ifstream fin(path);
	if (!fin.is_open())
	{
		GeneralLog("Error: Open world struct: " + path);
		return NULL;
	}
	World *world = NULL;

	uint16_t width, height, side, *energy;
	uint8_t *field;
	ListBot *llistBot;
	
	width = (uint16_t)(fin.get() + (fin.get() << 8));
	height = (uint16_t)(fin.get() + (fin.get() << 8));
	side = (uint16_t)(fin.get() + (fin.get() << 8));
	
	uint32_t size_ = width * height;
	field = new uint8_t[size_];
	energy = new uint16_t[size_ + size_ % 2];
	
	//загрузка поля
	for (uint32_t i = 0; i < size_; i += 4)
	{
		uint8_t inbyte = fin.get();
		for (uint32_t j = 0; (j + i < size_) && (j < 4); j++)
			field[j + i] = ((inbyte >> (j * 2)) % 4);
	}
	
	//загрузка карты распределения энергии
	for (uint32_t i = 0; i < size_; i += 2)
	{
		uint8_t inbyte[3] = {0, 0, 0};
		for (int j = 0; j < 3; j++)
			inbyte[j] = fin.get();
		energy[i] = inbyte[0] + ((inbyte[1] % 16) << 8);
		energy[i + 1] = (inbyte[1] >> 4) + (inbyte[2] << 4);
	}
	fin.close();
	
	
	//загрузка списка ботов
	path = "Worlds/" + name + "/Bots/0.list";
	fin.open(path);
	if (!fin.is_open())
	{
		GeneralLog("Error: Open world bots: " + path);
		return NULL;
	}
	auto LoadBot = [](uint8_t firstbyte, ifstream &fin) -> Bot * {
		Bot *bot = new Bot();
		bot->direction = ((firstbyte >> 2) % 8);
		bot->i_mutation = ((firstbyte >> 5) % 4);
		bot->coord[0] = fin.get() + (fin.get() << 8);
		bot->coord[1] = fin.get() + (fin.get() << 8);
		bot->minerals = fin.get() + (fin.get() << 8);
		bot->i_command = fin.get();
		for (int j = 0; j < 256; j++)
			bot->genome[j] = fin.get();
		return bot;
	};
	uint8_t firstbyte = fin.get();
	if ((firstbyte % 4) == 0)
	{
		llistBot = new ListBot(LoadBot(firstbyte, fin));
		firstbyte = fin.get();
	}
	while ((firstbyte % 4) == 0)
	{
		llistBot->NewBot(LoadBot(firstbyte, fin));
		firstbyte = fin.get();
	}
	fin.close();
	world = new World(width, height, side, field, energy, llistBot, name);

	return world;
}

#endif
