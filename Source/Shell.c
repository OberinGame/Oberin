#include "Shell.h"
#include "Network.h"
#include "FileIO.h"
#include "Resource.h"
#include "Shops.h"
#include "FX.h"
// #include "Floater.h"
#include "MacWindows.h"
#include "MacTextEditor.h"
#include "Updater.h"
#include "GuildHalls.h"

#pragma mark Types
/* Types */

typedef	Byte	int8;
typedef	short	int16;
typedef long	int32;

#pragma mark Constants
/* Constants */

#define kAppleID					128
#define kFileID						129
#define kEditID						130
#define kActionID					131
#define kViewID						132
#define kPrefsID					133
#define kSoundID					134
#define kMusicID					135
#define kHelpID 					-16490

#define kAppleM						0
#define kFileM						1
#define kEditM						2
#define kActionM					3
#define kViewM						4
#define kPrefsM						5
#define kSoundM						6
#define kMusicM						7

#define kPickUpItem					1
#define kPickUpAll					2
#define kBuyItem					3
#define kSellItem					4
#define kIdentifyItem				5
#define kChessItem					6
#define kRepairItem					7

#define kInventoryItem				1
#define kSkillsItem					2
#define kMapItem					3
#define kStatsItem					4
#define kTextLogItem				5
#define	kCalendarItem				6
#define kCharacterItem				7
#define kBankAccountItem			8
#define kPartyItem		    		9
#define kPetsItem		      		10
#define kHealthBarsItem				11

#define kCloseItem					1			// file menu
#define kLogOffItem					3			// file menu
#define kQuit						5			// file menu

#define kSoundItem					1
#define kMusicItem					2
#define kPathModeItem				3
#define kLogTextItem				4
#define kKeyPadItem       			5
#define kTextBacksItem    			6
//#define kSkipMessagesItem 			8
#define kBgNoteItem					7
#define kColorOwnNameItem			8
#define	kDisplayFeedbackItem		9

#define	begin				{
#define	end					}

const MenuCommand kCommandAbout = FOUR_CHAR_CODE ('abou');

#pragma mark Globals
/* Globals */
long                checkCount;
long                checkCount1;
long                checkCount2;
long                checkCount3;
long                checkCount4;
long                checkCount5;
long                checkCount6;
long                checkCount7;
long                checkCount8;
long                checkCount9;
long                checkCount10;
long                checkCount11;
long                checkCount12;
long                checkCount13;
long                checkCount14;
long                checkCount15;
unsigned long       checkGold;
unsigned long       checkMaxGold;
char								buildString[128];

EventRecord				*gMouseDownRef;
UniversalProcPtr		MyHandleOAppUPP;
UniversalProcPtr		MyHandleODocUPP;
UniversalProcPtr		MyHandlePDocUPP;
UniversalProcPtr		MyHandleQuitUPP;
ModalFilterUPP 		  	myGenericFilter;
UniversalProcPtr 		mySplashFilter;
FILE					*dummyStream;
OSErr					Error;
PicHandle				PictureHandle;
Boolean					Done;
Str255					appName;
short					apRefNum;
Handle					apParam;
MenuHandle				myMenus[9];
MenuHandle				theHelpMenu;
TRect					screenRect;
DialogPtr				theDialog;
short					itemHit=0;
long					exitTick;
Boolean					applicationSuspended;
Boolean					flag=false;
int						i;
int						horizontalOffset;
int						verticalOffset;
Movie					theMusic;
FSSpec					MusicFSSpec;
short					musicRefNum;
extern FSSpec			AppFSSpec;
Ptr 					restoreState;
FSSpec					GraphicsFSSpec;
FSSpec					EmblemsFSSpec;
FSSpec					SoundFSSpec;
int                 	origHelpItems;
int                 	callGMItem;
int                 	oberinWebsiteItem;
TPoint              	windowPosition[kMaxWindows];
TBoolean            	windowVisible[kMaxWindows];
TBoolean            	windowActive[kMaxWindows];
TPoint              	mainWindowPosition;
int                 	gMusicVolume;
extern    int           gSoundVolume;
TBoolean            	skipOldMessages;

WindowPtr				Window;
extern int				tbWindow;

WindowPtr				logonWindow;
int					    tbLogOnWindow;

WindowPtr				mapWindow;
TRect               	mapWindowRect;
int                 	tbMapWindow;

WindowPtr				inventoryWindow;
TRect               	inventoryWindowRect;
int                 	tbInventoryWindow;

WindowPtr				CalendarWindow;
TRect               	CalendarWindowRect;
int                 	tbCalendarWindow;

WindowPtr				packContentsWindow;
TRect               	packContentsWindowRect;
int                 	tbPackContentsWindow;

WindowPtr				shopContentsWindow;
TRect               	shopContentsWindowRect;
int                		tbShopContentsWindow;

WindowPtr				sellListWindow;
TRect               	sellListWindowRect;
int                 	tbSellListWindow;

WindowPtr				idWindow;
TRect               	idWindowRect;
int                 	tbIDWindow;

WindowPtr				makeWindow;
TRect               	makeWindowRect;
int                 	tbMakeWindow;

WindowPtr				skillsWindow;
TRect               	skillsWindowRect;
int                 	tbSkillsWindow;

WindowPtr				statsWindow;
TRect               	statsWindowRect;
int                 	tbStatsWindow;

WindowPtr				chatWindow;
TRect               	chatWindowRect;
int                 	tbChatWindow;

WindowPtr				characterWindow;
TRect               	characterWindowRect;
int                 	tbCharacterWindow;

WindowPtr				chessWindow;
TRect               	chessWindowRect;
int                 	tbChessWindow;

WindowPtr				bankWindow;
TRect               	bankWindowRect;
int                 	tbBankWindow;

WindowPtr				partyWindow;
TRect               	partyWindowRect;
int                 	tbPartyWindow;

WindowPtr				questWindow;
TRect               	questWindowRect;
int                 	tbQuestWindow;

WindowPtr				petsWindow;
TRect               	petsWindowRect;
int                 	tbPetsWindow;

WindowPtr				aboutWindow;
TRect               	aboutWindowRect;
int                 	tbAboutWindow;

TBoolean				logText;
TBoolean				displayFeedback;

TBoolean				usingX;

unsigned long			nextKeyCheck;

DragTrackingHandlerUPP		gDragTrackingHandlerUPP;
DragReceiveHandlerUPP		gDragReceiveHandlerUPP;


//		DialogPtr			chatDialog;

extern TBoolean				gotFirstRefresh;
extern TBoolean				musicOn;
extern TBoolean				pathFind;

extern int					monsterTime; // beta
extern int					refreshTime; // beta
extern int					decayTime; // beta
extern int					resourceTime; // beta
extern int					rotateShopTime; // beta
extern int					serverFXTime; // beta

extern TBoolean				standingOnContainer;
extern TBoolean				standingInShop;
extern TBoolean				standingOnActionSpot;
extern int					actionValue;
extern int          		targetMode;
extern TBoolean				drawHealthBars;
extern TBoolean				keyPadControl;
extern TBoolean     		darkTextBacks;
extern TBoolean     		attemptingReconnect;
extern TBoolean				backgroundnotification;
extern TBoolean				colorOwnName;

short						theSoundRes;
short						theGraphRes;
short						theEmblemsRes;

TBoolean					gUpdating;

extern int					guildHallsPtr;

extern DialogRef			gProgressWindow;
extern WindowRef			dragInitWindow;
extern unsigned long				logTime;
//  FSSpec					items[5000];

// TXNObject   			textObject = NULL;               // text object
// TXNFrameID  			frameID    = 0;                  // ID for text frame
// TXNFrameOptions 		frameOptions;

#ifndef _SERVERONLY
// ----------------------------------------------------------------- //
void sh_EndSession(void)
// ----------------------------------------------------------------- //

{
	fi_WriteToErrorLog("Connection lost");
	nw_ShutdownNetworking();
	onLine=false;
	attemptingReconnect=false;
	windowVisible[kChessWindow]         = false;
	windowVisible[kShopContentsWindow]  = false;
	windowVisible[kSellListWindow]      = false;
	windowVisible[kMakeWindow]          = false;
	windowVisible[kPackContentsWindow]  = false;
	windowVisible[kQuestWindow]         = false;
	windowVisible[kIDWindow]            = false;
	windowVisible[kBankWindow]          = false;
	gr_TempHideAllWindows();
	ShowWindow(logonWindow);
	sp_InitSplash();
	UpdateEvent(tbLogOnWindow);
	tb_AlertMessage("Sorry, but the server connection was lost.");

}

#endif

// ----------------------------------------------------------------- //
static pascal OSErr MyHandleOApp (const AppleEvent */*appleEvt*/, AppleEvent* /*reply*/, long /*refcon*/)
// ----------------------------------------------------------------- //

{

	return noErr;

}

// ----------------------------------------------------------------- //
static pascal OSErr MyHandleODoc (const AppleEvent */*aevt*/, AppleEvent* /*reply*/, long /*refcon*/)
// ----------------------------------------------------------------- //

{

	return noErr;

}

// ----------------------------------------------------------------- //
static pascal OSErr MyHandlePDoc (const AppleEvent */*appleEvt*/, AppleEvent* /*reply*/, long /*refcon*/)
// ----------------------------------------------------------------- //

{

	return noErr;

}

// ----------------------------------------------------------------- //
//pascal OSErr MyHandleQuit (AEDescList *aevt, AEDescList *reply, long refCon)
pascal OSErr MyHandleQuit( const AppleEvent */*appleEvt*/, AppleEvent */*reply*/, long /*refcon*/ )
// ----------------------------------------------------------------- //

{

	Done = true;
	return noErr;

}

// ----------------------------------------------------------------- //
void SetSizeAndPositionOfWindow(void)
// ----------------------------------------------------------------- //

