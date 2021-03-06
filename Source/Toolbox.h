#ifndef _TOOLBOX
#define _TOOLBOX

#include <sound.h>
#include <QDOffscreen.h>
//#include <MacTypes.h>
#include <Menus.h>
#include <MacWindows.h>
//#include <Windows.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <OSUtils.h>
#include <ToolUtils.h>
#include <SegLoad.h>
#include <AppleEvents.h>
#include <Movies.h>
#include <Gestalt.h>
#include <Events.h>
#include <Traps.h>
#include <Timer.h>
#include <Limits.h>
#include <palettes.h>
#include <Controls.h>
#include <balloons.h>
#include <Printing.h>
#include <PictUtils.h>
#include <FixMath.h>
#include <StandardFile.h>
#include <ImageCompression.h>
#include <Folders.h>
#include <NumberFormatting.h>
#include <InternetConfig.h>

#include <stdio.h>
#include <time.h>
#include <math.h>
//#include <fp.h>
#include <stdlib.h>

#include <string.h>

#include "Async Sound.h" //gs 12/29
#include "Remap.h"
#include "Keys.h"

#define	Debug									true

#define	tbPI									3.14159265359

#define kOKButton							1
#define kCancelButton					2

#define	kTETextBox							2
#define	TheControl 				 	 	3
#define	enterKey  						3
#define	CR  									13
#define	Home  								115
#define	Bottom  							119
#define	PageU  								116
#define	PageD  								121
#define	ArrowUp  							30
#define	ArrowDown  						31

#define	ErrorMsgLg						128
#define	tb_StringLength				128
#define	kStringLength         128

#define	tb_TextOr							1
#define	tb_TextCopy						2

#define	tb_Plain							0
#define	tb_Bold								1
#define	tb_Italic							2
#define	tb_Outline						4
#define	tb_Shadow							8
#define	tb_Underline					16

#define tb_Left								1
#define tb_Center							2
#define tb_Right							3

#define	tb_MaxRegionsPoints		15
#define	kMemBitmap						-1

#define	begin									{
#define	end										}
#define	MaxGWorlds						100
#define	MaxWindows						50
#define MaxRegions						100
#define	MaxCursors						30
#define	GWorldError						-1
#define	WindowError						-1		
#define WINBITMAP(w)					(((GrafPtr)(w))->portBits)
#define CopyPStr(dst,src)  		BlockMove(src,dst,src[0]+1)

#define kMillionsColors				32
#define kThousandsColors			16
#define	k256Colors						8
#define	k16Colors							4
#define	k4Colors							2
#define	k2Colors							1

#define	kGrayMode							0
#define	kColorMode						1

extern	WindowPtr					Window;
extern  ModalFilterUPP 	  myGenericFilter;
extern  UniversalProcPtr 	mySplashFilter;

extern	GWorldPtr					gw[MaxGWorlds];
extern	TBoolean					gwFree[MaxGWorlds];
extern	TRect							gwRect[MaxGWorlds];
extern	RGBColor					gwForeColor[MaxGWorlds];
extern	RGBColor					gwBackColor[MaxGWorlds];

extern	MenuHandle				theMenus[10];
extern	TBoolean					MenuFree[10];

extern	WindowPtr					win[MaxWindows];
extern	TBoolean					winFree[MaxWindows];
extern	TRect							winRect[MaxWindows];
extern	RGBColor					winForeColor[MaxGWorlds];
extern	RGBColor					winBackColor[MaxGWorlds];

extern	RgnHandle					tbRgn[MaxRegions];
extern	TBoolean					RgnFree[MaxRegions+1];

extern	RGBColor					myWhite;
extern	RGBColor    			myBlack;
extern	RGBColor    			myRed;

extern	CCrsrHandle				cur[MaxCursors];
extern	TBoolean					curID[MaxCursors];

