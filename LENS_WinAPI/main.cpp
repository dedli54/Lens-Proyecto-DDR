#define WIN32_LEAN_AND_MEAN //No agrega librerías que no se vayan a utilizar

#include <Windows.h>
#include <stdio.h>
#include <dinput.h>
#include <gdiplus.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <random>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include "Arrow.h"
#include "DimPos.h"
#include "gameMap.h"
#include "GamePadRR.h"
#include <cstdlib> 
#include <ctime>  
//#define mapaSize 10




std::random_device rd;
std::mt19937 gen(rd());

using namespace Gdiplus;
using namespace std;

fstream file;

/* Variable constante que define el intervalo del contador o timer en milisegundos, 
	con cada TICK del contador se ejecuta el codigo dentro del case WM_TIMER en la funcion WndProc */
const int TICK = 100;

/* Variables constantes de los colores primarios de un pixel de 32 bits */
const unsigned int BLUE = 0xFF0000FF;
const unsigned int GREEN = 0xFF00FF00;
const unsigned int RED = 0xFFFF0000;


/* Estructura con la enumeracion de algunas teclas.
	Se tiene un objeto o variable del tipo de esta estructura, llamado 'input' 
	que sera para acceder a cada uno de las elementos de la enumeracion; ejemplo:
	input.A para la tecla 'A'.*/
struct Input
{
	enum Keys
	{
		Backspace = 0x08, Tab,
		Clear = 0x0C, Enter,
		Shift = 0x10, Control, Alt,
		Escape = 0x1B,
		Space = 0x20, PageUp, PageDown, End, Home, Left, Up, Right, Down,
		Zero = 0x30, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
		A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		NumPad0 = 0x60, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9, 
		F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
	};
}input /* declaracion del objeto de la estructura Input */;


#pragma region Variables Globales
//Variables Globales
int *ptrBuffer;
unsigned char * ptrBack;
unsigned char * ptrMenu;
unsigned char * ptrPersonaje;
unsigned char * ptrNagat;
unsigned char * ptrLobo;
unsigned char * ptrpelota;
DIMENSION dmnBack, dmnMenu, dmnPersonaje, dmnNagat, dmnLobo,dmnPelota;
POSITION posPer;
POSITION posNagat;
POSITION posLob;
POSITION posPel;
int indiPersonaje = 1;
int indiLobo = 1;
int indiNagat = 1;
int indiPelota=1;
int initfondo;
bool KEYS[256];
int increfondo = 0;    //que incremente el valor del pixel 
int contadorsh = 10;

int coloresdif = 0;
bool mirror;
bool loboD=TRUE;
bool lobomirror = FALSE;
int start;
int scale = 1;



const int Limite = 10;
int mapArr[Limite];


#pragma endregion 


// Variable que contendra el mapa de flechas (el patron)
const int mapaSize = sizeof(mapArr) / sizeof(int);
gameMap gameArr[mapaSize];
HWND globHwnd;

#pragma region Funciones
//Declaracion de funciones
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void MainRender(HWND hWnd);
void menuRender(HWND hWnd);
void Init();
void KeysEvents();
unsigned char * CargaImagen(WCHAR rutaImagen[], DIMENSION * dmn);
POSITION setPosition(int x, int y);


void DibujaFondo(int *buffer, int *imagen, DIMENSION dmn, int incremento);

void DibujaPersonaje(int *buffer, int *personaje, DIMENSION dmn1, POSITION pos1);
void DibujaLobo(int *buffer, int *lobo, DIMENSION dmn2, POSITION pos2);
void DibujaPelota(int *buffer, int *pelota, DIMENSION dmn3, POSITION pos3);

// Animacion
void DibujaNagat(int*, int*, DIMENSION, POSITION);
#pragma endregion

#pragma region Sprites
#pragma region Fondo 2
int indiBack2 = 0;
unsigned char* ptrBack2;
DIMENSION dmnBack2;
POSITION posBack2;
void DibujaBack2(int*, int*, DIMENSION, POSITION);

#pragma endregion

// Static sprites
#pragma region HpBar
int indiHpBar = 0;
unsigned char* ptrHpBar;
DIMENSION dmnHpBar;
POSITION posHpBar;
void DibujaHpBar(int*, int*, DIMENSION, POSITION);
#pragma endregion

#pragma region baseArrows
int indiArrow = 0;
unsigned char* ptrBaseL, *ptrBaseR, *ptrBaseD, *ptrBaseU;
DIMENSION dmnBaseL, dmnBaseR, dmnBaseD, dmnBaseU;
POSITION posBaseL, posBaseR, posBaseD, posBaseU;
void DibujaArrowLR(int*, int*, DIMENSION, POSITION);
void DibujaArrowDU(int*, int*, DIMENSION, POSITION);
#pragma endregion

#pragma region staticText
int indiText = 0;
unsigned char* ptrScoreT, * ptrComboT;
DIMENSION dmnScoreT, dmnComboT;
POSITION posScoreT, posComboT;
void DibujaScoreT(int*, int*, DIMENSION, POSITION);
void DibujaComboT(int*, int*, DIMENSION, POSITION);
#pragma endregion


// Dynamic sprites

#pragma region comboNum
unsigned char* ptrComboU, * ptrComboD;
DIMENSION dmnComboU, dmnComboD;
POSITION posComboU, posComboD;
void DibujaComboN(int, int*, int*, DIMENSION, POSITION);
#pragma endregion

#pragma region scoreNum
unsigned char* ptrScoreU, * ptrScoreD, * ptrScoreC, * ptrScoreM;
DIMENSION dmnScoreU, dmnScoreD, dmnScoreC, dmnScoreM;
POSITION posScoreU, posScoreD, posScoreC, posScoreM;
void DibujaScoreN(int, int*, int*, DIMENSION, POSITION);
#pragma endregion

#pragma region Notes
enum Notes
{
	Perfect = 0,
	Great = 1,
	Good = 2,
	Miss = 3,
	None = 4
};
int indiNotes = None;
unsigned char* ptrNotes;
DIMENSION dmnNotes;
POSITION posNotes;
void DibujaNotes(int*, int*, DIMENSION, POSITION);
#pragma endregion

#pragma endregion