{
	//short					MenuHeight=LMGetMBarHeight()+1;
	short					MenuHeight=GetMBarHeight()+1;
	TRect					r;
	TRect					s;
	short					h;
	short					v;
	short					borderLeft;
	short					borderTop;
	short					borderRight;
	short					borderBottom;
	TPoint				cUpperLeft;
	TPoint				cLowerRight;
	TPoint				wUpperLeft;
	TPoint				wLowerRight;


	screenRect=screenBounds;
	SetPortWindowPort(Window);

	if (screenBounds.right>1024)
	{
		SizeWindow(Window,1024,768,false);
		h=2; // h = (screenBounds.right - (*Window).portRect.right) / 2; // gs carbon
		v=MenuHeight+2; //v = (screenBounds.bottom - (*Window).portRect.bottom) / 2 + MenuHeight; // gs carbon
		MoveWindow(Window, h, v, false);
		ShowWindow(Window);
		return;
	}

	s=screenBounds;

	if (s.right<800) s.right=800;

	if (s.bottom<600) s.bottom=600;

	SizeWindow(Window, s.right, s.bottom, false);

	ShowWindow(Window);

	tb_GetWindowRect(tbWindow, &r);

	wUpperLeft.h=r.left;

	wUpperLeft.v=r.top;

	wLowerRight.h=r.right;

	wLowerRight.v=r.bottom;

	tb_GetClientRect(tbWindow, &r);

	cUpperLeft.h=r.left;

	cUpperLeft.v=r.top;

	cLowerRight.h=r.right;

	cLowerRight.v=r.bottom;

	LocalToGlobal(&cUpperLeft);

	LocalToGlobal(&cLowerRight);

	borderLeft=cUpperLeft.h-wUpperLeft.h;

	borderTop=cUpperLeft.v-wUpperLeft.v;

	borderRight=wLowerRight.h-cLowerRight.h;

	borderBottom=wLowerRight.v-cLowerRight.v;

	r.left		=	s.left			+	borderLeft+1;

	r.top			=	s.top			+	borderTop+1+MenuHeight;

	r.right		=	s.right		-	borderRight-1;

	r.bottom	=	s.bottom		-	borderBottom;

	SizeWindow(Window,(r.right-r.left),(r.bottom-r.top),false);

	MoveWindow(Window,borderLeft+1,borderTop+MenuHeight,false);

}

// ----------------------------------------------------------------- //
void OpenWindow(void)
// ----------------------------------------------------------------- //

{
	short								height=GetMBarHeight();
	//TRect								q;
	TRect								r;
	//OSStatus						status;
	//SInt32			        justification;
	//TXNControlTag	      controlTag[1];
	//TXNControlData	    controlData[1];
	BitMap 									screenBits;


	GetQDGlobalsScreenBits(&screenBits);


#ifndef _SERVER
	logonWindow = GetNewCWindow(129, nil, (WindowPtr)-1L);

	if (logonWindow != nil)
	{
		tbLogOnWindow=tb_SetWindowPtr(logonWindow);
		ShowWindow(logonWindow);
		SelectWindow(logonWindow);
		//tb_DrawPictureToBitmap(tbLogOnWindow, 243, 0, 0, &r);
	}

#endif

#ifdef _SERVERONLY
	Window = GetNewCWindow(130, nil, (WindowPtr)-1L);

	if (Window != nil)
	{
		tbWindow=tb_SetWindowPtr(Window);
		ShowWindow(Window);
		SelectWindow(Window);
		tb_SetFont(tbWindow, kFontIDGeneva);
		tb_SetFontSize(tbWindow, 24);
		tb_SetFontStyle(tbWindow, tb_Bold);
		tb_DrawString(tbWindow, 5, 25, "Starting up...");
	}

	return;

#endif

	Window = GetNewCWindow(128, nil, (WindowPtr)-1L);

	if (Window != nil)
	{
		tbWindow=tb_SetWindowPtr(Window);
		MoveWindowStructure(Window,mainWindowPosition.h,mainWindowPosition.v);
#ifdef _SERVER
		ShowWindow(Window);
		SelectWindow(Window);
#endif
	}

	//	chatDialog = GetNewDialog(130, nil, (WindowPtr)-1L);
//   ShowWindow(GetDialogWindow(chatDialog));
	//  SelectWindow(GetDialogWindow(chatDialog));
	//SetModelessDialogCallbackProc(chatDialog,


	tb_GetClientRect(tbWindow, &r);

	horizontalOffset=0;

	verticalOffset=0;

	// character floater
	tb_SetRect(&characterWindowRect,0,0,238,308);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&characterWindowRect,&characterWindow);

	tbCharacterWindow=tb_SetWindowPtr(characterWindow);

	tb_SetWindowTitle(tbCharacterWindow, "Character");

	MoveWindowStructure(characterWindow,windowPosition[kCharacterWindow].h,windowPosition[kCharacterWindow].v);

	// map floater
	tb_SetRect(&mapWindowRect,0,0,147,131);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&mapWindowRect,&mapWindow);

	tbMapWindow=tb_SetWindowPtr(mapWindow);

	tb_SetWindowTitle(tbMapWindow, "Map");

	MoveWindowStructure(mapWindow,windowPosition[kMapWindow].h,windowPosition[kMapWindow].v);

	// inventory floater
	tb_SetRect(&inventoryWindowRect,0,0,238,564);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&inventoryWindowRect,&inventoryWindow);

	tbInventoryWindow=tb_SetWindowPtr(inventoryWindow);

	tb_SetWindowTitle(tbInventoryWindow, "Inventory");

	MoveWindowStructure(inventoryWindow,windowPosition[kInventoryWindow].h,windowPosition[kInventoryWindow].v);

	// Calendar floater
	tb_SetRect(&CalendarWindowRect,0,0,238,100);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&CalendarWindowRect,&CalendarWindow);

	tbCalendarWindow=tb_SetWindowPtr(CalendarWindow);

	tb_SetWindowTitle(tbCalendarWindow, "Calendar");

	MoveWindowStructure(CalendarWindow,windowPosition[kCalendarWindow].h,windowPosition[kCalendarWindow].v);

	// get floater
	tb_SetRect(&packContentsWindowRect,0,0,238,564);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&packContentsWindowRect,&packContentsWindow);

	tbPackContentsWindow=tb_SetWindowPtr(packContentsWindow);

	tb_SetWindowTitle(tbPackContentsWindow, "Get");

	MoveWindowStructure(packContentsWindow,windowPosition[kPackContentsWindow].h,windowPosition[kPackContentsWindow].v);

	// shop floater
	tb_SetRect(&shopContentsWindowRect,0,0,238,564);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&shopContentsWindowRect,&shopContentsWindow);

	tbShopContentsWindow=tb_SetWindowPtr(shopContentsWindow);

	tb_SetWindowTitle(tbShopContentsWindow, "Buy");

	MoveWindowStructure(shopContentsWindow,windowPosition[kShopContentsWindow].h,windowPosition[kShopContentsWindow].v);

	// sell floater
	tb_SetRect(&sellListWindowRect,0,0,238,564);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&sellListWindowRect,&sellListWindow);

	tbSellListWindow=tb_SetWindowPtr(sellListWindow);

	tb_SetWindowTitle(tbSellListWindow, "Sell");

	MoveWindowStructure(sellListWindow,windowPosition[kSellListWindow].h,windowPosition[kSellListWindow].v);

	// id floater
	tb_SetRect(&idWindowRect,0,0,238,564);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&idWindowRect,&idWindow);

	tbIDWindow=tb_SetWindowPtr(idWindow);

	tb_SetWindowTitle(tbIDWindow, "Identification");

	MoveWindowStructure(idWindow,windowPosition[kIDWindow].h,windowPosition[kIDWindow].v);

	// make floater
	tb_SetRect(&makeWindowRect,0,0,238,564);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&makeWindowRect,&makeWindow);

	tbMakeWindow=tb_SetWindowPtr(makeWindow);

	tb_SetWindowTitle(tbMakeWindow, "Use Skill");

	MoveWindowStructure(makeWindow,windowPosition[kMakeWindow].h,windowPosition[kMakeWindow].v);

	// skills floater
	tb_SetRect(&skillsWindowRect,0,0,152,464);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&skillsWindowRect,&skillsWindow);

	tbSkillsWindow=tb_SetWindowPtr(skillsWindow);

	tb_SetWindowTitle(tbSkillsWindow, "Skills");

	MoveWindowStructure(skillsWindow,windowPosition[kSkillsWindow].h,windowPosition[kSkillsWindow].v);

	// stats floater
	tb_SetRect(&statsWindowRect,0,0,238,105);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&statsWindowRect,&statsWindow);

	tbStatsWindow=tb_SetWindowPtr(statsWindow);

	tb_SetWindowTitle(tbStatsWindow, "Statistics");

	MoveWindowStructure(statsWindow,windowPosition[kStatsWindow].h,windowPosition[kStatsWindow].v);

	// chat floater
	tb_SetRect(&chatWindowRect,0,0,238,564);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&chatWindowRect,&chatWindow);

	tbChatWindow=tb_SetWindowPtr(chatWindow);

	tb_SetWindowTitle(tbChatWindow, "Text Log");

	MoveWindowStructure(chatWindow,windowPosition[kChatWindow].h,windowPosition[kChatWindow].v);

	// chess floater
	tb_SetRect(&chessWindowRect,0,0,238,373);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&chessWindowRect,&chessWindow);

	tbChessWindow=tb_SetWindowPtr(chessWindow);

	tb_SetWindowTitle(tbChessWindow, "Chess");

	MoveWindowStructure(chessWindow,windowPosition[kChessWindow].h,windowPosition[kChessWindow].v);

	// bank floater
	tb_SetRect(&bankWindowRect,0,0,238,564);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&bankWindowRect,&bankWindow);

	tbBankWindow=tb_SetWindowPtr(bankWindow);

	tb_SetWindowTitle(tbBankWindow, "Bank");

	MoveWindowStructure(bankWindow,windowPosition[kBankWindow].h,windowPosition[kBankWindow].v);

	// party floater
	tb_SetRect(&partyWindowRect,0,0,188,257);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&partyWindowRect,&partyWindow);

	tbPartyWindow=tb_SetWindowPtr(partyWindow);

	tb_SetWindowTitle(tbPartyWindow, "Party");

	MoveWindowStructure(partyWindow,windowPosition[kPartyWindow].h,windowPosition[kPartyWindow].v);

	// quest floater
	tb_SetRect(&questWindowRect,0,0,357,199);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&questWindowRect,&questWindow);

	tbQuestWindow=tb_SetWindowPtr(questWindow);

	tb_SetWindowTitle(tbQuestWindow, "Scroll"); //Dave "Quest"

	MoveWindowStructure(questWindow,windowPosition[kQuestWindow].h,windowPosition[kQuestWindow].v);

	// pets floater
	tb_SetRect(&petsWindowRect,0,0,188,267);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&petsWindowRect,&petsWindow);

	tbPetsWindow=tb_SetWindowPtr(petsWindow);

	tb_SetWindowTitle(tbPetsWindow, "Pets");

	MoveWindowStructure(petsWindow,windowPosition[kPetsWindow].h,windowPosition[kPetsWindow].v);

	// about floater
	tb_SetRect(&aboutWindowRect,0,0,267,290);

	CreateNewWindow (kFloatingWindowClass,kWindowStandardFloatingAttributes,&aboutWindowRect,&aboutWindow);

	tbAboutWindow=tb_SetWindowPtr(aboutWindow);

	tb_SetWindowTitle(tbAboutWindow, "About Oberin");

	MoveWindowStructure(aboutWindow,(screenBits.bounds.right/2)-136,(screenBits.bounds.bottom/2)-150);

	//Install drag handlers
	if (InstallTrackingHandler(gDragTrackingHandlerUPP,inventoryWindow,NULL)!=noErr)
		tb_Halt();

	if (InstallReceiveHandler(gDragReceiveHandlerUPP,inventoryWindow,NULL)!=noErr)
		tb_Halt();

	dragInitWindow=NULL;

	if(InstallTrackingHandler(gDragTrackingHandlerUPP,bankWindow,NULL)!=noErr)
		tb_Halt();
	if(InstallReceiveHandler(gDragReceiveHandlerUPP,bankWindow,NULL)!=noErr)
		tb_Halt();


}

