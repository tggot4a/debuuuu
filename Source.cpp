#include <iostream>
#include <iomanip> //std::setw
#include <string>
#include <cmath>
#include <vector>
#include <algorithm> //remove_if
#include <sstream>
#include <list>

#include "dysk.h"
#include "Interpreter.h"
#include "MetodaKaruzelowa.h"
#include "pamiec_operacyjna.h"
#include "semafor.h"
#include "wiadomosci.h" 
#include "ZarzadzanieProcesami.h"

////////////////////////////////////////////////////////////////////////////Zarzadzanie Procesami Definicje

Proces::Proces(char nazwa[2])
{
	blokPCB.nazwa[0] = nazwa[0];
	blokPCB.nazwa[1] = nazwa[1];
	if (nazwa[0] == '*'&& nazwa[1] == 'G')
		blokPCB.zatrzymany = 0;
	else
		blokPCB.zatrzymany = 1;
	blokPCB.zablokowany = 0;
	blokPCB.pierwszy_komunikat = nullptr;
	blokPCB.wspolny_semafor_komunikatow = new SemaforPCB(1);
	blokPCB.semafor_komunikatow_dla_odbiorcy = new SemaforPCB(1);
	blokPCB.nastepny_blok_PCB_tej_grupy = &blokPCB;
	blokPCB.poprzedni_blok_PCB_tej_grupy = &blokPCB;
	blokPCB.nastepny_z_wszytkich_blokow_PCB = &blokPCB;
	blokPCB.poprzedni_z_wszytkich_blokow_PCB = &blokPCB;
	if (nazwa[0] == '*'&& nazwa[1] == 'S')
	{
		lista_wskaznikow_na_procesy_nadzorcze.push_back(this);
	}
	if (wskaznik_na_liste_wszytkich_blokow_PCB == nullptr)
	{
		wskaznik_na_liste_wszytkich_blokow_PCB = &blokPCB;
	}
	else
		if (nazwa[0] == '*' && (nazwa[1] == 'S' || nazwa[1] == 'G'))//Przyjelem to że nazwa procesu nadzorczego to "*S" a procesu zawsze wykonywanego "*G"
		{
			PCB * Poprzedni = wskaznik_na_liste_wszytkich_blokow_PCB->poprzedni_z_wszytkich_blokow_PCB;
			Poprzedni->nastepny_z_wszytkich_blokow_PCB = &blokPCB;
			wskaznik_na_liste_wszytkich_blokow_PCB->poprzedni_z_wszytkich_blokow_PCB = &blokPCB;
			blokPCB.poprzedni_z_wszytkich_blokow_PCB = Poprzedni;
			blokPCB.nastepny_z_wszytkich_blokow_PCB = wskaznik_na_liste_wszytkich_blokow_PCB;
		}
	blokPCB.blok_PCB_następnego_czekajacego_pod_semaforem = nullptr;
	blokPCB.wielkosc_pamieci_wlasnej = 0;
	blokPCB.adres_pamieci_wlasnej = 0;
}
void Proces::utworz_proces(char nazwa[2], int rozmiar_programu, string lista_rozkazow)
{
	cout << " Proces "<<this->pobierz_PCB()->nazwa<<" tworzy proces "<<nazwa<< endl;
	if (znalezienie_bloku_PCB_o_podanej_nazwie(nazwa) == nullptr)
	{
		if (this->pobierz_PCB()->nazwa[0] != '*' && nazwa[0] == '*')
			zakonczenie_nieprawidlowe("Proces niesystemowy probuje zatrzymac proces systemowy o nazwie " + nazwa[0] + nazwa[1]);
		else
		{
			int k;
			listaProcesow.push_back(Proces::Proces(nazwa));
			doloczenie_bloku_PCB_do_lacuchow(listaProcesow.back().pobierz_PCB());
			listaProcesow.back().pobierz_PCB()->wielkosc_pamieci_wlasnej = rozmiar_programu;
			listaProcesow.back().pobierz_PCB()->adres_pamieci_wlasnej = wskaznik_na_Pamiec_ram_dla_Procesow->przydziel_pamiec(rozmiar_programu, listaProcesow.back().pobierz_PCB());
			for (int i = 0; i<lista_rozkazow.size(); i++)
			{
				k=i + listaProcesow.back().pobierz_PCB()->adres_pamieci_wlasnej;
				if ((lista_rozkazow[i] >= 'a' && lista_rozkazow[i] <= 'z') || (lista_rozkazow[i] >= 'A' && lista_rozkazow[i] <= 'a') || (lista_rozkazow[i] >= '0' && lista_rozkazow[i] <= '9') || lista_rozkazow[i] == '*')
					wskaznik_na_Pamiec_ram_dla_Procesow->wskaznik_na_RAM()[k] = lista_rozkazow[i];
			}
			cout << " Proces " << nazwa << " utworzono proces " <<endl;
		}
	}
	else
		zakonczenie_nieprawidlowe("W grupie procesow istnieje juz proces o nazwie " + nazwa[0] + nazwa[1]);
}
void Proces::usun_proces(char nazwa[2])
{
	PCB * usuwany = znalezienie_bloku_PCB_o_podanej_nazwie(nazwa);

	if (usuwany == nullptr)
		zakonczenie_nieprawidlowe("W grupie procesow nie wystempuje proces o nazwie " + nazwa[0] + nazwa[1]);
	else
	{
		if (this->pobierz_PCB()->nazwa[0] != '*' && usuwany->nazwa[0] == '*')
			zakonczenie_nieprawidlowe("Proces niesystemowy probuje zatrzymac proces systemowy o nazwie " + nazwa[0] + nazwa[1]);
		else
		{
			zatrzymywanie_procesu(nazwa);
			if (usuwany->zatrzymany == 1)
			{
				usuniecie_bloku_PCB_z_lacuchow(usuwany);
				while (this->pobierz_PCB()->zablokowany == 0)
				{
					this->pobierz_PCB()->pierwszy_komunikat->XR(this->pobierz_PCB());
				}
				wskaznik_na_Pamiec_ram_dla_Procesow->zwolnij_pamiec(usuwany->wielkosc_pamieci_wlasnej, usuwany->adres_pamieci_wlasnej, usuwany);
				for (int i = 0; i < listaProcesow.size(); i++)
				{
					if (usuwany == listaProcesow[i].pobierz_PCB())
					{
						listaProcesow.erase(listaProcesow.begin() + i);
						break;
					}
				}
			}
		}
	}
}
void Proces::zatrzymanie_zlecenia_i_powiadomienie_programu_nadzorczego()
{
	string wiad = "Proces zostal wykonany prawidlowo";
	this->pobierz_PCB()->pierwszy_komunikat->XS(this->pobierz_PCB(), "*S", wiad);
	while (this->pobierz_PCB()->zablokowany == 0)
	{
		this->pobierz_PCB()->pierwszy_komunikat->XR(this->pobierz_PCB());
	}
}
void Proces::doloczenie_bloku_PCB_do_lacuchow(PCB* nowyblokPCB)
{
	PCB*Poprzedni;
	if (wykonywany != nullptr)
	{
		Poprzedni = wykonywany->poprzedni_z_wszytkich_blokow_PCB;//załorzyłem że obecnie wykonywany proces pod wskaźnikiem o nazwie wykonywany
		Poprzedni->nastepny_z_wszytkich_blokow_PCB = nowyblokPCB;
		wykonywany->poprzedni_z_wszytkich_blokow_PCB = nowyblokPCB;
		nowyblokPCB->poprzedni_z_wszytkich_blokow_PCB = Poprzedni;
		nowyblokPCB->nastepny_z_wszytkich_blokow_PCB = wykonywany;
	}
	else
	{
		Poprzedni = wskaznik_na_liste_wszytkich_blokow_PCB->poprzedni_z_wszytkich_blokow_PCB;
		Poprzedni->nastepny_z_wszytkich_blokow_PCB = nowyblokPCB;
		wskaznik_na_liste_wszytkich_blokow_PCB->poprzedni_z_wszytkich_blokow_PCB = nowyblokPCB;
		nowyblokPCB->poprzedni_z_wszytkich_blokow_PCB = Poprzedni;
		nowyblokPCB->nastepny_z_wszytkich_blokow_PCB = wskaznik_na_liste_wszytkich_blokow_PCB;
	}
	if (znalezienie_bloku_PCB_o_podanej_nazwie(wykonywany->nazwa) == nullptr)
	{
		Poprzedni = this->pobierz_PCB()->poprzedni_blok_PCB_tej_grupy;
		Poprzedni->nastepny_blok_PCB_tej_grupy = nowyblokPCB;
		this->pobierz_PCB()->poprzedni_blok_PCB_tej_grupy = nowyblokPCB;
		nowyblokPCB->poprzedni_blok_PCB_tej_grupy = Poprzedni;
		nowyblokPCB->nastepny_blok_PCB_tej_grupy = this->pobierz_PCB();
	}
	else
	{
		Poprzedni = wykonywany->poprzedni_blok_PCB_tej_grupy;
		Poprzedni->nastepny_blok_PCB_tej_grupy = nowyblokPCB;
		wykonywany->poprzedni_blok_PCB_tej_grupy = nowyblokPCB;
		nowyblokPCB->poprzedni_blok_PCB_tej_grupy = Poprzedni;
		nowyblokPCB->nastepny_blok_PCB_tej_grupy = wykonywany;
	}
}
void Proces::usuniecie_bloku_PCB_z_lacuchow(PCB* usuwanyblokPCB)
{
	if (usuwanyblokPCB == wskaznik_na_liste_wszytkich_blokow_PCB)
		wskaznik_na_liste_wszytkich_blokow_PCB = usuwanyblokPCB->nastepny_z_wszytkich_blokow_PCB;

	usuwanyblokPCB->nastepny_z_wszytkich_blokow_PCB->poprzedni_z_wszytkich_blokow_PCB = usuwanyblokPCB->poprzedni_z_wszytkich_blokow_PCB;
	usuwanyblokPCB->poprzedni_z_wszytkich_blokow_PCB->nastepny_z_wszytkich_blokow_PCB = usuwanyblokPCB->nastepny_z_wszytkich_blokow_PCB;

	usuwanyblokPCB->nastepny_blok_PCB_tej_grupy->poprzedni_blok_PCB_tej_grupy = usuwanyblokPCB->poprzedni_blok_PCB_tej_grupy;
	usuwanyblokPCB->poprzedni_blok_PCB_tej_grupy->nastepny_blok_PCB_tej_grupy = usuwanyblokPCB->nastepny_blok_PCB_tej_grupy;
}
PCB* Proces::znalezienie_bloku_PCB_o_podanej_nazwie(char nazwa[2])
{
	PCB * poszukiwany = this->pobierz_PCB()->nastepny_blok_PCB_tej_grupy;
	while (poszukiwany != this->pobierz_PCB())
	{
		if (poszukiwany->nazwa[0] == nazwa[0] && poszukiwany->nazwa[1] == nazwa[1])
			return poszukiwany;
		poszukiwany = poszukiwany->nastepny_blok_PCB_tej_grupy;
	}
	return nullptr;
}
void Proces::uruchomienie_procesu(char nazwa[2])
{
	PCB * uruchamiany = znalezienie_bloku_PCB_o_podanej_nazwie(nazwa);
	if (uruchamiany == nullptr)
		zakonczenie_nieprawidlowe("W grupie procesow nie wystempuje proces o nazwie " + nazwa[0] + nazwa[1]);
	else
	{
		if (this->pobierz_PCB()->nazwa[0] != '*' && uruchamiany->nazwa[0] == '*')
			zakonczenie_nieprawidlowe(" Proces niesystemowy probuje zatrzymac proces systemowy o nazwie " + nazwa[0] + nazwa[1]);
		else
		{
			uruchamiany->zatrzymany = 0;
		}
	}
}
void Proces::zatrzymywanie_procesu(char nazwa[2])
{
	PCB * zatrzymywany = znalezienie_bloku_PCB_o_podanej_nazwie(nazwa);
	if (zatrzymywany == nullptr)
		zakonczenie_nieprawidlowe("W grupie procesow nie wystempuje proces o nazwie " + nazwa[0] + nazwa[1]);
	else
	{
		if (this->pobierz_PCB()->nazwa[0] != '*' && zatrzymywany->nazwa[0] == '*')
			zakonczenie_nieprawidlowe("Proces niesystemowy probuje zatrzymac proces systemowy o nazwie " + nazwa[0] + nazwa[1]);
		else
		{
			zatrzymywany->zatrzymany = 1;
		}
	}
}
void Proces::zakonczenie_nieprawidlowe(string tresc)
{
	string wiad = "Podczas wykonywania procesu " + this->pobierz_PCB()->nazwa[0] + this->pobierz_PCB()->nazwa[1];
	wiad = wiad + " wystapil blad:\n";
	wiad = wiad + tresc;
	wiad = wiad + "\n";
	this->pobierz_PCB()->pierwszy_komunikat->XS(this->pobierz_PCB(), "*S", wiad);
	while (this->pobierz_PCB()->zablokowany == 0)
	{
		this->pobierz_PCB()->pierwszy_komunikat->XR(this->pobierz_PCB());
	}
}
PCB *Proces::pobierz_PCB() {
	return &blokPCB;
}


