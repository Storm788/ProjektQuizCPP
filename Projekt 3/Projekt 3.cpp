#include <iostream>
#include <vector>
#include <string>
#include <fstream> 
#include <sqlite3.h>
using namespace std;

class Otazka {
public:
	string textOtazky;
	vector<string> moznosti;
	int spravnaMoznost;
	Otazka(const string& t, const vector<string>& m, int spravna)
		: textOtazky(t), moznosti(m), spravnaMoznost(spravna) {}
};

void pridatOtazku(vector<Otazka>& otazky, const string& textOtazky, const vector<string>& moznosti, int spravnaMoznost) {
	otazky.emplace_back(textOtazky, moznosti, spravnaMoznost);
}

void zobrazitOtazky(const vector<Otazka>& otazky) {
	if (otazky.empty()) {
		cout << "Zadne otazky k zobrazeni.\n";
		return;
	}

	// Zobrazeni otazek
	for (const auto& otazka : otazky) {
		cout << "Otazka: " << otazka.textOtazky << "\n";
		for (int i = 0; i < 4; ++i) {
			cout << i + 1 << ". " << otazka.moznosti[i] << "\n";
		}
		cout << "Spravna moznost: " << otazka.spravnaMoznost + 1 << endl;
	}
}

void spustitKviz(const vector<Otazka>& otazky) {
	if (otazky.empty()) {
		cout << "Zadne otazky k dispozici pro kviz.\n";
		return;
	}

	int skore = 0;
	for (const auto& otazka : otazky) {
		cout << otazka.textOtazky << endl; // Zobrazeni otazky
		for (int i = 0; i < 4; ++i) { // Zobrazeni moznosti
			cout << i + 1 << ". " << otazka.moznosti[i] << "\n";
		}

		int odpoved;
		cout << "Zadejte svou odpoved (1-4): ";
		cin >> odpoved;

		// Kontrola spravnosti odpovedi
		if (odpoved - 1 == otazka.spravnaMoznost) {
			cout << "Spravne!" << endl;
			cout << endl;
			++skore;
		}
		else {
			cout << "Spatne! Spravna odpoved byla -> (" << otazka.spravnaMoznost + 1 << ". " << otazka.moznosti[otazka.spravnaMoznost] << ")" << endl;
			cout << endl;
		}
	}
	for (int i = 0; i < 1; i++) {
		cout << endl;
	}
	cout << "Kviz dokoncen!\nZiskali jste " << skore << " z " << otazky.size() << ".\n";
	for (int i = 0; i < 2; i++) {
		cout << endl;
	}
}

void nacistOtazkyZeSouboru(const string& nazevSouboru, vector<Otazka>& otazky) {
	ifstream soubor(nazevSouboru);
	if (!soubor.is_open()) {
		cout << "Nepodarilo se otevrit soubor: " << nazevSouboru << endl;
		return;
	}

	string textOtazky;
	vector<string> moznosti(4);
	int spravnaMoznost;

	while (getline(soubor, textOtazky)) {
		for (int i = 0; i < 4; ++i) {
			getline(soubor, moznosti[i]);
		}
		soubor >> spravnaMoznost;
		soubor.ignore(); // Ignor zbytek radku

		pridatOtazku(otazky, textOtazky, moznosti, spravnaMoznost);
	}

	soubor.close();
}


void nacistOtazkyZDatabaze(const string& nazevDatabaze, vector<Otazka>& otazky) {

	sqlite3* databaze;
	const char* cteniSQL = "SELECT Otazka, Odpoved1, Odpoved2, Odpoved3, Odpoved4, SpravnaOd FROM Quiz";
	sqlite3_stmt* statement;

	if(sqlite3_open(nazevDatabaze.c_str(), &databaze)) {
		cout << "Nepodarilo se otevrit: " << nazevDatabaze << endl;
		return;
	}
	
	if (sqlite3_prepare_v2(databaze, cteniSQL, -1, &statement, nullptr) != SQLITE_OK) {
		sqlite3_close(databaze);
		return;
	}
	

	while (sqlite3_step(statement) == SQLITE_ROW) {
		string textOtazky = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
		vector<string> moznosti(4);
		for (int i = 0; i < 4; ++i) {
			moznosti[i] = reinterpret_cast<const char*>(sqlite3_column_text(statement, i + 1));
		}
		int spravnaMoznost = sqlite3_column_int(statement, 5);

		pridatOtazku(otazky, textOtazky, moznosti, spravnaMoznost);
	}

	sqlite3_finalize(statement);
	sqlite3_close(databaze);
}

int main() {
	vector<Otazka> otazky;
	//nacistOtazkyZeSouboru("otazky.txt", otazky);
	nacistOtazkyZDatabaze("Pro3.db", otazky);
	bool cyklus = true;

	int volba;
	while (cyklus) {
		cout << "1. Spustit kviz\n2. Zobrazit otazky\n3. Konec\n";
		cout << "Zadejte svou volbu: ";
		cin >> volba;

		switch (volba) {
		case 1:
			spustitKviz(otazky);
			break;
		case 2:
			zobrazitOtazky(otazky);
			break;
		case 3:
			cyklus = false;
			break;
		default:
			cout << "Neplatna volba. Zkuste to znovu.\n";
		}
	}
}