// ----------------------------------------------------------------- //
void System7Dialog(void)
// ----------------------------------------------------------------- //

{
	// carbon
	/*
			DialogPtr			Dialog;
			short					itemHit=0;
			int						h;
			int						v;


			tb_SetCursor(0);
			Dialog = GetNewDialog(167, nil, (WindowPtr)-1L);
			if (Dialog!=NULL)
				{
					SysBeep(0);
					SetPortWindowPort(Dialog);
					v = Dialog->portRect.top + ((screenBounds.bottom - Dialog->portRect.bottom) / 2) - 25;
					h = Dialog->portRect.left + ((screenBounds.right - Dialog->portRect.right) / 2);
					MoveWindow(Dialog, h, v, true);
					ShowWindow(Dialog);
					while (itemHit<1)
						ModalDialog(nil, &itemHit);
					DisposeDialog(Dialog);
				}

			ExitToShell();
	*/
}

// ----------------------------------------------------------------- //
void InitMLTE(void)
// ----------------------------------------------------------------- //

{
	//OSStatus							              status;
	//SInt16								              qdFontID;
	//TXNMacOSPreferredFontDescription	  defaults;  // fontFamilyID, point size, style, and script code
	//TXNInitOptions                      options;

	//GetFNum("\pGeneva", &qdFontID);

	//defaults.fontID = qdFontID;
	//	defaults.pointSize = kTXNDefaultFontSize;
	//	defaults.encoding = CreateTextEncoding(kTextEncodingMacRoman, kTextEncodingDefaultVariant, kTextEncodingDefaultFormat);
	//	defaults.fontStyle 	= kTXNDefaultFontStyle;

	//options = kTXNWantMoviesMask | kTXNWantSoundMask | kTXNWantGraphicsMask;
	//options = 0;

	//status = TXNInitTextension(&defaults, 1, options);


}

// ----------------------------------------------------------------- //
Boolean IsAppWindow(WindowPtr /*window*/, TXNObject */*object*/)
// ----------------------------------------------------------------- //

{
	//OSErr status = noErr;

	//status = GetWindowProperty(window, 'GRIT', 'tObj', sizeof(TXNObject), NULL, object);
	//return (window != NULL) && (GetWindowKind(window) == userKind);
}

// ----------------------------------------------------------------- //
void DoIdle()
// ----------------------------------------------------------------- //

{
	//WindowPtr	window = FrontWindow();
	//TXNObject	object = NULL;

	//if (IsAppWindow(window, &object))
	//	TXNIdle(object);

// if (chatTE!=NULL)
//   TEIdle(teChat);

}

// ----------------------------------------------------------------- //
void Initialize(void)
// ----------------------------------------------------------------- //

{
	//SysEnvRec							thisWorld;
	OSErr									e;
	long									theDirID;
	TBoolean							isDirectory;
	int                   i;
	long                  response=0;
	CFStringRef				locale=CFSTR("fr_FR");

	tb_SetCursor(0);

	gUpdating=false;

	rs_InitCalendar();

	remove("OldOberin"); //Auto-updater



	usingX=false;
	e = Gestalt(gestaltSystemVersion, &response);

	if (response>=0x00001000)
		usingX=true;

#ifndef _SERVERONLY
	// ------------- system 7 version ------------- //
//	if (SysEnvirons(1, &thisWorld)==noErr)
//		if (thisWorld.systemVersion < 0x0700)
//			System7Dialog();

	// ------------- screen size ------------- //
//	if ((screenBounds.right<800) || (screenBounds.bottom<600)) // gs carbon
//		{
//			tb_DoDialog(133);
//			ExitToShell();
//		}

	// ------------- screen depth ------------- //
//	if ((SupportsDepth(GetMainDevice(),8)==false) && (SupportsDepth(GetMainDevice(),16)==false) && (SupportsDepth(GetMainDevice(),32)==false))
//		{
//			tb_DoDialog(166);	// gs carbon
//			ExitToShell();
//		}

	// ------------- system 8.5 version ------------- //
//	if (SysEnvirons(1, &thisWorld)==noErr)
//		if (thisWorld.systemVersion < 0x0850)
//  		{
//  			tb_DoDialog(167);
//  			ExitToShell();
//  		}

	gDragTrackingHandlerUPP  = NewDragTrackingHandlerUPP((DragTrackingHandlerProcPtr) dragTrackingHandler);

	gDragReceiveHandlerUPP   = NewDragReceiveHandlerUPP((DragReceiveHandlerProcPtr) dragReceiveHandler);

	if (NSpInitialize == nil)
	{
		tb_DoDialog(129);
		ExitToShell();
	}

#endif

#ifndef _SERVERONLY
	EnterMovies();

#endif

	tb_InitializeToolbox();

	e=GetDirectoryID	(AppFSSpec.vRefNum,AppFSSpec.parID,"\pOberin Graphics",&theDirID,&isDirectory);

	e=FSMakeFSSpec		(AppFSSpec.vRefNum, theDirID, "\pGeneral.pxt", &GraphicsFSSpec);

	theGraphRes=FSpOpenResFile 	(&GraphicsFSSpec, fsRdPerm );

	if (ResError()!=noErr)
	{
		tb_AlertMessage("Can't open graphics data.  'General.pxt' should be in the Oberin Graphics folder.");
		ExitToShell();
	}

#ifndef _SERVERONLY
	e=FSMakeFSSpec		(AppFSSpec.vRefNum, theDirID, "\pEmblems.pxt", &EmblemsFSSpec);

	theEmblemsRes=FSpOpenResFile 	(&EmblemsFSSpec, fsRdPerm );

	if (ResError()!=noErr)
	{
		tb_AlertMessage("Can't open graphics data.  'Emblems.pxt' should be in the Oberin Graphics folder.");
		ExitToShell();
	}

	e=GetDirectoryID	(AppFSSpec.vRefNum,AppFSSpec.parID,"\pOberin Sounds",&theDirID,&isDirectory);

	e=FSMakeFSSpec		(AppFSSpec.vRefNum, theDirID, "\pSounds.sds", &SoundFSSpec);
	theSoundRes=FSpOpenResFile 	(&SoundFSSpec, fsRdPerm );
	e = HSetVol(NULL, AppFSSpec.vRefNum, AppFSSpec.parID);
#endif

//	GetDateTime((unsigned long*) &qd.randSeed); // gs carbon

#ifdef _DEBUGGING
	tb_GetPicture(5128, true);
	tb_GetPicture(5129, true);
	tb_GetPicture(5144, true);
	tb_GetPicture(5145, true);
	tb_GetPicture(5148, true);
	tb_GetPicture(5149, true);
	tb_GetPicture(5150, true);
	tb_GetPicture(5151, true);
	tb_GetPicture(5173, true);
	tb_GetPicture(5174, true);
	tb_GetPicture(5180, true);
	tb_GetPicture(5181, true);
	tb_GetPicture(5182, true);
	tb_GetPicture(5183, true);
	tb_GetPicture(5184, true);
	tb_GetPicture(5185, true);
	tb_GetPicture(5217, true);
	tb_GetPicture(5218, true);
	tb_GetPicture(5219, true);
	tb_GetPicture(5221, true);
	tb_GetPicture(5225, true);
	tb_GetPicture(5226, true);
	tb_GetPicture(5222, true);
	tb_GetPicture(5152, true);
	tb_GetPicture(5153, true);
	tb_GetPicture(5224, true);
	tb_GetPicture(5235, true);
	tb_GetPicture(5236, true);
	tb_GetPicture(5227, true);
	tb_GetPicture(5228, true);
	tb_GetPicture(5231, true);
	tb_GetPicture(5232, true);
	tb_GetPicture(5233, true);
	tb_GetPicture(5234, true);
	tb_GetPicture(5237, true);
	tb_GetPicture(5238, true);
	tb_GetPicture(5300, true);
	tb_GetPicture(5301, true);
	tb_GetPicture(5500, true);
	tb_GetPicture(5501, true);
	tb_GetPicture(5550, true);
	tb_GetPicture(5551, true);
	//tb_GetPicture(5600, true);
	//tb_GetPicture(5601, true);
#endif

//	Error = AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, NewAEEventHandlerProc(MyHandleOApp), 0, false);
//	Error = AEInstallEventHandler(kCoreEventClass, kAEOp}ocuments,   NewAEEventHandlerProc(MyHandleODoc), 0, false);
//	Error = AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments,  NewAEEventHandlerProc(MyHandlePDoc), 0, false);
//	Error = AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, NewAEEventHandlerProc(MyHandleQuit), 0, false);

//	NewRoutineDescriptor((ProcPtr)(QuitAppleEventHandler),uppAEEventHandlerProcInfo, GetCurrentArchitecture());
//	err = AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP(QuitAppleEventHandler), 0, false );

	Error = AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, NewAEEventHandlerUPP(MyHandleOApp), 0, false);
	Error = AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,   NewAEEventHandlerUPP(MyHandleODoc), 0, false);
	Error = AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments,  NewAEEventHandlerUPP(MyHandlePDoc), 0, false);
	Error = AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP(MyHandleQuit), 0, false);


