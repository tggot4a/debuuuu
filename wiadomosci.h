#pragma once
#ifndef komunikat_h
#define komunikat_h
#include <iostream>
#include <iomanip> //std::setw
#include <string>
#include <cmath>
#include <vector>
#include <algorithm> //remove_if
#include <sstream>
#include <list>



////////////////////////////////////////////////////////////////////////////Komunikat Naglowek


class komunikat { //klasa komunikatu przesy³anego
public:
	PCB *wskazniknadawcy;
	komunikat *wskazniknastepnejwiadomosci;

	std::string wiadomosc;

	std::string XR(PCB *pc); // read wiadomosc  
	void XS(PCB *pc, std::string odbiorca, std::string wiad); // send wiadomosc
};
#endif // !dysk
