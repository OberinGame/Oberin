#ifndef _COMMON  
#define _COMMON

#include "Keys.h"
#include "Remap.h"
#include "Inventory.h"

#define kCharacterDisplay							1
#define kInventoryDisplay							2
#define kSpellsDisplay								3
#define kSkillsDisplay								4
#define kPackContentsDisplay					5
#define kShopContentsDisplay					6
#define kSellListDisplay							7
#define kBankContentsDisplay					8
#define kMakeDisplay									9
#define kMakeCategoryDisplay					10
#define kIDDisplay										11
#define kActionDisplay								12
#define kChatDisplay                  13

#define kCommandStringMaxLength				80//64

#define kPointerCursor								0
#define kWatchCursor									128

#define kNull													-1
#define kOffScreen										0
#define kOnScreen											1
#define kOnAndOffScreen								2
#define kDontDraw											3

extern TWindow												tbWindow;										// main window

extern TBitmap												osBMP;											// offscreen bitmap

extern TBoolean												onLine;
extern int														currentInfoDisplay;
extern TBoolean												makeCategory						[kMaxInventorySize];
extern UInt16													playerRow;
extern UInt16													playerCol;

//--------------------------------------------------------------

void cm_MakeRect						(TRect *r,int left, int top, int width, int height);
void CreateBitmaps					(void);
void InitializeRectangles		(void);
void RenderPlayArea					(int xOff, int yOff);

//--------------------------------------------------------------

void GameMenuSelection			(short menuItem);
void BeginEvent							(void);
void EndEvent								(void);
void MouseDownEvent 				(int theWindow, TPoint pt);
void UpdateEvent						(int theWindow);
void MyMenuEvent						(int theMenu, int theMenuItem);
void NullEvent							(void);
TBoolean isGrass						(int row, int col);
void KeyDownEvent 					(char key,TBoolean repeatKey,UInt16 keyCode);
void checkHitCreature 			(TPoint pt, int theSkill, char failMessage[kStringLength]);
void CheckKeys							(TBoolean mouseEvent, int keyCode);
void ParseInputString 			(void);
void CheckMacro					(void);
void ClearPlayerAction			(void);
TBoolean DoButton 					(TPoint pt, TRect r, int active, int pressed, TBoolean reversed, TBoolean sticky, int theButton);
TBoolean PointerOverTarget 	(TPoint pt);
void chooseMakeCategory     (int category);
void cm_ClearPrefs          (void);
void cm_ReadPrefs           (void);
void cm_WritePrefs          (void);
void cm_ClearParty					(void);
void cm_ReadParty						(void);
void cm_WriteParty					(void);
void cm_HouseKeeping        (void);
void cm_MoveInsertionPoint  (int x);
void cm_ReloadSettings			(void);

void MouseDownEventInventoryWindow        (TPoint pt);
void MouseDownEventPackContentsWindow     (TPoint pt);
void MouseDownEventShopContentsWindow     (TPoint pt);
void MouseDownEventSellListWindow         (TPoint pt);
void MouseDownEventMakeWindow             (TPoint pt);
void MouseDownEventSkillsWindow           (TPoint pt);
void MouseDownEventChessWindow 						(TPoint pt);
void MouseDownEventBankWindow 						(TPoint pt);
void DoStartDrag							(TRect, TBitmap);
OSErr dragTrackingHandler			(DragTrackingMessage,WindowRef,void *,DragRef);
OSErr dragReceiveHandler			(WindowRef,void *,DragRef);


TBoolean DoFloaterButton (TBitmap bmp, TPoint pt, TRect r, int active, int pressed, TBoolean reversed, TBoolean sticky, int theButton, TBoolean use);

#endif