//AEEventHandlerUPP myUPP

//=NewRoutineDescriptor((ProcPtr)(QuitAppleEventHandler),uppAEEventHandlerProcInfo, GetCurrentArchitecture());


//     err = AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP(QuitAppleEventHandler), 0, false );


	//myGenericFilter	= (UniversalProcPtr)NewRoutineDescriptor((ProcPtr)&GenericFilter, uppModalFilterProcInfo, GetCurrentISA());

//	myGenericFilter	= (UniversalProcPtr)NewModalFilterUPP((ProcPtr)&GenericFilter, uppModalFilterProcInfo, GetCurrentISA());

//	myGenericFilter	= NewModalFilterUPP((ProcPtr)&GenericFilter);
	myGenericFilter=NewModalFilterUPP(GenericFilter);


	InitMLTE();

	Done 						= false;
	onLine					=	false;

#ifndef _SERVERONLY
	sp_InitSplash();
#endif

	for (i=0;i<kMaxWindows;i++)
		windowVisible[i]=false;

	gotFirstRefresh	= false;

	nextKeyCheck = 0;

	strcpy(buildString,__DATE__);

	strcat(buildString, " at ");

	strcat(buildString,__TIME__);

}

// ----------------------------------------------------------------- //
void startMusic (void)
// ----------------------------------------------------------------- //

{
	int										track;
	Str255								name;
	OSErr									err;
	long						      theDirID;
	TBoolean				      isDirectory;


	//if (musicOn==false)
	//  return;

	if (gMusicVolume==0)
		return;

	track=tb_Rnd(1,5);

	if (track==1)	tb_CtoPStr255("music 1",name);

	if (track==2)	tb_CtoPStr255("music 2",name);

	if (track==3)	tb_CtoPStr255("music 3",name);

	if (track==4)	tb_CtoPStr255("music 4",name);

	if (track==5)	tb_CtoPStr255("music 5",name);

	err=GetDirectoryID(AppFSSpec.vRefNum,AppFSSpec.parID,"\pOberin Sounds",&theDirID,&isDirectory);

	if (err!=noErr)
		return;

	err=FSMakeFSSpec(AppFSSpec.vRefNum, theDirID, name, &MusicFSSpec);

	if (err!=noErr)
		return;

	err=OpenMovieFile(&MusicFSSpec,&musicRefNum,fsRdPerm);

	if (err!=noErr)
		return;

	if (theMusic!=NULL)
		DisposeMovie(theMusic);

	theMusic=NULL;

	err=NewMovieFromFile(&theMusic,musicRefNum,0,0,0,0);

	if (err!=noErr)
		return;

	CloseMovieFile(musicRefNum);

	if (theMusic!=NULL)
	{
		SetMovieVolume(theMusic,gMusicVolume*36);
		StartMovie(theMusic);
	}
}

// ----------------------------------------------------------------- //
void MyQTModelessCallbackCallback (EventRecord */*theEvent*/, DialogPtr /*theDialog*/, SInt16 */*itemHit*/)
// ----------------------------------------------------------------- //

{



}

// ----------------------------------------------------------------- //
TBoolean EventFilter(EventRecord *theEvent, WindowPtr /*theFrontWindow*/)
// ----------------------------------------------------------------- //

{
	//Handle		editFieldHandle;
	//short		aType;
	Point		tempPt;
	Rect		editFieldRect;
	char 		theKey;
	//short 		theHiliteCode;

	//if ( ((WindowRecord *)theFrontWindow)->windowKind == dialogKind )
	//	GetDialogItem((DialogPtr)theFrontWindow, 1, &aType, &editFieldHandle, &editFieldRect);

	switch ( theEvent->what )
	{
		case mouseDown:
		case mouseUp:
			tempPt = theEvent->where;
			GlobalToLocal(&tempPt);

			/* Check if the mouseDown was in the editText field 		*/
			/* (like for text selection or cursor placement.  If 		*/
			/* the checkbox isn't checked, do nothing and return 		*/
			/* false so DialogSelect doesn't mess with it. 				*/

			if ( PtInRect(tempPt, &editFieldRect))
				return false;

			break;

		case keyDown:
		case autoKey:
			theKey = theEvent->message & charCodeMask;

			if ( (theEvent->modifiers & cmdKey) != 0 )
			{
				long menuResult;

				//AdjustMenus();
				menuResult = MenuKey(theKey);

				//if ( (menuResult  >> 16) != 0 )
				///{
				//	Boolean editOpPerformed = MenuCommand(menuResult);

				//	if ( editOpPerformed == true )
				/* You may ask yourself, "Why are we exiting when an 	*/
				/* edit operation is performed?"  Well, DialogSelect 	*/
				/* performs some automatic handling for any editText 	*/
				/* items that may be in the Dialog, and since we've 	*/
				/* already handled them in MenuCommand() we don't 		*/
				/* want  DialogSelect to do anything		 			*/
				//	return false;
				//}
			}
			else if ( ((theEvent->message) & charCodeMask) == 0x1B )
			{
				/* Was Cancel hit?  We could also check for Return 	*/
				/* or Enter being hit, but in this example the edit */
				/* text field is multi-lined, so I'm quitting on 	*/
				/* return.  You get the idea.. 						*/
				//FakeButtonHilite((DialogPtr)theFrontWindow, kCancelButton);
				//DisposeDialog((DialogPtr)theFrontWindow);
				//return false;
			}

			//if ( (((DialogPeek)theFrontWindow)->editField + 1 == kEditTextField)
			//	&& (GetControlValue(GetCtlHandle((DialogPtr)theFrontWindow, 1)) == false) )
			/* Finally, if any non-command keystrokes were 		*/
			/* entered and the edit field is disabled, exit. 	*/
			return false;

			break;

		case activateEvt:
			/* This is where we take care of hiliting the		*/
			/* controls according to whether or not the dialog 	*/
			/* is frontmost. 									*/
			//theFrontWindow = (WindowPtr)theEvent->message;
			//if ( (theEvent->modifiers & activeFlag) == true )
			//	theHiliteCode = 0;
			//else
			//	theHiliteCode = 255;

			//HiliteAllControls((WindowPtr)theFrontWindow, theHiliteCode);
			//InvalThisItemRect(theDialog, kEditTextField);

			return false;

	}

	/* If we haven't returned false by now, go ahead	*/
	/* and return true so DialogSelect can do its 		*/
	/* thing, like update the window, deal with an 		*/
	/* item hit, etc.									*/
	return true;
}

// ----------------------------------------------------------------- //
void DoDialogEvent(EventRecord */*theEvent*/)
// ----------------------------------------------------------------- //

{
	//DialogPtr	    theDialog = (DialogPtr)FrontWindow();
//	short		      theItem;
	//GrafPtr		    origPort;

	//GetPort(&origPort);
	//SetPortWindowPort(GetDialogWindow(chatDialog));

	//if ( EventFilter(theEvent, GetDialogWindow(chatDialog)) != false )
	//{
	//	if ( DialogSelect(theEvent, &chatDialog, &theItem) == true )
//		{
	//if ( theDialog != nil )
	//{
	//	ControlHandle	    tempHandle;
	//	short			        newTEItem = 1;
	//	Boolean			      newControlVal = true;

	//	switch ( theItem )
	//	{

	//		case 1:
	//			break;

	//	}
	//}
	//}
//	}

	//SetPort(origPort);
//	return;

//BailOut:
//	DebugStr((StringPtr)"\pGetCtlHandle in DoDialogEvent failed");
}

/*
// ----------------------------------------------------------------- //
pascal TBoolean GenericFilter (DialogPtr theDialog, EventRecord *theEvent, SInt16 *itemHit)
// ----------------------------------------------------------------- //

{ // gs carbon

	char						theChar;
	short						kind;
	Handle					item;
	TRect						box;

	switch (theEvent->what)
		{
			case keyDown:
				theChar=(char)(theEvent->message & charCodeMask);
				if ((((theEvent->modifiers & cmdKey) !=0) && (theChar=='.')) || (theChar==(char)27))
					{
						*itemHit=kCancelButton;
						GetDialogItem(theDialog, kCancelButton, &kind, &item, &box);
						HiliteControl((ControlHandle)item, 1);
						return true;
					};
				if ((theChar==(char)13) || (theChar==(char)3))
					{
						*itemHit=kOKButton;
						GetDialogItem(theDialog, 1, &kind, &item, &box);
						HiliteControl((ControlHandle)item, kOKButton);
						return true;
					};
				break;
			case updateEvt:

				{Update(GetDialogWindow(theDialog));
				SetPortWindowPort(GetDialogWindow(theDialog));
				//PenPat(&qd.black);
				DrawDialog(theDialog);
				//GetDialogItem(theDialog, kOKButton, &kind, &item, &box);
				//tb_InsetRect(&box, -4, -4);
				//PenSize(3,3);
				//FrameRoundRect(&box, 15, 15);
				}Update(GetDialogWindow(theDialog));
				//if (Window!=nil)
				//	{
				//		SetPortWindowPort(Window);
					//	InvalRect(&Window->portRect); // gs carbon
				//		{Update(Window);
				//		UpdateEvent(tbWindow);
				//		}Update(Window);
				//	}
				SetPortWindowPort(GetDialogWindow(theDialog));

				break;
		}
		return false;

}
*/
// ----------------------------------------------------------------- //
pascal TBoolean GenericFilter (DialogPtr theDialog, EventRecord *theEvent, SInt16 *itemHit)
// ----------------------------------------------------------------- //

