#define _CRT_SECURE_NO_WARNINGS
#define WIERSZE 20
#define KOLUMNY 20 
#include <iostream>
#include<stdio.h>
#include "conio2.h" 
#include <time.h>
#include <ctime>
#include<string.h>
using namespace std;
char kierunek_patrzenia(int *pionek, int zn);
int labirynt_2D_wyswietlanie(FILE*labirynt, const char string[128]);
int ruch_kolizja_tyl(FILE*labirynt, int *x, int *y, int i, const char string[128]);
int ruch_kolizja_przod(FILE*labirynt, int *x, int *y, int i, const char string[128]);
void poz_pocz_i_meta(FILE*labirynt, int *x, int *y, int *wyjscie_labiryntu, const char nazwa_lab[256]);
void restart(int *ruchy, int *y, int *i, int zn);
int startTime = clock();
void wczytaj_lab( char nazwa_lab[256]);
enum kierunek_t {
	LEWO = 0x4b,
	PRAWO = 0x4d,
	GORA = 0x48,
	DOL = 0x50
};

int main()
{
	textmode(18);
	int pionek = 0, zn = 0, poz_x, poz_y = 1, attr = 14, ruchy = 0, czas, wyjscie_labiryntu; //pionek to kierunek patrzenia: 0-dol, 1-lewo, 2-gora, 3-prawo
	char nazwa_lab[256];

	FILE*labirynt;
	strcpy(nazwa_lab, "labirynt_domyslny.txt");
	labirynt = fopen(nazwa_lab, "r");
	if (labirynt == NULL) {
		cout << "Nie udalo sie otworzyc pliku";
		return -1;
	}
	poz_pocz_i_meta(labirynt, &poz_x, &poz_y, &wyjscie_labiryntu, nazwa_lab);
	labirynt_2D_wyswietlanie(labirynt, "labirynt_domyslny.txt");
	fclose(labirynt);
	
	settitle("Jakub Kotowski 161969");
	textbackground(BLACK);
	do {
		czas = (clock() - startTime) / CLOCKS_PER_SEC;
		if (poz_y == wyjscie_labiryntu) {
			gotoxy(22, 22);
			cputs("GRATULACJE!");
		}
		textcolor(7);
		gotoxy(48, 1);
		cputs("q = wyjscie");
		gotoxy(48, 2);
		cputs("strzalki gora/dol = poruszanie");
		gotoxy(48, 3);
		cputs("strzalki lewo/prawo = kierunek");
		gotoxy(48, 4);
		cputs("spacja = zmiana koloru");
		gotoxy(48, 5);
		cputs("r = reset gry");
		gotoxy(48, 6);
		cputs("i = Wczytaj domyslny labirynt");
		gotoxy(48, 7);
		cputs("o = Wczytaj wlasny labirynt");
		gotoxy(48, 8);
		printf("Wykonane ruchy = %d", ruchy);
		gotoxy(48, 9);
		printf("Czas: %lld", czas);
		gotoxy(poz_x, poz_y);
		textcolor(attr);
		putch(kierunek_patrzenia(&pionek, (zn)));
		zn = getch();
		if (zn == 0) {
			zn = getch();
			if (zn == GORA) {
				if (ruch_kolizja_przod(labirynt, &poz_x, &poz_y, pionek, nazwa_lab) != -1) 
					ruchy++; 
			}
			else if (zn == DOL) {

				if (ruch_kolizja_tyl(labirynt, &poz_x, &poz_y, pionek, nazwa_lab) != -1)
					ruchy++;
			}
			else if (zn == LEWO) {
				pionek -= 1;
				kierunek_patrzenia(&pionek, LEWO);
			}
			else if (zn == PRAWO) {
				pionek += 1;
				kierunek_patrzenia(&pionek, PRAWO);
			}
		}
		else if (zn == ' ') attr = (attr + 1) % 16;
		else if (zn == 'r') {
			restart(&ruchy, &poz_y, &pionek, zn);
			gotoxy(1, 1);
			labirynt_2D_wyswietlanie(labirynt, nazwa_lab);
			poz_pocz_i_meta(labirynt, &poz_x, &poz_y, &wyjscie_labiryntu, nazwa_lab);
		}
		else if (zn == 'i') {
			restart(&ruchy, &poz_y, &pionek, zn);
			gotoxy(1, 1);
			strcpy(nazwa_lab, "labirynt_domyslny.txt");
			labirynt_2D_wyswietlanie(labirynt, nazwa_lab);
			poz_pocz_i_meta(labirynt, &poz_x, &poz_y, &wyjscie_labiryntu, nazwa_lab);
			pionek = 0;
			poz_y = 1;
		}
		else if (zn == 'o') {
			wczytaj_lab(nazwa_lab);
			poz_pocz_i_meta(labirynt, &poz_x, &poz_y, &wyjscie_labiryntu, nazwa_lab);
			restart(&ruchy, &poz_y, &pionek, zn); poz_y = 1;
		}
	} while (zn != 'q');
	return 0;
}