////////////////////////////////////////////////////////////////////////////Semafor Definicje
SemaforPCB::SemaforPCB(int wartoscpoczatkowasemafora) {
	WartoscSemafora = wartoscpoczatkowasemafora;
}
void SemaforPCB::operacjaV() {
	WartoscSemafora += 1;
	kolejka.front()->zatrzymany = 0;
	kolejka.pop_front();
	//POWIADOMIENIE O WYSTAPIENIU OPERACJI V //u zawiadowcy ?
}

void SemaforPCB::operacjaP(PCB *proces) { //<---- wywolanie funkcji, np. semafor.operacjaP(&proces);
	WartoscSemafora -= 1;
	if (WartoscSemafora < 0) {
		kolejka.push_back(proces);
		proces->zatrzymany = 1;
	}
}
int SemaforPCB::PobierzWartoscSemafora() {
	return WartoscSemafora;
}

void SemaforPCB::WyswietlKolejke() {
	int licz = 1;
	cout << "Zawartosc sprawdzanego semafora" << endl;
	for (int i = 0; i < kolejka.size(); i++)
	{
		std::list<PCB*>::iterator it = kolejka.begin();

		advance(it, i);
		PCB* pobierz = *it;
		cout << licz << ". Nazwa procesu: " << pobierz->nazwa << " Czy zatrzymany: " << pobierz->zatrzymany << endl;
		licz++;
	}
}

