#pragma once
#ifndef interpreter_h
#define interpreter_h
#include <iostream>
#include <iomanip> //std::setw
#include <string>
#include <cmath>
#include <vector>
#include <algorithm> //remove_if
#include <sstream>
#include <list>

#include "MetodaKaruzelowa.h"
#include "dysk.h"
#include "Interpreter.h"
#include "MetodaKaruzelowa.h"
#include "pamiec_operacyjna.h"
#include "semafor.h"
#include "wiadomosci.h" 
#include "ZarzadzanieProcesami.h"

////////////////////////////////////////////////////////////////////////////Iterpreter Naglowek

class Interpreter {

private:


	int rejestrA;
	int rejestrB;
	int rejestrC;

	int licznikRozkazow;

	bool ZF; //zero flag

	std::string nazwaProcesu;


	char obecnyRozkaz[3];
	char argA[3], argB[3]; //argumenty rozkazu

	Dysk dysk;

	Karuzela karuzela;
	Pamiec_ram RAM;

	Karuzela *wskaznik_na_karuzele = &karuzela;
	Pamiec_ram *wskaznik_na_pamiec_ram = &RAM;


	int sprawdzRozkaz(char rozkaz[2]);



	//2 argumenty

	void fr();//Odczyt z pliku	
	void df(); // delete file
	void fw();//Zapis do pliku, na jego ko�cu 
	void fm();//Tworzy plik o wskazanej nazwie 
	void mv(); //Kopiuje B do A
	void ad();//Sumuje A i B, wynik przechowany w A
	void sb();//Odejmuje A i B, wynik przechowany w A
	void mp();//Mno�y A i B, wynik przechowany w A
	void dv();//Dzieli A i B, wynik przechowany w A





			  //1 argument

	void pt();//Wypisane warto�ci na ekranie
	void j0();//Skok je�li wynik ostatniej operacji jest r�wny 0
	void jp();//Skok do wskazanego adresu

			  //0 argumentow
	void ex();//Wstrzymanie procesu
	void dl();//Usuwa proces


	void zerujZmienne();


	//shell

	void xs();//Wys�anie komunikatu do procesu o zadanej nazwie
	void xr();//odbiera komunikat wys�any przez xs

	void rp(std::string argumentA); // run process

	void go();	//rozkaz do przodu

	void wb(); // wyswietla wektor bitowy

	void sz(int index);// void, wyswietla sektor, nic nie zwraca, przyjmuje inta


	void wk(); // wyswietl katalog

	void fb(); //wyswietla liste fsb


public:
	void CreateProcesses();

	Interpreter() {
		rejestrA = rejestrB = rejestrC = licznikRozkazow = 0;
		nazwaProcesu.clear();

		argA[2] = '\0';
		argB[2] = '\0';
		obecnyRozkaz[2] = '\0';
		ZF = false;
	}

	void cp();//Tworzy proces o podanej nazwie, kt�ry wykonuje program przekazany w argumenice nr 2


	void zaladujProgramyNaDysk();
	void wyslijRejestry(); // wysy�a rejestry do pcb
	void pobierzRejestry();

	void wyswietlStany();
	void wyswietlWykonywanyRozkaz();
	void pobierzRozkaz(); // pobiera rozkaz z pamieci do aktualnyRozkaz i argumenty do argA i arg B


	void wykonajRozkaz();
	void komendaUzytkownika(); // rozkazy wpisywane z klawiatury?

	int stringToInt(std::string &str);//pomocne przy rozkazach



};
#endif // !dysk