int WINAPI wWinMain(HINSTANCE hInstance, 
					 HINSTANCE hPrevInstance, 
					 PWSTR pCmdLine, 
					 int nCmdShow)
{
	WNDCLASSEX wc;									// Windows Class Structure
	HWND hWnd;
	MSG msg;

	TCHAR szAppName[] = TEXT("MyWinAPIApp");		
	TCHAR szAppTitle[] = TEXT("LENS App");

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance	
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= (HBRUSH) (COLOR_WINDOW + 1);			// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu	
	wc.lpszClassName	= szAppName;							// Set The Class Name
	wc.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);
	
	if (!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,
			L"Fallo al registrar clase (Failed To Register The Window Class).",
			L"ERROR",
			MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}

	hWnd = CreateWindowEx(	
		WS_EX_CLIENTEDGE | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,	// Extended Style For The Window
		szAppName,							// Class Name
		szAppTitle,							// Window Title
		WS_OVERLAPPEDWINDOW |				// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		ANCHO_VENTANA,						// Calculate Window Width
		ALTO_VENTANA,						// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL);								// Pass this class To WM_CREATE								

	if(hWnd == NULL) {
		MessageBox(NULL, 
			L"Error al crear ventana (Window Creation Error).", 
			L"ERROR", 
			MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}
	
	globHwnd = hWnd;
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	Init();
	ShowWindow(hWnd, nCmdShow);
	SetFocus(hWnd);

	SetTimer(hWnd, TICK, TICK, NULL);
	ZeroMemory(&msg, sizeof(MSG));

	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	


	return(int)msg.wParam;
}

/* Funcion tipo Callback para el manejo de los eventos de la ventana. 
	*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uMsg)									// Check For Windows Messages
	{
		case WM_TIMER:

			if(wParam == TICK)
			{
				if (gameOver && !showGameOv){
					showGameOv = true;
					string txt = "Has perdido";
					const char* c = txt.c_str();
					MessageBoxA(hWnd, c, "Game Over", MB_ICONSTOP);
				}

				if (gameFinished && gameArr[mapaSize - 1].arrow.collDetected && !gameArr[mapaSize - 1].arrow.Draw && !showFinishOnce) {
					showFinishOnce = true;
					if (!savedScore) {
						scoreCmp();
						savedScore = true;
					}
					string score = to_string(actualScore);
					string mScore = to_string(maxScore);
					string txt = "Score: " + score + "\n" + "Max score: " + mScore;
					const char* c = txt.c_str();
					MessageBoxA(hWnd, c, "Nivel completado", MB_ICONINFORMATION);
				}

				if (isMenu) 
					menuRender(hWnd);
				else
					MainRender(hWnd);

				if(exitProgram) // Para salir del programa
					DestroyWindow(hWnd);

			}

			break;
		case WM_PAINT:
			{
				HDC hdc; 
				PAINTSTRUCT ps;
				hdc = BeginPaint(hWnd, &ps);

				BITMAP bm;
				HBITMAP h_CMC = CreateBitmap(ANCHO_VENTANA, ALTO_VENTANA, 1, 32, ptrBuffer);
				HDC hdcMem = CreateCompatibleDC(hdc);
				HBITMAP hbmOld = (HBITMAP) SelectObject(hdcMem, h_CMC);
				GetObject(h_CMC, sizeof(bm), &bm);

				BitBlt(hdc, 0, 0, ANCHO_VENTANA, ALTO_VENTANA, hdcMem, 0, 0, SRCCOPY);

				DeleteObject(h_CMC);
				SelectObject(hdcMem, hbmOld);
				DeleteDC(hdcMem);
				DeleteObject(hbmOld);
			}
			break;		
		case WM_KEYDOWN:							
			{
				KEYS[ wParam ] = true;
			}
			break;
		case WM_KEYUP:
			{
				KEYS[ wParam ] = false;
			}
			break;
		case WM_CLOSE: 
			{
				DestroyWindow(hWnd);
			}
			break;
		case WM_DESTROY: //Send A Quit Message
			{
				KillTimer(hWnd, TICK);
				PostQuitMessage(0);
			}
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

/* Funcion usada para la inicializacion de variables y reserva de espacio en memoria.
	*/
void Init()
{

	for (int i = 0; i < Limite; i++)
	{
		srand(time(nullptr));
		mapArr[i] = rand() % 4;
	};

	posNagat.X = 20;
	posNagat.Y = 280;

	posHpBar.X = 105;
	posHpBar.Y = 97;


	// ::::: BASE ARROW ::::: //
	posBaseL.X = 366;
	posBaseL.Y = 212;

	posBaseU.X = 444;
	posBaseU.Y = 212;

	posBaseD.X = 511;
	posBaseD.Y = 212;

	posBaseR.X = 581;
	posBaseR.Y = 212;


	// ::::: FONDO 2 ::::: //
	posBack2.X = 312;
	posBack2.Y = 87;



	// ::::: SCORE & COMBO TEXT ::::: //
	posScoreT.X = 400;
	posScoreT.Y = 103;

	posComboT.X = 121;
	posComboT.Y = 160;


	// ::::: COMBO NUMBERS ::::: //
	posComboD.X = 151;
	posComboD.Y = 200;

	posComboU.X = 198;
	posComboU.Y = 200;


	// ::::: SCORE NUMBERS ::::: //
	posScoreM.X = 513;
	posScoreM.Y = 103;

	posScoreC.X = 536;
	posScoreC.Y = 103;

	posScoreD.X = 559;
	posScoreD.Y = 103;

	posScoreU.X = 584;
	posScoreU.Y = 103;


	// ::::: NOTES ::::: //
	posNotes.X = 450;
	posNotes.Y = 180;


	for(int i = 0; i < 256; i++)
	{
		KEYS[i] = false;
	}

	//Inicializar el puntero tipo int 'ptrBuffer' que contiene la direccion inicial  del area de memoria reservada para el despliegue de sprites/imagenes.
	ptrBuffer = new int[ANCHO_VENTANA * ALTO_VENTANA];

	//Inicializar el puntero tipo unsigned char 'ptrBack' que contiene la direccion inicial en memoria del arreglo de pixeles de la imagen especificada en el primer parametro
	//y en la variable dmnBack de tipo DIMENSION* estan los valores de ANCHO y ALTO de la imagen.
	ptrBack = CargaImagen(TEXT("fondo.png"), &dmnBack); //puntero a la imagen
	ptrBack2 = CargaImagen(TEXT("fondo2.png"), &dmnBack2);
	ptrMenu = CargaImagen(TEXT("Menu.png"), &dmnMenu);

	// Plantilla
	//ptrPersonaje = CargaImagen(TEXT("personaje.png"), &dmnPersonaje);   //puntero a mi personaje sprite
	//ptrLobo = CargaImagen(TEXT("lobo.png"), &dmnLobo);
	//ptrpelota = CargaImagen(TEXT("pelotita.png"),&dmnPelota);


	// ::::: ANIMACION ::::: //
	ptrHpBar = CargaImagen(TEXT("hpBar.png"), &dmnHpBar);
	ptrNagat = CargaImagen(TEXT("test.png"), &dmnNagat);

	// ::::: BASE FLECHAS ::::: //
	ptrBaseR = CargaImagen(TEXT("bRight.png"), &dmnBaseR);
	ptrBaseD = CargaImagen(TEXT("bDown.png"), &dmnBaseD);
	ptrBaseU = CargaImagen(TEXT("bUp.png"), &dmnBaseU);
	ptrBaseL = CargaImagen(TEXT("bLeft.png"), &dmnBaseL);

	// ::::: SCORE & COMBO TEXT ::::: //
	ptrScoreT = CargaImagen(TEXT("tScore.png"), &dmnScoreT);
	ptrComboT = CargaImagen(TEXT("tCombo.png"), &dmnComboT);

	// ::::: COMBO NUMBERS ::::: //
	ptrComboD = CargaImagen(TEXT("cNumber.png"), &dmnComboD);
	ptrComboU = CargaImagen(TEXT("cNumber.png"), &dmnComboU);

	// ::::: SCORE NUMBERS ::::: //
	ptrScoreM = CargaImagen(TEXT("sNumber.png"), &dmnScoreM);
	ptrScoreC = CargaImagen(TEXT("sNumber.png"), &dmnScoreC);
	ptrScoreD = CargaImagen(TEXT("sNumber.png"), &dmnScoreD);
	ptrScoreU = CargaImagen(TEXT("sNumber.png"), &dmnScoreU);

	// ::::: NOTES ::::: //
	ptrNotes = CargaImagen(TEXT("notes.png"), &dmnNotes);
	
}

/* Funcion principal. Encargada de hacer el redibujado en pantalla cada intervalo (o "tick") del timer que se haya creado.
	@param hWnd. Manejador de la ventana.
	*/
void MainRender(HWND hWnd)
{

	KeysEvents();

	DibujaFondo(ptrBuffer, (int*)ptrBack, dmnBack, increfondo);   //recibe lo de incremento


	// ::::: BASE FLECHAS ::::: //
	DibujaArrowLR(ptrBuffer, (int*)ptrBaseL, dmnBaseL, posBaseL);
	DibujaArrowDU(ptrBuffer, (int*)ptrBaseU, dmnBaseU, posBaseU);
	DibujaArrowDU(ptrBuffer, (int*)ptrBaseD, dmnBaseD, posBaseD);
	DibujaArrowLR(ptrBuffer, (int*)ptrBaseR, dmnBaseR, posBaseR);

	//// ::::: FLECHAS ::::: //
	for (int i = 0; i < mapaSize; i++)
	{
		if (gameArr[i].arrow.Draw)
			gameArr[i].arrow.Dibuja(ptrBuffer);
	}

	posScoreD.X = 559;
	posScoreD.Y = 103;

	DibujaBack2(ptrBuffer, (int*)ptrBack2, dmnBack2, posBack2);

	DibujaHpBar(ptrBuffer, (int*)ptrHpBar, dmnHpBar, posHpBar);


	// ::::: ANIMACION ::::: //
	DibujaNagat(ptrBuffer, (int*)ptrNagat, dmnNagat, posNagat);


	// ::::: SCORE & COMBO TEXT ::::: //
	DibujaScoreT(ptrBuffer, (int*)ptrScoreT, dmnScoreT, posScoreT);
	DibujaComboT(ptrBuffer, (int*)ptrComboT, dmnComboT, posComboT);

	// ::::: COMBO NUMBERS ::::: //
	DibujaComboN(indiDecC, ptrBuffer, (int*)ptrComboD, dmnComboD, posComboD);
	DibujaComboN(indiUnitsC, ptrBuffer, (int*)ptrComboU, dmnComboU, posComboU);

	// ::::: SCORE NUMBERS ::::: //
	DibujaScoreN(indiMilS, ptrBuffer, (int*)ptrScoreM, dmnScoreM , posScoreM);
	DibujaScoreN(indiCentS, ptrBuffer, (int*)ptrScoreC, dmnScoreC, posScoreC);
	DibujaScoreN(indiDecS, ptrBuffer, (int*)ptrScoreD, dmnScoreD, posScoreD);
	DibujaScoreN(indiUnitsS, ptrBuffer, (int*)ptrScoreU, dmnScoreU, posScoreU);

	// ::::: NOTES ::::: //
	if(posNotes.Draw)
		DibujaNotes(ptrBuffer, (int*)ptrNotes, dmnNotes, posNotes);

	

	//Funciones que deberan estar el final de la funcion de Render.
	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}


void menuRender(HWND hWnd) {

	KeysEvents();

	DibujaFondo(ptrBuffer, (int*)ptrMenu, dmnMenu, increfondo);   //recibe lo de incremento

	//Funciones que deberan estar el final de la funcion de Render.
	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}


/* Funcion que regresa la posicion del sprite en pantalla.
	@param x. Coordenada X en la ventana.
	@param y. Coordenada Y en la ventana.
	*/
POSITION setPosition(int x, int y) {
	POSITION p;
	p.X = x;
	p.Y = y;
	return p;
}

/* Funcion para manejar eventos del teclado dependiendo de la(s) tecla(s) que se haya(n) presionado.
	*/
void KeysEvents() 
{

	if (isMenu) {
		if (KEYS[input.Space]) {
			isMenu = false;
		}
			return;
	}


	if (KEYS[input.Escape]) {
		exitProgram = true;
		return;
	}


	if (gamePaused && !gameFinished) {
		if (KEYS[input.P]) {
			gamePaused = false;
		}
		return;
	}
	else {
		if (KEYS[input.P])
			gamePaused = true;
	}

	if (gameOver)
		return;

	if (gameFinished && gameArr[mapaSize - 1].arrow.collDetected && !gameArr[mapaSize - 1].arrow.Draw) {
		return;
	}



	// ::::: MAPA ::::: //
	mapTimer++;
		if (mapTimer == 7) {
			gameArr->loadMap();
			mapTimer = 0;
		}

	// ::::: ANIMACION ::::: //
	indiNagat++;
	indiNagat = indiNagat >= 5 ? 0 : indiNagat;

	// ::::: KEYS USUARIO ::::: //
	if (KEYS[input.A] || KEYS[input.Left]/*|| XINPUT_GAMEPAD_DPAD_LEFT*/)
	{

		wasClicked = L;
		detectColls();
	}

	if (KEYS[input.D] || KEYS[input.Right] /*|| XINPUT_GAMEPAD_DPAD_RIGHT*/)
	{

		wasClicked = R;
		detectColls();
	}

	if (KEYS[input.W] || KEYS[input.Up] || KEYS[input.H]/*|| XINPUT_GAMEPAD_DPAD_UP*/)
	{

		wasClicked = U;
		detectColls();

	}

	if (KEYS[input.S] || KEYS[input.Down]/*|| XINPUT_GAMEPAD_DPAD_DOWN*/)
	{
		wasClicked = D;
		detectColls();
	}

	// ::::: ANIMACION DE FLECHAS ::::: //

	for (int i = 0; i < mapaSize; i++)
	{
		if (gameArr[i].arrow.Animation) {
			gameArr[i].arrow.pos.Y -= Velocidad;
			if (gameArr[i].arrow.pos.Y <= 100 && !gameArr[i].arrow.collDetected) {
				gameArr[i].arrow.Animation = false;
				gameArr[i].arrow.Draw = false;
				gameArr[i].arrow.collDetected = true;
				posNotes.Draw = true;
				indiNotes = Miss;
				hpBar();
				updateCombo();
			}
		}
		else if (gameArr[i].arrow.Draw) {
			gameArr[i].arrow.Timer++;
			if (gameArr[i].arrow.Timer++ == 15) {
				gameArr[i].arrow.Draw = false;
			}
		}
		
	}


	
}

/* Funcion para cargar imagenes y obtener un puntero al area de memoria reservada para la misma.
	@param rutaImagen.			Nombre o ruta de la imagen a cargar en memoria.
	@return unsigned char *.	Direccion base de la imagen.
	*/
unsigned char * CargaImagen(WCHAR rutaImagen[], DIMENSION * dmn)
{
	unsigned char * ptrImagen;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR  gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Bitmap *bitmap=new Bitmap(rutaImagen);
	BitmapData *bitmapData=new BitmapData;

	dmn->ANCHO = bitmap->GetWidth();
	dmn->ALTO = bitmap->GetHeight();

	Rect rect(0, 0, dmn->ANCHO, dmn->ALTO);

	//Reservamos espacio en memoria para la imagen
	bitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppRGB, bitmapData);

	//"pixels" es el puntero al area de memoria que ocupa la imagen
	unsigned char* pixels = (unsigned char*)bitmapData->Scan0;

	//"tamaño" lo usaremos para reservar los bytes que necesita la imagen. 
	//Para calcular la cantidad de bytes total necesitamos multiplicamos el area de la imagen * 4. 
	//Se multiplica por 4 debido a que cada pixel ocupa 4 bytes de memoria. Noten el 3er parametro de la funcion LockBits, dos lineas de codigo arriba.
	//PixelFormat32bppARGB -> Specifies that the format is 32 bits per pixel; 8 bits each are used for the alpha, red, green, and blue components.
	//Mas info: https://msdn.microsoft.com/en-us/library/system.drawing.imaging.pixelformat(v=vs.110).aspx
	int tamaño;
	tamaño = dmn->ANCHO * dmn->ALTO * 4;
	//hagamos un try de la reserva de memoria
	try
	{
		ptrImagen = new unsigned char [tamaño]; 
	}
	catch(...)
	{
		return NULL;
	}

	//Después de este for, ptrImagen contiene la direccion en memoria de la imagen.
	for(int i=0, j=tamaño; i < j; i++)
	{
		ptrImagen[i]=pixels[i];
	}

	//Es necesario liberar el espacio en memoria, de lo contrario marcaria una excepcion de no hay espacio de memoria suficiente.
	bitmap->UnlockBits(bitmapData);
	delete bitmapData;
	delete bitmap;
	  
	GdiplusShutdown(gdiplusToken);

	return ptrImagen;
}