////////////////////////////////////////////////////////////////////////////Karuzela Definicje

Karuzela::Karuzela() {
	this->NextTry = wskaznik_na_liste_wszytkich_blokow_PCB;
	this->Running = nullptr;
	this->IleDoKonca = 5;
	MinionyKwant = 0;
	this->NextTryModified = 0;
}

void Karuzela::run(PCB* NextTry) {
	this->Running = NextTry;
	wykonywany = Running;
	this->NextTry = Running->nastepny_z_wszytkich_blokow_PCB;
	this->MinionyKwant = 0;
}

void Karuzela::find() {
	if (NextTry->zablokowany == 0) {
		if (NextTry->zatrzymany == 0) {
			run(NextTry);
		}
		else {
			NextTry = NextTry->nastepny_z_wszytkich_blokow_PCB;
			find();
		}
	}
	else {
		NextTry = NextTry->nastepny_z_wszytkich_blokow_PCB;
		find();
	}
}

void Karuzela::ChangeNEXTTRY(PCB* Nowy) {
	this->NextTry = Nowy;
}

PCB* Karuzela::GetRunning() {
	return this->Running;
}

int Karuzela::IleRozkazow() {
	return this->IleDoKonca;
}

////////////////////////////////////////////////////////////////////////////Ram Definicje

SemaforPCB FSBSEM(1);   //semafor FSBSEM
SemaforPCB MEMORY(0);   //semafor MEMORY

FSB::FSB(int size, int begin) {
	Size = size;
	Begin = begin;
}

void FSB::set_Size(int size) { Size = size; }

void FSB::set_Begin(int begin) { Begin = begin; }

int FSB::get_Size() { return Size; }

int FSB::get_Begin() { return Begin; }

void FSB::print() { std::cout << "rozmiar bloku: " << Size << "  poczatkowy index: " << Begin << std::endl; }

Pamiec_ram::Pamiec_ram() {
	wskaznik_na_Pamiec_ram_dla_Procesow = this;
	FSB pierwszy_blok(256, 0);
	wskaznik_na_Pamiec_ram = pamiec_operacyjna;
	lista_FSB.push_back(pierwszy_blok);
}