char kierunek_patrzenia(int *pionek, int zn) {
	char wskaznik_gracz[4] = { 'v','<','^' ,'>' };
	if (zn == LEWO) {
		(*pionek)++; if (*pionek >= 4)*pionek = 0;
	}
	else if (zn == PRAWO) {
		(*pionek)--; if (*pionek >= 4)*pionek = 0; if (*pionek == -1)*pionek = 3;
	}

	return(wskaznik_gracz[*pionek]);
}

int labirynt_2D_wyswietlanie(FILE*labirynt, const char nazwa_lab[128]) {
	char labirynt_laduj[WIERSZE][KOLUMNY];

	labirynt = fopen(nazwa_lab, "r");
	if (labirynt == NULL) {
		cout << "Nie udalo sie otworzyc pliku";
	}
	for (int i = 0; i < WIERSZE; i++) { 
		for (int j = 0; j < KOLUMNY; j++) {
			fscanf(labirynt, "%c", &labirynt_laduj[i][j]);
		}
		fscanf(labirynt, "\n");
	}
	textcolor(WHITE);
	for (int i = 0; i < WIERSZE; i++) {
		for (int j = 0; j < KOLUMNY; j++) {			
			if (labirynt_laduj[i][j] == '0'&& labirynt_laduj[i][j + 1] == '0'){ //Sprawdzamy, czy istnieje pole 2x2
				if (labirynt_laduj[i+1][j] == '0'&& labirynt_laduj[i + 1][j+1] == '0') {
					clrscr();
					cout << "BLAD!\n Labirynt nie moze zawierac pustego pola 2x2";	
					return 0;
				}
		}
			if (labirynt_laduj[i][j] == '0')
				labirynt_laduj[i][j] = ' ';
			else if (labirynt_laduj[i][j] == '1')
				labirynt_laduj[i][j] = '*';
			else if (labirynt_laduj[i][j] == '2')
				labirynt_laduj[i][j] = '$';
			else if (labirynt_laduj[i][j] == '3')
				labirynt_laduj[i][j] = '%';
			else if (labirynt_laduj[i][j] == '4')
				labirynt_laduj[i][j] = '#';
			else if (labirynt_laduj[i][j] == '5')
				labirynt_laduj[i][j] = '@';
			printf("%c", labirynt_laduj[i][j]);
		}
		printf("\n");
	}
	fclose(labirynt);
}

int ruch_kolizja_tyl(FILE*labirynt, int *x, int *y, int pionek, const char nazwa_lab[128]) {
	char labirynt_wczytany[WIERSZE][KOLUMNY];
	char wskaznik_gracz[4] = { 'v','<','^' ,'>' };
	labirynt = fopen(nazwa_lab, "r");
	if (labirynt == NULL) {
		cout << "Nie udalo sie otworzyc pliku";
		return -1;
	}
	for (int i = 0; i < WIERSZE; i++) {
		for (int j = 0; j < KOLUMNY; j++) {
			fscanf(labirynt, "%c", &labirynt_wczytany[i][j]);
		}
		fscanf(labirynt, "\n");
	}
	if (wskaznik_gracz[pionek] =='v') {
		if (labirynt_wczytany[*y - 2][*x - 1] == '0') {
			gotoxy(*x, *y);
			putch(' '); 
			return (*y)--;
		}
		else return -1; //Jesli przednami jest sciana a chcemy wykonac ruch to nie licze tego jako wykonanie ruchu i zwracam -1 aby ruch sie nie dodal
	}
	else if (wskaznik_gracz[pionek] == '<') {
		if (labirynt_wczytany[*y - 1][*x] == '0') {
			gotoxy(*x, *y);
			putch(' ');
			return (*x)++;
		}
		else return -1;
	}
	else if (wskaznik_gracz[pionek] == '^') {
		if (labirynt_wczytany[*y][*x - 1] == '0') {
			gotoxy(*x, *y);
			putch(' ');
			return (*y)++;
		}
		else return -1;
	}
	else if (wskaznik_gracz[pionek] == '>') {
		if (labirynt_wczytany[*y - 1][*x - 2] == '0') {
			gotoxy(*x, *y);
			putch(' ');
			return (*x)--;
		}
		else return -1;
	}
	fclose(labirynt);
}