#pragma region Game

void gameMap::loadMap() {
	if (gameFinished)
		return;

	bool isGold = false; // Aleatoridad
	int Aleatorio = random(iMin, iMax);
	if(Aleatorio >= 7 && Aleatorio <= 10)
		isGold = true;

	Arrow arrow;
	arrow.Draw = arrow.Animation = true;

	switch (mapArr[mapCont])
	{

	case L: {
		arrow.setLeft(isGold);
		if(isGold)
			arrow.ptr = CargaImagen(TEXT("gLeft.png"), &arrow.dmn);
		else 
			arrow.ptr = CargaImagen(TEXT("aLeft.png"), &arrow.dmn);

	} break;
	case R: {
		arrow.setRight(isGold);
		if (isGold)
			arrow.ptr = CargaImagen(TEXT("gRight.png"), &arrow.dmn);
		else
			arrow.ptr = CargaImagen(TEXT("aRight.png"), &arrow.dmn);

	} break;
	case U: {
		arrow.setUp(isGold);
		if (isGold)
			arrow.ptr = CargaImagen(TEXT("gUp.png"), &arrow.dmn);
		else
			arrow.ptr = CargaImagen(TEXT("aUp.png"), &arrow.dmn);

	} break;
	case D: {
		arrow.setDown(isGold);
		if (isGold)
			arrow.ptr = CargaImagen(TEXT("gDown.png"), &arrow.dmn);
		else
			arrow.ptr = CargaImagen(TEXT("aDown.png"), &arrow.dmn);

	} break;

	default:
		break;
	}

	gameArr[mapCont].arrow = arrow;

	if (mapCont < mapaSize) {
		mapCont++; // Siguiente flecha
		arrowCount++;
	}
	else 
		gameFinished = true;

}



