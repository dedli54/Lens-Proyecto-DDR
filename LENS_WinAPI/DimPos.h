#pragma once

/* Estructura con las coordenadas de los sprites en pantalla en un plano 2D */
struct POSITION {
	int X;
	int Y;
	bool Animation = false;
	bool Draw = false;
	bool isGold = false;
};

/* Estructura con las dimensiones de los sprites a cargar y desplegar en pantalla */
struct DIMENSION {
	int ANCHO;		 // X
	int ALTO;		 // Y
};