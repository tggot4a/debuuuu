#pragma once
#ifndef DYSK_H
#define DYSK_H

#include <iostream>
#include <iomanip> //std::setw
#include <string>
#include <cmath>
#include <vector>
#include <algorithm> //remove_if

/*
Instrukcja:
Dysk dysk;	// tworzymy dysk
string nazwaPliku;	// nazwa pliku z programu Aleksandra
string zawartoscPliku;	// zawartosc pliku z programu Aleksandra
dysk.utworzNowyPustyPlik(nazwaPliku);	// tworzy PUSTY plik o podanej nazwie
dysk.wpiszDaneDoPustegoPliku(nazwaPliku, zawartoscPliku);	// zapisuje zawartoscPliku do pliku o nazwie nazwaPliku
dysk.zawartoscPliku(nazwaPliku);	// zwaraca zawartosc pliku jako stringa
lub
dysk.wyswietlZawartoscPliku(nazwaPliku);	// wyswietla zawartosc, nic nie zwraca, void
dysk.usunPlik(nazwaPliku);	// usuwa plik o podanej nazwie
dysk.wyswietlKatalog();	// void, wyswietla katalog, nic nie zwraca, nic nie przyjmuje, wymysl jakas komende bezargumentowa na to
dysk.wyswietlWektorBitowy();	// void, wyswietla wektor bitowy, nic nie zwraca, nic nie przyjmuje, wymysl jakas komende bezargumentowa na to
dysk.wyswietlSektorZnakowo(index); // void, wyswietla sektor, nic nie zwraca, przyjmuje inta - index sektora, wymysl jakas komende jednoargumentowa na to
PS
Prosze sie nie przejmowac warningami ze strony dysk.cpp, visual cos tam swiruje i sugeruje uzycie bezpiecznej wersji funkcji,
cos takiego jak bylo na podstawach programowania z printf a printf_s
*/

class Dysk {
private:

	class Parametry {
	private:
		std::string nazwa;
		int	rozmiar;
		int	indexPierwszegoSektora;
		int	indexOstatniegoSektora;

	public:
		Parametry(std::string nazwaPliku,
			char rozmiar,
			int indexPierwszegoSektora,
			int indexOstatniegoSektora);

		void wyswietlParametryPliku();

		std::string pobierzNazwe();

		int pobierzRozmiar();

		int pobierzIndexPierwszegoSektora();

		int pobierzIndexOstatniegoSektora();

		void wpiszIndexOstatniegoSektora(int indexOstatniegoSektora);

		void wpiszRozmiar(int rozmiar);
	};

	class Sektor {
	private:
		unsigned char dane[14];
		char indexNastepnegoSektora[2];

	public:
		Sektor();

		void wyczyscSektor();

		void wyswietlSektorZnakowo();

		void wyswietlSektorLiczbowo();

		void wyswietlIndexNastepnego();

		bool czySektorJestWolny();

		void wpiszDaneDoSektora(std::string danePliku);

		void wpiszIndexKolejnegoSektora(int idx);

		int pobierzIndexNastepnego();

		std::string zawartoscSektora();
	};

	// 0 - sektor wolny
	// 1 - sektor zajety

	Sektor sektory[64];
	bool wektorBitowy[64];
	std::vector<Parametry*> katalogPlikow;

	void utworzPustyPlik(std::string nazwaPliku);

	bool czyNazwaPlikuJestPoprawna(std::string &nazwaPliku);

	bool czyNazwaPlikuJestZajeta(std::string &nazwaPliku);

	std::string ustalZawartoscPliku();

	int ileSektorowJestPotrzebnych(std::string zawartosc);

	int ileSektorowJestWolnych();

	std::vector<int> indexyWymaganychWolnychSektorow(int wymaganaIloscSektorow);

	std::vector<std::string> podzielZawartosc(std::string zawartosc, int iloscPotrzebnychSektorow);

	void zainicjalizujWektorBitowy();

	int indexSzukanegoPliku(std::string nazwaPliku);

	void wyczyscKatalog();

	Sektor podajSektor(int idx);

public:
	Dysk();

	~Dysk();

	void utworzNowyPustyPlik(std::string nazwaPliku);

	void wpiszDaneDoPustegoPliku(std::string nazwaPliku, std::string zawartoscPliku);

	std::string zawartoscPliku(std::string nazwaPliku);

	void wyswietlZawartoscPliku(std::string nazwaPliku);

	void usunPlik(std::string nazwaPliku);

	void wyswietlKatalog();

	void wyswietlWektorBitowy();

	void wyswietlSektorZnakowo(int idx);
};

#endif