#ifndef _SPLASH  
#define _SPLASH

#include "Keys.h"
#include "Remap.h"

extern TWindow												tbWindow;										// main window
extern TBitmap												osBMP;											// offscreen bitmap

extern int														horizontalOffset;
extern int														verticalOffset;

extern Str31													playerName;
extern char														myPassword[kStringLength];

extern unsigned long									nextSwitch; // beta

//--------------------------------------------------------------

void sp_DrawSplash										(void);
void sp_SplashKeyDownEvent 						(char key);
void sp_SplashMouseDownEvent 					(int theWindow, TPoint pt);
void sp_SplashNullEvent								(void);
void sp_InitSplash										(void);
void sp_DrawSplashText								(TBoolean onScreen);

void sp_AutoLogoff 										(void);	// beta
void sp_AutoLogon 										(void);	// beta

#endif