int Pamiec_ram::przydziel_pamiec(int rozmiar, PCB* PCB) {  // funkcja przydzielajaca pamiec
	std::list<FSB>::iterator it = lista_FSB.begin();

	FSBSEM.operacjaP(PCB);   //operacja p na FSBSEM

	for (FSB x : lista_FSB) { //przeszukuje list w poszukiwaniu odpowiedniego bloku

		if (x.get_Size() == rozmiar) {
			int begin = x.get_Begin();
			lista_FSB.erase(it);

			FSBSEM.operacjaV();//operacja v na semaforze FSBSEM

			return begin;
		}
		else {
			if (x.get_Size() > rozmiar) {
				int begin = x.get_Begin();
				int size = x.get_Size();
				lista_FSB.erase(it);
				dodaj_blok_do_FSB(size - rozmiar, begin + rozmiar);

				FSBSEM.operacjaV();//operacja v na semaforze FSBSEM

				return begin;
			}
		}
		++it;
	}
	FSBSEM.operacjaV();//operacja v na semaforze FSBSEM
	MEMORY.operacjaP(PCB);//operacja p na semaforze Memory
	return 0;
}

void Pamiec_ram::print() {
	int index = 0;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			std::cout << pamiec_operacyjna[index];
			index++;
		}
		std::cout << std::endl;
	}
}

void Pamiec_ram::zwolnij_pamiec(int rozmiar, int poczatek, PCB* PCB) {

	FSBSEM.operacjaP(PCB);//operacja p na smeaforze FSBSEM

	FSB usuwany_blok(rozmiar, poczatek);
	std::list<FSB>::iterator it = lista_FSB.begin();
	for (FSB x : lista_FSB) {
		if (x.get_Size() + x.get_Begin() == poczatek) {
			usuwany_blok.set_Begin(x.get_Begin());
			usuwany_blok.set_Size(x.get_Size());
			lista_FSB.erase(it);
		}
		if (poczatek + rozmiar == x.get_Begin()) {
			usuwany_blok.set_Size(x.get_Size());
			lista_FSB.erase(it);
		}
		++it;
	}
	dodaj_blok_do_FSB(usuwany_blok.get_Size(), usuwany_blok.get_Begin());


	for (int i = 0; i<MEMORY.PobierzWartoscSemafora(); i++) {
		MEMORY.operacjaV();
	}
}

void Pamiec_ram::dodaj_blok_do_FSB(int rozmiar, int begin) {

	FSB dodawany_blok(rozmiar, begin);
	std::list<FSB>::iterator it = lista_FSB.begin();

	for (FSB x : lista_FSB) {
		if (x.get_Size() > rozmiar) {
			lista_FSB.insert(it, dodawany_blok);
			return;
		}
		++it;
	}
	lista_FSB.push_back(dodawany_blok);
}

void Pamiec_ram::wyswietl_liste_FSB() {
	for (FSB x : lista_FSB) x.print();
}

char* Pamiec_ram::wskaznik_na_RAM() {
	return wskaznik_na_Pamiec_ram;
}


////////////////////////////////////////////////////////////////////////////komunikat Definicje

std::string komunikat::XR(PCB *pc)
{
	pc->semafor_komunikatow_dla_odbiorcy->operacjaV();
	pc->wspolny_semafor_komunikatow->operacjaP(pc);
	std::string wiad = pc->pierwszy_komunikat->wiadomosc;

	pc->pierwszy_komunikat = pc->pierwszy_komunikat->wskazniknastepnejwiadomosci;
	//zaldaujdopoloadresiewrejestrze(2, wiad);  // jak sie dowiem jak mam sie dostac do rejestru to zmienie

	pc->wspolny_semafor_komunikatow->operacjaV();

	return wiad;
}

void komunikat::XS(PCB *pc, std::string odbiorca, std::string wiad)
{
	//std::string odbiorca = zaladujzrejesdtru(2);  // jak wyzej rejestr znowu
	//std::string wiad = zaladujzrejesdtru(2);
	//ujrad?em kod z Proces::znalezienie_bloku_PCB_o_podanej_nazwie(char nazwa[2])
	//w?a?ciwie to mo?na by to przenie?? na poziom pcb ale narazie starfczy
	PCB * odb = pc->nastepny_blok_PCB_tej_grupy;
	while (odb != pc)
	{
		if (odb->nazwa[0] == odbiorca[0] && odb->nazwa[1] == odbiorca[1]) {
			odb->wspolny_semafor_komunikatow->operacjaP(pc);
			komunikat wiadomka;
			wiadomka.wskazniknastepnejwiadomosci = 0;
			wiadomka.wskazniknadawcy = pc;
			wiadomka.wiadomosc = wiad;
			komunikat *messuplace = odb->pierwszy_komunikat;
			if (odb->semafor_komunikatow_dla_odbiorcy->PobierzWartoscSemafora() > 0) {
				for (int i = 0; i < odb->semafor_komunikatow_dla_odbiorcy->PobierzWartoscSemafora(); i++) {
					messuplace = messuplace->wskazniknastepnejwiadomosci;

				}
			}
			messuplace->wskazniknastepnejwiadomosci = &wiadomka;
			odb->wspolny_semafor_komunikatow->operacjaV();
			odb->semafor_komunikatow_dla_odbiorcy->operacjaV();
			return;
		}
		odb = odb->nastepny_blok_PCB_tej_grupy;
	}
	//PCB *odb = znalezienie_bloku_PCB_o_podanej_nazwie(odbiorca);

	return;
}
////////////////////////////////////////////////////////////////////////////Interpreter Definicje