{
	char						theChar;
	short						kind;
	Handle					item;
	TRect						box;
	BitMap 					screenBits;
	short						windowPart;
	Point						globalPt = theEvent->where;
	WindowPtr				wp;
	Pattern					black;

	GetQDGlobalsScreenBits(&screenBits);
	GetQDGlobalsBlack(&black);

	switch (theEvent->what)
	{
		case mouseDown:
			windowPart = FindWindow(globalPt, &wp);

			if (windowPart==inDrag)
			{
				InitCursor();
				SetPortWindowPort(wp);
				DragWindow(wp, theEvent->where, &screenBits.bounds);
				return (true);
			}

			if (windowPart==inGoAway)
			{
				SetPortWindowPort(wp);

				if (TrackGoAway(wp,globalPt))
					*itemHit=kCancelButton;

				return (true);
			}

			break;

		case keyDown:
			theChar=(char)(theEvent->message & charCodeMask);

			if ((((theEvent->modifiers & cmdKey) !=0) && (theChar=='.')) || (theChar==(char)27))
			{
				*itemHit=kCancelButton;
				GetDialogItem(theDialog, kCancelButton, &kind, &item, &box);
				HiliteControl((ControlHandle)item, 1);
				return true;
			};

			if ((theChar==(char)13) || (theChar==(char)3))
			{
				*itemHit=kOKButton;
				GetDialogItem(theDialog, 1, &kind, &item, &box);
				HiliteControl((ControlHandle)item, kOKButton);
				return true;
			};

			break;

		case updateEvt:
			BeginUpdate(GetDialogWindow(theDialog));

			SetPortDialogPort(theDialog);

			DrawDialog(theDialog);

			EndUpdate(GetDialogWindow(theDialog));

			//if (Window!=nil)
			//	{
			//		SetPortWindowPort(Window);
			//		{Update(Window);
			//		UpdateEvent(tbWindow);
			//		}Update(Window);
			//	}
			SetPortDialogPort(theDialog);

			break;
	}

	return false;

}

// ----------------------------------------------------------------- //
void SetUpMenus(void)
// ----------------------------------------------------------------- //

{
	//OSErr     e;
	short			Index;


	myMenus[kAppleM] 	= GetMenu(kAppleID);
//	App}ResMenu(myMenus[kAppleM], 'DRVR');

	myMenus[kFileM] 				= GetMenu(kFileID);
	myMenus[kEditM] 				= GetMenu(kEditID);
	myMenus[kActionM] 			= GetMenu(kActionID);
	myMenus[kViewM] 				= GetMenu(kViewID);
	myMenus[kPrefsM] 				= GetMenu(kPrefsID);

	myMenus[kSoundM] 				= GetMenu(kSoundID);
	myMenus[kMusicM] 				= GetMenu(kMusicID);

	for (Index=kAppleM; Index<=kPrefsM; Index++)
		InsertMenu(myMenus[Index], 0);

	InsertMenu(myMenus[kSoundM], -1);

	InsertMenu(myMenus[kMusicM], -1);

	if (usingX==false)
	{
		AppendMenuItemText(myMenus[kFileM], "\p-");
		AppendMenuItemText(myMenus[kFileM], "\pQuit");
		SetItemCmd(myMenus[kFileM],5,'Q');
	}

//	e=HMGetHelpMenuHandle(&theHelpMenu);
//	origHelpItems=CountMenuItems(theHelpMenu);
//	App}Menu(theHelpMenu, "\pCall Game Master");
//	App}Menu(theHelpMenu, "\pOberin Website...");
//	callGMItem=origHelpItems+1;
//	oberinWebsiteItem=origHelpItems+2;
	DrawMenuBar();

}

// ----------------------------------------------------------------- //
void DoMenus (int32 menuResult)
// ----------------------------------------------------------------- //

{
	int16 				theMenu 								= (menuResult >> 16);
	int16 				theItem 								= (menuResult & 0x0000FFFF);
	Str255				name;
	//int           		playerIndex;
	int           		top;
	//int           		temp;
	Str255				quitTitle="\pUpdate in process", quitString="\pDo you really want to abort the update process?";
	short				itemHit=0;

	tb_SetCursor(0);

	switch (theMenu)
	{

		case kAppleID:
		{
			GetMenuItemText(myMenus[kAppleM], theItem, name);
			//if (theItem>1)
			//	Op}eskAcc(name);
			//else

			if (theItem==1)
			{
				//tb_DoDialog(134);
#ifndef _SERVERONLY
				gr_OpenFloatingWindow(tbAboutWindow);
				windowVisible[kAboutWindow]=true;
				UpdateEvent(tbAboutWindow);
#endif
				//gr_AddText("--------------------------",true,true);
				//gr_AddText("Oberin version 0.1.7b2.",true,true);
				//gr_AddText("--------------------------",true,true);
			}
		}

		break;

		case kFileID:

			switch (theItem)
			{

				case kCloseItem:
				{
					top=gr_TopOfWindowStack();

					if (top>0)
						gr_CloseFloatingWindowID(top);
				}

				break;

				case kLogOffItem:
				{
#ifndef _SERVERONLY
													
					sh_ClearShopContents();
					sh_ClearSellList();
					gr_CloseFloatingWindow(tbShopContentsWindow);
					gr_CloseFloatingWindow(tbSellListWindow);
					ClearPlayerAction();
				
				
				
					logTime=TickCount()-100*60; //force text log
					fi_WriteToTextLog("");
					nw_SendAction(kLogOff, kHighPriority, kSendNow,true);
					nw_ShutdownNetworking();
					onLine=false;
					//gr_HideAllWindows();
					windowVisible[kChessWindow]         = false;
					windowVisible[kShopContentsWindow]  = false;
					windowVisible[kSellListWindow]      = false;
					windowVisible[kMakeWindow]          = false;
					windowVisible[kPackContentsWindow]  = false;
					windowVisible[kQuestWindow]         = false;
					windowVisible[kIDWindow]            = false;
					windowVisible[kBankWindow]          = false;
					gr_TempHideAllWindows();
					ShowWindow(logonWindow);
					sp_InitSplash();
					UpdateEvent(tbLogOnWindow);
					
#endif
				}

				break;

				case kQuit:
					logTime=TickCount()-100*60; //force text log
					fi_WriteToTextLog("");
					Done=true;
					break;

			}

			break;

		case kEditID:
			break;

		case kActionID:
		{
#ifndef _SERVERONLY

			if (theItem==kPickUpItem && !playerDead)  //----------------------------
			{
				if (!in_AnythingInPickUpList())
					nw_SendAction(kGetContainerContents, kHighPriority, kSendNow,true);

				gr_OpenFloatingWindow(tbPackContentsWindow);

				windowVisible[kPackContentsWindow]=true;

				UpdateEvent(tbPackContentsWindow);
			}

			if (theItem==kPickUpAll && !playerDead)  //----------------------------
			{
				if (standingOnContainer)
				{
					nw_SendAction(kPickUp, kNormalPriority, kCanWait,true);
					in_ClearPickUpPack();

					if (currentInfoDisplay==kPackContentsDisplay)
					{
						currentInfoDisplay=kCharacterDisplay;
						gr_RenderPlayerDisplay(true);
					}
				}
			}

			if (theItem==kBuyItem)  //----------------------------
			{
				if (!sh_AnythingInShopList())
					nw_SendAction(kGetShopContents, kHighPriority, kSendNow,true);

				gr_OpenFloatingWindow(tbShopContentsWindow);

				windowVisible[kShopContentsWindow]=true;

				UpdateEvent(tbShopContentsWindow);
			}

			if (theItem==kSellItem)  //----------------------------
			{
				if (!sh_AnythingInSellList())
					nw_SendAction(kGetSellList, kHighPriority, kSendNow,true);

				gr_OpenFloatingWindow(tbSellListWindow);

				windowVisible[kSellListWindow]=true;

				UpdateEvent(tbSellListWindow);
			}

			if (theItem==kChessItem)  //----------------------------
			{
				if (!ac_AnythingInActionDisplay())
					nw_SendAction(kGetActionUpdate, kHighPriority, kSendNow,true);

				gr_OpenFloatingWindow(tbChessWindow);

				windowVisible[kChessWindow]=true;

				UpdateEvent(tbChessWindow);
			}

			if (theItem==kIdentifyItem)  //----------------------------
			{
				targetMode=kIDTarget;
			}

			if (theItem==kRepairItem)  //----------------------------
			{
				ClearPlayerAction();
				gr_OpenFloatingWindow(tbInventoryWindow);
				windowVisible[kInventoryWindow]=true;
				gr_RenderInventoryWindow();
				gr_AddText("Target an item in your inventory for repair...",false,false);
				targetMode=kInventoryTarget;
			}

#endif
		}

		break;

		case kViewID:
		{
#ifndef _SERVERONLY

			if (theItem==kMapItem)  //----------------------------
			{
				gr_OpenFloatingWindow(tbMapWindow);
				windowVisible[kMapWindow]=true;
				UpdateEvent(tbMapWindow);
			}

			if (theItem==kInventoryItem)  //----------------------------
			{
				gr_OpenFloatingWindow(tbInventoryWindow);
				windowVisible[kInventoryWindow]=true;
				UpdateEvent(tbInventoryWindow);
			}

			if (theItem==kCalendarItem)  //----------------------------
			{
				gr_OpenFloatingWindow(tbCalendarWindow);
				windowVisible[kCalendarWindow]=true;
				UpdateEvent(tbCalendarWindow);
			}

			if (theItem==kSkillsItem)  //----------------------------
			{
				gr_OpenFloatingWindow(tbSkillsWindow);
				windowVisible[kSkillsWindow]=true;
				UpdateEvent(tbSkillsWindow);
			}

			if (theItem==kStatsItem)  //----------------------------
			{
				gr_OpenFloatingWindow(tbStatsWindow);
				windowVisible[kStatsWindow]=true;
				UpdateEvent(tbStatsWindow);
			}

			if (theItem==kTextLogItem)  //----------------------------
			{
				gr_OpenFloatingWindow(tbChatWindow);
				windowVisible[kChatWindow]=true;
				UpdateEvent(tbChatWindow);
			}

			if (theItem==kCharacterItem)  //----------------------------
			{
				gr_OpenFloatingWindow(tbCharacterWindow);
				windowVisible[kCharacterWindow]=true;
				UpdateEvent(tbCharacterWindow);
			}

			if (theItem==kBankAccountItem)  //----------------------------
			{
				if (!in_AnythingInBankList())
					nw_SendAction(kGetBankContents, kHighPriority, kSendNow,true);

				gr_OpenFloatingWindow(tbBankWindow);

				windowVisible[kBankWindow]=true;

				UpdateEvent(tbBankWindow);
			}

			if (theItem==kHealthBarsItem)  //----------------------------
				drawHealthBars=!drawHealthBars;

			if (theItem==kPartyItem)  //----------------------------
			{
				gr_OpenFloatingWindow(tbPartyWindow);
				windowVisible[kPartyWindow]=true;
				UpdateEvent(tbPartyWindow);
			}

			if (theItem==kPetsItem)  //----------------------------
			{
				gr_OpenFloatingWindow(tbPetsWindow);
				windowVisible[kPetsWindow]=true;
				UpdateEvent(tbPetsWindow);
			}

#endif
		}

		break;

		case kPrefsID:
		{
#ifndef _SERVERONLY
			// if (theItem==kSoundItem)  //----------------------------
			// 	tb_SoundOn=!tb_SoundOn;

			// if (theItem==kMusicItem)  //----------------------------
			//   {
			//		  if (musicOn)
			//	    {
			//		    musicOn=false;
			//        if (theMusic!=NULL)
			//	        StopMovie(theMusic);
			//         cm_WritePrefs();
			//		  }
			//		else
			//		  {
			//		    musicOn=true;
			//         startMusic();
			//         cm_WritePrefs();
			//			  }
			//   }

			if (theItem==kPathModeItem)  //----------------------------
				pathFind=!pathFind;

			if (theItem==kLogTextItem)  //----------------------------
				logText=!logText;

			if (theItem==kKeyPadItem)  //----------------------------
				keyPadControl=!keyPadControl;

			if (theItem==kTextBacksItem)  //----------------------------
				darkTextBacks=!darkTextBacks;

			if (theItem==kBgNoteItem)
				backgroundnotification=!backgroundnotification;

			if (theItem==kColorOwnNameItem)
				colorOwnName=!colorOwnName;
			
			if (theItem==kDisplayFeedbackItem)
				displayFeedback=!displayFeedback;

			//if (theItem==kSkipMessagesItem)  //----------------------------
			//	skipOldMessages=!skipOldMessages;
#endif
		}

		break;


		case kSoundID:
		{
#ifndef _SERVERONLY
			gSoundVolume=theItem-1;
			tb_PlaySound(9999);
			cm_WritePrefs();
#endif
		}

		break;

		case kMusicID:
		{
#ifndef _SERVERONLY

			if ((gMusicVolume==0) && (theItem>1))
			{
				gMusicVolume=theItem-1;
				startMusic();

				if (theMusic!=NULL)
					SetMovieVolume(theMusic,gMusicVolume*36);

				cm_WritePrefs();
			}
			else if ((gMusicVolume>0) && (theItem==1))
			{
				gMusicVolume=theItem-1;

				if (theMusic!=NULL)
					StopMovie(theMusic);

				cm_WritePrefs();
			}
			else
			{
				gMusicVolume=theItem-1;

				if (theMusic!=NULL)
					SetMovieVolume(theMusic,gMusicVolume*36);

				cm_WritePrefs();
			}

#endif
		}

		break;

		case kHelpID:
		{
#ifndef _SERVERONLY

			if (theItem==callGMItem)
			{
				nw_SendAction(kHelp, kHighPriority, kSendNow,false);
			}

			if (theItem==oberinWebsiteItem)
			{
				tb_LaunchURL("\phttp://www.oberin.com/");
			}

#endif
		}

		break;

	}

	HiliteMenu(0);

}