extern	PicHandle					PHandle;
extern	OSErr							Error;
extern	CGrafPtr 					tbsavedPort;
extern	GDHandle 					tbsavedDevice;
extern	StandardFileReply			currentFile;
extern	short							refNum;

extern short 							prefsvRefNum;
extern long 							prefsDirID;

extern TRect							screenBounds;

//---------------------------------------------------------------------------- 
//- Globals -----------------------------------------------------------------
//----------------------------------------------------------------------------

extern		TBoolean								tb_SoundOn;

OSStatus tb_LaunchURL(ConstStr255Param urlStr);

///////////////////////////////////////////////////////////////////
// Typedefs...
typedef struct TRGBTag
{
   char		r;
	 char		g;
	 char		b;   
} TRGB;

typedef struct TRGBColorTag
{
   unsigned int		r;
	 unsigned int		g;
	 unsigned int		b;   
} TRGBColor;

typedef	struct	TFilterTag
{
	 int			C[5][5];			// 5x5 array of filter coefficients
	 long			D;						// division factor
	 long			B;						// bias
	 TBoolean	r;						// apply filter to red component flag
	 TBoolean	g;						// apply filter to green component flag
	 TBoolean	b;						// apply filter to blue component flag
	 TBoolean	rand;					// randomize coefficients
	 int			min;					// min coefficient for randomizing
	 int			max;					// max coefficient for randomizing
} TFilter, *pTFilter;

//----------------------------------------------------------------------------
// Depth Library //
//----------------------------------------------------------------------------

extern pascal 		short	MaxScreenDepth( GDHandle theGDevice );
extern pascal 		Boolean SupportsDepth( GDHandle theGDevice, short aDepth);
extern pascal 		short GetScreenDepth( GDHandle theGDevice );
extern pascal 		void	SetScreenDepth( GDHandle theGDevice, short newDepth );
extern pascal 		Boolean SupportsMode( GDHandle theGDevice, short aMode);
extern pascal 		short GetScreenMode( GDHandle theGDevice );
extern pascal		 	void	SetScreenMode( GDHandle theGDevice, short newMode );
extern pascal 		void	SetScreenModeDepth( GDHandle theGDevice, short newDepth, short newMode );

//----------------------------------------------------------------------------
//- Misc functions ---------------------------------------------------------
//----------------------------------------------------------------------------

//int RC(int value, int range);

pascal	OSErr	GetVolumeInfoNoName(ConstStr255Param pathname,
									short vRefNum,
									HParmBlkPtr pb);

pascal	OSErr	DetermineVRefNum(ConstStr255Param pathname,
								 short vRefNum,
								 short *realVRefNum);

pascal	OSErr	GetDirItems(short vRefNum,
							long dirID,
							ConstStr255Param name,
							Boolean getFiles,
							Boolean getDirectories,
							FSSpecPtr items,
							short reqItemCount,
							short *actItemCount,
							short *itemIndex);

pascal	OSErr GetCatInfoNoName(short vRefNum,
							   long dirID,
							   ConstStr255Param name,
							   CInfoPBPtr pb);
							   
pascal	OSErr	GetDirectoryID(short vRefNum,
							   long dirID,
							   ConstStr255Param name,
							   long *theDirID,
							   Boolean *isDirectory);
							   
void tb_RemoveClip(TBitmap dstBitmap);
void tb_DeleteFile(char filename[tb_StringLength]);
void tb_SwithToApplicationFolder(void);
void tb_SwithToPrefsFolder(void);
void tb_RestorePreviousFolder(void);
pascal	OSErr SetDefault(short newVRefNum,long newDirID,short *oldVRefNum,long *oldDirID);
pascal	OSErr RestoreDefault(short oldVRefNum,long oldDirID);
pascal	OSErr	GetFileLocation(short refNum,short *vRefNum,long *dirID,StringPtr fileName);
pascal	OSErr	FSpGetFileLocation(short refNum, FSSpec *spec);

							   
//----------------------------------------------------------------------------
//- File functions ---------------------------------------------------------
//----------------------------------------------------------------------------