int ruch_kolizja_przod(FILE*labirynt, int *x, int *y, int pionek, const char nazwa_lab[128]) {
	char labirynt_wczytany[WIERSZE][KOLUMNY];
	char wskaznik_gracz[4] = { 'v','<','^' ,'>' };
	labirynt = fopen(nazwa_lab, "r");
	if (labirynt == NULL) {
		cout << "Nie udalo sie otworzyc pliku";
		return -1;
	}
	for (int i = 0; i < WIERSZE; i++) {
		for (int j = 0; j < KOLUMNY; j++) {
			fscanf(labirynt, "%c", &labirynt_wczytany[i][j]);
		}
		fscanf(labirynt, "\n");
	}
	if (wskaznik_gracz[pionek] == 'v') {
		if (labirynt_wczytany[*y][*x - 1] == '0') {
			gotoxy(*x, *y);
			putch(' ');
			return (*y)++;
		}
		else return -1;
	}
	else if (wskaznik_gracz[pionek] == '<') {
		if (labirynt_wczytany[*y - 1][*x - 2] == '0') {
			gotoxy(*x, *y);
			putch(' ');
			return (*x)--;
		}
		else return -1;
	}
	else if (wskaznik_gracz[pionek] == '^') {
		if (labirynt_wczytany[*y - 2][*x - 1] == '0') {
			gotoxy(*x, *y);
			putch(' ');
			return (*y)--;
		}
		else return -1;
	}
	else if (wskaznik_gracz[pionek] == '>') {
		if (labirynt_wczytany[*y - 1][*x] == '0') {
			gotoxy(*x, *y);
			putch(' ');
			return (*x)++;
		}
		else return -1;
	}
	fclose(labirynt);
}

void poz_pocz_i_meta(FILE*labirynt, int *poz_x, int *poz_y, int *wyjscie_labiryntu, const char nazwa_lab[256]) {
	int pozycja_pocz, pozycja_konc;
	char lab[WIERSZE][KOLUMNY];
	labirynt = fopen(nazwa_lab, "r");
	if (labirynt == NULL) {
		cout << "Nie udalo sie otworzyc pliku";
	}
	for (int p = 0; p < 20; p++) {
		fscanf(labirynt, "%c", &lab[0][p]);
		if (lab[0][p] == '0') {
			*poz_x = p + 1;
			break;
		}
	}
	for (int k = 0; k < 20; k++) {
		fscanf(labirynt, "%c", &lab[19][k]);
		if (lab[19][k] == '0')
			*wyjscie_labiryntu = k + 1;
	}
	fclose(labirynt);
}

void restart(int *ruchy, int *y, int *pionek, int zn) {
	if (zn == 'r') {
		clrscr(); //Nie chce czyscic odrazu ekranu jesli ktos wybierze opcje"wczytaj domyslny plik"(chodzi o estetyke)
	}
	startTime = clock();
	*ruchy = 0;
	*y = 1;
	*pionek = 0;

}

void wczytaj_lab(char nazwa_lab[256]) {
	char nazwa_lab_nowy[256];
	gotoxy(1, 22);
	cputs("Prosze wpisac nazwe labiryntu: ");
	gotoxy(1, 23);
	scanf("%s", nazwa_lab_nowy);
	strcpy(nazwa_lab, nazwa_lab_nowy);//ustawia na pozycje startowa/meta, nie trzeba wywolywac funkcji
	FILE*labirynt;
	labirynt = fopen(nazwa_lab, "r");
	if (labirynt == NULL) {
		cout << "Nie udalo sie otworzyc pliku";
	}
	clrscr();
	gotoxy(1, 1);
	labirynt_2D_wyswietlanie(labirynt, nazwa_lab_nowy);
	fclose(labirynt);
}