int random(int low, int high)
{
	std::uniform_int_distribution<> dist(low, high);
	return dist(gen);
}


void hpBar() {
	if (Hp < hpTotal) {   // Acierto
		// Sumando en caso de acertar
		if (indiNotes == Perfect)
			Hp += 100;
		if (indiNotes == Great)
			Hp += 50;
		if (indiNotes == Good)
			Hp += 25;

		// Si sobrepasa los 1000
		if (Hp > hpTotal) {
			int extra = Hp - hpTotal;
			Hp -= extra;
		}
	}

	if (indiNotes == Miss) { // Fallo
		Hp -= 100;
		if (Hp <= 0) {
			indiHpBar = 4;
			gameOver = true;
		}
	}

	// Porcentaje
	float Porcentaje = (Hp * 100) / hpTotal;
	if (Porcentaje > 75 && Porcentaje <= 100)
		indiHpBar = 0; // 100%
	if (Porcentaje > 50 && Porcentaje <= 75)
		indiHpBar = 1; // 75%
	if (Porcentaje > 25 && Porcentaje <= 50)
		indiHpBar = 2; // 50%
	if (Porcentaje > 0 && Porcentaje <= 25)
		indiHpBar = 3; // 25%

}



void updateCombo() {

	if (indiNotes == Miss) {
		indiDecC = indiUnitsC = 0;
		return;
	}

	if (indiUnitsC < 9) {
		indiUnitsC++;
	}
	else {
		indiUnitsC = 0;
		if(indiDecC < 9)
			indiDecC++;
	}

}


