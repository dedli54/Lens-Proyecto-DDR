#pragma once
#include "DimPos.h"

#pragma once
//Variables;
enum Arrows {
	L, R, U, D
};

//Variables constantes
/* Variable constante para calcular el ancho de la ventana */
const int ANCHO_VENTANA = 800;

/* Variable constante para calcular el alto de la ventana */
const int ALTO_VENTANA = 640;

/* Variable constante que define la cantidad de bytes por pixel, usada en las operaciones de desplegar sprites/imagenes en pantalla */
const int BPP = 4;

// Score

int actualScore;
int maxScore;
void scoreCmp();
int createNumber(int, int, int, int);
void getMaxScore();
void saveMaxScore();

// Random
int iMin = 1;
int iMax = 25;
int random(int, int);

//Mapa


//int mapArr[] = { L, R, U, D,L, R, U, D,L, R, U, D };
int mapTimer = 0;
int mapCont;
int arrowCount = 0;
int wasClicked = 0;
int goldArrow;

//Validaciones
bool isMenu = true;
bool exitProgram = false;
bool gameOver = false;
bool gameFinished = false;
bool gamePaused = false;
bool showFinishOnce = false;
bool showGameOv = false;
bool savedScore = false;

// Numeros en pantalla
const int hpTotal = 1000;
int Hp = hpTotal;
int indiMilS = 0, indiCentS = 0, indiDecS = 0, indiUnitsS = 0; // Score
int indiDecC = 0, indiUnitsC = 0;							   // Combo

// Timer de cada flecha
int timerL = 10;
int timerR = 10;
int timerD = 10;
int timerU = 10;

// Velocidad de las flechas
const int Velocidad = 9;

// Funciones
void detectColls();

void updateCombo();
void updateScore(bool);
void hpBar();

void initPosition(char);
void initGold(char);

void loadMap();

void randomNumber();



//borrar todo de aqui a main



class Base {
public:

	int arrowID;
	
	bool collDetected = false;

	bool Draw = false;
	bool Animation = false;
	bool isGold = false;

	int spriteSize;
	int indiSprite;
	
	int Timer;

	unsigned char* ptr;
	DIMENSION dmn;
	POSITION pos;

	void Dibuja(int* buffer) {

		int* iPtr = (int*)this->ptr;
		int index = this->indiSprite;
		int iSize = this->spriteSize;

		int anchoV = ANCHO_VENTANA;
		int altoV = ALTO_VENTANA;
		int bpp = BPP;

		int w = this->dmn.ANCHO;
		int h = this->dmn.ALTO;
		int x = this->pos.X;
		int y = this->pos.Y;


		// ASM
		__asm {
			cld

			mov esi, iPtr // :: aca ::
			mov edi, buffer

			mov eax, x
			mul BPP
			add edi, eax
			mov eax, anchoV
			mul BPP
			mul y
			add edi, eax


			mov eax, iSize        // :: aca ::
			mul BPP
			mul  index // :: aca ::
			add esi, eax

			xor ecx, ecx
			mov ecx, h    //asigno mi altura a ecx

			repetir :
			push ecx     //guardo en mi pila ecx
				mov ecx, iSize // :: aca ::

				//PARA QUITAR EL FONDO ROJO
				muestra :
				mov eax, [esi]         //mi imagen la paso a eax
				cmp eax, 0FFFF00D6h    //comparo lo rojo 
				je color1   //si es igual salta a color1
				/*or eax, coloresdif*/
				mov[edi], eax
				color1 :
			add esi, BPP   //incrementa y pasa el pixel rojo
				add edi, BPP
				loop muestra

				//suma el total para luego restar y posicionarlo en la linea de abajo         
				mov eax, anchoV
				mul BPP

				add edi, eax
				mov eax, iSize // :: aca ::
				mul BPP
				sub edi, eax


				//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
				mov eax, w
				mul BPP

				add esi, eax
				mov eax, iSize // :: aca ::
				mul BPP
				sub esi, eax

				pop ecx
				loop repetir
		}
	}
};


class Arrow : public Base {
public:

	void setUp(bool Gold) {
		arrowID = U;
		pos.X = 444;
		pos.Y = 392;
		spriteSize = 57;
		isGold = Gold;
	}

	void setDown(bool Gold) {
		arrowID = D;
		pos.X = 511;
		pos.Y = 392;
		spriteSize = 57;
		isGold = Gold;
	}

	void setLeft(bool Gold) {
		arrowID = L;
		pos.X = 366;
		pos.Y = 392;
		spriteSize = 65;
		isGold = Gold;
	}

	void setRight(bool Gold) {
		arrowID = R;
		pos.X = 581;
		pos.Y = 392;
		spriteSize = 65;
		isGold = Gold;
	}

	Arrow() {
		Draw = false;
		Animation = false;
		isGold = false;
		collDetected = false;

		indiSprite = 0;
		Timer = 10;
	}
	~Arrow() {

	}
};
