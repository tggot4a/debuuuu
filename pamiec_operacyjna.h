#pragma once
#ifndef ram_h
#define ram_h
#include <iostream>
#include <iomanip> //std::setw
#include <string>
#include <cmath>
#include <vector>
#include <algorithm> //remove_if
#include <sstream>
#include <list>



////////////////////////////////////////////////////////////////////////////RAM Naglowek

//INSTRUKCJA

//pamiec RAM tworzymy konstruktorem bez argumentowym jest domyslnie ustawione 256 Bajtow       -  Pamiec_ram RAM;

//aby przydzielic pamiec uzywamy funkcji      RAM.przydziel_pamiec(rozmiar);

//aby zwolnic pamiec uzywamy funkcji          RAM.zwolnij_pamiec(rozmiar,index pocz¹tku bolku zwalnianego)

//aby uzyskaæ wskaŸnik do pamieci u¿ywamy       RAM.wskaznik_do_RAM();



// KLASA FSB potrzebna jest tylko Mi do sprawdzenia czy jest wolna pamiec itd

class FSB {							//klasa bloku FSB (Free Storage Block) {Z nich sk³ada sie lista FSB}
private:
	int Size;    //Rozmiar bloku wolnej pamiêci
	int Begin;    //Pocz¹tek bloku wolnej pamiêci (index)

public:
	FSB(int size, int begin);
	void set_Size(int size);
	void set_Begin(int begin);
	int  get_Size();
	int  get_Begin();
	void print();
};

class Pamiec_ram {
private:
	std::list<FSB> lista_FSB;     // lista blokow wolnej pamieci
	char pamiec_operacyjna[256];   //tablica - symulacja pamieci RAM
	void dodaj_blok_do_FSB(int rozmiar, int begin); //doddawanie do list FSB
	char* wskaznik_na_Pamiec_ram;

public:
	Pamiec_ram();
	char* wskaznik_na_RAM();
	void wyswietl_liste_FSB();
	int  przydziel_pamiec(int rozmiar, PCB* PCB);    // funkcja przydzielajaca pamiec
	void zwolnij_pamiec(int rozmiar, int poczatek, PCB* PCB);  // funkcja zwalaniajaca pamiec
	void print();
};
#endif // !dysk
