#ifndef _UPDATE
#define _UPDATE

#include "Globals.h"
#include "Toolbox.h"
#include "Keys.h"
#include "Remap.h"
#include "Players.h"
#include "Creatures.h"
#include "Common.h"

#define	kUrlFlags						kURLExpandFileFlag | kURLReplaceExistingFlag
#define kTransferFailedError 			"\pThe transfer has failed : "
#define kTransferSuccessfulError 		"\pThe transfer was successful."
#define kURLAccessConfusedError 		"\pURL Access is confused."
#define	kDlTimeout						1800					
#define	kSizeTimeout					1800

#define	kMaxfiles						50
#define kDownloading					1
#define kDownloaded						2
#define kUnstuffing						3
#define kUnstuffed						4
#define kCompleted						5
#define kDownloadStalled				6
#define kStuffitStalled					7
#define	kAborted						8
#define kInstalling						9

void 			up_Update(UInt8, char*);
void			up_ContinueUpdate(int);
OSStatus		up_ShowProgressWindow(int);
void 			up_ShowResultsAndDisposeURLRef(URLReference *);
OSStatus 		MyNotifier(void*, URLEvent, URLCallbackInfo *);
OSStatus 		MyIdleTimer(EventLoopTimerRef, void *);
void 			up_ProcessData(URLReference *);
void 			up_ParseBuffer(char * bufferIn);
void			up_FinishUpdate();
OSErr 			up_FinderLaunch(long nTargets, FSSpec *targetList);
void			up_ClearUpdateFolder();
void			up_UpdateList(int);
void			up_DecompressFile(int);
void			up_InstallFile(int);
void			up_Idle();
void  			up_doLDEFDraw(Boolean selected,Rect *cellRect,Cell theCell,SInt16 dataLen, ListHandle theList);
void  			up_listDefFunction(SInt16 message,Boolean selected,Rect *cellRect,Cell theCell, SInt16 dataOffset,SInt16 dataLen,ListHandle theList);
void 			up_DialogEvent(EventRecord*);
pascal Boolean 	MyIdleCallback(EventRecord *, SInt32 *, RgnHandle *);

#endif