int 					tb_OpenPrefsForOutput(void);
int 					tb_OpenPrefsForInput(void);
int 					tb_OpenOutputFile(char name[tb_StringLength], TBoolean PromptForName);
int 					tb_OpenInputFile(char filename[tb_StringLength]);
int 					tb_WriteToFile(void	*d, long s);
int 					tb_ReadFromFile(void	*d, long s);
void 					tb_CloseFile(void);
int 					tb_OpenNamedInputFile(char filename[tb_StringLength]);
int 					tb_OpenPartyFileForOutput(char filename[kStringLength]);
int 					tb_OpenPartyFileForInput(char filename[kStringLength]);
void          tb_SetDefaultFolder(void);

//----------------------------------------------------------------------------
//- Dialog functions ---------------------------------------------------------
//----------------------------------------------------------------------------

int 					tb_DoDialog(int id);
void 					tb_AlertMessage(char alertMess[ErrorMsgLg]);
void 					tb_AlertMessage2(char alertMess1[ErrorMsgLg],char alertMess2[ErrorMsgLg]);
int 					tb_AlertMessage3(int id,char alertMess[ErrorMsgLg]);
void 					tb_ShowMessage(int	id,char errMess[ErrorMsgLg]);

//----------------------------------------------------------------------------
//- Startup & Shutdown functions  --------------------------------------------
//----------------------------------------------------------------------------

void 					tb_InitializeToolbox(void);
void 					tb_CloseToolbox(void);
void 					tb_Halt(void);

//----------------------------------------------------------------------------
//- Bitmap functions ---------------------------------------------------------
//----------------------------------------------------------------------------

void 					tb_GetBitmapRect(TBitmap Bitmap, TRect *r);
int 					tb_GetBitmapWidth(TBitmap Bitmap);
int 					tb_GetBitmapHeight(TBitmap Bitmap);

//----------------------------------------------------------------------------
//- Window functions ---------------------------------------------------------
//----------------------------------------------------------------------------

TWindow 			tb_OpenWindow(TRect *r, char *WindowTitle);
void 					tb_DisposeOfWindow(TWindow theWindow);
void					tb_GetClientRect(TWindow theWindow, TRect *crect);
void 					tb_GetWindowRect(TWindow theWindow, TRect *wrect);
int 					tb_SetWindowPtr(WindowPtr w);
void 					tb_SetWindowTitle(TWindow theWindow, char WindowTitle[tb_StringLength]);
TBoolean      tb_LocalPointInContent(TBitmap theWindow, TPoint pt);
void          tb_GetLocalMousePosition(TBitmap theWindow, TPoint *pt);

//----------------------------------------------------------------------------
//- Graphics functions -------------------------------------------------------
//----------------------------------------------------------------------------

TBitmap 			tb_CreateBitmap(TRect *r, int Picture, int Depth);
void 					tb_CopyMaskBitmap(TBitmap srcBitmap, TBitmap maskBitmap, TBitmap dstBitmap, TRect *srcRect, TRect *maskRect, TRect *dstRect);
void 					tb_DisposeBitmap(TBitmap Bitmap);
void 					tb_CopyBitmap(TBitmap srcBitmap, TBitmap dstBitmap, TRect *srcRect, TRect *dstRect, TBoolean Transparent);
void 					tb_DrawPictureToBitmap(TBitmap dstBitmap, int Picture, int h, int v, TRect *r);
void 					tb_CopyMaskBitmapClr(TBitmap srcBitmap, TBitmap maskBitmap, TBitmap dstBitmap, TRect *srcRect, TRect *maskRect, TRect *dstRect, unsigned int red, unsigned int green, unsigned int blue);
void 					tb_CopyBitmapClr(TBitmap srcBitmap, TBitmap dstBitmap, TRect *srcRect, TRect *dstRect, unsigned int red, unsigned int green, unsigned int blue);
void					tb_DarkenBitmap(int bitmap, TRect *r, int level);

