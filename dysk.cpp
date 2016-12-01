#include <iostream>
#include <iomanip> //std::setw
#include <string>
#include <cmath>
#include <vector>
#include <algorithm> //remove_if
#include "dysk.h"

Dysk::Parametry::Parametry(std::string nazwaPliku,
	char rozmiar,
	int indexPierwszegoSektora,
	int indexOstatniegoSektora)
	:nazwa(nazwaPliku),
	rozmiar(rozmiar),
	indexPierwszegoSektora(indexPierwszegoSektora),
	indexOstatniegoSektora(indexOstatniegoSektora) {
}

void Dysk::Parametry::wyswietlParametryPliku() {
	std::cout << "Nazwa: " << nazwa
		<< "  Rozmiar: " << rozmiar << " sektorow"
		<< "  Index pierwszgo: " << indexPierwszegoSektora
		<< "  Index ostatniego: " << indexOstatniegoSektora
		<< std::endl;
}

std::string Dysk::Parametry::pobierzNazwe() {
	return nazwa;
}

int Dysk::Parametry::pobierzRozmiar() {
	return rozmiar;
}

int Dysk::Parametry::pobierzIndexPierwszegoSektora() {
	return indexPierwszegoSektora;
}

int Dysk::Parametry::pobierzIndexOstatniegoSektora() {
	return indexOstatniegoSektora;
}

void Dysk::Parametry::wpiszIndexOstatniegoSektora(int indexOstatniegoSektora) {
	this->indexOstatniegoSektora = indexOstatniegoSektora;
}

void Dysk::Parametry::wpiszRozmiar(int rozmiar) {
	this->rozmiar = rozmiar;
}

Dysk::Sektor::Sektor() {
	wyczyscSektor();
}

void Dysk::Sektor::wyczyscSektor() {
	for (int i = 0; i < 14; i++) {
		dane[i] = 0;
	}
	std::string s = std::to_string(-1);
	strncpy(indexNastepnegoSektora, s.c_str(), sizeof(indexNastepnegoSektora));
}

void Dysk::Sektor::wyswietlSektorZnakowo() {
	for (int i = 0; i < 14; i++) {
		if (dane[i] != 0) {
			std::cout << dane[i];
		}
	}

	std::cout << " | ";

	for (int i = 0; i < 2; i++) {
		if (indexNastepnegoSektora[i] != 0) {
			std::cout << indexNastepnegoSektora[i];
		}
	}
	std::cout << std::endl;
}

void Dysk::Sektor::wyswietlSektorLiczbowo() {
	for (int i = 0; i < 14; i++) {
		if (dane[i] != 0) {
			std::cout << std::setw(3) << (int)dane[i] << " ";
		}
	}
	std::cout << " | ";

	for (int i = 0; i < 2; i++) {
		if (indexNastepnegoSektora[i] != 0) {
			std::cout << std::setw(3) << (int)indexNastepnegoSektora[i];
		}
	}
	std::cout << std::endl;
}

void Dysk::Sektor::wyswietlIndexNastepnego() {
	for (int i = 0; i < 2; i++) {
		if (indexNastepnegoSektora[i] != 0) {
			std::cout << indexNastepnegoSektora[i];
		}
	}
	std::cout << std::endl;
}

bool Dysk::Sektor::czySektorJestWolny() {
	int licznikWolnychBajtow = 0;

	for (int i = 0; i < 14; i++) {
		if (dane[i] == 0) {
			licznikWolnychBajtow++;
		}
	}

	if (licznikWolnychBajtow == 14) {
		return true;
	}
	else {
		return false;
	}
}

void Dysk::Sektor::wpiszDaneDoSektora(std::string danePliku) {
	for (int i = 0; i < danePliku.length(); i++) {
		dane[i] = danePliku[i];
	}
}

void Dysk::Sektor::wpiszIndexKolejnegoSektora(int idx) {
	std::string s = std::to_string(idx);
	strncpy(indexNastepnegoSektora, s.c_str(), sizeof(indexNastepnegoSektora));
}

