#include "Inventory.h"
#include "Toolbox.h"
#include "Common.h"
#include "Creatures.h"
#include "Players.h"
#include "Graphics.h"
#include "Items.h"
#include "Shops.h"
#include "FileIO.h"
#include "Quest.h"
#include "Network.h"
#include "Npc.h"

char                          questDescription[1024];
int                           questGold;
int                           questExperience;
int                           currentQuest;
int                           questItem;
int                           questCount;
questType											quest[kMaxQuests];
int                           lastQuest;

extern  int                   tbQuestWindow;
extern  TBoolean              windowVisible[kMaxWindows];
extern TBoolean					      canWalkOn					[kMaxIcons];
extern UInt16							    map[kMapWidth][kMapHeight];
extern TBoolean					      doorway						[kMaxIcons];
extern TBoolean					      sign							[kMaxIcons];

#ifdef _SERVER
// ----------------------------------------------------------------- //
int qu_GetQuestForSpawn(void)
// ----------------------------------------------------------------- //

{

	return (tb_Rnd(4000,lastQuest));

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void qu_SpawnQuest(void)
// ----------------------------------------------------------------- //

{
	int         offRow;
	int         offCol;
	int         r;
	int         row;
	int         col;
	int         timeToExitFunction=0;

	while (timeToExitFunction<1000)
	{
		timeToExitFunction++;
		r=tb_Rnd(1,10);

		if (r==1)               { row=2203; col=1852; }
		else if (r==2)          { row=2191; col=1819; }
		else if (r==3)          { row=2181; col=1818; }
		else if (r==4)          { row=2166; col=1818; }
		else if (r==5)          { row=2154; col=1818; }
		else if (r==6)          { row=2145; col=1822; }
		else if (r==7)          { row=2204; col=1829; }
		else if (r==8)          { row=2202; col=1841; }
		else if (r==9)          { row=2202; col=1841; }
		else                    { row=2153; col=1868; }

		offRow=tb_Rnd(0,20)-10;

		offCol=tb_Rnd(0,20)-10;

		row=row+offRow;
		col=col+offCol;

		if ((canWalkOn[map[row][col]]) && (doorway[map[row][col]]==false) && (containerMap[row][col]==0))
			if (in_DropItem(0,1,qu_GetQuestForSpawn(), 1, row, col, 15, false, "")>0)
				timeToExitFunction=1000;
	}

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void qu_InitializeQuests(void)
// ----------------------------------------------------------------- //

{
	int				i;

	for (i=0;i<=kMaxQuests;i++)
	{
		quest[i].active					=	false;
		quest[i].gold						=	0;
		quest[i].itemID					=	0;
		quest[i].itemCount			=	0;
		quest[i].experience			=	0;
		quest[i].npcGoal				= 0;
		quest[i].itemGoal				= 0;
		quest[i].itemCountGoal	= 0;
		quest[i].description	  = 0;
		quest[i].id	            = 0;
	}
	
	for (i=0; i<7; i++)
		quest[i].active=true;

	/*
	  //---------------------------------------------------------
		//----- rat hunt
		quest[0].active						=	true;
		quest[0].gold							=	10;
		quest[0].experience				=	50;
		quest[0].npcGoal					= kOliver;
		quest[0].itemGoal					= kRatTail;
		quest[0].itemCountGoal		= 15;
		quest[0].itemID						=	kKatana;
		quest[0].itemCount				=	127;
		quest[0].description		  =	1;
		quest[0].id		            =	kRatHuntQuest;

		//----- rat hunt
		quest[1].active						=	true;
		quest[1].gold							=	50;
		quest[1].experience				=	50;
		quest[1].npcGoal					= kOliver;
		quest[1].itemGoal					= kRatTail;
		quest[1].itemCountGoal		= 15;
		quest[1].description		  =	1;
		quest[1].id		            =	kRatHuntQuest;

		//----- rat hunt
		quest[2].active						=	true;
		quest[2].gold							=	10;
		quest[2].experience				=	50;
		quest[2].npcGoal					= kOliver;
		quest[2].itemGoal					= kRatTail;
		quest[2].itemCountGoal		= 15;
		quest[2].itemID						=	kWoodenShield;
		quest[2].itemCount				=	127;
		quest[2].description		  =	1;
		quest[2].id		            =	kRatHuntQuest;
	  //---------------------------------------------------------

		//----- lobster hunt
		quest[3].active						=	true;
		quest[3].gold							=	200;
		quest[3].experience				=	100;
		quest[3].npcGoal					= kBaxter;
		quest[3].itemGoal					= kLobster;
		quest[3].itemCountGoal		= 10;
		quest[3].description		  =	2;
		quest[3].id		            =	kLobsterHuntQuest;

		//----- lobster hunt
		quest[4].active						=	true;
		quest[4].gold							=	10;
		quest[4].experience				=	100;
		quest[4].npcGoal					= kBaxter;
		quest[4].itemGoal					= kLobster;
		quest[4].itemCountGoal		= 10;
		quest[4].itemID						=	kMedalShield;
		quest[4].itemCount				=	127;
		quest[4].description		  =	2;
		quest[4].id		            =	kLobsterHuntQuest;

	  //---------------------------------------------------------

		//----- scorpion stinger hunt
		quest[5].active						=	true;
		quest[5].gold							= 200;
		quest[5].experience				=	50;
		quest[5].npcGoal					= kDarius;
		quest[5].itemGoal					= kScorpionStinger;
		quest[5].itemCountGoal		= 15;
		quest[5].description		  = 3;
		quest[5].id		            =	kScorpionStingerQuest;

		//----- scorpion stinger hunt
		quest[6].active						=	true;
		quest[6].gold							=	10;
		quest[6].experience				=	50;
		quest[6].npcGoal					= kDarius;
		quest[6].itemGoal					= kScorpionStinger;
		quest[6].itemCountGoal		= 15;
		quest[6].itemID						=	kDragonTooth;
		quest[6].itemCount				=	5;
		quest[6].description		  = 3;
		quest[6].id		            =	kScorpionStingerQuest;

	  //---------------------------------------------------------

		//----- guard steak run
		quest[7].active						=	true;
		quest[7].gold							=	20;
		quest[7].experience				=	50;
		quest[7].npcGoal					= kBrunius;
		quest[7].itemGoal					= kCookedSteak;
		quest[7].itemCountGoal		= 15;
		quest[7].description		  = 4;
		quest[7].id		            =	kGuardSteakRunQuest;

		//----- guard steak run
		quest[8].active						=	true;
		quest[8].gold							=	5;
		quest[8].experience				=	50;
		quest[8].npcGoal					= kBrunius;
		quest[8].itemGoal					= kCookedSteak;
		quest[8].itemCountGoal		= 15;
		quest[8].itemID						=	kSilverRing;
		quest[8].itemCount				=	1;
		quest[8].description		  = 4;
		quest[8].id		            =	kGuardSteakRunQuest;

	  //---------------------------------------------------------

		//----- leather hide
		quest[9].active						=	true;
		quest[9].gold							=	100;
		quest[9].experience				=	75;
		quest[9].npcGoal					= kSimon;
		quest[9].itemGoal					= kLeatherHide;
		quest[9].itemCountGoal		= 5;
		quest[9].description		  = 5;
		quest[9].id		            =	kLeatherHideQuest;

		//----- leather hide
		quest[10].active					=	true;
		quest[10].gold						=	5;
		quest[10].experience			=	75;
		quest[10].npcGoal					= kSimon;
		quest[10].itemGoal				= kLeatherHide;
		quest[10].itemCountGoal		= 5;
		quest[10].itemID					=	kRedRobe;
		quest[10].itemCount				=	1;
		quest[10].description		  = 5;
		quest[10].id		          =	kLeatherHideQuest;

	  //---------------------------------------------------------

		//----- queens ruby ... special item in nw_SendContainerContents
		quest[11].active					=	true;
		quest[11].gold						=	500;
		quest[11].experience			=	500;
		quest[11].npcGoal					= kColbert;
		quest[11].itemGoal				= kQueensRuby;
		quest[11].itemCountGoal		= 1;
		quest[11].description		  = 6;
		quest[11].id		          =	kQueensRubyQuest;

		//----- queens ruby ... special item in nw_SendContainerContents
		quest[12].active					=	true;
		quest[12].gold						=	100;
		quest[12].experience			=	500;
		quest[12].npcGoal					= kColbert;
		quest[12].itemGoal				= kQueensRuby;
		quest[12].itemCountGoal		= 1;
		quest[12].itemID					=	kPlateGauntlets;
		quest[12].itemCount				=	127;
		quest[12].description		  = 6;
		quest[12].id		          =	kQueensRubyQuest;

	  //---------------------------------------------------------

		//----- holy dagger ... special item in nw_SendContainerContents
		quest[13].active					=	true;
		quest[13].gold						=	200;
		quest[13].experience			=	200;
		quest[13].npcGoal					= kGandar;
		quest[13].itemGoal				= kHolyDagger;
		quest[13].itemCountGoal		= 1;
		quest[13].description		  = 7;
		quest[13].id		          =	kHolyDaggerQuest;

		//----- holy dagger ... special item in nw_SendContainerContents
		quest[14].active					=	true;
		quest[14].gold						=	200;
		quest[14].experience			=	200;
		quest[14].npcGoal					= kGandar;
		quest[14].itemGoal				= kHolyDagger;
		quest[14].itemCountGoal		= 1;
		quest[14].itemID					=	kOpenPlateHelmet;
		quest[14].itemCount				=	127;
		quest[14].description		  = 7;
		quest[14].id		          =	kHolyDaggerQuest;

	  //---------------------------------------------------------

		//----- Frebble Nails
		quest[15].active					=	true;
		quest[15].gold						=	45;
		quest[15].experience			=	50;
		quest[15].npcGoal					= kFrebble;
		quest[15].itemGoal				= kPackOfNails;
		quest[15].itemCountGoal		= 10;
		quest[15].description		  = 8;
		quest[15].id		          =	kFrebbleNailsQuest;

		//----- Frebble Mining Axe
		quest[16].active					=	true;
		quest[16].gold						=	100;
		quest[16].experience			=	50;
		quest[16].npcGoal					= kFrebble;
		quest[16].itemGoal				= kMiningAxe;
		quest[16].itemCountGoal		= 5;
		quest[16].description		  = 9;
		quest[16].id		          =	kFrebbleMiningAxeQuest;

		//----- Frebble Mining Axe
		quest[17].active					=	true;
		quest[17].gold						=	100;
		quest[17].experience			=	50;
		quest[17].npcGoal					= kFrebble;
		quest[17].itemGoal				= kWoodworkingTools;
		quest[17].itemCountGoal		= 5;
		quest[17].description		  = 10;
		quest[17].id		          =	kFrebbleWoodworkingToolsQuest;

		//----- Trince Mining Axe
		quest[18].active					=	true;
		quest[18].gold						=	350;
		quest[18].experience			=	250;
		quest[18].npcGoal					= kAtter;
		quest[18].itemGoal				= kMiningAxe;
		quest[18].itemCountGoal		= 10;
		quest[18].description		  = 11;
		quest[18].id		          =	kTrinceMiningAxeQuest;

		//----- Trince Coal
		quest[19].active					=	true;
		quest[19].gold						=	500;
		quest[19].experience			=	300;
		quest[19].npcGoal					= kMibble;
		quest[19].itemGoal				= kCoal;
		quest[19].itemCountGoal		= 100;
		quest[19].description		  = 12;
		quest[19].id		          =	kTrinceCoalQuest;

		//----- skeleton hunt ---------------------------------------------------
		quest[20].active						=	true;
		quest[20].gold							=	75;
		quest[20].experience				=	500;
		quest[20].npcGoal					  = kGandar;
		quest[20].itemGoal					= kSkull;
		quest[20].itemCountGoal		  = 10;
		quest[20].itemID						=	kLongSwordPower;
		quest[20].itemCount				  =	127;
		quest[20].description		    =	13;
		quest[20].id		            =	kSkeletonHuntQuest;

		//----- skeleton hunt
		quest[21].active						=	true;
		quest[21].gold							=	75;
		quest[21].experience				=	500;
		quest[21].npcGoal					  = kGandar;
		quest[21].itemGoal					= kSkull;
		quest[21].itemCountGoal		  = 10;
		quest[21].itemID						=	kSickleSpeed;
		quest[21].itemCount				  =	127;
		quest[21].description		    =	13;
		quest[21].id		            =	kSkeletonHuntQuest;

		//----- skeleton hunt
		quest[22].active						=	true;
		quest[22].gold							=	75;
		quest[22].experience				=	500;
		quest[22].npcGoal					  = kGandar;
		quest[22].itemGoal					= kSkull;
		quest[22].itemCountGoal		  = 10;
		quest[22].itemID						=	kClosedPlateHelmet;
		quest[22].itemCount				  =	127;
		quest[22].description		    =	13;
		quest[22].id		            =	kSkeletonHuntQuest;

		//----- skeleton hunt
		quest[23].active						=	true;
		quest[23].gold							=	75;
		quest[23].experience				=	500;
		quest[23].npcGoal					  = kGandar;
		quest[23].itemGoal					= kSkull;
		quest[23].itemCountGoal		  = 10;
		quest[23].itemID						=	kKatanaPoison;
		quest[23].itemCount				  =	127;
		quest[23].description		    =	13;
		quest[23].id		            =	kSkeletonHuntQuest;

		//----- skeleton hunt
		quest[24].active						=	true;
		quest[24].gold							=	150;
		quest[24].experience				=	500;
		quest[24].npcGoal					  = kGandar;
		quest[24].itemGoal					= kSkull;
		quest[24].itemCountGoal		  = 10;
		quest[24].itemID						=	kRedRobe;
		quest[24].itemCount				  =	1;
		quest[24].description		    =	13;
		quest[24].id		            =	kSkeletonHuntQuest;

		//----- dragon hunt ---------------------------------------------------
		quest[25].active						=	true;
		quest[25].gold							=	1000;
		quest[25].experience				=	1000;
		quest[25].npcGoal					  = kBrenius;
		quest[25].itemGoal					= kDragonScale;
		quest[25].itemCountGoal		  = 20;
		quest[25].itemID						=	kGlaivePower;
		quest[25].itemCount				  =	127;
		quest[25].description		    =	14;
		quest[25].id		            =	kDragonHuntQuest;

		//----- dragon hunt
		quest[26].active						=	true;
		quest[26].gold							=	1000;
		quest[26].experience				=	1000;
		quest[26].npcGoal					  = kBrenius;
		quest[26].itemGoal					= kDragonScale;
		quest[26].itemCountGoal		  = 20;
		quest[26].itemID						=	kKatanaSilver;
		quest[26].itemCount				  =	127;
		quest[26].description		    =	14;
		quest[26].id		            =	kDragonHuntQuest;

		//----- dragon hunt
		quest[27].active						=	true;
		quest[27].gold							=	1000;
		quest[27].experience				=	1000;
		quest[27].npcGoal					  = kBrenius;
		quest[27].itemGoal					= kDragonScale;
		quest[27].itemCountGoal		  = 20;
		quest[27].itemID						=	kGlaiveSilver;
		quest[27].itemCount				  =	127;
		quest[27].description		    =	14;
		quest[27].id		            =	kDragonHuntQuest;
		//----- dragon hunt

		quest[28].active						=	true;
		quest[28].gold							=	1000;
		quest[28].experience				=	1000;
		quest[28].npcGoal					  = kBrenius;
		quest[28].itemGoal					= kDragonScale;
		quest[28].itemCountGoal		  = 20;
		quest[28].itemID						=	kBlackBreastPlate;
		quest[28].itemCount				  =	127;
		quest[28].description		    =	14;
		quest[28].id		            =	kDragonHuntQuest;

		//----- dragon hunt
		quest[29].active						=	true;
		quest[29].gold							=	1000;
		quest[29].experience				=	1000;
		quest[29].npcGoal					  = kBrenius;
		quest[29].itemGoal					= kDragonScale;
		quest[29].itemCountGoal		  = 20;
		quest[29].itemID						=	kBlackLegsPlate;
		quest[29].itemCount				  =	127;
		quest[29].description		    =	14;
		quest[29].id		            =	kDragonHuntQuest;

		//----- Greater Heal Potions
		quest[30].active						=	true;
		quest[30].gold							=	250;
		quest[30].experience				=	250;
		quest[30].npcGoal					  = kGandar;
		quest[30].itemGoal					= kGreaterHealPotion;
		quest[30].itemCountGoal		  = 5;
		quest[30].description		    =	15;
		quest[30].id		            =	kGreaterHealPotionQuest;

		//----- Greater Heal Potions
		quest[31].active						=	true;
		quest[31].gold							=	15;
		quest[31].experience				=	250;
		quest[31].npcGoal					  = kGandar;
		quest[31].itemGoal					= kGreaterHealPotion;
		quest[31].itemCountGoal		  = 5;
		quest[31].itemID						=	kScimitarPower;
		quest[31].itemCount				  =	127;
		quest[31].description		    =	15;
		quest[31].id		            =	kGreaterHealPotionQuest;

		//----- Greater Detoxify Potions
		quest[32].active						=	true;
		quest[32].gold							=	250;
		quest[32].experience				=	250;
		quest[32].npcGoal					  = kGandar;
		quest[32].itemGoal					= kGreaterDetoxifyPotion;
		quest[32].itemCountGoal		  = 5;
		quest[32].description		    =	16;
		quest[32].id		            =	kGreaterDetoxifyPotionQuest;

		//----- Greater Detoxify Potions
		quest[33].active						=	true;
		quest[33].gold							=	15;
		quest[33].experience				=	250;
		quest[33].npcGoal					  = kGandar;
		quest[33].itemGoal					= kGreaterDetoxifyPotion;
		quest[33].itemCountGoal		  = 5;
		quest[33].itemID						=	kBroadSwordSpeed;
		quest[33].itemCount				  =	127;
		quest[33].description		    =	16;
		quest[33].id		            =	kGreaterDetoxifyPotionQuest;

		//----- Gaper Stalk Quest ----------------------------------------------------------
		quest[34].active						=	true;
		quest[34].gold							=	75;
		quest[34].experience				=	100;
		quest[34].npcGoal					  = kVacken;
		quest[34].itemGoal					= kGaperStalk;
		quest[34].itemCountGoal		  = 10;
		quest[34].description		    =	17;
		quest[34].id		            =	kGaperStalkQuest;

		//----- Gaper Stalk Quest
		quest[35].active						=	true;
		quest[35].gold							=	10;
		quest[35].experience				=	100;
		quest[35].npcGoal					  = kVacken;
		quest[35].itemGoal					= kGaperStalk;
		quest[35].itemCountGoal		  = 10;
		quest[35].itemID						=	kDragonTooth;
		quest[35].itemCount				  =	1;
		quest[35].description		    =	17;
		quest[35].id		            =	kGaperStalkQuest;

		//----- Gaper Stalk Quest
		quest[36].active						=	true;
		quest[36].gold							=	25;
		quest[36].experience				=	100;
		quest[36].npcGoal					  = kVacken;
		quest[36].itemGoal					= kGaperStalk;
		quest[36].itemCountGoal		  = 10;
		quest[36].itemID						=	kFullHealPotion;
		quest[36].itemCount				  =	2;
		quest[36].description		    =	17;
		quest[36].id		            =	kGaperStalkQuest;

		//----- Gaper Stalk Quest
		quest[37].active						=	true;
		quest[37].gold							=	25;
		quest[37].experience				=	100;
		quest[37].npcGoal					  = kVacken;
		quest[37].itemGoal					= kGaperStalk;
		quest[37].itemCountGoal		  = 10;
		quest[37].itemID						=	kFullDetoxifyPotion;
		quest[37].itemCount				  =	2;
		quest[37].description		    =	17;
		quest[37].id		            =	kGaperStalkQuest;

		//----- Fur Quest ----------------------------------------------------------
		quest[38].active						=	true;
		quest[38].gold							=	10;
		quest[38].experience				=	50;
		quest[38].npcGoal					  = kGandar;
		quest[38].itemGoal					= kFurSkin;
		quest[38].itemCountGoal		  = 10;
		quest[38].description		    =	18;
		quest[38].id		            =	kFurQuest;

		//----- Fur Quest
		quest[39].active						=	true;
		quest[39].gold							=	10;
		quest[39].experience				=	50;
		quest[39].npcGoal					  = kGandar;
		quest[39].itemGoal					= kFurSkin;
		quest[39].itemCountGoal		  = 10;
		quest[39].itemID						=	kPoisonGland;
		quest[39].itemCount				  =	5;
		quest[39].description		    =	18;
		quest[39].id		            =	kFurQuest;

	  //---------------------------------------------------------

		//----- Spice Run ----------------------------------------------------------
		quest[40].active						=	true;
		quest[40].gold							=	150;
		quest[40].experience				=	150;
		quest[40].npcGoal					  = kGrifon;
		quest[40].itemGoal					= kSackOfSpice;
		quest[40].itemCountGoal		  = 1;
		quest[40].description		    =	19;
		quest[40].id		            =	kSpiceRunQuest;

		//----- Spice Run ----------------------------------------------------------
		quest[41].active						=	true;
		quest[41].gold							=	75;
		quest[41].experience				=	150;
		quest[41].npcGoal					  = kGrifon;
		quest[41].itemGoal					= kSackOfSpice;
		quest[41].itemCountGoal		  = 1;
		quest[41].itemID						=	kFullHealPotion;
		quest[41].itemCount				  =	3;
		quest[41].description		    =	19;
		quest[41].id		            =	kSpiceRunQuest;

		//----- Spice Run ----------------------------------------------------------
		quest[42].active						=	true;
		quest[42].gold							=	75;
		quest[42].experience				=	150;
		quest[42].npcGoal					  = kGrifon;
		quest[42].itemGoal					= kSackOfSpice;
		quest[42].itemCountGoal		  = 1;
		quest[42].itemID						=	kFullDetoxifyPotion;
		quest[42].itemCount				  =	3;
		quest[42].description		    =	19;
		quest[42].id		            =	kSpiceRunQuest;

		//----- Spice Run ----------------------------------------------------------
		quest[43].active						=	true;
		quest[43].gold							=	75;
		quest[43].experience				=	150;
		quest[43].npcGoal					  = kGrifon;
		quest[43].itemGoal					= kSackOfSpice;
		quest[43].itemCountGoal		  = 1;
		quest[43].itemID						=	kRedDye;
		quest[43].itemCount				  =	2;
		quest[43].description		    =	19;
		quest[43].id		            =	kSpiceRunQuest;
	  //---------------------------------------------------------

		//----- Mince Spice Run ----------------------------------------------------------
		quest[44].active						=	true;
		quest[44].gold							=	200;
		quest[44].experience				=	200;
		quest[44].npcGoal					  = kHathen;
		quest[44].itemGoal					= kSackOfSpice;
		quest[44].itemCountGoal		  = 1;
		quest[44].description		    =	20;
		quest[44].id		            =	kMinceSpiceRunQuest;

		//----- Mince Spice Run ----------------------------------------------------------
		quest[45].active						=	true;
		quest[45].gold							=	100;
		quest[45].experience				=	150;
		quest[45].npcGoal					  = kHathen;
		quest[45].itemGoal					= kSackOfSpice;
		quest[45].itemCountGoal		  = 1;
		quest[45].itemID						=	kRedRobe;
		quest[45].itemCount				  =	1;
		quest[45].description		    =	20;
		quest[45].id		            =	kSpiceRunQuest;

		//----- Mince Spice Run ----------------------------------------------------------
		quest[46].active						=	true;
		quest[46].gold							=	100;
		quest[46].experience				=	150;
		quest[46].npcGoal					  = kHathen;
		quest[46].itemGoal					= kSackOfSpice;
		quest[46].itemCountGoal		  = 1;
		quest[46].itemID						=	kRedWizardHat;
		quest[46].itemCount				  =	1;
		quest[46].description		    =	20;
		quest[46].id		            =	kSpiceRunQuest;

		//----- Mince Spice Run ----------------------------------------------------------
		quest[47].active						=	true;
		quest[47].gold							=	75;
		quest[47].experience				=	150;
		quest[47].npcGoal					  = kHathen;
		quest[47].itemGoal					= kSackOfSpice;
		quest[47].itemCountGoal		  = 1;
		quest[47].itemID						=	kPoisonGland;
		quest[47].itemCount				  =	20;
		quest[47].description		    =	20;
		quest[47].id		            =	kSpiceRunQuest;

		//----- tome quest  ----------------------------------------------------------
		quest[48].active						=	true;
		quest[48].gold							=	2000;
		quest[48].experience				=	2000;
		quest[48].npcGoal					  = kGais;
		quest[48].itemGoal					= kAncientTome;
		quest[48].itemCountGoal		  = 1;
		quest[48].description		    =	21;
		quest[48].id		            =	kTomeQuest;

		//----- ice crystal quest  ----------------------------------------------------------
		quest[49].active						=	true;
		quest[49].gold							=	5000;
		quest[49].experience				=	5000;
		quest[49].npcGoal					  = kColbert;
		quest[49].itemGoal					= kIceCrystal;
		quest[49].itemCountGoal		  = 1;
		quest[49].description		    =	22;
		quest[49].id		            =	kIceCrystalQuest;

	  //---------------------------------------------------------

		for (i=0;i<kMaxQuests;i++)
		  if (quest[i].active)
		    lastQuest=4000+i;
	*/
}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void qu_SendQuestDescription(int p, int q)
// ----------------------------------------------------------------- //

{
	TBoolean			found=false;
	char      		description[512];
	int           d;

	if (q<4000)
		return;

	q=q-4000;

	strcpy(description,"");

	d=quest[q].description;

	//------------------------------------------------------------------------------

	if (d==1)  // --------------- rat hunt
		strcpy(description,"Giant rats have been spotted near the castle.  This infestation has put the castle food stores at risk.  The king as decreed that a bounty shall be paid to any holder of this scroll who aids in the extermination of these pests.  The reward will be paid upon delivering 15 rat tails to Oliver, the castle grounds keeper.");

	if (d==2)  // ---------------- lobster hunt
		strcpy(description,"The king's personal cook is preparing a lavish banquet for visiting dignitaries.  He needs 10 fresh lobsters for the banquet.  Deliver the lobsters to Baxter to collect the reward.");

	if (d==3)  // ---------------- scorpion stinger hunt
		strcpy(description,"Darious, owner of the town magic shop, is experimenting with new reagents.  He needs 15 scorpion stingers for his experiments.  The reward will be paid upon delivery.");

	if (d==4)  // ---------------- guard steak run
		strcpy(description,"The guard outpost north of town is running low on rations.  Deliver 15 cooked steaks to Brunius, the outpost leader.  The payment will be made upon delivery.");

	if (d==5)  // ---------------- leather hide
		strcpy(description,"Simon, the town tailor, has been chosen to sew a custom leather suit of armor for the king's personal guard.  He needs 5 leather hides.  The reward will be paid upon delivery.");

	if (d==6)  // ---------------- queens ruby
		strcpy(description,"A caravan transporting one of the Queen's prized rubies was highjacked by a band of Lizard Men.  It is suspected that the stolen ruby is somewhere with in the walls of the Lizard Stronghold.  Recover the jewel and return it to Colbert, the town jeweler, for the reward.");

	if (d==7)  // ---------------- holy dagger
		strcpy(description,"One of the king's high priests was attacked by a skeleton during a holy ritual in the cemetery.  The priest escaped unscathed, but his holy dagger was lost in the encounter.  The king has posted a reward for the dagger's return.  Bring it to Squire Gandar to collect the reward.");

	if (d==8)  // ---------------- freeble nails
		strcpy(description,"Freeble's provisioner shop is running low on nails.  He needs an additional 10 packs of nails.  The payment will be made upon delivery.");

	if (d==9)  // ---------------- freeble nails
		strcpy(description,"Freeble's provisioner shop is running low on mining axes.  He needs an additional 5 mining axes.  The payment will be made upon delivery.");

	if (d==10)  // ---------------- freeble nails
		strcpy(description,"Freeble's provisioner shop is running low on woodworking tools.  He needs an additional 5 sets of woodworking tools.  The payment will be made upon delivery.");

	if (d==11)  // ---------------- trince mining axe
		strcpy(description,"The Trince mining outpost has a shortage of mining axes.  They need an additional 10 mining axes delivered as soon as possible.  Deliver the shipment to Atter to collect the reward.");

	if (d==12)  // ---------------- trince coal
		strcpy(description,"The Trince mining outpost has a shortage of coal.  They need an additional 100 pieces of coal delivered as soon as possible.  Deliver the shipment to Mibble to collect the reward.");

	if (d==13)  // ---------------- skeleton hunt
		strcpy(description,"Hordes of skeletons have been spotted near the cemetery.  These unholy creatures are spreading fear and terror throughout the countryside.  The king has decreed that a bounty shall be paid upon delivery of 10 skeleton skulls to Squire Gandar.");

	if (d==14)  // ---------------- skeleton hunt
		strcpy(description,"Brenius the armorer is experimenting with new materials.  He needs 20 dragon scales to further his experiments.  The reward will be paid upon delivery of the scales.");

	if (d==15)  // ---------------- greater heal potion
		strcpy(description,"The king's guard needs 5 greater heal potions for an upcoming expedition.  Deliver the potions to Squire Gandar for payment.");

	if (d==16)  // ---------------- greature detoxify potion
		strcpy(description,"The king's guard needs 5 greater detoxify potions for an upcoming expedition.  Deliver the potions to Squire Gandar for payment.");

	if (d==17)  // ---------------- gaper stalk
		strcpy(description,"Vacken the alchemist is developing a new healing solution.  He is in dire need of 10 gaper stalks.  The reward will be paid upon delivery.");

	if (d==18)  // ---------------- Fur
		strcpy(description,"The king is sponsoring an expedition to the arctic region.  The group will need 10 furs to }ure the cold.  Deliver the furs to Gandar for payment.");

	if (d==19)  // ---------------- main town spice run
		strcpy(description,"A sack of rare spice needs to be picked up from the island town of Carth.  The spice can be purchased at the Carth provisioner shop.  Deliver the shipment to Grifon the dockworker for payment.");

	if (d==20)  // ---------------- mince spice run
		strcpy(description,"A sack of rare spice needs to be picked up from the island town of Carth and delivered to Mince.  The spice can be purchased at the Carth provisioner shop.  Deliver the shipment to Hathen the dockworker for payment.");

	if (d==21)  // ---------------- tome
		strcpy(description,"Rumors abound of an ancient tome of knowledge.  The king's scholars feel that much can be learned from this tome, if in fact, it does exist.  The king has posted a generous reward to anyone who brings the tome to Gais, the town librarian.");

	if (d==22)  // ---------------- ice crystal
		strcpy(description,"The king is seeking to expand his prized ice crystal collection.  He is offering a generous reward to anyone who can retrieve an ice crystal from the ice dungeon and return it to Colbert, the town jeweler.");

	//------------------------------------------------------------------------------

	if (strcmp(description,"")!=0)
		nw_SendQuestDescription(p, quest[q].gold, quest[q].experience, quest[q].itemID, quest[q].itemCount, description);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
int qu_QuestComplete(int p, int theNPC, int item, int count)
// ----------------------------------------------------------------- //

{
	int         goldReward  = 0;
	int         goldAdded   = 0;
	int         itemReward  = 0;
	int         itemsAdded  = 0;
	int         itemCount   = 0;
	int         theQuest    = 0;
	int         gold1;
	int         gold2;
	int         questSlot;
	//int         experience;
	float       shareGold;
	float       shareExperience;
	float       shareItem;
	float       split;
	int         theFriend[17];
	int         fPtr;
	int         i;
	int         j;
	int         experienceReward;
	float       shareGoldRoundOff;
	float       shareExperienceRoundOff;
	float       shareItemRoundOff;

	// ------ search for quest completed ---------

	if (in_CanGroup(item))
	{
		questSlot=0;

		for (i=0;i<kMaxQuests;i++)
			if (quest[i].active)
				if (theNPC==quest[i].npcGoal)
					if (item==quest[i].itemGoal)
						if (count==quest[i].itemCountGoal)
						{
							questSlot   = in_GetSlotOf_s(player[p].creatureIndex, (i+4000));

							if (questSlot>0)
							{
								theQuest=i+4000;
								break;
							}
						}
	}
	else
	{
		questSlot=0;

		for (i=0;i<kMaxQuests;i++)
			if (quest[i].active)
				if (theNPC==quest[i].npcGoal)
					if (item==quest[i].itemGoal)
					{
						questSlot   = in_GetSlotOf_s(player[p].creatureIndex, (i+4000));

						if (questSlot>0)
						{
							theQuest=i+4000;
							break;
						}
					}
	}

	// ----------------------------------

	if (theQuest==0)
		return (theQuest);

	nw_SendDisplayMessage(p,kQuestFinished);

	goldReward        = quest[theQuest-4000].gold;

	itemReward        = quest[theQuest-4000].itemID;

	itemCount         = quest[theQuest-4000].itemCount;

	experienceReward  = quest[theQuest-4000].experience;

	//in_RemoveFromInventorySlot(p, questSlot, 1);
	in_RemoveFromInventorySlot(player[p].creatureIndex, questSlot, 1);	//018

	nw_SendToClientDropFromInventorySlot(p, kNoMessage, questSlot, 1, 0);

	// ----------------------------------

	if (theQuest==0)
		return (theQuest);

	shareGold       = goldReward;

	shareExperience = experienceReward;

	if (in_CanGroup(itemReward))
		shareItem     = itemCount;
	else
		shareItem     = 1;

	split           = 1;

	for (i=0;i<17;i++)
		theFriend[i]=0;

	theFriend[0]=p;

	fPtr=1;

	if (player[p].sharing)
	{
		for (i=1;i<kMaxPlayers;i++)
			if (player[i].online)
				if (player[i].stealth==false)
					for (j=0;j<16;j++)
						if (strcmp(player[p].friendName[j],player[i].playerNameLC)==0)
							if (strcmp(player[p].friendName[j],player[p].playerNameLC)!=0)
							{
								nw_SendDisplayMessage(i,kQuestFinished);
								theFriend[fPtr]=i;
								fPtr++;
								split++;
							}
	}

	shareGold       = tb_Trunc(shareGold/split);

	shareExperience = tb_Trunc(shareExperience/split);
	shareItem       = tb_Trunc(shareItem/split);

	shareGoldRoundOff       = goldReward-(shareGold*split);
	shareExperienceRoundOff = experienceReward-(shareExperience*split);
	shareItemRoundOff       = itemCount-(shareItem*split);

	// ------- Send reward -------

	// -------------------- divy up gold

	if (goldReward<split) // not enough gold to split
	{
		if (goldReward>0)
		{
			goldAdded=in_AddGoldtoInventory(player[p].creatureIndex, goldReward);

			if (goldAdded>0)
			{
				in_SetGoldTotal(goldAdded, &gold1, &gold2);
				nw_SendGoldAdd(p, kRewardItem, 0, gold1, gold2);
			}
		}
	}
	else
	{
		for (i=0;i<17;i++)
			if (theFriend[i]!=0)
			{
				goldReward=shareGold;

				if (i==0)
					goldReward=goldReward+shareGoldRoundOff;

				if (goldReward>0)
				{
					goldAdded=in_AddGoldtoInventory(player[theFriend[i]].creatureIndex, goldReward);

					if (goldAdded>0)
					{
						in_SetGoldTotal(goldAdded, &gold1, &gold2);
						nw_SendGoldAdd(theFriend[i], kRewardItem, 0, gold1, gold2);
					}
				}
			}
	}

	// -------------------- divy up experience points

	if (experienceReward<split) // not enough exp to split
	{
		player[theFriend[i]].title=player[theFriend[i]].title+experienceReward;
	}
	else
	{
		for (i=0;i<17;i++)
			if (theFriend[i]!=0)
			{
				player[theFriend[i]].title=player[theFriend[i]].title+shareExperience;

				if (i==0)
					player[theFriend[i]].title=player[theFriend[i]].title+shareExperienceRoundOff;
			}
	}

	// --------------------  divy up items

	if ((in_CanGroup(itemReward)==false) || (itemCount<split))  // single non groupable item or less than split
	{
		itemsAdded=in_AddtoInventory(player[p].creatureIndex, itemReward, itemCount);

		if (itemsAdded>0)
			nw_SendInventoryAdd(p, itemReward, itemsAdded, kRewardItem,0);
	}
	else
	{
		for (i=0;i<17;i++)
			if (theFriend[i]!=0)
			{
				if (i==0)
					itemsAdded=in_AddtoInventory(player[theFriend[i]].creatureIndex, itemReward, (shareItem+shareItemRoundOff));
				else
					itemsAdded=in_AddtoInventory(player[theFriend[i]].creatureIndex, itemReward, shareItem);

				if (itemsAdded>0)
					nw_SendInventoryAdd(theFriend[i], itemReward, itemsAdded, kRewardItem,0);
			}
	}

	// --------------------------

	//qu_SpawnQuest();

	return (theQuest);

}

#endif

// ----------------------------------------------------------------- //
void qu_GetQuestDescription(int q)	// client
// ----------------------------------------------------------------- //

{
	
	
	currentQuest=q;
	strcpy(questDescription,"");

	//if (!fi_ReadQuestFile(currentQuest, &questGold, &questExperience, &questItem, &questCount, questDescription))
	{
		strcpy(questDescription,"");
		questGold=0;
		questExperience=0;
		questItem=0;
		questCount=0;
		//nw_SendRequestQuestDescription(currentQuest);
	}

	gr_OpenFloatingWindow(tbQuestWindow);

	windowVisible[kQuestWindow]=true;
	UpdateEvent(tbQuestWindow);

}