// ----------------------------------------------------------------- //
void DoMouseDown (EventRecord *theEvent)
// ----------------------------------------------------------------- //

{
	Point									globalPt = theEvent->where;
	short									windowPart;
	WindowPtr							wp;
	int										match=-1;
	WindowClass           windowClass;
	//TRect									r;

	gMouseDownRef=theEvent;

//	if (flag)
//		SystemTask();

//HiliteWindowFrameForDrag(mapWindow,false);

	flag=true;

	windowPart = FindWindow(globalPt, &wp);

	switch (windowPart)
	{
		case inMenuBar:
		{
			tb_SetCursor(0);
			DoMenus(MenuSelect(globalPt));
		}

		break;

		//case inSysWindow:
		//	SystemClick(theEvent, wp);
		//break;

		case inDrag:
		{
			SetPortWindowPort(wp);
#ifndef _SERVERONLY
			GetWindowClass(wp,&windowClass);

			if (windowClass==kFloatingWindowClass)
			{
				match=tb_GetWindowMatch(wp);
				gr_HiliteToolboxWindow(match);
				SelectWindow(wp);
				//HiliteWindow(wp,true);
			}

#endif
			DragWindow(wp, theEvent->where, &screenBounds);

#ifndef _SERVERONLY
			gr_SaveWindowPrefs();

#endif
		}

		break;

		case inGoAway:
		{
			SetPortWindowPort(wp);
			//GlobalToLocal(&globalPt);

			if (TrackGoAway(wp,globalPt))
			{
				match=tb_GetWindowMatch(wp);
				gr_CloseFloatingWindow(match);
				//HideWindow(wp);
				//if (match==tbMapWindow)
				//  windowVisible[kMapWindow]=false;
				//if (match==tbInventoryWindow)
				//  windowVisible[kInventoryWindow]=false;
				//if (match==tbPackContentsWindow)
				//  windowVisible[kPackContentsWindow]=false;
				gr_SaveWindowPrefs();
			}
		}

		break;

		case inContent:
		{
			SetPortWindowPort(wp);
			GetWindowClass(wp,&windowClass);
			//if (wp==Window)
			//  {
			//    TXNClick(textObject,theEvent);
			//  }

			if (windowClass==kFloatingWindowClass)
			{
				match=tb_GetWindowMatch(wp);
				gr_HiliteToolboxWindow(match);
				GlobalToLocal(&globalPt);

				if (FrontWindow()!=wp)
				{
					SelectWindow(wp);
					//HiliteWindow(wp,true);
					match=tb_GetWindowMatch(wp);
					//UpdateEvent(match);
					return;
				}

				if (applicationSuspended==false)
				{


#ifndef _SERVERONLY

					if ((match==tbInventoryWindow) && (!playerDead))
					{
						MouseDownEventInventoryWindow(globalPt);
					}

					if ((match==tbPackContentsWindow) && (!playerDead))
					{
						MouseDownEventPackContentsWindow(globalPt);
					}

					if ((match==tbShopContentsWindow) && (!playerDead))
					{
						MouseDownEventShopContentsWindow(globalPt);
					}

					if ((match==tbSellListWindow) && (!playerDead))
					{
						MouseDownEventSellListWindow(globalPt);
					}

					if ((match==tbSkillsWindow) && (!playerDead))
					{
						MouseDownEventSkillsWindow(globalPt);
					}

					if ((match==tbChessWindow) && (!playerDead))
					{
						MouseDownEventChessWindow(globalPt);
					}

					if ((match==tbBankWindow) && (!playerDead))
					{
						MouseDownEventBankWindow(globalPt);
					}

					if ((match==tbMakeWindow) && (!playerDead))
					{
						MouseDownEventMakeWindow(globalPt);
					}

					if (match==tbChatWindow)
					{
						if (gr_CheckMouseOnUpButton(globalPt))              tb_Idle();
						else if (gr_CheckMouseOnDownButton(globalPt))       tb_Idle();
						else if (gr_DragThumb(globalPt))                    tb_Idle();
						else if (gr_CheckMouseOnPageUpButton(globalPt))     tb_Idle();
						else if (gr_CheckMouseOnPageDownButton(globalPt))   tb_Idle();
					}

#endif
					tb_FlushMouseEvents();
				}
			}
			else
			{
				GlobalToLocal(&globalPt);

				if (applicationSuspended==false)
				{
					match=tb_GetWindowMatch(wp);

					if (match==tbWindow)
					{
						MouseDownEvent(tbWindow,globalPt);
						tb_FlushMouseEvents();
					}

#ifndef _SERVERONLY
					if (match==tbLogOnWindow)
					{
						sp_SplashMouseDownEvent(tbLogOnWindow,globalPt);
						tb_FlushMouseEvents();
					}

#endif
				}
			}
		}

		break;

	}

	flag=false;

}

// ----------------------------------------------------------------- //
void DoKey (EventRecord *theEvent,TBoolean repeatKey)
// ----------------------------------------------------------------- //

{

	char				charPressed = (theEvent->message & charCodeMask);
	UInt16			keyPressed 	= (UInt16)((theEvent->message & keyCodeMask)>>8);

	if (theEvent->modifiers & cmdKey)
	{
		DoMenus(MenuKey(charPressed));
	}
	else
		KeyDownEvent (charPressed,repeatKey,keyPressed);

}