int Dysk::Sektor::pobierzIndexNastepnego() {
	std::string idx(indexNastepnegoSektora);
	int index = std::stoi(idx);
	return index;
}

std::string Dysk::Sektor::zawartoscSektora() {
	std::string zawartosc;

	for (int i = 0; i < 14; i++) {
		if (dane[i] != 0) {
			zawartosc += dane[i];
		}
	}

	return zawartosc;
}

void Dysk::utworzPustyPlik(std::string nazwaPliku) {
	bool czyNazwaJestPoprawna = czyNazwaPlikuJestPoprawna(nazwaPliku);
	bool czyNazwaJestZajeta = czyNazwaPlikuJestZajeta(nazwaPliku);

	if (czyNazwaJestPoprawna && !czyNazwaJestZajeta) {
		std::vector<int> indexyWolnychSektorow = indexyWymaganychWolnychSektorow(1);

		Parametry* p = new Parametry(nazwaPliku, 1, indexyWolnychSektorow[0], indexyWolnychSektorow[0]);

		katalogPlikow.push_back(p);
	}
	else if (!czyNazwaJestPoprawna) {
		std::cout << "Podana nazwa pliku jest niepoprawna. Nazwa nie moze miec wiecej niz dwa znaki" << std::endl;
	}
	else if (czyNazwaJestZajeta) {
		std::cout << "Plik o podanej nazwie juz istnieje." << std::endl;
	}
	else {
		std::cout << "Niepoprawna nazwa pliku." << std::endl;
	}
}

bool Dysk::czyNazwaPlikuJestZajeta(std::string &nazwaPliku) {
	bool czyPodanaNazwaJestZajeta = false;

	if (katalogPlikow.size()) {
		for (int i = 0; i < katalogPlikow.size(); i++) {
			if (katalogPlikow[i]->pobierzNazwe() == nazwaPliku) {
				czyPodanaNazwaJestZajeta = true;
				break;
			}
			else {
				czyPodanaNazwaJestZajeta = false;
			}
		}
	}

	return czyPodanaNazwaJestZajeta;
}

bool Dysk::czyNazwaPlikuJestPoprawna(std::string &nazwaPliku) {
	int length = nazwaPliku.length();

	if (length <= 0 || length > 2) {
		return false;
	}
	else {
		return true;
	}
}

std::string Dysk::ustalZawartoscPliku() {
	std::string zawartosc;
	std::cout << "Podaj zawartosc pliku: ";
	std::cin.ignore();
	std::getline(std::cin, zawartosc);
	return zawartosc;
}

int Dysk::ileSektorowJestPotrzebnych(std::string zawartosc) {
	int ileBajtow = zawartosc.length();
	return ceil((float)ileBajtow / 14);
}

int Dysk::ileSektorowJestWolnych() {
	int iloscWolnychSektorow = 0;

	for (int i = 0; i < 64; i++) {
		if (wektorBitowy[i] == 0) {
			iloscWolnychSektorow++;
		}
	}
	return iloscWolnychSektorow;
}

std::vector<int> Dysk::indexyWymaganychWolnychSektorow(int wymaganaIloscSektorow) {
	std::vector<int> indexyWolnychSektorow;
	int licznik = 0;

	for (int i = 0; i < 64; i++) {
		if (licznik == wymaganaIloscSektorow) {
			break;
		}

		if (wektorBitowy[i] == 0) {
			indexyWolnychSektorow.push_back(i);
			wektorBitowy[i] = 1;
			licznik++;
		}
	}
	return indexyWolnychSektorow;
}

std::vector<std::string> Dysk::podzielZawartosc(std::string zawartosc, int iloscPotrzebnychSektorow) {
	std::vector<std::string> podzielonaZawartosc;

	std::string bufer;
	int k = 0;
	int n = 14;
	for (int i = 0; i < iloscPotrzebnychSektorow; i++) {
		bufer = zawartosc.substr(k, n);
		podzielonaZawartosc.push_back(bufer);
		k += 14;
	}

	return podzielonaZawartosc;
}