void updateScore(bool isGold) {

	if (indiNotes == Miss)
		return;

	int Dec, Unit;

	switch (indiNotes)
	{
	case Perfect: {
		if (isGold) {
			Dec = 7;
			Unit = 5;
		}
		else {
			Dec = 5;
			Unit = 0;
		}
	} break;
	case Great: {
		if (isGold) {
			Dec = 5;
			Unit = 5;
		}
		else {
			Dec = 3;
			Unit = 0;
		}
	} break;
	case Good: {
		if (isGold) {
			Dec = 3;
			Unit = 5;
		}
		else {
			Dec = 1;
			Unit = 0;
		}
	} break;
	default:
		break;
	}

	int sumaU = Unit + indiUnitsS;
	int sumaD = Dec + indiDecS;
	int sumaC = indiCentS;
	int sumaM = indiMilS;

	if (sumaU <= 9) { // Unidades
		indiUnitsS = sumaU;
	}
	else {
		if (sumaU == 10)
			indiUnitsS = 0;
		else {
			sumaU -= 10;
			indiUnitsS = sumaU;
		}
		sumaD++; // Sumandole a la decena
	}

	if (sumaD <= 9) { // Decenas
		indiDecS = sumaD;
	}
	else {
		if (sumaD == 10)
			indiDecS = 0;
		else {
			sumaD -= 10;
			indiDecS = sumaD;
		}
		sumaC++; // Sumandole a la centena
	}

	if (sumaC <= 9) { // Centena
		indiCentS = sumaC;
	}
	else {
		if (sumaC == 10)
			indiCentS = 0;
		else {
			sumaC -= 10;
			indiCentS = sumaC;
		}
		sumaM++; // Sumandole a la milesima
	}

	if (sumaM <= 9) { // Milesima
		indiMilS = sumaM;
	}

}



void detectColls() {

	posNotes.Draw = true; // Para dibujar el puntaje de la Nota (Ya que no se veia)

	int i = 0;
	bool isFound = false;
	while (i < mapaSize)
	{
		if (!gameArr[i].arrow.collDetected && wasClicked == gameArr[i].arrow.arrowID && gameArr[i].arrow.Animation) {
			isFound = true;
			break;
		}
		i++;
	}

	if (!isFound)
		return;

	// Detectar colision en Y
	int Base, Arrow, Distance;
	if (gameArr[i].arrow.arrowID == L) 
		Base = posBaseL.Y + dmnBaseL.ALTO;
	if (gameArr[i].arrow.arrowID == R)
		Base = posBaseR.Y + dmnBaseR.ALTO;
	if (gameArr[i].arrow.arrowID == D)
		Base = posBaseD.Y + dmnBaseD.ALTO;
	if (gameArr[i].arrow.arrowID == U)
		Base = posBaseU.Y + dmnBaseU.ALTO;

	Arrow = gameArr[i].arrow.pos.Y + gameArr[i].arrow.dmn.ALTO;;
	Distance = Arrow - Base;
	gameArr[i].arrow.collDetected = true;
	gameArr[i].arrow.Animation = false;

	if (Distance >= -18 && Distance <= 18) {
		if (Distance >= -3 && Distance <= 3)
			indiNotes = Perfect;
		else if (Distance >= -10 && Distance <= 10)
			indiNotes = Great;
		else
			indiNotes = Good;
	}
	else {
		indiNotes = Miss;
	}





	hpBar();
	updateCombo();
	updateScore(gameArr[i].arrow.isGold);
}



void scoreCmp() {
	getMaxScore();
	actualScore = createNumber(indiMilS, indiCentS, indiDecS, indiUnitsS);
	if (actualScore > maxScore)
		maxScore = actualScore;
	saveMaxScore();
}

int createNumber(int iMil, int iCent, int iDec, int iUnit) {

	stringstream ssMil, ssCent, ssDec, ssUnit;
	ssMil << iMil;
	ssCent << iCent;
	ssDec << iDec;
	ssUnit << iUnit;
	string sScore = ssMil.str() + ssCent.str() + ssDec.str() + ssUnit.str();

	return atoi(sScore.c_str());

}

void getMaxScore() {
	file.open("maxScore.txt", ios::in);
	if (file.is_open()) {
		file >> maxScore;
		file.close();
	}
	else
		return;

}

void saveMaxScore() {
	file.open("maxScore.txt", ios::out | ios::trunc);
	if (file.is_open()) {
		file << maxScore;
		file.close();
	}
	else
		return;
}




#pragma endregion