// ----------------------------------------------------------------- //
void DoActivate (EventRecord *theEvent)
// ----------------------------------------------------------------- //

{

	WindowPtr			wp = (WindowPtr)theEvent->message;
	Boolean				activeFlag = theEvent->modifiers & 1;

	SetPortWindowPort(wp);

}

// ----------------------------------------------------------------- //
void DoSuspendResume(Boolean InForeground)
// ----------------------------------------------------------------- //

{
	//OSErr       e;
	//int					top;

	if (InForeground) // resume
	{
		applicationSuspended=false;
//			gr_Resume(); // 11/18
//			gr_CloseFloatingWindow(0);	// activates top floating window
		//gr_SelectFrontWindow();
#ifndef _SERVERONLY

		if (gMusicVolume>0)
			if (theMusic!=NULL)
				StartMovie(theMusic);

#endif
		FlushEvents(mDownMask + mUpMask, 0);
	}
	else						 // Suspend
	{
		applicationSuspended=true;
#ifndef _SERVERONLY

		if (gMusicVolume>0)
			if (theMusic!=NULL)
				StopMovie(theMusic);

#endif
	}

}

// ----------------------------------------------------------------- //
void SetWindowMenuChecks(void)
// ----------------------------------------------------------------- //

{

	int				i;
	int				c;

	GetKeyEvents();

	if (onLine==false)
	{
		DisableMenuItem(myMenus[kFileM],kCloseItem);
		DisableMenuItem(myMenus[kFileM],kLogOffItem);
		c=CountMenuItems(myMenus[kActionM]);

		for (i=1;i<=c;i++)
			DisableMenuItem(myMenus[kActionM],i);

		c=CountMenuItems(myMenus[kViewM]);

		for (i=1;i<=c;i++)
			DisableMenuItem(myMenus[kViewM],i);

		DisableMenuItem(theHelpMenu,callGMItem);
	}
	else
	{
		if (gr_TopOfWindowStack()>0)
			EnableMenuItem(myMenus[kFileM],kCloseItem);
		else
			DisableMenuItem(myMenus[kFileM],kCloseItem);

		EnableMenuItem(myMenus[kFileM],kLogOffItem);

		c=CountMenuItems(myMenus[kActionM]);

		for (i=1;i<=c;i++)
			EnableMenuItem(myMenus[kActionM],i);

		c=CountMenuItems(myMenus[kViewM]);

		for (i=1;i<=c;i++)
			EnableMenuItem(myMenus[kViewM],i);

		EnableMenuItem(theHelpMenu,callGMItem);
	}

	for (i=1;i<=CountMenuItems(myMenus[kSoundM]);i++)
		CheckMenuItem(myMenus[kSoundM],i,false);

	CheckMenuItem(myMenus[kSoundM],gSoundVolume+1,true);

	for (i=1;i<=CountMenuItems(myMenus[kMusicM]);i++)
		CheckMenuItem(myMenus[kMusicM],i,false);

	CheckMenuItem(myMenus[kMusicM],gMusicVolume+1,true);

	//CheckMenuItem(myMenus[kPrefsM],kSoundItem,tb_SoundOn);
	//CheckMenuItem(myMenus[kPrefsM],kMusicItem,musicOn);
	CheckMenuItem(myMenus[kPrefsM],kPathModeItem,pathFind);

	CheckMenuItem(myMenus[kPrefsM],kLogTextItem,logText);

	CheckMenuItem(myMenus[kPrefsM],kKeyPadItem,keyPadControl);

	CheckMenuItem(myMenus[kPrefsM],kTextBacksItem,!darkTextBacks);

	CheckMenuItem(myMenus[kPrefsM],kBgNoteItem,backgroundnotification);

	CheckMenuItem(myMenus[kPrefsM],kColorOwnNameItem,colorOwnName);

	CheckMenuItem(myMenus[kPrefsM],kDisplayFeedbackItem,displayFeedback);
	
	//CheckMenuItem(myMenus[kPrefsM],kSkipMessagesItem,skipOldMessages);

	CheckMenuItem(myMenus[kViewM],kHealthBarsItem,drawHealthBars);	

	if (standingOnContainer)
	{
		EnableMenuItem(myMenus[kActionM],kPickUpItem);
		EnableMenuItem(myMenus[kActionM],kPickUpAll);
	}
	else
	{
		DisableMenuItem(myMenus[kActionM],kPickUpItem);
		DisableMenuItem(myMenus[kActionM],kPickUpAll);
	}

	if (standingInShop)
	{
		EnableMenuItem(myMenus[kActionM],kBuyItem);
		EnableMenuItem(myMenus[kActionM],kSellItem);
	}
	else
	{
		DisableMenuItem(myMenus[kActionM],kBuyItem);
		DisableMenuItem(myMenus[kActionM],kSellItem);
	}

	if (standingOnActionSpot)
		EnableMenuItem(myMenus[kActionM],kChessItem);
	else
		DisableMenuItem(myMenus[kActionM],kChessItem);

	if (standingInBank)
		EnableMenuItem(myMenus[kViewM],kBankAccountItem);
	else
		DisableMenuItem(myMenus[kViewM],kBankAccountItem);

	/* 11/18
	  if (playerDead)
	    {
	  	  DisableMenuItem(myMenus[kActionM],kPickUpItem);
	  	  DisableMenuItem(myMenus[kActionM],kPickUpAll);
	  	  DisableMenuItem(myMenus[kActionM],kBuyItem);
	  	  DisableMenuItem(myMenus[kActionM],kSellItem);
	  	  DisableMenuItem(myMenus[kActionM],kIdentifyItem);
	  	  DisableMenuItem(myMenus[kActionM],kChessItem);
	  	  DisableMenuItem(myMenus[kActionM],kRepairItem);

	  	  DisableMenuItem(myMenus[kViewM],kInventoryItem);
	  	  DisableMenuItem(myMenus[kViewM],kSkillsItem);
	  	  DisableMenuItem(myMenus[kViewM],kStatsItem);
	  	  DisableMenuItem(myMenus[kViewM],kCharacterItem);
	  	  DisableMenuItem(myMenus[kViewM],kBankAccountItem);
	  	  DisableMenuItem(myMenus[kViewM],kPetsItem);
	    }
	*/

}


// ----------------------------------------------------------------- //
void GetKeyEvents(void)
// ----------------------------------------------------------------- //

{
	EventRecord						theEvent;
	static  long					sleepTime = 0L;

	//if (!usingX)

	if (TickCount()<nextKeyCheck)
		return;

	if (WaitNextEvent(keyDownMask+autoKeyMask, &theEvent, sleepTime, NULL))
	{
		switch (theEvent.what)
		{
			case keyDown:
			{
				DoKey(&theEvent,false);
			}

			break;

			case autoKey:
			{
				DoKey(&theEvent,true);
			}

			break;
		}
	}

	if (usingX)
		nextKeyCheck=TickCount()+2;
	else
		nextKeyCheck=TickCount()+15;

}

// ----------------------------------------------------------------- //
void MainLoop()
// ----------------------------------------------------------------- //

{

	EventRecord						theEvent;
	static  long					sleepTime = 1L; //0;//1L;
	int										match=-1;
	TBoolean							hiEvent=false;
#ifdef _SERVERONLY
	unsigned long         temp;
#endif
	//int										i;
	//short                 chatItemHit;
	//unsigned long			nextQuarter;


#ifdef	_SERVERONLY
	unsigned long					statTime;

#endif

#ifdef	_SERVERONLY
	maxMessageSize=0;
	maxPlayersOnline=0;
	numberOfLogoffs=0;
	gracefulLogoffs=0;
	failedLogons=0;
	serverFull=0;
	exceededNT=0;
	skillGain=0;
#endif

#ifdef	_SERVERONLY
	temp						= TickCount();
	decayTime				=	0;
	resourceTime		=	0;
	refreshTime			=	0;
	monsterTime			=	0;
	rotateShopTime	= 0;
	serverFXTime		= 0;
	messagesReceived= 0;
	messagesSent		= 0;
	bytesSent				= 0;
	bytesReceived		= 0;
	cycles					= 0;
#endif
	UpdateEvent(0);

#ifdef _SERVERONLY
	statTime=TickCount()+60;
	cycles=0;
#endif

	fi_WriteToTraceLog("Entering Main Loop-");

	while (!Done)
	{

#ifndef _SERVERONLY
		SetWindowMenuChecks();

		if (gMusicVolume>0)
			if (theMusic!=NULL)
			{
				MoviesTask(theMusic,0);

				if (IsMovieDone(theMusic))
					startMusic();
			}

#endif

#ifdef _SERVERONLY
		fi_WriteToTraceLog("A");

		in_CheckContainerDecay();

		nw_GetMessages();

		fi_WriteToTraceLog("B");

		rs_RefreshResources();

		nw_GetMessages();

		fi_WriteToTraceLog("C");

		sh_RotateShopInventory();

		nw_GetMessages();

		fi_WriteToTraceLog("D");

		cr_CheckCreaturePopulation();

		nw_GetMessages();

		fi_WriteToTraceLog("E");

		nw_SendRefreshToAllPlayers();

		nw_GetMessages();

		fi_WriteToTraceLog("F");

		cr_MoveCreatures();

		nw_GetMessages();

		fi_WriteToTraceLog("G");

		fx_ServiceFX();

		nw_GetMessages();

		fi_WriteToTraceLog("H");

		cm_HouseKeeping();

		nw_GetMessages();

		cycles++;

		if (TickCount()>=statTime)
		{
			gr_ShowServerStats();
			messagesReceived= 0;
			messagesSent		= 0;
			bytesSent				= 0;
			bytesReceived		= 0;
			cycles					=	0;
			statTime=TickCount()+60;
		}



#endif

#ifndef _SERVERONLY
		NullEvent();

		if (onLine)
		{
			GetKeyEvents();
			nw_GetMessages();
			GetKeyEvents();
			CheckKeys(false,0);
		}
		else
			sp_SplashNullEvent();

#endif

		rs_GetCurrentCalendar();

		if (WaitNextEvent(everyEvent, &theEvent, sleepTime, NULL))
		{
#ifndef _SERVER

			if (gUpdating)
			{
				DrawControls(GetDialogWindow(gProgressWindow));

				if ( IsDialogEvent(&theEvent) == true )
					if (theEvent.what==mouseDown)
					{
						up_DialogEvent(&theEvent);
						//	  continue;
					}
			}

#endif


			switch (theEvent.what)
			{

				case mouseDown:
				{
					DoMouseDown(&theEvent);
					hiEvent=true;
				}

				break;

				case keyDown:
				{
					DoKey(&theEvent,false);
					hiEvent=true;
				}

				break;

				case autoKey:
				{
					DoKey(&theEvent,true);
					hiEvent=true;
				}

				break;

				case activateEvt:
				{
					DoActivate(&theEvent);
					hiEvent=true;
				}

				break;

				case updateEvt:
				{
					match=tb_GetWindowMatch((WindowPtr)theEvent.message);

					if (match!=-1)
					{
						BeginUpdate((WindowPtr)theEvent.message);
						EndUpdate((WindowPtr)theEvent.message);

						if ((screenRect.right!=screenBounds.right) || (screenRect.bottom!=screenBounds.bottom))
						{
						}

						UpdateEvent(match);
					}

					hiEvent=true;
				}

				break;

				case osEvt:
				{
					if ((theEvent.message >> 24) & suspendResumeMessage )
						DoSuspendResume((theEvent.message & resumeFlag)!=0);

					hiEvent=true;
				}

				break;

				case kHighLevelEvent:
				{
					(void)AEProcessAppleEvent(&theEvent);
					hiEvent=true;
				}

			}
		}

		//DoIdle();

//gr_ShowDebugLocation(9); // server only
		//fi_WriteToDebuggerLog("}");

	}
}