void Dysk::zainicjalizujWektorBitowy() {
	for (int i = 0; i < 64; i++) {
		wektorBitowy[i] = 0;
	}
}

int Dysk::indexSzukanegoPliku(std::string nazwaPliku) {
	bool czyPodanyPlikIstnieje = false;
	int index;

	for (int i = 0; i < katalogPlikow.size(); i++) {
		if (katalogPlikow[i]->pobierzNazwe() == nazwaPliku) {
			czyPodanyPlikIstnieje = true;
			index = i;
			break;
		}
	}

	if (czyPodanyPlikIstnieje) {
		return index;
	}
	else {
		return -1;
	}
}

void Dysk::wyczyscKatalog() {
	for (int i = 0; i < katalogPlikow.size(); i++) {
		delete katalogPlikow[i];
	}
	katalogPlikow.clear();
}

Dysk::Dysk() {
	zainicjalizujWektorBitowy();
}

Dysk::~Dysk() {
	wyczyscKatalog();
}

void Dysk::utworzNowyPustyPlik(std::string nazwaPliku) {
	int iloscWolnychSektorow = ileSektorowJestWolnych();

	if (iloscWolnychSektorow > 0) {
		utworzPustyPlik(nazwaPliku);
	}
	else {
		std::cout << "Nie ma wolnych sektorow na dysku." << std::endl;
	}
}

void Dysk::wpiszDaneDoPustegoPliku(std::string nazwaPliku, std::string zawartoscPliku) {
	int index = indexSzukanegoPliku(nazwaPliku);

	if (index != -1) {
		Parametry* p = katalogPlikow[index];

		int iloscPotrzebnychSektorow = ileSektorowJestPotrzebnych(zawartoscPliku);
		int iloscWolnychSektorow = ileSektorowJestWolnych();

		if (iloscPotrzebnychSektorow > iloscWolnychSektorow) {
			std::cout << "Za malo miejsca na dysku. Nie mozna zapisac pliku. Dostepne miejsce na dysku: "
				<< iloscWolnychSektorow * 14 << "bitow." << std::endl;
			return;
		}

		std::vector<int> indexyWolnychSektorow = indexyWymaganychWolnychSektorow(iloscPotrzebnychSektorow - 1);

		indexyWolnychSektorow.insert(indexyWolnychSektorow.begin(), p->pobierzIndexPierwszegoSektora());

		p->wpiszIndexOstatniegoSektora(indexyWolnychSektorow[iloscPotrzebnychSektorow - 1]);
		p->wpiszRozmiar(iloscPotrzebnychSektorow);

		std::vector<std::string> podzielonaZawartosc = podzielZawartosc(zawartoscPliku, iloscPotrzebnychSektorow);

		for (int i = 0; i < iloscPotrzebnychSektorow; i++) {
			sektory[indexyWolnychSektorow[i]].wpiszDaneDoSektora(podzielonaZawartosc[i]);

			if (i + 1 < iloscPotrzebnychSektorow) {
				sektory[indexyWolnychSektorow[i]].wpiszIndexKolejnegoSektora(indexyWolnychSektorow[i + 1]);
			}
		}
	}
	else {
		std::cout << "Plik o podanej nazwie nieistnieje." << std::endl;
	}
}

std::string Dysk::zawartoscPliku(std::string nazwaPliku) {
	int idx = indexSzukanegoPliku(nazwaPliku);

	if (idx != -1) {
		Parametry* p = katalogPlikow[idx];
		std::string zawartosc;

		int index = p->pobierzIndexPierwszegoSektora();

		while (index != -1) {
			zawartosc += sektory[index].zawartoscSektora();
			index = sektory[index].pobierzIndexNastepnego();
		}
		std::cout << std::endl;

		return zawartosc;
	}
	else {
		return "ERROR: Plik o podanej nazwie nie istnieje.";
	}
}