#pragma region LENS_CODE
void DibujaFondo(int * buffer, int * imagen, DIMENSION dmn, int incremento) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	__asm {

		cld  
		mov esi, imagen   
		mov edi, buffer   

		mov ecx, 600

		//mov eax, incremento   //mi parametro para incrementar los pixeles, lo mando a eax
		//mul BPP               //multiplico por los bytes
		//add esi, eax          //se lo agrego a mi imagen para que se recorra

	PonerA :
			push ecx  //el alto lo pongo al inicio de mi linea
			mov ecx, 800 //recorre el ancho

			PonerL :
				mov eax, [esi]     //mi imagen la paso a eax
				mov[edi], eax      //eax lo pongo en mi ventana  (paso mi imagen a la ventana)
				add edi, BPP                     //incremento mi pixel 4 bytes
				add esi, BPP
			loop PonerL


			mov eax, 800                    //ancho de mi imagen
			mul BPP                          //multiplico por 4 bytes
			add esi, eax                     //incremento para que me pase a la linea de abajo
			mov eax, 800
			mul BPP
			sub esi, eax                     //resto para posicionarlo al principio de mi linea
			pop ecx   //para sacar el alto y restarle uno
			
	loop PonerA

		//; Inicializar registros indices
		//	; con la direccion inicial en memoria de la imagen a desplegar(fuente->imagen->registro indice fuente->ESI)
		//	; y del area de memoria reservada para el despliegue(destino->buffer->registro indice destino->EDI)
		//	; mov esi, imagen
		//	; mov edi, buffer
		//	; Cargar la direccion en memoria de la variable 'dmn' el registro base(EBX). 'dmn' es la estructura con el ANCHO y ALTO.
		//	; lea ebx, dmn
		//;Transferir o copiar 4 bytes en memoria a partir de la direccion en memoria especificada por EBX; 
		//; son 4 bytes porque el operando destino es de 4 bytes en este caso es el registro acumulador(EAX).
		//	; Es decir, EBX es un puntero a 'dmn' donde 'dmn' es de 8 bytes(4Bytes de la variable ANCHO y los siguientes 4Bytes de la variable ALTO),
		//	; entonces EAX = ANCHO
		//	; mov eax, [ebx]
		//	; sumar 4 al registro puntero(EBX) para apuntar al siguiente elemento de la estructura DIMENSION el cual es ALTO y vendria siendo el otro factor de la multiplicacion
		//	; add ebx, 4
		//	; Especificar el otro factor en la instruccion de multiplicacion que son los siguientes 4Bytes a partir de la direccion especificada por EBX(variable ALTO de DIMENSION)
		//	; Se multplican ANCHO(= EAX) * ALTO(= dword ptr[ebx])
		//	; con 'dword ptr [ebx]' estamos indicando que EBX se comporte como puntero tipo DWORD,
		//	; es decir, se esta especificando que se desean los 4Bytes (4Bytes = 1Dword) a partir de la direccion en memoria especificada por el registro EBX
		//	; el cual contiene la direccion inicial de la variable ALTO en la estructura DIMENSION.
		//	; mul dword ptr[ebx]
		//	; el resultado de la multiplicacion anterior es del doble del tamano de los factores, en este caso los factores son de 4Bytes por lo que el resultado es de 8Bytes
		//	; y se encuentra en EDX : EAX donde EDX contiene los 4Bytes de mayor peso del producto de la multiplicacion y EAX contiene los 4Bytes de menor peso.
		//	; Como el producto de la multiplicacion sabemos y esperamos que es un valor con un tamano menor a 4Bytes entonces dicho resultado esta en EAX
		//	; y con la siguiente instruccion se lo copiamos al registro contador(ECX)
		//	; para posteriormente hacer el ciclo / bucle de leer cada pixel de la imagen en memoria y pasarlo al buffer(area de mem.reservada para el despliegue)
		//	; mov ecx, eax
		//;poner codigo para el bucle de leer pixel por pixel y transferirlo al buffer.
	}
}





void DibujaNagat(int* buffer, int* nagat, DIMENSION dmn, POSITION pos)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = posNagat.X;
	int y = posNagat.Y;

	// ASM
	__asm {
		cld

		mov esi, nagat
		mov edi, buffer

		mov eax, x
		mul BPP
		add edi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		mul y
		add edi, eax


		mov eax, 290        //porque es la cantidad de pixeles de cada cuadrito
		mul BPP
		mul  indiNagat
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 290

			//PARA QUITAR EL FONDO ROJO
			muestra :
			mov eax, [esi]         //mi imagen la paso a eax
			cmp eax, 0FFFFFFFFh    //comparo lo rojo 
			je color1   //si es igual salta a color1
			/*or eax, coloresdif*/
			mov[edi], eax
			color1 :
		add esi, BPP   //incrementa y pasa el pixel rojo
			add edi, BPP
			loop muestra

			//suma el total para luego restar y posicionarlo en la linea de abajo         
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 290
			mul BPP
			sub edi, eax

			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 290
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}
	

}

void DibujaHpBar(int* buffer, int* hpBar, DIMENSION dmn, POSITION pos) {

	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	// ASM
	__asm {
		cld

		mov esi, hpBar
		mov edi, buffer

		mov eax, x
		mul BPP
		add edi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		mul y
		add edi, eax


		mov eax, 181        //porque es la cantidad de pixeles de cada cuadrito
		mul BPP
		mul  indiHpBar
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 181

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
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 181
			mul BPP
			sub edi, eax


			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 181
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}

}

void DibujaArrowLR(int* buffer, int* arrow, DIMENSION dmn, POSITION pos) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	// ASM
	__asm {
		cld

		mov esi, arrow // :: aca ::
		mov edi, buffer

		mov eax, x
		mul BPP
		add edi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		mul y
		add edi, eax


		mov eax, 65        // :: aca ::
		mul BPP
		mul  indiArrow // :: aca ::
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 65 // :: aca ::

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
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 65 // :: aca ::
			mul BPP
			sub edi, eax


			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 65 // :: aca ::
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}
}

void DibujaArrowDU(int* buffer, int* arrow, DIMENSION dmn, POSITION pos) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	// ASM
	__asm {
		cld

		mov esi, arrow // :: aca ::
		mov edi, buffer

		mov eax, x
		mul BPP
		add edi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		mul y
		add edi, eax


		mov eax, 57        // :: aca ::
		mul BPP
		mul  indiArrow // :: aca ::
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 57 // :: aca ::

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
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 57 // :: aca ::
			mul BPP
			sub edi, eax


			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 57 // :: aca ::
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}
}

void DibujaScoreT(int* buffer, int* texto, DIMENSION dmn, POSITION pos) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	// ASM
	__asm {
		cld

		mov esi, texto // :: aca ::
		mov edi, buffer

		mov eax, x
		mul BPP
		add edi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		mul y
		add edi, eax


		mov eax, 112        // :: aca ::
		mul BPP
		mul  indiText // :: aca ::
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 112 // :: aca ::

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
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 112 // :: aca ::
			mul BPP
			sub edi, eax


			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 112 // :: aca ::
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}
}