void 					tb_Rectangle(TBitmap Bitmap, TRect *drect);
void 					tb_PaintRect(TBitmap dstBitmap, TRect *drect);
void 					tb_FillRect(TBitmap dstBitmap, short PatternID, TRect *drect);
void 					tb_FrameRect(TBitmap dstBitmap, TRect *drect);
void 					tb_SetCursor(short id);
void          tb_PaintRegion(int dstBitmap, TRegion	r);
void 					tb_Line(TBitmap dstBitmap, int h1, int v1, int h2, int v2, int width);
void 					tb_SetClip(TBitmap dstBitmap, TRect *crect);
void 					tb_SetForeColor(TBitmap dstBitmap, short red, short green, short blue);
void 					tb_SetBackColor(TBitmap dstBitmap, unsigned int red, unsigned int green, unsigned int blue);
TRegion 			tb_CreateRegion(TPoint RgnArray[tb_MaxRegionsPoints]);
void 					tb_DisposeRegion(TRegion theRegion);
void 					tb_EraseRect(TBitmap dstBitmap, TRect *drect);

void 					tb_SetFont(TBitmap dstBitmap, int theFont);
void 					tb_SetFontSize(TBitmap dstBitmap, int theSize);
void 					tb_DrawString(TBitmap dstBitmap, int h, int v, char s[tb_StringLength]);
void 					tb_SetFontTransfer(TBitmap dstBitmap, int	transfer);
void 					tb_SetFontStyle(TBitmap dstBitmap, int	style);
void 					tb_DrawNumber(TBitmap dstBitmap, int h, int v, long n, int justify);

void 					tb_CopyORMaskBitmapStd(TBitmap srcBitmap, TBitmap dstBitmap, TRect *srcRect, TRect *dstRect);
GWorldPtr 		tb_PtrToGWorld(TBitmap Bitmap);
void					tb_GetPixelRGB(int bitmap, int x, int y, unsigned int* r, unsigned int* g, unsigned int* b);
void	        tb_GetPixelGray(int bitmap, int x, int y, unsigned int* g);
void					tb_SetPixelRGB(int bitmap, int x, int y, unsigned int r, unsigned int g, unsigned int b);
void					tb_SetWindowPixelRGB(int bitmap, int x, int y, unsigned int r, unsigned int g, unsigned int b);
void					tb_RenderLightSource(int bitmap, int x, int y, int radius, int gameTime);
void					tb_ColorizeWorkSpace(int workBMP, int color, TRect *r);
void 					tb_GetFontInfo(TBitmap dstBitmap, short* maxAscent, short *maxDescent);
void					tb_DarkenTextBackground(int bitmap, TRect *r);

void					tb_RenderColoredLightSource(int bitmap, int x, int y, int radius, int clr, int level, int gameTime);
void					tb_RenderLightning(int bitmap, int x1, int y1, int x2, int y2, unsigned int r, unsigned int g, unsigned int b, int thk, TBoolean blur);
void					tb_RenderWarp(int bitmap, int x, int y, int radius, int str);
void					tb_RenderLightRing(int bitmap, int x, int y, int radius, int rfactor, int clr, int level, int gameTime);
void					tb_RenderBitmap(TBitmap dest, int	src, int xc, int yc, int frames, int cr, int cg, int cb, int alpha);
void					tb_RenderBitmapFrame(TBitmap dest, int	src, int xc, int yc, int frames, int fra, int cr, int cg, int cb, int alpha);
void					tb_RenderLightEllipse(int bitmap, int x, int y, int w, int h, int clr, int level, int gameTime);
void					tb_RenderLightRingEllipse(int bitmap, int x, int y, int w, int h, int rfactor, int clr, int level, int gameTime);
void					tb_QuickApplyFilter(int bitmap, TRect *frect, pTFilter filter);
void					f_QuickBlur(int bitmap, TRect *rect);
void					tb_InitializeFilter(pTFilter filter);