void Interpreter::zerujZmienne() {
	this->rejestrA = 0;
	this->rejestrB = 0;
	this->rejestrC = 0;
	this->licznikRozkazow = 0;

	this->argA[0] = ' ';
	this->argA[1] = ' ';
	this->argB[0] = ' ';
	this->argB[1] = ' ';
	this->obecnyRozkaz[0] = ' ';
	this->obecnyRozkaz[1] = ' ';

	this->nazwaProcesu.clear();


}

void Interpreter::wyslijRejestry() {

	this->karuzela.GetRunning()->przechowywany_rejestr_A = rejestrA;
	this->karuzela.GetRunning()->przechowywany_rejestr_B = rejestrB;
	this->karuzela.GetRunning()->przechowywany_rejestr_C = rejestrC;
	this->karuzela.GetRunning()->przechowywany_licznik_Rozkazow = licznikRozkazow;



	zerujZmienne();




}

void Interpreter::pobierzRejestry() {
	if (karuzela.GetRunning()->nazwa[0] == '*' && karuzela.GetRunning()->nazwa[0] == 'G') {

		rejestrA = 0;
		rejestrB = 0;
		rejestrC = 0;
		licznikRozkazow = 0;
		nazwaProcesu = "*G";


	}

	rejestrA = this->karuzela.GetRunning()->przechowywany_rejestr_A;
	rejestrB = this->karuzela.GetRunning()->przechowywany_rejestr_B;
	rejestrC = this->karuzela.GetRunning()->przechowywany_rejestr_A;
	licznikRozkazow = this->karuzela.GetRunning()->przechowywany_licznik_Rozkazow;
	nazwaProcesu = this->karuzela.GetRunning()->nazwa;

}

void Interpreter::wyswietlStany() {
	std::cout << "A" << rejestrA << " B" << rejestrB << " C" << rejestrC << " Counter" << licznikRozkazow << " " << nazwaProcesu << std::endl;
}

int Interpreter::stringToInt(std::string &str) {
	int i;
	std::istringstream iss(str);
	iss >> i;
	return i;
}

void Interpreter::wykonajRozkaz() {

	if (karuzela.GetRunning()->nazwa[0] == '*' && karuzela.GetRunning()->nazwa[1] == 'G') {

	}
	else {

		if (obecnyRozkaz == "mv") {
			mv();
		}
		else if (obecnyRozkaz == "ad") {
			ad();
		}
		else if (obecnyRozkaz == "sb") {
			sb();
		}
		else if (obecnyRozkaz == "mp") {
			mp();
		}
		else if (obecnyRozkaz == "dv") {
			dv();
		}
		else if (obecnyRozkaz == "j0") {
			j0();
		}
		else if (obecnyRozkaz == "jp") {
			jp();
		}
		else if (obecnyRozkaz == "fm") {
			fm();
		}
		else if (obecnyRozkaz == "fw") {
			fw();
		}
		else if (obecnyRozkaz == "fr") {
			fr();
		}
		else if (obecnyRozkaz == "pt") {
			pt();
		}
		else if (obecnyRozkaz == "ex") {
			ex();
		}
		else if (obecnyRozkaz == "dl") {
			dl();
		}
		else if (obecnyRozkaz == "cp") {
			cp();
		}
		else if (obecnyRozkaz == "xs") {
			xs();
		}
		else if (obecnyRozkaz == "xr") {
			xr();
		}
		else if (obecnyRozkaz == "df") {
			df();
		}

	}


}




void Interpreter::wyswietlWykonywanyRozkaz() {
	std::cout << "Wykonywany Rozkaz: " << this->obecnyRozkaz << "arg A: " << argA << " arg B: " << argB << std::endl;
}
void Interpreter::zaladujProgramyNaDysk() {
	dysk.utworzNowyPustyPlik("p1");
	dysk.utworzNowyPustyPlik("p2");
	dysk.utworzNowyPustyPlik("p3");
	dysk.utworzNowyPustyPlik("p4");


	dysk.wpiszDaneDoPustegoPliku("p1", "mvRA08 mvRB00 mvRC01 ptRB ptRC adRBRC ptRB adRCRB ptRC sbRA02 j014 ex dl");
	dysk.wpiszDaneDoPustegoPliku("p2", "mvRA04 mvRB01 mpRBRB adRCRB adRB01 sbRA01 j012 fmTX fwTXRC frTXRA ptRA ex dl");
	dysk.wpiszDaneDoPustegoPliku("p3", "mvRA08 mvRB02 cpT2p4 xsT2go ex dl");
	dysk.wpiszDaneDoPustegoPliku("p4", "xr adRARB ptRA ex dl");

}