void DibujaBack2(int* buffer, int* back, DIMENSION dmn, POSITION pos) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	// ASM
	__asm {
		cld

		mov esi, back // :: aca ::
		mov edi, buffer

		mov eax, x
		mul BPP
		add edi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		mul y
		add edi, eax


		mov eax, 386        // :: aca ::
		mul BPP
		mul  indiBack2 // :: aca ::
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 386 // :: aca ::

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
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 386 // :: aca ::
			mul BPP
			sub edi, eax


			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 386 // :: aca ::
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}
}

void DibujaComboT(int* buffer, int* texto, DIMENSION dmn, POSITION pos) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	// ASM
	__asm {
		cld

		mov esi, texto // :: aca ::
		mov edi, buffer

		mov eax, x
		mul BPP
		add edi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		mul y
		add edi, eax


		mov eax, 147        // :: aca ::
		mul BPP
		mul  indiText // :: aca ::
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 147 // :: aca ::

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
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 147 // :: aca ::
			mul BPP
			sub edi, eax


			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 147 // :: aca ::
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}
}

void DibujaComboN(int indiComb, int* buffer, int* combo, DIMENSION dmn, POSITION pos) {

	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	// ASM
	__asm {
		cld

		mov esi, combo // :: aca ::
		mov edi, buffer

		mov eax, x
		mul BPP
		add edi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		mul y
		add edi, eax


		mov eax, 47        // :: aca ::
		mul BPP
		mul  indiComb // :: aca ::
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 47 // :: aca ::

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
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 47 // :: aca ::
			mul BPP
			sub edi, eax


			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 47 // :: aca ::
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}

}




void DibujaScoreN(int indiScore, int* buffer, int* score, DIMENSION dmn, POSITION pos) {

	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	// ASM
	__asm {
		cld

		mov esi, score // :: aca ::
		mov edi, buffer

		mov eax, x
		mul BPP
		add edi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		mul y
		add edi, eax


		mov eax, 25        // :: aca ::
		mul BPP
		mul  indiScore // :: aca ::
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 25 // :: aca ::

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
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 25 // :: aca ::
			mul BPP
			sub edi, eax


			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 25 // :: aca ::
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}

}

void DibujaNotes(int* buffer, int* note, DIMENSION dmn, POSITION pos) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	// ASM
	__asm {
		cld

		mov esi, note // :: aca ::
		mov edi, buffer

		mov eax, x
		mul BPP
		add edi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		mul y
		add edi, eax


		mov eax, 115        // :: aca ::
		mul BPP
		mul  indiNotes // :: aca ::
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 115 // :: aca ::

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
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 115 // :: aca ::
			mul BPP
			sub edi, eax


			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 115 // :: aca ::
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}
}



void DibujaPelota(int * buffer, int * pelota, DIMENSION dmn3, POSITION pos3)
{
	int w = dmn3.ANCHO;
	int h = dmn3.ALTO-1;
	int posX = pos3.X;
	int posY = pos3.Y;
	int total = 0;
	int total2 = 0;

	if (indiPelota == 0) {
		indiPelota++;
		__asm
		{
			cld
			mov esi, pelota
			mov edi, buffer

			mov eax, posX
			mul BPP
			add edi, eax
			mov eax, ANCHO_VENTANA
			mul BPP
			mul posY
			add edi, eax

			mov eax, w
			mul BPP
			add esi, eax

			xor ecx, ecx
			mov ecx, h

			repite2 :

			push ecx
				mov ecx, w


				mostrar2 :
				mov eax, [esi]
				cmp eax, 0FFED1C24h
				je Color2
				mov[edi], eax


				Color2 :

				add esi, BPP
				add edi, BPP


				loop mostrar2
				mov eax, ANCHO_VENTANA
				mul BPP

				add edi, eax
				MOV EAX, w
				MUL BPP
				SUB EDI, EAX

				mov eax, w
				mul BPP

				add esi, eax
				mov eax, w
				mul BPP
				sub esi, eax



				pop ecx

				loop repite2
		}
	}else if(indiPelota==1){
		indiPelota++;
		__asm{
		cld

			mov esi, pelota
			mov edi, buffer

			
			mov eax, w
			mul BPP
			mul h
			add esi, eax
			mov total, esi

			mov eax, posX
			mul BPP
			add edi, eax
			mov eax, ANCHO_VENTANA
			mul BPP
			mul posY
			add edi, eax

			xor ecx, ecx
			mov ecx, w


			repite :

		push ecx
			mov ecx, h


			mostrar :

		mov eax, w
			mul BPP
			sub esi, eax
			cmp[esi], 0FFED1C24h
			je Color1
			mov eax, [esi]
			mov[edi], eax


			Color1 :
		add edi, BPP
			loop mostrar

			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			MOV EAX, h
			MUL BPP
			SUB EDI, EAX
			
			add total, 4
			mov esi, total

			pop ecx

			loop repite



	}
	}
	else if (indiPelota == 2) {
		indiPelota++;
		__asm
		{
			cld

			mov esi, pelota
			mov edi, buffer

			mov eax, posX
			mul BPP
			add edi, eax
			mov eax, ANCHO_VENTANA
			mul BPP
			mul posY
			add edi, eax

			mov eax, w
			mul BPP
			mul h
			sub eax, BPP
			add esi, eax

			xor ecx, ecx
			mov ecx, h

			repite3 :

			push ecx
				mov ecx, w


				mostrar3 :
			
				mov eax, [esi]
				cmp eax, 0FFED1C24h
				je Color3
				
				mov[edi], eax


				Color3 :

				sub esi, BPP
				add edi, BPP


				loop mostrar3

				mov eax, ANCHO_VENTANA
				mul BPP

				add edi, eax
				MOV EAX, 80
				MUL BPP
				SUB EDI, EAX

				pop ecx

				loop repite3

		}
	}
	else if (indiPelota == 3) {
		indiPelota = 0;
		__asm {
			cld

			mov esi, pelota
			mov edi, buffer

			
			mov eax, w
			mul BPP
			mul h
			mov total2, eax
			sub esi, BPP
			mov total2, esi

			mov eax, posX
			mul BPP
			add edi, eax
			mov eax, ANCHO_VENTANA
			mul BPP
			mul posY
			add edi, eax

			xor ecx, ecx
			mov ecx, w


			repite4 :

			push ecx
				mov ecx, h


				mostrar4 :

			mov eax, w
				mul BPP
				add esi, eax
				cmp[esi], 0FFED1C24h
				je Color4
				
				mov eax, [esi]
				mov[edi], eax


				Color4 :
			add edi, BPP
				loop mostrar4

				mov eax, ANCHO_VENTANA
				mul BPP

				add edi, eax
				MOV EAX, h
				MUL BPP
				SUB EDI, EAX

				sub total2, 4
				mov esi, total2

				pop ecx

				loop repite4



		}
	}
}

