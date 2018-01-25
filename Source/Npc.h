#ifndef _NPC
#define _NPC

#include "Keys.h"
#include "Remap.h"
#include "Inventory.h"

	//-----------------------------------------------
/*	
	#define							kBaxter									1
	#define							kGunthor								2
	#define							kMonty									3
	#define							kGandar									4
	#define							kOliver									5
	#define							kSumter									6
	#define							kRiley									7
	#define							kReed										8
	#define							kCaldwell								9

	#define							kSimon									10
	#define							kMaxwell								11
	#define							kCharles								12

	#define							kAchard								  13
	#define							kGais								    14
	#define							kNasir								  15
	#define							kIther								  16
	#define							kGildon								  17
	#define							kVezian								  18
	#define							kGrifon								  19
	#define							kDarius								  20
	#define							kFreskin								21
	#define							kColbert								22
	#define							kBrenius								23
	#define							kRivalen								24
	#define							kFlambard								25
	#define							kPapin								  26
	#define							kTristan								27
	#define							kHesdin								  28 // use this one next
	#define							kCath								    29
	#define							kBernlak								30
	#define							kBochard								31
	#define							kEmont								  32
	#define							kBardin								  33
	#define							kSigan								  34
	#define							kLeavold								35
	#define							kLeal								    36
	#define							kSivis								  37
	#define							kParcin								  38
	#define							kAldemund								39
	#define							kAntor								  40

	#define							kGuard1								  41
	#define							kGuard2								  42
	#define							kGuard3								  43
	#define							kGuard4								  44
	#define							kGuard5								  45
	#define							kGuard6								  46
	#define							kGuard7								  47
	#define							kGuard8								  48
	#define							kGuard9								  49
	#define							kGuard10								50
	#define							kGuard11								51
	#define							kGuard12								52
	#define							kBrunius								53
	#define							kGuard14								54
	#define							kGuard15								55
	#define							kGuard16								56
	#define							kGuard17								57
	#define							kGuard18								58
	#define							kGuard19								59
	#define							kGuard20								60
	#define							kGuard21								61
	#define							kGuard22								62
	#define							kGuard23								63
	#define							kGuard24								64
	#define							kGuard25								65

	#define							kKrant								  66
	#define							kMibble								  67
	#define							kRint								  	68
	#define							kMottin								  69
	#define							kQuinn								  70
	#define							kWilt								  	71
	#define							kBrock								  72
	#define							kBrant								  73
	#define							kKrindle								74
	#define							kPrabble								75
	#define							kPrekkin								76
	#define							kRiggit								  77
	#define							kProtter								78
	#define							kWoffen								  79
	#define							kOster								  80
	#define							kAtter								  81
	#define							kFrebble								82
	#define							kVacken								  83
	#define							kHathen								  84
	#define							kRimel								  85
	#define							kCorst								  86
	#define							kShirel								  87
	#define							kUrthel								  88
	#define							kBock								    89

	#define							kGuard26								90
	#define							kGuard27								91
*/
	#define							kNPCRegular								1
	#define							kNPCGuard								2
	#define							kNPCBrigand								3
	#define							kNPCMercenary							4
	#define							kNPCDwarf1								5
	//Dave
	#define							kNPCPrivateer							6
	
	#define							kMaxNPCTypes							7
	
	#define							kMaxNPCs								1000//Dave 03-26-2012  750 
	
	#define							kNPCNameLength							32
	#define							kNPCQuestionLength						48
	#define							kMaxNPCQuestions						2500
	#define							kMaxNPCTrades							1000//Dave 03-26-2012 500
	
	//-----------------------------------------------
	
	typedef struct npcType {
		UInt16								startRow;
		UInt16 								startCol;
		UInt8									torso;
		UInt8									legs;
		UInt8									range;
		char									name[kNPCNameLength];
		char									talk[kStringLength];
		unsigned long					talkTime;
		UInt8							type;
		UInt8							NPCHairColor; 
		UInt8							NPCHairStyle;
		UInt8							NPCSex;
		TBoolean						spawned;
		TBoolean						forceMove;
		UInt8							magicResist;//Dave add 11-30-2010
	} npcType;	

	typedef struct npcQuestionType {
		int										id;
		char									question[kNPCQuestionLength];
		char									answer[kStringLength];
	} npcQuestionType;	

	typedef struct npcTradeType {
		int										id;
		int										giveItem;
		int										giveCount;
		int										takeItem;
		int										takeCount;
		int										experience;
		char									response[kStringLength];
	} npcTradeType;	

	//-----------------------------------------------
extern npcType														npc								[kMaxNPCs];
extern npcQuestionType										npcQuestion				[kMaxNPCQuestions];
extern npcTradeType												npcTrade					[kMaxNPCTrades];

//--------------------------------------------------------------

int 			np_SpawnNPC			(int, UInt16, UInt16, UInt8, UInt8, UInt8 , UInt8, UInt16, UInt16, UInt16, UInt16, TBoolean);
void 			np_InitNPCs			(void);
void 			np_Converse			(int i);
void 			np_GetReponse		(int id,int i,int theCreature);
void      		np_GetNPCName 	(int id, char name[kStringLength]);
void 			np_AddQuestion	(int id, char question[kStringLength], char answer[kStringLength]);
void 			np_ClearAllNPCs	(void);
void 			np_AddTrade			(int id, int giveCount, int giveItem, int takeCount, int takeItem, int experience, char response[kStringLength]);
int 		np_QuestComplete(int thePlayer, int theNPC, int theItem, int theCount);
TBoolean 		np_IsRegularNPC(int i);
void			np_DumpNPCInfo(int, int);
void			np_CheckNPCs	(void);
void			np_DressupNpc(int, int);

//--------------------------------------------------------------

#endif