void Dysk::wyswietlZawartoscPliku(std::string nazwaPliku) {
	int idx = indexSzukanegoPliku(nazwaPliku);

	if (idx != -1) {
		Parametry* p = katalogPlikow[idx];
		std::string zawartosc;

		int index = p->pobierzIndexPierwszegoSektora();

		while (index != -1) {
			zawartosc += sektory[index].zawartoscSektora();
			index = sektory[index].pobierzIndexNastepnego();
		}
		std::cout << zawartosc << std::endl;
	}
	else {
		std::cout << "Plik o podanej nazwie nie istnieje." << std::endl;
	}
}

void Dysk::usunPlik(std::string nazwaPliku) {
	int idx = indexSzukanegoPliku(nazwaPliku);

	if (idx != -1) {
		Parametry* p = katalogPlikow[idx];
		int index = p->pobierzIndexPierwszegoSektora();
		int indexNastepnego = sektory[index].pobierzIndexNastepnego();
		wektorBitowy[index] = 0;

		while (indexNastepnego != -1) {
			indexNastepnego = sektory[index].pobierzIndexNastepnego();
			sektory[index].wyczyscSektor();
			wektorBitowy[index] = 0;
			index = indexNastepnego;
		}

		delete p;
		katalogPlikow.erase(katalogPlikow.begin() + idx);
	}
	else {
		std::cout << "Plik o podanej nazwie nie istnieje." << std::endl;
	}
}

Dysk::Sektor Dysk::podajSektor(int idx) {
	return sektory[idx];
}

void Dysk::wyswietlKatalog() {
	if (katalogPlikow.size() == 0) {
		std::cout << "Katalog plikow jest pusty." << std::endl;
	}
	else {
		int index = 0;
		std::cout << "=============================================" << std::endl;
		std::cout << "	Katalog plikow" << std::endl;
		std::cout << "=============================================" << std::endl;
		std::cout << "Index:	Parametry:" << std::endl;
		for (auto const &p : katalogPlikow) {
			std::cout << index++ << "	";
			p->wyswietlParametryPliku();
		}
	}
}

void Dysk::wyswietlWektorBitowy() {
	std::cout << "=======================================================" << std::endl;
	std::cout << "		Wektor bitowy" << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << "Index: ";
	for (int i = 0; i < 16; i++) {
		std::cout << std::setw(2) << i << " ";
	}
	std::cout << std::endl;
	std::cout << "Dane:  ";
	for (int i = 0; i < 16; i++) {
		std::cout << std::setw(2) << wektorBitowy[i] << " ";
	}
	std::cout << std::endl << "-------------------------------------------------------" << std::endl;
	std::cout << "Index: ";
	for (int i = 16; i < 32; i++) {
		std::cout << std::setw(2) << i << " ";
	}
	std::cout << std::endl;
	std::cout << "Dane:  ";
	for (int i = 16; i < 32; i++) {
		std::cout << std::setw(2) << wektorBitowy[i] << " ";
	}
	std::cout << std::endl << "-------------------------------------------------------" << std::endl;
	std::cout << "Index: ";
	for (int i = 32; i < 48; i++) {
		std::cout << std::setw(2) << i << " ";
	}
	std::cout << std::endl;
	std::cout << "Dane:  ";
	for (int i = 32; i < 48; i++) {
		std::cout << std::setw(2) << wektorBitowy[i] << " ";
	}
	std::cout << std::endl << "-------------------------------------------------------" << std::endl;
	std::cout << "Index: ";
	for (int i = 48; i < 64; i++) {
		std::cout << std::setw(2) << i << " ";
	}
	std::cout << std::endl;
	std::cout << "Dane:  ";
	for (int i = 48; i < 64; i++) {
		std::cout << std::setw(2) << wektorBitowy[i] << " ";
	}
	std::cout << std::endl << "=======================================================" << std::endl;
	std::cout << "	//0 - sektor wolny, 1 - sektor zajety" << std::endl;
	std::cout << "=======================================================" << std::endl;
}

void Dysk::wyswietlSektorZnakowo(int index) {
	podajSektor(index).wyswietlSektorZnakowo();
}