int Interpreter::sprawdzRozkaz(char rozkaz[2]) {
	if (karuzela.GetRunning()->nazwa[0] == '*' && karuzela.GetRunning()->nazwa[0] == 'G') {}
	else {
		if (rozkaz[0] == 'm' && rozkaz[1] == 'v') { // mv
			return 2;
		}
		if (rozkaz[0] == 'd' && rozkaz[1] == 'f') {// df
			return 2;

		}
		if (rozkaz[0] == 'f' && rozkaz[1] == 'w') {// fw
			return 2;

		}
		if (rozkaz[0] == 'f' && rozkaz[1] == 'm') {//fm
			return 2;

		}
		if (rozkaz[0] == 'a' && rozkaz[1] == 'd') {//ad
			return 2;

		}
		if (rozkaz[0] == 's' && rozkaz[1] == 'b') {//sb
			return 2;

		}
		if (rozkaz[0] == 'm' && rozkaz[1] == 'p') {//mp
			return 2;

		}
		if (rozkaz[0] == 'd' && rozkaz[1] == 'v') {//dv
			return 2;

		}
		if (rozkaz[0] == 'c' && rozkaz[1] == 'p') {//cp
			return 2;

		}
		if (rozkaz[0] == 'p' && rozkaz[1] == 't') {//pt
			return 1;
		}
		if (rozkaz[0] == 'j' && rozkaz[1] == '0') {//j0
			return 1;

		}
		if (rozkaz[0] == 'j' && rozkaz[1] == 'p') {//jp
			return 1;

		}
		if (rozkaz[0] == 'e' && rozkaz[1] == 'x') {//ex
			return 0;
		}
		if (rozkaz[0] == 'd' && rozkaz[1] == 'l') {//dl
			return 0;
		}


	}
}
void Interpreter::pobierzRozkaz() {
	if (karuzela.GetRunning()->nazwa[0] == '*' && karuzela.GetRunning()->nazwa[0] == 'G') {}
	else {
		int wielkosc = karuzela.GetRunning()->wielkosc_pamieci_wlasnej;
		int begin = karuzela.GetRunning()->adres_pamieci_wlasnej;
		int WhiteSpace = 0;

		obecnyRozkaz[0] = RAM.wskaznik_na_RAM()[(licznikRozkazow * 2) + begin];
		obecnyRozkaz[1] = RAM.wskaznik_na_RAM()[(licznikRozkazow * 2) + begin + 1];


		if (obecnyRozkaz[0] == ' ') {
			WhiteSpace = 1;

			obecnyRozkaz[0] = RAM.wskaznik_na_RAM()[(licznikRozkazow * 2) + begin + 1];
			obecnyRozkaz[1] = RAM.wskaznik_na_RAM()[(licznikRozkazow * 2) + begin + 2];

		}


		switch (sprawdzRozkaz(obecnyRozkaz)) {
		case 0:break;
		case 1:

			argA[0] = RAM.wskaznik_na_RAM()[(licznikRozkazow * 2) + begin + WhiteSpace + 2];
			argA[1] = RAM.wskaznik_na_RAM()[(licznikRozkazow * 2) + begin + WhiteSpace + 3];

			break;


		case 2:

			argA[0] = RAM.wskaznik_na_RAM()[(licznikRozkazow * 2) + begin + WhiteSpace + 2];
			argA[1] = RAM.wskaznik_na_RAM()[(licznikRozkazow * 2) + begin + WhiteSpace + 3];
			argB[0] = RAM.wskaznik_na_RAM()[(licznikRozkazow * 2) + begin + WhiteSpace + 4];
			argB[1] = RAM.wskaznik_na_RAM()[(licznikRozkazow * 2) + begin + WhiteSpace + 5];

			break;


		}

		std::cout << "Zaladowano rozkaz z pamieci ram!";
	}
}
void Interpreter::mv() {
	std::string A = "RA";
	std::string B = "RB";
	std::string C = "RC";



	std::string pomoc(argA);
	std::string pomoc2(argB);

	int pomocnicza = stringToInt(pomoc2);

	if (argA == A) {
		rejestrA = pomocnicza;
	}
	else if (argA == B) {
		rejestrB = pomocnicza;
	}
	else if (argA == C) {
		rejestrC = pomocnicza;
	}

	licznikRozkazow += 3;
	ZF = false;

}
void Interpreter::ad() {
	std::string A = "RA";
	std::string B = "RB";
	std::string C = "RC";

	int zmiennaPomocnicza;
	if (argA == A) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrA + rejestrA;
			rejestrA = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrA + rejestrB;
			rejestrA = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrA + rejestrC;
			rejestrA = zmiennaPomocnicza;
		}
	}




	else if (argA == B) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrB + rejestrA;
			rejestrB = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrB + rejestrB;
			rejestrB = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrB + rejestrC;
			rejestrB = zmiennaPomocnicza;
		}
	}




	else if (argA == C) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrC + rejestrA;
			rejestrC = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrC + rejestrB;
			rejestrC = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrC + rejestrC;
			rejestrC = zmiennaPomocnicza;
		}
	}
	licznikRozkazow += 3;
	if (zmiennaPomocnicza == 0) { ZF = true; }


}
void Interpreter::sb() {
	std::string A = "RA";
	std::string B = "RB";
	std::string C = "RC";
	int zmiennaPomocnicza;

	if (argA == A) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrA - rejestrA;
			rejestrA = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrA - rejestrB;
			rejestrA = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrA - rejestrC;
			rejestrA = zmiennaPomocnicza;
		}
	}


	else if (argA == B) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrB - rejestrA;
			rejestrB = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrB - rejestrB;
			rejestrB = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrB - rejestrC;
			rejestrB = zmiennaPomocnicza;
		}
	}

	//


	else if (argA == C) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrC - rejestrA;
			rejestrC = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrC - rejestrB;
			rejestrC = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrC - rejestrC;
			rejestrC = zmiennaPomocnicza;
		}
	}
	licznikRozkazow += 3;
	if (zmiennaPomocnicza == 0) { ZF = true; }

}
void Interpreter::mp() {
	std::string A = "RA";
	std::string B = "RB";
	std::string C = "RC";
	int zmiennaPomocnicza;

	if (argA == A) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrA * rejestrA;
			rejestrA = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrA * rejestrB;
			rejestrA = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrA * rejestrC;
			rejestrA = zmiennaPomocnicza;
		}
	}

	//


	else if (argA == B) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrB * rejestrA;
			rejestrB = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrB * rejestrB;
			rejestrB = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrB * rejestrC;
			rejestrB = zmiennaPomocnicza;
		}
	}

	//


	else if (argA == C) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrC * rejestrA;
			rejestrC = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrC * rejestrB;
			rejestrC = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrC * rejestrC;
			rejestrC = zmiennaPomocnicza;
		}
	}
	licznikRozkazow += 3;
	if (zmiennaPomocnicza == 0) { ZF = true; }

}


