#pragma once
#ifndef karuzela_h
#define karuzela_h
#include <iostream>
#include <iomanip> //std::setw
#include <string>
#include <cmath>
#include <vector>
#include <algorithm> //remove_if
#include <sstream>
#include <list>


#include "ZarzadzanieProcesami.h"
////////////////////////////////////////////////////////////////////////////Karuzela Naglowek

class Karuzela {

private:
	PCB* Running, *NextTry;
	int IleDoKonca;

	void run(PCB* NextTry);

public:

	int MinionyKwant;
	bool NextTryModified;

	Karuzela();

	void find();

	void ChangeNEXTTRY(PCB* Nowy);

	PCB* GetRunning();

	int IleRozkazow();

};
#endif // !dysk