void 					tb_SetGWorld(TBitmap theBitmap);
void 					tb_RestorePort(void);
void 					tb_FastCopyMaskBitmap(TBitmap srcBitmap, TBitmap maskBitmap, TBitmap dstBitmap, TRect *srcRect, TRect *maskRect, TRect *dstRect);
PicHandle				tb_GetPicture(int i, TBoolean convert);
void					tb_GrayBMP(int bitmap, TRect *t);
void	        		tb_ColorizeCreature(int workBMP, int color, TRect *a);
void          			tb_CopyBitmapGray(TBitmap srcBitmap, TBitmap dstBitmap, TRect *srcRect, TRect *dstRect, TBoolean Transparent);
//----------------------------------------------------------------------------
//- String functions -----------------------------------------------------------
//----------------------------------------------------------------------------

int 					tb_StringWidth(TBitmap dstBitmap, char s[tb_StringLength]);
void 					tb_CopyString(char str1[tb_StringLength],char str2[tb_StringLength]);
void 					tb_ConcatString(char str1[tb_StringLength], char str2[tb_StringLength], char str3[tb_StringLength]);
void 					tb_IntegerToString(long n,char str1[tb_StringLength]);
void          tb_CopyStringAt(char *str1,char *str2, int pos);
void          tb_DeleteStringPadding(char *str1);
void          tb_MakeLowerCase(char str[kStringLength]);
void 					tb_StringToNum(char str[kStringLength], long *number);
void 					tb_StringToFloat(char str[kStringLength], float *number);
void 					tb_NumToString(long number, char str[kStringLength]);
char*					tb_Unscramble(char text[kStringLength]);
char* 					tb_getTimeAndDate(char getWhat[kStringLength]);

//----------------------------------------------------------------------------
//- Menu functions -----------------------------------------------------------
//----------------------------------------------------------------------------

int 					tb_InsertTheMenuItem(int theMenu,char *MenuItemTitle,char Key);
int 					tb_InsertTheMenu(char *MenuTitle);

//----------------------------------------------------------------------------
//- Sound functions ----------------------------------------------------------
//----------------------------------------------------------------------------

void 					tb_PlaySound(int id);
void 					tb_Beep(int);

//----------------------------------------------------------------------------
//- Miscellaneous functions --------------------------------------------------
//----------------------------------------------------------------------------

void 					tb_Idle(void);
void 					tb_SetPort(int theBitmap);
//void 					tb_InsetRect(TRect *r, int dh, int dv);
//void 					tb_GetMousePosition(TPoint *mpt);
//int 					tb_Round(double x);
int 					tb_Trunc(double x);
//int 					tb_Rnd (int min, int max);
void 					tb_DisplayError(char errMess[ErrorMsgLg]);
TBoolean 			tb_IsButtonDown(void);
TBoolean 			tb_IsKeyDown (short KeyCode);
void 					tb_GetMonitorRect(TRect *r);
void		 			tb_FlushMouseEvents(void);
TBoolean 			tb_PointInRegion(TPoint pt, TRegion theRegion);
void 					tb_Wait(unsigned long	ms);
unsigned long tb_CurrentMillisecond(void);

unsigned long tb_CurrentTick(void);
void 					tb_WaitTick(unsigned long	t);

//---------------------------------------------------------------------------
// Mac Only
int 					tb_GetWindowMatch(WindowPtr wp);
void 					tb_CtoPStr255(char *str,unsigned char str2[]);
void 					tb_CtoPStr31(char *str,unsigned char str2[]);
TBoolean 			tb_PascalStringsEqual(unsigned char str1[],unsigned char str2[]);
void 					tb_PascaltoCStr(Str255 thePString,char StrC[50]);
//
//---------------------------------------------------------------------------

#endif //  _TOOLBOX