void Interpreter::dv() {
	std::string A = "RA";
	std::string B = "RB";
	std::string C = "RC";
	int zmiennaPomocnicza;


	if (argA == A) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrA / rejestrA;
			rejestrA = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrA / rejestrB;
			rejestrA = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrA / rejestrC;
			rejestrA = zmiennaPomocnicza;
		}
	}

	//


	else if (argA == B) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrB / rejestrA;
			rejestrB = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrB / rejestrB;
			rejestrB = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrB / rejestrC;
			rejestrB = zmiennaPomocnicza;
		}
	}




	else if (argA == C) {
		if (argA == A) {
			zmiennaPomocnicza = rejestrC / rejestrA;
			rejestrC = zmiennaPomocnicza;
		}
		else if (argA == B) {
			zmiennaPomocnicza = rejestrC / rejestrB;
			rejestrC = zmiennaPomocnicza;
		}
		else {
			zmiennaPomocnicza = rejestrC / rejestrC;
			rejestrC = zmiennaPomocnicza;
		}
	}
	licznikRozkazow += 3;
	if (zmiennaPomocnicza == 0) { ZF = true; }

}

void Interpreter::j0() {
	if (ZF) {
		std::string help(argA);

		int pomoc = stringToInt(help);

		licznikRozkazow = pomoc;
		ZF = false;
	}
	else licznikRozkazow += 2;
}

void Interpreter::fm() {
	dysk.utworzNowyPustyPlik(argA);
	licznikRozkazow += 2;
	ZF = false;
}

void Interpreter::fw() {
	dysk.wpiszDaneDoPustegoPliku(argA, argB);
	licznikRozkazow += 2;
	ZF = false;
}


void Interpreter::fr() {
	dysk.wyswietlZawartoscPliku(argA);
	licznikRozkazow += 2;
	ZF = false;
}

void Interpreter::df() {
	dysk.usunPlik(argA);
	licznikRozkazow += 2;
	ZF = false;
}

void Interpreter::pt() {
	std::string A = "RA";
	std::string B = "RB";

	if (argA == A) {
		std::cout << rejestrA;
	}
	else if (argA == B) {
		std::cout << rejestrB;

	}
	else {
		std::cout << rejestrC;
	}
	licznikRozkazow += 2;
	ZF = false;
}


void Interpreter::jp() {

	std::string help = std::string(argA);
	int pomocnicza = stringToInt(help);
	licznikRozkazow = pomocnicza;
	ZF = false;
}




void Interpreter::ex() {
	Proces * zatrzymywany = nullptr;
	for (int i = 0; i < listaProcesow.size(); i++)
	{
		if (karuzela.GetRunning() == listaProcesow[i].pobierz_PCB())
		{
			zatrzymywany = &listaProcesow[i];
			break;
		}
	}
	PCB * ktory = zatrzymywany->znalezienie_bloku_PCB_o_podanej_nazwie("*S");
	if (ktory == lista_wskaznikow_na_procesy_nadzorcze[0]->pobierz_PCB())
	{
		lista_wskaznikow_na_procesy_nadzorcze[0]->zatrzymywanie_procesu(zatrzymywany->pobierz_PCB()->nazwa);
	}
	else
		lista_wskaznikow_na_procesy_nadzorcze[1]->zatrzymywanie_procesu(zatrzymywany->pobierz_PCB()->nazwa);


	licznikRozkazow++;

	ZF = false;
}


void Interpreter::dl() {

	Proces * usuwany = nullptr;
	for (int i = 0; i < listaProcesow.size(); i++)
	{
		if (karuzela.GetRunning() == listaProcesow[i].pobierz_PCB())
		{
			usuwany = &listaProcesow[i];
			break;
		}
	}
	PCB * ktory = usuwany->znalezienie_bloku_PCB_o_podanej_nazwie("*S");
	if (ktory == lista_wskaznikow_na_procesy_nadzorcze[0]->pobierz_PCB())
	{
		lista_wskaznikow_na_procesy_nadzorcze[0]->usun_proces(usuwany->pobierz_PCB()->nazwa);
	}
	else
		lista_wskaznikow_na_procesy_nadzorcze[1]->usun_proces(usuwany->pobierz_PCB()->nazwa);


	licznikRozkazow += 2;

	ZF = false;

}
void Interpreter::rp(std::string argumentA) {

	Proces * uruchamiany = nullptr;
	for (int i = 0; i < listaProcesow.size(); i++)
	{
		if (karuzela.GetRunning() == listaProcesow[i].pobierz_PCB())
		{
			uruchamiany = &listaProcesow[i];
			break;
		}
	}
	PCB * ktory = uruchamiany->znalezienie_bloku_PCB_o_podanej_nazwie("*S");
	if (ktory == lista_wskaznikow_na_procesy_nadzorcze[0]->pobierz_PCB())
	{
		lista_wskaznikow_na_procesy_nadzorcze[0]->uruchomienie_procesu(uruchamiany->pobierz_PCB()->nazwa);
	}
	else
		lista_wskaznikow_na_procesy_nadzorcze[1]->uruchomienie_procesu(uruchamiany->pobierz_PCB()->nazwa);



}
void Interpreter::CreateProcesses() {
	char proces1[3] = { 'p','1','\0' };
	char proces2[3] = { 'p','2','\0' };
	char proces3[3] = { 'p','3','\0' };
	lista_wskaznikow_na_procesy_nadzorcze[0]->utworz_proces(proces1, dysk.zawartoscPliku("p1").length(), dysk.zawartoscPliku("p1"));
	lista_wskaznikow_na_procesy_nadzorcze[0]->utworz_proces(proces2, dysk.zawartoscPliku("p2").length(), dysk.zawartoscPliku("p2"));
	lista_wskaznikow_na_procesy_nadzorcze[1]->utworz_proces(proces3, dysk.zawartoscPliku("p3").length(), dysk.zawartoscPliku("p3"));



}

