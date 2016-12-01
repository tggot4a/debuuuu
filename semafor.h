#pragma once
#ifndef semafor_h
#define semafor_h
#include <iostream>
#include <iomanip> //std::setw
#include <string>
#include <cmath>
#include <vector>
#include <algorithm> //remove_if
#include <sstream>
#include <list>
#include "ZarzadzanieProcesami.h"
////////////////////////////////////////////////////////////////////////////Semafory Naglowek

class SemaforPCB {
private:
	int WartoscSemafora;
	list<PCB*> kolejka;
public:
	SemaforPCB(int wartoscpoczatkowasemafora);
	void operacjaV();
	void operacjaP(PCB *proces);
	int PobierzWartoscSemafora();
	void WyswietlKolejke();
};
#endif // !dysk