// ----------------------------------------------------------------- //
void tb_DrawSplashPictureToDialog(void)
// ----------------------------------------------------------------- //

{
	TRect					r;


	SetRect(&r, 0, 0, 0, 0);
	PHandle = GetPicture(175);

	if (PHandle != nil)
	{
		HLock((Handle)PHandle);
		r.right = r.left + ((**(PHandle)).picFrame.right - (**(PHandle)).picFrame.left);
		r.bottom = r.top + ((**(PHandle)).picFrame.bottom - (**(PHandle)).picFrame.top);
		OffsetRect(&r, (screenBounds.right-r.right)/2, (screenBounds.bottom-r.bottom)/2);
		SetPortWindowPort(GetDialogWindow(theDialog));
		DrawPicture(PHandle, &r);
		HUnlock((Handle)PHandle);
		ReleaseResource((Handle)PHandle);
	}

}

// ----------------------------------------------------------------- //
void sh_CloseResFiles(void)
// ----------------------------------------------------------------- //
{
	CloseResFile(theGraphRes);
#ifndef _SERVERONLY
	CloseResFile(theSoundRes);
	CloseResFile(theEmblemsRes);
#endif
}



/*
// ----------------------------------------------------------------- //
void sh_CheatCheck(void)
// ----------------------------------------------------------------- //

{
	int							        i;
	int							        j;
	int							        button = -2;
	TBitmap					        tmp;
	TRect						        tr;
	TRGBColor				        c;
	TRect						        nr;
	char						        buf[tb_StringLength];
	TPoint					        mpt;
	TRect						        destRect;
	TRect						        r;
	OSErr						        err;
	FInfo						        fileInfo;
  short 									actItemCount;
  short 									itemIndex;
  long										theDirID;
  TBoolean								isDirectory;
	char						        name[64];

	itemIndex=1;
	err=GetDirectoryID(AppFSSpec.vRefNum,AppFSSpec.parID,"\pAccounts Check",&theDirID,&isDirectory);
	err=GetDirItems(AppFSSpec.vRefNum, AppFSSpec.parID, "\pAccounts Check", true, false, &items[0], 5000, &actItemCount, &itemIndex);
	j=0;
  checkCount=0;
  checkCount1=0;
  checkCount2=0;
  checkCount3=0;
  checkCount4=0;
  checkCount5=0;
  checkCount6=0;
  checkCount7=0;
  checkCount8=0;
  checkCount9=0;
  checkCount10=0;
  checkCount11=0;
  checkCount12=0;
  checkCount13=0;
  checkCount14=0;
  checkCount15=0;
  checkGold=0;
  checkMaxGold=0;
	for (i=0;i<actItemCount;i++)
		{
			FSpGetFInfo(&items[i],&fileInfo);
			tb_PascaltoCStr(items[i].name,name);
      fi_ScanFile(name);
		}
  tb_Halt();

}
*/
// ----------------------------------------------------------------- //
void main(void)
// ----------------------------------------------------------------- //

{

	//char                					testStr[kStringLength];
	//long                					num;
	//Handle								ipHandle;
	//int									i;
	//char								ipStr[256];
	//int									len;

	LaunchParamBlockRec					serverWatcher;
	FSSpec								theWatcher;
	Str255								watcherLoc="\p:Oberin Server Watcher";
	OSStatus								theErr;
	AppleEvent							someAE;
	ProcessSerialNumber					watcherPid = { 0, kNoProcess };
	AEAddressDesc       					theTarget;

	CFStringRef		theName, desiredName=CFSTR("Oberin Server Watcher");
	int				found=false;


	Initialize();

	SetUpMenus();

	/*
		ipHandle=NewHandle(0);
		if (URLSimpleDownload ("http://www.net.nih.gov/cgi-bin/wwwhoami",NULL,ipHandle,0,NULL,NULL)==noErr)
			{
	 			len = GetHandleSize( ipHandle );
	 			if (len>256) len=256;
	    	BlockMoveData( *ipHandle, ipStr, len );
				ipStr[len-1]='\0';
			}
	*/

	dummyStream = fopen(":Oberin Logs:Oberin Error Log", "r" );

	if (dummyStream == NULL)
	{
		fclose( dummyStream );
		fi_WriteToErrorLog("----------------------------------------------------------");
	}
	else
		fclose( dummyStream );

#ifdef _SERVER
	fi_LoadBanFile();

	//fi_OpenConversationLog();
	fi_OpenGMLog();

#endif

	fi_WriteToErrorLog(" ");

	fi_WriteToErrorLog("New session ----------------------------------------------");

	fi_WriteToErrorLog("Start Oberin Client Version 018");

	//fi_WriteToCSLog("New session ----------------------------------------------\n");

	nextSwitch=TickCount()+(60*30);

#ifndef _SERVERONLY
	currentSkillDisplay		= 99;

	currentCategory				= 99;

	cm_ReadPrefs();

#endif

	BeginEvent();

	OpenWindow();

	//{Event();

#ifndef _SERVERONLY
	// ------------- music ------------- //
	theMusic=NULL;

	if (gMusicVolume>0)
		startMusic();

#endif

	tb_FlushMouseEvents();

	FlushEvents(updateMask, 0);

#ifdef _SERVER

	/* Load server watcher */

	theErr=FSMakeFSSpec(0,0,watcherLoc,&theWatcher);

	if (theErr==noErr)
	{
		serverWatcher.launchBlockID=extendedBlock;
		serverWatcher.launchEPBLength=extendedBlockLen;
		serverWatcher.launchControlFlags=launchContinue;
		serverWatcher.launchAppSpec=&theWatcher;
		serverWatcher.launchAppParameters=NULL;

		theErr=LaunchApplication(&serverWatcher);

		if (theErr==noErr)
			watcherPid=serverWatcher.launchProcessSN;
		else
		{
			//Server restarted the watcher, find me plzkthxbye

			theErr=GetNextProcess(&watcherPid);

			while (theErr!=procNotFound && !found)
			{
				CopyProcessName(&watcherPid, &theName);

				if (CFStringCompare(theName, desiredName, 0)==0)
					found=true;

				if (!found)
					theErr=GetNextProcess(&watcherPid);
			}

		}

	}

	/***********************/

#endif

	MainLoop();

#ifndef _SERVERONLY
	fi_CloseMapFile();

	if (gNetGame)
		nw_SendAction(kLogOff, kHighPriority, kSendNow,true);

	if (gMusicVolume>0)
		if (theMusic!=NULL)
			StopMovie(theMusic);

#endif

#ifdef _SERVERONLY
	fi_CloseGMLog();

	//fi_CloseConversationLog();
	for (i=1;i<kMaxPlayers;i++)
		//if (player[i].online)
		if (player[i].active) //018
		{
			fi_Logout(i);
			nw_RemovePlayer(player[i].id);
			pl_ClearPlayerRecord(i);
			gr_DrawServerNameAt(i);
		}

#endif

#ifdef _SERVER
	fi_SaveAllContainers();

	//fi_SaveContainer(2294,1919);
	//fi_SaveContainer(2294,1934);
	//fi_SaveContainer(2285,1924);
	//fi_SaveContainer(2285,1923);
	//fi_SaveContainer(2285,1922);
	//fi_SaveContainer(2285,1921);
	//fi_SaveContainer(2285,1920);
	//fi_SaveContainer(2285,1919);
	//fi_SaveContainer(2283,1921);
	//fi_SaveContainer(2283,1922);
	//fi_SaveContainer(2283,1923);

	/* Quit Server Watcher */

	AECreateDesc(typeProcessSerialNumber, &watcherPid, sizeof(watcherPid), &theTarget);

	AECreateAppleEvent(kCoreEventClass, kAEQuitApplication, &theTarget, kAutoGenerateReturnID, kAnyTransactionID, &someAE);

	AESend(&someAE, NULL, kAENoReply | kAECanInteract, kAENormalPriority, kAEDefaultTimeout, NULL, NULL);

	/***********************/

#endif

	EndEvent();

	tb_CloseToolbox();

#ifndef _SERVER
	//}FullScreen(restoreState,nil);
	ExitMovies();

#endif

}