void Interpreter::cp() {

	char proces4[2] = { 'p','4' };
	lista_wskaznikow_na_procesy_nadzorcze[1]->utworz_proces(proces4, dysk.zawartoscPliku("p4").length(), dysk.zawartoscPliku("p4"));




	licznikRozkazow += 3;

	ZF = false;
}


void Interpreter::xs() {

	std::string odbiorca(argA);
	std::string wiadomosc(argB);

	karuzela.GetRunning()->pierwszy_komunikat->XS(karuzela.GetRunning(), odbiorca, wiadomosc);

	licznikRozkazow += 3;
	ZF = false;
}
void Interpreter::xr() {

	std::cout << "Proces o nazwie:" << karuzela.GetRunning()->nazwa << " Odczytal wiadomosc: " << karuzela.GetRunning()->pierwszy_komunikat->XR(karuzela.GetRunning());

	licznikRozkazow++;
	ZF = false;
}

void Interpreter::go() {


	if (karuzela.GetRunning() == nullptr) {
		karuzela.find();
	}

	if (karuzela.GetRunning()->nazwa == "*G") {

		karuzela.MinionyKwant++;

		if (karuzela.MinionyKwant >= karuzela.IleRozkazow() || karuzela.GetRunning()->zatrzymany == 1 || karuzela.GetRunning()->zablokowany == 1)
		{
			cout << "Proces oddaje procesor" << endl;
			karuzela.find();
		}

	}

	else
	{
		pobierzRejestry();
		wykonajRozkaz();
		wyswietlStany();
		wyslijRejestry();
		karuzela.MinionyKwant++;

		if (karuzela.MinionyKwant >= karuzela.IleRozkazow() || karuzela.GetRunning()->zatrzymany == 1 || karuzela.GetRunning()->zablokowany == 1)
		{
			cout << "Proces oddaje procesor" << endl;
			karuzela.find();
		}
	}

}

void Interpreter::wk() {

	dysk.wyswietlKatalog();

}

void Interpreter::wb() {

	dysk.wyswietlWektorBitowy();

}

void Interpreter::sz(int index) {
	dysk.wyswietlSektorZnakowo(index);

}

void Interpreter::fb() {
	RAM.wyswietl_liste_FSB();
}

void Interpreter::komendaUzytkownika() {

	std::string komenda, argument;

	int strToInt;




	std::cout << "Wpisz komende: ";
	std::cin >> komenda;


	while (komenda != "exit") {


		if (komenda == "rp") {
			std::cout << "Podaj argument: ";
			std::cin >> argument;

			rp(argument);

		}
		else if (komenda == "wb") {
			wb();
		}
		else if (komenda == "sz") {

			std::cout << "Podaj argument: ";
			std::cin >> argument;
			strToInt = stringToInt(argument);
			sz(strToInt);

		}
		else if (komenda == "wk") {
			wk();
		}
		else if (komenda == "fb") {
			fb();
		}
		else if (komenda == "rs") {
			wyswietlStany();
		}
		else if (komenda == "quit") {
			std::cout << "Do widzenia\n";
			return;
		}
		else if (komenda == "help") {
			std::cout << "Lista rozkazów:\n";
			std::cout << "rp - Uruchamia proces\n";
			std::cout << "wb - Wyswietla Wektor Bitowy\n";
			std::cout << "sz = Wyswietl Sektor Znakowo\n";
			std::cout << "wk - Wyswietl Katalog\n";
			std::cout << "fb - Wyswietl liste FSB\n";
			std::cout << "rs - wyswietla stany rejestrow\n";
			std::cout << "quit - Wylacza system\n";
			std::cout << "\n";

		}


		std::cout << "Wpisz komende: ";
		std::cin >> komenda;


	}
	//itd itp

}

////////////////////////////////////////////////////////////////////////////Zarzadzanie Procesami Definicje
////////////////////////////////////////////////////////////////////////////Zarzadzanie Procesami Definicje

void goodmorning() {	
	std::cout << "              _                      _    _              ____   _____ " << std::endl;
	std::cout << "             | |                    | |  (_)            / __ \ / ____|" << std::endl;
	std::cout << "  _ __   ___ | |___      _____  _ __| | ___ _ __   __ _| |  | | (___  " << std::endl;
	std::cout << " | '_ \ / _ \| __\ \ /\ / / _ \| '__| |/ / | '_ \ / _` | |  | |\___ \ " << std::endl;
	std::cout << " | | | | (_) | |_ \ V  V / (_) | |  |   <| | | | | (_| | |__| |____) |" << std::endl;
	std::cout << " |_| |_|\___/ \__| \_/\_/ \___/|_|  |_|\_\_|_| |_|\__, |\____/|_____/ " << std::endl;
	std::cout << "                                                   __/ |              " << std::endl;
	std::cout << "                                                  |___/               " << std::endl;
	


}
int main() {
	goodmorning();

	char G[2] = { '*','G' };
	char S[2] = { '*','S' };

	Proces proces_G = Proces(G);
	Proces proces_S1 = Proces(S);
	Proces proces_S2 = Proces(S);

	Interpreter interpreter;
	interpreter.zaladujProgramyNaDysk();
	interpreter.CreateProcesses();
	

	std::cout << "Dzien dobry \n";
	interpreter.komendaUzytkownika();

	cin.ignore(2);

	return 0;
}