void DibujaPersonaje(int* buffer, int* personaje, DIMENSION dmn, POSITION pos)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = posPer.X;
	int y = posPer.Y;


	if (mirror == FALSE)
	{
		__asm {
			cld
			//cargo el sprite
			mov esi, personaje
			mov edi, buffer

			//posicionar mi sprite		
			//mov eax, y
			//mov ebx, 9200
			//mul ebx
			//add edi, eax

			//mov eax, x
			//mul BPP
			//add edi, eax

		//	add edi, 769000


			mov eax, ANCHO_VENTANA
			mul BPP
			mul y
			add edi, eax

			mov eax, x
			mul BPP
			add edi, eax





		mov eax, 59   //porque es la cantidad de pixeles de cada cuadrito
		mul BPP
		mul indiPersonaje    //para moverlo con la tecla que asigne, si le pico una ves el 0 default se convierte en uno, de multiplica por 59 y se mueve el sprite
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx
		ScaladoY :
			push ecx
				mov ecx, scale
				repite :
			push ecx
				mov ecx, 59
				mostrar :
				mov eax, [esi]         //mi imagen la paso a eax
				cmp eax, 0FFFF0000h    //comparo lo rojo 
				je colorro   //si es igual salta a colorro
				or eax, coloresdif
				push ecx
				mov ecx, scale
				scalax :
			mov[edi], eax
				add edi, BPP
				loop scalax
				pop ecx
				jmp Salir
				colorro :
			mov eax, BPP
				mul scale
				add edi, eax
				Salir :
			add esi, BPP
				loop mostrar
				mov eax, ANCHO_VENTANA
				mul BPP
				add edi, eax
				MOV EAX, 59
				MUL BPP
				sub esi, eax
				mul scale
				SUB EDI, EAX
				pop ecx
				loop repite
				mov eax, w
				mul BPP
				add esi, eax

				pop ecx
				loop ScaladoY
		}


	}
	else {
		__asm {
			cld
			//cargo el sprite
			mov esi, personaje
			mov edi, buffer

			//posicionar mi sprite		
		/*	mov eax, y
			mov ebx, 9200
			mul ebx
			add edi, eax

			mov eax, x
			mul BPP
			add edi, eax

			add edi, 769000*/

			mov eax, ANCHO_VENTANA
			mul BPP
			mul y
			add edi, eax

			mov eax, x
			mul BPP
			add edi, eax


			mov eax, 59   //porque es la cantidad de pixeles de cada cuadrito
			mul BPP
			mul indiPersonaje    //para moverlo con la tecla que asigne, si le pico una ves el 0 default se convierte en uno, de multiplica por 59 y se mueve el sprite
			add esi, eax

			xor ecx, ecx
			mov ecx, h    //asigno mi altura a ecx
			ScaladoY2 :
			push ecx
				mov ecx, scale
				repite2 :
			push ecx
				mov ecx, 59
				mostrar2 :
				mov eax, [esi]
				cmp eax, 0FFFF0000h
				je Color2
				or eax, coloresdif
				push ecx
				mov ecx, scale
				scalax2 :
			mov[edi], eax
				add edi, BPP
				loop scalax2
				pop ecx
				jmp Salir2
				Color2 :
			mov eax, BPP
				mul scale
				add edi, eax
				Salir2 :
			sub esi, BPP//
				loop mostrar2
				mov eax, ANCHO_VENTANA
				mul BPP
				add edi, eax
				MOV EAX, 59
				MUL BPP
				add esi, eax
				mul scale
				SUB EDI, EAX
				pop ecx
				loop repite2
				mov eax, w
				mul BPP
				add esi, eax

				pop ecx
				loop ScaladoY2
		}
	}

}

void DibujaLobo(int* buffer, int* lobo, DIMENSION dmn, POSITION pos)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = posLob.X;
	int y = posLob.Y;
	if (lobomirror == FALSE) {
		__asm {
			cld

			mov esi, lobo
			mov edi, buffer

			//posicionar mi sprite		
			mov eax, y
			mov ebx, 9200
			mul ebx
			add edi, eax

			mov eax, x
			mul BPP
			add edi, eax

			add edi, 680000


			mov eax, 48        //porque es la cantidad de pixeles de cada cuadrito
			mul BPP
			mul  indiLobo
			add esi, eax

			xor ecx, ecx
			mov ecx, h    //asigno mi altura a ecx

			repetir :
			push ecx     //guardo en mi pila ecx
				mov ecx, 48

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
				mov eax, ANCHO_VENTANA
				mul BPP

				add edi, eax
				mov eax, 48
				mul BPP
				sub edi, eax

				//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
				mov eax, w
				mul BPP

				add esi, eax
				mov eax, 48
				mul BPP
				sub esi, eax

				pop ecx
				loop repetir
		}
	}
	else {
		__asm {
			cld

			mov esi, lobo
			mov edi, buffer

			//posicionar mi sprite		
			mov eax, y
			mov ebx, 9200
			mul ebx
			add edi, eax

			mov eax, x
			mul BPP
			add edi, eax

			add edi, 680000


			mov eax, 48        //porque es la cantidad de pixeles de cada cuadrito
			mul BPP
			mul  indiLobo
			sub esi, eax

			xor ecx, ecx
			mov ecx, h    //asigno mi altura a ecx

			repetir2 :
			push ecx     //guardo en mi pila ecx
				mov ecx, 48

				//PARA QUITAR EL FONDO ROJO
				muestra2 :
				mov eax, [esi]         //mi imagen la paso a eax
				cmp eax, 0FFFF00D6h    //comparo lo rojo 
				je color2   //si es igual salta a color1
				/*or eax, coloresdif*/
				mov[edi], eax
				color2 :
			sub esi, BPP   //incrementa y pasa el pixel rojo
				add edi, BPP
				loop muestra2

				//suma el total para luego restar y posicionarlo en la linea de abajo         
				mov eax, ANCHO_VENTANA
				mul BPP

				add edi, eax
				mov eax, 48
				mul BPP
				sub edi, eax

				//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
				mov eax, w
				mul BPP

				add esi, eax
				mov eax, 48
				mul BPP
				add esi, eax

				pop ecx
				loop repetir2
		}
	}
}



#pragma endregion



