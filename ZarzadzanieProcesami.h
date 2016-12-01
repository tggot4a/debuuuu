#pragma once
#ifndef zarzo_h
#define zarzo_h
#include <iostream>
#include <iomanip> //std::setw
#include <string>
#include <cmath>
#include <vector>
#include <algorithm> //remove_if
#include <sstream>
#include <list>




////////////////////////////////////////////////////////////////////////////Zarzadzanie Procesami Naglowek

class komunikat;
class SemaforPCB;
class Karuzela;
class Pamiec_ram;

using namespace std;
struct PCB {
	char nazwa[2];
	PCB *nastepny_blok_PCB_tej_grupy, *poprzedni_blok_PCB_tej_grupy, *nastepny_z_wszytkich_blokow_PCB, *poprzedni_z_wszytkich_blokow_PCB, *blok_PCB_nastêpnego_czekajacego_pod_semaforem;
	bool zatrzymany, zablokowany;
	komunikat* pierwszy_komunikat;
	SemaforPCB * wspolny_semafor_komunikatow;
	SemaforPCB * semafor_komunikatow_dla_odbiorcy;
	int wielkosc_pamieci_wlasnej, adres_pamieci_wlasnej;
	int przechowywany_rejestr_A;
	int przechowywany_rejestr_B;
	int przechowywany_rejestr_C;
	int przechowywany_licznik_Rozkazow;
};
class Proces
{
public:
	Proces(char nazwa[2]);//Nale¿y korzystaæ z konstruktora tylko dla procesu mielacego "*G" i procesow nadzorczych grup "*S" dla pozostalych nalezy skorzystac z utworz_proces()
	PCB * pobierz_PCB();//Zwraca wskaznik na PCB Procesu
	void utworz_proces(char nazwa[2], int rozmiar_programu, string lista_rozkazow);//tworzy proces w tej samej grupie
	void usun_proces(char nazwa[2]);//w przypadku wywolania tej funkcji automatycznie wywolywana jest funkcja zatrzymywanie_procesu
	void uruchomienie_procesu(char nazwa[2]);
	void zatrzymywanie_procesu(char nazwa[2]);
	PCB * znalezienie_bloku_PCB_o_podanej_nazwie(char nazwa[2]);//szuka procesu w grupie procesów procesu wywolujacego metode
	void zatrzymanie_zlecenia_i_powiadomienie_programu_nadzorczego();//
	void zakonczenie_nieprawidlowe(string tresc);
private:
	PCB blokPCB;
	void doloczenie_bloku_PCB_do_lacuchow(PCB* nowyblokPCB);
	void usuniecie_bloku_PCB_z_lacuchow(PCB* nowyblokPCB);
};
PCB * wskaznik_na_liste_wszytkich_blokow_PCB = nullptr;
vector<Proces> listaProcesow;
vector<Proces*> lista_wskaznikow_na_procesy_nadzorcze;
PCB * wykonywany;
Pamiec_ram * wskaznik_na_Pamiec_ram_dla_Procesow;
#endif // !dysk
