#include "Toolbox.h"
#include "Globals.h"
#include "Common.h"
#include "Creatures.h"
#include "Items.h"
#include "Npc.h"
#include "Players.h"
#include "FileIO.h"
#include "FX.h"
#include "Graphics.h"
#include "Skills.h"
#include "Resource.h"
#include "GameText.h"
#include "Async Sound.h"
#include <stdio.h>
//#include <math.h>

#define                   	kMaxAttackPath           100

#ifdef _SERVER
UInt16					creatureMap				[kMapWidth][kMapHeight];
creatureType			creature				[kMaxCreatures];
UInt16					creatureSpawnPtr;
UInt8					bloodStainMap			[kMapWidth][kMapHeight];
bloodStainType			bloodStain				[kMaxBloodStains];
#endif

//UInt8					chatLookUpTable   		[kMaxCreatures];
char					chatString        		[kMaxChatSlots][kStringLength];	// saved
UInt16					chatCreatureID    		[kMaxChatSlots];	// saved

creatureControls		creatureInfo			[kMaxCreatureTypes];
latterType				ladderJump				[kMaxLadders];
long					nextSpawnTime;
long					currentCreatures;
int                 	lastSpawnID;
int                 	lastDungeonSpawnID;
int                 	lastIceDungeonSpawnID;
int                 	lastFireDungeonSpawnID;
int                 	lastRegion=-1;
int                 	pathRow;
int                 	pathCol;
spawnRegionType			spawnRegion				[kMaxSpawnRegions];
// int                 	pathPtr;

extern UInt16			map						[kMapWidth][kMapHeight];
extern TBoolean			elevated				[kMaxIcons];							// specifies whether object is elevated above ground. ex: tree
extern TBoolean			obstacle				[kMaxIcons];							// specifies whether object is elevated above ground. ex: tree
extern TBoolean			canWalkOn				[kMaxIcons];
extern TBoolean			canFlyOver				[kMaxIcons];
extern TBoolean			canSwimIn				[kMaxIcons];
extern TBoolean			doorway					[kMaxIcons];
extern TBoolean			sign					[kMaxIcons];
extern npcType			npc						[kMaxNPCs];
extern int          	ginsengSpawn;
extern int          	lotusFlowerSpawn;
extern int          	mandrakeRootSpawn;
extern int          	garlicSpawn;
extern int          	volcanicAshSpawn;
extern int				mushroomSpawn; //Dave
//extern int				eggSpawn; //Dave
extern int          	currentNumberOfContainers;
extern TBoolean     	gmProp[kMaxContainerTypes];
extern unsigned long	soundMessageID;
extern UInt32       	chatMessageID;

extern UInt8			currentCycle;
extern TBoolean						fullMoon;

extern char				containerGift           [kMaxContainers][48]; //[15];
extern char				containerName			[kMaxContainers][48];

//-----------------------------------------------
// CLIENT FUNCTIONS
//-----------------------------------------------

// ----------------------------------------------------------------- //
void cr_AddSound(int c, int theSound) //[32])
// ----------------------------------------------------------------- //

{

	if ((c<0) || (c>=kMaxCreatures))
		return;

	if (creature[c].id==0)
		return;

	creature[c].soundFX			= theSound;

	creature[c].soundFXTime = TickCount();

	creature[c].soundID			= soundMessageID;

	soundMessageID++;

}

// ----------------------------------------------------------------- //
void cr_AddToCreatureControlInventory(int creature, int item, int min, int max)
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	int								i;

	if ((creature<0) || (creature>=kMaxCreatures))// beta debug
		fi_WriteToErrorLog("Out of Range - cr_AddToCreatureControlInventory");

	for (i=1;i<kMaxInventorySize;i++)
		if (creatureInfo[creature].inventoryItem[i]==0)
		{
			creatureInfo[creature].inventoryItem[i]					= item;
			creatureInfo[creature].inventoryMinCount[i]			= min;
			creatureInfo[creature].inventoryMaxCount[i]			= max;
			return;
		}

#endif
}

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean cr_NearPlayer(int c)
// ----------------------------------------------------------------- //

{
	int			i;
	int			j;

	for (i=creature[c].row-15;i<creature[c].row+15;i++)
		for (j=creature[c].col-15;j<creature[c].col+15;j++)
			if (creatureMap[i][j]!=0)
				if (creature[creatureMap[i][j]].playerIndex!=0)
					return (true);

	return (false);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void cr_CheckCreaturePopulation(void)
// ----------------------------------------------------------------- //
{
	//int							i;
	int							j;
	//int             k;
	int							row;
	int							col;
	int             fails=0;
	//int             pop;
	//int             rare;
	//int             x;
	//int             y;
	int             c;
	//TBoolean        usingSpawnSpot;
	//int             range;
	//int             region;
	//int							rr;
	//int							cc;
	TBoolean				doSpawn;
	int				cpt, cpt2;
	int				maxpop;
	
	

	if (TickCount()<nextSpawnTime)
		return;

	/* #ifdef _FUNCTIONTRACEDEBUGGING
	   fi_WriteToErrorLog("cr_CheckCreaturePopulation Entry");
	 #endif*/
	cpt2=2;

	if (currentCycle==kBleedingMoon && fullMoon)
		cpt2++;

	if (dayNight==2)
		cpt2++;

	for (j=1;j<kMaxSpawnRegions;j++)
		for (cpt=0; cpt<cpt2; cpt++)
		{
			maxpop=spawnRegion[j].maxPopulation;

			if (currentCycle==kBleedingMoon && fullMoon)
				maxpop+=3;

			if (spawnRegion[j].creatureID>0)
				if (spawnRegion[j].currentPopulation<maxpop)
				{
					row=tb_Rnd(spawnRegion[j].theRegion.left,spawnRegion[j].theRegion.right);
					col=tb_Rnd(spawnRegion[j].theRegion.top,spawnRegion[j].theRegion.bottom);
					doSpawn=false;

					if ((creatureInfo[spawnRegion[j].creatureID].domain==kLand) || (creatureInfo[spawnRegion[j].creatureID].domain==kAir))
						if (canWalkOn[map[row][col]])
							doSpawn=true;

					if (creatureInfo[spawnRegion[j].creatureID].domain==kWater)
						if (canSwimIn[map[row][col]])
							doSpawn=true;

					if (doSpawn)
					{
						c=cr_SpawnCreature(spawnRegion[j].creatureID, row, col);

						if (c>0)
						{
							creature[c].startRow=row;
							creature[c].startCol=col;
							creature[c].range=5;// gs bug range;
							creature[c].home=(UInt16)j;
							spawnRegion[j].currentPopulation++;
						}
					}
				}
		}

	nextSpawnTime=TickCount()+(3600*kSpawnInterval);

	if (currentCycle==kBleedingMoon && fullMoon)  //Bleeding moon - faster spawns
		nextSpawnTime-=3600;

	//04/07/07 random pop time
	nextSpawnTime+=-1800+tb_Rnd(0,3600);

//	#ifdef _FUNCTIONTRACEDEBUGGING
//	fi_WriteToErrorLog("cr_CheckCreaturePopulation Exit");
//  #endif

}

#endif
// ----------------------------------------------------------------- //
void cr_CreatureControls(void)
// ----------------------------------------------------------------- //

{
	int							i;
	int							j;
	int             totalPopulation=0;
		
	//int							d;
	// add to carbon 11/24 - modified crystal yields

	for (i=0;i<kMaxCreatureTypes;i++)
	{
		creatureInfo[i].speed										= 0;
		creatureInfo[i].domain										= 0;
		creatureInfo[i].terrain										= 0;
		creatureInfo[i].mobile										= true;
		creatureInfo[i].poisonous									= 0;
		creatureInfo[i].curse								  		= 0;
		creatureInfo[i].alignment									= kNeutral;
		creatureInfo[i].maxHitPoints								= 0;
		creatureInfo[i].maxStrength									= 0;
		creatureInfo[i].maxIntelligence								= 0;
		creatureInfo[i].maxDexterity								= 0;
		creatureInfo[i].maxMagicPoints								= 0;
		creatureInfo[i].maxStaminaPoints								= 0;
		creatureInfo[i].directionalGraphics							= false;
		creatureInfo[i].pauseGraphics								= false;
		creatureInfo[i].graphicsIndex								= 0;
		creatureInfo[i].minGold										= 0;
		creatureInfo[i].maxGold										= 0;
		creatureInfo[i].corpseID									= 1;
		creatureInfo[i].seekGold									= false;
		creatureInfo[i].seekMagic									= false;
		creatureInfo[i].bleeds										= true;
		creatureInfo[i].seekWeapons									= false;
		creatureInfo[i].magicResist									= 1.0;
		creatureInfo[i].tamingLevel									= 101;
		creatureInfo[i].population									= 0;
		creatureInfo[i].desiredPopulation							= 0;
		creatureInfo[i].randomSound									= 0;
		creatureInfo[i].attackSound									= 0;
		creatureInfo[i].attackRange									= 1;
		creatureInfo[i].attackDamageMin								= 0;
		creatureInfo[i].attackDamageMax								= 0;
		creatureInfo[i].armorRating									= 0;
		creatureInfo[i].immuneToPoison                				= false;
		creatureInfo[i].meatEater									= false;
		creatureInfo[i].vegEater									= false;
		creatureInfo[i].undead										= false;
		creatureInfo[i].metal										= false;
		creatureInfo[i].blunt										= 0;
		creatureInfo[i].points										= 0;
		creatureInfo[i].rareLevel									= 0;
		creatureInfo[i].rareChance									= 0;
		creatureInfo[i].color										= kNoCreatureColor;
		creatureInfo[i].lockStrength								= 0;
		creatureInfo[i].bmp											= 0;
		creatureInfo[i].canMorph									= false;
		creatureInfo[i].frost										= false;
		creatureInfo[i].fire										= false;
		creatureInfo[i].creatureLevel								= 0;
		creatureInfo[i].size										= 0;
		creatureInfo[i].trackingLevel								= 0;
		creatureInfo[i].mageryLevel									= 0;
		creatureInfo[i].spellcastChance								= 0;
		creatureInfo[i].selfhealChance								= 0;
		creatureInfo[i].selfhealTrigger								= 0;
		
		for (j=0; j<kMaxCreatureSpells; j++)
		{
			creatureInfo[i].spellList[j][0]						= 0;
			creatureInfo[i].spellList[j][1]						= 0;
			creatureInfo[i].spellList[j][2]						= 0;
		}

		for (j=0;j<kMaxTalkScripts;j++)
		{
			creatureInfo[i].talkScript[j].state					= 0;
			creatureInfo[i].talkScript[j].language				= 0;
			creatureInfo[i].talkScript[j].target				= 0;
			strcpy(creatureInfo[i].talkScript[j].say,"");
		}

		for (j=1;j<kMaxInventorySize;j++)
		{
			creatureInfo[i].inventoryItem[j]					= 0;
			creatureInfo[i].inventoryMinCount[j]				= 0;
			creatureInfo[i].inventoryMaxCount[j]				= 0;
		}

		for (j=0;j<kMaxWeaponModifiers;j++)
		{
			creatureInfo[i].weaponID[j]							= 0;
			creatureInfo[i].weaponModifier[j]					= 1.0;
		}
	}


	creatureInfo[kHuman].domain											= kLand;

	creatureInfo[kHuman].terrain										= kGrass;
	creatureInfo[kHuman].alignment										= kGood;
	creatureInfo[kHuman].maxHitPoints									= 100;
	creatureInfo[kHuman].maxStrength									= 100;
	creatureInfo[kHuman].maxIntelligence								= 100;
	creatureInfo[kHuman].maxDexterity									= 100;
	creatureInfo[kHuman].maxMagicPoints									= 100;
	creatureInfo[kHuman].maxStaminaPoints								= 100;
	creatureInfo[kHuman].corpseID										= 14;
	creatureInfo[kHuman].size											= 1;
	creatureInfo[kHuman].trackingLevel									= 2;

	creatureInfo[kNPCHuman].speed										= 150;
	creatureInfo[kNPCHuman].domain										= kAir;
	creatureInfo[kNPCHuman].terrain										= kGrass;
	creatureInfo[kNPCHuman].alignment									= kGood;
	creatureInfo[kNPCHuman].maxHitPoints								= 50;
	creatureInfo[kNPCHuman].maxStrength									= 50;
	creatureInfo[kNPCHuman].maxIntelligence								= 10;
	creatureInfo[kNPCHuman].maxDexterity								= 10;
	creatureInfo[kNPCHuman].maxMagicPoints								= 0;
	creatureInfo[kNPCHuman].maxStaminaPoints								= 100;
	creatureInfo[kNPCHuman].armorRating									= 2;
	creatureInfo[kNPCHuman].attackDamageMin								= 20;
	creatureInfo[kNPCHuman].attackDamageMax								= 22;
	creatureInfo[kNPCHuman].corpseID									= 14;
	creatureInfo[kNPCHuman].minGold										= 1000000;
	creatureInfo[kNPCHuman].maxGold										= 1000000;
	creatureInfo[kNPCHuman].creatureLevel								= 10;
	creatureInfo[kNPCHuman].magicResist									= 6;
	creatureInfo[kNPCHuman].size										= 1;
	creatureInfo[kNPCHuman].trackingLevel								= 2;
	
	creatureInfo[kNPCHumanFemale].speed										= 150;
	creatureInfo[kNPCHumanFemale].domain										= kAir;
	creatureInfo[kNPCHumanFemale].terrain										= kGrass;
	creatureInfo[kNPCHumanFemale].alignment									= kGood;
	creatureInfo[kNPCHumanFemale].maxHitPoints								= 50;
	creatureInfo[kNPCHumanFemale].maxStrength									= 50;
	creatureInfo[kNPCHumanFemale].maxIntelligence								= 10;
	creatureInfo[kNPCHumanFemale].maxDexterity								= 10;
	creatureInfo[kNPCHumanFemale].maxMagicPoints								= 0;
	creatureInfo[kNPCHumanFemale].maxStaminaPoints								= 100;
	creatureInfo[kNPCHumanFemale].armorRating									= 2;
	creatureInfo[kNPCHumanFemale].attackDamageMin								= 20;
	creatureInfo[kNPCHumanFemale].attackDamageMax								= 22;
	creatureInfo[kNPCHumanFemale].corpseID									= 124;
	creatureInfo[kNPCHumanFemale].minGold										= 1000000;
	creatureInfo[kNPCHumanFemale].maxGold										= 1000000;
	creatureInfo[kNPCHumanFemale].creatureLevel								= 10;
	creatureInfo[kNPCHumanFemale].magicResist									= 6;
	creatureInfo[kNPCHumanFemale].size										= 1;
	creatureInfo[kNPCHumanFemale].trackingLevel								= 2;
	
	

	creatureInfo[kBrigandShelter1].bmp								=	0;
	creatureInfo[kBrigandShelter1].speed							=	1000;						// -- speed is the minimum ticks between moves --
	creatureInfo[kBrigandShelter1].domain							=	kAir;
	creatureInfo[kBrigandShelter1].terrain						=	kGrass;
	creatureInfo[kBrigandShelter1].mobile							=	false;
	creatureInfo[kBrigandShelter1].bleeds							=	false;
	creatureInfo[kBrigandShelter1].alignment					= kEvil;
	creatureInfo[kBrigandShelter1].maxHitPoints				= 100;
	creatureInfo[kBrigandShelter1].maxStrength				= 100;
	creatureInfo[kBrigandShelter1].maxIntelligence		= 125;
	creatureInfo[kBrigandShelter1].maxDexterity				= 20;
	creatureInfo[kBrigandShelter1].maxMagicPoints			= 100;
	creatureInfo[kBrigandShelter1].minGold						= 0;
	creatureInfo[kBrigandShelter1].maxGold						= 12;
	creatureInfo[kBrigandShelter1].magicResist				= 0.5;
	creatureInfo[kBrigandShelter1].tamingLevel				= 101;
	creatureInfo[kBrigandShelter1].attackRange				= 0;
	creatureInfo[kBrigandShelter1].corpseID						= 45;
	creatureInfo[kBrigandShelter1].armorRating				= 25;
	creatureInfo[kBrigandShelter1].points							= 5;
	creatureInfo[kBrigandShelter1].desiredPopulation	= 20;//3000;
	creatureInfo[kBrigandShelter1].immuneToPoison     = true;
	creatureInfo[kBrigandShelter1].creatureLevel		= 1;
	creatureInfo[kBrigandShelter1].trackingLevel		= 101;
	creatureInfo[kBrigandShelter1].size					= 1;
	cr_AddToCreatureControlInventory(kBrigandShelter1, kLoafOfBread, 0, 3);
	cr_AddToCreatureControlInventory(kBrigandShelter1, kCookedSteak, 0, 3);
	cr_AddToCreatureControlInventory(kBrigandShelter1, kLobsterPie, 0, 3);
	cr_AddToCreatureControlInventory(kBrigandShelter1, kGohoranChocolate, 0, 2);

	creatureInfo[kBrigandShelter2].bmp								=	0;
	creatureInfo[kBrigandShelter2].speed							=	1000;						// -- speed is the minimum ticks between moves --
	creatureInfo[kBrigandShelter2].domain							=	kAir;
	creatureInfo[kBrigandShelter2].terrain						=	kGrass;
	creatureInfo[kBrigandShelter2].mobile							=	false;
	creatureInfo[kBrigandShelter2].bleeds							=	false;
	creatureInfo[kBrigandShelter2].alignment					= kEvil;
	creatureInfo[kBrigandShelter2].maxHitPoints				= 100;
	creatureInfo[kBrigandShelter2].maxStrength				= 100;
	creatureInfo[kBrigandShelter2].maxIntelligence		= 125;
	creatureInfo[kBrigandShelter2].maxDexterity				= 20;
	creatureInfo[kBrigandShelter2].maxMagicPoints			= 100;
	creatureInfo[kBrigandShelter2].minGold						= 0;
	creatureInfo[kBrigandShelter2].maxGold						= 12;
	creatureInfo[kBrigandShelter2].magicResist				= 0.5;
	creatureInfo[kBrigandShelter2].tamingLevel				= 101;
	creatureInfo[kBrigandShelter2].attackRange				= 0;
	creatureInfo[kBrigandShelter2].corpseID						= 45;
	creatureInfo[kBrigandShelter2].armorRating				= 25;
	creatureInfo[kBrigandShelter2].points							= 5;
	creatureInfo[kBrigandShelter2].desiredPopulation	= 20;//3000;
	creatureInfo[kBrigandShelter2].immuneToPoison     = true;
	creatureInfo[kBrigandShelter2].creatureLevel		= 1;
	creatureInfo[kBrigandShelter2].trackingLevel		= 101;
	creatureInfo[kBrigandShelter2].size					= 1;
	cr_AddToCreatureControlInventory(kBrigandShelter2, kLoafOfBread, 0, 3);
	cr_AddToCreatureControlInventory(kBrigandShelter2, kCookedSteak, 0, 3);
	cr_AddToCreatureControlInventory(kBrigandShelter2, kLobsterPie, 0, 3);
	cr_AddToCreatureControlInventory(kBrigandShelter2, kGohoranChocolate, 0, 2);

	creatureInfo[kBrigandShelter3].bmp								=	0;
	creatureInfo[kBrigandShelter3].speed							=	1000;						// -- speed is the minimum ticks between moves --
	creatureInfo[kBrigandShelter3].domain							=	kAir;
	creatureInfo[kBrigandShelter3].terrain						=	kGrass;
	creatureInfo[kBrigandShelter3].mobile							=	false;
	creatureInfo[kBrigandShelter3].bleeds							=	false;
	creatureInfo[kBrigandShelter3].alignment					= kEvil;
	creatureInfo[kBrigandShelter3].maxHitPoints				= 100;
	creatureInfo[kBrigandShelter3].maxStrength				= 100;
	creatureInfo[kBrigandShelter3].maxIntelligence		= 125;
	creatureInfo[kBrigandShelter3].maxDexterity				= 20;
	creatureInfo[kBrigandShelter3].maxMagicPoints			= 100;
	creatureInfo[kBrigandShelter3].minGold						= 0;
	creatureInfo[kBrigandShelter3].maxGold						= 12;
	creatureInfo[kBrigandShelter3].magicResist				= 0.5;
	creatureInfo[kBrigandShelter3].tamingLevel				= 101;
	creatureInfo[kBrigandShelter3].attackRange				= 0;
	creatureInfo[kBrigandShelter3].corpseID						= 45;
	creatureInfo[kBrigandShelter3].armorRating				= 25;
	creatureInfo[kBrigandShelter3].points							= 5;
	creatureInfo[kBrigandShelter3].desiredPopulation	= 20;//3000;
	creatureInfo[kBrigandShelter3].immuneToPoison     = true;
	creatureInfo[kBrigandShelter3].creatureLevel		= 1;
	creatureInfo[kBrigandShelter3].trackingLevel		= 101;
	creatureInfo[kBrigandShelter3].size					= 1;
	cr_AddToCreatureControlInventory(kBrigandShelter3, kLoafOfBread, 0, 3);
	cr_AddToCreatureControlInventory(kBrigandShelter3, kCookedSteak, 0, 3);
	cr_AddToCreatureControlInventory(kBrigandShelter3, kLobsterPie, 0, 3);
	cr_AddToCreatureControlInventory(kBrigandShelter3, kGohoranChocolate, 0, 2);

	creatureInfo[kReaper].bmp											=	0;
	creatureInfo[kReaper].domain									=	kLand;
	creatureInfo[kReaper].terrain									=	kGrass;
	creatureInfo[kReaper].mobile									=	false;
	creatureInfo[kReaper].bleeds									=	false;
	creatureInfo[kReaper].alignment								= kEvil;
	creatureInfo[kReaper].attackRange							= 5;
	creatureInfo[kReaper].corpseID								= 13;
	creatureInfo[kReaper].points							    = 5;
	creatureInfo[kReaper].desiredPopulation				= 1200;//3000;
	creatureInfo[kReaper].speed										=	fi_GetData(":Creature Settings:Bracken", "SPEED=", 320);
	creatureInfo[kReaper].maxHitPoints						= fi_GetData(":Creature Settings:Bracken", "HIT_POINTS=", 100);
	creatureInfo[kReaper].maxStrength							= fi_GetData(":Creature Settings:Bracken", "STRENGTH=", 100);
	creatureInfo[kReaper].maxIntelligence					= fi_GetData(":Creature Settings:Bracken", "INTELLIGENCE=", 125);
	creatureInfo[kReaper].maxDexterity						= fi_GetData(":Creature Settings:Bracken", "DEXTERITY=", 20);
	creatureInfo[kReaper].maxMagicPoints					= fi_GetData(":Creature Settings:Bracken", "MAGIC_POINTS=", 100);
	creatureInfo[kReaper].minGold								  = fi_GetData(":Creature Settings:Bracken", "MIN_GOLD=", 22);
	creatureInfo[kReaper].maxGold								  = fi_GetData(":Creature Settings:Bracken", "MAX_GOLD=", 50);
	creatureInfo[kReaper].armorRating							= fi_GetData(":Creature Settings:Bracken", "ARMOR_RATING=", 25);
	creatureInfo[kReaper].attackDamageMin					= fi_GetData(":Creature Settings:Bracken", "MIN_DAMAGE=", 6);
	creatureInfo[kReaper].attackDamageMax					= fi_GetData(":Creature Settings:Bracken", "MAX_DAMAGE=", 9);
	creatureInfo[kReaper].magicResist				      = (float)fi_GetData(":Creature Settings:Bracken", "MAGIC_RESIST=", 30)/100.0;
	creatureInfo[kReaper].lockStrength						= fi_GetData(":Creature Settings:Bracken", "LOCK=", 0);
	creatureInfo[kReaper].creatureLevel						= fi_GetData(":Creature Settings:Bracken", "LEVEL=", 1);
	creatureInfo[kReaper].tamingLevel							= fi_GetData(":Creature Settings:Bracken", "TAMING_LEVEL=", 101);
	creatureInfo[kReaper].immuneToPoison          = fi_GetBoolean(":Creature Settings:Bracken", "POISON_IMMUNITY=", false);
	creatureInfo[kReaper].trackingLevel							= fi_GetData(":Creature Settings:Bracken", "TRACKING_LEVEL=", 101);
	creatureInfo[kReaper].size							= fi_GetData(":Creature Settings:Bracken", "SIZE=", 2);
	creatureInfo[kReaper].mageryLevel					= fi_GetData(":Creature Settings:Bracken", "MAGERY_LEVEL=", 10);
	creatureInfo[kReaper].spellcastChance				= fi_GetData(":Creature Settings:Bracken", "SPELLCAST_CHANCE=", 100);
	creatureInfo[kReaper].selfhealChance				= fi_GetData(":Creature Settings:Bracken", "SELFHEAL_CHANCE=", 33);
	creatureInfo[kReaper].selfhealTrigger				= fi_GetData(":Creature Settings:Bracken", "SELFHEAL_TRIGGER=", 20);
	fi_GetSpellList(":Creature Settings:Bracken", kReaper);
	fi_GetCreatureTalkScript(":Creature Settings:Bracken", kReaper);
	fi_GetCreatureInventory(":Creature Settings:Bracken", kReaper);
	fi_GetWeaponModifier(":Creature Settings:Bracken", kReaper);
	/*
		cr_AddToCreatureControlInventory(kReaper, kLog, 12, 18);
		cr_AddToCreatureControlInventory(kReaper, kDragonTooth, 0, 1);
		cr_AddToCreatureControlInventory(kReaper, kGinseng, 0, 5);
		cr_AddToCreatureControlInventory(kReaper, kLotusFlower, 0, 5);
		cr_AddToCreatureControlInventory(kReaper, kMandrakeRoot, 0, 5);
		cr_AddToCreatureControlInventory(kReaper, kAmber, 0, 20);
		cr_AddToCreatureControlInventory(kReaper, kGarlic, 0, 5);
		cr_AddToCreatureControlInventory(kReaper, kVolcanicAsh, 0, 5);
		cr_AddToCreatureControlInventory(kReaper, kPoisonGland, 0, 5);
		cr_AddToCreatureControlInventory(kReaper, kCoal, 0, 5);
		cr_AddToCreatureControlInventory(kReaper, kElectricEel, 0, 5);
	*/
	creatureInfo[kGazer].bmp											=	0;
	creatureInfo[kGazer].domain										=	kAir;
	creatureInfo[kGazer].alignment								= kEvil;
	creatureInfo[kGazer].directionalGraphics			= true;
	creatureInfo[kGazer].graphicsIndex					  = 1;
	creatureInfo[kGazer].tamingLevel							= 7;
	creatureInfo[kGazer].attackRange							= 4;
	creatureInfo[kGazer].corpseID								  = 22;
	creatureInfo[kGazer].desiredPopulation				= 1200;//1500;
	creatureInfo[kGazer].points				            = 4;
	creatureInfo[kGazer].rareLevel								= 10;
	creatureInfo[kGazer].rareChance								= 10;
	creatureInfo[kGazer].meatEater								= true;
	creatureInfo[kGazer].canMorph								  = true;
	creatureInfo[kGazer].speed										=	fi_GetData(":Creature Settings:Gaper", "SPEED=", 65);
	creatureInfo[kGazer].maxHitPoints							= fi_GetData(":Creature Settings:Gaper", "HIT_POINTS=", 50);
	creatureInfo[kGazer].maxStrength							= fi_GetData(":Creature Settings:Gaper", "STRENGTH=", 50);
	creatureInfo[kGazer].maxIntelligence					= fi_GetData(":Creature Settings:Gaper", "INTELLIGENCE=", 125);
	creatureInfo[kGazer].maxDexterity							= fi_GetData(":Creature Settings:Gaper", "DEXTERITY=", 50);
	creatureInfo[kGazer].maxMagicPoints						= fi_GetData(":Creature Settings:Gaper", "MAGIC_POINTS=", 80);
	creatureInfo[kGazer].minGold								  = fi_GetData(":Creature Settings:Gaper", "MIN_GOLD=", 22);
	creatureInfo[kGazer].maxGold								  = fi_GetData(":Creature Settings:Gaper", "MAX_GOLD=", 50);
	creatureInfo[kGazer].armorRating							= fi_GetData(":Creature Settings:Gaper", "ARMOR_RATING=", 10);
	creatureInfo[kGazer].magicResist				      = (float)fi_GetData(":Creature Settings:Gaper", "MAGIC_RESIST=", 30)/100.0;
	creatureInfo[kGazer].lockStrength							= fi_GetData(":Creature Settings:Gaper", "LOCK=", 0);
	creatureInfo[kGazer].creatureLevel							= fi_GetData(":Creature Settings:Gaper", "LEVEL=", 1);
	creatureInfo[kGazer].tamingLevel							= fi_GetData(":Creature Settings:Gaper", "TAMING_LEVEL=", 7);
	creatureInfo[kGazer].immuneToPoison           = fi_GetBoolean(":Creature Settings:Gaper", "POISON_IMMUNITY=", false);
	creatureInfo[kGazer].seekGold									= fi_GetBoolean(":Creature Settings:Gaper", "SEEK_GOLD=", true);
	creatureInfo[kGazer].seekWeapons							= fi_GetBoolean(":Creature Settings:Gaper", "SEEK_WEAPONS=", false);
	creatureInfo[kGazer].seekMagic								= fi_GetBoolean(":Creature Settings:Gaper", "SEEK_MAGIC=", true);
	creatureInfo[kGazer].trackingLevel							= fi_GetData(":Creature Settings:Gaper", "TRACKING_LEVEL=", 101);
	creatureInfo[kGazer].size									= fi_GetData(":Creature Settings:Gaper", "SIZE=", 1);fi_GetCreatureTalkScript(":Creature Settings:Gaper", kGazer);
	creatureInfo[kGazer].mageryLevel					= fi_GetData(":Creature Settings:Gaper", "MAGERY_LEVEL=", 10);
	creatureInfo[kGazer].spellcastChance				= fi_GetData(":Creature Settings:Gaper", "SPELLCAST_CHANCE=", 100);
	creatureInfo[kGazer].selfhealChance					= fi_GetData(":Creature Settings:Gaper", "SELFHEAL_CHANCE=", 33);
	creatureInfo[kGazer].selfhealTrigger				= fi_GetData(":Creature Settings:Gaper", "SELFHEAL_TRIGGER=", 20);
	fi_GetSpellList(":Creature Settings:Gaper", kGazer);
	fi_GetCreatureInventory(":Creature Settings:Gaper", kGazer);
	fi_GetWeaponModifier(":Creature Settings:Gaper", kGazer);

//	cr_AddToCreatureControlInventory(kGazer, kGinseng, 0, 5);
//	cr_AddToCreatureControlInventory(kGazer, kLotusFlower, 0, 5);
//	cr_AddToCreatureControlInventory(kGazer, kMandrakeRoot, 0, 5);
//	cr_AddToCreatureControlInventory(kGazer, kAmber, 0, 5);
//	cr_AddToCreatureControlInventory(kGazer, kGarlic, 0, 5);
//	cr_AddToCreatureControlInventory(kGazer, kVolcanicAsh, 0, 5);
//	cr_AddToCreatureControlInventory(kGazer, kPoisonGland, 0, 5);
//	cr_AddToCreatureControlInventory(kGazer, kCoal, 0, 5);
//	cr_AddToCreatureControlInventory(kGazer, kElectricEel, 0, 5);
//	cr_AddToCreatureControlInventory(kGazer, kRawSteak, 2, 5);
	//cr_AddToCreatureControlInventory(kGazer, kDragonTooth, 0, 1);
//	cr_AddToCreatureControlInventory(kGazer, kGaperStalk, 5, 10);

	creatureInfo[kAirElemental].bmp								=	0;
	creatureInfo[kAirElemental].domain						=	kLand;
	creatureInfo[kAirElemental].terrain						=	kAnyTerrain;
	creatureInfo[kAirElemental].alignment					= kEvil;
	creatureInfo[kAirElemental].desiredPopulation	= 1950;//1300;
	creatureInfo[kAirElemental].points		        = 3;
	creatureInfo[kAirElemental].meatEater					= true;
	creatureInfo[kAirElemental].canMorph					= true;
	creatureInfo[kAirElemental].bleeds						= false;
	creatureInfo[kAirElemental].speed										=	fi_GetData(":Creature Settings:Air Elemental", "SPEED=", 70);
	creatureInfo[kAirElemental].poisonous					      = fi_GetData(":Creature Settings:Air Elemental", "POISONOUS=", 0);
	creatureInfo[kAirElemental].curse					      		= fi_GetData(":Creature Settings:Air Elemental", "CURSE=", 0);
	creatureInfo[kAirElemental].maxHitPoints						= fi_GetData(":Creature Settings:Air Elemental", "HIT_POINTS=", 75);
	creatureInfo[kAirElemental].maxStrength							= fi_GetData(":Creature Settings:Air Elemental", "STRENGTH=", 75);
	creatureInfo[kAirElemental].maxIntelligence					= fi_GetData(":Creature Settings:Air Elemental", "INTELLIGENCE=", 60);
	creatureInfo[kAirElemental].maxDexterity						= fi_GetData(":Creature Settings:Air Elemental", "DEXTERITY=", 50);
	creatureInfo[kAirElemental].minGold								  = fi_GetData(":Creature Settings:Air Elemental", "MIN_GOLD=", 10);
	creatureInfo[kAirElemental].maxGold								  = fi_GetData(":Creature Settings:Air Elemental", "MAX_GOLD=", 25);
	creatureInfo[kAirElemental].seekGold								= fi_GetBoolean(":Creature Settings:Air Elemental", "SEEK_GOLD=", true);
	creatureInfo[kAirElemental].seekMagic								= fi_GetBoolean(":Creature Settings:Air Elemental", "SEEK_MAGIC=", false);
	creatureInfo[kAirElemental].seekWeapons							= fi_GetBoolean(":Creature Settings:Air Elemental", "SEEK_WEAPONS=", false);
	creatureInfo[kAirElemental].magicResist				      = (float)fi_GetData(":Creature Settings:Air Elemental", "MAGIC_RESIST=", 80)/100.0;
	creatureInfo[kAirElemental].tamingLevel							= fi_GetData(":Creature Settings:Air Elemental", "TAMING_LEVEL=", 5);
	creatureInfo[kAirElemental].creatureLevel							= fi_GetData(":Creature Settings:Air Elemental", "LEVEL=", 1);
	creatureInfo[kAirElemental].armorRating							= fi_GetData(":Creature Settings:Air Elemental", "ARMOR_RATING=", 1);
	creatureInfo[kAirElemental].immuneToPoison          = fi_GetBoolean(":Creature Settings:Air Elemental", "POISON_IMMUNITY=", false);
	creatureInfo[kAirElemental].lockStrength						= fi_GetData(":Creature Settings:Air Elemental", "LOCK=", 0);
	creatureInfo[kAirElemental].attackDamageMin					= fi_GetData(":Creature Settings:Air Elemental", "MIN_DAMAGE=", 6);
	creatureInfo[kAirElemental].attackDamageMax					= fi_GetData(":Creature Settings:Air Elemental", "MAX_DAMAGE=", 9);
	creatureInfo[kAirElemental].trackingLevel							= fi_GetData(":Creature Settings:Air Elemental", "TRACKING_LEVEL=", 101);
	creatureInfo[kAirElemental].size							= fi_GetData(":Creature Settings:Air Elemental", "SIZE=", 2);
	creatureInfo[kAirElemental].mageryLevel					= fi_GetData(":Creature Settings:Air Elemental", "MAGERY_LEVEL=", 0);
	creatureInfo[kAirElemental].spellcastChance				= fi_GetData(":Creature Settings:Air Elemental", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kAirElemental].selfhealChance				= fi_GetData(":Creature Settings:Air Elemental", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kAirElemental].selfhealTrigger				= fi_GetData(":Creature Settings:Air Elemental", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Air Elemental", kAirElemental);
	fi_GetCreatureTalkScript(":Creature Settings:Air Elemental", kAirElemental);
	fi_GetCreatureInventory(":Creature Settings:Air Elemental", kAirElemental);
	fi_GetWeaponModifier(":Creature Settings:Air Elemental", kAirElemental);

	creatureInfo[kWaterElemental].bmp								=	0;
	creatureInfo[kWaterElemental].domain						=	kWater;
	creatureInfo[kWaterElemental].terrain						=	kAnyTerrain;
	creatureInfo[kWaterElemental].alignment					= kEvil;
	creatureInfo[kWaterElemental].desiredPopulation	= 3500;//4000;
	creatureInfo[kWaterElemental].points		        = 3;
	creatureInfo[kWaterElemental].color		          = kWaterElementalColor;
	creatureInfo[kWaterElemental].meatEater					= true;
	creatureInfo[kWaterElemental].canMorph					= true;
	creatureInfo[kWaterElemental].bleeds						= false;
	creatureInfo[kWaterElemental].speed										=	fi_GetData(":Creature Settings:Water Elemental", "SPEED=", 80);
	creatureInfo[kWaterElemental].poisonous					      = fi_GetData(":Creature Settings:Water Elemental", "POISONOUS=", 0);
	creatureInfo[kWaterElemental].curse					      		= fi_GetData(":Creature Settings:Water Elemental", "CURSE=", 0);
	creatureInfo[kWaterElemental].maxHitPoints						= fi_GetData(":Creature Settings:Water Elemental", "HIT_POINTS=", 75);
	creatureInfo[kWaterElemental].maxStrength							= fi_GetData(":Creature Settings:Water Elemental", "STRENGTH=", 75);
	creatureInfo[kWaterElemental].maxIntelligence					= fi_GetData(":Creature Settings:Water Elemental", "INTELLIGENCE=", 60);
	creatureInfo[kWaterElemental].maxDexterity						= fi_GetData(":Creature Settings:Water Elemental", "DEXTERITY=", 50);
	creatureInfo[kWaterElemental].minGold								  = fi_GetData(":Creature Settings:Water Elemental", "MIN_GOLD=", 18);
	creatureInfo[kWaterElemental].maxGold								  = fi_GetData(":Creature Settings:Water Elemental", "MAX_GOLD=", 32);
	creatureInfo[kWaterElemental].seekGold								= fi_GetBoolean(":Creature Settings:Water Elemental", "SEEK_GOLD=", true);
	creatureInfo[kWaterElemental].seekMagic								= fi_GetBoolean(":Creature Settings:Water Elemental", "SEEK_MAGIC=", false);
	creatureInfo[kWaterElemental].seekWeapons							= fi_GetBoolean(":Creature Settings:Water Elemental", "SEEK_WEAPONS=", false);
	creatureInfo[kWaterElemental].magicResist				      = (float)fi_GetData(":Creature Settings:Water Elemental", "MAGIC_RESIST=", 80)/100.0;
	creatureInfo[kWaterElemental].tamingLevel							= fi_GetData(":Creature Settings:Water Elemental", "TAMING_LEVEL=", 5);
	creatureInfo[kWaterElemental].creatureLevel							= fi_GetData(":Creature Settings:Water Elemental", "TAMING_LEVEL=", 5);
	creatureInfo[kWaterElemental].armorRating							= fi_GetData(":Creature Settings:Water Elemental", "LEVEL=", 1);
	creatureInfo[kWaterElemental].immuneToPoison          = fi_GetBoolean(":Creature Settings:Water Elemental", "POISON_IMMUNITY=", false);
	creatureInfo[kWaterElemental].lockStrength						= fi_GetData(":Creature Settings:Water Elemental", "LOCK=", 0);
	creatureInfo[kWaterElemental].attackDamageMin					= fi_GetData(":Creature Settings:Water Elemental", "MIN_DAMAGE=", 6);
	creatureInfo[kWaterElemental].attackDamageMax					= fi_GetData(":Creature Settings:Water Elemental", "MAX_DAMAGE=", 9);
	creatureInfo[kWaterElemental].trackingLevel							= fi_GetData(":Creature Settings:Water Elemental", "TRACKING_LEVEL=", 101);
	creatureInfo[kWaterElemental].size							= fi_GetData(":Creature Settings:Water Elemental", "SIZE=", 2);
	creatureInfo[kWaterElemental].mageryLevel					= fi_GetData(":Creature Settings:Water Elemental", "MAGERY_LEVEL=", 0);
	creatureInfo[kWaterElemental].spellcastChance				= fi_GetData(":Creature Settings:Water Elemental", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kWaterElemental].selfhealChance				= fi_GetData(":Creature Settings:Water Elemental", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kWaterElemental].selfhealTrigger				= fi_GetData(":Creature Settings:Water Elemental", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Water Elemental", kWaterElemental);
	fi_GetCreatureTalkScript(":Creature Settings:Water Elemental", kWaterElemental);
	fi_GetCreatureInventory(":Creature Settings:Water Elemental", kWaterElemental);
	fi_GetWeaponModifier(":Creature Settings:Water Elemental", kWaterElemental);

	creatureInfo[kFireElemental].bmp								=	0;
	creatureInfo[kFireElemental].domain							=	kLand;
	creatureInfo[kFireElemental].terrain						=	kAnyTerrain;
	creatureInfo[kFireElemental].alignment					= kEvil;
	creatureInfo[kFireElemental].desiredPopulation	= 875;
	creatureInfo[kFireElemental].points		        	= 4;
	creatureInfo[kFireElemental].color		        	= kFireElementalColor;
	creatureInfo[kFireElemental].meatEater					= true;
	creatureInfo[kFireElemental].canMorph					  = true;
	creatureInfo[kFireElemental].fire								= true;
	creatureInfo[kFireElemental].bleeds							= false;
	creatureInfo[kFireElemental].speed										=	fi_GetData(":Creature Settings:Fire Elemental", "SPEED=", 35);
	creatureInfo[kFireElemental].poisonous					      = fi_GetData(":Creature Settings:Fire Elemental", "POISONOUS=", 0);
	creatureInfo[kFireElemental].curse					      		= fi_GetData(":Creature Settings:Fire Elemental", "CURSE=", 0);
	creatureInfo[kFireElemental].maxHitPoints							= fi_GetData(":Creature Settings:Fire Elemental", "HIT_POINTS=", 75);
	creatureInfo[kFireElemental].maxStrength							= fi_GetData(":Creature Settings:Fire Elemental", "STRENGTH=", 75);
	creatureInfo[kFireElemental].maxIntelligence					= fi_GetData(":Creature Settings:Fire Elemental", "INTELLIGENCE=", 60);
	creatureInfo[kFireElemental].maxDexterity							= fi_GetData(":Creature Settings:Fire Elemental", "DEXTERITY=", 50);
	creatureInfo[kFireElemental].minGold								  = fi_GetData(":Creature Settings:Fire Elemental", "MIN_GOLD=", 30);
	creatureInfo[kFireElemental].maxGold								  = fi_GetData(":Creature Settings:Fire Elemental", "MAX_GOLD=", 55);
	creatureInfo[kFireElemental].seekGold									= fi_GetBoolean(":Creature Settings:Fire Elemental", "SEEK_GOLD=", true);
	creatureInfo[kFireElemental].seekMagic								= fi_GetBoolean(":Creature Settings:Fire Elemental", "SEEK_MAGIC=", false);
	creatureInfo[kFireElemental].seekWeapons							= fi_GetBoolean(":Creature Settings:Fire Elemental", "SEEK_WEAPONS=", false);
	creatureInfo[kFireElemental].magicResist				      = (float)fi_GetData(":Creature Settings:Fire Elemental", "MAGIC_RESIST=", 75)/100.0;
	creatureInfo[kFireElemental].tamingLevel							= fi_GetData(":Creature Settings:Fire Elemental", "TAMING_LEVEL=", 6);
	creatureInfo[kFireElemental].creatureLevel							= fi_GetData(":Creature Settings:Fire Elemental", "LEVEL=", 1);
	creatureInfo[kFireElemental].armorRating							= fi_GetData(":Creature Settings:Fire Elemental", "ARMOR_RATING=", 10);
	creatureInfo[kFireElemental].immuneToPoison           = fi_GetBoolean(":Creature Settings:Fire Elemental", "POISON_IMMUNITY=", false);
	creatureInfo[kFireElemental].lockStrength							= fi_GetData(":Creature Settings:Fire Elemental", "LOCK=", 0);
	creatureInfo[kFireElemental].attackDamageMin					= fi_GetData(":Creature Settings:Fire Elemental", "MIN_DAMAGE=", 6);
	creatureInfo[kFireElemental].attackDamageMax					= fi_GetData(":Creature Settings:Fire Elemental", "MAX_DAMAGE=", 9);
	creatureInfo[kFireElemental].trackingLevel							= fi_GetData(":Creature Settings:Fire Elemental", "TRACKING_LEVEL=", 101);
	creatureInfo[kFireElemental].size							= fi_GetData(":Creature Settings:Fire Elemental", "SIZE=", 2);
	creatureInfo[kFireElemental].mageryLevel					= fi_GetData(":Creature Settings:Fire Elemental", "MAGERY_LEVEL=", 0);
	creatureInfo[kFireElemental].spellcastChance				= fi_GetData(":Creature Settings:Fire Elemental", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kFireElemental].selfhealChance					= fi_GetData(":Creature Settings:Fire Elemental", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kFireElemental].selfhealTrigger				= fi_GetData(":Creature Settings:Fire Elemental", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Fire Elemental", kFireElemental);
	//cr_AddToCreatureControlInventory(kFireElemental, kVolcanicAsh, 4, 12);
	fi_GetCreatureTalkScript(":Creature Settings:Fire Elemental", kFireElemental);
	fi_GetCreatureInventory(":Creature Settings:Fire Elemental", kFireElemental);
	fi_GetWeaponModifier(":Creature Settings:Fire Elemental", kFireElemental);
	fi_GetWeaponModifier(":Creature Settings:Fire Elemental", kFireElemental);

	creatureInfo[kPoisonElemental].bmp							=	0;
	creatureInfo[kPoisonElemental].domain						=	kLand;
	creatureInfo[kPoisonElemental].terrain					=	kAnyTerrain;
	creatureInfo[kPoisonElemental].alignment				= kEvil;
	creatureInfo[kPoisonElemental].desiredPopulation= 450;
	creatureInfo[kPoisonElemental].points		        = 3;
	creatureInfo[kPoisonElemental].color		        = kPoisonElementalColor;
	creatureInfo[kPoisonElemental].meatEater				= true;
	creatureInfo[kPoisonElemental].canMorph					= true;
	creatureInfo[kPoisonElemental].bleeds						= false;
	creatureInfo[kPoisonElemental].speed										=	fi_GetData(":Creature Settings:Poison Elemental", "SPEED=", 75);
	creatureInfo[kPoisonElemental].poisonous					      = fi_GetData(":Creature Settings:Poison Elemental", "POISONOUS=", 100);
	creatureInfo[kPoisonElemental].curse					      		= fi_GetData(":Creature Settings:Poison Elemental", "CURSE=", 100);
	creatureInfo[kPoisonElemental].maxHitPoints							= fi_GetData(":Creature Settings:Poison Elemental", "HIT_POINTS=", 75);
	creatureInfo[kPoisonElemental].maxStrength							= fi_GetData(":Creature Settings:Poison Elemental", "STRENGTH=", 75);
	creatureInfo[kPoisonElemental].maxIntelligence					= fi_GetData(":Creature Settings:Poison Elemental", "INTELLIGENCE=", 60);
	creatureInfo[kPoisonElemental].maxDexterity							= fi_GetData(":Creature Settings:Poison Elemental", "DEXTERITY=", 50);
	creatureInfo[kPoisonElemental].minGold								  = fi_GetData(":Creature Settings:Poison Elemental", "MIN_GOLD=", 22);
	creatureInfo[kPoisonElemental].maxGold								  = fi_GetData(":Creature Settings:Poison Elemental", "MAX_GOLD=", 40);
	creatureInfo[kPoisonElemental].seekGold									= fi_GetBoolean(":Creature Settings:Poison Elemental", "SEEK_GOLD=", true);
	creatureInfo[kPoisonElemental].seekMagic								= fi_GetBoolean(":Creature Settings:Poison Elemental", "SEEK_MAGIC=", false);
	creatureInfo[kPoisonElemental].seekWeapons							= fi_GetBoolean(":Creature Settings:Poison Elemental", "SEEK_WEAPONS=", false);
	creatureInfo[kPoisonElemental].magicResist				      = (float)fi_GetData(":Creature Settings:Poison Elemental", "MAGIC_RESIST=", 75)/100.0;
	creatureInfo[kPoisonElemental].tamingLevel							= fi_GetData(":Creature Settings:Poison Elemental", "TAMING_LEVEL=", 6);
	creatureInfo[kPoisonElemental].creatureLevel							= fi_GetData(":Creature Settings:Poison Elemental", "LEVEL=", 1);
	creatureInfo[kPoisonElemental].armorRating							= fi_GetData(":Creature Settings:Poison Elemental", "ARMOR_RATING=", 10);
	creatureInfo[kPoisonElemental].immuneToPoison           = fi_GetBoolean(":Creature Settings:Poison Elemental", "POISON_IMMUNITY=", true);
	creatureInfo[kPoisonElemental].lockStrength							= fi_GetData(":Creature Settings:Poison Elemental", "LOCK=", 0);
	creatureInfo[kPoisonElemental].attackDamageMin					= fi_GetData(":Creature Settings:Poison Elemental", "MIN_DAMAGE=", 6);
	creatureInfo[kPoisonElemental].attackDamageMax					= fi_GetData(":Creature Settings:Poison Elemental", "MAX_DAMAGE=", 9);
	creatureInfo[kPoisonElemental].trackingLevel							= fi_GetData(":Creature Settings:Poison Elemental", "TRACKING_LEVEL=", 101);
	creatureInfo[kPoisonElemental].size							= fi_GetData(":Creature Settings:Poison Elemental", "SIZE=", 2);
	creatureInfo[kPoisonElemental].mageryLevel					= fi_GetData(":Creature Settings:Poison Elemental", "MAGERY_LEVEL=", 0);
	creatureInfo[kPoisonElemental].spellcastChance				= fi_GetData(":Creature Settings:Poison Elemental", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kPoisonElemental].selfhealChance				= fi_GetData(":Creature Settings:Poison Elemental", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kPoisonElemental].selfhealTrigger				= fi_GetData(":Creature Settings:Poison Elemental", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Poison Elemental", kPoisonElemental);
	//cr_AddToCreatureControlInventory(kPoisonElemental, kPoison, 2, 7);
	fi_GetCreatureTalkScript(":Creature Settings:Poison Elemental", kPoisonElemental);
	fi_GetCreatureInventory(":Creature Settings:Poison Elemental", kPoisonElemental);
	fi_GetWeaponModifier(":Creature Settings:Poison Elemental", kPoisonElemental);
	
	//Dave
	
	
	creatureInfo[kEarthElemental].bmp								=	0;
	creatureInfo[kEarthElemental].domain						=	kLand;
	creatureInfo[kEarthElemental].terrain						=	kAnyTerrain;
	creatureInfo[kEarthElemental].alignment					= kEvil;
	creatureInfo[kEarthElemental].desiredPopulation	= 3500;//4000;
	creatureInfo[kEarthElemental].points		        = 3;
	creatureInfo[kEarthElemental].color		          = kBrownWolfColor;
	creatureInfo[kEarthElemental].meatEater					= true;
	creatureInfo[kEarthElemental].canMorph					= true;
	creatureInfo[kEarthElemental].bleeds						= false;
	creatureInfo[kEarthElemental].speed										=	fi_GetData(":Creature Settings:Earth Elemental", "SPEED=", 80);
	creatureInfo[kEarthElemental].poisonous					      = fi_GetData(":Creature Settings:Earth Elemental", "POISONOUS=", 0);
	creatureInfo[kEarthElemental].curse					      		= fi_GetData(":Creature Settings:Earth Elemental", "CURSE=", 0);
	creatureInfo[kEarthElemental].maxHitPoints						= fi_GetData(":Creature Settings:Earth Elemental", "HIT_POINTS=", 75);
	creatureInfo[kEarthElemental].maxStrength							= fi_GetData(":Creature Settings:Earth Elemental", "STRENGTH=", 75);
	creatureInfo[kEarthElemental].maxIntelligence					= fi_GetData(":Creature Settings:Earth Elemental", "INTELLIGENCE=", 60);
	creatureInfo[kEarthElemental].maxDexterity						= fi_GetData(":Creature Settings:Earth Elemental", "DEXTERITY=", 50);
	creatureInfo[kEarthElemental].minGold								  = fi_GetData(":Creature Settings:Earth Elemental", "MIN_GOLD=", 18);
	creatureInfo[kEarthElemental].maxGold								  = fi_GetData(":Creature Settings:Earth Elemental", "MAX_GOLD=", 32);
	creatureInfo[kEarthElemental].seekGold								= fi_GetBoolean(":Creature Settings:Earth Elemental", "SEEK_GOLD=", true);
	creatureInfo[kEarthElemental].seekMagic								= fi_GetBoolean(":Creature Settings:Earth Elemental", "SEEK_MAGIC=", false);
	creatureInfo[kEarthElemental].seekWeapons							= fi_GetBoolean(":Creature Settings:Earth Elemental", "SEEK_WEAPONS=", false);
	creatureInfo[kEarthElemental].magicResist				      = (float)fi_GetData(":Creature Settings:Earth Elemental", "MAGIC_RESIST=", 80)/100.0;
	creatureInfo[kEarthElemental].tamingLevel							= fi_GetData(":Creature Settings:Earth Elemental", "TAMING_LEVEL=", 5);
	creatureInfo[kEarthElemental].creatureLevel							= fi_GetData(":Creature Settings:Earth Elemental", "TAMING_LEVEL=", 5);
	creatureInfo[kEarthElemental].armorRating							= fi_GetData(":Creature Settings:Earth Elemental", "LEVEL=", 1);
	creatureInfo[kEarthElemental].immuneToPoison          = fi_GetBoolean(":Creature Settings:Earth Elemental", "POISON_IMMUNITY=", false);
	creatureInfo[kEarthElemental].lockStrength						= fi_GetData(":Creature Settings:Earth Elemental", "LOCK=", 0);
	creatureInfo[kEarthElemental].attackDamageMin					= fi_GetData(":Creature Settings:Earth Elemental", "MIN_DAMAGE=", 6);
	creatureInfo[kEarthElemental].attackDamageMax					= fi_GetData(":Creature Settings:Earth Elemental", "MAX_DAMAGE=", 9);
	creatureInfo[kEarthElemental].trackingLevel							= fi_GetData(":Creature Settings:Earth Elemental", "TRACKING_LEVEL=", 101);
	creatureInfo[kEarthElemental].size							= fi_GetData(":Creature Settings:Earth Elemental", "SIZE=", 2);
	creatureInfo[kEarthElemental].mageryLevel					= fi_GetData(":Creature Settings:Earth Elemental", "MAGERY_LEVEL=", 0);
	creatureInfo[kEarthElemental].spellcastChance				= fi_GetData(":Creature Settings:Earth Elemental", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kEarthElemental].selfhealChance				= fi_GetData(":Creature Settings:Earth Elemental", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kEarthElemental].selfhealTrigger				= fi_GetData(":Creature Settings:Earth Elemental", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Earth Elemental", kEarthElemental);
	fi_GetCreatureTalkScript(":Creature Settings:Earth Elemental", kEarthElemental);
	fi_GetCreatureInventory(":Creature Settings:Earth Elemental", kEarthElemental);
	fi_GetWeaponModifier(":Creature Settings:Earth Elemental", kEarthElemental);
	//Dave^
	creatureInfo[kSlug].bmp												=	0;
	creatureInfo[kSlug].domain										=	kLand;
	creatureInfo[kSlug].terrain										=	kGrass;
	creatureInfo[kSlug].alignment									= kEvil;
	creatureInfo[kSlug].directionalGraphics				= true;
	creatureInfo[kSlug].graphicsIndex							= 1;
	creatureInfo[kSlug].corpseID									= 3;
	creatureInfo[kSlug].attackSound								= kSlugAttack;
	creatureInfo[kSlug].desiredPopulation					= 700;
	creatureInfo[kSlug].meatEater									= true;
	creatureInfo[kSlug].points									  = 3;
	creatureInfo[kSlug].color						    			= kForestDragonColor;
	creatureInfo[kSlug].canMorph					        = true;
	creatureInfo[kSlug].speed										=	fi_GetData(":Creature Settings:Sludge Beast", "SPEED=", 100);
	creatureInfo[kSlug].poisonous					      = fi_GetData(":Creature Settings:Sludge Beast", "POISONOUS=", 0);
	creatureInfo[kSlug].curse					      		= fi_GetData(":Creature Settings:Sludge Beast", "CURSE=", 0);
	creatureInfo[kSlug].maxHitPoints						= fi_GetData(":Creature Settings:Sludge Beast", "HIT_POINTS=", 100);
	creatureInfo[kSlug].maxStrength							= fi_GetData(":Creature Settings:Sludge Beast", "STRENGTH=", 100);
	creatureInfo[kSlug].maxIntelligence					= fi_GetData(":Creature Settings:Sludge Beast", "INTELLIGENCE=", 30);
	creatureInfo[kSlug].maxDexterity						= fi_GetData(":Creature Settings:Sludge Beast", "DEXTERITY=", 50);
	creatureInfo[kSlug].minGold								  = fi_GetData(":Creature Settings:Sludge Beast", "MIN_GOLD=", 30);
	creatureInfo[kSlug].maxGold								  = fi_GetData(":Creature Settings:Sludge Beast", "MAX_GOLD=", 45);
	creatureInfo[kSlug].seekGold								= fi_GetBoolean(":Creature Settings:Sludge Beast", "SEEK_GOLD=", FALSE);
	creatureInfo[kSlug].seekMagic								= fi_GetBoolean(":Creature Settings:Sludge Beast", "SEEK_MAGIC=", FALSE);
	creatureInfo[kSlug].seekWeapons							= fi_GetBoolean(":Creature Settings:Sludge Beast", "SEEK_WEAPONS=", FALSE);
	creatureInfo[kSlug].magicResist				      = (float)fi_GetData(":Creature Settings:Sludge Beast", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kSlug].tamingLevel							= fi_GetData(":Creature Settings:Sludge Beast", "TAMING_LEVEL=", 6);
	creatureInfo[kSlug].creatureLevel							= fi_GetData(":Creature Settings:Sludge Beast", "LEVEL=", 1);
	creatureInfo[kSlug].armorRating							= fi_GetData(":Creature Settings:Sludge Beast", "ARMOR_RATING=", 15);
	creatureInfo[kSlug].immuneToPoison          = fi_GetBoolean(":Creature Settings:Sludge Beast", "POISON_IMMUNITY=", false);
	creatureInfo[kSlug].lockStrength						= fi_GetData(":Creature Settings:Sludge Beast", "LOCK=", 0);
	creatureInfo[kSlug].attackDamageMin					= fi_GetData(":Creature Settings:Sludge Beast", "MIN_DAMAGE=", 6);
	creatureInfo[kSlug].attackDamageMax					= fi_GetData(":Creature Settings:Sludge Beast", "MAX_DAMAGE=", 10);
	creatureInfo[kSlug].trackingLevel							= fi_GetData(":Creature Settings:Sludge Beast", "TRACKING_LEVEL=", 101);
	creatureInfo[kSlug].size							= fi_GetData(":Creature Settings:Sludge Beast", "SIZE=", 2);
	creatureInfo[kSlug].mageryLevel					= fi_GetData(":Creature Settings:Sludge Beast", "MAGERY_LEVEL=", 0);
	creatureInfo[kSlug].spellcastChance				= fi_GetData(":Creature Settings:Sludge Beast", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSlug].selfhealChance				= fi_GetData(":Creature Settings:Sludge Beast", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSlug].selfhealTrigger				= fi_GetData(":Creature Settings:Sludge Beast", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Sludge Beast", kSlug);
	fi_GetCreatureTalkScript(":Creature Settings:Sludge Beast", kSlug);
	fi_GetCreatureInventory(":Creature Settings:Sludge Beast", kSlug);
	fi_GetWeaponModifier(":Creature Settings:Sludge Beast", kSlug);

	creatureInfo[kRustBeast].bmp									=	0;
	creatureInfo[kRustBeast].domain								=	kLand;
	creatureInfo[kRustBeast].terrain							=	kGrass;
	creatureInfo[kRustBeast].alignment						= kEvil;
	creatureInfo[kRustBeast].directionalGraphics	= true;
	creatureInfo[kRustBeast].graphicsIndex				= 1;
	creatureInfo[kRustBeast].corpseID							= 44;
	creatureInfo[kRustBeast].attackSound					= kSlugAttack;
	creatureInfo[kRustBeast].desiredPopulation		= 500;
	creatureInfo[kRustBeast].meatEater						= true;
	creatureInfo[kRustBeast].points								= 3;
	creatureInfo[kRustBeast].color						    = kRedDragonColor;
	creatureInfo[kRustBeast].canMorph					    = true;
	creatureInfo[kRustBeast].speed										=	fi_GetData(":Creature Settings:Rust Beast", "SPEED=", 100);
	creatureInfo[kRustBeast].poisonous					      = fi_GetData(":Creature Settings:Rust Beast", "POISONOUS=", 0);
	creatureInfo[kRustBeast].curse					      		= fi_GetData(":Creature Settings:Rust Beast", "CURSE=", 0);
	creatureInfo[kRustBeast].maxHitPoints							= fi_GetData(":Creature Settings:Rust Beast", "HIT_POINTS=", 100);
	creatureInfo[kRustBeast].maxStrength							= fi_GetData(":Creature Settings:Rust Beast", "STRENGTH=", 100);
	creatureInfo[kRustBeast].maxIntelligence					= fi_GetData(":Creature Settings:Rust Beast", "INTELLIGENCE=", 30);
	creatureInfo[kRustBeast].maxDexterity							= fi_GetData(":Creature Settings:Rust Beast", "DEXTERITY=", 50);
	creatureInfo[kRustBeast].minGold								  = fi_GetData(":Creature Settings:Rust Beast", "MIN_GOLD=", 30);
	creatureInfo[kRustBeast].maxGold								  = fi_GetData(":Creature Settings:Rust Beast", "MAX_GOLD=", 45);
	creatureInfo[kRustBeast].seekGold									= fi_GetBoolean(":Creature Settings:Rust Beast", "SEEK_GOLD=", false);
	creatureInfo[kRustBeast].seekMagic								= fi_GetBoolean(":Creature Settings:Rust Beast", "SEEK_MAGIC=", false);
	creatureInfo[kRustBeast].seekWeapons							= fi_GetBoolean(":Creature Settings:Rust Beast", "SEEK_WEAPONS=", false);
	creatureInfo[kRustBeast].magicResist				      = (float)fi_GetData(":Creature Settings:Rust Beast", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kRustBeast].tamingLevel							= fi_GetData(":Creature Settings:Rust Beast", "TAMING_LEVEL=", 6);
	creatureInfo[kRustBeast].creatureLevel							= fi_GetData(":Creature Settings:Rust Beast", "LEVEL=", 1);
	creatureInfo[kRustBeast].armorRating							= fi_GetData(":Creature Settings:Rust Beast", "ARMOR_RATING=", 15);
	creatureInfo[kRustBeast].immuneToPoison           = fi_GetBoolean(":Creature Settings:Rust Beast", "POISON_IMMUNITY=", false);
	creatureInfo[kRustBeast].lockStrength							= fi_GetData(":Creature Settings:Rust Beast", "LOCK=", 0);
	creatureInfo[kRustBeast].attackDamageMin					= fi_GetData(":Creature Settings:Rust Beast", "MIN_DAMAGE=", 6);
	creatureInfo[kRustBeast].attackDamageMax					= fi_GetData(":Creature Settings:Rust Beast", "MAX_DAMAGE=", 10);
	creatureInfo[kRustBeast].trackingLevel							= fi_GetData(":Creature Settings:Rust Beast", "TRACKING_LEVEL=", 101);
	creatureInfo[kRustBeast].size									= fi_GetData(":Creature Settings:Rust Beast", "SIZE=", 2);
	creatureInfo[kRustBeast].mageryLevel					= fi_GetData(":Creature Settings:Rust Beast", "MAGERY_LEVEL=", 0);
	creatureInfo[kRustBeast].spellcastChance				= fi_GetData(":Creature Settings:Rust Beast", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kRustBeast].selfhealChance				= fi_GetData(":Creature Settings:Rust Beast", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kRustBeast].selfhealTrigger				= fi_GetData(":Creature Settings:Rust Beast", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Rust Beast", kRustBeast);
	fi_GetCreatureTalkScript(":Creature Settings:Rust Beast", kRustBeast);
	fi_GetCreatureInventory(":Creature Settings:Rust Beast", kRustBeast);
	fi_GetWeaponModifier(":Creature Settings:Rust Beast", kRustBeast);

	creatureInfo[kPoisonBeast].bmp											=	0;
	creatureInfo[kPoisonBeast].domain										=	kLand;
	creatureInfo[kPoisonBeast].terrain									=	kGrass;
	creatureInfo[kPoisonBeast].alignment								= kEvil;
	creatureInfo[kPoisonBeast].directionalGraphics			= true;
	creatureInfo[kPoisonBeast].graphicsIndex						= 1;
	creatureInfo[kPoisonBeast].corpseID									= 73;
	creatureInfo[kPoisonBeast].attackSound							= kSlugAttack;
	creatureInfo[kPoisonBeast].desiredPopulation				= 500;
	creatureInfo[kPoisonBeast].meatEater								= true;
	creatureInfo[kPoisonBeast].points										= 3;
	creatureInfo[kPoisonBeast].color						    		= kBlackWolfColor;
	creatureInfo[kPoisonBeast].canMorph					    		= true;
	creatureInfo[kPoisonBeast].speed										=	fi_GetData(":Creature Settings:Poison Beast", "SPEED=", 100);
	creatureInfo[kPoisonBeast].poisonous					      = fi_GetData(":Creature Settings:Poison Beast", "POISONOUS=", 100);
	creatureInfo[kPoisonBeast].curse					      		= fi_GetData(":Creature Settings:Poison Beast", "CURSE=", 0);
	creatureInfo[kPoisonBeast].maxHitPoints							= fi_GetData(":Creature Settings:Poison Beast", "HIT_POINTS=", 100);
	creatureInfo[kPoisonBeast].maxStrength							= fi_GetData(":Creature Settings:Poison Beast", "STRENGTH=", 100);
	creatureInfo[kPoisonBeast].maxIntelligence					= fi_GetData(":Creature Settings:Poison Beast", "INTELLIGENCE=", 30);
	creatureInfo[kPoisonBeast].maxDexterity							= fi_GetData(":Creature Settings:Poison Beast", "DEXTERITY=", 50);
	creatureInfo[kPoisonBeast].minGold								  = fi_GetData(":Creature Settings:Poison Beast", "MIN_GOLD=", 30);
	creatureInfo[kPoisonBeast].maxGold								  = fi_GetData(":Creature Settings:Poison Beast", "MAX_GOLD=", 45);
	creatureInfo[kPoisonBeast].seekGold									= fi_GetBoolean(":Creature Settings:Poison Beast", "SEEK_GOLD=", false);
	creatureInfo[kPoisonBeast].seekMagic								= fi_GetBoolean(":Creature Settings:Poison Beast", "SEEK_MAGIC=", false);
	creatureInfo[kPoisonBeast].seekWeapons							= fi_GetBoolean(":Creature Settings:Poison Beast", "SEEK_WEAPONS=", false);
	creatureInfo[kPoisonBeast].magicResist				      = (float)fi_GetData(":Creature Settings:Poison Beast", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kPoisonBeast].tamingLevel							= fi_GetData(":Creature Settings:Poison Beast", "TAMING_LEVEL=", 6);
	creatureInfo[kPoisonBeast].creatureLevel							= fi_GetData(":Creature Settings:Poison Beast", "LEVEL=", 1);
	creatureInfo[kPoisonBeast].armorRating							= fi_GetData(":Creature Settings:Poison Beast", "ARMOR_RATING=", 15);
	creatureInfo[kPoisonBeast].immuneToPoison           = fi_GetBoolean(":Creature Settings:Poison Beast", "POISON_IMMUNITY=", false);
	creatureInfo[kPoisonBeast].lockStrength							= fi_GetData(":Creature Settings:Poison Beast", "LOCK=", 0);
	creatureInfo[kPoisonBeast].attackDamageMin					= fi_GetData(":Creature Settings:Poison Beast", "MIN_DAMAGE=", 6);
	creatureInfo[kPoisonBeast].attackDamageMax					= fi_GetData(":Creature Settings:Poison Beast", "MAX_DAMAGE=", 10);
	creatureInfo[kPoisonBeast].trackingLevel							= fi_GetData(":Creature Settings:Poison Beast", "TRACKING_LEVEL=", 101);
	creatureInfo[kPoisonBeast].size									= fi_GetData(":Creature Settings:Poison Beast", "SIZE=", 2);
	creatureInfo[kPoisonBeast].mageryLevel					= fi_GetData(":Creature Settings:Poison Beast", "MAGERY_LEVEL=", 0);
	creatureInfo[kPoisonBeast].spellcastChance				= fi_GetData(":Creature Settings:Poison Beast", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kPoisonBeast].selfhealChance				= fi_GetData(":Creature Settings:Poison Beast", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kPoisonBeast].selfhealTrigger				= fi_GetData(":Creature Settings:Poison Beast", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Poison Beast", kPoisonBeast);
	fi_GetCreatureTalkScript(":Creature Settings:Poison Beast", kPoisonBeast);
	fi_GetCreatureInventory(":Creature Settings:Poison Beast", kPoisonBeast);
	fi_GetWeaponModifier(":Creature Settings:Poison Beast", kPoisonBeast);

	creatureInfo[kSeaTurtle].bmp									=	0;
	creatureInfo[kSeaTurtle].domain								=	kWater;
	creatureInfo[kSeaTurtle].alignment						= kNeutral;
	creatureInfo[kSeaTurtle].directionalGraphics	= true;
	creatureInfo[kSeaTurtle].pauseGraphics				= true;
	creatureInfo[kSeaTurtle].graphicsIndex				= 1;
	creatureInfo[kSeaTurtle].corpseID							= 16;
	creatureInfo[kSeaTurtle].desiredPopulation		= 5000;
	creatureInfo[kSeaTurtle].meatEater						= true;
	creatureInfo[kSeaTurtle].canMorph					    = true;
	creatureInfo[kSeaTurtle].speed										=	fi_GetData(":Creature Settings:Sea Turtle", "SPEED=", 90);
	creatureInfo[kSeaTurtle].poisonous					      = fi_GetData(":Creature Settings:Sea Turtle", "POISONOUS=", 0);
	creatureInfo[kSeaTurtle].curse					      		= fi_GetData(":Creature Settings:Sea Turtle", "CURSE=", 0);
	creatureInfo[kSeaTurtle].maxHitPoints							= fi_GetData(":Creature Settings:Sea Turtle", "HIT_POINTS=", 20);
	creatureInfo[kSeaTurtle].maxStrength							= fi_GetData(":Creature Settings:Sea Turtle", "STRENGTH=", 20);
	creatureInfo[kSeaTurtle].maxIntelligence					= fi_GetData(":Creature Settings:Sea Turtle", "INTELLIGENCE=", 10);
	creatureInfo[kSeaTurtle].maxDexterity							= fi_GetData(":Creature Settings:Sea Turtle", "DEXTERITY=", 10);
	creatureInfo[kSeaTurtle].minGold								  = fi_GetData(":Creature Settings:Sea Turtle", "MIN_GOLD=", 0);
	creatureInfo[kSeaTurtle].maxGold								  = fi_GetData(":Creature Settings:Sea Turtle", "MAX_GOLD=", 0);
	creatureInfo[kSeaTurtle].seekGold									= fi_GetBoolean(":Creature Settings:Sea Turtle", "SEEK_GOLD=", false);
	creatureInfo[kSeaTurtle].seekMagic								= fi_GetBoolean(":Creature Settings:Sea Turtle", "SEEK_MAGIC=", false);
	creatureInfo[kSeaTurtle].seekWeapons							= fi_GetBoolean(":Creature Settings:Sea Turtle", "SEEK_WEAPONS=", false);
	creatureInfo[kSeaTurtle].magicResist				      = (float)fi_GetData(":Creature Settings:Sea Turtle", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kSeaTurtle].tamingLevel							= fi_GetData(":Creature Settings:Sea Turtle", "TAMING_LEVEL=", 2);
	creatureInfo[kSeaTurtle].creatureLevel							= fi_GetData(":Creature Settings:Sea Turtle", "LEVEL=", 1);
	creatureInfo[kSeaTurtle].armorRating							= fi_GetData(":Creature Settings:Sea Turtle", "ARMOR_RATING=", 30);
	creatureInfo[kSeaTurtle].immuneToPoison           = fi_GetBoolean(":Creature Settings:Sea Turtle", "POISON_IMMUNITY=", false);
	creatureInfo[kSeaTurtle].lockStrength							= fi_GetData(":Creature Settings:Sea Turtle", "LOCK=", 0);
	creatureInfo[kSeaTurtle].attackDamageMin					= fi_GetData(":Creature Settings:Sea Turtle", "MIN_DAMAGE=", 1);
	creatureInfo[kSeaTurtle].attackDamageMax					= fi_GetData(":Creature Settings:Sea Turtle", "MAX_DAMAGE=", 2);
	creatureInfo[kSeaTurtle].trackingLevel							= fi_GetData(":Creature Settings:Sea Turtle", "TRACKING_LEVEL=", 101);
	creatureInfo[kSeaTurtle].size									= fi_GetData(":Creature Settings:Sea Turtle", "SIZE=", 1);
	creatureInfo[kSeaTurtle].mageryLevel					= fi_GetData(":Creature Settings:Sea Turtle", "MAGERY_LEVEL=", 0);
	creatureInfo[kSeaTurtle].spellcastChance				= fi_GetData(":Creature Settings:Sea Turtle", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSeaTurtle].selfhealChance					= fi_GetData(":Creature Settings:Sea Turtle", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSeaTurtle].selfhealTrigger				= fi_GetData(":Creature Settings:Sea Turtle", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Sea Turtle", kSeaTurtle);
	//cr_AddToCreatureControlInventory(kSeaTurtle, kRawSteak, 2, 4);
	//cr_AddToCreatureControlInventory(kSeaTurtle, kTurtleShell, 1, 1);
	fi_GetCreatureTalkScript(":Creature Settings:Sea Turtle", kSeaTurtle);
	fi_GetCreatureInventory(":Creature Settings:Sea Turtle", kSeaTurtle);
	fi_GetWeaponModifier(":Creature Settings:Sea Turtle", kSeaTurtle);

	creatureInfo[kSerpent].bmp										=	2;
	creatureInfo[kSerpent].domain								  =	kWater;
	creatureInfo[kSerpent].alignment							= kEvil;
	creatureInfo[kSerpent].directionalGraphics		= true;
	creatureInfo[kSerpent].graphicsIndex					= 1;
	creatureInfo[kSerpent].corpseID							  = 42;
	creatureInfo[kSerpent].attackRange						= 2;
	creatureInfo[kSerpent].desiredPopulation			= 500;
	creatureInfo[kSerpent].points			            = 8;
	creatureInfo[kSerpent].rareLevel						  = 3;
	creatureInfo[kSerpent].rareChance					    = 50;
	creatureInfo[kSerpent].meatEater						  = true;
	creatureInfo[kSerpent].canMorph					      = true;
	creatureInfo[kSerpent].attackSound						= kSerpentAttackSnd;
	creatureInfo[kSerpent].speed										=	fi_GetData(":Creature Settings:Sea Serpent", "SPEED=", 50);
	creatureInfo[kSerpent].poisonous					      = fi_GetData(":Creature Settings:Sea Serpent", "POISONOUS=", 0);
	creatureInfo[kSerpent].curse					      		= fi_GetData(":Creature Settings:Sea Serpent", "CURSE=", 0);
	creatureInfo[kSerpent].maxHitPoints							= fi_GetData(":Creature Settings:Sea Serpent", "HIT_POINTS=", 175);
	creatureInfo[kSerpent].maxStrength							= fi_GetData(":Creature Settings:Sea Serpent", "STRENGTH=", 175);
	creatureInfo[kSerpent].maxIntelligence					= fi_GetData(":Creature Settings:Sea Serpent", "INTELLIGENCE=", 60);
	creatureInfo[kSerpent].maxDexterity							= fi_GetData(":Creature Settings:Sea Serpent", "DEXTERITY=", 50);
	creatureInfo[kSerpent].minGold								  = fi_GetData(":Creature Settings:Sea Serpent", "MIN_GOLD=", 50);
	creatureInfo[kSerpent].maxGold								  = fi_GetData(":Creature Settings:Sea Serpent", "MAX_GOLD=", 100);
	creatureInfo[kSerpent].seekGold									= fi_GetBoolean(":Creature Settings:Sea Serpent", "SEEK_GOLD=", true);
	creatureInfo[kSerpent].seekMagic								= fi_GetBoolean(":Creature Settings:Sea Serpent", "SEEK_MAGIC=", false);
	creatureInfo[kSerpent].seekWeapons							= fi_GetBoolean(":Creature Settings:Sea Serpent", "SEEK_WEAPONS=", true);
	creatureInfo[kSerpent].magicResist				      = (float)fi_GetData(":Creature Settings:Sea Serpent", "MAGIC_RESIST=", 40)/100.0;
	creatureInfo[kSerpent].tamingLevel							= fi_GetData(":Creature Settings:Sea Serpent", "TAMING_LEVEL=", 8);
	creatureInfo[kSerpent].creatureLevel							= fi_GetData(":Creature Settings:Sea Serpent", "LEVEL=", 1);
	creatureInfo[kSerpent].armorRating							= fi_GetData(":Creature Settings:Sea Serpent", "ARMOR_RATING=", 65);
	creatureInfo[kSerpent].immuneToPoison           = fi_GetBoolean(":Creature Settings:Sea Serpent", "POISON_IMMUNITY=", true);
	creatureInfo[kSerpent].lockStrength							= fi_GetData(":Creature Settings:Sea Serpent", "LOCK=", 0);
	creatureInfo[kSerpent].attackDamageMin					= fi_GetData(":Creature Settings:Sea Serpent", "MIN_DAMAGE=", 40);
	creatureInfo[kSerpent].attackDamageMax					= fi_GetData(":Creature Settings:Sea Serpent", "MAX_DAMAGE=", 90);
	creatureInfo[kSerpent].trackingLevel							= fi_GetData(":Creature Settings:Sea Serpent", "TRACKING_LEVEL=", 101);
	creatureInfo[kSerpent].size									= fi_GetData(":Creature Settings:Sea Serpent", "SIZE=", 2);
	creatureInfo[kSerpent].mageryLevel					= fi_GetData(":Creature Settings:Sea Serpent", "MAGERY_LEVEL=", 0);
	creatureInfo[kSerpent].spellcastChance				= fi_GetData(":Creature Settings:Sea Serpent", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSerpent].selfhealChance				= fi_GetData(":Creature Settings:Sea Serpent", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSerpent].selfhealTrigger				= fi_GetData(":Creature Settings:Sea Serpent", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Sea Serpent", kSerpent);
	/*
	cr_AddToCreatureControlInventory(kSerpent, kRawSteak, 6, 10);
	cr_AddToCreatureControlInventory(kSerpent, kGreenCrystal, 0, 2);
	cr_AddToCreatureControlInventory(kSerpent, kRedCrystal, 0, 2);
	cr_AddToCreatureControlInventory(kSerpent, kBlueCrystal, 0, 2);
	cr_AddToCreatureControlInventory(kSerpent, kPurpleCrystal, 0, 2);
	cr_AddToCreatureControlInventory(kSerpent, kOrangeCrystal, 0, 2);
	cr_AddToCreatureControlInventory(kSerpent, kYellowCrystal, 0, 2);
	cr_AddToCreatureControlInventory(kSerpent, kFireCrystal, 0, 2);
	cr_AddToCreatureControlInventory(kSerpent, kBlackCrystal, 0, 2);
	*/
	fi_GetCreatureTalkScript(":Creature Settings:Sea Serpent", kSerpent);
	fi_GetCreatureInventory(":Creature Settings:Sea Serpent", kSerpent);
	fi_GetWeaponModifier(":Creature Settings:Sea Serpent", kSerpent);

	creatureInfo[kSkeleton].bmp										=	0;
	creatureInfo[kSkeleton].domain								=	kLand;
	creatureInfo[kSkeleton].terrain								=	kAnyTerrain;
	creatureInfo[kSkeleton].alignment							= kEvil;
	creatureInfo[kSkeleton].directionalGraphics		= true;
	creatureInfo[kSkeleton].graphicsIndex					= 1;
	creatureInfo[kSkeleton].corpseID							= 2;
	creatureInfo[kSkeleton].desiredPopulation			= 550;
	creatureInfo[kSkeleton].points			          = 2;
	creatureInfo[kSkeleton].undead			          = true;
	creatureInfo[kSkeleton].rareLevel						  = 10;
	creatureInfo[kSkeleton].rareChance					  = 5;
	creatureInfo[kSkeleton].metal					        = true;
	creatureInfo[kSkeleton].canMorph					    = true;
	creatureInfo[kSkeleton].bleeds					    	= false;
	creatureInfo[kSkeleton].speed										=	fi_GetData(":Creature Settings:Skeleton", "SPEED=", 75);
	creatureInfo[kSkeleton].poisonous					      = fi_GetData(":Creature Settings:Skeleton", "POISONOUS=", 0);
	creatureInfo[kSkeleton].curse					      		= fi_GetData(":Creature Settings:Skeleton", "CURSE=", 0);
	creatureInfo[kSkeleton].maxHitPoints						= fi_GetData(":Creature Settings:Skeleton", "HIT_POINTS=", 75);
	creatureInfo[kSkeleton].maxStrength							= fi_GetData(":Creature Settings:Skeleton", "STRENGTH=", 75);
	creatureInfo[kSkeleton].maxIntelligence					= fi_GetData(":Creature Settings:Skeleton", "INTELLIGENCE=", 60);
	creatureInfo[kSkeleton].maxDexterity						= fi_GetData(":Creature Settings:Skeleton", "DEXTERITY=", 50);
	creatureInfo[kSkeleton].minGold								  = fi_GetData(":Creature Settings:Skeleton", "MIN_GOLD=", 10);
	creatureInfo[kSkeleton].maxGold								  = fi_GetData(":Creature Settings:Skeleton", "MAX_GOLD=", 30);
	creatureInfo[kSkeleton].seekGold								= fi_GetBoolean(":Creature Settings:Skeleton", "SEEK_GOLD=", true);
	creatureInfo[kSkeleton].seekMagic								= fi_GetBoolean(":Creature Settings:Skeleton", "SEEK_MAGIC=", false);
	creatureInfo[kSkeleton].seekWeapons							= fi_GetBoolean(":Creature Settings:Skeleton", "SEEK_WEAPONS=", true);
	creatureInfo[kSkeleton].magicResist				      = (float)fi_GetData(":Creature Settings:Skeleton", "MAGIC_RESIST=", 70)/100.0;
	creatureInfo[kSkeleton].tamingLevel							= fi_GetData(":Creature Settings:Skeleton", "TAMING_LEVEL=", 101);
	creatureInfo[kSkeleton].creatureLevel							= fi_GetData(":Creature Settings:Skeleton", "LEVEL=", 1);
	creatureInfo[kSkeleton].armorRating							= fi_GetData(":Creature Settings:Skeleton", "ARMOR_RATING=", 30);
	creatureInfo[kSkeleton].immuneToPoison          = fi_GetBoolean(":Creature Settings:Skeleton", "POISON_IMMUNITY=", true);
	creatureInfo[kSkeleton].lockStrength						= fi_GetData(":Creature Settings:Skeleton", "LOCK=", 2);
	creatureInfo[kSkeleton].attackDamageMin					= fi_GetData(":Creature Settings:Skeleton", "MIN_DAMAGE=", 4);
	creatureInfo[kSkeleton].attackDamageMax					= fi_GetData(":Creature Settings:Skeleton", "MAX_DAMAGE=", 11);
	creatureInfo[kSkeleton].trackingLevel							= fi_GetData(":Creature Settings:Skeleton", "TRACKING_LEVEL=", 101);
	creatureInfo[kSkeleton].size									= fi_GetData(":Creature Settings:Skeleton", "SIZE=", 2);
	creatureInfo[kSkeleton].mageryLevel					= fi_GetData(":Creature Settings:Skeleton", "MAGERY_LEVEL=", 0);
	creatureInfo[kSkeleton].spellcastChance				= fi_GetData(":Creature Settings:Skeleton", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSkeleton].selfhealChance				= fi_GetData(":Creature Settings:Skeleton", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSkeleton].selfhealTrigger				= fi_GetData(":Creature Settings:Skeleton", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Skeleton", kSkeleton);
	//cr_AddToCreatureControlInventory(kSkeleton, kSkull, 1, 1);
	fi_GetCreatureTalkScript(":Creature Settings:Skeleton", kSkeleton);
	fi_GetCreatureInventory(":Creature Settings:Skeleton", kSkeleton);
	fi_GetWeaponModifier(":Creature Settings:Skeleton", kSkeleton);
	
	creatureInfo[kSkeletonArcher].bmp										=	5;
	creatureInfo[kSkeletonArcher].domain								=	kLand;
	creatureInfo[kSkeletonArcher].terrain								=	kAnyTerrain;
	creatureInfo[kSkeletonArcher].alignment							= kEvil;
	creatureInfo[kSkeletonArcher].directionalGraphics		= true;
	creatureInfo[kSkeletonArcher].graphicsIndex					= 1;
	creatureInfo[kSkeletonArcher].corpseID							= 2;
	creatureInfo[kSkeletonArcher].desiredPopulation			= 550;
	creatureInfo[kSkeletonArcher].points			          = 2;
	creatureInfo[kSkeletonArcher].undead			          = true;
	creatureInfo[kSkeletonArcher].rareLevel						  = 10;
	creatureInfo[kSkeletonArcher].rareChance					  = 5;
	creatureInfo[kSkeletonArcher].attackSound					= kArrowSnd;
	creatureInfo[kSkeletonArcher].attackRange						= 8;
	creatureInfo[kSkeletonArcher].canMorph					    = true;
	creatureInfo[kSkeletonArcher].bleeds					    	= false;
	creatureInfo[kSkeletonArcher].speed										=	fi_GetData(":Creature Settings:Skeleton Archer", "SPEED=", 75);
	creatureInfo[kSkeletonArcher].poisonous					      = fi_GetData(":Creature Settings:Skeleton Archer", "POISONOUS=", 0);
	creatureInfo[kSkeletonArcher].curse					      		= fi_GetData(":Creature Settings:Skeleton Archer", "CURSE=", 0);
	creatureInfo[kSkeletonArcher].maxHitPoints						= fi_GetData(":Creature Settings:Skeleton Archer", "HIT_POINTS=", 75);
	creatureInfo[kSkeletonArcher].maxStrength							= fi_GetData(":Creature Settings:Skeleton Archer", "STRENGTH=", 75);
	creatureInfo[kSkeletonArcher].maxIntelligence					= fi_GetData(":Creature Settings:Skeleton Archer", "INTELLIGENCE=", 60);
	creatureInfo[kSkeletonArcher].maxDexterity						= fi_GetData(":Creature Settings:Skeleton Archer", "DEXTERITY=", 50);
	creatureInfo[kSkeletonArcher].minGold								  = fi_GetData(":Creature Settings:Skeleton Archer", "MIN_GOLD=", 10);
	creatureInfo[kSkeletonArcher].maxGold								  = fi_GetData(":Creature Settings:Skeleton Archer", "MAX_GOLD=", 30);
	creatureInfo[kSkeletonArcher].seekGold								= fi_GetBoolean(":Creature Settings:Skeleton Archer", "SEEK_GOLD=", true);
	creatureInfo[kSkeletonArcher].seekMagic								= fi_GetBoolean(":Creature Settings:Skeleton Archer", "SEEK_MAGIC=", false);
	creatureInfo[kSkeletonArcher].seekWeapons							= fi_GetBoolean(":Creature Settings:Skeleton Archer", "SEEK_WEAPONS=", true);
	creatureInfo[kSkeletonArcher].magicResist				      = (float)fi_GetData(":Creature Settings:Skeleton Archer", "MAGIC_RESIST=", 70)/100.0;
	creatureInfo[kSkeletonArcher].tamingLevel							= fi_GetData(":Creature Settings:Skeleton Archer", "TAMING_LEVEL=", 101);
	creatureInfo[kSkeletonArcher].creatureLevel							= fi_GetData(":Creature Settings:Skeleton Archer", "LEVEL=", 1);
	creatureInfo[kSkeletonArcher].armorRating							= fi_GetData(":Creature Settings:Skeleton Archer", "ARMOR_RATING=", 30);
	creatureInfo[kSkeletonArcher].immuneToPoison          = fi_GetBoolean(":Creature Settings:Skeleton Archer", "POISON_IMMUNITY=", true);
	creatureInfo[kSkeletonArcher].lockStrength						= fi_GetData(":Creature Settings:Skeleton Archer", "LOCK=", 2);
	creatureInfo[kSkeletonArcher].attackDamageMin					= fi_GetData(":Creature Settings:Skeleton Archer", "MIN_DAMAGE=", 4);
	creatureInfo[kSkeletonArcher].attackDamageMax					= fi_GetData(":Creature Settings:Skeleton Archer", "MAX_DAMAGE=", 11);
	creatureInfo[kSkeletonArcher].trackingLevel							= fi_GetData(":Creature Settings:Skeleton Archer", "TRACKING_LEVEL=", 101);
	creatureInfo[kSkeletonArcher].size									= fi_GetData(":Creature Settings:Skeleton Archer", "SIZE=", 2);
	creatureInfo[kSkeletonArcher].mageryLevel					= fi_GetData(":Creature Settings:Skeleton Archer", "MAGERY_LEVEL=", 0);
	creatureInfo[kSkeletonArcher].spellcastChance				= fi_GetData(":Creature Settings:Skeleton Archer", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSkeletonArcher].selfhealChance				= fi_GetData(":Creature Settings:Skeleton Archer", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSkeletonArcher].selfhealTrigger				= fi_GetData(":Creature Settings:Skeleton Archer", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Skeleton Archer", kSkeletonArcher);
	//cr_AddToCreatureControlInventory(kSkeleton, kSkull, 1, 1);
	fi_GetCreatureTalkScript(":Creature Settings:Skeleton Archer", kSkeletonArcher);
	fi_GetCreatureInventory(":Creature Settings:Skeleton Archer", kSkeletonArcher);
	fi_GetWeaponModifier(":Creature Settings:Skeleton Archer", kSkeletonArcher);

	creatureInfo[kLizardMan].bmp									=	0;
	creatureInfo[kLizardMan].domain								=	kLand;
	creatureInfo[kLizardMan].terrain							=	kAnyTerrain;
	creatureInfo[kLizardMan].alignment						= kEvil;
	creatureInfo[kLizardMan].directionalGraphics	= true;
	creatureInfo[kLizardMan].graphicsIndex				= 1;
	creatureInfo[kLizardMan].corpseID							= 26;
	creatureInfo[kLizardMan].desiredPopulation		= 300;
	creatureInfo[kLizardMan].points			          = 4;
	creatureInfo[kLizardMan].rareLevel						= 50;
	creatureInfo[kLizardMan].rareChance					  = 1;
	creatureInfo[kLizardMan].metal					      = true;
	creatureInfo[kLizardMan].randomSound					= kLizardSnd;
	creatureInfo[kLizardMan].attackSound					= kLizardSnd;
	creatureInfo[kLizardMan].canMorph					    = true;
	creatureInfo[kLizardMan].speed										=	fi_GetData(":Creature Settings:Lizard Man", "SPEED=", 45);
	creatureInfo[kLizardMan].poisonous					      = fi_GetData(":Creature Settings:Lizard Man", "POISONOUS=", 0);
	creatureInfo[kLizardMan].curse					      		= fi_GetData(":Creature Settings:Lizard Man", "CURSE=", 0);
	creatureInfo[kLizardMan].maxHitPoints							= fi_GetData(":Creature Settings:Lizard Man", "HIT_POINTS=", 90);
	creatureInfo[kLizardMan].maxStrength							= fi_GetData(":Creature Settings:Lizard Man", "STRENGTH=", 90);
	creatureInfo[kLizardMan].maxIntelligence					= fi_GetData(":Creature Settings:Lizard Man", "INTELLIGENCE=", 60);
	creatureInfo[kLizardMan].maxDexterity							= fi_GetData(":Creature Settings:Lizard Man", "DEXTERITY=", 50);
	creatureInfo[kLizardMan].minGold								  = fi_GetData(":Creature Settings:Lizard Man", "MIN_GOLD=", 5);
	creatureInfo[kLizardMan].maxGold								  = fi_GetData(":Creature Settings:Lizard Man", "MAX_GOLD=", 20);
	creatureInfo[kLizardMan].seekGold									= fi_GetBoolean(":Creature Settings:Lizard Man", "SEEK_GOLD=", true);
	creatureInfo[kLizardMan].seekMagic								= fi_GetBoolean(":Creature Settings:Lizard Man", "SEEK_MAGIC=", false);
	creatureInfo[kLizardMan].seekWeapons							= fi_GetBoolean(":Creature Settings:Lizard Man", "SEEK_WEAPONS=", true);
	creatureInfo[kLizardMan].magicResist				      = (float)fi_GetData(":Creature Settings:Lizard Man", "MAGIC_RESIST=", 75)/100.0;
	creatureInfo[kLizardMan].tamingLevel							= fi_GetData(":Creature Settings:Lizard Man", "TAMING_LEVEL=", 101);
	creatureInfo[kLizardMan].creatureLevel							= fi_GetData(":Creature Settings:Lizard Man", "LEVEL=", 1);
	creatureInfo[kLizardMan].armorRating							= fi_GetData(":Creature Settings:Lizard Man", "ARMOR_RATING=", 40);
	creatureInfo[kLizardMan].immuneToPoison           = fi_GetBoolean(":Creature Settings:Lizard Man", "POISON_IMMUNITY=", false);
	creatureInfo[kLizardMan].lockStrength							= fi_GetData(":Creature Settings:Lizard Man", "LOCK=", 0);
	creatureInfo[kLizardMan].attackDamageMin					= fi_GetData(":Creature Settings:Lizard Man", "MIN_DAMAGE=", 5);
	creatureInfo[kLizardMan].attackDamageMax					= fi_GetData(":Creature Settings:Lizard Man", "MAX_DAMAGE=", 11);
	creatureInfo[kLizardMan].trackingLevel							= fi_GetData(":Creature Settings:Lizard Man", "TRACKING_LEVEL=", 101);
	creatureInfo[kLizardMan].size									= fi_GetData(":Creature Settings:Lizard Man", "SIZE=", 2);
	creatureInfo[kLizardMan].mageryLevel					= fi_GetData(":Creature Settings:Lizard Man", "MAGERY_LEVEL=", 0);
	creatureInfo[kLizardMan].spellcastChance				= fi_GetData(":Creature Settings:Lizard Man", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kLizardMan].selfhealChance				= fi_GetData(":Creature Settings:Lizard Man", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kLizardMan].selfhealTrigger				= fi_GetData(":Creature Settings:Lizard Man", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Lizard Man", kLizardMan);
	//cr_AddToCreatureControlInventory(kLizardMan, kLeatherHide, 2, 3);
	//cr_AddToCreatureControlInventory(kLizardMan, kRawSteak, 2, 5);
	fi_GetCreatureTalkScript(":Creature Settings:Lizard Man", kLizardMan);
	fi_GetCreatureInventory(":Creature Settings:Lizard Man", kLizardMan);
	fi_GetWeaponModifier(":Creature Settings:Lizard Man", kLizardMan);

	creatureInfo[kBoneMage].bmp										=	0;
	creatureInfo[kBoneMage].domain								=	kLand;
	creatureInfo[kBoneMage].terrain								=	kAnyTerrain;
	creatureInfo[kBoneMage].alignment							= kEvil;
	creatureInfo[kBoneMage].directionalGraphics		= true;
	creatureInfo[kBoneMage].graphicsIndex					= 1;
	creatureInfo[kBoneMage].corpseID							= 21;
	creatureInfo[kBoneMage].attackRange						= 4;
	creatureInfo[kBoneMage].desiredPopulation			= 150;
	creatureInfo[kBoneMage].points			          = 3;
	creatureInfo[kBoneMage].undead			          = true;
	creatureInfo[kBoneMage].rareLevel						  = 10;
	creatureInfo[kBoneMage].rareChance					  = 20;
	creatureInfo[kBoneMage].canMorph					    = true;
	creatureInfo[kBoneMage].bleeds					    	= false;
	creatureInfo[kBoneMage].speed										=	fi_GetData(":Creature Settings:Bone Mage", "SPEED=", 70);
	creatureInfo[kBoneMage].maxHitPoints						= fi_GetData(":Creature Settings:Bone Mage", "HIT_POINTS=", 75);
	creatureInfo[kBoneMage].maxStrength							= fi_GetData(":Creature Settings:Bone Mage", "STRENGTH=", 75);
	creatureInfo[kBoneMage].maxIntelligence					= fi_GetData(":Creature Settings:Bone Mage", "INTELLIGENCE=", 60);
	creatureInfo[kBoneMage].maxDexterity						= fi_GetData(":Creature Settings:Bone Mage", "DEXTERITY=", 50);
	creatureInfo[kBoneMage].maxMagicPoints					= fi_GetData(":Creature Settings:Bone Mage", "MAGIC_POINTS=", 70);
	creatureInfo[kBoneMage].minGold								  = fi_GetData(":Creature Settings:Bone Mage", "MIN_GOLD=", 15);
	creatureInfo[kBoneMage].maxGold								  = fi_GetData(":Creature Settings:Bone Mage", "MAX_GOLD=", 50);
	creatureInfo[kBoneMage].seekGold								= fi_GetBoolean(":Creature Settings:Bone Mage", "SEEK_GOLD=", true);
	creatureInfo[kBoneMage].seekMagic								= fi_GetBoolean(":Creature Settings:Bone Mage", "SEEK_MAGIC=", true);
	creatureInfo[kBoneMage].seekWeapons							= fi_GetBoolean(":Creature Settings:Bone Mage", "SEEK_WEAPONS=", false);
	creatureInfo[kBoneMage].magicResist				      = (float)fi_GetData(":Creature Settings:Bone Mage", "MAGIC_RESIST=", 30)/100.0;
	creatureInfo[kBoneMage].tamingLevel							= fi_GetData(":Creature Settings:Bone Mage", "TAMING_LEVEL=", 101);
	creatureInfo[kBoneMage].creatureLevel							= fi_GetData(":Creature Settings:Bone Mage", "LEVEL=", 1);
	creatureInfo[kBoneMage].armorRating							= fi_GetData(":Creature Settings:Bone Mage", "ARMOR_RATING=", 30);
	creatureInfo[kBoneMage].immuneToPoison          = fi_GetBoolean(":Creature Settings:Bone Mage", "POISON_IMMUNITY=", true);
	creatureInfo[kBoneMage].lockStrength						= fi_GetData(":Creature Settings:Bone Mage", "LOCK=", 3);
	creatureInfo[kBoneMage].trackingLevel							= fi_GetData(":Creature Settings:Bone Mage", "TRACKING_LEVEL=", 101);
	creatureInfo[kBoneMage].size									= fi_GetData(":Creature Settings:Bone Mage", "SIZE=", 2);
	creatureInfo[kBoneMage].mageryLevel					= fi_GetData(":Creature Settings:Bone Mage", "MAGERY_LEVEL=", 10);
	creatureInfo[kBoneMage].spellcastChance				= fi_GetData(":Creature Settings:Bone Mage", "SPELLCAST_CHANCE=", 100);
	creatureInfo[kBoneMage].selfhealChance				= fi_GetData(":Creature Settings:Bone Mage", "SELFHEAL_CHANCE=", 33);
	creatureInfo[kBoneMage].selfhealTrigger				= fi_GetData(":Creature Settings:Bone Mage", "SELFHEAL_TRIGGER=", 20);
	fi_GetSpellList(":Creature Settings:Bone Mage", kBoneMage);
	fi_GetCreatureTalkScript(":Creature Settings:Bone Mage", kBoneMage);
	fi_GetCreatureInventory(":Creature Settings:Bone Mage", kBoneMage);
	fi_GetWeaponModifier(":Creature Settings:Bone Mage", kBoneMage);
	
	creatureInfo[kLich].bmp								=	4;
	creatureInfo[kLich].domain								=	kLand;
	creatureInfo[kLich].terrain							=	kAnyTerrain;
	creatureInfo[kLich].alignment						= kEvil;
	creatureInfo[kLich].directionalGraphics	= true;
	creatureInfo[kLich].graphicsIndex				= 1;
	creatureInfo[kLich].randomSound							= kLichLaughSnd;
	creatureInfo[kLich].corpseID							= 137;
	creatureInfo[kLich].attackRange					= 1;
	creatureInfo[kLich].desiredPopulation		= 5;
	creatureInfo[kLich].meatEater						= true;
	creatureInfo[kLich].points						    = 10;
	creatureInfo[kLich].rareLevel						= 1;
	creatureInfo[kLich].rareChance						= 100;
	creatureInfo[kLich].canMorph					    = true;
	creatureInfo[kLich].undead							= true;
	creatureInfo[kLich].speed										=	fi_GetData(":Creature Settings:Lich", "SPEED=", 50);
	creatureInfo[kLich].poisonous					      = fi_GetData(":Creature Settings:Lich", "POISONOUS=", 0);
	creatureInfo[kLich].curse					      		= fi_GetData(":Creature Settings:Lich", "CURSE=", 0);
	creatureInfo[kLich].maxHitPoints							= fi_GetData(":Creature Settings:Lich", "HIT_POINTS=", 200);
	creatureInfo[kLich].maxStrength							= fi_GetData(":Creature Settings:Lich", "STRENGTH=", 200);
	creatureInfo[kLich].maxIntelligence					= fi_GetData(":Creature Settings:Lich", "INTELLIGENCE=", 200);
	creatureInfo[kLich].maxDexterity							= fi_GetData(":Creature Settings:Lich", "DEXTERITY=", 200);
	creatureInfo[kLich].minGold								  = fi_GetData(":Creature Settings:Lich", "MIN_GOLD=", 200);
	creatureInfo[kLich].maxGold								  = fi_GetData(":Creature Settings:Lich", "MAX_GOLD=", 500);
	creatureInfo[kLich].seekGold									= fi_GetBoolean(":Creature Settings:Lich", "SEEK_GOLD=", true);
	creatureInfo[kLich].seekMagic								= fi_GetBoolean(":Creature Settings:Lich", "SEEK_MAGIC=", false);
	creatureInfo[kLich].seekWeapons							= fi_GetBoolean(":Creature Settings:Lich", "SEEK_WEAPONS=", true);
	creatureInfo[kLich].magicResist				      = (float)fi_GetData(":Creature Settings:Lich", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kLich].tamingLevel							= fi_GetData(":Creature Settings:Lich", "TAMING_LEVEL=", 8);
	creatureInfo[kLich].creatureLevel							= fi_GetData(":Creature Settings:Lich", "LEVEL=", 1);
	creatureInfo[kLich].armorRating							= fi_GetData(":Creature Settings:Lich", "ARMOR_RATING=", 75);
	creatureInfo[kLich].immuneToPoison           = fi_GetBoolean(":Creature Settings:Lich", "POISON_IMMUNITY=", false);
	creatureInfo[kLich].lockStrength							= fi_GetData(":Creature Settings:Lich", "LOCK=", 5);
	creatureInfo[kLich].attackDamageMin					= fi_GetData(":Creature Settings:Lich", "MIN_DAMAGE=", 60);
	creatureInfo[kLich].attackDamageMax					= fi_GetData(":Creature Settings:Lich", "MAX_DAMAGE=", 150);
	creatureInfo[kLich].trackingLevel							= fi_GetData(":Creature Settings:Lich", "TRACKING_LEVEL=", 101);
	creatureInfo[kLich].size									= fi_GetData(":Creature Settings:Lich", "SIZE=", 3);
	creatureInfo[kLich].mageryLevel					= fi_GetData(":Creature Settings:Lich", "MAGERY_LEVEL=", 15);
	creatureInfo[kLich].spellcastChance				= fi_GetData(":Creature Settings:Lich", "SPELLCAST_CHANCE=", 50);
	creatureInfo[kLich].selfhealChance				= fi_GetData(":Creature Settings:Lich", "SELFHEAL_CHANCE=", 40);
	creatureInfo[kLich].selfhealTrigger				= fi_GetData(":Creature Settings:Lich", "SELFHEAL_TRIGGER=", 40);
	fi_GetSpellList(":Creature Settings:Lich", kLich);
	fi_GetCreatureTalkScript(":Creature Settings:Lich", kLich);
	fi_GetCreatureInventory(":Creature Settings:Lich", kLich);
	fi_GetWeaponModifier(":Creature Settings:Lich", kLich);

	creatureInfo[kHellHound].bmp									=	0;
	creatureInfo[kHellHound].domain								=	kLand;
	creatureInfo[kHellHound].terrain							=	kAnyTerrain;
	creatureInfo[kHellHound].alignment						= kEvil;
	creatureInfo[kHellHound].directionalGraphics	= true;
	creatureInfo[kHellHound].graphicsIndex				= 1;
	creatureInfo[kHellHound].corpseID							= 4;
	creatureInfo[kHellHound].attackSound					= kWolfAttack;
	creatureInfo[kHellHound].desiredPopulation		= 300;
	creatureInfo[kHellHound].meatEater						= true;
	creatureInfo[kHellHound].points						    = 4;
	creatureInfo[kHellHound].canMorph					    = true;
	creatureInfo[kHellHound].fire								  = true;
	creatureInfo[kHellHound].speed										=	fi_GetData(":Creature Settings:Hell Hound", "SPEED=", 65);
	creatureInfo[kHellHound].poisonous					      = fi_GetData(":Creature Settings:Hell Hound", "POISONOUS=", 0);
	creatureInfo[kHellHound].curse					      		= fi_GetData(":Creature Settings:Hell Hound", "CURSE=", 0);
	creatureInfo[kHellHound].maxHitPoints							= fi_GetData(":Creature Settings:Hell Hound", "HIT_POINTS=", 100);
	creatureInfo[kHellHound].maxStrength							= fi_GetData(":Creature Settings:Hell Hound", "STRENGTH=", 100);
	creatureInfo[kHellHound].maxIntelligence					= fi_GetData(":Creature Settings:Hell Hound", "INTELLIGENCE=", 25);
	creatureInfo[kHellHound].maxDexterity							= fi_GetData(":Creature Settings:Hell Hound", "DEXTERITY=", 75);
	creatureInfo[kHellHound].minGold								  = fi_GetData(":Creature Settings:Hell Hound", "MIN_GOLD=", 30);
	creatureInfo[kHellHound].maxGold								  = fi_GetData(":Creature Settings:Hell Hound", "MAX_GOLD=", 45);
	creatureInfo[kHellHound].seekGold									= fi_GetBoolean(":Creature Settings:Hell Hound", "SEEK_GOLD=", false);
	creatureInfo[kHellHound].seekMagic								= fi_GetBoolean(":Creature Settings:Hell Hound", "SEEK_MAGIC=", false);
	creatureInfo[kHellHound].seekWeapons							= fi_GetBoolean(":Creature Settings:Hell Hound", "SEEK_WEAPONS=", false);
	creatureInfo[kHellHound].magicResist				      = (float)fi_GetData(":Creature Settings:Hell Hound", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kHellHound].tamingLevel							= fi_GetData(":Creature Settings:Hell Hound", "TAMING_LEVEL=", 7);
	creatureInfo[kHellHound].creatureLevel							= fi_GetData(":Creature Settings:Hell Hound", "LEVEL=", 1);
	creatureInfo[kHellHound].armorRating							= fi_GetData(":Creature Settings:Hell Hound", "ARMOR_RATING=", 10);
	creatureInfo[kHellHound].immuneToPoison           = fi_GetBoolean(":Creature Settings:Hell Hound", "POISON_IMMUNITY=", false);
	creatureInfo[kHellHound].lockStrength							= fi_GetData(":Creature Settings:Hell Hound", "LOCK=", 0);
	creatureInfo[kHellHound].attackDamageMin					= fi_GetData(":Creature Settings:Hell Hound", "MIN_DAMAGE=", 7);
	creatureInfo[kHellHound].attackDamageMax					= fi_GetData(":Creature Settings:Hell Hound", "MAX_DAMAGE=", 13);
	creatureInfo[kHellHound].trackingLevel							= fi_GetData(":Creature Settings:Hell Hound", "TRACKING_LEVEL=", 101);
	creatureInfo[kHellHound].size									= fi_GetData(":Creature Settings:Hell Hound", "SIZE=", 2);
	creatureInfo[kHellHound].mageryLevel					= fi_GetData(":Creature Settings:Hell Hound", "MAGERY_LEVEL=", 0);
	creatureInfo[kHellHound].spellcastChance				= fi_GetData(":Creature Settings:Hell Hound", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kHellHound].selfhealChance				= fi_GetData(":Creature Settings:Hell Hound", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kHellHound].selfhealTrigger				= fi_GetData(":Creature Settings:Hell Hound", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Hell Hound", kHellHound);
	fi_GetCreatureTalkScript(":Creature Settings:Hell Hound", kHellHound);
	fi_GetCreatureInventory(":Creature Settings:Hell Hound", kHellHound);
	fi_GetWeaponModifier(":Creature Settings:Hell Hound", kHellHound);
//	cr_AddToCreatureControlInventory(kHellHound, kFurSkin, 4, 6);
//	cr_AddToCreatureControlInventory(kHellHound, kRawSteak, 4, 6);

	creatureInfo[kBlinkHound].bmp									=	3;
	creatureInfo[kBlinkHound].domain								=	kLand;
	creatureInfo[kBlinkHound].terrain							=	kAnyTerrain;
	creatureInfo[kBlinkHound].alignment						= kEvil;
	creatureInfo[kBlinkHound].directionalGraphics	= true;
	creatureInfo[kBlinkHound].graphicsIndex				= 1;
	creatureInfo[kBlinkHound].corpseID							= 118;
	creatureInfo[kBlinkHound].attackSound					= kBlinkHoundAttack;
	creatureInfo[kBlinkHound].desiredPopulation		= 60;
	creatureInfo[kBlinkHound].meatEater						= true;
	creatureInfo[kBlinkHound].points						    = 4;
	creatureInfo[kBlinkHound].canMorph					    = true;
	creatureInfo[kBlinkHound].fire								  = true;
	creatureInfo[kBlinkHound].speed										=	fi_GetData(":Creature Settings:Blink Hound", "SPEED=", 65);
	creatureInfo[kBlinkHound].poisonous					      = fi_GetData(":Creature Settings:Blink Hound", "POISONOUS=", 0);
	creatureInfo[kBlinkHound].curse					      		= fi_GetData(":Creature Settings:Blink Hound", "CURSE=", 0);
	creatureInfo[kBlinkHound].maxHitPoints							= fi_GetData(":Creature Settings:Blink Hound", "HIT_POINTS=", 100);
	creatureInfo[kBlinkHound].maxStrength							= fi_GetData(":Creature Settings:Blink Hound", "STRENGTH=", 100);
	creatureInfo[kBlinkHound].maxIntelligence					= fi_GetData(":Creature Settings:Blink Hound", "INTELLIGENCE=", 25);
	creatureInfo[kBlinkHound].maxDexterity							= fi_GetData(":Creature Settings:Blink Hound", "DEXTERITY=", 75);
	creatureInfo[kBlinkHound].minGold								  = fi_GetData(":Creature Settings:Blink Hound", "MIN_GOLD=", 30);
	creatureInfo[kBlinkHound].maxGold								  = fi_GetData(":Creature Settings:Blink Hound", "MAX_GOLD=", 45);
	creatureInfo[kBlinkHound].seekGold									= fi_GetBoolean(":Creature Settings:Blink Hound", "SEEK_GOLD=", false);
	creatureInfo[kBlinkHound].seekMagic								= fi_GetBoolean(":Creature Settings:Blink Hound", "SEEK_MAGIC=", false);
	creatureInfo[kBlinkHound].seekWeapons							= fi_GetBoolean(":Creature Settings:Blink Hound", "SEEK_WEAPONS=", false);
	creatureInfo[kBlinkHound].magicResist				      = (float)fi_GetData(":Creature Settings:Blink Hound", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kBlinkHound].tamingLevel							= fi_GetData(":Creature Settings:Blink Hound", "TAMING_LEVEL=", 7);
	creatureInfo[kBlinkHound].creatureLevel							= fi_GetData(":Creature Settings:Blink Hound", "LEVEL=", 1);
	creatureInfo[kBlinkHound].armorRating							= fi_GetData(":Creature Settings:Blink Hound", "ARMOR_RATING=", 10);
	creatureInfo[kBlinkHound].immuneToPoison           = fi_GetBoolean(":Creature Settings:Blink Hound", "POISON_IMMUNITY=", false);
	creatureInfo[kBlinkHound].lockStrength							= fi_GetData(":Creature Settings:Blink Hound", "LOCK=", 0);
	creatureInfo[kBlinkHound].attackDamageMin					= fi_GetData(":Creature Settings:Blink Hound", "MIN_DAMAGE=", 7);
	creatureInfo[kBlinkHound].attackDamageMax					= fi_GetData(":Creature Settings:Blink Hound", "MAX_DAMAGE=", 13);
	creatureInfo[kBlinkHound].trackingLevel							= fi_GetData(":Creature Settings:Blink Hound", "TRACKING_LEVEL=", 101);
	creatureInfo[kBlinkHound].size									= fi_GetData(":Creature Settings:Blink Hound", "SIZE=", 2);
	creatureInfo[kBlinkHound].mageryLevel					= fi_GetData(":Creature Settings:Blink Hound", "MAGERY_LEVEL=", 0);
	creatureInfo[kBlinkHound].spellcastChance				= fi_GetData(":Creature Settings:Blink Hound", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kBlinkHound].selfhealChance				= fi_GetData(":Creature Settings:Blink Hound", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBlinkHound].selfhealTrigger				= fi_GetData(":Creature Settings:Blink Hound", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Blink Hound", kBlinkHound);
	fi_GetCreatureTalkScript(":Creature Settings:Blink Hound", kBlinkHound);
	fi_GetCreatureInventory(":Creature Settings:Blink Hound", kBlinkHound);
	fi_GetWeaponModifier(":Creature Settings:Blink Hound", kBlinkHound);
	
	creatureInfo[kDwarf].bmp								=	3;
	creatureInfo[kDwarf].domain							=	kLand;
	creatureInfo[kDwarf].terrain							=	kAnyTerrain;
	creatureInfo[kDwarf].alignment							= kGood;
	creatureInfo[kDwarf].directionalGraphics				= true;
	creatureInfo[kDwarf].graphicsIndex						= 1;
	creatureInfo[kDwarf].corpseID							= 228;
	creatureInfo[kDwarf].desiredPopulation					= 60;
	creatureInfo[kDwarf].meatEater							= true;
	creatureInfo[kDwarf].points						    = 4;
	creatureInfo[kDwarf].canMorph					    	= true;
	creatureInfo[kDwarf].fire								= true;
	creatureInfo[kDwarf].speed								=	fi_GetData(":Creature Settings:Dwarf", "SPEED=", 65);
	creatureInfo[kDwarf].poisonous					      	= fi_GetData(":Creature Settings:Dwarf", "POISONOUS=", 0);
	creatureInfo[kDwarf].curse					      		= fi_GetData(":Creature Settings:Dwarf", "CURSE=", 0);
	creatureInfo[kDwarf].maxHitPoints						= fi_GetData(":Creature Settings:Dwarf", "HIT_POINTS=", 100);
	creatureInfo[kDwarf].maxStrength						= fi_GetData(":Creature Settings:Dwarf", "STRENGTH=", 100);
	creatureInfo[kDwarf].maxIntelligence					= fi_GetData(":Creature Settings:Dwarf", "INTELLIGENCE=", 25);
	creatureInfo[kDwarf].maxDexterity						= fi_GetData(":Creature Settings:Dwarf", "DEXTERITY=", 75);
	creatureInfo[kDwarf].minGold							= fi_GetData(":Creature Settings:Dwarf", "MIN_GOLD=", 30);
	creatureInfo[kDwarf].maxGold							= fi_GetData(":Creature Settings:Dwarf", "MAX_GOLD=", 45);
	creatureInfo[kDwarf].seekGold							= fi_GetBoolean(":Creature Settings:Dwarf", "SEEK_GOLD=", false);
	creatureInfo[kDwarf].seekMagic							= fi_GetBoolean(":Creature Settings:Dwarf", "SEEK_MAGIC=", false);
	creatureInfo[kDwarf].seekWeapons						= fi_GetBoolean(":Creature Settings:Dwarf", "SEEK_WEAPONS=", false);
	creatureInfo[kDwarf].magicResist				      	= (float)fi_GetData(":Creature Settings:Dwarf", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kDwarf].tamingLevel						= fi_GetData(":Creature Settings:Dwarf Hound", "TAMING_LEVEL=", 7);
	creatureInfo[kDwarf].creatureLevel						= fi_GetData(":Creature Settings:Dwarf Hound", "LEVEL=", 1);
	creatureInfo[kDwarf].armorRating						= fi_GetData(":Creature Settings:Dwarf Hound", "ARMOR_RATING=", 10);
	creatureInfo[kDwarf].immuneToPoison           			= fi_GetBoolean(":Creature Settings:Dwarf Hound", "POISON_IMMUNITY=", false);
	creatureInfo[kDwarf].lockStrength						= fi_GetData(":Creature Settings:Dwarf Hound", "LOCK=", 0);
	creatureInfo[kDwarf].attackDamageMin					= fi_GetData(":Creature Settings:Dwarf Hound", "MIN_DAMAGE=", 7);
	creatureInfo[kDwarf].attackDamageMax					= fi_GetData(":Creature Settings:Dwarf Hound", "MAX_DAMAGE=", 13);
	creatureInfo[kDwarf].trackingLevel						= fi_GetData(":Creature Settings:Dwarf Hound", "TRACKING_LEVEL=", 101);
	creatureInfo[kDwarf].size								= fi_GetData(":Creature Settings:Dwarf Hound", "SIZE=", 2);
	creatureInfo[kDwarf].mageryLevel					= fi_GetData(":Creature Settings:Dwarf", "MAGERY_LEVEL=", 0);
	creatureInfo[kDwarf].spellcastChance				= fi_GetData(":Creature Settings:Dwarf", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kDwarf].selfhealChance					= fi_GetData(":Creature Settings:Dwarf", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kDwarf].selfhealTrigger				= fi_GetData(":Creature Settings:Dwarf", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Dwarf", kDwarf);
	fi_GetCreatureTalkScript(":Creature Settings:Dwarf", kDwarf);
	fi_GetCreatureInventory(":Creature Settings:Dwarf", kDwarf);
	fi_GetWeaponModifier(":Creature Settings:Dwarf", kDwarf);	

	creatureInfo[kGreyWolf].bmp												=	0;
	creatureInfo[kGreyWolf].domain										=	kLand;
	creatureInfo[kGreyWolf].terrain										=	kAnyTerrain;
	creatureInfo[kGreyWolf].alignment									= kNeutral;
	creatureInfo[kGreyWolf].directionalGraphics				= true;
	creatureInfo[kGreyWolf].graphicsIndex							= 1;
	creatureInfo[kGreyWolf].corpseID									= 5;
	creatureInfo[kGreyWolf].randomSound								= kWolfHowl;
	creatureInfo[kGreyWolf].attackSound								= kWolfAttack;
	creatureInfo[kGreyWolf].desiredPopulation					= 400;
	creatureInfo[kGreyWolf].meatEater									= true;
	creatureInfo[kGreyWolf].canMorph					        = true;
	creatureInfo[kGreyWolf].speed										=	fi_GetData(":Creature Settings:Grey Wolf", "SPEED=", 80);
	creatureInfo[kGreyWolf].poisonous					      = fi_GetData(":Creature Settings:Grey Wolf", "POISONOUS=", 0);
	creatureInfo[kGreyWolf].curse					      		= fi_GetData(":Creature Settings:Grey Wolf", "CURSE=", 0);
	creatureInfo[kGreyWolf].maxHitPoints						= fi_GetData(":Creature Settings:Grey Wolf", "HIT_POINTS=", 15);
	creatureInfo[kGreyWolf].maxStrength							= fi_GetData(":Creature Settings:Grey Wolf", "STRENGTH=", 15);
	creatureInfo[kGreyWolf].maxIntelligence					= fi_GetData(":Creature Settings:Grey Wolf", "INTELLIGENCE=", 25);
	creatureInfo[kGreyWolf].maxDexterity						= fi_GetData(":Creature Settings:Grey Wolf", "DEXTERITY=", 75);
	creatureInfo[kGreyWolf].minGold								  = fi_GetData(":Creature Settings:Grey Wolf", "MIN_GOLD=", 0);
	creatureInfo[kGreyWolf].maxGold								  = fi_GetData(":Creature Settings:Grey Wolf", "MAX_GOLD=", 0);
	creatureInfo[kGreyWolf].seekGold								= fi_GetBoolean(":Creature Settings:Grey Wolf", "SEEK_GOLD=", false);
	creatureInfo[kGreyWolf].seekMagic								= fi_GetBoolean(":Creature Settings:Grey Wolf", "SEEK_MAGIC=", false);
	creatureInfo[kGreyWolf].seekWeapons							= fi_GetBoolean(":Creature Settings:Grey Wolf", "SEEK_WEAPONS=", false);
	creatureInfo[kGreyWolf].magicResist				      = (float)fi_GetData(":Creature Settings:Grey Wolf", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kGreyWolf].tamingLevel							= fi_GetData(":Creature Settings:Grey Wolf", "TAMING_LEVEL=", 2);
	creatureInfo[kGreyWolf].creatureLevel							= fi_GetData(":Creature Settings:Grey Wolf", "LEVEL=", 1);
	creatureInfo[kGreyWolf].armorRating							= fi_GetData(":Creature Settings:Grey Wolf", "ARMOR_RATING=", 10);
	creatureInfo[kGreyWolf].immuneToPoison          = fi_GetBoolean(":Creature Settings:Grey Wolf", "POISON_IMMUNITY=", false);
	creatureInfo[kGreyWolf].lockStrength						= fi_GetData(":Creature Settings:Grey Wolf", "LOCK=", 0);
	creatureInfo[kGreyWolf].attackDamageMin					= fi_GetData(":Creature Settings:Grey Wolf", "MIN_DAMAGE=", 4);
	creatureInfo[kGreyWolf].attackDamageMax					= fi_GetData(":Creature Settings:Grey Wolf", "MAX_DAMAGE=", 9);
	creatureInfo[kGreyWolf].trackingLevel							= fi_GetData(":Creature Settings:Grey Wolf", "TRACKING_LEVEL=", 101);
	creatureInfo[kGreyWolf].size									= fi_GetData(":Creature Settings:Grey Wolf", "SIZE=", 2);
	creatureInfo[kGreyWolf].mageryLevel					= fi_GetData(":Creature Settings:Grey Wolf", "MAGERY_LEVEL=", 0);
	creatureInfo[kGreyWolf].spellcastChance				= fi_GetData(":Creature Settings:Grey Wolf", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kGreyWolf].selfhealChance					= fi_GetData(":Creature Settings:Grey Wolf", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kGreyWolf].selfhealTrigger				= fi_GetData(":Creature Settings:Grey Wolf:", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Grey Wolf", kGreyWolf);
	fi_GetCreatureTalkScript(":Creature Settings:Grey Wolf", kGreyWolf);
	fi_GetCreatureInventory(":Creature Settings:Grey Wolf", kGreyWolf);
	fi_GetWeaponModifier(":Creature Settings:Grey Wolf", kGreyWolf);

	creatureInfo[kBrownWolf].bmp											=	0;
	creatureInfo[kBrownWolf].domain										=	kLand;
	creatureInfo[kBrownWolf].terrain									=	kAnyTerrain;
	creatureInfo[kBrownWolf].alignment								= kNeutral;
	creatureInfo[kBrownWolf].directionalGraphics			= true;
	creatureInfo[kBrownWolf].graphicsIndex						= 1;
	creatureInfo[kBrownWolf].corpseID									= 23;
	creatureInfo[kBrownWolf].randomSound							= kWolfHowl;
	creatureInfo[kBrownWolf].attackSound							= kWolfAttack;
	creatureInfo[kBrownWolf].desiredPopulation				= 1000;
	creatureInfo[kBrownWolf].meatEater								= true;
	creatureInfo[kBrownWolf].color		        				= kBrownWolfColor;
	creatureInfo[kBrownWolf].canMorph					        = true;
	creatureInfo[kBrownWolf].speed										=	fi_GetData(":Creature Settings:Brown Wolf", "SPEED=", 80);
	creatureInfo[kBrownWolf].poisonous					      = fi_GetData(":Creature Settings:Brown Wolf", "POISONOUS=", 0);
	creatureInfo[kBrownWolf].curse					      		= fi_GetData(":Creature Settings:Brown Wolf", "CURSE=", 0);
	creatureInfo[kBrownWolf].maxHitPoints							= fi_GetData(":Creature Settings:Brown Wolf", "HIT_POINTS=", 15);
	creatureInfo[kBrownWolf].maxStrength							= fi_GetData(":Creature Settings:Brown Wolf", "STRENGTH=", 15);
	creatureInfo[kBrownWolf].maxIntelligence					= fi_GetData(":Creature Settings:Brown Wolf", "INTELLIGENCE=", 25);
	creatureInfo[kBrownWolf].maxDexterity							= fi_GetData(":Creature Settings:Brown Wolf", "DEXTERITY=", 75);
	creatureInfo[kBrownWolf].minGold								  = fi_GetData(":Creature Settings:Brown Wolf", "MIN_GOLD=", 0);
	creatureInfo[kBrownWolf].maxGold								  = fi_GetData(":Creature Settings:Brown Wolf", "MAX_GOLD=", 0);
	creatureInfo[kBrownWolf].seekGold									= fi_GetBoolean(":Creature Settings:Brown Wolf", "SEEK_GOLD=", false);
	creatureInfo[kBrownWolf].seekMagic								= fi_GetBoolean(":Creature Settings:Brown Wolf", "SEEK_MAGIC=", false);
	creatureInfo[kBrownWolf].seekWeapons							= fi_GetBoolean(":Creature Settings:Brown Wolf", "SEEK_WEAPONS=", false);
	creatureInfo[kBrownWolf].magicResist				      = (float)fi_GetData(":Creature Settings:Brown Wolf", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kBrownWolf].tamingLevel							= fi_GetData(":Creature Settings:Brown Wolf", "TAMING_LEVEL=", 2);
	creatureInfo[kBrownWolf].creatureLevel							= fi_GetData(":Creature Settings:Brown Wolf", "LEVEL=", 1);
	creatureInfo[kBrownWolf].armorRating							= fi_GetData(":Creature Settings:Brown Wolf", "ARMOR_RATING=", 10);
	creatureInfo[kBrownWolf].immuneToPoison          	= fi_GetBoolean(":Creature Settings:Brown Wolf", "POISON_IMMUNITY=", false);
	creatureInfo[kBrownWolf].lockStrength							= fi_GetData(":Creature Settings:Brown Wolf", "LOCK=", 0);
	creatureInfo[kBrownWolf].attackDamageMin					= fi_GetData(":Creature Settings:Brown Wolf", "MIN_DAMAGE=", 4);
	creatureInfo[kBrownWolf].attackDamageMax					= fi_GetData(":Creature Settings:Brown Wolf", "MAX_DAMAGE=", 9);
	creatureInfo[kBrownWolf].trackingLevel							= fi_GetData(":Creature Settings:Brown Wolf", "TRACKING_LEVEL=", 101);
	creatureInfo[kBrownWolf].size									= fi_GetData(":Creature Settings:Brown Wolf", "SIZE=", 2);
	creatureInfo[kBrownWolf].mageryLevel					= fi_GetData(":Creature Settings:Brown Wolf", "MAGERY_LEVEL=", 0);
	creatureInfo[kBrownWolf].spellcastChance				= fi_GetData(":Creature Settings:Brown Wolf", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kBrownWolf].selfhealChance					= fi_GetData(":Creature Settings:Brown Wolf", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBrownWolf].selfhealTrigger				= fi_GetData(":Creature Settings:Brown Wolf", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Brown Wolf", kBrownWolf);
	fi_GetCreatureTalkScript(":Creature Settings:Brown Wolf", kBrownWolf);
	fi_GetCreatureInventory(":Creature Settings:Brown Wolf", kBrownWolf);
	fi_GetWeaponModifier(":Creature Settings:Brown Wolf", kBrownWolf);

	creatureInfo[kBlackWolf].bmp											=	0;
	creatureInfo[kBlackWolf].domain										=	kLand;
	creatureInfo[kBlackWolf].terrain									=	kAnyTerrain;
	creatureInfo[kBlackWolf].alignment								= kNeutral;
	creatureInfo[kBlackWolf].directionalGraphics			= true;
	creatureInfo[kBlackWolf].graphicsIndex						= 1;
	creatureInfo[kBlackWolf].corpseID									= 24;
	creatureInfo[kBlackWolf].randomSound							= kWolfHowl;
	creatureInfo[kBlackWolf].attackSound							= kWolfAttack;
	creatureInfo[kBlackWolf].desiredPopulation				= 300;
	creatureInfo[kBlackWolf].meatEater								= true;
	creatureInfo[kBlackWolf].color		        				= kBlackWolfColor;
	creatureInfo[kBlackWolf].canMorph					        = true;
	creatureInfo[kBlackWolf].speed										=	fi_GetData(":Creature Settings:Black Wolf", "SPEED=", 80);
	creatureInfo[kBlackWolf].poisonous					      = fi_GetData(":Creature Settings:Black Wolf", "POISONOUS=", 0);
	creatureInfo[kBlackWolf].curse					      		= fi_GetData(":Creature Settings:Black Wolf", "CURSE=", 0);
	creatureInfo[kBlackWolf].maxHitPoints							= fi_GetData(":Creature Settings:Black Wolf", "HIT_POINTS=", 15);
	creatureInfo[kBlackWolf].maxStrength							= fi_GetData(":Creature Settings:Black Wolf", "STRENGTH=", 15);
	creatureInfo[kBlackWolf].maxIntelligence					= fi_GetData(":Creature Settings:Black Wolf", "INTELLIGENCE=", 25);
	creatureInfo[kBlackWolf].maxDexterity							= fi_GetData(":Creature Settings:Black Wolf", "DEXTERITY=", 75);
	creatureInfo[kBlackWolf].minGold								  = fi_GetData(":Creature Settings:Black Wolf", "MIN_GOLD=", 0);
	creatureInfo[kBlackWolf].maxGold								  = fi_GetData(":Creature Settings:Black Wolf", "MAX_GOLD=", 0);
	creatureInfo[kBlackWolf].seekGold									= fi_GetBoolean(":Creature Settings:Black Wolf", "SEEK_GOLD=", false);
	creatureInfo[kBlackWolf].seekMagic								= fi_GetBoolean(":Creature Settings:Black Wolf", "SEEK_MAGIC=", false);
	creatureInfo[kBlackWolf].seekWeapons							= fi_GetBoolean(":Creature Settings:Black Wolf", "SEEK_WEAPONS=", false);
	creatureInfo[kBlackWolf].magicResist				      = (float)fi_GetData(":Creature Settings:Black Wolf", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kBlackWolf].tamingLevel							= fi_GetData(":Creature Settings:Black Wolf", "TAMING_LEVEL=", 2);
	creatureInfo[kBlackWolf].creatureLevel							= fi_GetData(":Creature Settings:Black Wolf", "LEVEL=", 1);
	creatureInfo[kBlackWolf].armorRating							= fi_GetData(":Creature Settings:Black Wolf", "ARMOR_RATING=", 10);
	creatureInfo[kBlackWolf].immuneToPoison          	= fi_GetBoolean(":Creature Settings:Black Wolf", "POISON_IMMUNITY=", false);
	creatureInfo[kBlackWolf].lockStrength							= fi_GetData(":Creature Settings:Black Wolf", "LOCK=", 0);
	creatureInfo[kBlackWolf].attackDamageMin					= fi_GetData(":Creature Settings:Black Wolf", "MIN_DAMAGE=", 4);
	creatureInfo[kBlackWolf].attackDamageMax					= fi_GetData(":Creature Settings:Black Wolf", "MAX_DAMAGE=", 9);
	creatureInfo[kBlackWolf].trackingLevel							= fi_GetData(":Creature Settings:Black Wolf", "TRACKING_LEVEL=", 101);
	creatureInfo[kBlackWolf].size									= fi_GetData(":Creature Settings:Black Wolf", "SIZE=", 2);
	creatureInfo[kBlackWolf].mageryLevel					= fi_GetData(":Creature Settings:Black Wolf", "MAGERY_LEVEL=", 0);
	creatureInfo[kBlackWolf].spellcastChance				= fi_GetData(":Creature Settings:Black Wolf", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kBlackWolf].selfhealChance					= fi_GetData(":Creature Settings:Black Wolf", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBlackWolf].selfhealTrigger				= fi_GetData(":Creature Settings:Black Wolf", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Black Wolf", kBlackWolf);
	fi_GetCreatureTalkScript(":Creature Settings:Black Wolf", kBlackWolf);
	fi_GetCreatureInventory(":Creature Settings:Black Wolf", kBlackWolf);
	fi_GetWeaponModifier(":Creature Settings:Black Wolf", kBlackWolf);

	creatureInfo[kGreyDog].bmp												=	0;
	creatureInfo[kGreyDog].domain											=	kLand;
	creatureInfo[kGreyDog].terrain										=	kAnyTerrain;
	creatureInfo[kGreyDog].alignment									= kNeutral;
	creatureInfo[kGreyDog].directionalGraphics				= true;
	creatureInfo[kGreyDog].graphicsIndex							= 1;
	creatureInfo[kGreyDog].corpseID										= 5;
	creatureInfo[kGreyDog].attackSound								= kWolfAttack;
	creatureInfo[kGreyDog].desiredPopulation					= 400;
	creatureInfo[kGreyDog].meatEater									= true;
	creatureInfo[kGreyDog].canMorph					        	= true;
	creatureInfo[kGreyDog].speed											=	fi_GetData(":Creature Settings:Grey Dog", "SPEED=", 80);
	creatureInfo[kGreyDog].poisonous					      	= fi_GetData(":Creature Settings:Grey Dog", "POISONOUS=", 0);
	creatureInfo[kGreyDog].curse					      			= fi_GetData(":Creature Settings:Grey Dog", "CURSE=", 0);
	creatureInfo[kGreyDog].maxHitPoints								= fi_GetData(":Creature Settings:Grey Dog", "HIT_POINTS=", 15);
	creatureInfo[kGreyDog].maxStrength								= fi_GetData(":Creature Settings:Grey Dog", "STRENGTH=", 15);
	creatureInfo[kGreyDog].maxIntelligence						= fi_GetData(":Creature Settings:Grey Dog", "INTELLIGENCE=", 25);
	creatureInfo[kGreyDog].maxDexterity								= fi_GetData(":Creature Settings:Grey Dog", "DEXTERITY=", 75);
	creatureInfo[kGreyDog].minGold								  	= fi_GetData(":Creature Settings:Grey Dog", "MIN_GOLD=", 0);
	creatureInfo[kGreyDog].maxGold								  	= fi_GetData(":Creature Settings:Grey Dog", "MAX_GOLD=", 0);
	creatureInfo[kGreyDog].seekGold										= fi_GetBoolean(":Creature Settings:Grey Dog", "SEEK_GOLD=", false);
	creatureInfo[kGreyDog].seekMagic									= fi_GetBoolean(":Creature Settings:Grey Dog", "SEEK_MAGIC=", false);
	creatureInfo[kGreyDog].seekWeapons								= fi_GetBoolean(":Creature Settings:Grey Dog", "SEEK_WEAPONS=", false);
	creatureInfo[kGreyDog].magicResist				      	= (float)fi_GetData(":Creature Settings:Grey Dog", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kGreyDog].tamingLevel								= fi_GetData(":Creature Settings:Grey Dog", "TAMING_LEVEL=", 2);
	creatureInfo[kGreyDog].creatureLevel								= fi_GetData(":Creature Settings:Grey Dog", "LEVEL=", 1);
	creatureInfo[kGreyDog].armorRating								= fi_GetData(":Creature Settings:Grey Dog", "ARMOR_RATING=", 10);
	creatureInfo[kGreyDog].immuneToPoison          		= fi_GetBoolean(":Creature Settings:Grey Dog", "POISON_IMMUNITY=", false);
	creatureInfo[kGreyDog].lockStrength								= fi_GetData(":Creature Settings:Grey Dog", "LOCK=", 0);
	creatureInfo[kGreyDog].attackDamageMin						= fi_GetData(":Creature Settings:Grey Dog", "MIN_DAMAGE=", 4);
	creatureInfo[kGreyDog].attackDamageMax						= fi_GetData(":Creature Settings:Grey Dog", "MAX_DAMAGE=", 9);
	creatureInfo[kGreyDog].trackingLevel							= fi_GetData(":Creature Settings:Grey Dog", "TRACKING_LEVEL=", 101);
	creatureInfo[kGreyDog].size									= fi_GetData(":Creature Settings:Grey Dog", "SIZE=", 2);
	creatureInfo[kGreyDog].mageryLevel					= fi_GetData(":Creature Settings:Grey Dog", "MAGERY_LEVEL=", 0);
	creatureInfo[kGreyDog].spellcastChance				= fi_GetData(":Creature Settings:Grey Dog", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kGreyDog].selfhealChance					= fi_GetData(":Creature Settings:Grey Dog", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kGreyDog].selfhealTrigger				= fi_GetData(":Creature Settings:Grey Dog", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Grey Dog", kGreyDog);
	fi_GetCreatureTalkScript(":Creature Settings:Grey Dog", kGreyDog);
	fi_GetCreatureInventory(":Creature Settings:Grey Dog", kGreyDog);
	fi_GetWeaponModifier(":Creature Settings:Grey Dog", kGreyDog);

	creatureInfo[kBrownDog].bmp												=	0;
	creatureInfo[kBrownDog].domain										=	kLand;
	creatureInfo[kBrownDog].terrain										=	kAnyTerrain;
	creatureInfo[kBrownDog].alignment									= kNeutral;
	creatureInfo[kBrownDog].directionalGraphics				= true;
	creatureInfo[kBrownDog].graphicsIndex							= 1;
	creatureInfo[kBrownDog].corpseID									= 23;
	creatureInfo[kBrownDog].attackSound								= kWolfAttack;
	creatureInfo[kBrownDog].desiredPopulation					= 1000;
	creatureInfo[kBrownDog].meatEater									= true;
	creatureInfo[kBrownDog].color		        					= kBrownWolfColor;
	creatureInfo[kBrownDog].canMorph					        = true;
	creatureInfo[kBrownDog].speed											=	fi_GetData(":Creature Settings:Brown Dog", "SPEED=", 80);
	creatureInfo[kBrownDog].poisonous					      	= fi_GetData(":Creature Settings:Brown Dog", "POISONOUS=", 0);
	creatureInfo[kBrownDog].curse					      			= fi_GetData(":Creature Settings:Brown Dog", "CURSE=", 0);
	creatureInfo[kBrownDog].maxHitPoints							= fi_GetData(":Creature Settings:Brown Dog", "HIT_POINTS=", 15);
	creatureInfo[kBrownDog].maxStrength								= fi_GetData(":Creature Settings:Brown Dog", "STRENGTH=", 15);
	creatureInfo[kBrownDog].maxIntelligence						= fi_GetData(":Creature Settings:Brown Dog", "INTELLIGENCE=", 25);
	creatureInfo[kBrownDog].maxDexterity							= fi_GetData(":Creature Settings:Brown Dog", "DEXTERITY=", 75);
	creatureInfo[kBrownDog].minGold								  	= fi_GetData(":Creature Settings:Brown Dog", "MIN_GOLD=", 0);
	creatureInfo[kBrownDog].maxGold								  	= fi_GetData(":Creature Settings:Brown Dog", "MAX_GOLD=", 0);
	creatureInfo[kBrownDog].seekGold									= fi_GetBoolean(":Creature Settings:Brown Dog", "SEEK_GOLD=", false);
	creatureInfo[kBrownDog].seekMagic									= fi_GetBoolean(":Creature Settings:Brown Dog", "SEEK_MAGIC=", false);
	creatureInfo[kBrownDog].seekWeapons								= fi_GetBoolean(":Creature Settings:Brown Dog", "SEEK_WEAPONS=", false);
	creatureInfo[kBrownDog].magicResist				      	= (float)fi_GetData(":Creature Settings:Brown Dog", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kBrownDog].tamingLevel								= fi_GetData(":Creature Settings:Brown Dog", "TAMING_LEVEL=", 2);
	creatureInfo[kBrownDog].creatureLevel								= fi_GetData(":Creature Settings:Brown Dog", "LEVEL=", 1);
	creatureInfo[kBrownDog].armorRating								= fi_GetData(":Creature Settings:Brown Dog", "ARMOR_RATING=", 10);
	creatureInfo[kBrownDog].immuneToPoison          	= fi_GetBoolean(":Creature Settings:Brown Dog", "POISON_IMMUNITY=", false);
	creatureInfo[kBrownDog].lockStrength							= fi_GetData(":Creature Settings:Brown Dog", "LOCK=", 0);
	creatureInfo[kBrownDog].attackDamageMin						= fi_GetData(":Creature Settings:Brown Dog", "MIN_DAMAGE=", 4);
	creatureInfo[kBrownDog].attackDamageMax						= fi_GetData(":Creature Settings:Brown Dog", "MAX_DAMAGE=", 9);
	creatureInfo[kBrownDog].trackingLevel							= fi_GetData(":Creature Settings:Brown Dog", "TRACKING_LEVEL=", 101);
	creatureInfo[kBrownDog].size									= fi_GetData(":Creature Settings:Brown Dog", "SIZE=", 2);
	creatureInfo[kBrownDog].mageryLevel					= fi_GetData(":Creature Settings:Brown Dog", "MAGERY_LEVEL=", 0);
	creatureInfo[kBrownDog].spellcastChance				= fi_GetData(":Creature Settings:Brown Dog", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kBrownDog].selfhealChance					= fi_GetData(":Creature Settings:Brown Dog", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBrownDog].selfhealTrigger				= fi_GetData(":Creature Settings:Brown Dog", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Brown Dog", kBrownDog);
	fi_GetCreatureTalkScript(":Creature Settings:Brown Dog", kBrownDog);
	fi_GetCreatureInventory(":Creature Settings:Brown Dog", kBrownDog);
	fi_GetWeaponModifier(":Creature Settings:Brown Dog", kBrownDog);

	creatureInfo[kBlackDog].bmp												=	0;
	creatureInfo[kBlackDog].domain										=	kLand;
	creatureInfo[kBlackDog].terrain										=	kAnyTerrain;
	creatureInfo[kBlackDog].alignment									= kNeutral;
	creatureInfo[kBlackDog].directionalGraphics				= true;
	creatureInfo[kBlackDog].graphicsIndex							= 1;
	creatureInfo[kBlackDog].corpseID									= 24;
	creatureInfo[kBlackDog].attackSound								= kWolfAttack;
	creatureInfo[kBlackDog].desiredPopulation					= 300;
	creatureInfo[kBlackDog].meatEater									= true;
	creatureInfo[kBlackDog].color		        					= kBlackWolfColor;
	creatureInfo[kBlackDog].canMorph					        = true;
	creatureInfo[kBlackDog].speed											=	fi_GetData(":Creature Settings:Black Dog", "SPEED=", 80);
	creatureInfo[kBlackDog].poisonous					      	= fi_GetData(":Creature Settings:Black Dog", "POISONOUS=", 0);
	creatureInfo[kBlackDog].curse					      			= fi_GetData(":Creature Settings:Black Dog", "CURSE=", 0);
	creatureInfo[kBlackDog].maxHitPoints							= fi_GetData(":Creature Settings:Black Dog", "HIT_POINTS=", 15);
	creatureInfo[kBlackDog].maxStrength								= fi_GetData(":Creature Settings:Black Dog", "STRENGTH=", 15);
	creatureInfo[kBlackDog].maxIntelligence						= fi_GetData(":Creature Settings:Black Dog", "INTELLIGENCE=", 25);
	creatureInfo[kBlackDog].maxDexterity							= fi_GetData(":Creature Settings:Black Dog", "DEXTERITY=", 75);
	creatureInfo[kBlackDog].minGold								  	= fi_GetData(":Creature Settings:Black Dog", "MIN_GOLD=", 0);
	creatureInfo[kBlackDog].maxGold								  	= fi_GetData(":Creature Settings:Black Dog", "MAX_GOLD=", 0);
	creatureInfo[kBlackDog].seekGold									= fi_GetBoolean(":Creature Settings:Black Dog", "SEEK_GOLD=", false);
	creatureInfo[kBlackDog].seekMagic									= fi_GetBoolean(":Creature Settings:Black Dog", "SEEK_MAGIC=", false);
	creatureInfo[kBlackDog].seekWeapons								= fi_GetBoolean(":Creature Settings:Black Dog", "SEEK_WEAPONS=", false);
	creatureInfo[kBlackDog].magicResist				      	= (float)fi_GetData(":Creature Settings:Black Dog", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kBlackDog].tamingLevel								= fi_GetData(":Creature Settings:Black Dog", "TAMING_LEVEL=", 2);
	creatureInfo[kBlackDog].creatureLevel								= fi_GetData(":Creature Settings:Black Dog", "LEVEL=", 1);
	creatureInfo[kBlackDog].armorRating								= fi_GetData(":Creature Settings:Black Dog", "ARMOR_RATING=", 10);
	creatureInfo[kBlackDog].immuneToPoison          	= fi_GetBoolean(":Creature Settings:Black Dog", "POISON_IMMUNITY=", false);
	creatureInfo[kBlackDog].lockStrength							= fi_GetData(":Creature Settings:Black Dog", "LOCK=", 0);
	creatureInfo[kBlackDog].attackDamageMin						= fi_GetData(":Creature Settings:Black Dog", "MIN_DAMAGE=", 4);
	creatureInfo[kBlackDog].attackDamageMax						= fi_GetData(":Creature Settings:Black Dog", "MAX_DAMAGE=", 9);
	creatureInfo[kBlackDog].trackingLevel							= fi_GetData(":Creature Settings:Black Dog", "TRACKING_LEVEL=", 101);
	creatureInfo[kBlackDog].size									= fi_GetData(":Creature Settings:Black Dog", "SIZE=", 2);
	creatureInfo[kBlackDog].mageryLevel					= fi_GetData(":Creature Settings:Black Dog", "MAGERY_LEVEL=", 0);
	creatureInfo[kBlackDog].spellcastChance				= fi_GetData(":Creature Settings:Black Dog", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kBlackDog].selfhealChance					= fi_GetData(":Creature Settings:Black Dog", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBlackDog].selfhealTrigger				= fi_GetData(":Creature Settings:Black Dog", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Black Dog", kBlackDog);
	fi_GetCreatureTalkScript(":Creature Settings:Black Dog", kBlackDog);
	fi_GetCreatureInventory(":Creature Settings:Black Dog", kBlackDog);
	fi_GetWeaponModifier(":Creature Settings:Black Dog", kBlackDog);

	creatureInfo[kBlackDragon].domain								=	kAir; //Dave kLand
	creatureInfo[kBlackDragon].terrain							=	kAnyTerrain;
	creatureInfo[kBlackDragon].alignment						= kEvil;
	creatureInfo[kBlackDragon].directionalGraphics	= true;
	creatureInfo[kBlackDragon].graphicsIndex				= 1;
	creatureInfo[kBlackDragon].corpseID							= 46;
	creatureInfo[kBlackDragon].randomSound					= kDragonGrowl;
	creatureInfo[kBlackDragon].attackSound					= kDragonAttack;
	creatureInfo[kBlackDragon].attackRange					= 4;
	creatureInfo[kBlackDragon].desiredPopulation		= 5;
	creatureInfo[kBlackDragon].meatEater						= true;
	creatureInfo[kBlackDragon].points						    = 10;
	creatureInfo[kBlackDragon].rareLevel						= 1;
	creatureInfo[kBlackDragon].rareChance						= 100;
	creatureInfo[kBlackDragon].color						    = kBlackDragonColor;
	creatureInfo[kBlackDragon].canMorph					    = true;
	creatureInfo[kBlackDragon].speed										=	fi_GetData(":Creature Settings:Black Dragon", "SPEED=", 50);
	creatureInfo[kBlackDragon].poisonous					      = fi_GetData(":Creature Settings:Black Dragon", "POISONOUS=", 0);
	creatureInfo[kBlackDragon].curse					      		= fi_GetData(":Creature Settings:Black Dragon", "CURSE=", 0);
	creatureInfo[kBlackDragon].maxHitPoints							= fi_GetData(":Creature Settings:Black Dragon", "HIT_POINTS=", 250);
	creatureInfo[kBlackDragon].maxStrength							= fi_GetData(":Creature Settings:Black Dragon", "STRENGTH=", 250);
	creatureInfo[kBlackDragon].maxIntelligence					= fi_GetData(":Creature Settings:Black Dragon", "INTELLIGENCE=", 250);
	creatureInfo[kBlackDragon].maxDexterity							= fi_GetData(":Creature Settings:Black Dragon", "DEXTERITY=", 250);
	creatureInfo[kBlackDragon].minGold								  = fi_GetData(":Creature Settings:Black Dragon", "MIN_GOLD=", 200);
	creatureInfo[kBlackDragon].maxGold								  = fi_GetData(":Creature Settings:Black Dragon", "MAX_GOLD=", 500);
	creatureInfo[kBlackDragon].seekGold									= fi_GetBoolean(":Creature Settings:Black Dragon", "SEEK_GOLD=", true);
	creatureInfo[kBlackDragon].seekMagic								= fi_GetBoolean(":Creature Settings:Black Dragon", "SEEK_MAGIC=", false);
	creatureInfo[kBlackDragon].seekWeapons							= fi_GetBoolean(":Creature Settings:Black Dragon", "SEEK_WEAPONS=", true);
	creatureInfo[kBlackDragon].magicResist				      = (float)fi_GetData(":Creature Settings:Black Dragon", "MAGIC_RESIST=", 0)/100.0;
	creatureInfo[kBlackDragon].tamingLevel							= fi_GetData(":Creature Settings:Black Dragon", "TAMING_LEVEL=", 101);
	creatureInfo[kBlackDragon].creatureLevel							= fi_GetData(":Creature Settings:Black Dragon", "LEVEL=", 1);
	creatureInfo[kBlackDragon].armorRating							= fi_GetData(":Creature Settings:Black Dragon", "ARMOR_RATING=", 100);
	creatureInfo[kBlackDragon].immuneToPoison           = fi_GetBoolean(":Creature Settings:Black Dragon", "POISON_IMMUNITY=", true);
	creatureInfo[kBlackDragon].lockStrength							= fi_GetData(":Creature Settings:Black Dragon", "LOCK=", 5);
	creatureInfo[kBlackDragon].attackDamageMin					= fi_GetData(":Creature Settings:Black Dragon", "MIN_DAMAGE=", 250);
	creatureInfo[kBlackDragon].attackDamageMax					= fi_GetData(":Creature Settings:Black Dragon", "MAX_DAMAGE=", 250);
	creatureInfo[kBlackDragon].trackingLevel							= fi_GetData(":Creature Settings:Black Dragon", "TRACKING_LEVEL=", 101);
	creatureInfo[kBlackDragon].size									= fi_GetData(":Creature Settings:Black Dragon", "SIZE=", 3);
	creatureInfo[kBlackDragon].mageryLevel					= fi_GetData(":Creature Settings:Black Dragon", "MAGERY_LEVEL=", 0);
	creatureInfo[kBlackDragon].spellcastChance				= fi_GetData(":Creature Settings:Black Dragon", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kBlackDragon].selfhealChance					= fi_GetData(":Creature Settings:Black Dragon", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBlackDragon].selfhealTrigger				= fi_GetData(":Creature Settings:Black Dragon", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Black Dragon", kBlackDragon);
	//cr_AddToCreatureControlInventory(kBlackDragon, kRawSteak, 20, 30);
	//cr_AddToCreatureControlInventory(kBlackDragon, kDragonTooth, 10, 20);
	//cr_AddToCreatureControlInventory(kBlackDragon, kDragonScale, 15, 25);
	fi_GetCreatureTalkScript(":Creature Settings:Black Dragon", kBlackDragon);
	fi_GetCreatureInventory(":Creature Settings:Black Dragon", kBlackDragon);
	fi_GetWeaponModifier(":Creature Settings:Black Dragon", kBlackDragon);

	creatureInfo[kPinkDragon].domain								=	kLand;
	creatureInfo[kPinkDragon].terrain							=	kAnyTerrain;
	creatureInfo[kPinkDragon].alignment						= kEvil;
	creatureInfo[kPinkDragon].directionalGraphics	= true;
	creatureInfo[kPinkDragon].graphicsIndex				= 1;
	creatureInfo[kPinkDragon].corpseID							= 74;
	creatureInfo[kPinkDragon].randomSound					= kDragonGrowl;
	creatureInfo[kPinkDragon].attackSound					= kDragonAttack;
	creatureInfo[kPinkDragon].attackRange					= 4;
	creatureInfo[kPinkDragon].desiredPopulation		= 5;
	creatureInfo[kPinkDragon].meatEater						= true;
	creatureInfo[kPinkDragon].points						    = 10;
	creatureInfo[kPinkDragon].rareLevel						= 1;
	creatureInfo[kPinkDragon].rareChance						= 100;
	creatureInfo[kPinkDragon].color						    = kPinkDragonColor;
	creatureInfo[kPinkDragon].canMorph					    = true;
	creatureInfo[kPinkDragon].speed										=	fi_GetData(":Creature Settings:Abyss Dragon", "SPEED=", 50);
	creatureInfo[kPinkDragon].poisonous					      = fi_GetData(":Creature Settings:Abyss Dragon", "POISONOUS=", 0);
	creatureInfo[kPinkDragon].curse					      		= fi_GetData(":Creature Settings:Abyss Dragon", "CURSE=", 0);
	creatureInfo[kPinkDragon].maxHitPoints							= fi_GetData(":Creature Settings:Abyss Dragon", "HIT_POINTS=", 250);
	creatureInfo[kPinkDragon].maxStrength							= fi_GetData(":Creature Settings:Abyss Dragon", "STRENGTH=", 250);
	creatureInfo[kPinkDragon].maxIntelligence					= fi_GetData(":Creature Settings:Abyss Dragon", "INTELLIGENCE=", 250);
	creatureInfo[kPinkDragon].maxDexterity							= fi_GetData(":Creature Settings:Abyss Dragon", "DEXTERITY=", 250);
	creatureInfo[kPinkDragon].minGold								  = fi_GetData(":Creature Settings:Abyss Dragon", "MIN_GOLD=", 200);
	creatureInfo[kPinkDragon].maxGold								  = fi_GetData(":Creature Settings:Abyss Dragon", "MAX_GOLD=", 500);
	creatureInfo[kPinkDragon].seekGold									= fi_GetBoolean(":Creature Settings:Abyss Dragon", "SEEK_GOLD=", true);
	creatureInfo[kPinkDragon].seekMagic								= fi_GetBoolean(":Creature Settings:Abyss Dragon", "SEEK_MAGIC=", false);
	creatureInfo[kPinkDragon].seekWeapons							= fi_GetBoolean(":Creature Settings:Abyss Dragon", "SEEK_WEAPONS=", true);
	creatureInfo[kPinkDragon].magicResist				      = (float)fi_GetData(":Creature Settings:Abyss Dragon", "MAGIC_RESIST=", 0)/100.0;
	creatureInfo[kPinkDragon].tamingLevel							= fi_GetData(":Creature Settings:Abyss Dragon", "TAMING_LEVEL=", 101);
	creatureInfo[kPinkDragon].creatureLevel							= fi_GetData(":Creature Settings:Abyss Dragon", "LEVEL=", 1);
	creatureInfo[kPinkDragon].armorRating							= fi_GetData(":Creature Settings:Abyss Dragon", "ARMOR_RATING=", 100);
	creatureInfo[kPinkDragon].immuneToPoison           = fi_GetBoolean(":Creature Settings:Abyss Dragon", "POISON_IMMUNITY=", true);
	creatureInfo[kPinkDragon].lockStrength							= fi_GetData(":Creature Settings:Abyss Dragon", "LOCK=", 5);
	creatureInfo[kPinkDragon].attackDamageMin					= fi_GetData(":Creature Settings:Abyss Dragon", "MIN_DAMAGE=", 250);
	creatureInfo[kPinkDragon].attackDamageMax					= fi_GetData(":Creature Settings:Abyss Dragon", "MAX_DAMAGE=", 250);
	creatureInfo[kPinkDragon].trackingLevel							= fi_GetData(":Creature Settings:Abyss Dragon", "TRACKING_LEVEL=", 101);
	creatureInfo[kPinkDragon].size									= fi_GetData(":Creature Settings:Abyss Dragon", "SIZE=", 3);
	creatureInfo[kPinkDragon].mageryLevel					= fi_GetData(":Creature Settings:Abyss Dragon", "MAGERY_LEVEL=", 0);
	creatureInfo[kPinkDragon].spellcastChance				= fi_GetData(":Creature Settings:Abyss Dragon", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kPinkDragon].selfhealChance					= fi_GetData(":Creature Settings:Pink Dragon", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kPinkDragon].selfhealTrigger				= fi_GetData(":Creature Settings:Pink Dragon", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Abyss Dragon", kPinkDragon);
	fi_GetCreatureTalkScript(":Creature Settings:Abyss Dragon", kPinkDragon);
	fi_GetCreatureInventory(":Creature Settings:Abyss Dragon", kPinkDragon);
	fi_GetWeaponModifier(":Creature Settings:Abyss Dragon", kPinkDragon);

	creatureInfo[kRedDragon].domain								=	kLand;
	creatureInfo[kRedDragon].terrain							=	kAnyTerrain;
	creatureInfo[kRedDragon].alignment						= kEvil;
	creatureInfo[kRedDragon].directionalGraphics	= true;
	creatureInfo[kRedDragon].graphicsIndex				= 1;
	creatureInfo[kRedDragon].corpseID							= 15;
	creatureInfo[kRedDragon].randomSound					= kDragonGrowl;
	creatureInfo[kRedDragon].attackSound					= kDragonAttack;
	creatureInfo[kRedDragon].attackRange					= 4;
	creatureInfo[kRedDragon].desiredPopulation		= 5;
	creatureInfo[kRedDragon].meatEater						= true;
	creatureInfo[kRedDragon].points						    = 10;
	creatureInfo[kRedDragon].rareLevel						= 1;
	creatureInfo[kRedDragon].rareChance						= 100;
	creatureInfo[kRedDragon].color						    = kRedDragonColor;
	creatureInfo[kRedDragon].canMorph					    = true;
	creatureInfo[kRedDragon].speed										=	fi_GetData(":Creature Settings:Red Dragon", "SPEED=", 50);
	creatureInfo[kRedDragon].poisonous					      = fi_GetData(":Creature Settings:Red Dragon", "POISONOUS=", 0);
	creatureInfo[kRedDragon].curse					      		= fi_GetData(":Creature Settings:Red Dragon", "CURSE=", 0);
	creatureInfo[kRedDragon].maxHitPoints							= fi_GetData(":Creature Settings:Red Dragon", "HIT_POINTS=", 200);
	creatureInfo[kRedDragon].maxStrength							= fi_GetData(":Creature Settings:Red Dragon", "STRENGTH=", 200);
	creatureInfo[kRedDragon].maxIntelligence					= fi_GetData(":Creature Settings:Red Dragon", "INTELLIGENCE=", 200);
	creatureInfo[kRedDragon].maxDexterity							= fi_GetData(":Creature Settings:Red Dragon", "DEXTERITY=", 200);
	creatureInfo[kRedDragon].minGold								  = fi_GetData(":Creature Settings:Red Dragon", "MIN_GOLD=", 200);
	creatureInfo[kRedDragon].maxGold								  = fi_GetData(":Creature Settings:Red Dragon", "MAX_GOLD=", 500);
	creatureInfo[kRedDragon].seekGold									= fi_GetBoolean(":Creature Settings:Red Dragon", "SEEK_GOLD=", true);
	creatureInfo[kRedDragon].seekMagic								= fi_GetBoolean(":Creature Settings:Red Dragon", "SEEK_MAGIC=", false);
	creatureInfo[kRedDragon].seekWeapons							= fi_GetBoolean(":Creature Settings:Red Dragon", "SEEK_WEAPONS=", true);
	creatureInfo[kRedDragon].magicResist				      = (float)fi_GetData(":Creature Settings:Red Dragon", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kRedDragon].tamingLevel							= fi_GetData(":Creature Settings:Red Dragon", "TAMING_LEVEL=", 8);
	creatureInfo[kRedDragon].creatureLevel							= fi_GetData(":Creature Settings:Red Dragon", "LEVEL=", 1);
	creatureInfo[kRedDragon].armorRating							= fi_GetData(":Creature Settings:Red Dragon", "ARMOR_RATING=", 75);
	creatureInfo[kRedDragon].immuneToPoison           = fi_GetBoolean(":Creature Settings:Red Dragon", "POISON_IMMUNITY=", false);
	creatureInfo[kRedDragon].lockStrength							= fi_GetData(":Creature Settings:Red Dragon", "LOCK=", 5);
	creatureInfo[kRedDragon].attackDamageMin					= fi_GetData(":Creature Settings:Red Dragon", "MIN_DAMAGE=", 60);
	creatureInfo[kRedDragon].attackDamageMax					= fi_GetData(":Creature Settings:Red Dragon", "MAX_DAMAGE=", 150);
	creatureInfo[kRedDragon].trackingLevel							= fi_GetData(":Creature Settings:Red Dragon", "TRACKING_LEVEL=", 101);
	creatureInfo[kRedDragon].size									= fi_GetData(":Creature Settings:Red Dragon", "SIZE=", 3);
	creatureInfo[kRedDragon].mageryLevel					= fi_GetData(":Creature Settings:Red Dragon", "MAGERY_LEVEL=", 0);
	creatureInfo[kRedDragon].spellcastChance				= fi_GetData(":Creature Settings:Red Dragon", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kRedDragon].selfhealChance					= fi_GetData(":Creature Settings:Red Dragon", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kRedDragon].selfhealTrigger				= fi_GetData(":Creature Settings:Red Dragon", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Red Dragon", kRedDragon);
	fi_GetCreatureTalkScript(":Creature Settings:Red Dragon", kRedDragon);
	fi_GetCreatureInventory(":Creature Settings:Red Dragon", kRedDragon);
	fi_GetWeaponModifier(":Creature Settings:Red Dragon", kRedDragon);
	/*
		cr_AddToCreatureControlInventory(kRedDragon, kRawSteak, 20, 30);
		cr_AddToCreatureControlInventory(kRedDragon, kDragonTooth, 10, 20);
		cr_AddToCreatureControlInventory(kRedDragon, kDragonScale, 15, 25);
		cr_AddToCreatureControlInventory(kRedDragon, kGreenCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kRedDragon, kRedCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kRedDragon, kBlueCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kRedDragon, kPurpleCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kRedDragon, kOrangeCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kRedDragon, kYellowCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kRedDragon, kFireCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kRedDragon, kBlackCrystal, 0, 2);
	*/
	creatureInfo[kIceDragon].domain								=	kLand;
	creatureInfo[kIceDragon].terrain							=	kAnyTerrain;
	creatureInfo[kIceDragon].alignment						= kEvil;
	creatureInfo[kIceDragon].directionalGraphics	= true;
	creatureInfo[kIceDragon].graphicsIndex				= 1;
	creatureInfo[kIceDragon].corpseID							= 20;
	creatureInfo[kIceDragon].randomSound					= kDragonGrowl;
	creatureInfo[kIceDragon].attackSound					= kDragonAttack;
	creatureInfo[kIceDragon].attackRange					= 3;
	creatureInfo[kIceDragon].desiredPopulation		= 8;
	creatureInfo[kIceDragon].meatEater						= true;
	creatureInfo[kIceDragon].points						    = 10;
	creatureInfo[kIceDragon].rareLevel						= 1;
	creatureInfo[kIceDragon].rareChance						= 100;
	creatureInfo[kIceDragon].color						    = kIceDragonColor;
	creatureInfo[kIceDragon].canMorph					    = true;
	creatureInfo[kIceDragon].frost					    	= true;
	creatureInfo[kIceDragon].speed										=	fi_GetData(":Creature Settings:Ice Dragon", "SPEED=", 50);
	creatureInfo[kIceDragon].poisonous					      = fi_GetData(":Creature Settings:Ice Dragon", "POISONOUS=", 0);
	creatureInfo[kIceDragon].curse					      		= fi_GetData(":Creature Settings:Ice Dragon", "CURSE=", 0);
	creatureInfo[kIceDragon].maxHitPoints							= fi_GetData(":Creature Settings:Ice Dragon", "HIT_POINTS=", 200);
	creatureInfo[kIceDragon].maxStrength							= fi_GetData(":Creature Settings:Ice Dragon", "STRENGTH=", 200);
	creatureInfo[kIceDragon].maxIntelligence					= fi_GetData(":Creature Settings:Ice Dragon", "INTELLIGENCE=", 200);
	creatureInfo[kIceDragon].maxDexterity							= fi_GetData(":Creature Settings:Ice Dragon", "DEXTERITY=", 50);
	creatureInfo[kIceDragon].minGold								  = fi_GetData(":Creature Settings:Ice Dragon", "MIN_GOLD=", 200);
	creatureInfo[kIceDragon].maxGold								  = fi_GetData(":Creature Settings:Ice Dragon", "MAX_GOLD=", 500);
	creatureInfo[kIceDragon].seekGold									= fi_GetBoolean(":Creature Settings:Ice Dragon", "SEEK_GOLD=", true);
	creatureInfo[kIceDragon].seekMagic								= fi_GetBoolean(":Creature Settings:Ice Dragon", "SEEK_MAGIC=", false);
	creatureInfo[kIceDragon].seekWeapons							= fi_GetBoolean(":Creature Settings:Ice Dragon", "SEEK_WEAPONS=", true);
	creatureInfo[kIceDragon].magicResist				      = (float)fi_GetData(":Creature Settings:Ice Dragon", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kIceDragon].tamingLevel							= fi_GetData(":Creature Settings:Ice Dragon", "TAMING_LEVEL=", 8);
	creatureInfo[kIceDragon].creatureLevel							= fi_GetData(":Creature Settings:Ice Dragon", "LEVEL=", 1);
	creatureInfo[kIceDragon].armorRating							= fi_GetData(":Creature Settings:Ice Dragon", "ARMOR_RATING=", 75);
	creatureInfo[kIceDragon].immuneToPoison           = fi_GetBoolean(":Creature Settings:Ice Dragon", "POISON_IMMUNITY=", false);
	creatureInfo[kIceDragon].lockStrength							= fi_GetData(":Creature Settings:Ice Dragon", "LOCK=", 5);
	creatureInfo[kIceDragon].attackDamageMin					= fi_GetData(":Creature Settings:Ice Dragon", "MIN_DAMAGE=", 45);
	creatureInfo[kIceDragon].attackDamageMax					= fi_GetData(":Creature Settings:Ice Dragon", "MAX_DAMAGE=", 125);
	creatureInfo[kIceDragon].trackingLevel							= fi_GetData(":Creature Settings:Ice Dragon", "TRACKING_LEVEL=", 101);
	creatureInfo[kIceDragon].size									= fi_GetData(":Creature Settings:Ice Dragon", "SIZE=", 3);
	creatureInfo[kIceDragon].mageryLevel					= fi_GetData(":Creature Settings:Ice Dragon", "MAGERY_LEVEL=", 0);
	creatureInfo[kIceDragon].spellcastChance				= fi_GetData(":Creature Settings:Ice Dragon", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kIceDragon].selfhealChance					= fi_GetData(":Creature Settings:Ice Dragon", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kIceDragon].selfhealTrigger				= fi_GetData(":Creature Settings:Ice Dragon", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Ice Dragon", kIceDragon);
	/*
		cr_AddToCreatureControlInventory(kIceDragon, kRawSteak, 20, 30);
		cr_AddToCreatureControlInventory(kIceDragon, kDragonTooth, 10, 20);
		cr_AddToCreatureControlInventory(kIceDragon, kDragonScale, 15, 25);
		cr_AddToCreatureControlInventory(kIceDragon, kGreenCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kIceDragon, kRedCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kIceDragon, kBlueCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kIceDragon, kPurpleCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kIceDragon, kOrangeCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kIceDragon, kYellowCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kIceDragon, kFireCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kIceDragon, kBlackCrystal, 0, 2);
	*/
	fi_GetCreatureTalkScript(":Creature Settings:Ice Dragon", kIceDragon);
	fi_GetCreatureInventory(":Creature Settings:Ice Dragon", kIceDragon);
	fi_GetWeaponModifier(":Creature Settings:Ice Dragon", kIceDragon);

	creatureInfo[kForestDragon].domain							=	kLand;
	creatureInfo[kForestDragon].terrain							=	kGrass;
	creatureInfo[kForestDragon].alignment						= kEvil;
	creatureInfo[kForestDragon].directionalGraphics	= true;
	creatureInfo[kForestDragon].graphicsIndex				= 1;
	creatureInfo[kForestDragon].corpseID						= 31;
	creatureInfo[kForestDragon].randomSound					= kDragonGrowl;
	creatureInfo[kForestDragon].attackSound					= kDragonAttack;
	creatureInfo[kForestDragon].attackRange					= 3;
	creatureInfo[kForestDragon].desiredPopulation		= 5;
	creatureInfo[kForestDragon].meatEater						= true;
	creatureInfo[kForestDragon].points						  = 10;
	creatureInfo[kForestDragon].rareLevel						= 1;
	creatureInfo[kForestDragon].rareChance					= 100;
	creatureInfo[kForestDragon].color						    = kForestDragonColor;
	creatureInfo[kForestDragon].canMorph					  = true;
	creatureInfo[kForestDragon].speed										=	fi_GetData(":Creature Settings:Forest Dragon", "SPEED=", 50);
	creatureInfo[kForestDragon].poisonous					      = fi_GetData(":Creature Settings:Forest Dragon", "POISONOUS=", 0);
	creatureInfo[kForestDragon].curse					      		= fi_GetData(":Creature Settings:Forest Dragon", "CURSE=", 0);
	creatureInfo[kForestDragon].maxHitPoints						= fi_GetData(":Creature Settings:Forest Dragon", "HIT_POINTS=", 200);
	creatureInfo[kForestDragon].maxStrength							= fi_GetData(":Creature Settings:Forest Dragon", "STRENGTH=", 200);
	creatureInfo[kForestDragon].maxIntelligence					= fi_GetData(":Creature Settings:Forest Dragon", "INTELLIGENCE=", 200);
	creatureInfo[kForestDragon].maxDexterity						= fi_GetData(":Creature Settings:Forest Dragon", "DEXTERITY=", 50);
	creatureInfo[kForestDragon].minGold								  = fi_GetData(":Creature Settings:Forest Dragon", "MIN_GOLD=", 200);
	creatureInfo[kForestDragon].maxGold								  = fi_GetData(":Creature Settings:Forest Dragon", "MAX_GOLD=", 500);
	creatureInfo[kForestDragon].seekGold								= fi_GetBoolean(":Creature Settings:Forest Dragon", "SEEK_GOLD=", true);
	creatureInfo[kForestDragon].seekMagic								= fi_GetBoolean(":Creature Settings:Forest Dragon", "SEEK_MAGIC=", false);
	creatureInfo[kForestDragon].seekWeapons							= fi_GetBoolean(":Creature Settings:Forest Dragon", "SEEK_WEAPONS=", true);
	creatureInfo[kForestDragon].magicResist				      = (float)fi_GetData(":Creature Settings:Forest Dragon", "MAGIC_RESIST=", 15)/100.0;
	creatureInfo[kForestDragon].tamingLevel							= fi_GetData(":Creature Settings:Forest Dragon", "TAMING_LEVEL=", 8);
	creatureInfo[kForestDragon].creatureLevel							= fi_GetData(":Creature Settings:Forest Dragon", "LEVEL=", 1);
	creatureInfo[kForestDragon].armorRating							= fi_GetData(":Creature Settings:Forest Dragon", "ARMOR_RATING=", 75);
	creatureInfo[kForestDragon].immuneToPoison          = fi_GetBoolean(":Creature Settings:Forest Dragon", "POISON_IMMUNITY=", false);
	creatureInfo[kForestDragon].lockStrength						= fi_GetData(":Creature Settings:Forest Dragon", "LOCK=", 5);
	creatureInfo[kForestDragon].attackDamageMin					= fi_GetData(":Creature Settings:Forest Dragon", "MIN_DAMAGE=", 45);
	creatureInfo[kForestDragon].attackDamageMax					= fi_GetData(":Creature Settings:Forest Dragon", "MAX_DAMAGE=", 125);
	creatureInfo[kForestDragon].trackingLevel							= fi_GetData(":Creature Settings:Forest Dragon", "TRACKING_LEVEL=", 101);
	creatureInfo[kForestDragon].size									= fi_GetData(":Creature Settings:Forest Dragon", "SIZE=", 3);
	creatureInfo[kForestDragon].mageryLevel					= fi_GetData(":Creature Settings:Forest Dragon", "MAGERY_LEVEL=", 0);
	creatureInfo[kForestDragon].spellcastChance				= fi_GetData(":Creature Settings:Forest Dragon", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kForestDragon].selfhealChance					= fi_GetData(":Creature Settings:Forest Dragon", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kForestDragon].selfhealTrigger				= fi_GetData(":Creature Settings:Forest Dragon", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Forest Dragon", kForestDragon);
	/*
		cr_AddToCreatureControlInventory(kForestDragon, kAncientTome, 1, 1);
		cr_AddToCreatureControlInventory(kForestDragon, kRawSteak, 20, 30);
		cr_AddToCreatureControlInventory(kForestDragon, kDragonTooth, 10, 20);
		cr_AddToCreatureControlInventory(kForestDragon, kDragonScale, 15, 25);
		cr_AddToCreatureControlInventory(kForestDragon, kPurpleRobeProtection, 0, 1);
		cr_AddToCreatureControlInventory(kForestDragon, kRedRobeProtection, 0, 1);
		cr_AddToCreatureControlInventory(kForestDragon, kYellowRobeResistance, 0, 1);
		cr_AddToCreatureControlInventory(kForestDragon, kBlueRobeResistance, 0, 1);
		cr_AddToCreatureControlInventory(kForestDragon, kGreenCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kForestDragon, kRedCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kForestDragon, kBlueCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kForestDragon, kPurpleCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kForestDragon, kOrangeCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kForestDragon, kYellowCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kForestDragon, kFireCrystal, 0, 2);
		cr_AddToCreatureControlInventory(kForestDragon, kBlackCrystal, 0, 2);
	*/
	fi_GetCreatureTalkScript(":Creature Settings:Forest Dragon", kForestDragon);
	fi_GetCreatureInventory(":Creature Settings:Forest Dragon", kForestDragon);
	fi_GetWeaponModifier(":Creature Settings:Forest Dragon", kForestDragon);

	creatureInfo[kBlackDragonAncient].bmp								=	4;
	creatureInfo[kBlackDragonAncient].domain								=	kLand;
	creatureInfo[kBlackDragonAncient].terrain							=	kAnyTerrain;
	creatureInfo[kBlackDragonAncient].alignment						= kEvil;
	creatureInfo[kBlackDragonAncient].directionalGraphics	= true;
	creatureInfo[kBlackDragonAncient].graphicsIndex				= 1;
	creatureInfo[kBlackDragonAncient].corpseID							= 133;
	creatureInfo[kBlackDragonAncient].randomSound					= kDragonGrowl;
	creatureInfo[kBlackDragonAncient].attackSound					= kDragonAttack;
	creatureInfo[kBlackDragonAncient].attackRange					= 4;
	creatureInfo[kBlackDragonAncient].desiredPopulation		= 0;
	creatureInfo[kBlackDragonAncient].meatEater						= true;
	creatureInfo[kBlackDragonAncient].points						    = 10;
	creatureInfo[kBlackDragonAncient].rareLevel						= 1;
	creatureInfo[kBlackDragonAncient].rareChance						= 100;
	creatureInfo[kBlackDragonAncient].color						    = kBlackDragonColor;
	creatureInfo[kBlackDragonAncient].canMorph					    = true;
	creatureInfo[kBlackDragonAncient].speed										=	fi_GetData(":Creature Settings:Ancient Black Dragon", "SPEED=", 50);
	creatureInfo[kBlackDragonAncient].poisonous					      = fi_GetData(":Creature Settings:Ancient Black Dragon", "POISONOUS=", 0);
	creatureInfo[kBlackDragonAncient].curse					      		= fi_GetData(":Creature Settings:Ancient Black Dragon", "CURSE=", 0);
	creatureInfo[kBlackDragonAncient].maxHitPoints							= fi_GetData(":Creature Settings:Ancient Black Dragon", "HIT_POINTS=", 250);
	creatureInfo[kBlackDragonAncient].maxStrength							= fi_GetData(":Creature Settings:Ancient Black Dragon", "STRENGTH=", 250);
	creatureInfo[kBlackDragonAncient].maxIntelligence					= fi_GetData(":Creature Settings:Ancient Black Dragon", "INTELLIGENCE=", 250);
	creatureInfo[kBlackDragonAncient].maxDexterity							= fi_GetData(":Creature Settings:Ancient Black Dragon", "DEXTERITY=", 250);
	creatureInfo[kBlackDragonAncient].minGold								  = fi_GetData(":Creature Settings:Ancient Black Dragon", "MIN_GOLD=", 200);
	creatureInfo[kBlackDragonAncient].maxGold								  = fi_GetData(":Creature Settings:Ancient Black Dragon", "MAX_GOLD=", 500);
	creatureInfo[kBlackDragonAncient].seekGold									= fi_GetBoolean(":Creature Settings:Ancient Black Dragon", "SEEK_GOLD=", true);
	creatureInfo[kBlackDragonAncient].seekMagic								= fi_GetBoolean(":Creature Settings:Ancient Black Dragon", "SEEK_MAGIC=", false);
	creatureInfo[kBlackDragonAncient].seekWeapons							= fi_GetBoolean(":Creature Settings:Ancient Black Dragon", "SEEK_WEAPONS=", true);
	creatureInfo[kBlackDragonAncient].magicResist				      = (float)fi_GetData(":Creature Settings:Ancient Black Dragon", "MAGIC_RESIST=", 0)/100.0;
	creatureInfo[kBlackDragonAncient].tamingLevel							= fi_GetData(":Creature Settings:Ancient Black Dragon", "TAMING_LEVEL=", 101);
	creatureInfo[kBlackDragonAncient].creatureLevel							= fi_GetData(":Creature Settings:Ancient Black Dragon", "LEVEL=", 1);
	creatureInfo[kBlackDragonAncient].armorRating							= fi_GetData(":Creature Settings:Ancient Black Dragon", "ARMOR_RATING=", 100);
	creatureInfo[kBlackDragonAncient].immuneToPoison           = fi_GetBoolean(":Creature Settings:Ancient Black Dragon", "POISON_IMMUNITY=", true);
	creatureInfo[kBlackDragonAncient].lockStrength							= fi_GetData(":Creature Settings:Ancient Black Dragon", "LOCK=", 5);
	creatureInfo[kBlackDragonAncient].attackDamageMin					= fi_GetData(":Creature Settings:Ancient Black Dragon", "MIN_DAMAGE=", 250);
	creatureInfo[kBlackDragonAncient].attackDamageMax					= fi_GetData(":Creature Settings:Ancient Black Dragon", "MAX_DAMAGE=", 250);
	creatureInfo[kBlackDragonAncient].trackingLevel							= fi_GetData(":Creature Settings:Ancient Black Dragon", "TRACKING_LEVEL=", 101);
	creatureInfo[kBlackDragonAncient].size									= fi_GetData(":Creature Settings:Ancient Black Dragon", "SIZE=", 3);
	creatureInfo[kBlackDragonAncient].mageryLevel					= fi_GetData(":Creature Settings:Ancient Black Dragon", "MAGERY_LEVEL=", 15);
	creatureInfo[kBlackDragonAncient].spellcastChance				= fi_GetData(":Creature Settings:Ancient Black Dragon", "SPELLCAST_CHANCE=", 10);
	creatureInfo[kBlackDragonAncient].selfhealChance					= fi_GetData(":Creature Settings:Ancient Black Dragon", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBlackDragonAncient].selfhealTrigger				= fi_GetData(":Creature Settings:Ancient Black Dragon", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Ancient Black Dragon", kBlackDragonAncient);
	fi_GetCreatureTalkScript(":Creature Settings:Ancient Black Dragon", kBlackDragonAncient);
	fi_GetCreatureInventory(":Creature Settings:Ancient Black Dragon", kBlackDragonAncient);
	fi_GetWeaponModifier(":Creature Settings:Ancient Black Dragon", kBlackDragonAncient);

	creatureInfo[kRedDragonAncient].bmp								=	4;
	creatureInfo[kRedDragonAncient].domain								=	kLand;
	creatureInfo[kRedDragonAncient].terrain							=	kAnyTerrain;
	creatureInfo[kRedDragonAncient].alignment						= kEvil;
	creatureInfo[kRedDragonAncient].directionalGraphics	= true;
	creatureInfo[kRedDragonAncient].graphicsIndex				= 1;
	creatureInfo[kRedDragonAncient].corpseID							= 136;
	creatureInfo[kRedDragonAncient].randomSound					= kDragonGrowl;
	creatureInfo[kRedDragonAncient].attackSound					= kDragonAttack;
	creatureInfo[kRedDragonAncient].attackRange					= 4;
	creatureInfo[kRedDragonAncient].desiredPopulation		= 5;
	creatureInfo[kRedDragonAncient].meatEater						= true;
	creatureInfo[kRedDragonAncient].points						    = 10;
	creatureInfo[kRedDragonAncient].rareLevel						= 1;
	creatureInfo[kRedDragonAncient].rareChance						= 100;
	creatureInfo[kRedDragonAncient].color						    = kRedDragonColor;
	creatureInfo[kRedDragonAncient].canMorph					    = true;
	creatureInfo[kRedDragonAncient].speed										=	fi_GetData(":Creature Settings:Ancient Red Dragon", "SPEED=", 50);
	creatureInfo[kRedDragonAncient].poisonous					      = fi_GetData(":Creature Settings:Ancient Red Dragon", "POISONOUS=", 0);
	creatureInfo[kRedDragonAncient].curse					      		= fi_GetData(":Creature Settings:Ancient Red Dragon", "CURSE=", 0);
	creatureInfo[kRedDragonAncient].maxHitPoints							= fi_GetData(":Creature Settings:Ancient Red Dragon", "HIT_POINTS=", 200);
	creatureInfo[kRedDragonAncient].maxStrength							= fi_GetData(":Creature Settings:Ancient Red Dragon", "STRENGTH=", 200);
	creatureInfo[kRedDragonAncient].maxIntelligence					= fi_GetData(":Creature Settings:Ancient Red Dragon", "INTELLIGENCE=", 200);
	creatureInfo[kRedDragonAncient].maxDexterity							= fi_GetData(":Creature Settings:Ancient Red Dragon", "DEXTERITY=", 200);
	creatureInfo[kRedDragonAncient].minGold								  = fi_GetData(":Creature Settings:Ancient Red Dragon", "MIN_GOLD=", 200);
	creatureInfo[kRedDragonAncient].maxGold								  = fi_GetData(":Creature Settings:Ancient Red Dragon", "MAX_GOLD=", 500);
	creatureInfo[kRedDragonAncient].seekGold									= fi_GetBoolean(":Creature Settings:Ancient Red Dragon", "SEEK_GOLD=", true);
	creatureInfo[kRedDragonAncient].seekMagic								= fi_GetBoolean(":Creature Settings:Ancient Red Dragon", "SEEK_MAGIC=", false);
	creatureInfo[kRedDragonAncient].seekWeapons							= fi_GetBoolean(":Creature Settings:Ancient Red Dragon", "SEEK_WEAPONS=", true);
	creatureInfo[kRedDragonAncient].magicResist				      = (float)fi_GetData(":Creature Settings:Ancient Red Dragon", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kRedDragonAncient].tamingLevel							= fi_GetData(":Creature Settings:Ancient Red Dragon", "TAMING_LEVEL=", 8);
	creatureInfo[kRedDragonAncient].creatureLevel							= fi_GetData(":Creature Settings:Ancient Red Dragon", "LEVEL=", 1);
	creatureInfo[kRedDragonAncient].armorRating							= fi_GetData(":Creature Settings:Ancient Red Dragon", "ARMOR_RATING=", 75);
	creatureInfo[kRedDragonAncient].immuneToPoison           = fi_GetBoolean(":Creature Settings:Ancient Red Dragon", "POISON_IMMUNITY=", false);
	creatureInfo[kRedDragonAncient].lockStrength							= fi_GetData(":Creature Settings:Ancient Red Dragon", "LOCK=", 5);
	creatureInfo[kRedDragonAncient].attackDamageMin					= fi_GetData(":Creature Settings:Ancient Red Dragon", "MIN_DAMAGE=", 60);
	creatureInfo[kRedDragonAncient].attackDamageMax					= fi_GetData(":Creature Settings:Ancient Red Dragon", "MAX_DAMAGE=", 150);
	creatureInfo[kRedDragonAncient].trackingLevel							= fi_GetData(":Creature Settings:Ancient Red Dragon", "TRACKING_LEVEL=", 101);
	creatureInfo[kRedDragonAncient].size									= fi_GetData(":Creature Settings:Ancient Red Dragon", "SIZE=", 3);
	creatureInfo[kRedDragonAncient].mageryLevel					= fi_GetData(":Creature Settings:Ancient Red Dragon", "MAGERY_LEVEL=", 15);
	creatureInfo[kRedDragonAncient].spellcastChance				= fi_GetData(":Creature Settings:Ancient Red Dragon", "SPELLCAST_CHANCE=", 10);
	creatureInfo[kRedDragonAncient].selfhealChance					= fi_GetData(":Creature Settings:Ancient Red Dragon", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kRedDragonAncient].selfhealTrigger				= fi_GetData(":Creature Settings:Ancient Red Dragon", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Ancient Red Dragon", kRedDragonAncient);
	fi_GetCreatureTalkScript(":Creature Settings:Ancient Red Dragon", kRedDragonAncient);
	fi_GetCreatureInventory(":Creature Settings:Ancient Red Dragon", kRedDragonAncient);
	fi_GetWeaponModifier(":Creature Settings:Ancient Red Dragon", kRedDragonAncient);

	creatureInfo[kIceDragonAncient].bmp								=	4;
	creatureInfo[kIceDragonAncient].domain								=	kLand;
	creatureInfo[kIceDragonAncient].terrain							=	kAnyTerrain;
	creatureInfo[kIceDragonAncient].alignment						= kEvil;
	creatureInfo[kIceDragonAncient].directionalGraphics	= true;
	creatureInfo[kIceDragonAncient].graphicsIndex				= 1;
	creatureInfo[kIceDragonAncient].corpseID							= 135;
	creatureInfo[kIceDragonAncient].randomSound					= kDragonGrowl;
	creatureInfo[kIceDragonAncient].attackSound					= kDragonAttack;
	creatureInfo[kIceDragonAncient].attackRange					= 3;
	creatureInfo[kIceDragonAncient].desiredPopulation		= 8;
	creatureInfo[kIceDragonAncient].meatEater						= true;
	creatureInfo[kIceDragonAncient].points						    = 10;
	creatureInfo[kIceDragonAncient].rareLevel						= 1;
	creatureInfo[kIceDragonAncient].rareChance						= 100;
	creatureInfo[kIceDragonAncient].color						    = kIceDragonColor;
	creatureInfo[kIceDragonAncient].canMorph					    = true;
	creatureInfo[kIceDragonAncient].frost					    	= true;
	creatureInfo[kIceDragonAncient].speed										=	fi_GetData(":Creature Settings:Ancient Ice Dragon", "SPEED=", 50);
	creatureInfo[kIceDragonAncient].poisonous					      = fi_GetData(":Creature Settings:Ancient Ice Dragon", "POISONOUS=", 0);
	creatureInfo[kIceDragonAncient].curse					      		= fi_GetData(":Creature Settings:Ancient Ice Dragon", "CURSE=", 0);
	creatureInfo[kIceDragonAncient].maxHitPoints							= fi_GetData(":Creature Settings:Ancient Ice Dragon", "HIT_POINTS=", 200);
	creatureInfo[kIceDragonAncient].maxStrength							= fi_GetData(":Creature Settings:Ancient Ice Dragon", "STRENGTH=", 200);
	creatureInfo[kIceDragonAncient].maxIntelligence					= fi_GetData(":Creature Settings:Ancient Ice Dragon", "INTELLIGENCE=", 200);
	creatureInfo[kIceDragonAncient].maxDexterity							= fi_GetData(":Creature Settings:Ancient Ice Dragon", "DEXTERITY=", 50);
	creatureInfo[kIceDragonAncient].minGold								  = fi_GetData(":Creature Settings:Ancient Ice Dragon", "MIN_GOLD=", 200);
	creatureInfo[kIceDragonAncient].maxGold								  = fi_GetData(":Creature Settings:Ancient Ice Dragon", "MAX_GOLD=", 500);
	creatureInfo[kIceDragonAncient].seekGold									= fi_GetBoolean(":Creature Settings:Ancient Ice Dragon", "SEEK_GOLD=", true);
	creatureInfo[kIceDragonAncient].seekMagic								= fi_GetBoolean(":Creature Settings:Ancient Ice Dragon", "SEEK_MAGIC=", false);
	creatureInfo[kIceDragonAncient].seekWeapons							= fi_GetBoolean(":Creature Settings:Ancient Ice Dragon", "SEEK_WEAPONS=", true);
	creatureInfo[kIceDragonAncient].magicResist				      = (float)fi_GetData(":Creature Settings:Ancient Ice Dragon", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kIceDragonAncient].tamingLevel							= fi_GetData(":Creature Settings:Ancient Ice Dragon", "TAMING_LEVEL=", 8);
	creatureInfo[kIceDragonAncient].creatureLevel							= fi_GetData(":Creature Settings:Ancient Ice Dragon", "LEVEL=", 1);
	creatureInfo[kIceDragonAncient].armorRating							= fi_GetData(":Creature Settings:Ancient Ice Dragon", "ARMOR_RATING=", 75);
	creatureInfo[kIceDragonAncient].immuneToPoison           = fi_GetBoolean(":Creature Settings:Ancient Ice Dragon", "POISON_IMMUNITY=", false);
	creatureInfo[kIceDragonAncient].lockStrength							= fi_GetData(":Creature Settings:Ancient Ice Dragon", "LOCK=", 5);
	creatureInfo[kIceDragonAncient].attackDamageMin					= fi_GetData(":Creature Settings:Ancient Ice Dragon", "MIN_DAMAGE=", 45);
	creatureInfo[kIceDragonAncient].attackDamageMax					= fi_GetData(":Creature Settings:Ancient Ice Dragon", "MAX_DAMAGE=", 125);
	creatureInfo[kIceDragonAncient].trackingLevel							= fi_GetData(":Creature Settings:Ancient Ice Dragon", "TRACKING_LEVEL=", 101);
	creatureInfo[kIceDragonAncient].size									= fi_GetData(":Creature Settings:Ancient Ice Dragon", "SIZE=", 3);
	creatureInfo[kIceDragonAncient].mageryLevel					= fi_GetData(":Creature Settings:Ancient Ice Dragon", "MAGERY_LEVEL=", 15);
	creatureInfo[kIceDragonAncient].spellcastChance				= fi_GetData(":Creature Settings:Ancient Ice Dragon", "SPELLCAST_CHANCE=", 10);
	creatureInfo[kIceDragonAncient].selfhealChance					= fi_GetData(":Creature Settings:Ancient Ice Dragon", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kIceDragonAncient].selfhealTrigger				= fi_GetData(":Creature Settings:Ancient Ice Dragon", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Ancient Ice Dragon", kIceDragonAncient);
	fi_GetCreatureTalkScript(":Creature Settings:Ancient Ice Dragon", kIceDragonAncient);
	fi_GetCreatureInventory(":Creature Settings:Ancient Ice Dragon", kIceDragonAncient);
	fi_GetWeaponModifier(":Creature Settings:Ancient Ice Dragon", kIceDragonAncient);

	creatureInfo[kForestDragonAncient].bmp							=	4;
	creatureInfo[kForestDragonAncient].domain							=	kLand;
	creatureInfo[kForestDragonAncient].terrain							=	kGrass;
	creatureInfo[kForestDragonAncient].alignment						= kEvil;
	creatureInfo[kForestDragonAncient].directionalGraphics	= true;
	creatureInfo[kForestDragonAncient].graphicsIndex				= 1;
	creatureInfo[kForestDragonAncient].corpseID						= 134;
	creatureInfo[kForestDragonAncient].randomSound					= kDragonGrowl;
	creatureInfo[kForestDragonAncient].attackSound					= kDragonAttack;
	creatureInfo[kForestDragonAncient].attackRange					= 3;
	creatureInfo[kForestDragonAncient].desiredPopulation		= 2;
	creatureInfo[kForestDragonAncient].meatEater						= true;
	creatureInfo[kForestDragonAncient].points						  = 10;
	creatureInfo[kForestDragonAncient].rareLevel						= 1;
	creatureInfo[kForestDragonAncient].rareChance					= 100;
	creatureInfo[kForestDragonAncient].color						    = kForestDragonColor;
	creatureInfo[kForestDragonAncient].canMorph					  = true;
	creatureInfo[kForestDragonAncient].speed										=	fi_GetData(":Creature Settings:Ancient Forest Dragon", "SPEED=", 50);
	creatureInfo[kForestDragonAncient].poisonous					      = fi_GetData(":Creature Settings:Ancient Forest Dragon", "POISONOUS=", 0);
	creatureInfo[kForestDragonAncient].curse					      		= fi_GetData(":Creature Settings:Ancient Forest Dragon", "CURSE=", 0);
	creatureInfo[kForestDragonAncient].maxHitPoints						= fi_GetData(":Creature Settings:Ancient Forest Dragon", "HIT_POINTS=", 200);
	creatureInfo[kForestDragonAncient].maxStrength							= fi_GetData(":Creature Settings:Ancient Forest Dragon", "STRENGTH=", 200);
	creatureInfo[kForestDragonAncient].maxIntelligence					= fi_GetData(":Creature Settings:Ancient Forest Dragon", "INTELLIGENCE=", 200);
	creatureInfo[kForestDragonAncient].maxDexterity						= fi_GetData(":Creature Settings:Ancient Forest Dragon", "DEXTERITY=", 50);
	creatureInfo[kForestDragonAncient].minGold								  = fi_GetData(":Creature Settings:Ancient Forest Dragon", "MIN_GOLD=", 200);
	creatureInfo[kForestDragonAncient].maxGold								  = fi_GetData(":Creature Settings:Ancient Forest Dragon", "MAX_GOLD=", 500);
	creatureInfo[kForestDragonAncient].seekGold								= fi_GetBoolean(":Creature Settings:Ancient Forest Dragon", "SEEK_GOLD=", true);
	creatureInfo[kForestDragonAncient].seekMagic								= fi_GetBoolean(":Creature Settings:Ancient Forest Dragon", "SEEK_MAGIC=", false);
	creatureInfo[kForestDragonAncient].seekWeapons							= fi_GetBoolean(":Creature Settings:Ancient Forest Dragon", "SEEK_WEAPONS=", true);
	creatureInfo[kForestDragonAncient].magicResist				      = (float)fi_GetData(":Creature Settings:Ancient Forest Dragon", "MAGIC_RESIST=", 15)/100.0;
	creatureInfo[kForestDragonAncient].tamingLevel							= fi_GetData(":Creature Settings:Ancient Forest Dragon", "TAMING_LEVEL=", 8);
	creatureInfo[kForestDragonAncient].creatureLevel							= fi_GetData(":Creature Settings:Ancient Forest Dragon", "LEVEL=", 1);
	creatureInfo[kForestDragonAncient].armorRating							= fi_GetData(":Creature Settings:Ancient Forest Dragon", "ARMOR_RATING=", 75);
	creatureInfo[kForestDragonAncient].immuneToPoison          = fi_GetBoolean(":Creature Settings:Ancient Forest Dragon", "POISON_IMMUNITY=", false);
	creatureInfo[kForestDragonAncient].lockStrength						= fi_GetData(":Creature Settings:Ancient Forest Dragon", "LOCK=", 5);
	creatureInfo[kForestDragonAncient].attackDamageMin					= fi_GetData(":Creature Settings:Ancient Forest Dragon", "MIN_DAMAGE=", 45);
	creatureInfo[kForestDragonAncient].attackDamageMax					= fi_GetData(":Creature Settings:Ancient Forest Dragon", "MAX_DAMAGE=", 125);
	creatureInfo[kForestDragonAncient].trackingLevel							= fi_GetData(":Creature Settings:Ancient Forest Dragon", "TRACKING_LEVEL=", 101);
	creatureInfo[kForestDragonAncient].size									= fi_GetData(":Creature Settings:Ancient Forest Dragon", "SIZE=", 3);
	creatureInfo[kForestDragonAncient].mageryLevel					= fi_GetData(":Creature Settings:Ancient Forest Dragon", "MAGERY_LEVEL=", 15);
	creatureInfo[kForestDragonAncient].spellcastChance				= fi_GetData(":Creature Settings:Ancient Forest Dragon", "SPELLCAST_CHANCE=", 10);
	creatureInfo[kForestDragonAncient].selfhealChance					= fi_GetData(":Creature Settings:Ancient Forest Dragon", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kForestDragonAncient].selfhealTrigger				= fi_GetData(":Creature Settings:Ancient Forest Dragon", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Ancient Forest Dragon", kForestDragonAncient);
	fi_GetCreatureTalkScript(":Creature Settings:Ancient Forest Dragon", kForestDragonAncient);
	fi_GetCreatureInventory(":Creature Settings:Ancient Forest Dragon", kForestDragonAncient);
	fi_GetWeaponModifier(":Creature Settings:Ancient Forest Dragon", kForestDragonAncient);

	creatureInfo[kBlackDraco].bmp								=3;
	creatureInfo[kBlackDraco].domain								=	kLand;
	creatureInfo[kBlackDraco].terrain							=	kAnyTerrain;
	creatureInfo[kBlackDraco].alignment						= kEvil;
	creatureInfo[kBlackDraco].directionalGraphics	= true;
	creatureInfo[kBlackDraco].graphicsIndex				= 1;
	creatureInfo[kBlackDraco].corpseID							= 122;
	creatureInfo[kBlackDraco].randomSound					= kDracoGrowlSnd;
	creatureInfo[kBlackDraco].attackSound					= kDracoAttackSnd;
	creatureInfo[kBlackDraco].desiredPopulation		= 0;
	creatureInfo[kBlackDraco].meatEater						= true;
	creatureInfo[kBlackDraco].points						    = 10;
	creatureInfo[kBlackDraco].rareLevel						= 1;
	creatureInfo[kBlackDraco].rareChance						= 100;
	creatureInfo[kBlackDraco].color						    = kBlackDragonColor;
	creatureInfo[kBlackDraco].canMorph					    = true;
	creatureInfo[kBlackDraco].speed										=	fi_GetData(":Creature Settings:Black Draco", "SPEED=", 50);
	creatureInfo[kBlackDraco].poisonous					      = fi_GetData(":Creature Settings:Black Draco", "POISONOUS=", 0);
	creatureInfo[kBlackDraco].curse					      		= fi_GetData(":Creature Settings:Black Draco", "CURSE=", 0);
	creatureInfo[kBlackDraco].maxHitPoints							= fi_GetData(":Creature Settings:Black Draco", "HIT_POINTS=", 250);
	creatureInfo[kBlackDraco].maxStrength							= fi_GetData(":Creature Settings:Black Draco", "STRENGTH=", 250);
	creatureInfo[kBlackDraco].maxIntelligence					= fi_GetData(":Creature Settings:Black Draco", "INTELLIGENCE=", 250);
	creatureInfo[kBlackDraco].maxDexterity							= fi_GetData(":Creature Settings:Black Draco", "DEXTERITY=", 250);
	creatureInfo[kBlackDraco].minGold								  = fi_GetData(":Creature Settings:Black Draco", "MIN_GOLD=", 200);
	creatureInfo[kBlackDraco].maxGold								  = fi_GetData(":Creature Settings:Black Draco", "MAX_GOLD=", 500);
	creatureInfo[kBlackDraco].seekGold									= fi_GetBoolean(":Creature Settings:Black Draco", "SEEK_GOLD=", true);
	creatureInfo[kBlackDraco].seekMagic								= fi_GetBoolean(":Creature Settings:Black Draco", "SEEK_MAGIC=", false);
	creatureInfo[kBlackDraco].seekWeapons							= fi_GetBoolean(":Creature Settings:Black Draco", "SEEK_WEAPONS=", true);
	creatureInfo[kBlackDraco].magicResist				      = (float)fi_GetData(":Creature Settings:Black Draco", "MAGIC_RESIST=", 0)/100.0;
	creatureInfo[kBlackDraco].tamingLevel							= fi_GetData(":Creature Settings:Black Draco", "TAMING_LEVEL=", 101);
	creatureInfo[kBlackDraco].creatureLevel							= fi_GetData(":Creature Settings:Black Draco", "LEVEL=", 1);
	creatureInfo[kBlackDraco].armorRating							= fi_GetData(":Creature Settings:Black Draco", "ARMOR_RATING=", 100);
	creatureInfo[kBlackDraco].immuneToPoison           = fi_GetBoolean(":Creature Settings:Black Draco", "POISON_IMMUNITY=", true);
	creatureInfo[kBlackDraco].lockStrength							= fi_GetData(":Creature Settings:Black Draco", "LOCK=", 5);
	creatureInfo[kBlackDraco].attackDamageMin					= fi_GetData(":Creature Settings:Black Draco", "MIN_DAMAGE=", 250);
	creatureInfo[kBlackDraco].attackDamageMax					= fi_GetData(":Creature Settings:Black Draco", "MAX_DAMAGE=", 250);
	creatureInfo[kBlackDraco].trackingLevel							= fi_GetData(":Creature Settings:Black Draco", "TRACKING_LEVEL=", 101);
	creatureInfo[kBlackDraco].size									= fi_GetData(":Creature Settings:Black Draco", "SIZE=", 2);
	creatureInfo[kBlackDraco].mageryLevel					= fi_GetData(":Creature Settings:Black Draco", "MAGERY_LEVEL=", 0);
	creatureInfo[kBlackDraco].spellcastChance				= fi_GetData(":Creature Settings:Black Draco", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kBlackDraco].selfhealChance					= fi_GetData(":Creature Settings:Black Draco", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBlackDraco].selfhealTrigger				= fi_GetData(":Creature Settings:Black Draco", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Black Draco", kBlackDraco);
	fi_GetCreatureTalkScript(":Creature Settings:Black Draco", kBlackDraco);
	fi_GetCreatureInventory(":Creature Settings:Black Draco", kBlackDraco);
	fi_GetWeaponModifier(":Creature Settings:Black Draco", kBlackDraco);

	creatureInfo[kRedDraco].bmp								=3;
	creatureInfo[kRedDraco].domain								=	kLand;
	creatureInfo[kRedDraco].terrain							=	kAnyTerrain;
	creatureInfo[kRedDraco].alignment						= kEvil;
	creatureInfo[kRedDraco].directionalGraphics	= true;
	creatureInfo[kRedDraco].graphicsIndex				= 1;
	creatureInfo[kRedDraco].corpseID							= 104;
	creatureInfo[kRedDraco].randomSound					= kDracoGrowlSnd;
	creatureInfo[kRedDraco].attackSound					= kDracoAttackSnd;
	creatureInfo[kRedDraco].desiredPopulation		= 5;
	creatureInfo[kRedDraco].meatEater						= true;
	creatureInfo[kRedDraco].points						    = 10;
	creatureInfo[kRedDraco].rareLevel						= 1;
	creatureInfo[kRedDraco].rareChance						= 100;
	creatureInfo[kRedDraco].color						    = kRedDragonColor;
	creatureInfo[kRedDraco].canMorph					    = true;
	creatureInfo[kRedDraco].speed										=	fi_GetData(":Creature Settings:Red Draco", "SPEED=", 50);
	creatureInfo[kRedDraco].poisonous					      = fi_GetData(":Creature Settings:Red Draco", "POISONOUS=", 0);
	creatureInfo[kRedDraco].curse					      		= fi_GetData(":Creature Settings:Red Draco", "CURSE=", 0);
	creatureInfo[kRedDraco].maxHitPoints							= fi_GetData(":Creature Settings:Red Draco", "HIT_POINTS=", 200);
	creatureInfo[kRedDraco].maxStrength							= fi_GetData(":Creature Settings:Red Draco", "STRENGTH=", 200);
	creatureInfo[kRedDraco].maxIntelligence					= fi_GetData(":Creature Settings:Red Draco", "INTELLIGENCE=", 200);
	creatureInfo[kRedDraco].maxDexterity							= fi_GetData(":Creature Settings:Red Draco", "DEXTERITY=", 200);
	creatureInfo[kRedDraco].minGold								  = fi_GetData(":Creature Settings:Red Draco", "MIN_GOLD=", 200);
	creatureInfo[kRedDraco].maxGold								  = fi_GetData(":Creature Settings:Red Draco", "MAX_GOLD=", 500);
	creatureInfo[kRedDraco].seekGold									= fi_GetBoolean(":Creature Settings:Red Draco", "SEEK_GOLD=", true);
	creatureInfo[kRedDraco].seekMagic								= fi_GetBoolean(":Creature Settings:Red Draco", "SEEK_MAGIC=", false);
	creatureInfo[kRedDraco].seekWeapons							= fi_GetBoolean(":Creature Settings:Red Draco", "SEEK_WEAPONS=", true);
	creatureInfo[kRedDraco].magicResist				      = (float)fi_GetData(":Creature Settings:Red Draco", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kRedDraco].tamingLevel							= fi_GetData(":Creature Settings:Red Draco", "TAMING_LEVEL=", 8);
	creatureInfo[kRedDraco].creatureLevel							= fi_GetData(":Creature Settings:Red Draco", "LEVEL=", 1);
	creatureInfo[kRedDraco].armorRating							= fi_GetData(":Creature Settings:Red Draco", "ARMOR_RATING=", 75);
	creatureInfo[kRedDraco].immuneToPoison           = fi_GetBoolean(":Creature Settings:Red Draco", "POISON_IMMUNITY=", false);
	creatureInfo[kRedDraco].lockStrength							= fi_GetData(":Creature Settings:Red Draco", "LOCK=", 5);
	creatureInfo[kRedDraco].attackDamageMin					= fi_GetData(":Creature Settings:Red Draco", "MIN_DAMAGE=", 60);
	creatureInfo[kRedDraco].attackDamageMax					= fi_GetData(":Creature Settings:Red Draco", "MAX_DAMAGE=", 150);
	creatureInfo[kRedDraco].trackingLevel							= fi_GetData(":Creature Settings:Red Draco", "TRACKING_LEVEL=", 101);
	creatureInfo[kRedDraco].size									= fi_GetData(":Creature Settings:Red Draco", "SIZE=", 2);
	creatureInfo[kRedDraco].mageryLevel					= fi_GetData(":Creature Settings:Red Draco", "MAGERY_LEVEL=", 0);
	creatureInfo[kRedDraco].spellcastChance				= fi_GetData(":Creature Settings:Red Draco", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kRedDraco].selfhealChance					= fi_GetData(":Creature Settings:Red Draco", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kRedDraco].selfhealTrigger				= fi_GetData(":Creature Settings:Red Draco", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Red Draco", kRedDraco);
	fi_GetCreatureTalkScript(":Creature Settings:Red Draco", kRedDraco);
	fi_GetCreatureInventory(":Creature Settings:Red Draco", kRedDraco);
	fi_GetWeaponModifier(":Creature Settings:Red Draco", kRedDraco);

	creatureInfo[kIceDraco].bmp								=3;
	creatureInfo[kIceDraco].domain								=	kLand;
	creatureInfo[kIceDraco].terrain							=	kAnyTerrain;
	creatureInfo[kIceDraco].alignment						= kEvil;
	creatureInfo[kIceDraco].directionalGraphics	= true;
	creatureInfo[kIceDraco].graphicsIndex				= 1;
	creatureInfo[kIceDraco].corpseID							= 120;
	creatureInfo[kIceDraco].randomSound					= kDracoGrowlSnd;
	creatureInfo[kIceDraco].attackSound					= kDracoAttackSnd;
	creatureInfo[kIceDraco].desiredPopulation		= 8;
	creatureInfo[kIceDraco].meatEater						= true;
	creatureInfo[kIceDraco].points						    = 10;
	creatureInfo[kIceDraco].rareLevel						= 1;
	creatureInfo[kIceDraco].rareChance						= 100;
	creatureInfo[kIceDraco].color						    = kIceDragonColor;
	creatureInfo[kIceDraco].canMorph					    = true;
	creatureInfo[kIceDraco].frost					    	= true;
	creatureInfo[kIceDraco].speed										=	fi_GetData(":Creature Settings:Ice Draco", "SPEED=", 50);
	creatureInfo[kIceDraco].poisonous					      = fi_GetData(":Creature Settings:Ice Draco", "POISONOUS=", 0);
	creatureInfo[kIceDraco].curse					      		= fi_GetData(":Creature Settings:Ice Draco", "CURSE=", 0);
	creatureInfo[kIceDraco].maxHitPoints							= fi_GetData(":Creature Settings:Ice Draco", "HIT_POINTS=", 200);
	creatureInfo[kIceDraco].maxStrength							= fi_GetData(":Creature Settings:Ice Draco", "STRENGTH=", 200);
	creatureInfo[kIceDraco].maxIntelligence					= fi_GetData(":Creature Settings:Ice Draco", "INTELLIGENCE=", 200);
	creatureInfo[kIceDraco].maxDexterity							= fi_GetData(":Creature Settings:Ice Draco", "DEXTERITY=", 50);
	creatureInfo[kIceDraco].minGold								  = fi_GetData(":Creature Settings:Ice Draco", "MIN_GOLD=", 200);
	creatureInfo[kIceDraco].maxGold								  = fi_GetData(":Creature Settings:Ice Draco", "MAX_GOLD=", 500);
	creatureInfo[kIceDraco].seekGold									= fi_GetBoolean(":Creature Settings:Ice Draco", "SEEK_GOLD=", true);
	creatureInfo[kIceDraco].seekMagic								= fi_GetBoolean(":Creature Settings:Ice Draco", "SEEK_MAGIC=", false);
	creatureInfo[kIceDraco].seekWeapons							= fi_GetBoolean(":Creature Settings:Ice Draco", "SEEK_WEAPONS=", true);
	creatureInfo[kIceDraco].magicResist				      = (float)fi_GetData(":Creature Settings:Ice Draco", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kIceDraco].tamingLevel							= fi_GetData(":Creature Settings:Ice Draco", "TAMING_LEVEL=", 8);
	creatureInfo[kIceDraco].creatureLevel							= fi_GetData(":Creature Settings:Ice Draco", "LEVEL=", 1);
	creatureInfo[kIceDraco].armorRating							= fi_GetData(":Creature Settings:Ice Draco", "ARMOR_RATING=", 75);
	creatureInfo[kIceDraco].immuneToPoison           = fi_GetBoolean(":Creature Settings:Ice Draco", "POISON_IMMUNITY=", false);
	creatureInfo[kIceDraco].lockStrength							= fi_GetData(":Creature Settings:Ice Draco", "LOCK=", 5);
	creatureInfo[kIceDraco].attackDamageMin					= fi_GetData(":Creature Settings:Ice Draco", "MIN_DAMAGE=", 45);
	creatureInfo[kIceDraco].attackDamageMax					= fi_GetData(":Creature Settings:Ice Draco", "MAX_DAMAGE=", 125);
	creatureInfo[kIceDraco].trackingLevel							= fi_GetData(":Creature Settings:Ice Draco", "TRACKING_LEVEL=", 101);
	creatureInfo[kIceDraco].size									= fi_GetData(":Creature Settings:Ice Draco", "SIZE=", 2);
	creatureInfo[kIceDraco].mageryLevel					= fi_GetData(":Creature Settings:Ice Draco", "MAGERY_LEVEL=", 0);
	creatureInfo[kIceDraco].spellcastChance				= fi_GetData(":Creature Settings:Ice Draco", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kIceDraco].selfhealChance					= fi_GetData(":Creature Settings:Ice Draco", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kIceDraco].selfhealTrigger				= fi_GetData(":Creature Settings:Ice Draco", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Ice Draco", kIceDraco);
	fi_GetCreatureTalkScript(":Creature Settings:Ice Draco", kIceDraco);
	fi_GetCreatureInventory(":Creature Settings:Ice Draco", kIceDraco);
	fi_GetWeaponModifier(":Creature Settings:Ice Draco", kIceDraco);

	creatureInfo[kForestDraco].bmp								=3;
	creatureInfo[kForestDraco].domain							=	kLand;
	creatureInfo[kForestDraco].terrain							=	kGrass;
	creatureInfo[kForestDraco].alignment						= kEvil;
	creatureInfo[kForestDraco].directionalGraphics	= true;
	creatureInfo[kForestDraco].graphicsIndex				= 1;
	creatureInfo[kForestDraco].corpseID						= 121;
	creatureInfo[kForestDraco].randomSound					= kDracoGrowlSnd;
	creatureInfo[kForestDraco].attackSound					= kDracoAttackSnd;
	creatureInfo[kForestDraco].desiredPopulation		= 2;
	creatureInfo[kForestDraco].meatEater						= true;
	creatureInfo[kForestDraco].points						  = 10;
	creatureInfo[kForestDraco].rareLevel						= 1;
	creatureInfo[kForestDraco].rareChance					= 100;
	creatureInfo[kForestDraco].color						    = kForestDragonColor;
	creatureInfo[kForestDraco].canMorph					  = true;
	creatureInfo[kForestDraco].speed										=	fi_GetData(":Creature Settings:Forest Draco", "SPEED=", 50);
	creatureInfo[kForestDraco].poisonous					      = fi_GetData(":Creature Settings:Forest Draco", "POISONOUS=", 0);
	creatureInfo[kForestDraco].curse					      		= fi_GetData(":Creature Settings:Forest Draco", "CURSE=", 0);
	creatureInfo[kForestDraco].maxHitPoints						= fi_GetData(":Creature Settings:Forest Draco", "HIT_POINTS=", 200);
	creatureInfo[kForestDraco].maxStrength							= fi_GetData(":Creature Settings:Forest Draco", "STRENGTH=", 200);
	creatureInfo[kForestDraco].maxIntelligence					= fi_GetData(":Creature Settings:Forest Draco", "INTELLIGENCE=", 200);
	creatureInfo[kForestDraco].maxDexterity						= fi_GetData(":Creature Settings:Forest Draco", "DEXTERITY=", 50);
	creatureInfo[kForestDraco].minGold								  = fi_GetData(":Creature Settings:Forest Draco", "MIN_GOLD=", 200);
	creatureInfo[kForestDraco].maxGold								  = fi_GetData(":Creature Settings:Forest Draco", "MAX_GOLD=", 500);
	creatureInfo[kForestDraco].seekGold								= fi_GetBoolean(":Creature Settings:Forest Draco", "SEEK_GOLD=", true);
	creatureInfo[kForestDraco].seekMagic								= fi_GetBoolean(":Creature Settings:Forest Draco", "SEEK_MAGIC=", false);
	creatureInfo[kForestDraco].seekWeapons							= fi_GetBoolean(":Creature Settings:Forest Draco", "SEEK_WEAPONS=", true);
	creatureInfo[kForestDraco].magicResist				      = (float)fi_GetData(":Creature Settings:Forest Draco", "MAGIC_RESIST=", 15)/100.0;
	creatureInfo[kForestDraco].tamingLevel							= fi_GetData(":Creature Settings:Forest Draco", "TAMING_LEVEL=", 8);
	creatureInfo[kForestDraco].creatureLevel							= fi_GetData(":Creature Settings:Forest Draco", "LEVEL=", 1);
	creatureInfo[kForestDraco].armorRating							= fi_GetData(":Creature Settings:Forest Draco", "ARMOR_RATING=", 75);
	creatureInfo[kForestDraco].immuneToPoison          = fi_GetBoolean(":Creature Settings:Forest Draco", "POISON_IMMUNITY=", false);
	creatureInfo[kForestDraco].lockStrength						= fi_GetData(":Creature Settings:Forest Draco", "LOCK=", 5);
	creatureInfo[kForestDraco].attackDamageMin					= fi_GetData(":Creature Settings:Forest Draco", "MIN_DAMAGE=", 45);
	creatureInfo[kForestDraco].attackDamageMax					= fi_GetData(":Creature Settings:Forest Draco", "MAX_DAMAGE=", 125);
	creatureInfo[kForestDraco].trackingLevel							= fi_GetData(":Creature Settings:Forest Draco", "TRACKING_LEVEL=", 101);
	creatureInfo[kForestDraco].size									= fi_GetData(":Creature Settings:Forest Draco", "SIZE=", 2);
	creatureInfo[kForestDraco].mageryLevel					= fi_GetData(":Creature Settings:Forest Draco", "MAGERY_LEVEL=", 0);
	creatureInfo[kForestDraco].spellcastChance				= fi_GetData(":Creature Settings:Forest Draco", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kForestDraco].selfhealChance					= fi_GetData(":Creature Settings:Forest Draco", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kForestDraco].selfhealTrigger				= fi_GetData(":Creature Settings:Forest Draco", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Forest Draco", kForestDraco);
	fi_GetCreatureTalkScript(":Creature Settings:Forest Draco", kForestDraco);
	fi_GetCreatureInventory(":Creature Settings:Forest Draco", kForestDraco);
	fi_GetWeaponModifier(":Creature Settings:Forest Draco", kForestDraco);

	creatureInfo[kSandDraco].bmp								=3;
	creatureInfo[kSandDraco].domain							=	kLand;
	creatureInfo[kSandDraco].terrain							=	kGrass;
	creatureInfo[kSandDraco].alignment						= kEvil;
	creatureInfo[kSandDraco].directionalGraphics	= true;
	creatureInfo[kSandDraco].graphicsIndex				= 1;
	creatureInfo[kSandDraco].corpseID						= 195;
	creatureInfo[kSandDraco].randomSound					= kDracoGrowlSnd;
	creatureInfo[kSandDraco].attackSound					= kDracoAttackSnd;
	creatureInfo[kSandDraco].desiredPopulation		= 2;
	creatureInfo[kSandDraco].meatEater						= true;
	creatureInfo[kSandDraco].points						  = 10;
	creatureInfo[kSandDraco].rareLevel						= 1;
	creatureInfo[kSandDraco].rareChance					= 100;
	creatureInfo[kSandDraco].color						    = kSandSpiderColor;
	creatureInfo[kSandDraco].canMorph					  = true;
	creatureInfo[kSandDraco].speed										=	fi_GetData(":Creature Settings:Sand Draco", "SPEED=", 50);
	creatureInfo[kSandDraco].poisonous					      = fi_GetData(":Creature Settings:Sand Draco", "POISONOUS=", 0);
	creatureInfo[kSandDraco].curse					      		= fi_GetData(":Creature Settings:Sand Draco", "CURSE=", 0);
	creatureInfo[kSandDraco].maxHitPoints						= fi_GetData(":Creature Settings:Sand Draco", "HIT_POINTS=", 200);
	creatureInfo[kSandDraco].maxStrength							= fi_GetData(":Creature Settings:Sand Draco", "STRENGTH=", 200);
	creatureInfo[kSandDraco].maxIntelligence					= fi_GetData(":Creature Settings:Sand Draco", "INTELLIGENCE=", 200);
	creatureInfo[kSandDraco].maxDexterity						= fi_GetData(":Creature Settings:Sand Draco", "DEXTERITY=", 50);
	creatureInfo[kSandDraco].minGold								  = fi_GetData(":Creature Settings:Sand Draco", "MIN_GOLD=", 200);
	creatureInfo[kSandDraco].maxGold								  = fi_GetData(":Creature Settings:Sand Draco", "MAX_GOLD=", 500);
	creatureInfo[kSandDraco].seekGold								= fi_GetBoolean(":Creature Settings:Sand Draco", "SEEK_GOLD=", true);
	creatureInfo[kSandDraco].seekMagic								= fi_GetBoolean(":Creature Settings:Sand Draco", "SEEK_MAGIC=", false);
	creatureInfo[kSandDraco].seekWeapons							= fi_GetBoolean(":Creature Settings:Sand Draco", "SEEK_WEAPONS=", true);
	creatureInfo[kSandDraco].magicResist				      = (float)fi_GetData(":Creature Settings:Sand Draco", "MAGIC_RESIST=", 15)/100.0;
	creatureInfo[kSandDraco].tamingLevel							= fi_GetData(":Creature Settings:Sand Draco", "TAMING_LEVEL=", 8);
	creatureInfo[kSandDraco].creatureLevel							= fi_GetData(":Creature Settings:Sand Draco", "LEVEL=", 1);
	creatureInfo[kSandDraco].armorRating							= fi_GetData(":Creature Settings:Sand Draco", "ARMOR_RATING=", 75);
	creatureInfo[kSandDraco].immuneToPoison          = fi_GetBoolean(":Creature Settings:Sand Draco", "POISON_IMMUNITY=", false);
	creatureInfo[kSandDraco].lockStrength						= fi_GetData(":Creature Settings:Sand Draco", "LOCK=", 5);
	creatureInfo[kSandDraco].attackDamageMin					= fi_GetData(":Creature Settings:Sand Draco", "MIN_DAMAGE=", 45);
	creatureInfo[kSandDraco].attackDamageMax					= fi_GetData(":Creature Settings:Sand Draco", "MAX_DAMAGE=", 125);
	creatureInfo[kSandDraco].trackingLevel							= fi_GetData(":Creature Settings:Sand Draco", "TRACKING_LEVEL=", 101);
	creatureInfo[kSandDraco].size									= fi_GetData(":Creature Settings:Sand Draco", "SIZE=", 2);
	creatureInfo[kSandDraco].mageryLevel					= fi_GetData(":Creature Settings:Sand Draco", "MAGERY_LEVEL=", 0);
	creatureInfo[kSandDraco].spellcastChance				= fi_GetData(":Creature Settings:Sand Draco", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSandDraco].selfhealChance					= fi_GetData(":Creature Settings:Sand Draco", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSandDraco].selfhealTrigger				= fi_GetData(":Creature Settings:Sand Draco", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Sand Draco", kSandDraco);
	fi_GetCreatureTalkScript(":Creature Settings:Sand Draco", kSandDraco);
	fi_GetCreatureInventory(":Creature Settings:Sand Draco", kSandDraco);
	fi_GetWeaponModifier(":Creature Settings:Sand Draco", kSandDraco);

	creatureInfo[kCube].bmp											=	3;
	creatureInfo[kCube].domain										=	kLand;
	creatureInfo[kCube].terrain									=	kAnyTerrain;
	creatureInfo[kCube].alignment								= kEvil;
	creatureInfo[kCube].directionalGraphics			= true;
	creatureInfo[kCube].graphicsIndex						= 1;
	creatureInfo[kCube].corpseID									= 119;
	creatureInfo[kCube].desiredPopulation				= 60;
	creatureInfo[kCube].meatEater								= true;
	creatureInfo[kCube].canMorph					        = true;
	//creatureInfo[kCube].color						    = kWaterElementalColor;
	creatureInfo[kCube].attackSound							= kCubeAttack;
	creatureInfo[kCube].speed										=	fi_GetData(":Creature Settings:Gooey", "SPEED=", 50);
	creatureInfo[kCube].poisonous					      = fi_GetData(":Creature Settings:Gooey", "POISONOUS=", 0);
	creatureInfo[kCube].curse					      		= fi_GetData(":Creature Settings:Gooey", "CURSE=", 0);
	creatureInfo[kCube].maxHitPoints						= fi_GetData(":Creature Settings:Gooey", "HIT_POINTS=", 200);
	creatureInfo[kCube].maxStrength							= fi_GetData(":Creature Settings:Gooey", "STRENGTH=", 200);
	creatureInfo[kCube].maxIntelligence					= fi_GetData(":Creature Settings:Gooey", "INTELLIGENCE=", 200);
	creatureInfo[kCube].maxDexterity						= fi_GetData(":Creature Settings:Gooey", "DEXTERITY=", 50);
	creatureInfo[kCube].minGold								  = fi_GetData(":Creature Settings:Gooey", "MIN_GOLD=", 200);
	creatureInfo[kCube].maxGold								  = fi_GetData(":Creature Settings:Gooey", "MAX_GOLD=", 500);
	creatureInfo[kCube].seekGold								= fi_GetBoolean(":Creature Settings:Gooey", "SEEK_GOLD=", true);
	creatureInfo[kCube].seekMagic								= fi_GetBoolean(":Creature Settings:Gooey", "SEEK_MAGIC=", false);
	creatureInfo[kCube].seekWeapons							= fi_GetBoolean(":Creature Settings:Gooey", "SEEK_WEAPONS=", true);
	creatureInfo[kCube].magicResist				      = (float)fi_GetData(":Creature Settings:Gooey", "MAGIC_RESIST=", 15)/100.0;
	creatureInfo[kCube].tamingLevel							= fi_GetData(":Creature Settings:Gooey", "TAMING_LEVEL=", 8);
	creatureInfo[kCube].creatureLevel							= fi_GetData(":Creature Settings:Gooey", "LEVEL=", 1);
	creatureInfo[kCube].armorRating							= fi_GetData(":Creature Settings:Gooey", "ARMOR_RATING=", 75);
	creatureInfo[kCube].immuneToPoison          = fi_GetBoolean(":Creature Settings:Gooey", "POISON_IMMUNITY=", false);
	creatureInfo[kCube].lockStrength						= fi_GetData(":Creature Settings:Gooey", "LOCK=", 5);
	creatureInfo[kCube].attackDamageMin					= fi_GetData(":Creature Settings:Gooey", "MIN_DAMAGE=", 45);
	creatureInfo[kCube].attackDamageMax					= fi_GetData(":Creature Settings:Gooey", "MAX_DAMAGE=", 125);
	creatureInfo[kCube].trackingLevel							= fi_GetData(":Creature Settings:Gooey", "TRACKING_LEVEL=", 101);
	creatureInfo[kCube].size									= fi_GetData(":Creature Settings:Gooey", "SIZE=", 1);
	creatureInfo[kCube].mageryLevel					= fi_GetData(":Creature Settings:Gooey", "MAGERY_LEVEL=", 0);
	creatureInfo[kCube].spellcastChance				= fi_GetData(":Creature Settings:Gooey", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kCube].selfhealChance					= fi_GetData(":Creature Settings:Gooey", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kCube].selfhealTrigger				= fi_GetData(":Creature Settings:Gooey", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Gooey", kCube);
	fi_GetCreatureTalkScript(":Creature Settings:Gooey", kCube);
	fi_GetCreatureInventory(":Creature Settings:Gooey", kCube);
	fi_GetWeaponModifier(":Creature Settings:Gooey", kCube);

	creatureInfo[kSheep].bmp											=	0;
	creatureInfo[kSheep].domain										=	kLand;
	creatureInfo[kSheep].terrain									=	kGrass;
	creatureInfo[kSheep].alignment								= kNeutral;
	creatureInfo[kSheep].directionalGraphics			= true;
	creatureInfo[kSheep].graphicsIndex						= 1;
	creatureInfo[kSheep].corpseID									= 17;
	creatureInfo[kSheep].randomSound							= kSheepSound;
	creatureInfo[kSheep].attackSound							= kSheepAttack;
	creatureInfo[kSheep].vegEater									= true;
	creatureInfo[kSheep].desiredPopulation				= 2300;
	creatureInfo[kSheep].canMorph					        = true;
	creatureInfo[kSheep].speed										=	fi_GetData(":Creature Settings:Sheep", "SPEED=", 100);
	creatureInfo[kSheep].poisonous					      = fi_GetData(":Creature Settings:Sheep", "POISONOUS=", 0);
	creatureInfo[kSheep].curse					      		= fi_GetData(":Creature Settings:Sheep", "CURSE=", 0);
	creatureInfo[kSheep].maxHitPoints							= fi_GetData(":Creature Settings:Sheep", "HIT_POINTS=", 6);
	creatureInfo[kSheep].maxStrength							= fi_GetData(":Creature Settings:Sheep", "STRENGTH=", 6);
	creatureInfo[kSheep].maxIntelligence					= fi_GetData(":Creature Settings:Sheep", "INTELLIGENCE=", 20);
	creatureInfo[kSheep].maxDexterity							= fi_GetData(":Creature Settings:Sheep", "DEXTERITY=", 20);
	creatureInfo[kSheep].minGold								  = fi_GetData(":Creature Settings:Sheep", "MIN_GOLD=", 0);
	creatureInfo[kSheep].maxGold								  = fi_GetData(":Creature Settings:Sheep", "MAX_GOLD=", 0);
	creatureInfo[kSheep].seekGold									= fi_GetBoolean(":Creature Settings:Sheep", "SEEK_GOLD=", false);
	creatureInfo[kSheep].seekMagic								= fi_GetBoolean(":Creature Settings:Sheep", "SEEK_MAGIC=", false);
	creatureInfo[kSheep].seekWeapons							= fi_GetBoolean(":Creature Settings:Sheep", "SEEK_WEAPONS=", false);
	creatureInfo[kSheep].magicResist				      = (float)fi_GetData(":Creature Settings:Sheep", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kSheep].tamingLevel							= fi_GetData(":Creature Settings:Sheep", "TAMING_LEVEL=", 1);
	creatureInfo[kSheep].creatureLevel							= fi_GetData(":Creature Settings:Sheep", "LEVEL=", 1);
	creatureInfo[kSheep].armorRating							= fi_GetData(":Creature Settings:Sheep", "ARMOR_RATING=", 5);
	creatureInfo[kSheep].immuneToPoison           = fi_GetBoolean(":Creature Settings:Sheep", "POISON_IMMUNITY=", false);
	creatureInfo[kSheep].lockStrength							= fi_GetData(":Creature Settings:Sheep", "LOCK=", 0);
	creatureInfo[kSheep].attackDamageMin					= fi_GetData(":Creature Settings:Sheep", "MIN_DAMAGE=", 1);
	creatureInfo[kSheep].attackDamageMax					= fi_GetData(":Creature Settings:Sheep", "MAX_DAMAGE=", 3);
	creatureInfo[kSheep].trackingLevel							= fi_GetData(":Creature Settings:Sheep", "TRACKING_LEVEL=", 101);
	creatureInfo[kSheep].size									= fi_GetData(":Creature Settings:Sheep", "SIZE=", 1);
	creatureInfo[kSheep].mageryLevel					= fi_GetData(":Creature Settings:Sheep", "MAGERY_LEVEL=", 0);
	creatureInfo[kSheep].spellcastChance				= fi_GetData(":Creature Settings:Sheep", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSheep].selfhealChance					= fi_GetData(":Creature Settings:Sheep", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSheep].selfhealTrigger				= fi_GetData(":Creature Settings:Sheep", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Sheep", kSheep);
	//cr_AddToCreatureControlInventory(kSheep, kRawSteak, 2, 4);
	fi_GetCreatureTalkScript(":Creature Settings:Sheep", kSheep);
	fi_GetCreatureInventory(":Creature Settings:Sheep", kSheep);
	fi_GetWeaponModifier(":Creature Settings:Sheep", kSheep);

	creatureInfo[kScorpion].bmp										=	0;
	creatureInfo[kScorpion].domain								=	kLand;
	creatureInfo[kScorpion].terrain								=	kDesert;
	creatureInfo[kScorpion].alignment							= kEvil;
	creatureInfo[kScorpion].directionalGraphics		= true;
	creatureInfo[kScorpion].graphicsIndex					= 1;
	creatureInfo[kScorpion].corpseID							= 18;
	creatureInfo[kScorpion].desiredPopulation			= 300;
	creatureInfo[kScorpion].meatEater							= true;
	creatureInfo[kScorpion].points							  = 3;
	creatureInfo[kScorpion].canMorph					    = true;
	creatureInfo[kScorpion].speed										=	fi_GetData(":Creature Settings:Giant Scorpion", "SPEED=", 80);
	creatureInfo[kScorpion].poisonous					      = fi_GetData(":Creature Settings:Giant Scorpion", "POISONOUS=", 33);
	creatureInfo[kScorpion].curse					      		= fi_GetData(":Creature Settings:Giant Scorpion", "CURSE=", 0);
	creatureInfo[kScorpion].maxHitPoints						= fi_GetData(":Creature Settings:Giant Scorpion", "HIT_POINTS=", 60);
	creatureInfo[kScorpion].maxStrength							= fi_GetData(":Creature Settings:Giant Scorpion", "STRENGTH=", 60);
	creatureInfo[kScorpion].maxIntelligence					= fi_GetData(":Creature Settings:Giant Scorpion", "INTELLIGENCE=", 30);
	creatureInfo[kScorpion].maxDexterity						= fi_GetData(":Creature Settings:Giant Scorpion", "DEXTERITY=", 50);
	creatureInfo[kScorpion].minGold								  = fi_GetData(":Creature Settings:Giant Scorpion", "MIN_GOLD=", 0);
	creatureInfo[kScorpion].maxGold								  = fi_GetData(":Creature Settings:Giant Scorpion", "MAX_GOLD=", 0);
	creatureInfo[kScorpion].seekGold								= fi_GetBoolean(":Creature Settings:Giant Scorpion", "SEEK_GOLD=", false);
	creatureInfo[kScorpion].seekMagic								= fi_GetBoolean(":Creature Settings:Giant Scorpion", "SEEK_MAGIC=", false);
	creatureInfo[kScorpion].seekWeapons							= fi_GetBoolean(":Creature Settings:Giant Scorpion", "SEEK_WEAPONS=", false);
	creatureInfo[kScorpion].magicResist				      = (float)fi_GetData(":Creature Settings:Giant Scorpion", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kScorpion].tamingLevel							= fi_GetData(":Creature Settings:Giant Scorpion", "TAMING_LEVEL=", 3);
	creatureInfo[kScorpion].creatureLevel							= fi_GetData(":Creature Settings:Giant Scorpion", "LEVEL=", 1);
	creatureInfo[kScorpion].armorRating							= fi_GetData(":Creature Settings:Giant Scorpion", "ARMOR_RATING=", 30);
	creatureInfo[kScorpion].immuneToPoison          = fi_GetBoolean(":Creature Settings:Giant Scorpion", "POISON_IMMUNITY=", true);
	creatureInfo[kScorpion].lockStrength						= fi_GetData(":Creature Settings:Giant Scorpion", "LOCK=", 0);
	creatureInfo[kScorpion].attackDamageMin					= fi_GetData(":Creature Settings:Giant Scorpion", "MIN_DAMAGE=", 3);
	creatureInfo[kScorpion].attackDamageMax					= fi_GetData(":Creature Settings:Giant Scorpion", "MAX_DAMAGE=", 6);
	creatureInfo[kScorpion].trackingLevel							= fi_GetData(":Creature Settings:Giant Scorpion", "TRACKING_LEVEL=", 101);
	creatureInfo[kScorpion].size									= fi_GetData(":Creature Settings:Giant Scorpion", "SIZE=", 1);
	creatureInfo[kScorpion].mageryLevel					= fi_GetData(":Creature Settings:Giant Scorpion", "MAGERY_LEVEL=", 0);
	creatureInfo[kScorpion].spellcastChance				= fi_GetData(":Creature Settings:Giant Scorpion", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kScorpion].selfhealChance					= fi_GetData(":Creature Settings:Giant Scorpion", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kScorpion].selfhealTrigger				= fi_GetData(":Creature Settings:Giant Scorpion", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Giant Scorpion", kScorpion);
	//cr_AddToCreatureControlInventory(kScorpion, kPoisonGland, 2, 4);
	//cr_AddToCreatureControlInventory(kScorpion, kScorpionStinger, 1, 1);
	fi_GetCreatureTalkScript(":Creature Settings:Giant Scorpion", kScorpion);
	fi_GetCreatureInventory(":Creature Settings:Giant Scorpion", kScorpion);
	fi_GetWeaponModifier(":Creature Settings:Giant Scorpion", kScorpion);

	creatureInfo[kSpider].bmp										=	1;
	creatureInfo[kSpider].domain								=	kLand;
	creatureInfo[kSpider].terrain								=	kAnyTerrain;
	creatureInfo[kSpider].alignment							= kEvil;
	creatureInfo[kSpider].directionalGraphics		= true;
	creatureInfo[kSpider].graphicsIndex					= 1;
	creatureInfo[kSpider].corpseID							= 34;
	creatureInfo[kSpider].desiredPopulation			= 75;
	creatureInfo[kSpider].meatEater							= true;
	creatureInfo[kSpider].points							  = 4;
	creatureInfo[kSpider].canMorph					    = true;
	creatureInfo[kSpider].attackSound					  = kSpiderAttackSnd;
	creatureInfo[kSpider].speed										=	fi_GetData(":Creature Settings:Giant Spider", "SPEED=", 60);
	creatureInfo[kSpider].poisonous					      = fi_GetData(":Creature Settings:Giant Spider", "POISONOUS=", 33);
	creatureInfo[kSpider].curse					      		= fi_GetData(":Creature Settings:Giant Spider", "CURSE=", 0);
	creatureInfo[kSpider].maxHitPoints						= fi_GetData(":Creature Settings:Giant Spider", "HIT_POINTS=", 80);
	creatureInfo[kSpider].maxStrength							= fi_GetData(":Creature Settings:Giant Spider", "STRENGTH=", 80);
	creatureInfo[kSpider].maxIntelligence					= fi_GetData(":Creature Settings:Giant Spider", "INTELLIGENCE=", 30);
	creatureInfo[kSpider].maxDexterity						= fi_GetData(":Creature Settings:Giant Spider", "DEXTERITY=", 50);
	creatureInfo[kSpider].minGold								  = fi_GetData(":Creature Settings:Giant Spider", "MIN_GOLD=", 0);
	creatureInfo[kSpider].maxGold								  = fi_GetData(":Creature Settings:Giant Spider", "MAX_GOLD=", 0);
	creatureInfo[kSpider].seekGold								= fi_GetBoolean(":Creature Settings:Giant Spider", "SEEK_GOLD=", false);
	creatureInfo[kSpider].seekMagic								= fi_GetBoolean(":Creature Settings:Giant Spider", "SEEK_MAGIC=", false);
	creatureInfo[kSpider].seekWeapons							= fi_GetBoolean(":Creature Settings:Giant Spider", "SEEK_WEAPONS=", false);
	creatureInfo[kSpider].magicResist				      = (float)fi_GetData(":Creature Settings:Giant Spider", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kSpider].tamingLevel							= fi_GetData(":Creature Settings:Giant Spider", "TAMING_LEVEL=", 5);
	creatureInfo[kSpider].creatureLevel							= fi_GetData(":Creature Settings:Giant Spider", "LEVEL=", 1);
	creatureInfo[kSpider].armorRating							= fi_GetData(":Creature Settings:Giant Spider", "ARMOR_RATING=", 40);
	creatureInfo[kSpider].immuneToPoison          = fi_GetBoolean(":Creature Settings:Giant Spider", "POISON_IMMUNITY=", true);
	creatureInfo[kSpider].lockStrength						= fi_GetData(":Creature Settings:Giant Spider", "LOCK=", 0);
	creatureInfo[kSpider].attackDamageMin					= fi_GetData(":Creature Settings:Giant Spider", "MIN_DAMAGE=", 5);
	creatureInfo[kSpider].attackDamageMax					= fi_GetData(":Creature Settings:Giant Spider", "MAX_DAMAGE=", 10);
	creatureInfo[kSpider].trackingLevel							= fi_GetData(":Creature Settings:Giant Spider", "TRACKING_LEVEL=", 101);
	creatureInfo[kSpider].size									= fi_GetData(":Creature Settings:Giant Spider", "SIZE=", 2);
	creatureInfo[kSpider].mageryLevel					= fi_GetData(":Creature Settings:Giant Spider", "MAGERY_LEVEL=", 0);
	creatureInfo[kSpider].spellcastChance				= fi_GetData(":Creature Settings:Giant Spider", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSpider].selfhealChance					= fi_GetData(":Creature Settings:Giant Spider", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSpider].selfhealTrigger				= fi_GetData(":Creature Settings:Giant Spider", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Giant Spider", kSpider);
	fi_GetCreatureTalkScript(":Creature Settings:Giant Spider", kSpider);
	fi_GetCreatureInventory(":Creature Settings:Giant Spider", kSpider);
	fi_GetWeaponModifier(":Creature Settings:Giant Spider", kSpider);
	/*
		cr_AddToCreatureControlInventory(kSpider, kPoisonGland, 6, 10);
		cr_AddToCreatureControlInventory(kSpider, kSpiderFang, 2, 2);
		cr_AddToCreatureControlInventory(kSpider, kGreenCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSpider, kRedCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSpider, kBlueCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSpider, kPurpleCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSpider, kOrangeCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSpider, kYellowCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSpider, kFireCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSpider, kBlackCrystal, 0, 1);
	*/
	creatureInfo[kFrostSpider].bmp										=	1;
	creatureInfo[kFrostSpider].domain								  =	kLand;
	creatureInfo[kFrostSpider].terrain								=	kAnyTerrain;
	creatureInfo[kFrostSpider].alignment							= kEvil;
	creatureInfo[kFrostSpider].directionalGraphics		= true;
	creatureInfo[kFrostSpider].graphicsIndex					= 1;
	creatureInfo[kFrostSpider].corpseID							  = 35;
	creatureInfo[kFrostSpider].desiredPopulation			= 150;
	creatureInfo[kFrostSpider].meatEater							= true;
	creatureInfo[kFrostSpider].points							    = 4;
	creatureInfo[kFrostSpider].color						      = kIceDragonColor;
	creatureInfo[kFrostSpider].canMorph					      = true;
	creatureInfo[kFrostSpider].frost					      	= true;
	creatureInfo[kFrostSpider].attackSound					  = kSpiderAttackSnd;
	creatureInfo[kFrostSpider].speed									=	fi_GetData(":Creature Settings:Frost Spider", "SPEED=", 60);
	creatureInfo[kFrostSpider].poisonous					    = fi_GetData(":Creature Settings:Frost Spider", "POISONOUS=", 43);
	creatureInfo[kFrostSpider].curse					    		= fi_GetData(":Creature Settings:Frost Spider", "CURSE=", 0);
	creatureInfo[kFrostSpider].maxHitPoints						= fi_GetData(":Creature Settings:Frost Spider", "HIT_POINTS=", 80);
	creatureInfo[kFrostSpider].maxStrength						= fi_GetData(":Creature Settings:Frost Spider", "STRENGTH=", 80);
	creatureInfo[kFrostSpider].maxIntelligence				= fi_GetData(":Creature Settings:Frost Spider", "INTELLIGENCE=", 30);
	creatureInfo[kFrostSpider].maxDexterity						= fi_GetData(":Creature Settings:Frost Spider", "DEXTERITY=", 50);
	creatureInfo[kFrostSpider].minGold								= fi_GetData(":Creature Settings:Frost Spider", "MIN_GOLD=", 0);
	creatureInfo[kFrostSpider].maxGold								= fi_GetData(":Creature Settings:Frost Spider", "MAX_GOLD=", 0);
	creatureInfo[kFrostSpider].seekGold								= fi_GetBoolean(":Creature Settings:Frost Spider", "SEEK_GOLD=", false);
	creatureInfo[kFrostSpider].seekMagic							= fi_GetBoolean(":Creature Settings:Frost Spider", "SEEK_MAGIC=", false);
	creatureInfo[kFrostSpider].seekWeapons						= fi_GetBoolean(":Creature Settings:Frost Spider", "SEEK_WEAPONS=", false);
	creatureInfo[kFrostSpider].magicResist				    = (float)fi_GetData(":Creature Settings:Frost Spider", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kFrostSpider].tamingLevel						= fi_GetData(":Creature Settings:Frost Spider", "TAMING_LEVEL=", 5);
	creatureInfo[kFrostSpider].creatureLevel						= fi_GetData(":Creature Settings:Frost Spider", "LEVEL=", 1);
	creatureInfo[kFrostSpider].armorRating						= fi_GetData(":Creature Settings:Frost Spider", "ARMOR_RATING=", 40);
	creatureInfo[kFrostSpider].immuneToPoison         = fi_GetBoolean(":Creature Settings:Frost Spider", "POISON_IMMUNITY=", true);
	creatureInfo[kFrostSpider].lockStrength						= fi_GetData(":Creature Settings:Frost Spider", "LOCK=", 0);
	creatureInfo[kFrostSpider].attackDamageMin				= fi_GetData(":Creature Settings:Frost Spider", "MIN_DAMAGE=", 5);
	creatureInfo[kFrostSpider].attackDamageMax				= fi_GetData(":Creature Settings:Frost Spider", "MAX_DAMAGE=", 18);
	creatureInfo[kFrostSpider].trackingLevel							= fi_GetData(":Creature Settings:Frost Spider", "TRACKING_LEVEL=", 101);
	creatureInfo[kFrostSpider].size									= fi_GetData(":Creature Settings:Frost Spider", "SIZE=", 2);
	creatureInfo[kFrostSpider].mageryLevel					= fi_GetData(":Creature Settings:Frost Spider", "MAGERY_LEVEL=", 0);
	creatureInfo[kFrostSpider].spellcastChance				= fi_GetData(":Creature Settings:Frost Spider", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kFrostSpider].selfhealChance					= fi_GetData(":Creature Settings:Frost Spider", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kFrostSpider].selfhealTrigger				= fi_GetData(":Creature Settings:Frost Spider", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Frost Spider", kFrostSpider);
	fi_GetCreatureTalkScript(":Creature Settings:Frost Spider", kFrostSpider);
	fi_GetCreatureInventory(":Creature Settings:Frost Spider", kFrostSpider);
	fi_GetWeaponModifier(":Creature Settings:Frost Spider", kFrostSpider);
	/*
		cr_AddToCreatureControlInventory(kFrostSpider, kPoisonGland, 6, 10);
		cr_AddToCreatureControlInventory(kFrostSpider, kSpiderFang, 2, 2);
		cr_AddToCreatureControlInventory(kFrostSpider, kGreenCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kFrostSpider, kRedCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kFrostSpider, kBlueCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kFrostSpider, kPurpleCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kFrostSpider, kOrangeCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kFrostSpider, kYellowCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kFrostSpider, kFireCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kFrostSpider, kBlackCrystal, 0, 1);
	*/

	creatureInfo[kSandSpider].bmp										  =	1;
	creatureInfo[kSandSpider].domain								  =	kLand;
	creatureInfo[kSandSpider].terrain								  =	kDesert;
	creatureInfo[kSandSpider].alignment							  = kEvil;
	creatureInfo[kSandSpider].directionalGraphics		  = true;
	creatureInfo[kSandSpider].graphicsIndex					  = 1;
	creatureInfo[kSandSpider].corpseID							  = 71;
	creatureInfo[kSandSpider].desiredPopulation			  = 40;
	creatureInfo[kSandSpider].meatEater							  = true;
	creatureInfo[kSandSpider].points							    = 4;
	creatureInfo[kSandSpider].color						        = kSandSpiderColor;
	creatureInfo[kSandSpider].canMorph					      = true;
	creatureInfo[kSandSpider].attackSound					    = kSpiderAttackSnd;
	creatureInfo[kSandSpider].speed									=	fi_GetData(":Creature Settings:Sand Spider", "SPEED=", 60);
	creatureInfo[kSandSpider].poisonous					    = fi_GetData(":Creature Settings:Sand Spider", "POISONOUS=", 33);
	creatureInfo[kSandSpider].curse					    		= fi_GetData(":Creature Settings:Sand Spider", "CURSE=", 0);
	creatureInfo[kSandSpider].maxHitPoints					= fi_GetData(":Creature Settings:Sand Spider", "HIT_POINTS=", 80);
	creatureInfo[kSandSpider].maxStrength						= fi_GetData(":Creature Settings:Sand Spider", "STRENGTH=", 80);
	creatureInfo[kSandSpider].maxIntelligence				= fi_GetData(":Creature Settings:Sand Spider", "INTELLIGENCE=", 30);
	creatureInfo[kSandSpider].maxDexterity					= fi_GetData(":Creature Settings:Sand Spider", "DEXTERITY=", 50);
	creatureInfo[kSandSpider].minGold								= fi_GetData(":Creature Settings:Sand Spider", "MIN_GOLD=", 0);
	creatureInfo[kSandSpider].maxGold								= fi_GetData(":Creature Settings:Sand Spider", "MAX_GOLD=", 0);
	creatureInfo[kSandSpider].seekGold							= fi_GetBoolean(":Creature Settings:Sand Spider", "SEEK_GOLD=", false);
	creatureInfo[kSandSpider].seekMagic							= fi_GetBoolean(":Creature Settings:Sand Spider", "SEEK_MAGIC=", false);
	creatureInfo[kSandSpider].seekWeapons						= fi_GetBoolean(":Creature Settings:Sand Spider", "SEEK_WEAPONS=", false);
	creatureInfo[kSandSpider].magicResist				    = (float)fi_GetData(":Creature Settings:Sand Spider", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kSandSpider].tamingLevel						= fi_GetData(":Creature Settings:Sand Spider", "TAMING_LEVEL=", 5);
	creatureInfo[kSandSpider].creatureLevel						= fi_GetData(":Creature Settings:Sand Spider", "LEVEL=", 1);
	creatureInfo[kSandSpider].armorRating						= fi_GetData(":Creature Settings:Sand Spider", "ARMOR_RATING=", 40);
	creatureInfo[kSandSpider].immuneToPoison        = fi_GetBoolean(":Creature Settings:Sand Spider", "POISON_IMMUNITY=", true);
	creatureInfo[kSandSpider].lockStrength					= fi_GetData(":Creature Settings:Sand Spider", "LOCK=", 0);
	creatureInfo[kSandSpider].attackDamageMin				= fi_GetData(":Creature Settings:Sand Spider", "MIN_DAMAGE=", 5);
	creatureInfo[kSandSpider].attackDamageMax				= fi_GetData(":Creature Settings:Sand Spider", "MAX_DAMAGE=", 18);
	creatureInfo[kSandSpider].trackingLevel							= fi_GetData(":Creature Settings:Sand Spider", "TRACKING_LEVEL=", 101);
	creatureInfo[kSandSpider].size									= fi_GetData(":Creature Settings:Sand Spider", "SIZE=", 2);
	creatureInfo[kSandSpider].mageryLevel					= fi_GetData(":Creature Settings:Sand Spider", "MAGERY_LEVEL=", 0);
	creatureInfo[kSandSpider].spellcastChance				= fi_GetData(":Creature Settings:Sand Spider", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSandSpider].selfhealChance					= fi_GetData(":Creature Settings:Sand Spider", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSandSpider].selfhealTrigger				= fi_GetData(":Creature Settings:Sand Spider", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Sand Spider", kSandSpider);
	fi_GetCreatureTalkScript(":Creature Settings:Sand Spider", kSandSpider);
	fi_GetCreatureInventory(":Creature Settings:Sand Spider", kSandSpider);
	fi_GetWeaponModifier(":Creature Settings:Sand Spider", kSandSpider);

	/*	cr_AddToCreatureControlInventory(kSandSpider, kPoisonGland, 6, 10);
		cr_AddToCreatureControlInventory(kSandSpider, kSpiderFang, 2, 2);
		cr_AddToCreatureControlInventory(kSandSpider, kGreenCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSandSpider, kRedCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSandSpider, kBlueCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSandSpider, kPurpleCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSandSpider, kOrangeCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSandSpider, kYellowCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSandSpider, kFireCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kSandSpider, kBlackCrystal, 0, 1);
	*/

	creatureInfo[kLavaSpider].bmp										=	1;
	creatureInfo[kLavaSpider].domain								=	kLand;
	creatureInfo[kLavaSpider].terrain								=	kAnyTerrain;
	creatureInfo[kLavaSpider].alignment							= kEvil;
	creatureInfo[kLavaSpider].directionalGraphics		= true;
	creatureInfo[kLavaSpider].graphicsIndex					= 1;
	creatureInfo[kLavaSpider].corpseID							= 36;
	creatureInfo[kLavaSpider].desiredPopulation			= 130;
	creatureInfo[kLavaSpider].meatEater							= true;
	creatureInfo[kLavaSpider].points							  = 4;
	creatureInfo[kLavaSpider].color						      = kRedDragonColor;
	creatureInfo[kLavaSpider].canMorph					    = true;
	creatureInfo[kLavaSpider].attackSound					  = kSpiderAttackSnd;
	creatureInfo[kLavaSpider].fire								  = true;
	creatureInfo[kLavaSpider].speed									=	fi_GetData(":Creature Settings:Lava Spider", "SPEED=", 50);
	creatureInfo[kLavaSpider].poisonous					    = fi_GetData(":Creature Settings:Lava Spider", "POISONOUS=", 43);
	creatureInfo[kLavaSpider].curse					    		= fi_GetData(":Creature Settings:Lava Spider", "CURSE=", 0);
	creatureInfo[kLavaSpider].maxHitPoints					= fi_GetData(":Creature Settings:Lava Spider", "HIT_POINTS=", 90);
	creatureInfo[kLavaSpider].maxStrength						= fi_GetData(":Creature Settings:Lava Spider", "STRENGTH=", 90);
	creatureInfo[kLavaSpider].maxIntelligence				= fi_GetData(":Creature Settings:Lava Spider", "INTELLIGENCE=", 30);
	creatureInfo[kLavaSpider].maxDexterity					= fi_GetData(":Creature Settings:Lava Spider", "DEXTERITY=", 50);
	creatureInfo[kLavaSpider].minGold								= fi_GetData(":Creature Settings:Lava Spider", "MIN_GOLD=", 0);
	creatureInfo[kLavaSpider].maxGold								= fi_GetData(":Creature Settings:Lava Spider", "MAX_GOLD=", 0);
	creatureInfo[kLavaSpider].seekGold							= fi_GetBoolean(":Creature Settings:Lava Spider", "SEEK_GOLD=", false);
	creatureInfo[kLavaSpider].seekMagic							= fi_GetBoolean(":Creature Settings:Lava Spider", "SEEK_MAGIC=", false);
	creatureInfo[kLavaSpider].seekWeapons						= fi_GetBoolean(":Creature Settings:Lava Spider", "SEEK_WEAPONS=", false);
	creatureInfo[kLavaSpider].magicResist				    = (float)fi_GetData(":Creature Settings:Lava Spider", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kLavaSpider].tamingLevel						= fi_GetData(":Creature Settings:Lava Spider", "TAMING_LEVEL=", 6);
	creatureInfo[kLavaSpider].creatureLevel						= fi_GetData(":Creature Settings:Lava Spider", "LEVEL=", 1);
	creatureInfo[kLavaSpider].armorRating						= fi_GetData(":Creature Settings:Lava Spider", "ARMOR_RATING=", 50);
	creatureInfo[kLavaSpider].immuneToPoison        = fi_GetBoolean(":Creature Settings:Lava Spider", "POISON_IMMUNITY=", true);
	creatureInfo[kLavaSpider].lockStrength					= fi_GetData(":Creature Settings:Lava Spider", "LOCK=", 0);
	creatureInfo[kLavaSpider].attackDamageMin				= fi_GetData(":Creature Settings:Lava Spider", "MIN_DAMAGE=", 7);
	creatureInfo[kLavaSpider].attackDamageMax				= fi_GetData(":Creature Settings:Lava Spider", "MAX_DAMAGE=", 23);
	creatureInfo[kLavaSpider].trackingLevel							= fi_GetData(":Creature Settings:Lava Spider", "TRACKING_LEVEL=", 101);
	creatureInfo[kLavaSpider].size									= fi_GetData(":Creature Settings:Lava Spider", "SIZE=", 2);
	creatureInfo[kLavaSpider].mageryLevel					= fi_GetData(":Creature Settings:Lava Spider", "MAGERY_LEVEL=", 0);
	creatureInfo[kLavaSpider].spellcastChance				= fi_GetData(":Creature Settings:Lava Spider", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kLavaSpider].selfhealChance					= fi_GetData(":Creature Settings:Lava Spider", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kLavaSpider].selfhealTrigger				= fi_GetData(":Creature Settings:Lava Spider", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Lava Spider", kLavaSpider);
	fi_GetCreatureTalkScript(":Creature Settings:Lava Spider", kLavaSpider);
	fi_GetCreatureInventory(":Creature Settings:Lava Spider", kLavaSpider);
	fi_GetWeaponModifier(":Creature Settings:Lava Spider", kLavaSpider);

	/*	cr_AddToCreatureControlInventory(kLavaSpider, kPoisonGland, 6, 10);
		cr_AddToCreatureControlInventory(kLavaSpider, kVolcanicAsh, 4, 12);
		cr_AddToCreatureControlInventory(kLavaSpider, kSpiderFang, 2, 2);
		cr_AddToCreatureControlInventory(kLavaSpider, kGreenCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kLavaSpider, kRedCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kLavaSpider, kBlueCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kLavaSpider, kPurpleCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kLavaSpider, kOrangeCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kLavaSpider, kYellowCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kLavaSpider, kFireCrystal, 0, 1);
		cr_AddToCreatureControlInventory(kLavaSpider, kBlackCrystal, 0, 1);
	*/

	creatureInfo[kHarpie].bmp										=	0;
	creatureInfo[kHarpie].domain								=	kAir;
	creatureInfo[kHarpie].alignment							= kEvil;
	creatureInfo[kHarpie].directionalGraphics		= true;
	creatureInfo[kHarpie].graphicsIndex					= 1;
	creatureInfo[kHarpie].corpseID							= 19;
	creatureInfo[kHarpie].desiredPopulation			= 1500;
	creatureInfo[kHarpie].attackSound						= kHarpieAttack;
	creatureInfo[kHarpie].meatEater							= true;
	creatureInfo[kHarpie].points							  = 3;
	creatureInfo[kHarpie].canMorph					    = true;
	creatureInfo[kHarpie].speed										=	fi_GetData(":Creature Settings:Harpie", "SPEED=", 60);
	creatureInfo[kHarpie].poisonous					      = fi_GetData(":Creature Settings:Harpie", "POISONOUS=", 0);
	creatureInfo[kHarpie].curse					      		= fi_GetData(":Creature Settings:Harpie", "CURSE=", 0);
	creatureInfo[kHarpie].maxHitPoints						= fi_GetData(":Creature Settings:Harpie", "HIT_POINTS=", 60);
	creatureInfo[kHarpie].maxStrength							= fi_GetData(":Creature Settings:Harpie", "STRENGTH=", 60);
	creatureInfo[kHarpie].maxIntelligence					= fi_GetData(":Creature Settings:Harpie", "INTELLIGENCE=", 50);
	creatureInfo[kHarpie].maxDexterity						= fi_GetData(":Creature Settings:Harpie", "DEXTERITY=", 50);
	creatureInfo[kHarpie].minGold								  = fi_GetData(":Creature Settings:Harpie", "MIN_GOLD=", 10);
	creatureInfo[kHarpie].maxGold								  = fi_GetData(":Creature Settings:Harpie", "MAX_GOLD=", 18);
	creatureInfo[kHarpie].seekGold								= fi_GetBoolean(":Creature Settings:Harpie", "SEEK_GOLD=", true);
	creatureInfo[kHarpie].seekMagic								= fi_GetBoolean(":Creature Settings:Harpie", "SEEK_MAGIC=", false);
	creatureInfo[kHarpie].seekWeapons							= fi_GetBoolean(":Creature Settings:Harpie", "SEEK_WEAPONS=", false);
	creatureInfo[kHarpie].magicResist				      = (float)fi_GetData(":Creature Settings:Harpie", "MAGIC_RESIST=", 80)/100.0;
	creatureInfo[kHarpie].tamingLevel							= fi_GetData(":Creature Settings:Harpie", "TAMING_LEVEL=", 4);
	creatureInfo[kHarpie].creatureLevel							= fi_GetData(":Creature Settings:Harpie", "LEVEL=", 1);
	creatureInfo[kHarpie].armorRating							= fi_GetData(":Creature Settings:Harpie", "ARMOR_RATING=", 30);
	creatureInfo[kHarpie].immuneToPoison          = fi_GetBoolean(":Creature Settings:Harpie", "POISON_IMMUNITY=", false);
	creatureInfo[kHarpie].lockStrength						= fi_GetData(":Creature Settings:Harpie", "LOCK=", 0);
	creatureInfo[kHarpie].attackDamageMin					= fi_GetData(":Creature Settings:Harpie", "MIN_DAMAGE=", 4);
	creatureInfo[kHarpie].attackDamageMax					= fi_GetData(":Creature Settings:Harpie", "MAX_DAMAGE=", 7);
	creatureInfo[kHarpie].trackingLevel							= fi_GetData(":Creature Settings:Harpie", "TRACKING_LEVEL=", 101);
	creatureInfo[kHarpie].size									= fi_GetData(":Creature Settings:Harpie", "SIZE=", 1);
	creatureInfo[kHarpie].mageryLevel					= fi_GetData(":Creature Settings:Harpie", "MAGERY_LEVEL=", 0);
	creatureInfo[kHarpie].spellcastChance				= fi_GetData(":Creature Settings:Harpie", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kHarpie].selfhealChance					= fi_GetData(":Creature Settings:Harpie", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kHarpie].selfhealTrigger				= fi_GetData(":Creature Settings:Harpie", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Harpie", kHarpie);
	fi_GetCreatureTalkScript(":Creature Settings:Harpie", kHarpie);
	fi_GetCreatureInventory(":Creature Settings:Harpie", kHarpie);
	fi_GetWeaponModifier(":Creature Settings:Harpie", kHarpie);

//	cr_AddToCreatureControlInventory(kHarpie, kRawSteak, 2, 4);
//	cr_AddToCreatureControlInventory(kHarpie, kFeather, 30, 50);
//	cr_AddToCreatureControlInventory(kHarpie, kApple, 0, 2);

	creatureInfo[kRat].bmp											=	0;
	creatureInfo[kRat].domain										=	kLand;
	creatureInfo[kRat].terrain									=	kAnyTerrain;
	creatureInfo[kRat].alignment								= kEvil;
	creatureInfo[kRat].directionalGraphics			= true;
	creatureInfo[kRat].graphicsIndex						= 1;
	creatureInfo[kRat].corpseID									= 25;
	creatureInfo[kRat].desiredPopulation				= 600;
	creatureInfo[kRat].meatEater								= true;
	creatureInfo[kRat].randomSound							= kRatSqueak;
	creatureInfo[kRat].attackSound							= kRatAttack;
	creatureInfo[kRat].canMorph					        = true;
	creatureInfo[kRat].speed										=	fi_GetData(":Creature Settings:Giant Rat", "SPEED=", 90);
	creatureInfo[kRat].poisonous					      = fi_GetData(":Creature Settings:Giant Rat", "POISONOUS=", 0);
	creatureInfo[kRat].curse					      		= fi_GetData(":Creature Settings:Giant Rat", "CURSE=", 0);
	creatureInfo[kRat].maxHitPoints							= fi_GetData(":Creature Settings:Giant Rat", "HIT_POINTS=", 5);
	creatureInfo[kRat].maxStrength							= fi_GetData(":Creature Settings:Giant Rat", "STRENGTH=", 5);
	creatureInfo[kRat].maxIntelligence					= fi_GetData(":Creature Settings:Giant Rat", "INTELLIGENCE=", 5);
	creatureInfo[kRat].maxDexterity							= fi_GetData(":Creature Settings:Giant Rat", "DEXTERITY=", 10);
	creatureInfo[kRat].minGold								  = fi_GetData(":Creature Settings:Giant Rat", "MIN_GOLD=", 0);
	creatureInfo[kRat].maxGold								  = fi_GetData(":Creature Settings:Giant Rat", "MAX_GOLD=", 2);
	creatureInfo[kRat].seekGold									= fi_GetBoolean(":Creature Settings:Giant Rat", "SEEK_GOLD=", true);
	creatureInfo[kRat].seekMagic								= fi_GetBoolean(":Creature Settings:Giant Rat", "SEEK_MAGIC=", false);
	creatureInfo[kRat].seekWeapons							= fi_GetBoolean(":Creature Settings:Giant Rat", "SEEK_WEAPONS=", false);
	creatureInfo[kRat].magicResist				      = (float)fi_GetData(":Creature Settings:Giant Rat", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kRat].tamingLevel							= fi_GetData(":Creature Settings:Giant Rat", "TAMING_LEVEL=", 1);
	creatureInfo[kRat].creatureLevel							= fi_GetData(":Creature Settings:Giant Rat", "TAMING_LEVEL=", 1);
	creatureInfo[kRat].armorRating							= fi_GetData(":Creature Settings:Giant Rat", "ARMOR_RATING=", 10);
	creatureInfo[kRat].immuneToPoison           = fi_GetBoolean(":Creature Settings:Giant Rat", "POISON_IMMUNITY=", false);
	creatureInfo[kRat].lockStrength							= fi_GetData(":Creature Settings:Giant Rat", "LOCK=", 0);
	creatureInfo[kRat].attackDamageMin					= fi_GetData(":Creature Settings:Giant Rat", "MIN_DAMAGE=", 1);
	creatureInfo[kRat].attackDamageMax					= fi_GetData(":Creature Settings:Giant Rat", "MAX_DAMAGE=", 3);
	creatureInfo[kRat].trackingLevel							= fi_GetData(":Creature Settings:Giant Rat", "TRACKING_LEVEL=", 101);
	creatureInfo[kRat].size									= fi_GetData(":Creature Settings:Giant Rat", "SIZE=", 0);
	creatureInfo[kRat].mageryLevel					= fi_GetData(":Creature Settings:Giant Rat", "MAGERY_LEVEL=", 0);
	creatureInfo[kRat].spellcastChance				= fi_GetData(":Creature Settings:Giant Rat", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kRat].selfhealChance					= fi_GetData(":Creature Settings:Giant Rat", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kRat].selfhealTrigger				= fi_GetData(":Creature Settings:Giant Rat", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Giant Rat", kRat);
//	cr_AddToCreatureControlInventory(kRat, kFurSkin, 1, 1);
//	cr_AddToCreatureControlInventory(kRat, kRawSteak, 1, 1);
//	cr_AddToCreatureControlInventory(kRat, kRatTail, 1, 1);
	fi_GetCreatureTalkScript(":Creature Settings:Giant Rat", kRat);
	fi_GetCreatureInventory(":Creature Settings:Giant Rat", kRat);
	fi_GetWeaponModifier(":Creature Settings:Giant Rat", kRat);

	creatureInfo[kGiant].bmp											=	2;
	creatureInfo[kGiant].domain										=	kLand;
	creatureInfo[kGiant].terrain									=	kAnyTerrain;
	creatureInfo[kGiant].alignment								= kEvil;
	creatureInfo[kGiant].directionalGraphics			= true;
	creatureInfo[kGiant].graphicsIndex						= 1;
	creatureInfo[kGiant].corpseID									= 40;
	creatureInfo[kGiant].desiredPopulation				= 100;
	creatureInfo[kGiant].meatEater								= true;
	creatureInfo[kGiant].canMorph					        = true;
	creatureInfo[kGiant].rareLevel								= 5;
	creatureInfo[kGiant].rareChance					  		= 50;
	creatureInfo[kGiant].points			          		= 4;
	creatureInfo[kGiant].randomSound							= kGiantSnd;
	creatureInfo[kGiant].blunt										= kBluntAttackSnd;
	creatureInfo[kGiant].speed										=	fi_GetData(":Creature Settings:Giant", "SPEED=", 75);
	creatureInfo[kGiant].poisonous					      = fi_GetData(":Creature Settings:Giant", "POISONOUS=", 0);
	creatureInfo[kGiant].curse					      		= fi_GetData(":Creature Settings:Giant", "CURSE=", 0);
	creatureInfo[kGiant].maxHitPoints							= fi_GetData(":Creature Settings:Giant", "HIT_POINTS=", 150);
	creatureInfo[kGiant].maxStrength							= fi_GetData(":Creature Settings:Giant", "STRENGTH=", 150);
	creatureInfo[kGiant].maxIntelligence					= fi_GetData(":Creature Settings:Giant", "INTELLIGENCE=", 50);
	creatureInfo[kGiant].maxDexterity							= fi_GetData(":Creature Settings:Giant", "DEXTERITY=", 50);
	creatureInfo[kGiant].minGold								  = fi_GetData(":Creature Settings:Giant", "MIN_GOLD=", 50);
	creatureInfo[kGiant].maxGold								  = fi_GetData(":Creature Settings:Giant", "MAX_GOLD=", 75);
	creatureInfo[kGiant].seekGold									= fi_GetBoolean(":Creature Settings:Giant", "SEEK_GOLD=", true);
	creatureInfo[kGiant].seekMagic								= fi_GetBoolean(":Creature Settings:Giant", "SEEK_MAGIC=", false);
	creatureInfo[kGiant].seekWeapons							= fi_GetBoolean(":Creature Settings:Giant", "SEEK_WEAPONS=", true);
	creatureInfo[kGiant].magicResist				      = (float)fi_GetData(":Creature Settings:Giant", "MAGIC_RESIST=", 45)/100.0;
	creatureInfo[kGiant].tamingLevel							= fi_GetData(":Creature Settings:Giant", "TAMING_LEVEL=", 101);
	creatureInfo[kGiant].creatureLevel							= fi_GetData(":Creature Settings:Giant", "LEVEL=", 1);
	creatureInfo[kGiant].armorRating							= fi_GetData(":Creature Settings:Giant", "ARMOR_RATING=", 60);
	creatureInfo[kGiant].immuneToPoison           = fi_GetBoolean(":Creature Settings:Giant", "POISON_IMMUNITY=", false);
	creatureInfo[kGiant].lockStrength							= fi_GetData(":Creature Settings:Giant", "LOCK=", 0);
	creatureInfo[kGiant].attackDamageMin					= fi_GetData(":Creature Settings:Giant", "MIN_DAMAGE=", 15);
	creatureInfo[kGiant].attackDamageMax					= fi_GetData(":Creature Settings:Giant", "MAX_DAMAGE=", 35);
	creatureInfo[kGiant].trackingLevel							= fi_GetData(":Creature Settings:Giant", "TRACKING_LEVEL=", 101);
	creatureInfo[kGiant].size									= fi_GetData(":Creature Settings:Giant", "SIZE=", 2);
	creatureInfo[kGiant].mageryLevel					= fi_GetData(":Creature Settings:Giant", "MAGERY_LEVEL=", 0);
	creatureInfo[kGiant].spellcastChance				= fi_GetData(":Creature Settings:Giant", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kGiant].selfhealChance					= fi_GetData(":Creature Settings:Giant", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kGiant].selfhealTrigger				= fi_GetData(":Creature Settings:Giant", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Giant", kGiant);
	fi_GetCreatureTalkScript(":Creature Settings:Giant", kGiant);
	fi_GetCreatureInventory(":Creature Settings:Giant", kGiant);
	fi_GetWeaponModifier(":Creature Settings:Giant", kGiant);

	creatureInfo[kHillGiant].bmp											=	2;
	creatureInfo[kHillGiant].domain										=	kLand;
	creatureInfo[kHillGiant].terrain									=	kAnyTerrain;
	creatureInfo[kHillGiant].alignment								= kEvil;
	creatureInfo[kHillGiant].directionalGraphics			= true;
	creatureInfo[kHillGiant].graphicsIndex						= 1;
	creatureInfo[kHillGiant].corpseID									= 40;
	creatureInfo[kHillGiant].desiredPopulation				= 60;
	creatureInfo[kHillGiant].meatEater								= true;
	creatureInfo[kHillGiant].canMorph					        = true;
	creatureInfo[kHillGiant].rareLevel								= 5;
	creatureInfo[kHillGiant].rareChance					  		= 50;
	creatureInfo[kHillGiant].points			          		= 4;
	creatureInfo[kHillGiant].randomSound							= kGiantSnd;
	creatureInfo[kHillGiant].blunt										= kBluntAttackSnd;
	creatureInfo[kHillGiant].speed										=	fi_GetData(":Creature Settings:Hill Giant", "SPEED=", 75);
	creatureInfo[kHillGiant].poisonous					      = fi_GetData(":Creature Settings:Hill Giant", "POISONOUS=", 0);
	creatureInfo[kHillGiant].curse					      		= fi_GetData(":Creature Settings:Hill Giant", "CURSE=", 0);
	creatureInfo[kHillGiant].maxHitPoints							= fi_GetData(":Creature Settings:Hill Giant", "HIT_POINTS=", 150);
	creatureInfo[kHillGiant].maxStrength							= fi_GetData(":Creature Settings:Hill Giant", "STRENGTH=", 150);
	creatureInfo[kHillGiant].maxIntelligence					= fi_GetData(":Creature Settings:Hill Giant", "INTELLIGENCE=", 50);
	creatureInfo[kHillGiant].maxDexterity							= fi_GetData(":Creature Settings:Hill Giant", "DEXTERITY=", 50);
	creatureInfo[kHillGiant].minGold								  = fi_GetData(":Creature Settings:Hill Giant", "MIN_GOLD=", 50);
	creatureInfo[kHillGiant].maxGold								  = fi_GetData(":Creature Settings:Hill Giant", "MAX_GOLD=", 75);
	creatureInfo[kHillGiant].seekGold									= fi_GetBoolean(":Creature Settings:Hill Giant", "SEEK_GOLD=", true);
	creatureInfo[kHillGiant].seekMagic								= fi_GetBoolean(":Creature Settings:Hill Giant", "SEEK_MAGIC=", false);
	creatureInfo[kHillGiant].seekWeapons							= fi_GetBoolean(":Creature Settings:Hill Giant", "SEEK_WEAPONS=", true);
	creatureInfo[kHillGiant].magicResist				      = (float)fi_GetData(":Creature Settings:Hill Giant", "MAGIC_RESIST=", 45)/100.0;
	creatureInfo[kHillGiant].tamingLevel							= fi_GetData(":Creature Settings:Hill Giant", "TAMING_LEVEL=", 101);
	creatureInfo[kHillGiant].creatureLevel							= fi_GetData(":Creature Settings:Hill Giant", "LEVEL=", 1);
	creatureInfo[kHillGiant].armorRating							= fi_GetData(":Creature Settings:Hill Giant", "ARMOR_RATING=", 60);
	creatureInfo[kHillGiant].immuneToPoison           = fi_GetBoolean(":Creature Settings:Hill Giant", "POISON_IMMUNITY=", false);
	creatureInfo[kHillGiant].lockStrength							= fi_GetData(":Creature Settings:Hill Giant", "LOCK=", 0);
	creatureInfo[kHillGiant].attackDamageMin					= fi_GetData(":Creature Settings:Hill Giant", "MIN_DAMAGE=", 15);
	creatureInfo[kHillGiant].attackDamageMax					= fi_GetData(":Creature Settings:Hill Giant", "MAX_DAMAGE=", 35);
	creatureInfo[kHillGiant].trackingLevel							= fi_GetData(":Creature Settings:Hill Giant", "TRACKING_LEVEL=", 101);
	creatureInfo[kHillGiant].size									= fi_GetData(":Creature Settings:Hill Giant", "SIZE=", 2);
	creatureInfo[kHillGiant].mageryLevel					= fi_GetData(":Creature Settings:Hill Giant", "MAGERY_LEVEL=", 0);
	creatureInfo[kHillGiant].spellcastChance				= fi_GetData(":Creature Settings:Hill Giant", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kHillGiant].selfhealChance					= fi_GetData(":Creature Settings:Hill Giant", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kHillGiant].selfhealTrigger				= fi_GetData(":Creature Settings:Hill Giant", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Hill Giant", kHillGiant);
	fi_GetCreatureTalkScript(":Creature Settings:Hill Giant", kHillGiant);
	fi_GetCreatureInventory(":Creature Settings:Hill Giant", kHillGiant);
	fi_GetWeaponModifier(":Creature Settings:Hill Giant", kHillGiant);

	creatureInfo[kFrostGiant].bmp											=	2;
	creatureInfo[kFrostGiant].domain									=	kLand;
	creatureInfo[kFrostGiant].terrain									=	kAnyTerrain;
	creatureInfo[kFrostGiant].alignment								= kEvil;
	creatureInfo[kFrostGiant].directionalGraphics			= true;
	creatureInfo[kFrostGiant].graphicsIndex						= 1;
	creatureInfo[kFrostGiant].corpseID								= 40;
	creatureInfo[kFrostGiant].desiredPopulation				= 60;
	creatureInfo[kFrostGiant].meatEater								= true;
	creatureInfo[kFrostGiant].canMorph					      = true;
	creatureInfo[kFrostGiant].rareLevel								= 5;
	creatureInfo[kFrostGiant].rareChance					  	= 50;
	creatureInfo[kFrostGiant].points			          	= 4;
	creatureInfo[kFrostGiant].randomSound							= kGiantSnd;
	creatureInfo[kFrostGiant].blunt										= kBluntAttackSnd;
	creatureInfo[kFrostGiant].speed										=	fi_GetData(":Creature Settings:Frost Giant", "SPEED=", 75);
	creatureInfo[kFrostGiant].poisonous					      = fi_GetData(":Creature Settings:Frost Giant", "POISONOUS=", 0);
	creatureInfo[kFrostGiant].curse					      		= fi_GetData(":Creature Settings:Frost Giant", "CURSE=", 0);
	creatureInfo[kFrostGiant].maxHitPoints						= fi_GetData(":Creature Settings:Frost Giant", "HIT_POINTS=", 150);
	creatureInfo[kFrostGiant].maxStrength							= fi_GetData(":Creature Settings:Frost Giant", "STRENGTH=", 150);
	creatureInfo[kFrostGiant].maxIntelligence					= fi_GetData(":Creature Settings:Frost Giant", "INTELLIGENCE=", 50);
	creatureInfo[kFrostGiant].maxDexterity						= fi_GetData(":Creature Settings:Frost Giant", "DEXTERITY=", 50);
	creatureInfo[kFrostGiant].minGold								  = fi_GetData(":Creature Settings:Frost Giant", "MIN_GOLD=", 50);
	creatureInfo[kFrostGiant].maxGold								  = fi_GetData(":Creature Settings:Frost Giant", "MAX_GOLD=", 75);
	creatureInfo[kFrostGiant].seekGold								= fi_GetBoolean(":Creature Settings:Frost Giant", "SEEK_GOLD=", true);
	creatureInfo[kFrostGiant].seekMagic								= fi_GetBoolean(":Creature Settings:Frost Giant", "SEEK_MAGIC=", false);
	creatureInfo[kFrostGiant].seekWeapons							= fi_GetBoolean(":Creature Settings:Frost Giant", "SEEK_WEAPONS=", true);
	creatureInfo[kFrostGiant].magicResist				      = (float)fi_GetData(":Creature Settings:Frost Giant", "MAGIC_RESIST=", 45)/100.0;
	creatureInfo[kFrostGiant].tamingLevel							= fi_GetData(":Creature Settings:Frost Giant", "TAMING_LEVEL=", 101);
	creatureInfo[kFrostGiant].creatureLevel							= fi_GetData(":Creature Settings:Frost Giant", "LEVEL=", 1);
	creatureInfo[kFrostGiant].armorRating							= fi_GetData(":Creature Settings:Frost Giant", "ARMOR_RATING=", 60);
	creatureInfo[kFrostGiant].immuneToPoison          = fi_GetBoolean(":Creature Settings:Frost Giant", "POISON_IMMUNITY=", false);
	creatureInfo[kFrostGiant].lockStrength						= fi_GetData(":Creature Settings:Frost Giant", "LOCK=", 0);
	creatureInfo[kFrostGiant].attackDamageMin					= fi_GetData(":Creature Settings:Frost Giant", "MIN_DAMAGE=", 15);
	creatureInfo[kFrostGiant].attackDamageMax					= fi_GetData(":Creature Settings:Frost Giant", "MAX_DAMAGE=", 35);
	creatureInfo[kFrostGiant].trackingLevel							= fi_GetData(":Creature Settings:Frost Giant", "TRACKING_LEVEL=", 101);
	creatureInfo[kFrostGiant].size									= fi_GetData(":Creature Settings:Frost Giant", "SIZE=", 2);
	creatureInfo[kFrostGiant].mageryLevel					= fi_GetData(":Creature Settings:Frost Giant", "MAGERY_LEVEL=", 0);
	creatureInfo[kFrostGiant].spellcastChance				= fi_GetData(":Creature Settings:Frost Giant", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kFrostGiant].selfhealChance					= fi_GetData(":Creature Settings:Frost Giant", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kFrostGiant].selfhealTrigger				= fi_GetData(":Creature Settings:Frost Giant", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Frost Giant", kGiant);
	fi_GetCreatureTalkScript(":Creature Settings:Frost Giant", kFrostGiant);
	fi_GetCreatureInventory(":Creature Settings:Frost Giant", kFrostGiant);
	fi_GetWeaponModifier(":Creature Settings:Frost Giant", kFrostGiant);

	creatureInfo[kCentaur].bmp											=	2;
	creatureInfo[kCentaur].domain										=	kLand;
	creatureInfo[kCentaur].terrain									=	kAnyTerrain;
	creatureInfo[kCentaur].alignment								= kNeutral;
	creatureInfo[kCentaur].directionalGraphics			= true;
	creatureInfo[kCentaur].graphicsIndex						= 1;
	creatureInfo[kCentaur].corpseID									= 41;
	creatureInfo[kCentaur].desiredPopulation				= 60;
	creatureInfo[kCentaur].canMorph					        = true;
	creatureInfo[kCentaur].blunt										= kBluntAttackSnd;
	creatureInfo[kCentaur].speed										=	fi_GetData(":Creature Settings:Centaur", "SPEED=", 45);
	creatureInfo[kCentaur].poisonous					      = fi_GetData(":Creature Settings:Centaur", "POISONOUS=", 0);
	creatureInfo[kCentaur].curse					      		= fi_GetData(":Creature Settings:Centaur", "CURSE=", 0);
	creatureInfo[kCentaur].maxHitPoints							= fi_GetData(":Creature Settings:Centaur", "HIT_POINTS=", 95);
	creatureInfo[kCentaur].maxStrength							= fi_GetData(":Creature Settings:Centaur", "STRENGTH=", 95);
	creatureInfo[kCentaur].maxIntelligence					= fi_GetData(":Creature Settings:Centaur", "INTELLIGENCE=", 60);
	creatureInfo[kCentaur].maxDexterity							= fi_GetData(":Creature Settings:Centaur", "DEXTERITY=", 50);
	creatureInfo[kCentaur].minGold								  = fi_GetData(":Creature Settings:Centaur", "MIN_GOLD=", 5);
	creatureInfo[kCentaur].maxGold								  = fi_GetData(":Creature Settings:Centaur", "MAX_GOLD=", 25);
	creatureInfo[kCentaur].seekGold									= fi_GetBoolean(":Creature Settings:Centaur", "SEEK_GOLD=", true);
	creatureInfo[kCentaur].seekMagic								= fi_GetBoolean(":Creature Settings:Centaur", "SEEK_MAGIC=", false);
	creatureInfo[kCentaur].seekWeapons							= fi_GetBoolean(":Creature Settings:Centaur", "SEEK_WEAPONS=", true);
	creatureInfo[kCentaur].magicResist				      = (float)fi_GetData(":Creature Settings:Centaur", "MAGIC_RESIST=", 80)/100.0;
	creatureInfo[kCentaur].tamingLevel							= fi_GetData(":Creature Settings:Centaur", "TAMING_LEVEL=", 101);
	creatureInfo[kCentaur].creatureLevel							= fi_GetData(":Creature Settings:Centaur", "LEVEL=", 1);
	creatureInfo[kCentaur].armorRating							= fi_GetData(":Creature Settings:Centaur", "ARMOR_RATING=", 35);
	creatureInfo[kCentaur].immuneToPoison           = fi_GetBoolean(":Creature Settings:Centaur", "POISON_IMMUNITY=", false);
	creatureInfo[kCentaur].lockStrength							= fi_GetData(":Creature Settings:Centaur", "LOCK=", 0);
	creatureInfo[kCentaur].attackDamageMin					= fi_GetData(":Creature Settings:Centaur", "MIN_DAMAGE=", 5);
	creatureInfo[kCentaur].attackDamageMax					= fi_GetData(":Creature Settings:Centaur", "MAX_DAMAGE=", 12);
	creatureInfo[kCentaur].trackingLevel							= fi_GetData(":Creature Settings:Centaur", "TRACKING_LEVEL=", 101);
	creatureInfo[kCentaur].size									= fi_GetData(":Creature Settings:Centaur", "SIZE=", 1);
	creatureInfo[kCentaur].mageryLevel					= fi_GetData(":Creature Settings:Centaur", "MAGERY_LEVEL=", 0);
	creatureInfo[kCentaur].spellcastChance				= fi_GetData(":Creature Settings:Centaur", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kCentaur].selfhealChance					= fi_GetData(":Creature Settings:Centaur", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kCentaur].selfhealTrigger				= fi_GetData(":Creature Settings:Centaur", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Centaur", kCentaur);
	//cr_AddToCreatureControlInventory(kCentaur, kRawSteak, 3, 5);
	//cr_AddToCreatureControlInventory(kCentaur, kLeatherHide, 2, 3);
	fi_GetCreatureTalkScript(":Creature Settings:Centaur", kCentaur);
	fi_GetCreatureInventory(":Creature Settings:Centaur", kCentaur);
	fi_GetWeaponModifier(":Creature Settings:Centaur", kCentaur);

	creatureInfo[kElderCentaur].bmp											=	2;
	creatureInfo[kElderCentaur].domain										=	kLand;
	creatureInfo[kElderCentaur].terrain									=	kAnyTerrain;
	creatureInfo[kElderCentaur].alignment								= kNeutral;
	creatureInfo[kElderCentaur].directionalGraphics			= true;
	creatureInfo[kElderCentaur].graphicsIndex						= 1;
	creatureInfo[kElderCentaur].corpseID									= 41;
	creatureInfo[kElderCentaur].desiredPopulation				= 60;
	creatureInfo[kElderCentaur].canMorph					        = true;
	creatureInfo[kElderCentaur].blunt										= kBluntAttackSnd;
	creatureInfo[kElderCentaur].speed										=	fi_GetData(":Creature Settings:Elder Centaur", "SPEED=", 45);
	creatureInfo[kElderCentaur].poisonous					      = fi_GetData(":Creature Settings:Elder Centaur", "POISONOUS=", 0);
	creatureInfo[kElderCentaur].curse					      		= fi_GetData(":Creature Settings:Elder Centaur", "CURSE=", 0);
	creatureInfo[kElderCentaur].maxHitPoints							= fi_GetData(":Creature Settings:Elder Centaur", "HIT_POINTS=", 95);
	creatureInfo[kElderCentaur].maxStrength							= fi_GetData(":Creature Settings:Elder Centaur", "STRENGTH=", 95);
	creatureInfo[kElderCentaur].maxIntelligence					= fi_GetData(":Creature Settings:Elder Centaur", "INTELLIGENCE=", 60);
	creatureInfo[kElderCentaur].maxDexterity							= fi_GetData(":Creature Settings:Elder Centaur", "DEXTERITY=", 50);
	creatureInfo[kElderCentaur].minGold								  = fi_GetData(":Creature Settings:Elder Centaur", "MIN_GOLD=", 5);
	creatureInfo[kElderCentaur].maxGold								  = fi_GetData(":Creature Settings:Elder Centaur", "MAX_GOLD=", 25);
	creatureInfo[kElderCentaur].seekGold									= fi_GetBoolean(":Creature Settings:Elder Centaur", "SEEK_GOLD=", true);
	creatureInfo[kElderCentaur].seekMagic								= fi_GetBoolean(":Creature Settings:Elder Centaur", "SEEK_MAGIC=", false);
	creatureInfo[kElderCentaur].seekWeapons							= fi_GetBoolean(":Creature Settings:Elder Centaur", "SEEK_WEAPONS=", true);
	creatureInfo[kElderCentaur].magicResist				      = (float)fi_GetData(":Creature Settings:Elder Centaur", "MAGIC_RESIST=", 80)/100.0;
	creatureInfo[kElderCentaur].tamingLevel							= fi_GetData(":Creature Settings:Elder Centaur", "TAMING_LEVEL=", 101);
	creatureInfo[kElderCentaur].creatureLevel							= fi_GetData(":Creature Settings:Elder Centaur", "LEVEL=", 1);
	creatureInfo[kElderCentaur].armorRating							= fi_GetData(":Creature Settings:Elder Centaur", "ARMOR_RATING=", 35);
	creatureInfo[kElderCentaur].immuneToPoison           = fi_GetBoolean(":Creature Settings:Elder Centaur", "POISON_IMMUNITY=", false);
	creatureInfo[kElderCentaur].lockStrength							= fi_GetData(":Creature Settings:Elder Centaur", "LOCK=", 0);
	creatureInfo[kElderCentaur].attackDamageMin					= fi_GetData(":Creature Settings:Elder Centaur", "MIN_DAMAGE=", 5);
	creatureInfo[kElderCentaur].attackDamageMax					= fi_GetData(":Creature Settings:Elder Centaur", "MAX_DAMAGE=", 12);
	creatureInfo[kElderCentaur].trackingLevel							= fi_GetData(":Creature Settings:Elder Centaur", "TRACKING_LEVEL=", 101);
	creatureInfo[kElderCentaur].size									= fi_GetData(":Creature Settings:Elder Centaur", "SIZE=", 1);
	creatureInfo[kElderCentaur].mageryLevel					= fi_GetData(":Creature Settings:Elder Centaur", "MAGERY_LEVEL=", 0);
	creatureInfo[kElderCentaur].spellcastChance				= fi_GetData(":Creature Settings:Elder Centaur", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kElderCentaur].selfhealChance					= fi_GetData(":Creature Settings:Elder Centaur", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kElderCentaur].selfhealTrigger				= fi_GetData(":Creature Settings:Elder Centaur", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Elder Centaur", kElderCentaur);
	fi_GetCreatureTalkScript(":Creature Settings:Elder Centaur", kElderCentaur);
	fi_GetCreatureInventory(":Creature Settings:Elder Centaur", kElderCentaur);
	fi_GetWeaponModifier(":Creature Settings:Elder Centaur", kElderCentaur);

	creatureInfo[kZombie].bmp											=	2;
	creatureInfo[kZombie].domain									=	kLand;
	creatureInfo[kZombie].terrain									=	kAnyTerrain;
	creatureInfo[kZombie].alignment								= kEvil;
	creatureInfo[kZombie].directionalGraphics			= true;
	creatureInfo[kZombie].graphicsIndex						= 1;
	creatureInfo[kZombie].corpseID								= 43;
	creatureInfo[kZombie].desiredPopulation				= 200;
	creatureInfo[kZombie].undead			            = true;
	creatureInfo[kZombie].canMorph					      = true;
	creatureInfo[kZombie].bleeds					      = false;
	creatureInfo[kZombie].randomSound							= kZombieSnd;
	creatureInfo[kZombie].blunt										= kZombieAttackSnd;
	creatureInfo[kZombie].speed										=	fi_GetData(":Creature Settings:Zombie", "SPEED=", 90);
	creatureInfo[kZombie].poisonous					      = fi_GetData(":Creature Settings:Zombie", "POISONOUS=", 0);
	creatureInfo[kZombie].curse					      		= fi_GetData(":Creature Settings:Zombie", "CURSE=", 0);
	creatureInfo[kZombie].maxHitPoints						= fi_GetData(":Creature Settings:Zombie", "HIT_POINTS=", 85);
	creatureInfo[kZombie].maxStrength							= fi_GetData(":Creature Settings:Zombie", "STRENGTH=", 85);
	creatureInfo[kZombie].maxIntelligence					= fi_GetData(":Creature Settings:Zombie", "INTELLIGENCE=", 50);
	creatureInfo[kZombie].maxDexterity						= fi_GetData(":Creature Settings:Zombie", "DEXTERITY=", 40);
	creatureInfo[kZombie].minGold								  = fi_GetData(":Creature Settings:Zombie", "MIN_GOLD=", 10);
	creatureInfo[kZombie].maxGold								  = fi_GetData(":Creature Settings:Zombie", "MAX_GOLD=", 40);
	creatureInfo[kZombie].seekGold								= fi_GetBoolean(":Creature Settings:Zombie", "SEEK_GOLD=", true);
	creatureInfo[kZombie].seekMagic								= fi_GetBoolean(":Creature Settings:Zombie", "SEEK_MAGIC=", false);
	creatureInfo[kZombie].seekWeapons							= fi_GetBoolean(":Creature Settings:Zombie", "SEEK_WEAPONS=", false);
	creatureInfo[kZombie].magicResist				      = (float)fi_GetData(":Creature Settings:Zombie", "MAGIC_RESIST=", 65)/100.0;
	creatureInfo[kZombie].tamingLevel							= fi_GetData(":Creature Settings:Zombie", "TAMING_LEVEL=", 101);
	creatureInfo[kZombie].creatureLevel							= fi_GetData(":Creature Settings:Zombie", "LEVEL=", 1);
	creatureInfo[kZombie].armorRating							= fi_GetData(":Creature Settings:Zombie", "ARMOR_RATING=", 10);
	creatureInfo[kZombie].immuneToPoison          = fi_GetBoolean(":Creature Settings:Zombie", "POISON_IMMUNITY=", true);
	creatureInfo[kZombie].lockStrength						= fi_GetData(":Creature Settings:Zombie", "LOCK=", 0);
	creatureInfo[kZombie].attackDamageMin					= fi_GetData(":Creature Settings:Zombie", "MIN_DAMAGE=", 10);
	creatureInfo[kZombie].attackDamageMax					= fi_GetData(":Creature Settings:Zombie", "MAX_DAMAGE=", 20);
	creatureInfo[kZombie].trackingLevel							= fi_GetData(":Creature Settings:Zombie", "TRACKING_LEVEL=", 101);
	creatureInfo[kZombie].size									= fi_GetData(":Creature Settings:Zombie", "SIZE=", 1);
	creatureInfo[kZombie].mageryLevel					= fi_GetData(":Creature Settings:Zombie", "MAGERY_LEVEL=", 0);
	creatureInfo[kZombie].spellcastChance				= fi_GetData(":Creature Settings:Zombie", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kZombie].selfhealChance					= fi_GetData(":Creature Settings:Zombie", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kZombie].selfhealTrigger				= fi_GetData(":Creature Settings:Zombie", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Zombie", kZombie);
	//cr_AddToCreatureControlInventory(kZombie, kRawSteak, 3, 5);
	fi_GetCreatureTalkScript(":Creature Settings:Zombie", kZombie);
	fi_GetCreatureInventory(":Creature Settings:Zombie", kZombie);
	fi_GetWeaponModifier(":Creature Settings:Zombie", kZombie);

	creatureInfo[kTroll].bmp											=	1;
	creatureInfo[kTroll].domain										=	kLand;
	creatureInfo[kTroll].terrain									=	kAnyTerrain;
	creatureInfo[kTroll].alignment								= kEvil;
	creatureInfo[kTroll].directionalGraphics			= true;
	creatureInfo[kTroll].graphicsIndex						= 1;
	creatureInfo[kTroll].corpseID									= 39;
	creatureInfo[kTroll].desiredPopulation				= 200;
	creatureInfo[kTroll].canMorph					        = true;
	creatureInfo[kTroll].rareLevel								= 50;
	creatureInfo[kTroll].rareChance					  		= 50;
	creatureInfo[kTroll].points			          		= 4;
	creatureInfo[kTroll].metal					          = true;
	creatureInfo[kTroll].randomSound							= kTrollSnd;
	creatureInfo[kTroll].speed										=	fi_GetData(":Creature Settings:Troll", "SPEED=", 50);
	creatureInfo[kTroll].maxHitPoints							= fi_GetData(":Creature Settings:Troll", "HIT_POINTS=", 80);
	creatureInfo[kTroll].maxStrength							= fi_GetData(":Creature Settings:Troll", "STRENGTH=", 80);
	creatureInfo[kTroll].maxIntelligence					= fi_GetData(":Creature Settings:Troll", "INTELLIGENCE=", 75);
	creatureInfo[kTroll].maxDexterity							= fi_GetData(":Creature Settings:Troll", "DEXTERITY=", 50);
	creatureInfo[kTroll].minGold								  = fi_GetData(":Creature Settings:Troll", "MIN_GOLD=", 25);
	creatureInfo[kTroll].maxGold								  = fi_GetData(":Creature Settings:Troll", "MAX_GOLD=", 38);
	creatureInfo[kTroll].armorRating							= fi_GetData(":Creature Settings:Troll", "ARMOR_RATING=", 30);
	creatureInfo[kTroll].attackDamageMin					= fi_GetData(":Creature Settings:Troll", "MIN_DAMAGE=", 7);
	creatureInfo[kTroll].attackDamageMax					= fi_GetData(":Creature Settings:Troll", "MAX_DAMAGE=", 18);
	creatureInfo[kTroll].magicResist				      = (float)fi_GetData(":Creature Settings:Troll", "MAGIC_RESIST=", 70)/100.0;
	creatureInfo[kTroll].lockStrength							= fi_GetData(":Creature Settings:Troll", "LOCK=", 0);
	creatureInfo[kTroll].tamingLevel							= fi_GetData(":Creature Settings:Troll", "TAMING_LEVEL=", 101);
	creatureInfo[kTroll].creatureLevel							= fi_GetData(":Creature Settings:Troll", "LEVEL=", 1);
	creatureInfo[kTroll].poisonous					      = fi_GetData(":Creature Settings:Troll", "POISONOUS=", 0);
	creatureInfo[kTroll].curse					      		= fi_GetData(":Creature Settings:Troll", "CURSE=", 0);
	creatureInfo[kTroll].immuneToPoison           = fi_GetBoolean(":Creature Settings:Troll", "POISON_IMMUNITY=", false);
	creatureInfo[kTroll].seekGold									= fi_GetBoolean(":Creature Settings:Troll", "SEEK_GOLD=", true);
	creatureInfo[kTroll].seekWeapons							= fi_GetBoolean(":Creature Settings:Troll", "SEEK_WEAPONS=", true);
	creatureInfo[kTroll].seekMagic								= fi_GetBoolean(":Creature Settings:Troll", "SEEK_MAGIC=", false);
	creatureInfo[kTroll].trackingLevel							= fi_GetData(":Creature Settings:Troll", "TRACKING_LEVEL=", 101);
	creatureInfo[kTroll].size									= fi_GetData(":Creature Settings:Troll", "SIZE=", 1);
	creatureInfo[kTroll].mageryLevel					= fi_GetData(":Creature Settings:Troll", "MAGERY_LEVEL=", 0);
	creatureInfo[kTroll].spellcastChance				= fi_GetData(":Creature Settings:Troll", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kTroll].selfhealChance					= fi_GetData(":Creature Settings:Troll", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kTroll].selfhealTrigger				= fi_GetData(":Creature Settings:Troll", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Troll", kTroll);
	fi_GetCreatureTalkScript(":Creature Settings:Troll", kTroll);
	fi_GetCreatureInventory(":Creature Settings:Troll", kTroll);
	fi_GetWeaponModifier(":Creature Settings:Troll", kTroll);

	creatureInfo[kForestTroll].bmp											=	1;
	creatureInfo[kForestTroll].domain										=	kLand;
	creatureInfo[kForestTroll].terrain									=	kAnyTerrain;
	creatureInfo[kForestTroll].alignment								= kEvil;
	creatureInfo[kForestTroll].directionalGraphics			= true;
	creatureInfo[kForestTroll].graphicsIndex						= 1;
	creatureInfo[kForestTroll].corpseID									= 39;
	creatureInfo[kForestTroll].desiredPopulation				= 100;
	creatureInfo[kForestTroll].canMorph					        = true;
	creatureInfo[kForestTroll].rareLevel								= 50;
	creatureInfo[kForestTroll].rareChance					  		= 50;
	creatureInfo[kForestTroll].points			          		= 4;
	creatureInfo[kForestTroll].metal					          = true;
	creatureInfo[kForestTroll].randomSound							= kTrollSnd;
	creatureInfo[kForestTroll].speed										=	fi_GetData(":Creature Settings:Forest Troll", "SPEED=", 50);
	creatureInfo[kForestTroll].maxHitPoints							= fi_GetData(":Creature Settings:Forest Troll", "HIT_POINTS=", 80);
	creatureInfo[kForestTroll].maxStrength							= fi_GetData(":Creature Settings:Forest Troll", "STRENGTH=", 80);
	creatureInfo[kForestTroll].maxIntelligence					= fi_GetData(":Creature Settings:Forest Troll", "INTELLIGENCE=", 75);
	creatureInfo[kForestTroll].maxDexterity							= fi_GetData(":Creature Settings:Forest Troll", "DEXTERITY=", 50);
	creatureInfo[kForestTroll].minGold								  = fi_GetData(":Creature Settings:Forest Troll", "MIN_GOLD=", 25);
	creatureInfo[kForestTroll].maxGold								  = fi_GetData(":Creature Settings:Forest Troll", "MAX_GOLD=", 38);
	creatureInfo[kForestTroll].armorRating							= fi_GetData(":Creature Settings:Forest Troll", "ARMOR_RATING=", 30);
	creatureInfo[kForestTroll].attackDamageMin					= fi_GetData(":Creature Settings:Forest Troll", "MIN_DAMAGE=", 7);
	creatureInfo[kForestTroll].attackDamageMax					= fi_GetData(":Creature Settings:Forest Troll", "MAX_DAMAGE=", 18);
	creatureInfo[kForestTroll].magicResist				      = (float)fi_GetData(":Creature Settings:Forest Troll", "MAGIC_RESIST=", 70)/100.0;
	creatureInfo[kForestTroll].lockStrength							= fi_GetData(":Creature Settings:Forest Troll", "LOCK=", 0);
	creatureInfo[kForestTroll].tamingLevel							= fi_GetData(":Creature Settings:Forest Troll", "TAMING_LEVEL=", 101);
	creatureInfo[kForestTroll].creatureLevel							= fi_GetData(":Creature Settings:Forest Troll", "LEVEL=", 1);
	creatureInfo[kForestTroll].poisonous					      = fi_GetData(":Creature Settings:Forest Troll", "POISONOUS=", 0);
	creatureInfo[kForestTroll].curse					      		= fi_GetData(":Creature Settings:Forest Troll", "CURSE=", 0);
	creatureInfo[kForestTroll].immuneToPoison           = fi_GetBoolean(":Creature Settings:Forest Troll", "POISON_IMMUNITY=", false);
	creatureInfo[kForestTroll].seekGold									= fi_GetBoolean(":Creature Settings:Forest Troll", "SEEK_GOLD=", true);
	creatureInfo[kForestTroll].seekWeapons							= fi_GetBoolean(":Creature Settings:Forest Troll", "SEEK_WEAPONS=", true);
	creatureInfo[kForestTroll].seekMagic								= fi_GetBoolean(":Creature Settings:Forest Troll", "SEEK_MAGIC=", false);
	creatureInfo[kForestTroll].trackingLevel							= fi_GetData(":Creature Settings:Forest Troll", "TRACKING_LEVEL=", 101);
	creatureInfo[kForestTroll].size									= fi_GetData(":Creature Settings:Forest Troll", "SIZE=", 1);
	creatureInfo[kForestTroll].mageryLevel					= fi_GetData(":Creature Settings:Forest Troll", "MAGERY_LEVEL=", 0);
	creatureInfo[kForestTroll].spellcastChance				= fi_GetData(":Creature Settings:Forest Troll", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kForestTroll].selfhealChance					= fi_GetData(":Creature Settings:Forest Troll", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kForestTroll].selfhealTrigger				= fi_GetData(":Creature Settings:Forest Troll", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Forest Troll", kForestTroll);
	fi_GetCreatureTalkScript(":Creature Settings:Forest Troll", kForestTroll);
	fi_GetCreatureInventory(":Creature Settings:Forest Troll", kForestTroll);
	fi_GetWeaponModifier(":Creature Settings:Forest Troll", kForestTroll);

	creatureInfo[kSnail].bmp											=	1;
	creatureInfo[kSnail].domain										=	kLand;
	creatureInfo[kSnail].terrain									=	kAnyTerrain;
	creatureInfo[kSnail].alignment								= kEvil;
	creatureInfo[kSnail].directionalGraphics			= true;
	creatureInfo[kSnail].graphicsIndex						= 1;
	creatureInfo[kSnail].corpseID									= 37;
	creatureInfo[kSnail].desiredPopulation				= 200;
	creatureInfo[kSnail].meatEater								= true;
	creatureInfo[kSnail].canMorph					        = true;
	creatureInfo[kSnail].attackSound							= kSnailAttackSnd;
	creatureInfo[kSnail].speed										=	fi_GetData(":Creature Settings:Giant Snail", "SPEED=", 100);
	creatureInfo[kSnail].poisonous					      = fi_GetData(":Creature Settings:Giant Snail", "POISONOUS=", 0);
	creatureInfo[kSnail].curse					      		= fi_GetData(":Creature Settings:Giant Snail", "CURSE=", 0);
	creatureInfo[kSnail].maxHitPoints							= fi_GetData(":Creature Settings:Giant Snail", "HIT_POINTS=", 12);
	creatureInfo[kSnail].maxStrength							= fi_GetData(":Creature Settings:Giant Snail", "STRENGTH=", 12);
	creatureInfo[kSnail].maxIntelligence					= fi_GetData(":Creature Settings:Giant Snail", "INTELLIGENCE=", 12);
	creatureInfo[kSnail].maxDexterity							= fi_GetData(":Creature Settings:Giant Snail", "DEXTERITY=", 10);
	creatureInfo[kSnail].minGold								  = fi_GetData(":Creature Settings:Giant Snail", "MIN_GOLD=", 0);
	creatureInfo[kSnail].maxGold								  = fi_GetData(":Creature Settings:Giant Snail", "MAX_GOLD=", 5);
	creatureInfo[kSnail].seekGold									= fi_GetBoolean(":Creature Settings:Giant Snail", "SEEK_GOLD=", true);
	creatureInfo[kSnail].seekMagic								= fi_GetBoolean(":Creature Settings:Giant Snail", "SEEK_MAGIC=", false);
	creatureInfo[kSnail].seekWeapons							= fi_GetBoolean(":Creature Settings:Giant Snail", "SEEK_WEAPONS=", false);
	creatureInfo[kSnail].magicResist				      = (float)fi_GetData(":Creature Settings:Giant Snail", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kSnail].tamingLevel							= fi_GetData(":Creature Settings:Giant Snail", "TAMING_LEVEL=", 3);
	creatureInfo[kSnail].creatureLevel							= fi_GetData(":Creature Settings:Giant Snail", "LEVEL=", 1);
	creatureInfo[kSnail].armorRating							= fi_GetData(":Creature Settings:Giant Snail", "ARMOR_RATING=", 15);
	creatureInfo[kSnail].immuneToPoison           = fi_GetBoolean(":Creature Settings:Giant Snail", "POISON_IMMUNITY=", false);
	creatureInfo[kSnail].lockStrength							= fi_GetData(":Creature Settings:Giant Snail", "LOCK=", 0);
	creatureInfo[kSnail].attackDamageMin					= fi_GetData(":Creature Settings:Giant Snail", "MIN_DAMAGE=", 2);
	creatureInfo[kSnail].attackDamageMax					= fi_GetData(":Creature Settings:Giant Snail", "MAX_DAMAGE=", 6);
	creatureInfo[kSnail].trackingLevel							= fi_GetData(":Creature Settings:Giant Snail", "TRACKING_LEVEL=", 101);
	creatureInfo[kSnail].size									= fi_GetData(":Creature Settings:Giant Snail", "SIZE=", 1);
	creatureInfo[kSnail].mageryLevel					= fi_GetData(":Creature Settings:Giant Snail", "MAGERY_LEVEL=", 0);
	creatureInfo[kSnail].spellcastChance				= fi_GetData(":Creature Settings:Giant Snail", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSnail].selfhealChance					= fi_GetData(":Creature Settings:Giant Snail", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSnail].selfhealTrigger				= fi_GetData(":Creature Settings:Giant Snail", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Giant Snail", kSnail);
	//cr_AddToCreatureControlInventory(kSnail, kRawSteak, 1, 1);
	fi_GetCreatureTalkScript(":Creature Settings:Giant Snail", kSnail);
	fi_GetCreatureInventory(":Creature Settings:Giant Snail", kSnail);
	fi_GetWeaponModifier(":Creature Settings:Giant Snail", kSnail);

	creatureInfo[kAnt].bmp											=	1;
	creatureInfo[kAnt].domain										=	kLand;
	creatureInfo[kAnt].terrain									=	kAnyTerrain;
	creatureInfo[kAnt].alignment								= kEvil;
	creatureInfo[kAnt].directionalGraphics			= true;
	creatureInfo[kAnt].graphicsIndex						= 1;
	creatureInfo[kAnt].corpseID									= 38;
	creatureInfo[kAnt].desiredPopulation				= 100;
	creatureInfo[kAnt].meatEater								= true;
	creatureInfo[kAnt].canMorph					        = true;
	creatureInfo[kAnt].attackSound							= kAntAttackSnd;
	creatureInfo[kAnt].speed										=	fi_GetData(":Creature Settings:Giant Ant", "SPEED=", 70);
	creatureInfo[kAnt].poisonous					      = fi_GetData(":Creature Settings:Giant Ant", "POISONOUS=", 0);
	creatureInfo[kAnt].curse					      		= fi_GetData(":Creature Settings:Giant Ant", "CURSE=", 0);
	creatureInfo[kAnt].maxHitPoints							= fi_GetData(":Creature Settings:Giant Ant", "HIT_POINTS=", 60);
	creatureInfo[kAnt].maxStrength							= fi_GetData(":Creature Settings:Giant Ant", "STRENGTH=", 60);
	creatureInfo[kAnt].maxIntelligence					= fi_GetData(":Creature Settings:Giant Ant", "INTELLIGENCE=", 30);
	creatureInfo[kAnt].maxDexterity							= fi_GetData(":Creature Settings:Giant Ant", "DEXTERITY=", 50);
	creatureInfo[kAnt].minGold								  = fi_GetData(":Creature Settings:Giant Ant", "MIN_GOLD=", 0);
	creatureInfo[kAnt].maxGold								  = fi_GetData(":Creature Settings:Giant Ant", "MAX_GOLD=", 5);
	creatureInfo[kAnt].seekGold									= fi_GetBoolean(":Creature Settings:Giant Ant", "SEEK_GOLD=", true);
	creatureInfo[kAnt].seekMagic								= fi_GetBoolean(":Creature Settings:Giant Ant", "SEEK_MAGIC=", false);
	creatureInfo[kAnt].seekWeapons							= fi_GetBoolean(":Creature Settings:Giant Ant", "SEEK_WEAPONS=", false);
	creatureInfo[kAnt].magicResist				      = (float)fi_GetData(":Creature Settings:Giant Ant", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kAnt].tamingLevel							= fi_GetData(":Creature Settings:Giant Ant", "TAMING_LEVEL=", 4);
	creatureInfo[kAnt].creatureLevel							= fi_GetData(":Creature Settings:Giant Ant", "LEVEL=", 1);
	creatureInfo[kAnt].armorRating							= fi_GetData(":Creature Settings:Giant Ant", "ARMOR_RATING=", 15);
	creatureInfo[kAnt].immuneToPoison           = fi_GetBoolean(":Creature Settings:Giant Ant", "POISON_IMMUNITY=", false);
	creatureInfo[kAnt].lockStrength							= fi_GetData(":Creature Settings:Giant Ant", "LOCK=", 0);
	creatureInfo[kAnt].attackDamageMin					= fi_GetData(":Creature Settings:Giant Ant", "MIN_DAMAGE=", 3);
	creatureInfo[kAnt].attackDamageMax					= fi_GetData(":Creature Settings:Giant Ant", "MAX_DAMAGE=", 7);
	creatureInfo[kAnt].trackingLevel							= fi_GetData(":Creature Settings:Giant Ant", "TRACKING_LEVEL=", 101);
	creatureInfo[kAnt].size									= fi_GetData(":Creature Settings:Giant Ant", "SIZE=", 1);
	creatureInfo[kAnt].mageryLevel					= fi_GetData(":Creature Settings:Giant Ant", "MAGERY_LEVEL=", 0);
	creatureInfo[kAnt].spellcastChance				= fi_GetData(":Creature Settings:Giant Ant", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kAnt].selfhealChance					= fi_GetData(":Creature Settings:Giant Ant", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kAnt].selfhealTrigger				= fi_GetData(":Creature Settings:Giant Ant", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Giant Ant", kAnt);
	//cr_AddToCreatureControlInventory(kAnt, kRawSteak, 1, 1);
	fi_GetCreatureTalkScript(":Creature Settings:Giant Ant", kAnt);
	fi_GetCreatureInventory(":Creature Settings:Giant Ant", kAnt);
	fi_GetWeaponModifier(":Creature Settings:Giant Ant", kAnt);

	creatureInfo[kWorkAnt].bmp											=	1;
	creatureInfo[kWorkAnt].domain										=	kLand;
	creatureInfo[kWorkAnt].terrain									=	kAnyTerrain;
	creatureInfo[kWorkAnt].alignment								= kEvil;
	creatureInfo[kWorkAnt].directionalGraphics			= true;
	creatureInfo[kWorkAnt].graphicsIndex						= 1;
	creatureInfo[kWorkAnt].corpseID									= 38;
	creatureInfo[kWorkAnt].desiredPopulation				= 100;
	creatureInfo[kWorkAnt].meatEater								= true;
	creatureInfo[kWorkAnt].canMorph					        = true;
	creatureInfo[kWorkAnt].attackSound							= kAntAttackSnd;
	creatureInfo[kWorkAnt].speed										=	fi_GetData(":Creature Settings:Worker Ant", "SPEED=", 70);
	creatureInfo[kWorkAnt].poisonous					      = fi_GetData(":Creature Settings:Worker Ant", "POISONOUS=", 0);
	creatureInfo[kWorkAnt].curse					      		= fi_GetData(":Creature Settings:Worker Ant", "CURSE=", 0);
	creatureInfo[kWorkAnt].maxHitPoints							= fi_GetData(":Creature Settings:Worker Ant", "HIT_POINTS=", 60);
	creatureInfo[kWorkAnt].maxStrength							= fi_GetData(":Creature Settings:Worker Ant", "STRENGTH=", 60);
	creatureInfo[kWorkAnt].maxIntelligence					= fi_GetData(":Creature Settings:Worker Ant", "INTELLIGENCE=", 30);
	creatureInfo[kWorkAnt].maxDexterity							= fi_GetData(":Creature Settings:Worker Ant", "DEXTERITY=", 50);
	creatureInfo[kWorkAnt].minGold								  = fi_GetData(":Creature Settings:Worker Ant", "MIN_GOLD=", 0);
	creatureInfo[kWorkAnt].maxGold								  = fi_GetData(":Creature Settings:Worker Ant", "MAX_GOLD=", 5);
	creatureInfo[kWorkAnt].seekGold									= fi_GetBoolean(":Creature Settings:Worker Ant", "SEEK_GOLD=", true);
	creatureInfo[kWorkAnt].seekMagic								= fi_GetBoolean(":Creature Settings:Worker Ant", "SEEK_MAGIC=", false);
	creatureInfo[kWorkAnt].seekWeapons							= fi_GetBoolean(":Creature Settings:Worker Ant", "SEEK_WEAPONS=", false);
	creatureInfo[kWorkAnt].magicResist				      = (float)fi_GetData(":Creature Settings:Worker Ant", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kWorkAnt].tamingLevel							= fi_GetData(":Creature Settings:Worker Ant", "TAMING_LEVEL=", 4);
	creatureInfo[kWorkAnt].creatureLevel							= fi_GetData(":Creature Settings:Worker Ant", "LEVEL=", 1);
	creatureInfo[kWorkAnt].armorRating							= fi_GetData(":Creature Settings:Worker Ant", "ARMOR_RATING=", 15);
	creatureInfo[kWorkAnt].immuneToPoison           = fi_GetBoolean(":Creature Settings:Worker Ant", "POISON_IMMUNITY=", false);
	creatureInfo[kWorkAnt].lockStrength							= fi_GetData(":Creature Settings:Worker Ant", "LOCK=", 0);
	creatureInfo[kWorkAnt].attackDamageMin					= fi_GetData(":Creature Settings:Worker Ant", "MIN_DAMAGE=", 3);
	creatureInfo[kWorkAnt].attackDamageMax					= fi_GetData(":Creature Settings:Worker Ant", "MAX_DAMAGE=", 7);
	creatureInfo[kWorkAnt].trackingLevel							= fi_GetData(":Creature Settings:Worker Ant", "TRACKING_LEVEL=", 101);
	creatureInfo[kWorkAnt].size									= fi_GetData(":Creature Settings:Worker Ant", "SIZE=", 1);
	creatureInfo[kWorkAnt].mageryLevel					= fi_GetData(":Creature Settings:Worker Ant", "MAGERY_LEVEL=", 0);
	creatureInfo[kWorkAnt].spellcastChance				= fi_GetData(":Creature Settings:Worker Ant", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kWorkAnt].selfhealChance					= fi_GetData(":Creature Settings:Worker Ant", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kWorkAnt].selfhealTrigger				= fi_GetData(":Creature Settings:Worker Ant", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Worker Ant", kWorkAnt);
	fi_GetCreatureTalkScript(":Creature Settings:Worker Ant", kWorkAnt);
	fi_GetCreatureInventory(":Creature Settings:Worker Ant", kWorkAnt);
	fi_GetWeaponModifier(":Creature Settings:Worker Ant", kWorkAnt);

	creatureInfo[kWarriorAnt].bmp											=	1;
	creatureInfo[kWarriorAnt].domain										=	kLand;
	creatureInfo[kWarriorAnt].terrain									=	kAnyTerrain;
	creatureInfo[kWarriorAnt].alignment								= kEvil;
	creatureInfo[kWarriorAnt].directionalGraphics			= true;
	creatureInfo[kWarriorAnt].graphicsIndex						= 1;
	creatureInfo[kWarriorAnt].corpseID									= 38;
	creatureInfo[kWarriorAnt].desiredPopulation				= 100;
	creatureInfo[kWarriorAnt].meatEater								= true;
	creatureInfo[kWarriorAnt].canMorph					        = true;
	creatureInfo[kWarriorAnt].attackSound							= kAntAttackSnd;
	creatureInfo[kWarriorAnt].speed										=	fi_GetData(":Creature Settings:Warrior Ant", "SPEED=", 70);
	creatureInfo[kWarriorAnt].poisonous					      = fi_GetData(":Creature Settings:Warrior Ant", "POISONOUS=", 0);
	creatureInfo[kWarriorAnt].curse					      		= fi_GetData(":Creature Settings:Warrior Ant", "CURSE=", 0);
	creatureInfo[kWarriorAnt].maxHitPoints							= fi_GetData(":Creature Settings:Warrior Ant", "HIT_POINTS=", 60);
	creatureInfo[kWarriorAnt].maxStrength							= fi_GetData(":Creature Settings:Warrior Ant", "STRENGTH=", 60);
	creatureInfo[kWarriorAnt].maxIntelligence					= fi_GetData(":Creature Settings:Warrior Ant", "INTELLIGENCE=", 30);
	creatureInfo[kWarriorAnt].maxDexterity							= fi_GetData(":Creature Settings:Warrior Ant", "DEXTERITY=", 50);
	creatureInfo[kWarriorAnt].minGold								  = fi_GetData(":Creature Settings:Warrior Ant", "MIN_GOLD=", 0);
	creatureInfo[kWarriorAnt].maxGold								  = fi_GetData(":Creature Settings:Warrior Ant", "MAX_GOLD=", 5);
	creatureInfo[kWarriorAnt].seekGold									= fi_GetBoolean(":Creature Settings:Warrior Ant", "SEEK_GOLD=", true);
	creatureInfo[kWarriorAnt].seekMagic								= fi_GetBoolean(":Creature Settings:Warrior Ant", "SEEK_MAGIC=", false);
	creatureInfo[kWarriorAnt].seekWeapons							= fi_GetBoolean(":Creature Settings:Warrior Ant", "SEEK_WEAPONS=", false);
	creatureInfo[kWarriorAnt].magicResist				      = (float)fi_GetData(":Creature Settings:Warrior Ant", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kWarriorAnt].tamingLevel							= fi_GetData(":Creature Settings:Warrior Ant", "TAMING_LEVEL=", 4);
	creatureInfo[kWarriorAnt].creatureLevel							= fi_GetData(":Creature Settings:Warrior Ant", "LEVEL=", 1);
	creatureInfo[kWarriorAnt].armorRating							= fi_GetData(":Creature Settings:Warrior Ant", "ARMOR_RATING=", 15);
	creatureInfo[kWarriorAnt].immuneToPoison           = fi_GetBoolean(":Creature Settings:Warrior Ant", "POISON_IMMUNITY=", false);
	creatureInfo[kWarriorAnt].lockStrength							= fi_GetData(":Creature Settings:Warrior Ant", "LOCK=", 0);
	creatureInfo[kWarriorAnt].attackDamageMin					= fi_GetData(":Creature Settings:Warrior Ant", "MIN_DAMAGE=", 3);
	creatureInfo[kWarriorAnt].attackDamageMax					= fi_GetData(":Creature Settings:Warrior Ant", "MAX_DAMAGE=", 7);
	creatureInfo[kWarriorAnt].trackingLevel							= fi_GetData(":Creature Settings:Warrior Ant", "TRACKING_LEVEL=", 101);
	creatureInfo[kWarriorAnt].size									= fi_GetData(":Creature Settings:Warrior Ant", "SIZE=", 1);
	creatureInfo[kWarriorAnt].mageryLevel					= fi_GetData(":Creature Settings:Warrior Ant", "MAGERY_LEVEL=", 0);
	creatureInfo[kWarriorAnt].spellcastChance				= fi_GetData(":Creature Settings:Warrior Ant", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kWarriorAnt].selfhealChance					= fi_GetData(":Creature Settings:Warrior Ant", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kWarriorAnt].selfhealTrigger				= fi_GetData(":Creature Settings:Warrior Ant", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Warrior Ant", kWarriorAnt);
	fi_GetCreatureTalkScript(":Creature Settings:Warrior Ant", kWarriorAnt);
	fi_GetCreatureInventory(":Creature Settings:Warrior Ant", kWarriorAnt);
	fi_GetWeaponModifier(":Creature Settings:Warrior Ant", kWarriorAnt);

	creatureInfo[kForagerAnt].bmp											=	1;
	creatureInfo[kForagerAnt].domain										=	kLand;
	creatureInfo[kForagerAnt].terrain									=	kAnyTerrain;
	creatureInfo[kForagerAnt].alignment								= kEvil;
	creatureInfo[kForagerAnt].directionalGraphics			= true;
	creatureInfo[kForagerAnt].graphicsIndex						= 1;
	creatureInfo[kForagerAnt].corpseID									= 38;
	creatureInfo[kForagerAnt].desiredPopulation				= 100;
	creatureInfo[kForagerAnt].meatEater								= true;
	creatureInfo[kForagerAnt].canMorph					        = true;
	creatureInfo[kForagerAnt].attackSound							= kAntAttackSnd;
	creatureInfo[kForagerAnt].speed										=	fi_GetData(":Creature Settings:Forager Ant", "SPEED=", 70);
	creatureInfo[kForagerAnt].poisonous					      = fi_GetData(":Creature Settings:Forager Ant", "POISONOUS=", 0);
	creatureInfo[kForagerAnt].curse					      		= fi_GetData(":Creature Settings:Forager Ant", "CURSE=", 0);
	creatureInfo[kForagerAnt].maxHitPoints							= fi_GetData(":Creature Settings:Forager Ant", "HIT_POINTS=", 60);
	creatureInfo[kForagerAnt].maxStrength							= fi_GetData(":Creature Settings:Forager Ant", "STRENGTH=", 60);
	creatureInfo[kForagerAnt].maxIntelligence					= fi_GetData(":Creature Settings:Forager Ant", "INTELLIGENCE=", 30);
	creatureInfo[kForagerAnt].maxDexterity							= fi_GetData(":Creature Settings:Forager Ant", "DEXTERITY=", 50);
	creatureInfo[kForagerAnt].minGold								  = fi_GetData(":Creature Settings:Forager Ant", "MIN_GOLD=", 0);
	creatureInfo[kForagerAnt].maxGold								  = fi_GetData(":Creature Settings:Forager Ant", "MAX_GOLD=", 5);
	creatureInfo[kForagerAnt].seekGold									= fi_GetBoolean(":Creature Settings:Forager Ant", "SEEK_GOLD=", true);
	creatureInfo[kForagerAnt].seekMagic								= fi_GetBoolean(":Creature Settings:Forager Ant", "SEEK_MAGIC=", false);
	creatureInfo[kForagerAnt].seekWeapons							= fi_GetBoolean(":Creature Settings:Forager Ant", "SEEK_WEAPONS=", false);
	creatureInfo[kForagerAnt].magicResist				      = (float)fi_GetData(":Creature Settings:Forager Ant", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kForagerAnt].tamingLevel							= fi_GetData(":Creature Settings:Forager Ant", "TAMING_LEVEL=", 4);
	creatureInfo[kForagerAnt].creatureLevel							= fi_GetData(":Creature Settings:Forager Ant", "LEVEL=", 1);
	creatureInfo[kForagerAnt].armorRating							= fi_GetData(":Creature Settings:Forager Ant", "ARMOR_RATING=", 15);
	creatureInfo[kForagerAnt].immuneToPoison           = fi_GetBoolean(":Creature Settings:Forager Ant", "POISON_IMMUNITY=", false);
	creatureInfo[kForagerAnt].lockStrength							= fi_GetData(":Creature Settings:Forager Ant", "LOCK=", 0);
	creatureInfo[kForagerAnt].attackDamageMin					= fi_GetData(":Creature Settings:Forager Ant", "MIN_DAMAGE=", 3);
	creatureInfo[kForagerAnt].attackDamageMax					= fi_GetData(":Creature Settings:Forager Ant", "MAX_DAMAGE=", 7);
	creatureInfo[kForagerAnt].trackingLevel							= fi_GetData(":Creature Settings:Forager Ant", "TRACKING_LEVEL=", 101);
	creatureInfo[kForagerAnt].size									= fi_GetData(":Creature Settings:Forager Ant", "SIZE=", 1);
	creatureInfo[kForagerAnt].mageryLevel					= fi_GetData(":Creature Settings:Forager Ant", "MAGERY_LEVEL=", 0);
	creatureInfo[kForagerAnt].spellcastChance				= fi_GetData(":Creature Settings:Forager Ant", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kForagerAnt].selfhealChance					= fi_GetData(":Creature Settings:Forager Ant", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kForagerAnt].selfhealTrigger				= fi_GetData(":Creature Settings:Forager Ant", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Forager Ant", kForagerAnt);
	fi_GetCreatureTalkScript(":Creature Settings:Forager Ant", kForagerAnt);
	fi_GetCreatureInventory(":Creature Settings:Forager Ant", kForagerAnt);
	fi_GetWeaponModifier(":Creature Settings:Forager Ant", kForagerAnt);

	creatureInfo[kSnake].bmp											=	3;
	creatureInfo[kSnake].domain										=	kLand;
	creatureInfo[kSnake].terrain									=	kAnyTerrain;
	creatureInfo[kSnake].alignment								= kEvil;
	creatureInfo[kSnake].directionalGraphics			= true;
	creatureInfo[kSnake].graphicsIndex						= 1;
	creatureInfo[kSnake].corpseID									= 97;
	creatureInfo[kSnake].desiredPopulation				= 100;
	creatureInfo[kSnake].meatEater								= true;
	creatureInfo[kSnake].canMorph					        = true;
	creatureInfo[kSnake].attackSound							= kSnakeAttackSnd;
	creatureInfo[kSnake].randomSound							= kSnakeSnd;
	creatureInfo[kSnake].speed										=	fi_GetData(":Creature Settings:Giant Snake", "SPEED=", 70);
	creatureInfo[kSnake].poisonous					      = fi_GetData(":Creature Settings:Giant Snake", "POISONOUS=", 0);
	creatureInfo[kSnake].curse					      		= fi_GetData(":Creature Settings:Giant Snake", "CURSE=", 0);
	creatureInfo[kSnake].maxHitPoints							= fi_GetData(":Creature Settings:Giant Snake", "HIT_POINTS=", 60);
	creatureInfo[kSnake].maxStrength							= fi_GetData(":Creature Settings:Giant Snake", "STRENGTH=", 60);
	creatureInfo[kSnake].maxIntelligence					= fi_GetData(":Creature Settings:Giant Snake", "INTELLIGENCE=", 30);
	creatureInfo[kSnake].maxDexterity							= fi_GetData(":Creature Settings:Giant Snake", "DEXTERITY=", 50);
	creatureInfo[kSnake].minGold								  = fi_GetData(":Creature Settings:Giant Snake", "MIN_GOLD=", 0);
	creatureInfo[kSnake].maxGold								  = fi_GetData(":Creature Settings:Giant Snake", "MAX_GOLD=", 5);
	creatureInfo[kSnake].seekGold									= fi_GetBoolean(":Creature Settings:Giant Snake", "SEEK_GOLD=", true);
	creatureInfo[kSnake].seekMagic								= fi_GetBoolean(":Creature Settings:Giant Snake", "SEEK_MAGIC=", false);
	creatureInfo[kSnake].seekWeapons							= fi_GetBoolean(":Creature Settings:Giant Snake", "SEEK_WEAPONS=", false);
	creatureInfo[kSnake].magicResist				      = (float)fi_GetData(":Creature Settings:Giant Snake", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kSnake].tamingLevel							= fi_GetData(":Creature Settings:Giant Snake", "TAMING_LEVEL=", 4);
	creatureInfo[kSnake].creatureLevel							= fi_GetData(":Creature Settings:Giant Snake", "LEVEL=", 1);
	creatureInfo[kSnake].armorRating							= fi_GetData(":Creature Settings:Giant Snake", "ARMOR_RATING=", 15);
	creatureInfo[kSnake].immuneToPoison           = fi_GetBoolean(":Creature Settings:Giant Snake", "POISON_IMMUNITY=", false);
	creatureInfo[kSnake].lockStrength							= fi_GetData(":Creature Settings:Giant Snake", "LOCK=", 0);
	creatureInfo[kSnake].attackDamageMin					= fi_GetData(":Creature Settings:Giant Snake", "MIN_DAMAGE=", 3);
	creatureInfo[kSnake].attackDamageMax					= fi_GetData(":Creature Settings:Giant Snake", "MAX_DAMAGE=", 7);
	creatureInfo[kSnake].trackingLevel							= fi_GetData(":Creature Settings:Giant Snake", "TRACKING_LEVEL=", 101);
	creatureInfo[kSnake].size									= fi_GetData(":Creature Settings:Giant Snake", "SIZE=", 1);
	creatureInfo[kSnake].mageryLevel					= fi_GetData(":Creature Settings:Giant Snake", "MAGERY_LEVEL=", 0);
	creatureInfo[kSnake].spellcastChance				= fi_GetData(":Creature Settings:Giant Snake", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSnake].selfhealChance					= fi_GetData(":Creature Settings:Giant Snake", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSnake].selfhealTrigger				= fi_GetData(":Creature Settings:Giant Snake", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Giant Snake", kSnake);
	fi_GetCreatureTalkScript(":Creature Settings:Giant Snake", kSnake);
	fi_GetCreatureInventory(":Creature Settings:Giant Snake", kSnake);
	fi_GetWeaponModifier(":Creature Settings:Giant Snake", kSnake);


	creatureInfo[kQueenAnt].bmp											=	3;
	creatureInfo[kQueenAnt].domain										=	kLand;
	creatureInfo[kQueenAnt].terrain									=	kAnyTerrain;
	creatureInfo[kQueenAnt].alignment								= kEvil;
	creatureInfo[kQueenAnt].directionalGraphics			= true;
	creatureInfo[kQueenAnt].graphicsIndex						= 1;
	creatureInfo[kQueenAnt].corpseID									= 96;
	creatureInfo[kQueenAnt].desiredPopulation				= 5;
	creatureInfo[kQueenAnt].meatEater								= true;
	creatureInfo[kQueenAnt].canMorph					        = true;
	creatureInfo[kQueenAnt].attackSound							= kQueenAntAttackSnd;
	creatureInfo[kQueenAnt].speed										=	fi_GetData(":Creature Settings:Queen Ant", "SPEED=", 70);
	creatureInfo[kQueenAnt].poisonous					      = fi_GetData(":Creature Settings:Queen Ant", "POISONOUS=", 0);
	creatureInfo[kQueenAnt].curse					      		= fi_GetData(":Creature Settings:Queen Ant", "CURSE=", 0);
	creatureInfo[kQueenAnt].maxHitPoints							= fi_GetData(":Creature Settings:Queen Ant", "HIT_POINTS=", 60);
	creatureInfo[kQueenAnt].maxStrength							= fi_GetData(":Creature Settings:Queen Ant", "STRENGTH=", 60);
	creatureInfo[kQueenAnt].maxIntelligence					= fi_GetData(":Creature Settings:Queen Ant", "INTELLIGENCE=", 30);
	creatureInfo[kQueenAnt].maxDexterity							= fi_GetData(":Creature Settings:Queen Ant", "DEXTERITY=", 50);
	creatureInfo[kQueenAnt].minGold								  = fi_GetData(":Creature Settings:Queen Ant", "MIN_GOLD=", 0);
	creatureInfo[kQueenAnt].maxGold								  = fi_GetData(":Creature Settings:Queen Ant", "MAX_GOLD=", 5);
	creatureInfo[kQueenAnt].seekGold									= fi_GetBoolean(":Creature Settings:Queen Ant", "SEEK_GOLD=", true);
	creatureInfo[kQueenAnt].seekMagic								= fi_GetBoolean(":Creature Settings:Queen Ant", "SEEK_MAGIC=", false);
	creatureInfo[kQueenAnt].seekWeapons							= fi_GetBoolean(":Creature Settings:Queen Ant", "SEEK_WEAPONS=", false);
	creatureInfo[kQueenAnt].magicResist				      = (float)fi_GetData(":Creature Settings:Queen Ant", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kQueenAnt].tamingLevel							= fi_GetData(":Creature Settings:Queen Ant", "TAMING_LEVEL=", 4);
	creatureInfo[kQueenAnt].creatureLevel							= fi_GetData(":Creature Settings:Queen Ant", "LEVEL=", 1);
	creatureInfo[kQueenAnt].armorRating							= fi_GetData(":Creature Settings:Queen Ant", "ARMOR_RATING=", 15);
	creatureInfo[kQueenAnt].immuneToPoison           = fi_GetBoolean(":Creature Settings:Queen Ant", "POISON_IMMUNITY=", false);
	creatureInfo[kQueenAnt].lockStrength							= fi_GetData(":Creature Settings:Queen Ant", "LOCK=", 0);
	creatureInfo[kQueenAnt].attackDamageMin					= fi_GetData(":Creature Settings:Queen Ant", "MIN_DAMAGE=", 3);
	creatureInfo[kQueenAnt].attackDamageMax					= fi_GetData(":Creature Settings:Queen Ant", "MAX_DAMAGE=", 7);
	creatureInfo[kQueenAnt].trackingLevel							= fi_GetData(":Creature Settings:Queen Ant", "TRACKING_LEVEL=", 101);
	creatureInfo[kQueenAnt].size									= fi_GetData(":Creature Settings:Queen Ant", "SIZE=", 1);
	creatureInfo[kQueenAnt].mageryLevel					= fi_GetData(":Creature Settings:Queen Ant", "MAGERY_LEVEL=", 0);
	creatureInfo[kQueenAnt].spellcastChance				= fi_GetData(":Creature Settings:Queen Ant", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kQueenAnt].selfhealChance					= fi_GetData(":Creature Settings:Queen Ant", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kQueenAnt].selfhealTrigger				= fi_GetData(":Creature Settings:Queen Ant", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Queen Ant", kQueenAnt);
	fi_GetCreatureTalkScript(":Creature Settings:Queen Ant", kQueenAnt);
	fi_GetCreatureInventory(":Creature Settings:Queen Ant", kQueenAnt);
	fi_GetWeaponModifier(":Creature Settings:Queen Ant", kQueenAnt);


	creatureInfo[kBat].bmp											=	1;
	creatureInfo[kBat].domain										=	kAir;
	creatureInfo[kBat].terrain									=	kAnyTerrain;
	creatureInfo[kBat].alignment								= kEvil;
	creatureInfo[kBat].directionalGraphics			= true;
	creatureInfo[kBat].graphicsIndex						= 1;
	creatureInfo[kBat].corpseID									= 33;
	creatureInfo[kBat].desiredPopulation				= 300;
	creatureInfo[kBat].meatEater								= true;
	creatureInfo[kBat].canMorph					        = true;
	creatureInfo[kBat].attackSound							= kBatAttackSnd;
	creatureInfo[kBat].speed										=	fi_GetData(":Creature Settings:Giant Bat", "SPEED=", 75);
	creatureInfo[kBat].poisonous					      = fi_GetData(":Creature Settings:Giant Bat", "POISONOUS=", 0);
	creatureInfo[kBat].curse					      		= fi_GetData(":Creature Settings:Giant Bat", "CURSE=", 0);
	creatureInfo[kBat].maxHitPoints							= fi_GetData(":Creature Settings:Giant Bat", "HIT_POINTS=", 12);
	creatureInfo[kBat].maxStrength							= fi_GetData(":Creature Settings:Giant Bat", "STRENGTH=", 12);
	creatureInfo[kBat].maxIntelligence					= fi_GetData(":Creature Settings:Giant Bat", "INTELLIGENCE=", 12);
	creatureInfo[kBat].maxDexterity							= fi_GetData(":Creature Settings:Giant Bat", "DEXTERITY=", 12);
	creatureInfo[kBat].minGold								  = fi_GetData(":Creature Settings:Giant Bat", "MIN_GOLD=", 0);
	creatureInfo[kBat].maxGold								  = fi_GetData(":Creature Settings:Giant Bat", "MAX_GOLD=", 5);
	creatureInfo[kBat].seekGold									= fi_GetBoolean(":Creature Settings:Giant Bat", "SEEK_GOLD=", true);
	creatureInfo[kBat].seekMagic								= fi_GetBoolean(":Creature Settings:Giant Bat", "SEEK_MAGIC=", false);
	creatureInfo[kBat].seekWeapons							= fi_GetBoolean(":Creature Settings:Giant Bat", "SEEK_WEAPONS=", false);
	creatureInfo[kBat].magicResist				      = (float)fi_GetData(":Creature Settings:Giant Bat", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kBat].tamingLevel							= fi_GetData(":Creature Settings:Giant Bat", "TAMING_LEVEL=", 2);
	creatureInfo[kBat].creatureLevel							= fi_GetData(":Creature Settings:Giant Bat", "LEVEL=", 1);
	creatureInfo[kBat].armorRating							= fi_GetData(":Creature Settings:Giant Bat", "ARMOR_RATING=", 12);
	creatureInfo[kBat].immuneToPoison           = fi_GetBoolean(":Creature Settings:Giant Bat", "POISON_IMMUNITY=", false);
	creatureInfo[kBat].lockStrength							= fi_GetData(":Creature Settings:Giant Bat", "LOCK=", 0);
	creatureInfo[kBat].attackDamageMin					= fi_GetData(":Creature Settings:Giant Bat", "MIN_DAMAGE=", 2);
	creatureInfo[kBat].attackDamageMax					= fi_GetData(":Creature Settings:Giant Bat", "MAX_DAMAGE=", 7);
	creatureInfo[kBat].trackingLevel							= fi_GetData(":Creature Settings:Giant Bat", "TRACKING_LEVEL=", 101);
	creatureInfo[kBat].size									= fi_GetData(":Creature Settings:Giant Bat", "SIZE=", 1);
	creatureInfo[kBat].mageryLevel					= fi_GetData(":Creature Settings:Giant Bat", "MAGERY_LEVEL=", 0);
	creatureInfo[kBat].spellcastChance				= fi_GetData(":Creature Settings:Giant Bat", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kBat].selfhealChance					= fi_GetData(":Creature Settings:Giant Bat", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBat].selfhealTrigger				= fi_GetData(":Creature Settings:Giant Bat", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Giant Bat", kBat);
	//cr_AddToCreatureControlInventory(kBat, kRawSteak, 1, 1);
	fi_GetCreatureTalkScript(":Creature Settings:Giant Bat", kBat);
	fi_GetCreatureInventory(":Creature Settings:Giant Bat", kBat);
	fi_GetWeaponModifier(":Creature Settings:Giant Bat", kBat);

	creatureInfo[kTusker].bmp								=	3;
	creatureInfo[kTusker].domain							=	kLand;
	creatureInfo[kTusker].terrain							=	kAnyTerrain;
	creatureInfo[kTusker].alignment							= kEvil;
	creatureInfo[kTusker].directionalGraphics				= true;
	creatureInfo[kTusker].graphicsIndex						= 1;
	creatureInfo[kTusker].corpseID							= 125;
	creatureInfo[kTusker].attackSound						= kTuskerAttackSnd;
	creatureInfo[kTusker].attackRange						= 1;
	creatureInfo[kTusker].desiredPopulation					= 80;
	//creatureInfo[kTusker].color						      = kPinkDragonColor;
	creatureInfo[kTusker].meatEater							= true;
	creatureInfo[kTusker].points						    = 10;
	creatureInfo[kTusker].rareLevel							= 1;
	creatureInfo[kTusker].rareChance						= 100;
	creatureInfo[kTusker].canMorph					    	= true;
	creatureInfo[kTusker].speed								= fi_GetData(":Creature Settings:Tusker", "SPEED=", 50);
	creatureInfo[kTusker].poisonous					      	= fi_GetData(":Creature Settings:Tusker", "POISONOUS=", 0);
	creatureInfo[kTusker].curse					      		= fi_GetData(":Creature Settings:Tusker", "CURSE=", 0);
	creatureInfo[kTusker].maxHitPoints						= fi_GetData(":Creature Settings:Tusker", "HIT_POINTS=", 250);
	creatureInfo[kTusker].maxStrength						= fi_GetData(":Creature Settings:Tusker", "STRENGTH=", 250);
	creatureInfo[kTusker].maxIntelligence					= fi_GetData(":Creature Settings:Tusker", "INTELLIGENCE=", 250);
	creatureInfo[kTusker].maxDexterity						= fi_GetData(":Creature Settings:Tusker", "DEXTERITY=", 250);
	creatureInfo[kTusker].minGold							= fi_GetData(":Creature Settings:Tusker", "MIN_GOLD=", 200);
	creatureInfo[kTusker].maxGold							= fi_GetData(":Creature Settings:Tusker", "MAX_GOLD=", 500);
	creatureInfo[kTusker].seekGold							= fi_GetBoolean(":Creature Settings:Tusker", "SEEK_GOLD=", true);
	creatureInfo[kTusker].seekMagic							= fi_GetBoolean(":Creature Settings:Tusker", "SEEK_MAGIC=", false);
	creatureInfo[kTusker].seekWeapons						= fi_GetBoolean(":Creature Settings:Tusker", "SEEK_WEAPONS=", true);
	creatureInfo[kTusker].magicResist				      	= (float)fi_GetData(":Creature Settings:Tusker", "MAGIC_RESIST=", 0)/100.0;
	creatureInfo[kTusker].tamingLevel						= fi_GetData(":Creature Settings:Tusker", "TAMING_LEVEL=", 101);
	creatureInfo[kTusker].creatureLevel						= fi_GetData(":Creature Settings:Tusker", "LEVEL=", 1);
	creatureInfo[kTusker].armorRating						= fi_GetData(":Creature Settings:Tusker", "ARMOR_RATING=", 100);
	creatureInfo[kTusker].immuneToPoison           			= fi_GetBoolean(":Creature Settings:Tusker", "POISON_IMMUNITY=", true);
	creatureInfo[kTusker].lockStrength						= fi_GetData(":Creature Settings:Tusker", "LOCK=", 5);
	creatureInfo[kTusker].attackDamageMin					= fi_GetData(":Creature Settings:Tusker", "MIN_DAMAGE=", 250);
	creatureInfo[kTusker].attackDamageMax					= fi_GetData(":Creature Settings:Tusker", "MAX_DAMAGE=", 250);
	creatureInfo[kTusker].trackingLevel						= fi_GetData(":Creature Settings:Tusker", "TRACKING_LEVEL=", 101);
	creatureInfo[kTusker].size								= fi_GetData(":Creature Settings:Tusker", "SIZE=", 3);
	creatureInfo[kTusker].mageryLevel						= fi_GetData(":Creature Settings:Tusker", "MAGERY_LEVEL=", 0);
	creatureInfo[kTusker].spellcastChance					= fi_GetData(":Creature Settings:Tusker", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kTusker].selfhealChance					= fi_GetData(":Creature Settings:Tusker", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kTusker].selfhealTrigger					= fi_GetData(":Creature Settings:Tusker", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Tusker", kTusker);
	fi_GetCreatureTalkScript(":Creature Settings:Tusker", kTusker);
	fi_GetCreatureInventory(":Creature Settings:Tusker", kTusker);
	fi_GetWeaponModifier(":Creature Settings:Tusker", kTusker);
	
	creatureInfo[kLion].bmp								=	4;
	creatureInfo[kLion].domain								=	kLand;
	creatureInfo[kLion].terrain							=	kAnyTerrain;
	creatureInfo[kLion].alignment						= kEvil;
	creatureInfo[kLion].directionalGraphics	= true;
	creatureInfo[kLion].graphicsIndex				= 1;
	creatureInfo[kLion].attackSound							= kLionAttackSnd;
	creatureInfo[kLion].corpseID							= 138;
	creatureInfo[kLion].attackRange					= 1;
	creatureInfo[kLion].desiredPopulation		= 1500;
	creatureInfo[kLion].meatEater						= true;
	creatureInfo[kLion].points						    = 10;
	creatureInfo[kLion].rareLevel						= 1;
	creatureInfo[kLion].rareChance						= 100;
	creatureInfo[kLion].canMorph					    = true;
	creatureInfo[kLion].speed										=	fi_GetData(":Creature Settings:Lion", "SPEED=", 50);
	creatureInfo[kLion].poisonous					      = fi_GetData(":Creature Settings:Lion", "POISONOUS=", 0);
	creatureInfo[kLion].curse					      		= fi_GetData(":Creature Settings:Lion", "CURSE=", 0);
	creatureInfo[kLion].maxHitPoints							= fi_GetData(":Creature Settings:Lion", "HIT_POINTS=", 200);
	creatureInfo[kLion].maxStrength							= fi_GetData(":Creature Settings:Lion", "STRENGTH=", 200);
	creatureInfo[kLion].maxIntelligence					= fi_GetData(":Creature Settings:Lion", "INTELLIGENCE=", 200);
	creatureInfo[kLion].maxDexterity							= fi_GetData(":Creature Settings:Lion", "DEXTERITY=", 200);
	creatureInfo[kLion].minGold								  = fi_GetData(":Creature Settings:Lion", "MIN_GOLD=", 200);
	creatureInfo[kLion].maxGold								  = fi_GetData(":Creature Settings:Lion", "MAX_GOLD=", 500);
	creatureInfo[kLion].seekGold									= fi_GetBoolean(":Creature Settings:Lion", "SEEK_GOLD=", true);
	creatureInfo[kLion].seekMagic								= fi_GetBoolean(":Creature Settings:Lion", "SEEK_MAGIC=", false);
	creatureInfo[kLion].seekWeapons							= fi_GetBoolean(":Creature Settings:Lion", "SEEK_WEAPONS=", true);
	creatureInfo[kLion].magicResist				      = (float)fi_GetData(":Creature Settings:Lion", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kLion].tamingLevel							= fi_GetData(":Creature Settings:Lion", "TAMING_LEVEL=", 8);
	creatureInfo[kLion].creatureLevel							= fi_GetData(":Creature Settings:Lion", "LEVEL=", 1);
	creatureInfo[kLion].armorRating							= fi_GetData(":Creature Settings:Lion", "ARMOR_RATING=", 75);
	creatureInfo[kLion].immuneToPoison           = fi_GetBoolean(":Creature Settings:Lion", "POISON_IMMUNITY=", false);
	creatureInfo[kLion].lockStrength							= fi_GetData(":Creature Settings:Lion", "LOCK=", 5);
	creatureInfo[kLion].attackDamageMin					= fi_GetData(":Creature Settings:Lion", "MIN_DAMAGE=", 60);
	creatureInfo[kLion].attackDamageMax					= fi_GetData(":Creature Settings:Lion", "MAX_DAMAGE=", 150);
	creatureInfo[kLion].trackingLevel							= fi_GetData(":Creature Settings:Lion", "TRACKING_LEVEL=", 101);
	creatureInfo[kLion].size									= fi_GetData(":Creature Settings:Lion", "SIZE=", 2);
	creatureInfo[kLion].mageryLevel					= fi_GetData(":Creature Settings:Lion", "MAGERY_LEVEL=", 15);
	creatureInfo[kLion].spellcastChance				= fi_GetData(":Creature Settings:Lion", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kLion].selfhealChance				= fi_GetData(":Creature Settings:Lion", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kLion].selfhealTrigger				= fi_GetData(":Creature Settings:Lion", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Lion", kLion);
	fi_GetCreatureTalkScript(":Creature Settings:Lion", kLion);
	fi_GetCreatureInventory(":Creature Settings:Lion", kLion);
	fi_GetWeaponModifier(":Creature Settings:Lion", kLion);
	
	creatureInfo[kGolem].bmp								=	4;
	creatureInfo[kGolem].domain								=	kLand;
	creatureInfo[kGolem].terrain							=	kAnyTerrain;
	creatureInfo[kGolem].alignment						= kEvil;
	creatureInfo[kGolem].directionalGraphics	= true;
	creatureInfo[kGolem].graphicsIndex				= 1;
	creatureInfo[kGolem].attackSound							= kGolemAttackSnd;
	creatureInfo[kGolem].corpseID							= 142;
	creatureInfo[kGolem].attackRange					= 1;
	creatureInfo[kGolem].desiredPopulation		= 100;
	creatureInfo[kGolem].meatEater						= false;
	creatureInfo[kGolem].points						    = 10;
	creatureInfo[kGolem].rareLevel						= 1;
	creatureInfo[kGolem].rareChance						= 100;
	creatureInfo[kGolem].canMorph					    = true;
	creatureInfo[kGolem].speed										=	fi_GetData(":Creature Settings:Stone Golem", "SPEED=", 50);
	creatureInfo[kGolem].poisonous					      = fi_GetData(":Creature Settings:Stone Golem", "POISONOUS=", 0);
	creatureInfo[kGolem].curse					      		= fi_GetData(":Creature Settings:Stone Golem", "CURSE=", 0);
	creatureInfo[kGolem].maxHitPoints							= fi_GetData(":Creature Settings:Stone Golem", "HIT_POINTS=", 200);
	creatureInfo[kGolem].maxStrength							= fi_GetData(":Creature Settings:Stone Golem", "STRENGTH=", 200);
	creatureInfo[kGolem].maxIntelligence					= fi_GetData(":Creature Settings:Stone Golem", "INTELLIGENCE=", 200);
	creatureInfo[kGolem].maxDexterity							= fi_GetData(":Creature Settings:Stone Golem", "DEXTERITY=", 200);
	creatureInfo[kGolem].minGold								  = fi_GetData(":Creature Settings:Stone Golem", "MIN_GOLD=", 200);
	creatureInfo[kGolem].maxGold								  = fi_GetData(":Creature Settings:Stone Golem", "MAX_GOLD=", 500);
	creatureInfo[kGolem].seekGold									= fi_GetBoolean(":Creature Settings:Stone Golem", "SEEK_GOLD=", true);
	creatureInfo[kGolem].seekMagic								= fi_GetBoolean(":Creature Settings:Stone Golem", "SEEK_MAGIC=", false);
	creatureInfo[kGolem].seekWeapons							= fi_GetBoolean(":Creature Settings:Stone Golem", "SEEK_WEAPONS=", true);
	creatureInfo[kGolem].magicResist				      = (float)fi_GetData(":Creature Settings:Stone Golem", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kGolem].tamingLevel							= fi_GetData(":Creature Settings:Stone Golem", "TAMING_LEVEL=", 8);
	creatureInfo[kGolem].creatureLevel							= fi_GetData(":Creature Settings:Stone Golem", "LEVEL=", 1);
	creatureInfo[kGolem].armorRating							= fi_GetData(":Creature Settings:Stone Golem", "ARMOR_RATING=", 75);
	creatureInfo[kGolem].immuneToPoison           = fi_GetBoolean(":Creature Settings:Stone Golem", "POISON_IMMUNITY=", false);
	creatureInfo[kGolem].lockStrength							= fi_GetData(":Creature Settings:Stone Golem", "LOCK=", 5);
	creatureInfo[kGolem].attackDamageMin					= fi_GetData(":Creature Settings:Stone Golem", "MIN_DAMAGE=", 60);
	creatureInfo[kGolem].attackDamageMax					= fi_GetData(":Creature Settings:Stone Golem", "MAX_DAMAGE=", 150);
	creatureInfo[kGolem].trackingLevel							= fi_GetData(":Creature Settings:Stone Golem", "TRACKING_LEVEL=", 101);
	creatureInfo[kGolem].size									= fi_GetData(":Creature Settings:Stone Golem", "SIZE=", 2);
	creatureInfo[kGolem].mageryLevel					= fi_GetData(":Creature Settings:Stone Golem", "MAGERY_LEVEL=", 15);
	creatureInfo[kGolem].spellcastChance				= fi_GetData(":Creature Settings:Stone Golem", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kGolem].selfhealChance				= fi_GetData(":Creature Settings:Stone Golem", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kGolem].selfhealTrigger				= fi_GetData(":Creature Settings:Stone Golem", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Stone Golem", kGolem);
	fi_GetCreatureTalkScript(":Creature Settings:Stone Golem", kGolem);
	fi_GetCreatureInventory(":Creature Settings:Stone Golem", kGolem);
	fi_GetWeaponModifier(":Creature Settings:Stone Golem", kGolem);
	
	//Dave
	
	creatureInfo[kIceGolem].bmp								=	4;
	creatureInfo[kIceGolem].domain								=	kLand;
	creatureInfo[kIceGolem].terrain							=	kAnyTerrain;
	creatureInfo[kIceGolem].alignment						= kEvil;
	creatureInfo[kIceGolem].directionalGraphics	= true;
	creatureInfo[kIceGolem].graphicsIndex				= 1;
	creatureInfo[kIceGolem].attackSound							= kGolemAttackSnd;
	creatureInfo[kIceGolem].corpseID							= 183;
	creatureInfo[kIceGolem].attackRange					= 1;
	creatureInfo[kIceGolem].desiredPopulation		= 50;
	creatureInfo[kIceGolem].meatEater						= false;
	creatureInfo[kIceGolem].points						    = 10;
	creatureInfo[kIceGolem].color						      = kIceDragonColor;
	creatureInfo[kIceGolem].rareLevel						= 1;
	creatureInfo[kIceGolem].rareChance						= 100;
	creatureInfo[kIceGolem].canMorph					    = true;
	creatureInfo[kIceGolem].speed										=	fi_GetData(":Creature Settings:Ice Golem", "SPEED=", 50);
	creatureInfo[kIceGolem].poisonous					      = fi_GetData(":Creature Settings:Ice Golem", "POISONOUS=", 0);
	creatureInfo[kIceGolem].curse					      		= fi_GetData(":Creature Settings:Ice Golem", "CURSE=", 0);
	creatureInfo[kIceGolem].maxHitPoints							= fi_GetData(":Creature Settings:Ice Golem", "HIT_POINTS=", 200);
	creatureInfo[kIceGolem].maxStrength							= fi_GetData(":Creature Settings:Ice Golem", "STRENGTH=", 200);
	creatureInfo[kIceGolem].maxIntelligence					= fi_GetData(":Creature Settings:Ice Golem", "INTELLIGENCE=", 200);
	creatureInfo[kIceGolem].maxDexterity							= fi_GetData(":Creature Settings:Ice Golem", "DEXTERITY=", 200);
	creatureInfo[kIceGolem].minGold								  = fi_GetData(":Creature Settings:Ice Golem", "MIN_GOLD=", 200);
	creatureInfo[kIceGolem].maxGold								  = fi_GetData(":Creature Settings:Ice Golem", "MAX_GOLD=", 500);
	creatureInfo[kIceGolem].seekGold									= fi_GetBoolean(":Creature Settings:Ice Golem", "SEEK_GOLD=", true);
	creatureInfo[kIceGolem].seekMagic								= fi_GetBoolean(":Creature Settings:Ice Golem", "SEEK_MAGIC=", false);
	creatureInfo[kIceGolem].seekWeapons							= fi_GetBoolean(":Creature Settings:Ice Golem", "SEEK_WEAPONS=", true);
	creatureInfo[kIceGolem].magicResist				      = (float)fi_GetData(":Creature Settings:Ice Golem", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kIceGolem].tamingLevel							= fi_GetData(":Creature Settings:Ice Golem", "TAMING_LEVEL=", 8);
	creatureInfo[kIceGolem].creatureLevel							= fi_GetData(":Creature Settings:Ice Golem", "LEVEL=", 1);
	creatureInfo[kIceGolem].armorRating							= fi_GetData(":Creature Settings:Ice Golem", "ARMOR_RATING=", 75);
	creatureInfo[kIceGolem].immuneToPoison           = fi_GetBoolean(":Creature Settings:Ice Golem", "POISON_IMMUNITY=", false);
	creatureInfo[kIceGolem].lockStrength							= fi_GetData(":Creature Settings:Ice Golem", "LOCK=", 5);
	creatureInfo[kIceGolem].attackDamageMin					= fi_GetData(":Creature Settings:Ice Golem", "MIN_DAMAGE=", 60);
	creatureInfo[kIceGolem].attackDamageMax					= fi_GetData(":Creature Settings:Ice Golem", "MAX_DAMAGE=", 150);
	creatureInfo[kIceGolem].trackingLevel							= fi_GetData(":Creature Settings:Ice Golem", "TRACKING_LEVEL=", 101);
	creatureInfo[kIceGolem].size									= fi_GetData(":Creature Settings:Ice Golem", "SIZE=", 2);
	creatureInfo[kIceGolem].mageryLevel					= fi_GetData(":Creature Settings:Ice Golem", "MAGERY_LEVEL=", 15);
	creatureInfo[kIceGolem].spellcastChance				= fi_GetData(":Creature Settings:Ice Golem", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kIceGolem].selfhealChance				= fi_GetData(":Creature Settings:Ice Golem", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kIceGolem].selfhealTrigger				= fi_GetData(":Creature Settings:Ice Golem", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Ice Golem", kIceGolem);
	fi_GetCreatureTalkScript(":Creature Settings:Ice Golem", kIceGolem);
	fi_GetCreatureInventory(":Creature Settings:Ice Golem", kIceGolem);
	fi_GetWeaponModifier(":Creature Settings:Ice Golem", kIceGolem);
	
	creatureInfo[kMagmaGolem].bmp								=	4;
	creatureInfo[kMagmaGolem].domain								=	kLand;
	creatureInfo[kMagmaGolem].terrain							=	kAnyTerrain;
	creatureInfo[kMagmaGolem].alignment						= kEvil;
	creatureInfo[kMagmaGolem].directionalGraphics	= true;
	creatureInfo[kMagmaGolem].graphicsIndex				= 1;
	creatureInfo[kMagmaGolem].attackSound							= kGolemAttackSnd;
	creatureInfo[kMagmaGolem].corpseID							= 184;
	creatureInfo[kMagmaGolem].attackRange					= 1;
	creatureInfo[kMagmaGolem].desiredPopulation		= 50;
	creatureInfo[kMagmaGolem].meatEater						= false;
	creatureInfo[kMagmaGolem].points						    = 10;
	creatureInfo[kMagmaGolem].color						      = kRedDragonColor;
	creatureInfo[kMagmaGolem].rareLevel						= 1;
	creatureInfo[kMagmaGolem].rareChance						= 100;
	creatureInfo[kMagmaGolem].canMorph					    = true;
	creatureInfo[kMagmaGolem].speed										=	fi_GetData(":Creature Settings:Magma Golem", "SPEED=", 50);
	creatureInfo[kMagmaGolem].poisonous					      = fi_GetData(":Creature Settings:Magma Golem", "POISONOUS=", 0);
	creatureInfo[kMagmaGolem].curse					      		= fi_GetData(":Creature Settings:Magma Golem", "CURSE=", 0);
	creatureInfo[kMagmaGolem].maxHitPoints							= fi_GetData(":Creature Settings:Magma Golem", "HIT_POINTS=", 200);
	creatureInfo[kMagmaGolem].maxStrength							= fi_GetData(":Creature Settings:Magma Golem", "STRENGTH=", 200);
	creatureInfo[kMagmaGolem].maxIntelligence					= fi_GetData(":Creature Settings:Magma Golem", "INTELLIGENCE=", 200);
	creatureInfo[kMagmaGolem].maxDexterity							= fi_GetData(":Creature Settings:Magma Golem", "DEXTERITY=", 200);
	creatureInfo[kMagmaGolem].minGold								  = fi_GetData(":Creature Settings:Magma Golem", "MIN_GOLD=", 200);
	creatureInfo[kMagmaGolem].maxGold								  = fi_GetData(":Creature Settings:Magma Golem", "MAX_GOLD=", 500);
	creatureInfo[kMagmaGolem].seekGold									= fi_GetBoolean(":Creature Settings:Magma Golem", "SEEK_GOLD=", true);
	creatureInfo[kMagmaGolem].seekMagic								= fi_GetBoolean(":Creature Settings:Magma Golem", "SEEK_MAGIC=", false);
	creatureInfo[kMagmaGolem].seekWeapons							= fi_GetBoolean(":Creature Settings:Magma Golem", "SEEK_WEAPONS=", true);
	creatureInfo[kMagmaGolem].magicResist				      = (float)fi_GetData(":Creature Settings:Magma Golem", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kMagmaGolem].tamingLevel							= fi_GetData(":Creature Settings:Magma Golem", "TAMING_LEVEL=", 8);
	creatureInfo[kMagmaGolem].creatureLevel							= fi_GetData(":Creature Settings:Magma Golem", "LEVEL=", 1);
	creatureInfo[kMagmaGolem].armorRating							= fi_GetData(":Creature Settings:Magma Golem", "ARMOR_RATING=", 75);
	creatureInfo[kMagmaGolem].immuneToPoison           = fi_GetBoolean(":Creature Settings:Magma Golem", "POISON_IMMUNITY=", false);
	creatureInfo[kMagmaGolem].lockStrength							= fi_GetData(":Creature Settings:Magma Golem", "LOCK=", 5);
	creatureInfo[kMagmaGolem].attackDamageMin					= fi_GetData(":Creature Settings:Magma Golem", "MIN_DAMAGE=", 60);
	creatureInfo[kMagmaGolem].attackDamageMax					= fi_GetData(":Creature Settings:Magma Golem", "MAX_DAMAGE=", 150);
	creatureInfo[kMagmaGolem].trackingLevel							= fi_GetData(":Creature Settings:Magma Golem", "TRACKING_LEVEL=", 101);
	creatureInfo[kMagmaGolem].size									= fi_GetData(":Creature Settings:Magma Golem", "SIZE=", 2);
	creatureInfo[kMagmaGolem].mageryLevel					= fi_GetData(":Creature Settings:Magma Golem", "MAGERY_LEVEL=", 15);
	creatureInfo[kMagmaGolem].spellcastChance				= fi_GetData(":Creature Settings:Magma Golem", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kMagmaGolem].selfhealChance				= fi_GetData(":Creature Settings:Magma Golem", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kMagmaGolem].selfhealTrigger				= fi_GetData(":Creature Settings:Magma Golem", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Magma Golem", kMagmaGolem);
	fi_GetCreatureTalkScript(":Creature Settings:Magma Golem", kMagmaGolem);
	fi_GetCreatureInventory(":Creature Settings:Magma Golem", kMagmaGolem);
	fi_GetWeaponModifier(":Creature Settings:Magma Golem", kMagmaGolem);
	
	creatureInfo[kSandGolem].bmp								=	4;
	creatureInfo[kSandGolem].domain								=	kLand;
	creatureInfo[kSandGolem].terrain							=	kAnyTerrain;
	creatureInfo[kSandGolem].alignment							= kEvil;
	creatureInfo[kSandGolem].directionalGraphics				= true;
	creatureInfo[kSandGolem].graphicsIndex						= 1;
	creatureInfo[kSandGolem].attackSound							= kGolemAttackSnd;
	creatureInfo[kSandGolem].corpseID							= 193;
	creatureInfo[kSandGolem].attackRange					= 1;
	creatureInfo[kSandGolem].desiredPopulation		= 100;
	creatureInfo[kSandGolem].meatEater						= false;
	creatureInfo[kSandGolem].points						    = 10;
	creatureInfo[kSandGolem].color						      = kSandSpiderColor;
	creatureInfo[kSandGolem].rareLevel						= 1;
	creatureInfo[kSandGolem].rareChance						= 100;
	creatureInfo[kSandGolem].canMorph					    = true;
	creatureInfo[kSandGolem].speed										=	fi_GetData(":Creature Settings:Sand Golem", "SPEED=", 50);
	creatureInfo[kSandGolem].poisonous					      = fi_GetData(":Creature Settings:Sand Golem", "POISONOUS=", 0);
	creatureInfo[kSandGolem].curse					      		= fi_GetData(":Creature Settings:Sand Golem", "CURSE=", 0);
	creatureInfo[kSandGolem].maxHitPoints							= fi_GetData(":Creature Settings:Sand Golem", "HIT_POINTS=", 200);
	creatureInfo[kSandGolem].maxStrength							= fi_GetData(":Creature Settings:Sand Golem", "STRENGTH=", 200);
	creatureInfo[kSandGolem].maxIntelligence					= fi_GetData(":Creature Settings:Sand Golem", "INTELLIGENCE=", 200);
	creatureInfo[kSandGolem].maxDexterity							= fi_GetData(":Creature Settings:Sand Golem", "DEXTERITY=", 200);
	creatureInfo[kSandGolem].minGold								  = fi_GetData(":Creature Settings:Sand Golem", "MIN_GOLD=", 200);
	creatureInfo[kSandGolem].maxGold								  = fi_GetData(":Creature Settings:Sand Golem", "MAX_GOLD=", 500);
	creatureInfo[kSandGolem].seekGold									= fi_GetBoolean(":Creature Settings:Sand Golem", "SEEK_GOLD=", true);
	creatureInfo[kSandGolem].seekMagic								= fi_GetBoolean(":Creature Settings:Sand Golem", "SEEK_MAGIC=", false);
	creatureInfo[kSandGolem].seekWeapons							= fi_GetBoolean(":Creature Settings:Sand Golem", "SEEK_WEAPONS=", true);
	creatureInfo[kSandGolem].magicResist				      = (float)fi_GetData(":Creature Settings:Sand Golem", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kSandGolem].tamingLevel							= fi_GetData(":Creature Settings:Sand Golem", "TAMING_LEVEL=", 8);
	creatureInfo[kSandGolem].creatureLevel							= fi_GetData(":Creature Settings:Sand Golem", "LEVEL=", 1);
	creatureInfo[kSandGolem].armorRating							= fi_GetData(":Creature Settings:Sand Golem", "ARMOR_RATING=", 75);
	creatureInfo[kSandGolem].immuneToPoison           = fi_GetBoolean(":Creature Settings:Sand Golem", "POISON_IMMUNITY=", false);
	creatureInfo[kSandGolem].lockStrength							= fi_GetData(":Creature Settings:Sand Golem", "LOCK=", 5);
	creatureInfo[kSandGolem].attackDamageMin					= fi_GetData(":Creature Settings:Sand Golem", "MIN_DAMAGE=", 60);
	creatureInfo[kSandGolem].attackDamageMax					= fi_GetData(":Creature Settings:Sand Golem", "MAX_DAMAGE=", 150);
	creatureInfo[kSandGolem].trackingLevel							= fi_GetData(":Creature Settings:Sand Golem", "TRACKING_LEVEL=", 101);
	creatureInfo[kSandGolem].size									= fi_GetData(":Creature Settings:Sand Golem", "SIZE=", 2);
	creatureInfo[kSandGolem].mageryLevel					= fi_GetData(":Creature Settings:Sand Golem", "MAGERY_LEVEL=", 15);
	creatureInfo[kSandGolem].spellcastChance				= fi_GetData(":Creature Settings:Sand Golem", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSandGolem].selfhealChance				= fi_GetData(":Creature Settings:Sand Golem", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSandGolem].selfhealTrigger				= fi_GetData(":Creature Settings:Sand Golem", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Sand Golem", kSandGolem);
	fi_GetCreatureTalkScript(":Creature Settings:Sand Golem", kSandGolem);
	fi_GetCreatureInventory(":Creature Settings:Sand Golem", kSandGolem);
	fi_GetWeaponModifier(":Creature Settings:Sand Golem", kSandGolem);
	
	
	creatureInfo[kPixie].bmp										=	3;
	creatureInfo[kPixie].domain								=	kAir;
	creatureInfo[kPixie].alignment							= kEvil;
	creatureInfo[kPixie].directionalGraphics		= true;
	creatureInfo[kPixie].graphicsIndex					= 1;
	creatureInfo[kPixie].corpseID							= 192;
	creatureInfo[kPixie].desiredPopulation			= 300;
	creatureInfo[kPixie].attackSound						= kPixieAttackSnd;
	creatureInfo[kPixie].randomSound							= kPixieAttackSnd;
	creatureInfo[kPixie].attackRange					= 3;
	creatureInfo[kPixie].vegEater									= true;
	creatureInfo[kPixie].points							  = 3;
	creatureInfo[kPixie].canMorph					    = true;
	//creatureInfo[kPixie].color						      = kPoisonElementalColor;
	creatureInfo[kPixie].speed										=	fi_GetData(":Creature Settings:Pixie", "SPEED=", 60);
	creatureInfo[kPixie].poisonous					      = fi_GetData(":Creature Settings:Pixie", "POISONOUS=", 0);
	creatureInfo[kPixie].curse					      		= fi_GetData(":Creature Settings:Pixie", "CURSE=", 0);
	creatureInfo[kPixie].maxHitPoints						= fi_GetData(":Creature Settings:Pixie", "HIT_POINTS=", 60);
	creatureInfo[kPixie].maxStrength							= fi_GetData(":Creature Settings:Pixie", "STRENGTH=", 60);
	creatureInfo[kPixie].maxIntelligence					= fi_GetData(":Creature Settings:Pixie", "INTELLIGENCE=", 50);
	creatureInfo[kPixie].maxDexterity						= fi_GetData(":Creature Settings:Pixie", "DEXTERITY=", 50);
	creatureInfo[kPixie].minGold								  = fi_GetData(":Creature Settings:Pixie", "MIN_GOLD=", 10);
	creatureInfo[kPixie].maxGold								  = fi_GetData(":Creature Settings:Pixie", "MAX_GOLD=", 18);
	creatureInfo[kPixie].seekGold								= fi_GetBoolean(":Creature Settings:Pixie", "SEEK_GOLD=", true);
	creatureInfo[kPixie].seekMagic								= fi_GetBoolean(":Creature Settings:Pixie", "SEEK_MAGIC=", false);
	creatureInfo[kPixie].seekWeapons							= fi_GetBoolean(":Creature Settings:Pixie", "SEEK_WEAPONS=", false);
	creatureInfo[kPixie].magicResist				      = (float)fi_GetData(":Creature Settings:Pixie", "MAGIC_RESIST=", 80)/100.0;
	creatureInfo[kPixie].tamingLevel							= fi_GetData(":Creature Settings:Pixie", "TAMING_LEVEL=", 4);
	creatureInfo[kPixie].creatureLevel							= fi_GetData(":Creature Settings:Pixie", "LEVEL=", 1);
	creatureInfo[kPixie].armorRating							= fi_GetData(":Creature Settings:Pixie", "ARMOR_RATING=", 30);
	creatureInfo[kPixie].immuneToPoison          = fi_GetBoolean(":Creature Settings:Pixie", "POISON_IMMUNITY=", false);
	creatureInfo[kPixie].lockStrength						= fi_GetData(":Creature Settings:Pixie", "LOCK=", 0);
	creatureInfo[kPixie].attackDamageMin					= fi_GetData(":Creature Settings:Pixie", "MIN_DAMAGE=", 4);
	creatureInfo[kPixie].attackDamageMax					= fi_GetData(":Creature Settings:Pixie", "MAX_DAMAGE=", 7);
	creatureInfo[kPixie].trackingLevel							= fi_GetData(":Creature Settings:Pixie", "TRACKING_LEVEL=", 101);
	creatureInfo[kPixie].size									= fi_GetData(":Creature Settings:Pixie", "SIZE=", 1);
	creatureInfo[kPixie].mageryLevel					= fi_GetData(":Creature Settings:Pixie", "MAGERY_LEVEL=", 0);
	creatureInfo[kPixie].spellcastChance				= fi_GetData(":Creature Settings:Pixie", "SPELLCAST_CHANCE=", 100);
	creatureInfo[kPixie].selfhealChance					= fi_GetData(":Creature Settings:Pixie", "SELFHEAL_CHANCE=", 20);
	creatureInfo[kPixie].selfhealTrigger				= fi_GetData(":Creature Settings:Pixie", "SELFHEAL_TRIGGER=", 10);
	fi_GetSpellList(":Creature Settings:Pixie", kPixie);
	fi_GetCreatureTalkScript(":Creature Settings:Pixie", kPixie);
	fi_GetCreatureInventory(":Creature Settings:Pixie", kPixie);
	fi_GetWeaponModifier(":Creature Settings:Pixie", kPixie);
	
	
	creatureInfo[kSandDragon].domain								=	kLand;
	creatureInfo[kSandDragon].terrain							=	kAnyTerrain;
	creatureInfo[kSandDragon].alignment						= kEvil;
	creatureInfo[kSandDragon].directionalGraphics	= true;
	creatureInfo[kSandDragon].graphicsIndex				= 1;
	creatureInfo[kSandDragon].corpseID							= 194;
	creatureInfo[kSandDragon].randomSound					= kDragonGrowl;
	creatureInfo[kSandDragon].attackSound					= kDragonAttack;
	creatureInfo[kSandDragon].attackRange					= 3;
	creatureInfo[kSandDragon].desiredPopulation		= 8;
	creatureInfo[kSandDragon].meatEater						= true;
	creatureInfo[kSandDragon].points						    = 10;
	creatureInfo[kSandDragon].rareLevel						= 1;
	creatureInfo[kSandDragon].rareChance						= 100;
	creatureInfo[kSandDragon].color						    = kSandSpiderColor;
	creatureInfo[kSandDragon].canMorph					    = true;
	creatureInfo[kSandDragon].frost					    	= true;
	creatureInfo[kSandDragon].speed										=	fi_GetData(":Creature Settings:Sand Dragon", "SPEED=", 50);
	creatureInfo[kSandDragon].poisonous					      = fi_GetData(":Creature Settings:Sand Dragon", "POISONOUS=", 0);
	creatureInfo[kSandDragon].curse					      		= fi_GetData(":Creature Settings:Sand Dragon", "CURSE=", 0);
	creatureInfo[kSandDragon].maxHitPoints							= fi_GetData(":Creature Settings:Sand Dragon", "HIT_POINTS=", 200);
	creatureInfo[kSandDragon].maxStrength							= fi_GetData(":Creature Settings:Sand Dragon", "STRENGTH=", 200);
	creatureInfo[kSandDragon].maxIntelligence					= fi_GetData(":Creature Settings:Sand Dragon", "INTELLIGENCE=", 200);
	creatureInfo[kSandDragon].maxDexterity							= fi_GetData(":Creature Settings:Sand Dragon", "DEXTERITY=", 50);
	creatureInfo[kSandDragon].minGold								  = fi_GetData(":Creature Settings:Sand Dragon", "MIN_GOLD=", 200);
	creatureInfo[kSandDragon].maxGold								  = fi_GetData(":Creature Settings:Sand Dragon", "MAX_GOLD=", 500);
	creatureInfo[kSandDragon].seekGold									= fi_GetBoolean(":Creature Settings:Sand Dragon", "SEEK_GOLD=", true);
	creatureInfo[kSandDragon].seekMagic								= fi_GetBoolean(":Creature Settings:Sand Dragon", "SEEK_MAGIC=", false);
	creatureInfo[kSandDragon].seekWeapons							= fi_GetBoolean(":Creature Settings:Sand Dragon", "SEEK_WEAPONS=", true);
	creatureInfo[kSandDragon].magicResist				      = (float)fi_GetData(":Creature Settings:Sand Dragon", "MAGIC_RESIST=", 20)/100.0;
	creatureInfo[kSandDragon].tamingLevel							= fi_GetData(":Creature Settings:Sand Dragon", "TAMING_LEVEL=", 8);
	creatureInfo[kSandDragon].creatureLevel							= fi_GetData(":Creature Settings:Sand Dragon", "LEVEL=", 1);
	creatureInfo[kSandDragon].armorRating							= fi_GetData(":Creature Settings:Sand Dragon", "ARMOR_RATING=", 75);
	creatureInfo[kSandDragon].immuneToPoison           = fi_GetBoolean(":Creature Settings:Sand Dragon", "POISON_IMMUNITY=", false);
	creatureInfo[kSandDragon].lockStrength							= fi_GetData(":Creature Settings:Sand Dragon", "LOCK=", 5);
	creatureInfo[kSandDragon].attackDamageMin					= fi_GetData(":Creature Settings:Sand Dragon", "MIN_DAMAGE=", 45);
	creatureInfo[kSandDragon].attackDamageMax					= fi_GetData(":Creature Settings:Sand Dragon", "MAX_DAMAGE=", 125);
	creatureInfo[kSandDragon].trackingLevel							= fi_GetData(":Creature Settings:Sand Dragon", "TRACKING_LEVEL=", 101);
	creatureInfo[kSandDragon].size									= fi_GetData(":Creature Settings:Sand Dragon", "SIZE=", 3);
	creatureInfo[kSandDragon].mageryLevel					= fi_GetData(":Creature Settings:Sand Dragon", "MAGERY_LEVEL=", 0);
	creatureInfo[kSandDragon].spellcastChance				= fi_GetData(":Creature Settings:Sand Dragon", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSandDragon].selfhealChance					= fi_GetData(":Creature Settings:Sand Dragon", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSandDragon].selfhealTrigger				= fi_GetData(":Creature Settings:Sand Dragon", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Sand Dragon", kSandDragon);
	fi_GetCreatureTalkScript(":Creature Settings:Sand Dragon", kSandDragon);
	fi_GetCreatureInventory(":Creature Settings:Sand Dragon", kSandDragon);
	fi_GetWeaponModifier(":Creature Settings:Sand Dragon", kSandDragon);
	
	creatureInfo[kPhaseSerpent].bmp										=	2;
	creatureInfo[kPhaseSerpent].domain								  =	kAir;
	creatureInfo[kPhaseSerpent].terrain								=	kGrass;
	creatureInfo[kPhaseSerpent].alignment							= kEvil;
	creatureInfo[kPhaseSerpent].directionalGraphics		= true;
	creatureInfo[kPhaseSerpent].graphicsIndex					= 1;
	creatureInfo[kPhaseSerpent].corpseID							  = 197;
	creatureInfo[kPhaseSerpent].attackRange						= 3;
	creatureInfo[kPhaseSerpent].desiredPopulation			= 500;
	creatureInfo[kPhaseSerpent].points			            = 8;
	creatureInfo[kPhaseSerpent].color			            = kBlackDragonColor;
	creatureInfo[kPhaseSerpent].rareLevel						  = 3;
	creatureInfo[kPhaseSerpent].rareChance					    = 50;
	creatureInfo[kPhaseSerpent].meatEater						  = true;
	creatureInfo[kPhaseSerpent].canMorph					      = true;
	creatureInfo[kPhaseSerpent].attackSound						= kSerpentAttackSnd;
	creatureInfo[kPhaseSerpent].speed										=	fi_GetData(":Creature Settings:Phase Serpent", "SPEED=", 50);
	creatureInfo[kPhaseSerpent].poisonous					      = fi_GetData(":Creature Settings:Phase Serpent", "POISONOUS=", 0);
	creatureInfo[kPhaseSerpent].curse					      		= fi_GetData(":Creature Settings:Phase Serpent", "CURSE=", 0);
	creatureInfo[kPhaseSerpent].maxHitPoints							= fi_GetData(":Creature Settings:Phase Serpent", "HIT_POINTS=", 175);
	creatureInfo[kPhaseSerpent].maxStrength							= fi_GetData(":Creature Settings:Phase Serpent", "STRENGTH=", 175);
	creatureInfo[kPhaseSerpent].maxIntelligence					= fi_GetData(":Creature Settings:Phase Serpent", "INTELLIGENCE=", 60);
	creatureInfo[kPhaseSerpent].maxDexterity							= fi_GetData(":Creature Settings:Phase Serpent", "DEXTERITY=", 50);
	creatureInfo[kPhaseSerpent].minGold								  = fi_GetData(":Creature Settings:Phase Serpent", "MIN_GOLD=", 100);
	creatureInfo[kPhaseSerpent].maxGold								  = fi_GetData(":Creature Settings:Phase Serpent", "MAX_GOLD=", 150);
	creatureInfo[kPhaseSerpent].seekGold									= fi_GetBoolean(":Creature Settings:Phase Serpent", "SEEK_GOLD=", true);
	creatureInfo[kPhaseSerpent].seekMagic								= fi_GetBoolean(":Creature Settings:Phase Serpent", "SEEK_MAGIC=", false);
	creatureInfo[kPhaseSerpent].seekWeapons							= fi_GetBoolean(":Creature Settings:Phase Serpent", "SEEK_WEAPONS=", true);
	creatureInfo[kPhaseSerpent].magicResist				      = (float)fi_GetData(":Creature Settings:Phase Serpent", "MAGIC_RESIST=", 40)/100.0;
	creatureInfo[kPhaseSerpent].tamingLevel							= fi_GetData(":Creature Settings:Phase Serpent", "TAMING_LEVEL=", 8);
	creatureInfo[kPhaseSerpent].creatureLevel							= fi_GetData(":Creature Settings:Phase Serpent", "LEVEL=", 1);
	creatureInfo[kPhaseSerpent].armorRating							= fi_GetData(":Creature Settings:Phase Serpent", "ARMOR_RATING=", 65);
	creatureInfo[kPhaseSerpent].immuneToPoison           = fi_GetBoolean(":Creature Settings:Phase Serpent", "POISON_IMMUNITY=", true);
	creatureInfo[kPhaseSerpent].lockStrength							= fi_GetData(":Creature Settings:Phase Serpent", "LOCK=", 0);
	creatureInfo[kPhaseSerpent].attackDamageMin					= fi_GetData(":Creature Settings:Phase Serpent", "MIN_DAMAGE=", 40);
	creatureInfo[kPhaseSerpent].attackDamageMax					= fi_GetData(":Creature Settings:Phase Serpent", "MAX_DAMAGE=", 90);
	creatureInfo[kPhaseSerpent].trackingLevel							= fi_GetData(":Creature Settings:Phase Serpent", "TRACKING_LEVEL=", 101);
	creatureInfo[kPhaseSerpent].size									= fi_GetData(":Creature Settings:Phase Serpent", "SIZE=", 2);
	creatureInfo[kPhaseSerpent].mageryLevel					= fi_GetData(":Creature Settings:Phase Serpent", "MAGERY_LEVEL=", 0);
	creatureInfo[kPhaseSerpent].spellcastChance				= fi_GetData(":Creature Settings:Phase Serpent", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kPhaseSerpent].selfhealChance				= fi_GetData(":Creature Settings:Phase Serpent", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kPhaseSerpent].selfhealTrigger				= fi_GetData(":Creature Settings:Phase Serpent", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Phase Serpent", kPhaseSerpent);
	fi_GetCreatureTalkScript(":Creature Settings:Phase Serpent", kPhaseSerpent);
	fi_GetCreatureInventory(":Creature Settings:Phase Serpent", kPhaseSerpent);
	fi_GetWeaponModifier(":Creature Settings:Phase Serpent", kPhaseSerpent);
	
	creatureInfo[kBlackBear].bmp										=	1;
	creatureInfo[kBlackBear].domain								  =	kLand;
	creatureInfo[kBlackBear].terrain								=	kAnyTerrain;
	creatureInfo[kBlackBear].alignment							= kEvil;
	creatureInfo[kBlackBear].directionalGraphics		= true;
	creatureInfo[kBlackBear].graphicsIndex					= 1;
	creatureInfo[kBlackBear].corpseID							  = 202;
	creatureInfo[kBlackBear].attackRange						= 1;
	creatureInfo[kBlackBear].desiredPopulation			= 1500;
	creatureInfo[kBlackBear].points			            = 8;
	creatureInfo[kBlackBear].color			            = kBlackDragonColor;
	creatureInfo[kBlackBear].rareLevel						  = 3;
	creatureInfo[kBlackBear].rareChance					    = 50;
	creatureInfo[kBlackBear].meatEater						  = true;
	creatureInfo[kBlackBear].canMorph					      = true;
	creatureInfo[kBlackBear].attackSound						= kBearAttackSnd;
	creatureInfo[kBlackBear].speed										=	fi_GetData(":Creature Settings:Black Bear", "SPEED=", 50);
	creatureInfo[kBlackBear].poisonous					      = fi_GetData(":Creature Settings:Black Bear", "POISONOUS=", 0);
	creatureInfo[kBlackBear].curse					      		= fi_GetData(":Creature Settings:Black Bear", "CURSE=", 0);
	creatureInfo[kBlackBear].maxHitPoints							= fi_GetData(":Creature Settings:Black Bear", "HIT_POINTS=", 175);
	creatureInfo[kBlackBear].maxStrength							= fi_GetData(":Creature Settings:Black Bear", "STRENGTH=", 175);
	creatureInfo[kBlackBear].maxIntelligence					= fi_GetData(":Creature Settings:Black Bear", "INTELLIGENCE=", 60);
	creatureInfo[kBlackBear].maxDexterity							= fi_GetData(":Creature Settings:Black Bear", "DEXTERITY=", 50);
	creatureInfo[kBlackBear].minGold								  = fi_GetData(":Creature Settings:Black Bear", "MIN_GOLD=", 100);
	creatureInfo[kBlackBear].maxGold								  = fi_GetData(":Creature Settings:Black Bear", "MAX_GOLD=", 150);
	creatureInfo[kBlackBear].seekGold									= fi_GetBoolean(":Creature Settings:Black Bear", "SEEK_GOLD=", true);
	creatureInfo[kBlackBear].seekMagic								= fi_GetBoolean(":Creature Settings:Black Bear", "SEEK_MAGIC=", false);
	creatureInfo[kBlackBear].seekWeapons							= fi_GetBoolean(":Creature Settings:Black Bear", "SEEK_WEAPONS=", true);
	creatureInfo[kBlackBear].magicResist				      = (float)fi_GetData(":Creature Settings:Black Bear", "MAGIC_RESIST=", 40)/100.0;
	creatureInfo[kBlackBear].tamingLevel							= fi_GetData(":Creature Settings:Black Bear", "TAMING_LEVEL=", 8);
	creatureInfo[kBlackBear].creatureLevel							= fi_GetData(":Creature Settings:Black Bear", "LEVEL=", 1);
	creatureInfo[kBlackBear].armorRating							= fi_GetData(":Creature Settings:Black Bear", "ARMOR_RATING=", 65);
	creatureInfo[kBlackBear].immuneToPoison           = fi_GetBoolean(":Creature Settings:Black Bear", "POISON_IMMUNITY=", true);
	creatureInfo[kBlackBear].lockStrength							= fi_GetData(":Creature Settings:Black Bear", "LOCK=", 0);
	creatureInfo[kBlackBear].attackDamageMin					= fi_GetData(":Creature Settings:Black Bear", "MIN_DAMAGE=", 40);
	creatureInfo[kBlackBear].attackDamageMax					= fi_GetData(":Creature Settings:Black Bear", "MAX_DAMAGE=", 90);
	creatureInfo[kBlackBear].trackingLevel							= fi_GetData(":Creature Settings:Black Bear", "TRACKING_LEVEL=", 101);
	creatureInfo[kBlackBear].size									= fi_GetData(":Creature Settings:Black Bear", "SIZE=", 2);
	creatureInfo[kBlackBear].mageryLevel					= fi_GetData(":Creature Settings:Black Bear", "MAGERY_LEVEL=", 0);
	creatureInfo[kBlackBear].spellcastChance				= fi_GetData(":Creature Settings:Black Bear", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kBlackBear].selfhealChance				= fi_GetData(":Creature Settings:Black Bear", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBlackBear].selfhealTrigger				= fi_GetData(":Creature Settings:Black Bear", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Black Bear", kBlackBear);
	fi_GetCreatureTalkScript(":Creature Settings:Black Bear", kBlackBear);
	fi_GetCreatureInventory(":Creature Settings:Black Bear", kBlackBear);
	fi_GetWeaponModifier(":Creature Settings:Black Bear", kBlackBear);	
	
	creatureInfo[kBrownBear].bmp										=	1;
	creatureInfo[kBrownBear].domain								  =	kLand;
	creatureInfo[kBrownBear].terrain								=	kAnyTerrain;
	creatureInfo[kBrownBear].alignment							= kEvil;
	creatureInfo[kBrownBear].directionalGraphics		= true;
	creatureInfo[kBrownBear].graphicsIndex					= 1;
	creatureInfo[kBrownBear].corpseID							  = 203;
	creatureInfo[kBrownBear].attackRange						= 1;
	creatureInfo[kBrownBear].desiredPopulation			= 1500;
	creatureInfo[kBrownBear].points			            = 8;
	creatureInfo[kBrownBear].color			            = kBrownWolfColor;
	creatureInfo[kBrownBear].rareLevel						  = 3;
	creatureInfo[kBrownBear].rareChance					    = 50;
	creatureInfo[kBrownBear].meatEater						  = true;
	creatureInfo[kBrownBear].canMorph					      = true;
	creatureInfo[kBrownBear].attackSound						= kBearAttackSnd;
	creatureInfo[kBrownBear].speed										=	fi_GetData(":Creature Settings:Brown Bear", "SPEED=", 50);
	creatureInfo[kBrownBear].poisonous					      = fi_GetData(":Creature Settings:Brown Bear", "POISONOUS=", 0);
	creatureInfo[kBrownBear].curse					      		= fi_GetData(":Creature Settings:Brown Bear", "CURSE=", 0);
	creatureInfo[kBrownBear].maxHitPoints							= fi_GetData(":Creature Settings:Brown Bear", "HIT_POINTS=", 175);
	creatureInfo[kBrownBear].maxStrength							= fi_GetData(":Creature Settings:Brown Bear", "STRENGTH=", 175);
	creatureInfo[kBrownBear].maxIntelligence					= fi_GetData(":Creature Settings:Brown Bear", "INTELLIGENCE=", 60);
	creatureInfo[kBrownBear].maxDexterity							= fi_GetData(":Creature Settings:Brown Bear", "DEXTERITY=", 50);
	creatureInfo[kBrownBear].minGold								  = fi_GetData(":Creature Settings:Brown Bear", "MIN_GOLD=", 100);
	creatureInfo[kBrownBear].maxGold								  = fi_GetData(":Creature Settings:Brown Bear", "MAX_GOLD=", 150);
	creatureInfo[kBrownBear].seekGold									= fi_GetBoolean(":Creature Settings:Brown Bear", "SEEK_GOLD=", true);
	creatureInfo[kBrownBear].seekMagic								= fi_GetBoolean(":Creature Settings:Brown Bear", "SEEK_MAGIC=", false);
	creatureInfo[kBrownBear].seekWeapons							= fi_GetBoolean(":Creature Settings:Brown Bear", "SEEK_WEAPONS=", true);
	creatureInfo[kBrownBear].magicResist				      = (float)fi_GetData(":Creature Settings:Brown Bear", "MAGIC_RESIST=", 40)/100.0;
	creatureInfo[kBrownBear].tamingLevel							= fi_GetData(":Creature Settings:Brown Bear", "TAMING_LEVEL=", 8);
	creatureInfo[kBrownBear].creatureLevel							= fi_GetData(":Creature Settings:Brown Bear", "LEVEL=", 1);
	creatureInfo[kBrownBear].armorRating							= fi_GetData(":Creature Settings:Brown Bear", "ARMOR_RATING=", 65);
	creatureInfo[kBrownBear].immuneToPoison           = fi_GetBoolean(":Creature Settings:Brown Bear", "POISON_IMMUNITY=", true);
	creatureInfo[kBrownBear].lockStrength							= fi_GetData(":Creature Settings:Brown Bear", "LOCK=", 0);
	creatureInfo[kBrownBear].attackDamageMin					= fi_GetData(":Creature Settings:Brown Bear", "MIN_DAMAGE=", 40);
	creatureInfo[kBrownBear].attackDamageMax					= fi_GetData(":Creature Settings:Brown Bear", "MAX_DAMAGE=", 90);
	creatureInfo[kBrownBear].trackingLevel							= fi_GetData(":Creature Settings:Brown Bear", "TRACKING_LEVEL=", 101);
	creatureInfo[kBrownBear].size									= fi_GetData(":Creature Settings:Brown Bear", "SIZE=", 2);
	creatureInfo[kBrownBear].mageryLevel					= fi_GetData(":Creature Settings:Brown Bear", "MAGERY_LEVEL=", 0);
	creatureInfo[kBrownBear].spellcastChance				= fi_GetData(":Creature Settings:Brown Bear", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kBrownBear].selfhealChance				= fi_GetData(":Creature Settings:Brown Bear", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kBrownBear].selfhealTrigger				= fi_GetData(":Creature Settings:Brown Bear", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Brown Bear", kBrownBear);
	fi_GetCreatureTalkScript(":Creature Settings:Brown Bear", kBrownBear);
	fi_GetCreatureInventory(":Creature Settings:Brown Bear", kBrownBear);
	fi_GetWeaponModifier(":Creature Settings:Brown Bear", kBrownBear);	
	
	
	
	creatureInfo[kShark].bmp										=	1;
	creatureInfo[kShark].domain								  =	kWater;
	creatureInfo[kShark].terrain								=	kAnyTerrain;
	creatureInfo[kShark].alignment							= kEvil;
	creatureInfo[kShark].directionalGraphics		= true;
	creatureInfo[kShark].graphicsIndex					= 1;
	creatureInfo[kShark].corpseID							  = 210;
	creatureInfo[kShark].attackRange						= 1;
	creatureInfo[kShark].desiredPopulation			= 1000;
	creatureInfo[kShark].points			            = 8;
	//creatureInfo[kShark].color			            = kBlackDragonColor;
	creatureInfo[kShark].rareLevel						  = 3;
	creatureInfo[kShark].rareChance					    = 50;
	creatureInfo[kShark].meatEater						  = true;
	creatureInfo[kShark].canMorph					      = true;
	creatureInfo[kShark].attackSound						= kSharkAttackSnd;
	creatureInfo[kShark].speed										=	fi_GetData(":Creature Settings:Shark", "SPEED=", 50);
	creatureInfo[kShark].poisonous					      = fi_GetData(":Creature Settings:Shark", "POISONOUS=", 0);
	creatureInfo[kShark].curse					      		= fi_GetData(":Creature Settings:Shark", "CURSE=", 0);
	creatureInfo[kShark].maxHitPoints							= fi_GetData(":Creature Settings:Shark", "HIT_POINTS=", 175);
	creatureInfo[kShark].maxStrength							= fi_GetData(":Creature Settings:Shark", "STRENGTH=", 175);
	creatureInfo[kShark].maxIntelligence					= fi_GetData(":Creature Settings:Shark", "INTELLIGENCE=", 60);
	creatureInfo[kShark].maxDexterity							= fi_GetData(":Creature Settings:Shark", "DEXTERITY=", 50);
	creatureInfo[kShark].minGold								  = fi_GetData(":Creature Settings:Shark", "MIN_GOLD=", 100);
	creatureInfo[kShark].maxGold								  = fi_GetData(":Creature Settings:Shark", "MAX_GOLD=", 150);
	creatureInfo[kShark].seekGold									= fi_GetBoolean(":Creature Settings:Shark", "SEEK_GOLD=", true);
	creatureInfo[kShark].seekMagic								= fi_GetBoolean(":Creature Settings:Shark", "SEEK_MAGIC=", false);
	creatureInfo[kShark].seekWeapons							= fi_GetBoolean(":Creature Settings:Shark", "SEEK_WEAPONS=", true);
	creatureInfo[kShark].magicResist				      = (float)fi_GetData(":Creature Settings:Shark", "MAGIC_RESIST=", 40)/100.0;
	creatureInfo[kShark].tamingLevel							= fi_GetData(":Creature Settings:Shark", "TAMING_LEVEL=", 8);
	creatureInfo[kShark].creatureLevel							= fi_GetData(":Creature Settings:Shark", "LEVEL=", 1);
	creatureInfo[kShark].armorRating							= fi_GetData(":Creature Settings:Shark", "ARMOR_RATING=", 65);
	creatureInfo[kShark].immuneToPoison           = fi_GetBoolean(":Creature Settings:Shark", "POISON_IMMUNITY=", true);
	creatureInfo[kShark].lockStrength							= fi_GetData(":Creature Settings:Shark", "LOCK=", 0);
	creatureInfo[kShark].attackDamageMin					= fi_GetData(":Creature Settings:Shark", "MIN_DAMAGE=", 40);
	creatureInfo[kShark].attackDamageMax					= fi_GetData(":Creature Settings:Shark", "MAX_DAMAGE=", 90);
	creatureInfo[kShark].trackingLevel							= fi_GetData(":Creature Settings:Shark", "TRACKING_LEVEL=", 101);
	creatureInfo[kShark].size									= fi_GetData(":Creature Settings:Shark", "SIZE=", 2);
	creatureInfo[kShark].mageryLevel					= fi_GetData(":Creature Settings:Shark", "MAGERY_LEVEL=", 0);
	creatureInfo[kShark].spellcastChance				= fi_GetData(":Creature Settings:Shark", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kShark].selfhealChance				= fi_GetData(":Creature Settings:Shark", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kShark].selfhealTrigger				= fi_GetData(":Creature Settings:Shark", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Shark", kShark);
	fi_GetCreatureTalkScript(":Creature Settings:Shark", kShark);
	fi_GetCreatureInventory(":Creature Settings:Shark", kShark);
	fi_GetWeaponModifier(":Creature Settings:Shark", kShark);	
	
	creatureInfo[kLizardManShaman].bmp									=	0;
	creatureInfo[kLizardManShaman].domain								=	kLand;
	creatureInfo[kLizardManShaman].terrain							=	kAnyTerrain;
	creatureInfo[kLizardManShaman].alignment						= kEvil;
	creatureInfo[kLizardManShaman].directionalGraphics	= true;
	creatureInfo[kLizardManShaman].graphicsIndex				= 1;
	creatureInfo[kLizardManShaman].corpseID							= 26;
	creatureInfo[kLizardManShaman].desiredPopulation		= 300;
	creatureInfo[kLizardManShaman].points			          = 4;
	creatureInfo[kLizardManShaman].rareLevel						= 50;
	creatureInfo[kLizardManShaman].rareChance					  = 1;
	creatureInfo[kLizardManShaman].attackRange					= 2;
	creatureInfo[kLizardManShaman].metal					      = true;
	creatureInfo[kLizardManShaman].randomSound					= kLizardSnd;
	creatureInfo[kLizardManShaman].attackSound					= kLizardSnd;
	creatureInfo[kLizardManShaman].canMorph					    = true;
	creatureInfo[kLizardManShaman].speed										=	fi_GetData(":Creature Settings:Lizard Shaman", "SPEED=", 45);
	creatureInfo[kLizardManShaman].poisonous					      = fi_GetData(":Creature Settings:Lizard Shaman", "POISONOUS=", 0);
	creatureInfo[kLizardManShaman].curse					      		= fi_GetData(":Creature Settings:Lizard Shaman", "CURSE=", 0);
	creatureInfo[kLizardManShaman].maxHitPoints							= fi_GetData(":Creature Settings:Lizard Shaman", "HIT_POINTS=", 90);
	creatureInfo[kLizardManShaman].maxStrength							= fi_GetData(":Creature Settings:Lizard Shaman", "STRENGTH=", 90);
	creatureInfo[kLizardManShaman].maxIntelligence					= fi_GetData(":Creature Settings:Lizard Shaman", "INTELLIGENCE=", 60);
	creatureInfo[kLizardManShaman].maxDexterity							= fi_GetData(":Creature Settings:Lizard Shaman", "DEXTERITY=", 50);
	creatureInfo[kLizardManShaman].minGold								  = fi_GetData(":Creature Settings:Lizard Shaman", "MIN_GOLD=", 5);
	creatureInfo[kLizardManShaman].maxGold								  = fi_GetData(":Creature Settings:Lizard Shaman", "MAX_GOLD=", 20);
	creatureInfo[kLizardManShaman].seekGold									= fi_GetBoolean(":Creature Settings:Lizard Shaman", "SEEK_GOLD=", true);
	creatureInfo[kLizardManShaman].seekMagic								= fi_GetBoolean(":Creature Settings:Lizard Shaman", "SEEK_MAGIC=", false);
	creatureInfo[kLizardManShaman].seekWeapons							= fi_GetBoolean(":Creature Settings:Lizard Shaman", "SEEK_WEAPONS=", true);
	creatureInfo[kLizardManShaman].magicResist				      = (float)fi_GetData(":Creature Settings:Lizard Shaman", "MAGIC_RESIST=", 75)/100.0;
	creatureInfo[kLizardManShaman].tamingLevel							= fi_GetData(":Creature Settings:Lizard Shaman", "TAMING_LEVEL=", 101);
	creatureInfo[kLizardManShaman].creatureLevel							= fi_GetData(":Creature Settings:Lizard Shaman", "LEVEL=", 1);
	creatureInfo[kLizardManShaman].armorRating							= fi_GetData(":Creature Settings:Lizard Shaman", "ARMOR_RATING=", 40);
	creatureInfo[kLizardManShaman].immuneToPoison           = fi_GetBoolean(":Creature Settings:Lizard Shaman", "POISON_IMMUNITY=", false);
	creatureInfo[kLizardManShaman].lockStrength							= fi_GetData(":Creature Settings:Lizard Shaman", "LOCK=", 0);
	creatureInfo[kLizardManShaman].attackDamageMin					= fi_GetData(":Creature Settings:Lizard Shaman", "MIN_DAMAGE=", 5);
	creatureInfo[kLizardManShaman].attackDamageMax					= fi_GetData(":Creature Settings:Lizard Shaman", "MAX_DAMAGE=", 11);
	creatureInfo[kLizardManShaman].trackingLevel							= fi_GetData(":Creature Settings:Lizard Shaman", "TRACKING_LEVEL=", 101);
	creatureInfo[kLizardManShaman].size									= fi_GetData(":Creature Settings:Lizard Shaman", "SIZE=", 2);
	creatureInfo[kLizardManShaman].mageryLevel					= fi_GetData(":Creature Settings:Lizard Shaman", "MAGERY_LEVEL=", 0);
	creatureInfo[kLizardManShaman].spellcastChance				= fi_GetData(":Creature Settings:Lizard Shaman", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kLizardManShaman].selfhealChance				= fi_GetData(":Creature Settings:Lizard Shaman", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kLizardManShaman].selfhealTrigger				= fi_GetData(":Creature Settings:Lizard Shaman", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Lizard Shaman", kLizardManShaman);
	//cr_AddToCreatureControlInventory(kLizardManShaman, kLeatherHide, 2, 3);
	//cr_AddToCreatureControlInventory(kLizardManShaman, kRawSteak, 2, 5);
	fi_GetCreatureTalkScript(":Creature Settings:Lizard Shaman", kLizardManShaman);
	fi_GetCreatureInventory(":Creature Settings:Lizard Shaman", kLizardManShaman);
	fi_GetWeaponModifier(":Creature Settings:Lizard Shaman", kLizardManShaman);
	
	
	creatureInfo[kHellMage].bmp										=	0;
	creatureInfo[kHellMage].domain								=	kLand;
	creatureInfo[kHellMage].terrain								=	kAnyTerrain;
	creatureInfo[kHellMage].alignment							= kEvil;
	creatureInfo[kHellMage].directionalGraphics		= true;
	creatureInfo[kHellMage].graphicsIndex					= 1;
	creatureInfo[kHellMage].color		        				= kRedDragonColor;
	creatureInfo[kHellMage].corpseID							= 229;
	creatureInfo[kHellMage].attackRange						= 3;
	creatureInfo[kHellMage].desiredPopulation			= 150;
	creatureInfo[kHellMage].points			          = 3;
	creatureInfo[kHellMage].undead			          = true;
	creatureInfo[kHellMage].rareLevel						  = 10;
	creatureInfo[kHellMage].rareChance					  = 20;
	creatureInfo[kHellMage].canMorph					    = true;
	creatureInfo[kHellMage].bleeds					    	= false;
	creatureInfo[kHellMage].speed										=	fi_GetData(":Creature Settings:Hell Mage", "SPEED=", 70);
	creatureInfo[kHellMage].maxHitPoints						= fi_GetData(":Creature Settings:Hell Mage", "HIT_POINTS=", 75);
	creatureInfo[kHellMage].maxStrength							= fi_GetData(":Creature Settings:Hell Mage", "STRENGTH=", 75);
	creatureInfo[kHellMage].maxIntelligence					= fi_GetData(":Creature Settings:Hell Mage", "INTELLIGENCE=", 60);
	creatureInfo[kHellMage].maxDexterity						= fi_GetData(":Creature Settings:Hell Mage", "DEXTERITY=", 50);
	creatureInfo[kHellMage].maxMagicPoints					= fi_GetData(":Creature Settings:Hell Mage", "MAGIC_POINTS=", 70);
	creatureInfo[kHellMage].minGold								  = fi_GetData(":Creature Settings:Hell Mage", "MIN_GOLD=", 15);
	creatureInfo[kHellMage].maxGold								  = fi_GetData(":Creature Settings:Hell Mage", "MAX_GOLD=", 50);
	creatureInfo[kHellMage].seekGold								= fi_GetBoolean(":Creature Settings:Hell Mage", "SEEK_GOLD=", true);
	creatureInfo[kHellMage].seekMagic								= fi_GetBoolean(":Creature Settings:Hell Mage", "SEEK_MAGIC=", true);
	creatureInfo[kHellMage].seekWeapons							= fi_GetBoolean(":Creature Settings:Hell Mage", "SEEK_WEAPONS=", false);
	creatureInfo[kHellMage].magicResist				      = (float)fi_GetData(":Creature Settings:Hell Mage", "MAGIC_RESIST=", 30)/100.0;
	creatureInfo[kHellMage].tamingLevel							= fi_GetData(":Creature Settings:Hell Mage", "TAMING_LEVEL=", 101);
	creatureInfo[kHellMage].creatureLevel							= fi_GetData(":Creature Settings:Hell Mage", "LEVEL=", 1);
	creatureInfo[kHellMage].armorRating							= fi_GetData(":Creature Settings:Hell Mage", "ARMOR_RATING=", 30);
	creatureInfo[kHellMage].immuneToPoison          = fi_GetBoolean(":Creature Settings:Hell Mage", "POISON_IMMUNITY=", true);
	creatureInfo[kHellMage].lockStrength						= fi_GetData(":Creature Settings:Hell Mage", "LOCK=", 3);
	creatureInfo[kHellMage].trackingLevel							= fi_GetData(":Creature Settings:Hell Mage", "TRACKING_LEVEL=", 101);
	creatureInfo[kHellMage].size									= fi_GetData(":Creature Settings:Hell Mage", "SIZE=", 2);
	creatureInfo[kHellMage].mageryLevel					= fi_GetData(":Creature Settings:Hell Mage", "MAGERY_LEVEL=", 10);
	creatureInfo[kHellMage].spellcastChance				= fi_GetData(":Creature Settings:Hell Mage", "SPELLCAST_CHANCE=", 100);
	creatureInfo[kHellMage].selfhealChance				= fi_GetData(":Creature Settings:Hell Mage", "SELFHEAL_CHANCE=", 33);
	creatureInfo[kHellMage].selfhealTrigger				= fi_GetData(":Creature Settings:Hell Mage", "SELFHEAL_TRIGGER=", 20);
	fi_GetSpellList(":Creature Settings:Hell Mage", kHellMage);
	fi_GetCreatureTalkScript(":Creature Settings:Hell Mage", kHellMage);
	fi_GetCreatureInventory(":Creature Settings:Hell Mage", kHellMage);
	fi_GetWeaponModifier(":Creature Settings:Hell Mage", kHellMage);
	
	
	creatureInfo[kWrath].bmp										=	5;
	creatureInfo[kWrath].domain								=	kAir;
	creatureInfo[kWrath].terrain								=	kAnyTerrain;
	creatureInfo[kWrath].alignment							= kEvil;
	creatureInfo[kWrath].directionalGraphics		= true;
	creatureInfo[kWrath].graphicsIndex					= 1;
	//creatureInfo[kWrath].color		        				= kForestDragonColor;
	creatureInfo[kWrath].corpseID							= 229;
	creatureInfo[kWrath].attackRange						= 2;
	creatureInfo[kWrath].desiredPopulation			= 150;
	creatureInfo[kWrath].points			          = 10;
	creatureInfo[kWrath].undead			          = true;
	creatureInfo[kWrath].rareLevel						  = 10;
	creatureInfo[kWrath].rareChance					  = 20;
	creatureInfo[kWrath].canMorph					    = true;
	creatureInfo[kWrath].bleeds					    	= false;
	creatureInfo[kWrath].speed										=	fi_GetData(":Creature Settings:Wrath", "SPEED=", 100);
	creatureInfo[kWrath].maxHitPoints						= fi_GetData(":Creature Settings:Wrath", "HIT_POINTS=", 75);
	creatureInfo[kWrath].maxStrength							= fi_GetData(":Creature Settings:Wrath", "STRENGTH=", 120);
	creatureInfo[kWrath].maxIntelligence					= fi_GetData(":Creature Settings:Wrath", "INTELLIGENCE=", 120);
	creatureInfo[kWrath].maxDexterity						= fi_GetData(":Creature Settings:Wrath", "DEXTERITY=", 120);
	creatureInfo[kWrath].maxMagicPoints					= fi_GetData(":Creature Settings:Wrath", "MAGIC_POINTS=", 120);
	creatureInfo[kWrath].minGold								  = fi_GetData(":Creature Settings:Wrath", "MIN_GOLD=", 15);
	creatureInfo[kWrath].maxGold								  = fi_GetData(":Creature Settings:Wrath", "MAX_GOLD=", 50);
	creatureInfo[kWrath].seekGold								= fi_GetBoolean(":Creature Settings:Wrath", "SEEK_GOLD=", true);
	creatureInfo[kWrath].seekMagic								= fi_GetBoolean(":Creature Settings:Wrath", "SEEK_MAGIC=", true);
	creatureInfo[kWrath].seekWeapons							= fi_GetBoolean(":Creature Settings:Wrath", "SEEK_WEAPONS=", false);
	creatureInfo[kWrath].magicResist				      = (float)fi_GetData(":Creature Settings:Wrath", "MAGIC_RESIST=", 70)/100.0;
	creatureInfo[kWrath].tamingLevel							= fi_GetData(":Creature Settings:Wrath", "TAMING_LEVEL=", 101);
	creatureInfo[kWrath].creatureLevel							= fi_GetData(":Creature Settings:Wrath", "LEVEL=", 1);
	creatureInfo[kWrath].armorRating							= fi_GetData(":Creature Settings:Wrath", "ARMOR_RATING=", 30);
	creatureInfo[kWrath].immuneToPoison          = fi_GetBoolean(":Creature Settings:Wrath", "POISON_IMMUNITY=", true);
	creatureInfo[kWrath].lockStrength						= fi_GetData(":Creature Settings:Wrath", "LOCK=", 3);
	creatureInfo[kWrath].trackingLevel							= fi_GetData(":Creature Settings:Wrath", "TRACKING_LEVEL=", 101);
	creatureInfo[kWrath].attackDamageMin					= fi_GetData(":Creature Settings:Wrath", "MIN_DAMAGE=", 100);
	creatureInfo[kWrath].attackDamageMax					= fi_GetData(":Creature Settings:Wrath", "MAX_DAMAGE=", 140);
	creatureInfo[kWrath].size									= fi_GetData(":Creature Settings:Wrath", "SIZE=", 3);
	creatureInfo[kWrath].mageryLevel					= fi_GetData(":Creature Settings:Wrath", "MAGERY_LEVEL=", 10);
	creatureInfo[kWrath].spellcastChance				= fi_GetData(":Creature Settings:Wrath", "SPELLCAST_CHANCE=", 5);
	creatureInfo[kWrath].selfhealChance				= fi_GetData(":Creature Settings:Wrath", "SELFHEAL_CHANCE=", 33);
	creatureInfo[kWrath].selfhealTrigger				= fi_GetData(":Creature Settings:Wrath", "SELFHEAL_TRIGGER=", 20);
	fi_GetSpellList(":Creature Settings:Wrath", kWrath);
	fi_GetCreatureTalkScript(":Creature Settings:Wrath", kWrath);
	fi_GetCreatureInventory(":Creature Settings:Wrath", kWrath);
	fi_GetWeaponModifier(":Creature Settings:Wrath", kWrath);
	
	
	/*creatureInfo[kSnapTurtle].bmp									=	0;
	creatureInfo[kSnapTurtle].domain								=	kWater;
	creatureInfo[kSnapTurtle].alignment						= kEvil;
	creatureInfo[kSnapTurtle].directionalGraphics	= true;
	creatureInfo[kSnapTurtle].pauseGraphics				= true;
	creatureInfo[kSnapTurtle].graphicsIndex				= 1;
	creatureInfo[kSnapTurtle].corpseID							= 16;
	creatureInfo[kSnapTurtle].color			            = kPoisonElementalColor;
	creatureInfo[kSnapTurtle].desiredPopulation		= 5000;
	creatureInfo[kSnapTurtle].meatEater						= true;
	creatureInfo[kSnapTurtle].canMorph					    = true;
	creatureInfo[kSnapTurtle].speed										=	fi_GetData(":Creature Settings:Snapping Turtle", "SPEED=", 90);
	creatureInfo[kSnapTurtle].poisonous					      = fi_GetData(":Creature Settings:Snapping Turtle", "POISONOUS=", 0);
	creatureInfo[kSnapTurtle].curse					      		= fi_GetData(":Creature Settings:Snapping Turtle", "CURSE=", 0);
	creatureInfo[kSnapTurtle].maxHitPoints							= fi_GetData(":Creature Settings:Snapping Turtle", "HIT_POINTS=", 20);
	creatureInfo[kSnapTurtle].maxStrength							= fi_GetData(":Creature Settings:Snapping Turtle", "STRENGTH=", 20);
	creatureInfo[kSnapTurtle].maxIntelligence					= fi_GetData(":Creature Settings:Snapping Turtle", "INTELLIGENCE=", 10);
	creatureInfo[kSnapTurtle].maxDexterity							= fi_GetData(":Creature Settings:Snapping Turtle", "DEXTERITY=", 10);
	creatureInfo[kSnapTurtle].minGold								  = fi_GetData(":Creature Settings:Snapping Turtle", "MIN_GOLD=", 0);
	creatureInfo[kSnapTurtle].maxGold								  = fi_GetData(":Creature Settings:Snapping Turtle", "MAX_GOLD=", 0);
	creatureInfo[kSnapTurtle].seekGold									= fi_GetBoolean(":Creature Settings:Snapping Turtle", "SEEK_GOLD=", false);
	creatureInfo[kSnapTurtle].seekMagic								= fi_GetBoolean(":Creature Settings:Snapping Turtle", "SEEK_MAGIC=", false);
	creatureInfo[kSnapTurtle].seekWeapons							= fi_GetBoolean(":Creature Settings:Snapping Turtle", "SEEK_WEAPONS=", false);
	creatureInfo[kSnapTurtle].magicResist				      = (float)fi_GetData(":Creature Settings:Snapping Turtle", "MAGIC_RESIST=", 100)/100.0;
	creatureInfo[kSnapTurtle].tamingLevel							= fi_GetData(":Creature Settings:Snapping Turtle", "TAMING_LEVEL=", 2);
	creatureInfo[kSnapTurtle].creatureLevel							= fi_GetData(":Creature Settings:Snapping Turtle", "LEVEL=", 1);
	creatureInfo[kSnapTurtle].armorRating							= fi_GetData(":Creature Settings:Snapping Turtle", "ARMOR_RATING=", 30);
	creatureInfo[kSnapTurtle].immuneToPoison           = fi_GetBoolean(":Creature Settings:Snapping Turtle", "POISON_IMMUNITY=", false);
	creatureInfo[kSnapTurtle].lockStrength							= fi_GetData(":Creature Settings:Snapping Turtle", "LOCK=", 0);
	creatureInfo[kSnapTurtle].attackDamageMin					= fi_GetData(":Creature Settings:Snapping Turtle", "MIN_DAMAGE=", 1);
	creatureInfo[kSnapTurtle].attackDamageMax					= fi_GetData(":Creature Settings:Snapping Turtle", "MAX_DAMAGE=", 2);
	creatureInfo[kSnapTurtle].trackingLevel							= fi_GetData(":Creature Settings:Snapping Turtle", "TRACKING_LEVEL=", 101);
	creatureInfo[kSnapTurtle].size									= fi_GetData(":Creature Settings:Snapping Turtle", "SIZE=", 1);
	creatureInfo[kSnapTurtle].mageryLevel					= fi_GetData(":Creature Settings:Snapping Turtle", "MAGERY_LEVEL=", 0);
	creatureInfo[kSnapTurtle].spellcastChance				= fi_GetData(":Creature Settings:Snapping Turtle", "SPELLCAST_CHANCE=", 0);
	creatureInfo[kSnapTurtle].selfhealChance					= fi_GetData(":Creature Settings:Snapping Turtle", "SELFHEAL_CHANCE=", 0);
	creatureInfo[kSnapTurtle].selfhealTrigger				= fi_GetData(":Creature Settings:Snapping Turtle", "SELFHEAL_TRIGGER=", 0);
	fi_GetSpellList(":Creature Settings:Snapping Turtle", kSnapTurtle);
	fi_GetCreatureTalkScript(":Creature Settings:Snapping Turtle", kSnapTurtle);
	fi_GetCreatureInventory(":Creature Settings:Snapping Turtle", kSnapTurtle);
	fi_GetWeaponModifier(":Creature Settings:Snapping Turtle", kSnapTurtle);
	*/
	
	
#ifdef _SERVER
	fi_GetCreatureSpawns();

//fi_WriteToCSLog("----------------Initial populating--------------\n");

	for (i=0;i<100000;i++)  // populate the world with bad guys
	//for (i=0;i<50;i++)  // populate the world with bad guys
	{
		nextSpawnTime=0;
		cr_CheckCreaturePopulation();
	}

//fi_WriteToCSLog("-------------World populated and ready----------\n");

	for (i=0;i<kMaxCreatureTypes;i++)
		totalPopulation=totalPopulation+creatureInfo[i].desiredPopulation;

	if (totalPopulation>(kMaxCreatures-5000))
		fi_WriteToErrorLog("Desired population Out of Range");

#endif

}

//-----------------------------------------------
// SERVER FUNCTIONS
//-----------------------------------------------

#ifdef _SERVER

// ----------------------------------------------------------------- //
TBoolean cr_BothInPVPZone(int c1, int c2)
// ----------------------------------------------------------------- //

{

	if ((creature[c1].row>1076) && (creature[c1].row<1150))
		if ((creature[c1].col>1681) && (creature[c1].col<1766))
			if ((creature[c2].row>1076) && (creature[c2].row<1150))
				if ((creature[c2].col>1681) && (creature[c2].col<1766))
				
		return (true);
		/**/		
	 if ((creature[c1].row>960) && (creature[c1].row<967))
		 if ((creature[c1].col>647) && (creature[c1].col<659))
			 if ((creature[c2].row>960) && (creature[c2].row<967))
				 if ((creature[c2].col>647) && (creature[c2].col<659))
		
		return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean cr_InPVPZone(int c)
// ----------------------------------------------------------------- //

{

	if ((creature[c].row>1076) && (creature[c].row<1150))
		if ((creature[c].col>1681) && (creature[c].col<1766))
			return (true);
	/**/	//Mirith Jail
			 if ((creature[c].row>960) && (creature[c].row<967))
				 if ((creature[c].col>647) && (creature[c].col<659))
			return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean cr_InTown(int c)
// ----------------------------------------------------------------- //

{
	if ((pl_AuthorizedTo(c, kIsAnyGM)) || (pl_AuthorizedTo(c, kIsDeveloper))){return (false);}
	

	if ((creature[c].row>921) && (creature[c].row<1022))
		if ((creature[c].col>665) && (creature[c].col<735))
			return (true);  //Mirith

	if ((creature[c].row>146) && (creature[c].row<195))
		if ((creature[c].col>1124) && (creature[c].col<1189))
			return (true);  //Andris 1

	if ((creature[c].row>146) && (creature[c].row<238))
		if ((creature[c].col>1068) && (creature[c].col<1125))
			return (true);  //Andris 2

	if ((creature[c].row>1619) && (creature[c].row<1628))
		if ((creature[c].col>1641) && (creature[c].col<1647))
			return (true);  //Marali 1

	if ((creature[c].row>1602) && (creature[c].row<1642))
		if ((creature[c].col>1614) && (creature[c].col<1641))
			return (true);  //Marali 2

	if ((creature[c].row>1602) && (creature[c].row<1662))
		if ((creature[c].col>1556) && (creature[c].col<1614))
			return (true);  //Marali 3

	if ((creature[c].row>758) && (creature[c].row<805))
		if ((creature[c].col>1583) && (creature[c].col<1650))
			return (true);	//Lerilin
			
	//Dave add 02-20-2011
	if ((creature[c].row>960) && (creature[c].row<967))
		if ((creature[c].col>647) && (creature[c].col<659))
			return (true);	//Mirith Jail
	
	if ((creature[c].row>172) && (creature[c].row<179))
		if ((creature[c].col>1200) && (creature[c].col<1210))
			return (true);	//Andris Jail
	
	if ((creature[c].row>1966) && (creature[c].row<1973))
		if ((creature[c].col>1927) && (creature[c].col<1936))
			return (true);	//GMRoom

	return (false);

}


// ----------------------------------------------------------------- //
void cr_SpeakSpell(int i, int t, int spell)
// ----------------------------------------------------------------- //
{
	char				magicWords[kStringLength];
	int					j;
	int					rowDelta;
	int					colDelta;

	// --------------- face direction ---------------

	rowDelta=creature[t].row-creature[i].row;
	colDelta=creature[t].col-creature[i].col;

	if ((rowDelta<=-1) && (colDelta<=-1)) 				creature[i].facing=0;
	else if ((rowDelta<=-1) && (colDelta==0)) 		creature[i].facing=1;
	else if ((rowDelta<=-1) && (colDelta>=1)) 		creature[i].facing=2;
	else if ((rowDelta==0) && (colDelta<=-1)) 		creature[i].facing=7;
	else if ((rowDelta==0) && (colDelta>=1)) 			creature[i].facing=3;
	else if ((rowDelta>=1) && (colDelta<=-1)) 		creature[i].facing=6;
	else if ((rowDelta>=1) && (colDelta==0)) 			creature[i].facing=5;
	else if ((rowDelta>=1) && (colDelta>=1)) 			creature[i].facing=4;

	strcpy(magicWords,"¥  ");

	for (j=0;j<5;j++)
		if (itemList[spell].ingredientCount[j]>0)
		{
			if (itemList[spell].ingredientID[j]==kGinseng)
				strcat(magicWords,"pur ");
			else if (itemList[spell].ingredientID[j]==kLotusFlower)
				strcat(magicWords,"clar ");
			else if (itemList[spell].ingredientID[j]==kMandrakeRoot)
				strcat(magicWords,"shel ");
			else if (itemList[spell].ingredientID[j]==kGarlic)
				strcat(magicWords,"rel ");
			else if (itemList[spell].ingredientID[j]==kVolcanicAsh)
				strcat(magicWords,"flas ");
			else if (itemList[spell].ingredientID[j]==kPoisonGland)
				strcat(magicWords,"pos ");
			else if (itemList[spell].ingredientID[j]==kElectricEel)
				strcat(magicWords,"bol ");
			else if (itemList[spell].ingredientID[j]==kAmber)
				strcat(magicWords,"ber ");
			else if (itemList[spell].ingredientID[j]==kCoal)
				strcat(magicWords,"bur ");
			else if (itemList[spell].ingredientID[j]==kDragonTooth)
				strcat(magicWords,"des ");
		}

	strcat(magicWords,"¥");

	creature[i].talkWhen	= TickCount();
	creature[i].talkTime	= TickCount();
	strcpy(creature[i].talk,magicWords);
	creature[i].talkID	  = chatMessageID;
	chatMessageID++;
}

// ----------------------------------------------------------------- //
void cr_SteppedOnLatter(int i, UInt16 *row, UInt16 *col)
// ----------------------------------------------------------------- //

{
	UInt16							trow=*row;
	UInt16							tcol=*col;
	int									j;
	TBoolean						jumpSpot;

	for (j=0;j<kMaxLadders;j++)
		if (ladderJump[j].sRow==0)
			return;
		else
			if ((ladderJump[j].sRow==trow) && (ladderJump[j].sCol==tcol))
			{
				trow=ladderJump[j].dRow;
				tcol=ladderJump[j].dCol;

				if ((map[trow][tcol]==395) || 
						(map[trow][tcol]==908) || 
							(map[trow][tcol]==900) || 
								(map[trow][tcol]==901) || 
									(map[trow][tcol]==1009) || 
										(map[trow][tcol]==1010) || 
											(map[trow][tcol]==1109))	// stepped on latter
					jumpSpot=true;
				else
					jumpSpot=false;
				

				if ((jumpSpot==false) && (creatureMap[trow][tcol]==0) && (cr_InDomain(i,trow,tcol)))
				{
					trow=trow;
					tcol=tcol;
				}
				else
				{
					cr_EmptySpotNear(i,&trow, &tcol,false);
				}

				creature[i].startRow=0;

				creature[i].startCol=0;
				creature[i].range=0;

				if (creature[i].activeFX) //Clear FX
				{
					creature[i].activeFX=false;

					for (j=0;j<kMaxCreatureEffects;j++)
						if (creature[i].fx[j]!=0)
						{
							serverFX[RC(creature[i].fx[j],kMaxFX)]	=	0;
							creature[i].fx[j]						=	0;
							creature[i].fxKillTime[j]		=	0;
							creature[i].fxTarget[j]			=	0;
							creature[i].fxTargetRow[j]	=	0;
							creature[i].fxTargetCol[j]	=	0;
						}
				}


				*row=trow;

				*col=tcol;

				if (pl_AuthorizedTo(i, kRegularPlayer))
					if (player[creature[i].playerIndex].stealth)
					{
						nw_SendDisplayMessage(creature[i].playerIndex, kLeavingStealth);
						player[creature[i].playerIndex].stealth = false;
						player[creature[i].playerIndex].stealthCool = TickCount()+3*60;
					}


				return;
			}

	*row=trow;

	*col=tcol;

}

// ----------------------------------------------------------------- //
void cr_EmptySpotNear(int i, UInt16 *row, UInt16 *col, TBoolean includeOrigin)
// ----------------------------------------------------------------- //

{
	int								r;
	int								c;
	int								tRow		=	*row;
	int								tCol		=	*col;
	int								minDist	= 10000;
	int								tempDist= 0;
	int								minRow	= 0;
	int								minCol	= 0;


	if ((tRow<=0) || (tRow>=kMapWidth) || (tCol<=0) || (tCol>=kMapHeight))
	{
		fi_WriteToErrorLog("Out of Range - cr_EmptySpotNear");
		*row=-1;
		*col=-1;
		return;
	}

	//if (includeOrigin)
	//{
	for (r=5;r>=-5;r--)
		for (c=5;c>=-5;c--)
			if (((tRow+r)>=0) && ((tRow+r)<kMapWidth) && ((tCol+c)>=0) && ((tCol+c)<kMapHeight))
				if (!(r==0 && c==0) || includeOrigin)
					if ((creatureMap[tRow+r][tCol+c]==0))
						if (cr_InDomain(i,tRow+r,tCol+c))
							if (cr_ViewBlocked(tRow, tCol, tRow+r, tCol+c)==false)
								if ((map[tRow+r][tCol+c]!=908) && (map[tRow+r][tCol+c]!=900) && (map[tRow+r][tCol+c]!=901) && (map[tRow+r][tCol+c]!=1009) && (map[tRow+r][tCol+c]!=1010))
								{
									tempDist=fabs(r)+fabs(c);

									if (tempDist<minDist)
									{
										minDist=tempDist;
										minRow=tRow+r;
										minCol=tCol+c;
									}
								}

	if (minDist<10000)
	{
		*row=minRow;
		*col=minCol;
		return;
	}

	/*}
	else
	{
		for (r=5;r>=-5;r--)
			for (c=5;c>=-5;c--)
				if (((tRow+r)>=0) && ((tRow+r)<kMapWidth) && ((tCol+c)>=0) && ((tCol+c)<kMapHeight))
					
						if ((creatureMap[tRow+r][tCol+c]==0))
							if (cr_InDomain(i,tRow+r,tCol+c))
							  if (cr_ViewBlocked(tRow, tCol, tRow+r, tCol+c)==false)
							    if ((map[tRow+r][tCol+c]!=908) && (map[tRow+r][tCol+c]!=900) && (map[tRow+r][tCol+c]!=901) && (map[tRow+r][tCol+c]!=1009) && (map[tRow+r][tCol+c]!=1010))
								{
									tempDist=fabs(r)+fabs(c);
									if (tempDist<minDist)
										{
											minDist=tempDist;
											minRow=tRow+r;
											minCol=tCol+c;
										}
								}

		if (minDist<10000)
			{
				*row=minRow;
				*col=minCol;
				return;
			}
	}*/

	*row=0;//-1;

	*col=0;//-1;

}

// ----------------------------------------------------------------- //
void cr_GetNearestFreeLocation(int i, UInt16 *row, UInt16 *col, char callingFunction[kStringLength])
// ----------------------------------------------------------------- //

{
	int								r;
	int								c;
	int								tRow		=	*row;
	int								tCol		=	*col;
	int								sRow		=	*row;
	int								sCol		=	*col;
	int								minDist	= 10000;
	int								tempDist= 0;
	int								minRow	= 0;
	int								minCol	= 0;
	int								range		= 2;
	int								search;


	if ((sRow<0) || (sRow>=kMapWidth) || (sCol<0) || (sCol>=kMapHeight))
	{
		fi_WriteToErrorLog("Out of Range - cr_GetNearestFreeLocation");
		fi_WriteToErrorLog(callingFunction);
		*row=0;
		*col=0;
		return;
	}

	if (creatureMap[sRow][sCol]==i)
		return;

	if (((creatureMap[sRow][sCol]==0) && (cr_InDomain(i,sRow,sCol))))
		return;

	if (creature[i].id==kHuman)
	{
		for (search=0;search<8;search++)
		{

			for (r=-range;r<=range;r++)
				for (c=-range;c<=range;c++)
					if (((tRow+r)>=0) && ((tRow+r)<kMapWidth) && ((tCol+c)>=0) && ((tCol+c)<kMapHeight))
					{

						if ((creatureMap[tRow+r][tCol+c]==0))
							if (cr_InDomain(i,tRow+r,tCol+c))
							{

								tempDist=fabs(r)+fabs(c);

								if (tempDist==1)
								{
									*row=tRow+r;
									*col=tCol+c;
									return;
								}

								if (tempDist<minDist)
								{
									minDist=tempDist;
									minRow=tRow+r;
									minCol=tCol+c;
								}

							}

					}

			if (minDist<10000)
			{
				*row=minRow;
				*col=minCol;
				return;
			}

			range=range*2;

		}
	}
	else
	{

		for (r=-5;r<=5;r++)
			for (c=-5;c<=5;c++)
				if (((tRow+r)>=0) && ((tRow+r)<kMapWidth) && ((tCol+c)>=0) && ((tCol+c)<kMapHeight))
				{

					if ((creatureMap[tRow+r][tCol+c]==0))
						if (cr_InDomain(i,tRow+r,tCol+c))
						{

							tempDist=fabs(r)+fabs(c);

							if (tempDist==1)
							{
								*row=tRow+r;
								*col=tCol+c;
								return;
							}

							if (tempDist<minDist)
							{
								minDist=tempDist;
								minRow=tRow+r;
								minCol=tCol+c;
							}

						}

				}

		if (minDist<10000)
		{
			*row=minRow;
			*col=minCol;
			return;
		}

	}

	*row=minRow;

	*col=minCol;

}

// ----------------------------------------------------------------- //
int cr_SpawnCreature(UInt16 id, UInt16 row, UInt16 col)
// ----------------------------------------------------------------- //

{
	int								i;
	int								j;
	int								cpt;
	UInt32						gold;
	int								gold1;
	int								gold2;
	UInt16						theRow;
	UInt16						theCol;
	//int               rare;
	unsigned long			currentTime;

	//#ifdef _FUNCTIONTRACEDEBUGGING
	//fi_WriteToErrorLog("cr_SpawnCreature");
	//#endif


	if ((id<0) || (id>=kMaxCreatureTypes))			// beta debug
	{
		fi_WriteToErrorLog("Out of Range - cr_SpawnCreature ID");
		return (-1);
	}


	if ((row<0) || (row>=kMapWidth) || (col<0) || (col>=kMapHeight))
	{
		fi_WriteToErrorLog("Out of Range - cr_SpawnCreature location");
		return (-1);
	}

	for (i=creatureSpawnPtr;i<kMaxCreatures;i++)
	{
		//if ((i<0) || (i>=kMaxCreatures))// beta debug
		//	fi_WriteToErrorLog("Out of Range - cr_SpawnCreature 2");

		if (creature[i].id==0)
		{
			theRow=row;
			theCol=col;

			creature[i].id									      =	id;
			cr_GetNearestFreeLocation(i, &theRow, &theCol,"cr_SpawnCreature");

			if ((theRow==0) && (theCol==0))
			{
				creature[i].id									  =	0;
				return (-1);
			}

			currentTime=TickCount();

			creatureMap[theRow][theCol]			      =	i;
			creature[i].id									      =	id;
			creature[i].row									      =	theRow;
			creature[i].col									      =	theCol;
			creature[i].moveDirection				      = 4;
			creature[i].nextMoveTime				      =	currentTime+creatureInfo[id].speed+tb_Rnd(0,120);
			creature[i].dieTime				            =	0;
			creature[i].hiding	=false;
			creature[i].npcIndex =0;
			creature[i].playerIndex =0;
			creature[i].timeStamp						      = currentTime;
			creature[i].strikePose					      = 0;
			creature[i].nextStrikeTime			      = 0;
			creature[i].numberOfStrikes			      = 0;
			creature[i].hitPoints						      =	creatureInfo[id].maxHitPoints;
			creature[i].strength						      = creatureInfo[id].maxStrength;
			creature[i].intelligence				      = creatureInfo[id].maxIntelligence;
			creature[i].dexterity						      = creatureInfo[id].maxDexterity;
			creature[i].magicPoints					      = creatureInfo[id].maxMagicPoints;
			creature[i].alignment						      = creatureInfo[id].alignment;
			creature[i].strengthOffset			      = 0;
			creature[i].intelligenceOffset	      = 0;
			creature[i].dexterityOffset			      = 0;
			creature[i].strengthTime				      = 0;
			creature[i].intelligenceTime		      = 0;
			creature[i].dexterityTime				      = 0;
			creature[i].cornered				          = false;
			creature[i].wounded				            = false;
			creature[i].respawn				            = true;
			creature[i].level				              = creatureInfo[id].creatureLevel;
			creature[i].goodTarget								= 0;
			creature[i].respawnTime				        = currentTime+tb_Rnd(18000,5184000);
			creature[i].nextTrackRow              = 0;
			creature[i].nextTrackCol              = 0;
			creature[i].possibleTarget            = 0;
			creature[i].regeneratingTime		=0;
			creature[i].regeneratingPower		=0;
			creature[i].mute					  =false;
			creature[i].noDrop					  =false;
			creature[i].autoLock				=false;
			creature[i].godMode					=false;
			creature[i].noDeath					=false;
			creature[i].opportunityTime			=0;
			
			for (cpt=0; cpt<kMaxMacroTrace; cpt++)
			{
				creature[i].attackTargets[cpt][0]=0;
				creature[i].attackTargets[cpt][1]=0;
			}

			creatureInfo[id].population++;

			for (j=1;j<kMaxInventorySize;j++)
			{

				/*
					if (creatureInfo[id].inventoryItem[j]==0)
					  {
					    if ((creatureInfo[id].rareLevel>0) && (tb_Rnd(0,99)<creatureInfo[id].rareChance))
					      {
					        rare	=	it_GetRare(creatureInfo[id].rareLevel);
					        if (rare>0)
					          {
					            creature[i].inventory[j].itemID=rare;
					            if (in_CanGroup(rare))
					              creature[i].inventory[j].itemCount=1;
					            else
					              creature[i].inventory[j].itemCount=127;
					          }
					      }
					    break;
					  }
					 */

				creature[i].inventory[j].itemID						=	creatureInfo[id].inventoryItem[j];

				if (in_CanGroup(creature[i].inventory[j].itemID))
					creature[i].inventory[j].itemCount			=	tb_Rnd(creatureInfo[id].inventoryMinCount[j],creatureInfo[id].inventoryMaxCount[j]);
				else
				{
					if (tb_Rnd(1,creatureInfo[id].inventoryMaxCount[j])==1)
						creature[i].inventory[j].itemCount =	127;
					else
					{
						creature[i].inventory[j].itemID =	0;
						creature[i].inventory[j].itemCount =	0;
					}
				}

				if (creature[i].inventory[j].itemCount==0)
					creature[i].inventory[j].itemID=0;

				if (creature[i].inventory[j].itemID==0)
					creature[i].inventory[j].itemCount=0;
			}


			gold=tb_Rnd(creatureInfo[id].minGold,creatureInfo[id].maxGold);

			in_SetGoldTotal(gold, &gold1, &gold2);
			creature[i].inventory[0].itemID			=	gold1;
			creature[i].inventory[0].itemCount	=	gold2;
			creatureSpawnPtr =	i+1;

			if (creatureSpawnPtr>=kMaxCreatures)
				creatureSpawnPtr=5000;
			
			return (i);
		}
	}

	for (i=5000;i<creatureSpawnPtr;i++)
	{

		if (creature[i].id==0)
		{
			theRow=row;
			theCol=col;

			creature[i].id									      =	id;
			cr_GetNearestFreeLocation(i, &theRow, &theCol,"cr_SpawnCreature");

			if ((theRow==0) && (theCol==0))
			{
				creature[i].id									  =	0;
				return (-1);
			}

			currentTime=TickCount();

			creatureMap[theRow][theCol]			      =	i;
			creature[i].id									      =	id;
			creature[i].row									      =	theRow;
			creature[i].col									      =	theCol;
			creature[i].moveDirection				      = 4;
			creature[i].nextMoveTime				      =	currentTime+creatureInfo[id].speed+tb_Rnd(0,120);
			creature[i].dieTime				            =	0;
			creature[i].hiding	=false;
			creature[i].npcIndex =0;
			creature[i].playerIndex =0;
			creature[i].timeStamp						      = currentTime;
			creature[i].strikePose					      = 0;
			creature[i].nextStrikeTime			      = 0;
			creature[i].numberOfStrikes			      = 0;
			creature[i].hitPoints						      =	creatureInfo[id].maxHitPoints;
			creature[i].strength						      = creatureInfo[id].maxStrength;
			creature[i].intelligence				      = creatureInfo[id].maxIntelligence;
			creature[i].dexterity						      = creatureInfo[id].maxDexterity;
			creature[i].magicPoints					      = creatureInfo[id].maxMagicPoints;
			creature[i].alignment						      = creatureInfo[id].alignment;
			creature[i].strengthOffset			      = 0;
			creature[i].intelligenceOffset	      = 0;
			creature[i].regeneratingTime		=0;
			creature[i].regeneratingPower		=0;
			creature[i].dexterityOffset			      = 0;
			creature[i].strengthTime				      = 0;
			creature[i].intelligenceTime		      = 0;
			creature[i].dexterityTime				      = 0;
			creature[i].cornered				          = false;
			creature[i].respawn				            = true;
			creature[i].level				              = creatureInfo[id].creatureLevel;
			creature[i].goodTarget								= 0;
			creature[i].respawnTime				        = currentTime+tb_Rnd(18000,5184000);
			creature[i].nextTrackRow              = 0;
			creature[i].nextTrackCol              = 0;
			creature[i].possibleTarget            = 0;
			creature[i].mute						=false;
			creature[i].noDrop					  	=false;
			creature[i].godMode						=false;
			creature[i].noDeath						=false;
			creature[i].autoLock					=false;
			creature[i].opportunityTime				=0;

			for (cpt=0; cpt<kMaxMacroTrace; cpt++)
			{
				creature[i].attackTargets[cpt][0]=0;
				creature[i].attackTargets[cpt][1]=0;
			}

			creatureInfo[id].population++;

			for (j=1;j<kMaxInventorySize;j++)
			{

				/*
					if (creatureInfo[id].inventoryItem[j]==0)
					  {
					    if ((creatureInfo[id].rareLevel>0) && (tb_Rnd(0,99)<creatureInfo[id].rareChance))
					      {
					        rare	=	it_GetRare(creatureInfo[id].rareLevel);
					        if (rare>0)
					          {
					            creature[i].inventory[j].itemID=rare;
					            if (in_CanGroup(rare))
					              creature[i].inventory[j].itemCount=1;
					            else
					              creature[i].inventory[j].itemCount=127;
					          }
					      }
					    break;
					  }
					*/

				creature[i].inventory[j].itemID						=	creatureInfo[id].inventoryItem[j];

				if (in_CanGroup(creature[i].inventory[j].itemID))
					creature[i].inventory[j].itemCount			=	tb_Rnd(creatureInfo[id].inventoryMinCount[j],creatureInfo[id].inventoryMaxCount[j]);
				else
				{
					if (tb_Rnd(1,creatureInfo[id].inventoryMaxCount[j])==1)
						creature[i].inventory[j].itemCount =	127;
					else
					{
						creature[i].inventory[j].itemID =	0;
						creature[i].inventory[j].itemCount =	0;
					}
				}

				if (creature[i].inventory[j].itemCount==0) // put in carbon 11/24
					creature[i].inventory[j].itemID=0;

				if (creature[i].inventory[j].itemID==0) // put in carbon 11/24
					creature[i].inventory[j].itemCount=0;

			}

			gold=tb_Rnd(creatureInfo[id].minGold,creatureInfo[id].maxGold);

			in_SetGoldTotal(gold, &gold1, &gold2);
			creature[i].inventory[0].itemID			=	gold1;
			creature[i].inventory[0].itemCount	=	gold2;
			creatureSpawnPtr							=	i+1;

			if (creatureSpawnPtr>=kMaxCreatures)
				creatureSpawnPtr=5000;

			return (i);
		}
	}

	return (-1);

}

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean cr_ImmuneToFF(int	type)
// ----------------------------------------------------------------- //
{
	if (type==kRedDragon)				return true;
	if (type==kForestDragon)			return true;
	if (type==kBlackDragon)				return true;
	if (type==kPinkDragon)				return true;
	if (type==kLich)					return true;
	if (type==kIceDragon)				return true;
	if (type==kHillGiant)				return true;
	if (type==kCentaur)					return true;
	if (type==kElderCentaur)			return true;
	if (type==kTusker)					return true;
	if (type==kRedDragonAncient)		return true;
	if (type==kForestDragonAncient)		return true;
	if (type==kBlackDragonAncient)		return true;
	if (type==kIceDragonAncient)		return true;
	//Dave
	if (type==kHellHound)				return true;
	if (type==kRedDraco)				return true;
	//if (type==kForestDraco)				return true;
	//if (type==kIceDraco)				return true;
	//if (type==kSandDraco)				return true;
	//if (type==kBlackDraco)				return true;
	if (type==kGolem)					return true;
	if (type==kMagmaGolem)				return true;
	if (type==kIceGolem)				return true;
	if (type==kSandGolem)				return true;
	if (type==kGolem)					return true;
	if (type==kSandDragon)				return true;
	if (type==kHellMage)				return true;
	
	return false;
}


// ----------------------------------------------------------------- //
TBoolean cr_InDomain(int	i, int row, int col)
// ----------------------------------------------------------------- //

{

	if ((row<0) || (row>=kMapWidth) || (col<0) || (col>=kMapHeight))
	{
		fi_WriteToErrorLog("Out of Range - cr_InDomain");
		return (false);
	}

	if (creature[i].id!=kHuman)
	{
		if (fxMap[row][col]<kMaxFX)
			if (serverFX[fxMap[row][col]]==kFireField)	 // standing in fire
			  if (cr_ImmuneToFF(creature[i].id)==false)
				return (false);
	}

	if (creatureMap[row][col]	!= 0)
		return (false);

	if (containerMap[row][col]!=0)
		if (gmProp[containerID[containerMap[row][col]]])
			return (false);

	if (creatureInfo[creature[i].id].domain==kLand)
		if (canWalkOn[map[row][col]])
			return (true);

	if (creatureInfo[creature[i].id].domain==kAir)
		if (canFlyOver[map[row][col]])
			return (true);

	if (creatureInfo[creature[i].id].domain==kWater)
		if (canSwimIn[map[row][col]])
			return (true);

	if (creature[i].id==kHuman)
		if (canSwimIn[map[row][col]])
			if (in_QuantityOf_s(i,kRaft)>0)
				return (true);

	return (false);

}

// ----------------------------------------------------------------- //
void cr_CreatureMoveTo (int i, UInt16 row, UInt16 col)
// ----------------------------------------------------------------- //
{
	int														oRow;
	int														oCol;
	//int														test;
	//char													debug[255];

	oRow=creature[i].row;
	oCol=creature[i].col;

	creatureMap[oRow][oCol]	=	0;

	cr_GetNearestFreeLocation(i, &row, &col,"cr_creatureMoveTo");

	if ((row<=0) && (col<=0))
	{
		row=oRow;
		col=oCol;
	}

	creature[i].row							=	row;

	creature[i].col							=	col;
	creatureMap[row][col]	=	i;
	creature[i].moveDirection		=	4;
	creature[i].timeStamp				= TickCount();
	creature[i].strikePose			= 0;
	creature[i].nextStrikeTime	= 0;
	creature[i].numberOfStrikes	= 0;
	creature[i].attackTarget		= 0;

	strcpy(creature[i].talk, "");
	creature[i].talkWhen=0;
	creature[i].talkTime=0;
	creature[i].talkID=0;
}


#endif

// ----------------------------------------------------------------- // // put in carbon 11/20
void cr_Dead(int c)
// ----------------------------------------------------------------- //

{
	int						i,j;
	long					gold;
	int           master;
	TBoolean      SendRefresh;
	TBoolean      SendSkillRefresh;
	TBoolean		found;
	char			theName[kStringLength];
	UInt16        row;
	UInt16        col;


	if ((c<0) || (c>=kMaxCreatures))// beta debug
		fi_WriteToErrorLog("Out of Range - cr_Dead");

	if (creature[c].playerIndex!=0 && (creature[c].noDeath || creature[c].godMode))  //God mode
	{
		creature[c].hitPoints=1;
		creature[c].dieTime = 0;
		return;
	}
	
	if (creature[c].master!=0 && cr_InPVPZone(c)) //pets in pvp ->stable with 1hp
	{
		creature[c].hitPoints=1;
		creature[c].dieTime = 0;
		
		for (j=0; j<kMaxPets; j++)
		{
			if (player[creature[creature[c].master].playerIndex].pet[j]==c)
			{
				pl_StablePet(creature[creature[c].master].playerIndex, j);
			}
		}
	}

	for (j=0;j<kMaxCreatureEffects;j++)
		if (creature[c].fx[j]>0)
		{
			serverFX[RC(creature[c].fx[j],kMaxFX)]	=	0;
			creature[c].fx[j]						=	0;
			creature[c].fxKillTime[j]		=	0;
			creature[c].fxTarget[j]			=	0;
			creature[c].fxTargetRow[j]	=	0;
			creature[c].fxTargetCol[j]	=	0;
			creature[c].activeFX				= false;
		}

	if (doorway[map[creature[c].row][creature[c].col]])  //If creature is in a doorway, move BEFORE inventory dropping to an empty spot to prevent inventory deletion
	{
		row=creature[c].row;
		col=creature[c].col;
		found=false;

		for (j=0; j<25; j++)
		{
			if (!found)
			{
				cr_EmptySpotNear(c,&row, &col,false);

				if (strcmp(containerName[containerMap[row][col]], "")==0 && strcmp(containerGift[containerMap[row][col]], "")==0)
					//if (cr_ViewBlocked(creature[c].row, creature[c].col, row, col)==false)
					found=true;
			}
		}

		if (found==false)
			row=0;

		if ((row>0) && (col>0))
		{
			if (creature[c].playerIndex==0)//Monster
				cr_CreatureMoveTo(c, row, col);
			else
				pl_PlayerMoveTo(creature[c].playerIndex,row,col);
		}
	}

	if (containerMap[creature[c].row][creature[c].col]!=0)
		if (strcmp(containerName[containerMap[creature[c].row][creature[c].col]], "")!=0 || strcmp(containerGift[containerMap[creature[c].row][creature[c].col]], "")!=0)//Died on another player's corpse or an /ownered container, move BEFORE inventory dropping to an empty spot to prevent inventory deletion
		{
			row=creature[c].row;
			col=creature[c].col;
			found=false;

			for (j=0; j<25; j++)
			{
				if (!found)
				{
					cr_EmptySpotNear(c,&row, &col,false);

					if (strcmp(containerName[containerMap[row][col]], "")==0 && strcmp(containerGift[containerMap[row][col]], "")==0)
						//if (cr_ViewBlocked(creature[c].row, creature[c].col, row, col)==false)
						found=true;
				}
			}

			if (found==false)
				row=0;

			if ((row>0) && (col>0))
			{
				if (creature[c].playerIndex==0)//Monster
					cr_CreatureMoveTo(c, row, col);
				else
					pl_PlayerMoveTo(creature[c].playerIndex,row,col);
			}
		}
	
	cr_ClearSpat(c, true);
	
	if (creature[c].playerIndex==0) // not player controlled ------------------------------------------------
	{
		if (creature[c].home>0)
		{
			spawnRegion[creature[c].home].currentPopulation--;

			if (spawnRegion[creature[c].home].currentPopulation<0)
				spawnRegion[creature[c].home].currentPopulation++;
		}

		if (creature[c].npcIndex!=0)
		{
			if (npc[creature[c].npcIndex].spawned==false && creature[c].alignment!=kEvil) //npc comes from the NPC settings sheets, unkillable
				//if (creature[c].master==0)
				{
					creature[c].hitPoints   = creatureInfo[creature[c].id].maxHitPoints;
					creature[c].dieTime		  =	0;
					return;
				}
		}

#ifdef _SERVERONLY
		creatureDeaths++;

#endif
		creatureInfo[creature[c].id].population--;

		if ((creature[c].master!=0 && creature[c].npcIndex==0) || creature[c].noDrop==true)
		{
			if (creatureInfo[creature[c].id].corpseID!=1)
				in_MakeContainer(creature[c].row, creature[c].col, creatureInfo[creature[c].id].corpseID, false); // put in carbon 11/12
		}
		else
		{
			// drop inventory
			gold=in_GetGoldTotal(creature[c].inventory[0].itemID, creature[c].inventory[0].itemCount);

			if (gold>0)
				in_DropGold(creatureInfo[creature[c].id].corpseID,gold, creature[c].row, creature[c].col, kRandomDecay, true, "");

			for (j=1;j<kMaxInventorySize;j++)
				if (creature[c].inventory[j].itemID!=0)
				{
					if ((in_CanGroup(creature[c].inventory[j].itemID)==false) && (creature[c].inventory[j].itemCount<25))
						continue;

					in_DropItem(c,creatureInfo[creature[c].id].corpseID,creature[c].inventory[j].itemID, creature[c].inventory[j].itemCount, creature[c].row, creature[c].col, kThreeMinuteDecay, true, "");
				}

			if (creatureInfo[creature[c].id].lockStrength>0)		
				if (tb_Rnd(1,3)>=2) //Dave change 03-31-2012 if (tb_Rnd(1,2)==1)
					in_DropItem(c,1,kLock, 1, creature[c].row, creature[c].col, tb_Rnd(creatureInfo[creature[c].id].size,creatureInfo[creature[c].id].lockStrength), false, ""); //Dave change 03-31-2012 in_DropItem(c,1,kLock, 1, creature[c].row, creature[c].col, tb_Rnd(1,creatureInfo[creature[c].id].lockStrength), false, "");
		}

		// clear creature
		creatureMap[creature[c].row][creature[c].col]	= 0;

		SendRefresh=false;

		if (creature[c].master!=0)
		{
			master=creature[creature[c].master].playerIndex;

			for (j=0;j<kMaxPets;j++)
				if (player[creature[creature[c].master].playerIndex].pet[j]==c)
				{
					player[creature[creature[c].master].playerIndex].sendPetsRefresh	=	true;
					player[creature[creature[c].master].playerIndex].pet[j]					  =	0;
					player[creature[creature[c].master].playerIndex].petInStable[j]	  = false;
					player[creature[creature[c].master].playerIndex].petType[j]			  = 0;
					player[creature[creature[c].master].playerIndex].petExperience[j]			  = 0;
					player[creature[creature[c].master].playerIndex].petHealth[j]		=0;
					strcpy(player[creature[creature[c].master].playerIndex].petName[j],"");
					SendRefresh=true;
					break;
				}
		}

		if (SendRefresh)
			nw_SendPetNameRefresh(master);

		cr_ClearCreatureRecord(c);
	}
	else	// player controlled ---------------------------------------------------------------------------------------------------------------------------
	{
		if (player[creature[c].playerIndex].morph!=0)
		{
			if (pl_AuthorizedTo(c, kRegularPlayer)==false)
			{
				//player doesn't really die
				creature[c].hitPoints=1;
				creature[c].dieTime=0;
				
				//break attack
				creature[c].attackTarget=0;
				
				for (i=creature[c].row-8; i<creature[c].row+8; i++)
					for (j=creature[c].col-8; j<creature[c].col+8; j++)
						if (i>0)
							if (i<kMapWidth)
								if (j>0)
									if (j<kMapHeight)
										if (creatureMap[i][j]!=0)
											if (creature[creatureMap[i][j]].attackTarget==c)
												creature[creatureMap[i][j]].attackTarget=0;
				
				//insta stealth
				player[creature[c].playerIndex].stealth=true;
				nw_SendSystemTextToClient(creature[c].playerIndex, "Stealth On");
				
				//drop dummy body
				if (containerMap[creature[c].row][creature[c].col]==0)
				{
					in_MakeContainer(creature[c].row, creature[c].col, creatureInfo[player[creature[c].playerIndex].morph].corpseID, true); // carbon 11/12
				}
				return;	
			}
			else
			{
				player[creature[c].playerIndex].morph=0;
				nw_SendClearName(creature[c].playerIndex);
			}
			
		}
		
#ifdef _SERVERONLY
		playerDeaths++;
#endif

		SendSkillRefresh=false;

		if (player[creature[c].playerIndex].online)
			if (pl_AuthorizedTo(c, kRegularPlayer))
				if (cr_InPVPZone(c)==false)
					for (j=0;j<kMaxSkills;j++)
						if (player[creature[c].playerIndex].skill[j].level>425)
							
							{
								player[creature[c].playerIndex].skill[j].level =	player[creature[c].playerIndex].skill[j].level - tb_Rnd(15,35); // Skill drop on death was 25
							SendSkillRefresh=true;
						}

		if (SendSkillRefresh)
			nw_SendSkillsRefresh(creature[c].playerIndex);

		
		if (player[creature[c].playerIndex].gender==1)
			cr_AddSound(c, kFemaleGaspSnd);
		else
			cr_AddSound(c, kGaspSnd);

		//creature[c].soundFX					      = kGaspSnd;
		//creature[c].soundFXTime			      = TickCount();

		//pl_StablePets(creature[c].playerIndex);	// 0.1.4
		for (j=0;j<kMaxPets;j++)
			if (player[creature[c].playerIndex].pet[j]!=0)
				if (!player[creature[c].playerIndex].petInStable[j])
				{
					creature[player[creature[c].playerIndex].pet[j]].command 					= kAttack;//Dave dead master pet was kHeel
					creature[player[creature[c].playerIndex].pet[j]].attackTarget			=	0;
					creature[player[creature[c].playerIndex].pet[j]].nextStrikeTime		=	0;
					creature[player[creature[c].playerIndex].pet[j]].numberOfStrikes	=	0;
					creature[player[creature[c].playerIndex].pet[j]].trackRow					= 0;
					creature[player[creature[c].playerIndex].pet[j]].trackCol					= 0;
					creature[player[creature[c].playerIndex].pet[j]].tracking					= false;
				}

		// drop inventory ----------------------------------------------------------------

		strcpy(theName, player[creature[c].playerIndex].playerName);

		if (strcmp(player[creature[c].playerIndex].playerNameTemp, "none")!=0)
			if (strcmp(player[creature[c].playerIndex].playerNameTemp, "None")!=0)
				strcpy(theName, player[creature[c].playerIndex].playerNameTemp);


		gold=in_GetGoldTotal(creature[c].inventory[0].itemID, creature[c].inventory[0].itemCount);

		if (gold>0)
		{
			if (player[creature[c].playerIndex].gender==0)
				in_DropGold(14,gold, creature[c].row, creature[c].col, 45, true, theName);	// 45 minute human corpse decay
			else
				in_DropGold(124,gold, creature[c].row, creature[c].col, 45, true, theName);	// 45 minute human corpse decay
		}//in_DropGold(creatureInfo[creature[c].id].corpseID,gold, creature[c].row, creature[c].col, 45, true, theName);	// 45 minute human corpse decay

		if (strcmp("brigand",player[creature[c].playerIndex].playerNameLC)==0)  // mod 8/4
			in_AddtoInventory(c, kBrigandHead, 1);

		if (strcmp("pirate",player[creature[c].playerIndex].playerNameLC)==0)  // mod 8/4
			in_AddtoInventory(c, kPirateHead, 1);

		if (strcmp("sorcerer",player[creature[c].playerIndex].playerNameLC)==0)  // mod 8/4
			in_AddtoInventory(c, kSorcererHead, 1);

		for (j=1;j<kMaxInventorySize;j++)
			if (creature[c].inventory[j].itemID!=0)
			{
				if (itemList[creature[c].inventory[j].itemID].blessed==false)
				{
					if (player[creature[c].playerIndex].gender==0)
						in_DropItem(c,14,creature[c].inventory[j].itemID, creature[c].inventory[j].itemCount, creature[c].row, creature[c].col, 45, true, theName);  // 45 minute human corpse decay
					else
						in_DropItem(c,124,creature[c].inventory[j].itemID, creature[c].inventory[j].itemCount, creature[c].row, creature[c].col, 45, true, theName);  // 45 minute human corpse decay
				}//in_DropItem(c,creatureInfo[creature[c].id].corpseID,creature[c].inventory[j].itemID, creature[c].inventory[j].itemCount, creature[c].row, creature[c].col, 45, true, theName);  // 45 minute human corpse decay
			}

		if (cr_InPVPZone(c) || pl_AuthorizedTo(c, kIsAnyGM) || (pl_AuthorizedTo(c, kIsQuestCharacter) && creature[c].autoLock==true)) //PVP/GMs/Qchars - auto lock
			strcpy(containerGift[containerMap[creature[c].row][creature[c].col]], player[creature[c].playerIndex].playerName);

		for (j=0;j<kMaxInventorySize;j++)
			if (itemList[creature[c].inventory[j].itemID].blessed==false)
			{
				creature[c].inventory[j].itemID			=0;
				creature[c].inventory[j].itemCount	=0;
			}

		//---------------------------------------------------------------------------------

		for (j=0;j<13;j++)
			player[creature[c].playerIndex].arm[j]							=	0;

		for (j=1;j<kMaxInventorySize;j++)
			if (creature[c].inventory[j].itemID==0)
			{
				creature[c].inventory[j].itemID							=	kGrayRobe;

				creature[c].inventory[j].itemCount						=	1;
				player[creature[c].playerIndex].arm[kTorso]				=	j;
				it_CalculateArmorRating(creature[c].playerIndex);	//Recalculate Armor Rating
				break;
			}

		nw_SendInventoryRefresh(creature[c].playerIndex);

		nw_SendArmRefresh(creature[c].playerIndex);
		player[creature[c].playerIndex].curse								=	0;
		player[creature[c].playerIndex].gathering							=	0;
		player[creature[c].playerIndex].gatherType							= 	0;
		player[creature[c].playerIndex].gatherRow							=	0;
		player[creature[c].playerIndex].gatherCol							=	0;
		player[creature[c].playerIndex].meditating							=	false;
		player[creature[c].playerIndex].healing								=	false;
		player[creature[c].playerIndex].revealHidden						=	0;
		player[creature[c].playerIndex].thiefUntil						  	=	0;
		creature[c].poison																			=	0;
		creature[c].paralyzed																		= 0;
		creature[c].disorientation															= 0;
		creature[c].lifespan															      = 0;
		creature[c].calmUntil															      = 0;
		creature[c].attackTarget																=	0;
		creature[c].reflectDamage																=	0;
		player[creature[c].playerIndex].dead								=	true;
		creature[c].hitPoints																		=	0;
		creature[c].magicPoints																	=	0;
		creature[c].magicReflection															= false;
		creature[c].strengthOffset															=	0;
		creature[c].intelligenceOffset													=	0;
		creature[c].regeneratingTime		=0;
		creature[c].regeneratingPower		=0;
		creature[c].dexterityOffset															= 0;
		creature[c].strengthTime																=	0;
		creature[c].intelligenceTime														=	0;
		creature[c].dexterityTime																= 0;
		creature[c].dieTime																      = 0;
		creature[c].beingAttacked																= 0;
		creature[c].finalStrikeSound                            = 0;


	}

}

// ----------------------------------------------------------------- //
int cr_TamedCreatureBetween(int Ax, int Ay, int Bx, int By)
// ----------------------------------------------------------------- //

{
	//------------------------------------------------------------------------
	// INITIALIZE THE COMPONENTS OF THE ALGORITHM THAT ARE NOT AFFECTED BY THE
	// SLOPE OR DIRECTION OF THE LINE
	//------------------------------------------------------------------------
	int dX = abs(Bx-Ax);	// store the change in X and Y of the line EndPoints
	int dY = abs(By-Ay);

	int	Sx = Ax;
	int	Sy = Ay;
	//------------------------------------------------------------------------
	// DETERMINE "DIRECTIONS" TO INCREMENT X AND Y (REGARDLESS OF DECISION)
	//------------------------------------------------------------------------
	int Xincr, Yincr;

	if (Ax > Bx) { Xincr=-1; }
	else { Xincr=1; }	// which direction in X?

	if (Ay > By) { Yincr=-1; }
	else { Yincr=1; }	// which direction in Y?

	//------------------------------------------------------------------------
	// DETERMINE INDEP}ENT VARIABLE (ONE THAT ALWAYS INCREMENTS BY 1 (OR -1) )
	// AND INITIATE APPROPRIATE LINE DRAWING ROUTINE (BASED ON FIRST OCTANT
	// ALWAYS). THE X AND Y'S MAY BE FLIPPED IF Y IS THE INDEP}ENT VARIABLE.
	//------------------------------------------------------------------------
	if (dX >= dY)	// if X is the indep}ent variable
	{
		int dPr 	= dY<<1;           // amount to increment decision if right is chosen (always)
		int dPru 	= dPr - (dX<<1);   // amount to increment decision if up is chosen
		int P 		= dPr - dX;  // decision variable start value

		for (; dX>=0; dX--)            // process each point in the line one at a time (just use dX)
		{

			if (creatureMap[Ax][Ay]!=0)
				if (creature[creatureMap[Ax][Ay]].master!=0)
					return (creatureMap[Ax][Ay]);

			if (P > 0)               // is the pixel going right AND up?
			{
				Ax+=Xincr;	       // increment indep}ent variable
				Ay+=Yincr;         // increment dep}ent variable
				P+=dPru;           // increment decision (for up)
			}
			else                     // is the pixel just going right?
			{
				Ax+=Xincr;         // increment indep}ent variable
				P+=dPr;            // increment decision (for right)
			}
		}
	}
	else              // if Y is the indep}ent variable
	{
		int dPr 	= dX<<1;           // amount to increment decision if right is chosen (always)
		int dPru 	= dPr - (dY<<1);   // amount to increment decision if up is chosen
		int P 		= dPr - dY;  // decision variable start value

		for (; dY>=0; dY--)            // process each point in the line one at a time (just use dY)
		{

			if (creatureMap[Ax][Ay]!=0)
				if (creature[creatureMap[Ax][Ay]].master!=0)
					return (creatureMap[Ax][Ay]);

			if (P > 0)               // is the pixel going up AND right?
			{
				Ax+=Xincr;         // increment dep}ent variable
				Ay+=Yincr;         // increment indep}ent variable
				P+=dPru;           // increment decision (for up)
			}
			else                     // is the pixel just going up?
			{
				Ay+=Yincr;         // increment indep}ent variable
				P+=dPr;            // increment decision (for right)
			}
		}
	}

	return (0);

}

// ----------------------------------------------------------------- //
TBoolean cr_ObstacleBetween(int Ax, int Ay, int Bx, int By, int domain)
// ----------------------------------------------------------------- //

{
	//------------------------------------------------------------------------
	// INITIALIZE THE COMPONENTS OF THE ALGORITHM THAT ARE NOT AFFECTED BY THE
	// SLOPE OR DIRECTION OF THE LINE
	//------------------------------------------------------------------------
	int dX = abs(Bx-Ax);	// store the change in X and Y of the line EndPoints
	int dY = abs(By-Ay);

	int	Sx = Ax;
	int	Sy = Ay;
	//------------------------------------------------------------------------
	// DETERMINE "DIRECTIONS" TO INCREMENT X AND Y (REGARDLESS OF DECISION)
	//------------------------------------------------------------------------
	int Xincr, Yincr;

	if (Ax > Bx) { Xincr=-1; }
	else { Xincr=1; }	// which direction in X?

	if (Ay > By) { Yincr=-1; }
	else { Yincr=1; }	// which direction in Y?

	//------------------------------------------------------------------------
	// DETERMINE INDEP}ENT VARIABLE (ONE THAT ALWAYS INCREMENTS BY 1 (OR -1) )
	// AND INITIATE APPROPRIATE LINE DRAWING ROUTINE (BASED ON FIRST OCTANT
	// ALWAYS). THE X AND Y'S MAY BE FLIPPED IF Y IS THE INDEP}ENT VARIABLE.
	//------------------------------------------------------------------------
	if (dX >= dY)	// if X is the indep}ent variable
	{
		int dPr 	= dY<<1;           // amount to increment decision if right is chosen (always)
		int dPru 	= dPr - (dX<<1);   // amount to increment decision if up is chosen
		int P 		= dPr - dX;  // decision variable start value

		for (; dX>=0; dX--)            // process each point in the line one at a time (just use dX)
		{

			if ((canWalkOn[map[Ax][Ay]]==false) && (canSwimIn[map[Ax][Ay]]==false) && (elevated[map[Ax][Ay]]))
				return (true);

			if ((canWalkOn[map[Ax][Ay]]) && (domain==kWater))
				return (true);

			if ((canSwimIn[map[Ax][Ay]]) && (domain==kLand))
				return (true);

			if (!(((Ax==Sx) && (Ay==Sy)) || ((Ax==Bx) && (Ay==By))))
				if (creatureMap[Ax][Ay]!=0)
					return (true);

			if ((doorway[map[Ax][Ay]]) && ((Sx!=Ax) || (Sy!=Ay)))
				return (true);

			if (P > 0)               // is the pixel going right AND up?
			{
				Ax+=Xincr;	       // increment indep}ent variable
				Ay+=Yincr;         // increment dep}ent variable
				P+=dPru;           // increment decision (for up)
			}
			else                     // is the pixel just going right?
			{
				Ax+=Xincr;         // increment indep}ent variable
				P+=dPr;            // increment decision (for right)
			}
		}
	}
	else              // if Y is the indep}ent variable
	{
		int dPr 	= dX<<1;           // amount to increment decision if right is chosen (always)
		int dPru 	= dPr - (dY<<1);   // amount to increment decision if up is chosen
		int P 		= dPr - dY;  // decision variable start value

		for (; dY>=0; dY--)            // process each point in the line one at a time (just use dY)
		{

			if ((canWalkOn[map[Ax][Ay]]==false) && (canSwimIn[map[Ax][Ay]]==false) && (elevated[map[Ax][Ay]]))
				return (true);

			if ((canWalkOn[map[Ax][Ay]]) && (domain==kWater))
				return (true);

			if ((canSwimIn[map[Ax][Ay]]) && (domain==kLand))
				return (true);

			if (!(((Ax==Sx) && (Ay==Sy)) || ((Ax==Bx) && (Ay==By))))
				if (creatureMap[Ax][Ay]!=0)
					return (true);

			if ((doorway[map[Ax][Ay]]) && ((Sx!=Ax) || (Sy!=Ay)))
				return (true);

			if (P > 0)               // is the pixel going up AND right?
			{
				Ax+=Xincr;         // increment dep}ent variable
				Ay+=Yincr;         // increment indep}ent variable
				P+=dPru;           // increment decision (for up)
			}
			else                     // is the pixel just going up?
			{
				Ay+=Yincr;         // increment indep}ent variable
				P+=dPr;            // increment decision (for right)
			}
		}
	}

	return (false);

}

// ----------------------------------------------------------------- //
void cr_CreatePath(int c, int Ax, int Ay, int Bx, int By)
// ----------------------------------------------------------------- //

{
	//------------------------------------------------------------------------
	// INITIALIZE THE COMPONENTS OF THE ALGORITHM THAT ARE NOT AFFECTED BY THE
	// SLOPE OR DIRECTION OF THE LINE
	//------------------------------------------------------------------------
	int dX = abs(Bx-Ax);	// store the change in X and Y of the line EndPoints
	int dY = abs(By-Ay);

	int	Sx = Ax;
	int	Sy = Ay;
	//------------------------------------------------------------------------
	// DETERMINE "DIRECTIONS" TO INCREMENT X AND Y (REGARDLESS OF DECISION)
	//------------------------------------------------------------------------
	int             Xincr, Yincr;
	int             j;
	TBoolean        canMove;

	if (Ax > Bx) { Xincr=-1; }
	else { Xincr=1; }	// which direction in X?

	if (Ay > By) { Yincr=-1; }
	else { Yincr=1; }	// which direction in Y?

	creature[c].pathPtr=0;

	for (j=0;j<kMaxPath;j++)
	{
		creature[c].pathRow[j]	=	0;
		creature[c].pathCol[j]	=	0;
	}

	j=0;

	//------------------------------------------------------------------------
	// DETERMINE INDEP}ENT VARIABLE (ONE THAT ALWAYS INCREMENTS BY 1 (OR -1) )
	// AND INITIATE APPROPRIATE LINE DRAWING ROUTINE (BASED ON FIRST OCTANT
	// ALWAYS). THE X AND Y'S MAY BE FLIPPED IF Y IS THE INDEP}ENT VARIABLE.
	//------------------------------------------------------------------------

	if (dX >= dY)	// if X is the indep}ent variable
	{
		int dPr 	= dY<<1;           // amount to increment decision if right is chosen (always)
		int dPru 	= dPr - (dX<<1);   // amount to increment decision if up is chosen
		int P 		= dPr - dX;  // decision variable start value

		for (; dX>=0; dX--)            // process each point in the line one at a time (just use dX)
		{

			canMove=canWalkOn[map[Ax][Ay]];

			if (canSwimIn[map[Ax][Ay]])
				if (in_QuantityOf_s(c,kRaft)>0)
					canMove=true;

			if (canMove==false)
				return;

			creature[c].pathRow[j]=Ax;

			creature[c].pathCol[j]=Ay;

			creature[c].pathPtr=1;

			j++;

			if (j>=kMaxPath)
				return;

			if (P > 0)               // is the pixel going right AND up?
			{
				Ax+=Xincr;	       // increment indep}ent variable
				Ay+=Yincr;         // increment dep}ent variable
				P+=dPru;           // increment decision (for up)
			}
			else                     // is the pixel just going right?
			{
				Ax+=Xincr;         // increment indep}ent variable
				P+=dPr;            // increment decision (for right)
			}
		}
	}
	else              // if Y is the indep}ent variable
	{
		int dPr 	= dX<<1;           // amount to increment decision if right is chosen (always)
		int dPru 	= dPr - (dY<<1);   // amount to increment decision if up is chosen
		int P 		= dPr - dY;  // decision variable start value

		for (; dY>=0; dY--)            // process each point in the line one at a time (just use dY)
		{

			canMove=canWalkOn[map[Ax][Ay]];

			if (doorway[map[Ax][Ay]])
				canMove=true;

			if (canSwimIn[map[Ax][Ay]])
				if (in_QuantityOf_s(c,kRaft)>0)
					canMove=true;

			if (canMove==false)
				return;

			creature[c].pathRow[j]=Ax;

			creature[c].pathCol[j]=Ay;

			creature[c].pathPtr=1;

			j++;

			if (j>=kMaxPath)
				return;

			if (P > 0)               // is the pixel going up AND right?
			{
				Ax+=Xincr;         // increment dep}ent variable
				Ay+=Yincr;         // increment indep}ent variable
				P+=dPru;           // increment decision (for up)
			}
			else                     // is the pixel just going up?
			{
				Ay+=Yincr;         // increment indep}ent variable
				P+=dPr;            // increment decision (for right)
			}
		}
	}

	return;

}

// ----------------------------------------------------------------- //
TBoolean cr_ViewBlocked(int Ax, int Ay, int Bx, int By)
// ----------------------------------------------------------------- //

{
	//------------------------------------------------------------------------
	// INITIALIZE THE COMPONENTS OF THE ALGORITHM THAT ARE NOT AFFECTED BY THE
	// SLOPE OR DIRECTION OF THE LINE
	//------------------------------------------------------------------------
	int dX = abs(Bx-Ax);	// store the change in X and Y of the line EndPoints
	int dY = abs(By-Ay);

	int	Sx = Ax;
	int	Sy = Ay;
	//------------------------------------------------------------------------
	// DETERMINE "DIRECTIONS" TO INCREMENT X AND Y (REGARDLESS OF DECISION)
	//------------------------------------------------------------------------
	int Xincr, Yincr;

	if (Ax > Bx) { Xincr=-1; }
	else { Xincr=1; }	// which direction in X?

	if (Ay > By) { Yincr=-1; }
	else { Yincr=1; }	// which direction in Y?

	//------------------------------------------------------------------------
	// DETERMINE INDEP}ENT VARIABLE (ONE THAT ALWAYS INCREMENTS BY 1 (OR -1) )
	// AND INITIATE APPROPRIATE LINE DRAWING ROUTINE (BASED ON FIRST OCTANT
	// ALWAYS). THE X AND Y'S MAY BE FLIPPED IF Y IS THE INDEP}ENT VARIABLE.
	//------------------------------------------------------------------------
	if (dX >= dY)	// if X is the indep}ent variable
	{
		int dPr 	= dY<<1;           // amount to increment decision if right is chosen (always)
		int dPru 	= dPr - (dX<<1);   // amount to increment decision if up is chosen
		int P 		= dPr - dX;  // decision variable start value

		for (; dX>=0; dX--)            // process each point in the line one at a time (just use dX)
		{

			if ((canWalkOn[map[Ax][Ay]]==false) && (canSwimIn[map[Ax][Ay]]==false) && (obstacle[map[Ax][Ay]]))
				return (true);

			if (gmProp[containerID[containerMap[Ax][Ay]]])
				return (true);

			if (!((Ax==Bx) && (Ay==By)))
				if ((doorway[map[Ax][Ay]]) && ((Sx!=Ax) || (Sy!=Ay)))
					return (true);

			if (P > 0)               // is the pixel going right AND up?
			{
				Ax+=Xincr;	       // increment indep}ent variable
				Ay+=Yincr;         // increment dep}ent variable
				P+=dPru;           // increment decision (for up)
			}
			else                     // is the pixel just going right?
			{
				Ax+=Xincr;         // increment indep}ent variable
				P+=dPr;            // increment decision (for right)
			}
		}
	}
	else              // if Y is the indep}ent variable
	{
		int dPr 	= dX<<1;           // amount to increment decision if right is chosen (always)
		int dPru 	= dPr - (dY<<1);   // amount to increment decision if up is chosen
		int P 		= dPr - dY;  // decision variable start value

		for (; dY>=0; dY--)            // process each point in the line one at a time (just use dY)
		{

			if ((canWalkOn[map[Ax][Ay]]==false) && (canSwimIn[map[Ax][Ay]]==false) && (obstacle[map[Ax][Ay]]))
				return (true);

			if (gmProp[containerID[containerMap[Ax][Ay]]])
				return (true);

			if (!((Ax==Bx) && (Ay==By)))
				if ((doorway[map[Ax][Ay]]) && ((Sx!=Ax) || (Sy!=Ay)))
					return (true);

			if (P > 0)               // is the pixel going up AND right?
			{
				Ax+=Xincr;         // increment dep}ent variable
				Ay+=Yincr;         // increment indep}ent variable
				P+=dPru;           // increment decision (for up)
			}
			else                     // is the pixel just going up?
			{
				Ay+=Yincr;         // increment indep}ent variable
				P+=dPr;            // increment decision (for right)
			}
		}
	}

	return (false);

}

// ----------------------------------------------------------------- //
int cr_DistanceBetween(int c1, int c2)
// ----------------------------------------------------------------- //

{
	int						rowDist;
	int						colDist;

	rowDist=fabs(creature[c1].row-creature[c2].row);
	colDist=fabs(creature[c1].col-creature[c2].col);

	if (rowDist>colDist)
		return (rowDist);
	else
		return (colDist);

}

// ----------------------------------------------------------------- //
int cr_DistanceFrom(int c, int row, int col)
// ----------------------------------------------------------------- //

{
	int						rowDist;
	int						colDist;

	rowDist=fabs(row-creature[c].row);
	colDist=fabs(col-creature[c].col);

	if (rowDist>colDist)
		return (rowDist);
	else
		return (colDist);

}

// ----------------------------------------------------------------- //
int cr_RealDistanceFrom(int c, int row, int col)
// ----------------------------------------------------------------- //

{

	return (((row-creature[c].row) * (row-creature[c].row)) + ((col-creature[c].col) * (col-creature[c].col)));

}

// ----------------------------------------------------------------- //
void cr_setDieTime(int c, unsigned long theTime)
// ----------------------------------------------------------------- //

{

	if (creature[c].dieTime==0)
		creature[c].dieTime=theTime;

}

// ----------------------------------------------------------------- //
TBoolean cr_AdjacentSpotInDomain(int row, int col, int domain)
// ----------------------------------------------------------------- //

{

	if ((domain==kWater) || (domain==kAir))
	{
		if (canSwimIn[map[row-1][col-1]]) 		return (true);

		if (canSwimIn[map[row-1][col]]) 			return (true);

		if (canSwimIn[map[row-1][col+1]]) 		return (true);

		if (canSwimIn[map[row][col-1]]) 			return (true);

		if (canSwimIn[map[row][col+1]]) 			return (true);

		if (canSwimIn[map[row+1][col-1]]) 		return (true);

		if (canSwimIn[map[row+1][col]]) 			return (true);

		if (canSwimIn[map[row+1][col+1]]) 		return (true);
	}

	if ((domain==kLand) || (domain==kAir))
	{
		if (canWalkOn[map[row-1][col-1]]) 		return (true);

		if (canWalkOn[map[row-1][col]]) 			return (true);

		if (canWalkOn[map[row-1][col+1]]) 		return (true);

		if (canWalkOn[map[row][col-1]]) 			return (true);

		if (canWalkOn[map[row][col+1]]) 			return (true);

		if (canWalkOn[map[row+1][col-1]]) 		return (true);

		if (canWalkOn[map[row+1][col]]) 			return (true);

		if (canWalkOn[map[row+1][col+1]]) 		return (true);
	}

	return (false);

}

/*
// ----------------------------------------------------------------- //
void cr_GetSpotAwayFromHuman(int c, int *stepX, int *stepY)
// ----------------------------------------------------------------- //

{
  int                 row;
  int                 col;
  int                 sx[8];
  int                 sy[8];
  int                 sp=-1;
  int                 r;

  row=creature[c].row;
  col=creature[c].col;

  *stepX=0;
  *stepY=0;

	if (creatureMap[row-1][col-1]==0)
    if (cr_InDomain(c, (row-1), (col-1)))
      if (cr_AdjacentSpotHasHuman((row-1), (col-1))==false)
  	    {
  	      sp++;
  	      sx[sp]=-1;
  	      sy[sp]=-1;
          //*stepX=-1;
          //*stepY=-1;
          //return;
  	    }

	if (creatureMap[row-1][col]==0)
    if (cr_InDomain(c, (row-1), (col)))
      if (cr_AdjacentSpotHasHuman((row-1), (col))==false)
  	    {
  	      sp++;
  	      sx[sp]=-1;
  	      sy[sp]=0;
          //*stepX=-1;
          //*stepY=0;
          //return;
  	    }

	if (creatureMap[row-1][col+1]==0)
    if (cr_InDomain(c, (row-1), (col+1)))
      if (cr_AdjacentSpotHasHuman((row-1), (col+1))==false)
  	    {
  	      sp++;
  	      sx[sp]=-1;
  	      sy[sp]=1;
          //*stepX=-1;
          //*stepY=1;
          //return;
  	    }

	if (creatureMap[row][col-1]==0)
    if (cr_InDomain(c, (row), (col-1)))
      if (cr_AdjacentSpotHasHuman((row), (col-1))==false)
  	    {
  	      sp++;
  	      sx[sp]=0;
  	      sy[sp]=-1;
          //*stepX=0;
          //*stepY=-1;
          //return;
  	    }

	if (creatureMap[row][col+1]==0)
    if (cr_InDomain(c, (row), (col+1)))
      if (cr_AdjacentSpotHasHuman((row), (col+1))==false)
  	    {
  	      sp++;
  	      sx[sp]=0;
  	      sy[sp]=1;
          //*stepX=0;
          //*stepY=1;
          //return;
 	    }

	if (creatureMap[row+1][col-1]==0)
    if (cr_InDomain(c, (row+1), (col-1)))
      if (cr_AdjacentSpotHasHuman((row+1), (col-1))==false)
  	    {
  	      sp++;
  	      sx[sp]=1;
  	      sy[sp]=-1;
          //*stepX=1;
          //*stepY=-1;
          //return;
  	    }

	if (creatureMap[row+1][col]==0)
    if (cr_InDomain(c, (row+1), (col)))
      if (cr_AdjacentSpotHasHuman((row+1), (col))==false)
  	    {
  	      sp++;
  	      sx[sp]=1;
  	      sy[sp]=0;
          //*stepX=1;
          //*stepY=0;
          //return;
  	    }

	if (creatureMap[row+1][col+1]==0)
    if (cr_InDomain(c, (row+1), (col+1)))
      if (cr_AdjacentSpotHasHuman((row+1), (col+1))==false)
  	    {
  	      sp++;
  	      sx[sp]=1;
  	      sy[sp]=1;
          //*stepX=1;
          //*stepY=1;
          //return;
  	    }

  if (sp>=0)
    {
      r=tb_Rnd(0,sp);
      *stepX=sx[r];
      *stepY=sx[r];
    }

}
*/
// ----------------------------------------------------------------- //
TBoolean cr_AdjacentSpotHasHuman(int row, int col)
// ----------------------------------------------------------------- //

{
//02/05/05 - NPCs will move if there's a stealthed or hidden human near

	if (creatureMap[row-1][col-1]!=0)
		if (creature[creatureMap[row-1][col-1]].id==kHuman && creature[creatureMap[row-1][col-1]].hiding==false && player[creature[creatureMap[row-1][col-1]].playerIndex].stealth==false)    return (true);

	if (creatureMap[row-1][col]!=0)
		if (creature[creatureMap[row-1][col]].id==kHuman && creature[creatureMap[row-1][col]].hiding==false && player[creature[creatureMap[row-1][col]].playerIndex].stealth==false)        return (true);

	if (creatureMap[row-1][col+1]!=0)
		if (creature[creatureMap[row-1][col+1]].id==kHuman && creature[creatureMap[row-1][col+1]].hiding==false && player[creature[creatureMap[row-1][col+1]].playerIndex].stealth==false)    return (true);

	if (creatureMap[row][col-1]!=0)
		if (creature[creatureMap[row][col-1]].id==kHuman && creature[creatureMap[row][col-1]].hiding==false && player[creature[creatureMap[row][col-1]].playerIndex].stealth==false)        return (true);

	if (creatureMap[row][col+1]!=0)
		if (creature[creatureMap[row][col+1]].id==kHuman && creature[creatureMap[row][col+1]].hiding==false && player[creature[creatureMap[row][col+1]].playerIndex].stealth==false)        return (true);

	if (creatureMap[row+1][col-1]!=0)
		if (creature[creatureMap[row+1][col-1]].id==kHuman && creature[creatureMap[row+1][col-1]].hiding==false && player[creature[creatureMap[row+1][col-1]].playerIndex].stealth==false)    return (true);

	if (creatureMap[row+1][col]!=0)
		if (creature[creatureMap[row+1][col]].id==kHuman && creature[creatureMap[row+1][col]].hiding==false && player[creature[creatureMap[row+1][col]].playerIndex].stealth==false)        return (true);

	if (creatureMap[row+1][col+1]!=0)
		if (creature[creatureMap[row+1][col+1]].id==kHuman && creature[creatureMap[row+1][col+1]].hiding==false && player[creature[creatureMap[row+1][col+1]].playerIndex].stealth==false)    return (true);

	return (false);

	/*
		if (creatureMap[row-1][col-1]!=0)
		  if ((creature[creatureMap[row-1][col-1]].id==kHuman) || (creature[creatureMap[row-1][col-1]].master!=0))    return (true);

		if (creatureMap[row-1][col]!=0)
		  if ((creature[creatureMap[row-1][col]].id==kHuman) || (creature[creatureMap[row-1][col]].master!=0))        return (true);

		if (creatureMap[row-1][col+1]!=0)
		  if ((creature[creatureMap[row-1][col+1]].id==kHuman) || (creature[creatureMap[row-1][col+1]].master!=0))    return (true);

		if (creatureMap[row][col-1]!=0)
		  if ((creature[creatureMap[row][col-1]].id==kHuman) || (creature[creatureMap[row][col-1]].master!=0))        return (true);

		if (creatureMap[row][col+1]!=0)
		  if ((creature[creatureMap[row][col+1]].id==kHuman) || (creature[creatureMap[row][col+1]].master!=0))        return (true);

		if (creatureMap[row+1][col-1]!=0)
		  if ((creature[creatureMap[row+1][col-1]].id==kHuman) || (creature[creatureMap[row+1][col-1]].master!=0))    return (true);

		if (creatureMap[row+1][col]!=0)
		  if ((creature[creatureMap[row+1][col]].id==kHuman) || (creature[creatureMap[row+1][col]].master!=0))        return (true);

		if (creatureMap[row+1][col+1]!=0)
		  if ((creature[creatureMap[row+1][col+1]].id==kHuman) || (creature[creatureMap[row+1][col+1]].master!=0))    return (true);

		return (false);
	*/
}

// ----------------------------------------------------------------- //
TBoolean cr_AdjacentSpotHasNPCHuman(int row, int col)
// ----------------------------------------------------------------- //

{
	int theId=0;

	if (creatureMap[row-1][col-1]!=0)
		if (creature[creatureMap[row-1][col-1]].npcIndex!=0) 	return (true);
		
	if (creatureMap[row-1][col]!=0)
		if (creature[creatureMap[row-1][col]].npcIndex!=0)		return (true);

	if (creatureMap[row-1][col+1]!=0)
		if (creature[creatureMap[row-1][col+1]].npcIndex!=0)	return (true);

	if (creatureMap[row][col-1]!=0)
		if (creature[creatureMap[row][col-1]].npcIndex!=0)		return (true);

	if (creatureMap[row][col+1]!=0)
		if (creature[creatureMap[row-1][col+1]].npcIndex!=0)	return (true);

	if (creatureMap[row+1][col-1]!=0)
		if (creature[creatureMap[row+1][col-1]].npcIndex!=0)	return (true);

	if (creatureMap[row+1][col]!=0)
		if (creature[creatureMap[row+1][col]].npcIndex!=0)		return (true);

	if (creatureMap[row+1][col+1]!=0)
		if (creature[creatureMap[row+1][col+1]].npcIndex!=0)	return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean cr_LowMorale(int c)
// ----------------------------------------------------------------- //

{
	float         lowHealth;


	lowHealth=tb_Round(((float)creatureInfo[creature[c].id].maxHitPoints/2.0));
	//if (creature[c].hitPoints>=lowHealth)

	if (creature[c].hitPoints>=creatureInfo[creature[c].id].maxHitPoints)
		creature[c].cornered=false;

	lowHealth=tb_Round(((float)creatureInfo[creature[c].id].maxHitPoints/3.0));

	if (creature[c].cornered)
		return (false);

	if (creature[c].master==0 && creature[c].wounded)
		return (true);

	if (creature[c].npcIndex !=0 && npc[creature[c].npcIndex].type==kNPCGuard) //guards never run
		return(false);

	if (creatureInfo[creature[c].id].mobile==false)
		return (false);

	if (creatureInfo[creature[c].id].maxMagicPoints>0)
		return (false);

	if ((creature[c].id==kRedDragon) || (creature[c].id==kIceDragon) || (creature[c].id==kForestDragon) 
			|| (creature[c].id==kBlackDragon) || (creature[c].id==kPinkDragon) || (creature[c].id==kSandDragon))
		return (false);

	if ((creature[c].id==kRedDragonAncient) || (creature[c].id==kIceDragonAncient) || (creature[c].id==kForestDragonAncient) || (creature[c].id==kBlackDragonAncient))
		return (false);

	if ((creature[c].id==kRedDraco) || (creature[c].id==kIceDraco) || (creature[c].id==kForestDraco) || (creature[c].id==kBlackDraco)  || (creature[c].id==kSandDraco))
		return (false);

	if (creatureInfo[creature[c].id].undead==true) //018 Undeads don't run
		return(false);
	
	if ((creature[c].id==kGolem) || (creature[c].id==kIceGolem) || (creature[c].id==kMagmaGolem) || (creature[c].id==kSandGolem))
		return (false);

	if (creature[c].hitPoints<lowHealth)
	{
		// merge 7/23
		return (true);
	} // merge 7/23

	return (false);

}

// ----------------------------------------------------------------- //
void cr_ClearAttackTarget(int i)
// ----------------------------------------------------------------- //

{
	int               j;

	if (i==0)
		return;

	for (j=0;j<kMaxCreatures;j++)
		if (creature[j].id!=0)
			if (creature[j].attackTarget==i)
			{
				creature[j].attackTarget			=	0;
				creature[j].possibleTarget		=	0;
				creature[j].nextStrikeTime		=	0;
				creature[j].numberOfStrikes	  =	0;
				creature[j].trackRow					= 0;
				creature[j].trackCol					= 0;
				creature[j].tracking					= false;
			}

}


// ----------------------------------------------------------------- //
void cr_GetTrackingMove(int i, int *row, int *col)
// ----------------------------------------------------------------- //

{
	int         rowDir=0;
	int         colDir=0;

	rowDir=creature[i].nextTrackRow-creature[i].row;
	colDir=creature[i].nextTrackCol-creature[i].col;

	*row=rowDir;
	*col=colDir;

}

// ----------------------------------------------------------------- //
int cr_AttackRange(int c)
// ----------------------------------------------------------------- //

{

	if (creature[c].playerIndex!=0)
	{
		if (player[creature[c].playerIndex].arm[kRightHand]==0)
			return (1);
		else
			return (itemList[creature[c].inventory[player[creature[c].playerIndex].arm[kRightHand]].itemID].weaponRange);
	}
	else
	{
		return (creatureInfo[creature[c].id].attackRange);
	}

}

// ----------------------------------------------------------------- //
TBoolean cr_ObstacleBetween2(int x0, int y0, int x1, int y1, int domain, int *d, int c1, int c2, TBoolean fire)
// ----------------------------------------------------------------- //

{
	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;
	int	Sx = x0;
	int	Sy = y0;

	if (dy < 0) { dy = -dy;  stepy = -1; }
	else { stepy = 1; }

	if (dx < 0) { dx = -dx;  stepx = -1; }
	else { stepx = 1; }

	dy <<= 1;

	dx <<= 1;

	*d=1;

	if (containerMap[x0][y0]!=0)
		if (gmProp[containerID[containerMap[x0][y0]]])
			return (true);

	if ((canWalkOn[map[x0][y0]]==false) && (canSwimIn[map[x0][y0]]==false))
		return (true);

	if ((canWalkOn[map[x0][y0]]) && (domain==kWater))
		return (true);

	if ((canSwimIn[map[x0][y0]]) && (domain==kLand))
		return (true);

	if (!(((x0==Sx) && (y0==Sy)) || ((x0==x1) && (y0==y1))))
		if (creatureMap[x0][y0]!=0)
			return (true);

	if (creatureMap[x0][y0]!=0)
	{
		if (c2!=0)
		{
			if (!((creatureMap[x0][y0]==c1) || (creatureMap[x0][y0]==c2)))
				return (true);
		}
		else
			if (creatureMap[x0][y0]!=c1)
				return (true);
	}

	if (fxMap[x0][y0]!=0)
		if (serverFX[fxMap[x0][y0]]==kFireField)
			if (!fire)
				if (creatureMap[x0][y0]==0)
					return (true);


	if (dx > dy)
	{
		int fraction = dy - (dx >> 1);
		fi_StartDebugTimer();

		while (x0 != x1)
		{
			fi_CheckDebugTimer("cr_ObstacleBetween2 A");

			if (fraction >= 0)
			{
				y0 += stepy;
				fraction -= dx;
			}

			x0 += stepx;

			fraction += dy;

			if (containerMap[x0][y0]!=0)
				if (gmProp[containerID[containerMap[x0][y0]]])
					return (true);

			if ((canWalkOn[map[x0][y0]]==false) && (canSwimIn[map[x0][y0]]==false))
				return (true);

			if ((canWalkOn[map[x0][y0]]) && (domain==kWater))
				return (true);

			if ((canSwimIn[map[x0][y0]]) && (domain==kLand))
				return (true);

			if (creatureMap[x0][y0]!=0)
			{
				if (c2!=0)
				{
					if (!((creatureMap[x0][y0]==c1) || (creatureMap[x0][y0]==c2)))
						return (true);
				}
				else
					if (creatureMap[x0][y0]!=c1)
						return (true);
			}

			if (fxMap[x0][y0]!=0)
				if (serverFX[fxMap[x0][y0]]==kFireField)
					if (!fire)
						if (creatureMap[x0][y0]==0)
							return (true);

			(*d)++;
		}
	}
	else
	{
		int fraction = dx - (dy >> 1);
		fi_StartDebugTimer();

		while (y0 != y1)
		{
			fi_CheckDebugTimer("cr_ObstacleBetween2 B");

			if (fraction >= 0)
			{
				x0 += stepx;
				fraction -= dy;
			}

			y0 += stepy;

			fraction += dx;

			if (containerMap[x0][y0]!=0)
				if (gmProp[containerID[containerMap[x0][y0]]])
					return (true);

			if ((canWalkOn[map[x0][y0]]==false) && (canSwimIn[map[x0][y0]]==false))
				return (true);

			if ((canWalkOn[map[x0][y0]]) && (domain==kWater))
				return (true);

			if ((canSwimIn[map[x0][y0]]) && (domain==kLand))
				return (true);

			if (creatureMap[x0][y0]!=0)
			{
				if (c2!=0)
				{
					if (!((creatureMap[x0][y0]==c1) || (creatureMap[x0][y0]==c2)))
						return (true);
				}
				else
					if (creatureMap[x0][y0]!=c1)
						return (true);
			}

			if (fxMap[x0][y0]!=0)
				if (serverFX[fxMap[x0][y0]]==kFireField)
					if (!fire)
						if (creatureMap[x0][y0]==0)
							return (true);

			(*d)++;
		}
	}

	return (false);

}

// ----------------------------------------------------------------- //
void cr_CreatePath2(int x0, int y0, int x1, int y1)
// ----------------------------------------------------------------- //

{
	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;
	int count=0;

	if (dy < 0) { dy = -dy;  stepy = -1; }
	else { stepy = 1; }

	if (dx < 0) { dx = -dx;  stepx = -1; }
	else { stepx = 1; }

	dy <<= 1;                                                  // dy is now 2*dy

	dx <<= 1;                                                  // dx is now 2*dx

	// pathRow[pathPtr]=x0;
	// pathCol[pathPtr]=y0;
	// pathPtr++;
	count++;

	if (dx > dy)
	{
		int fraction = dy - (dx >> 1);                         // same as 2*dy - dx
		fi_StartDebugTimer();

		while (x0 != x1)
		{
			fi_CheckDebugTimer("cr_CreatePath2 A");

			if (fraction >= 0)
			{
				y0 += stepy;
				fraction -= dx;                                // same as fraction -= 2*dx
			}

			x0 += stepx;

			fraction += dy;                                    // same as fraction -= 2*dy

			pathRow=x0;
			pathCol=y0;
			count++;

			if (count>=2)  return;

			//pathRow[pathPtr]=x0;
			// pathCol[pathPtr]=y0;
			//pathPtr++;
			//if (pathPtr>=2)
			return;
		}
	}
	else
	{
		int fraction = dx - (dy >> 1);
		fi_StartDebugTimer();

		while (y0 != y1)
		{
			fi_CheckDebugTimer("cr_CreatePath2 B");

			if (fraction >= 0)
			{
				x0 += stepx;
				fraction -= dy;
			}

			y0 += stepy;

			fraction += dx;

			pathRow=x0;
			pathCol=y0;
			count++;

			if (count>=2)  return;

			//pathRow[pathPtr]=x0;
			// pathCol[pathPtr]=y0;
			//pathPtr++;
			//if (pathPtr>=2)
			return;
		}
	}
}


// ----------------------------------------------------------------- //
TBoolean cr_BuildAttackPath(int s, int d, int *stepX, int *stepY)
// ----------------------------------------------------------------- //

{
	//int             j;
	int             row;
	int             col;
	int             row2;
	int             col2;
	int             d1;
	int             d2;
	//int             d3;
	int             minDistance;
	int             bestX1;
	int             bestY1;
	int             bestX2;
	int             bestY2;
// int             bestX3;
// int             bestY3;
	int             firstRow;
	int             lastRow;
	int             firstCol;
	int             lastCol;
	int             startRow;
	int             startCol;
	int             endRow;
	int             endCol;
	int             firstRow2;
	int             lastRow2;
	int             firstCol2;
	int             lastCol2;
	int             range;
	int             range2;
	int							minDist;
	int							r;
	int							c;

	*stepX=0;
	*stepY=0;

#ifdef _REFRESHTEST
	fi_WriteToErrorLog("buildpath Entry");
#endif

	if (creature[d].hiding)
		if (creatureInfo[creature[s].id].maxMagicPoints==0)
		{
			if ((creature[s].playerIndex==0) && (creature[d].playerIndex!=0))
				tx_CreatureTalkText(s, d, kJustHidTalk);

			return (false);
		}
		else
			return (true);

	if (creature[d].playerIndex!=0 && pl_AuthorizedTo(d, kRegularPlayer) && player[creature[d].playerIndex].stealth)
	{
		if ((creature[s].playerIndex==0))
			tx_CreatureTalkText(s, d, kJustHidTalk);

		return (false);
	}


	if (creatureInfo[creature[s].id].mobile==false)
		return (false);

	startRow=creature[s].row;

	startCol=creature[s].col;

	endRow=creature[d].row;

	endCol=creature[d].col;

	minDist=1000;

	if (creatureInfo[creature[s].id].domain==kLand)
	{
		for (r=(-creatureInfo[creature[s].id].attackRange);r<=creatureInfo[creature[s].id].attackRange;r++)
			for (c=(-creatureInfo[creature[s].id].attackRange);c<=creatureInfo[creature[s].id].attackRange;c++)
				if (canWalkOn[map[r+creature[d].row][c+creature[d].col]])
					if (creatureMap[r+creature[d].row][c+creature[d].col]==0)
						if (cr_DistanceFrom(s,r+creature[d].row,c+creature[d].col)<minDist)
							if (cr_ObstacleBetween2(r+creature[d].row, c+creature[d].col, endRow, endCol, creatureInfo[creature[s].id].domain, &d1, s, d, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false) // put in carbon 11/21
							{
								minDist=cr_DistanceFrom(s,r+creature[d].row,c+creature[d].col);
								endRow=r+creature[d].row;
								endCol=c+creature[d].col;
							}
	}
	else if (creatureInfo[creature[s].id].domain==kWater)
	{
		for (r=(-creatureInfo[creature[s].id].attackRange);r<=creatureInfo[creature[s].id].attackRange;r++)
			for (c=(-creatureInfo[creature[s].id].attackRange);c<=creatureInfo[creature[s].id].attackRange;c++)
				if (canSwimIn[map[r+creature[d].row][c+creature[d].col]])
					if (creatureMap[r+creature[d].row][c+creature[d].col]==0)
						if (cr_DistanceFrom(s,r+creature[d].row,c+creature[d].col)<minDist)
							if (cr_ObstacleBetween2(r+creature[d].row, c+creature[d].col, endRow, endCol, creatureInfo[creature[s].id].domain, &d1, s, d, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false) // put in carbon 11/21
							{
								minDist=cr_DistanceFrom(s,r+creature[d].row,c+creature[d].col);
								endRow=r+creature[d].row;
								endCol=c+creature[d].col;
							}
	}

	if (cr_ObstacleBetween2(startRow, startCol, endRow, endCol, creatureInfo[creature[s].id].domain, &d1, s, d, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
	{
		cr_CreatePath2(startRow, startCol, endRow, endCol);
		*stepX=pathRow;
		*stepY=pathCol;
		return (true);
	}

	bestX1=0;

	bestY1=0;
	bestX2=0;
	bestY2=0;
	//bestX3=0;
	//bestY3=0;
	minDistance=1000;

	if (creatureInfo[creature[s].id].attackSound==kDragonAttack)
		range=15;
	else if (creatureInfo[creature[s].id].maxIntelligence>100)
		range=8;
	else if (creatureInfo[creature[s].id].maxIntelligence>50)
		range=7;
	else if (creatureInfo[creature[s].id].maxIntelligence>20)
		range=6;
	else
		range=5;

	firstRow2=endRow-range;

	lastRow2=endRow+range;

	firstCol2=endCol-range;

	lastCol2=endCol+range;

	if (firstRow2<0)            firstRow2=0;

	if (lastRow2>=kMapWidth)    lastRow2=kMapWidth-1;

	if (firstCol2<0)            firstCol2=0;

	if (lastCol2>=kMapHeight)   lastCol2=kMapHeight-1;

	for (row=firstRow2;row<lastRow2;row++)
		for (col=firstCol2;col<lastCol2;col++)
		{
			if (cr_ObstacleBetween2(startRow, startCol, row, col, creatureInfo[creature[s].id].domain, &d1, s, d, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
				if (cr_ObstacleBetween2(row, col, endRow, endCol, creatureInfo[creature[s].id].domain, &d2, s, d, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
				{
					if ((d1+d2)<minDistance)
					{
						bestX1=startRow;
						bestY1=startCol;
						bestX2=row;
						bestY2=col;
						//bestX3=endRow;
						//bestY3=endCol;
						minDistance=d1+d2;
					}
				}
		}

	if (minDistance<1000)
	{
		cr_CreatePath2(bestX1, bestY1, bestX2, bestY2);

		*stepX=pathRow;
		*stepY=pathCol;
		return (true);

	}
	else
	{

		if (creatureInfo[creature[s].id].attackSound==kDragonAttack)
			range2=6;
		else if (creatureInfo[creature[s].id].maxIntelligence>100)
			range2=5;
		else if (creatureInfo[creature[s].id].maxIntelligence>50)
			range2=4;
		else if (creatureInfo[creature[s].id].maxIntelligence>20)
			range2=3;
		else
			range2=2;

		firstRow=endRow-range2;

		lastRow=endRow+range2;

		firstCol=endCol-range2;

		lastCol=endCol+range2;

		if (firstRow<0)           firstRow=0;

		if (lastRow>=kMapWidth)   lastRow=kMapWidth-1;

		if (firstCol<0)           firstCol=0;

		if (lastCol>=kMapHeight)  lastCol=kMapHeight-1;

		for (row2=firstRow;row2<lastRow;row2++)
			for (col2=firstCol;col2<lastCol;col2++)
				if (cr_ObstacleBetween2(endRow, endCol, row2, col2, creatureInfo[creature[s].id].domain, &d1, s, d, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
				{

					bestX1=0;
					bestY1=0;
					bestX2=0;
					bestY2=0;
					//bestX3=0;
					//bestY3=0;
					minDistance=1000;

					firstRow2=endRow-range;
					lastRow2=endRow+range;
					firstCol2=endCol-range;
					lastCol2=endCol+range;

					if (firstRow2<0)            firstRow2=0;

					if (lastRow2>=kMapWidth)    lastRow2=kMapWidth-1;

					if (firstCol2<0)            firstCol2=0;

					if (lastCol2>=kMapHeight)   lastCol2=kMapHeight-1;

					for (row=firstRow2;row<lastRow2;row++)
						for (col=firstCol2;col<lastCol2;col++)
							if (cr_ObstacleBetween2(startRow, startCol, row, col, creatureInfo[creature[s].id].domain, &d1, s, d, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
								if (cr_ObstacleBetween2(row, col, row2, col2, creatureInfo[creature[s].id].domain, &d2, s, d, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
								{
									if ((d1+d2)<minDistance)
									{
										bestX1=startRow;
										bestY1=startCol;
										bestX2=row;
										bestY2=col;
										//bestX3=row2;
										//bestY3=col2;
										minDistance=d1+d2;
									}
								}

					if (minDistance<1000)
					{
						cr_CreatePath2(bestX1, bestY1, bestX2, bestY2);

						*stepX=pathRow;
						*stepY=pathCol;
						return (true);

					}

				}

	}

	*stepX=0;

	*stepY=0;
	return (false);

}

// ----------------------------------------------------------------- //
TBoolean cr_BuildPathTo(int s, int drow, int dcol, int *stepX, int *stepY)
// ----------------------------------------------------------------- //

{
	//int             j;
	int             row;
	int             col;
	int             row2;
	int             col2;
	int             d1;
	int             d2;
	//int             d3;
	int             minDistance;
	int             bestX1;
	int             bestY1;
	int             bestX2;
	int             bestY2;
// int             bestX3;
// int             bestY3;
	int             firstRow;
	int             lastRow;
	int             firstCol;
	int             lastCol;
	int             startRow;
	int             startCol;
	int             endRow;
	int             endCol;
	int             firstRow2;
	int             lastRow2;
	int             firstCol2;
	int             lastCol2;


	*stepX=0;
	*stepY=0;

	startRow=creature[s].row;
	startCol=creature[s].col;

	endRow=drow;
	endCol=dcol;

	if (cr_ObstacleBetween2(startRow, startCol, endRow, endCol, creatureInfo[creature[s].id].domain, &d1, s, 0, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
	{
		cr_CreatePath2(startRow, startCol, endRow, endCol);
		*stepX=pathRow;
		*stepY=pathCol;
		return (true);
	}

	bestX1=0;

	bestY1=0;
	bestX2=0;
	bestY2=0;
	//bestX3=0;
	//bestY3=0;
	minDistance=1000;

	firstRow2=endRow-10;
	lastRow2=endRow+10;
	firstCol2=endCol-10;
	lastCol2=endCol+10;

	if (firstRow2<0)            firstRow2=0;

	if (lastRow2>=kMapWidth)    lastRow2=kMapWidth-1;

	if (firstCol2<0)            firstCol2=0;

	if (lastCol2>=kMapHeight)   lastCol2=kMapHeight-1;

	for (row=firstRow2;row<lastRow2;row++)
		for (col=firstCol2;col<lastCol2;col++)
		{
			if (cr_ObstacleBetween2(startRow, startCol, row, col, creatureInfo[creature[s].id].domain, &d1, s, 0, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
				if (cr_ObstacleBetween2(row, col, endRow, endCol, creatureInfo[creature[s].id].domain, &d2, s, 0, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
				{
					if ((d1+d2)<minDistance)
					{
						bestX1=startRow;
						bestY1=startCol;
						bestX2=row;
						bestY2=col;
						//bestX3=endRow;
						//bestY3=endCol;
						minDistance=d1+d2;
					}
				}
		}

	if (minDistance<1000)
	{
		cr_CreatePath2(bestX1, bestY1, bestX2, bestY2);
		*stepX=pathRow;
		*stepY=pathCol;
		return (true);
	}
	else
	{
		firstRow=endRow-3;
		lastRow=endRow+3;
		firstCol=endCol-3;
		lastCol=endCol+3;

		if (firstRow<0)           firstRow=0;

		if (lastRow>=kMapWidth)   lastRow=kMapWidth-1;

		if (firstCol<0)           firstCol=0;

		if (lastCol>=kMapHeight)  lastCol=kMapHeight-1;

		for (row2=firstRow;row2<lastRow;row2++)
			for (col2=firstCol;col2<lastCol;col2++)
				if (cr_ObstacleBetween2(endRow, endCol, row2, col2, creatureInfo[creature[s].id].domain, &d1, s, 0, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
				{

					bestX1=0;
					bestY1=0;
					bestX2=0;
					bestY2=0;
					//bestX3=0;
					//bestY3=0;
					minDistance=1000;

					firstRow2=endRow-10;
					lastRow2=endRow+10;
					firstCol2=endCol-10;
					lastCol2=endCol+10;

					if (firstRow2<0)            firstRow2=0;

					if (lastRow2>=kMapWidth)    lastRow2=kMapWidth-1;

					if (firstCol2<0)            firstCol2=0;

					if (lastCol2>=kMapHeight)   lastCol2=kMapHeight-1;

					for (row=firstRow2;row<lastRow2;row++)
						for (col=firstCol2;col<lastCol2;col++)
							if (cr_ObstacleBetween2(startRow, startCol, row, col, creatureInfo[creature[s].id].domain, &d1, s, 0, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
								if (cr_ObstacleBetween2(row, col, row2, col2, creatureInfo[creature[s].id].domain, &d2, s, 0, (cr_ImmuneToFF(creature[s].id) || creatureInfo[creature[s].id].fire))==false)
								{
									if ((d1+d2)<minDistance)
									{
										bestX1=startRow;
										bestY1=startCol;
										bestX2=row;
										bestY2=col;
										//bestX3=row2;
										//bestY3=col2;
										minDistance=d1+d2;
									}
								}

					if (minDistance<1000)
					{
						cr_CreatePath2(bestX1, bestY1, bestX2, bestY2);
						*stepX=pathRow;
						*stepY=pathCol;
						return (true);
					}

				}

	}

	*stepX=0;

	*stepY=0;
	return (false);

}

// ----------------------------------------------------------------- //
TBoolean cr_RustDamage(int c, int item) // mod 8/9 new function
// ----------------------------------------------------------------- //

{

	if (creature[c].id!=kRustBeast)
		return (false);

	if (itemList[item].metal)
		return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean cr_Attack(int c1, int c2)
// ----------------------------------------------------------------- //

{
	int										dist;
	unsigned long					currentTime;
	float									damage;
	float									condition;
	float									anatomy;
	float									proficiency;
	float									strength;
	float									dexterity;
	float									total;
	int										weaponRange;
	int										rowDelta;
	int										colDelta;
	int										opport;
	//int										spell;
	float									resist;
	//float									Mresist;
	float									weaponModifier;
	//int										cpt;
	int										useSkill;
	int										i;
	int                   slot;
	float									attackerResist;
	float									attackerDamage;
	//TBoolean              retailiate;
	TBoolean              attackerMetal;
	TBoolean              targetMetal;
	TBoolean              magicSuccess;
	int										rowDist;
	int										colDist;
	//TBoolean              targetInTown;
	//TBoolean              attackerInTown;
	int                   loyaltyDamage;
	int										master;	// merge 7/17
	int										j;	// merge 7/17
	int                   itemDamage;
	int					  defyChance, s;
	TBoolean			  macroing, found;
	int					  totalExp, expGained;
	//char				  gainStr[kStringLength], gainStr2[kStringLength];
	float				  tmpExp, tmpExp2;
	int					  backstab=0, crit;
	TBoolean			  critical=false, missed=false;
	int 				anatLevel;  //Dave
	int 				meleeLevel;  //Dave
	int 				archLevel;  //Dave
	
	
		
	if (creature[c2].master!=0 && player[creature[c2].master].category!=5 && player[creature[c1].playerIndex].category!=5 && player[creature[c2].master].category!=8 && player[creature[c1].playerIndex].category!=8) // can't attack pet, unless its master is /evil or we ourselves are /evil
		if (creature[c1].playerIndex!=0)
			//if (!((player[creature[c1].playerIndex].category==1) || (player[creature[c1].playerIndex].category==5))) // merge 7/28
			if (pl_AuthorizedTo(c1,kAttackPets)) // put in carbon 11/27
				if (cr_BothInPVPZone(c1,c2)==false)
				{
					creature[c1].attackTarget			=	0;
					creature[c1].possibleTarget		=	0;
					creature[c1].nextStrikeTime		=	0;
					creature[c1].numberOfStrikes	=	0;
					creature[c1].trackRow					= 0;
					creature[c1].trackCol					= 0;
					creature[c1].tracking					= false;
					return (false);
				}

	if (creature[c1].playerIndex!=0 && player[creature[c1].playerIndex].category!=5 && player[creature[c1].playerIndex].category!=8)  // can't attack non thief human unless we are /evil
		if (creature[c2].playerIndex!=0)
			if (pl_IsThief(c2)==false)
				//if (!((player[c1].category==1) || (player[c1].category==5))) // merge 7/28
				if (pl_AuthorizedTo(c1,kAttackPlayers)) // put in carbon 11/27
					if (cr_BothInPVPZone(c1,c2)==false)
					{
						creature[c1].attackTarget			=	0;
						creature[c1].nextStrikeTime		=	0;
						creature[c1].numberOfStrikes	=	0;
						creature[c1].trackRow					= 0;
						creature[c1].trackCol					= 0;
						creature[c1].tracking					= false;
						return (false);
					}

	if (creature[c1].master!=0 && player[creature[c1].master].category!=5 && player[creature[c1].master].category!=8)  // pet can't attack non thief human unless their master is /evil
		if (creature[c2].playerIndex!=0)
			if (pl_IsThief(c2)==false)
				//if (!((player[creature[c1].master].category==1) || (player[creature[c1].master].category==5))) // merge 7/28
				if (pl_AuthorizedTo(creature[c1].master,kPetCanAttackPlayers)) // put in carbon 11/27
					if (cr_BothInPVPZone(c1,c2)==false)
					{
						creature[c1].attackTarget			=	0;
						creature[c1].nextStrikeTime		=	0;
						creature[c1].numberOfStrikes	=	0;
						creature[c1].trackRow					= 0;
						creature[c1].trackCol					= 0;
						creature[c1].tracking					= false;
						return (false);
					}

	if (creature[c1].master!=0)  // pet can't attack pet, unless their master is /evil or we are /evil 11/12 // gs put in carbon
		if (creature[c2].master!=0)
			if (player[creature[c1].master].category!=5 && player[creature[c1].master].category!=8)
				if (player[creature[c2].master].category!=5  && player[creature[c2].master].category!=8)
					if (cr_BothInPVPZone(c1,c2)==false)
					{
						creature[c1].attackTarget			=	0;
						creature[c1].nextStrikeTime		=	0;
						creature[c1].numberOfStrikes	=	0;
						creature[c1].trackRow					= 0;
						creature[c1].trackCol					= 0;
						creature[c1].tracking					= false;
						return (false);
					}

	if (creature[c2].id==0)	// creature target already dead
	{
		creature[c1].attackTarget			=	0;
		creature[c1].nextStrikeTime		=	0;
		creature[c1].numberOfStrikes	=	0;
		creature[c1].trackRow					= 0;
		creature[c1].trackCol					= 0;
		creature[c1].tracking					= false;
		return (false);
	}

	if (creature[c2].playerIndex!=0) // player target already dead
		if (player[creature[c2].playerIndex].dead)
		{
			if (creature[c1].playerIndex==0)
				if (creature[c1].master==0) // carbon
					if (creature[c1].level<creatureInfo[creature[c1].id].creatureLevel * 2)
						creature[c1].level++;

			creature[c1].attackTarget			=	0;

			creature[c1].nextStrikeTime		=	0;

			creature[c1].numberOfStrikes	=	0;

			creature[c1].trackRow					= 0;

			creature[c1].trackCol					= 0;

			creature[c1].tracking					= false;

			return (false);
		}

	if (creature[c1].playerIndex!=0) // ghost can't attack
		if (player[creature[c1].playerIndex].dead)
		{
			creature[c1].attackTarget			=	0;
			creature[c1].nextStrikeTime		=	0;
			creature[c1].numberOfStrikes	=	0;
			creature[c1].trackRow					= 0;
			creature[c1].trackCol					= 0;
			creature[c1].tracking					= false;
			return (false);
		}

	if (creature[c2].playerIndex!=0)
		if ((creatureInfo[creature[c1].id].domain==kWater))
			if (creature[c2].attackTarget!=c1)
				if (in_ItemEquipedServer(creature[c2].playerIndex,kNeptunesRing))
				{
					creature[c1].attackTarget			=	0;
					creature[c1].nextStrikeTime		=	0;
					creature[c1].numberOfStrikes	=	0;
					creature[c1].trackRow					= 0;
					creature[c1].trackCol					= 0;
					creature[c1].tracking					= false;
					return (false);
				}

	if (creature[c2].playerIndex!=0)
		if ((creatureInfo[creature[c1].id].fire))
			if (creature[c2].attackTarget!=c1)
				if (in_ItemEquipedServer(creature[c2].playerIndex,kHadesRing))
				{
					creature[c1].attackTarget			=	0;
					creature[c1].nextStrikeTime		=	0;
					creature[c1].numberOfStrikes	=	0;
					creature[c1].trackRow					= 0;
					creature[c1].trackCol					= 0;
					creature[c1].tracking					= false;
					return (false);
				}

	if (creature[c2].playerIndex!=0)
		if ((creature[c1].id==kAirElemental) || (creature[c1].id==kWaterElemental) || (creature[c1].id==kPoisonElemental) || (creature[c1].id==kFireElemental) || (creature[c1].id==kEarthElemental)) //Dave  || (creature[c1].id==kEarthElemental)
			if (creature[c2].attackTarget!=c1)
				if (in_ItemEquipedServer(creature[c2].playerIndex,kElementalRing))
				{
					creature[c1].attackTarget			=	0;
					creature[c1].nextStrikeTime		=	0;
					creature[c1].numberOfStrikes	=	0;
					creature[c1].trackRow					= 0;
					creature[c1].trackCol					= 0;
					creature[c1].tracking					= false;
					return (false);
				}

	if (creature[c2].playerIndex!=0)
		if ((creature[c1].id==kRedDraco) || (creature[c1].id==kIceDraco) || (creature[c1].id==kForestDraco) || (creature[c1].id==kBlackDraco) 
			|| (creature[c1].id==kSandDraco) || (creature[c1].id==kRedDragon) || (creature[c1].id==kIceDragon) || (creature[c1].id==kForestDragon) 
				|| (creature[c1].id==kBlackDragon) || (creature[c1].id==kPinkDragon)  || (creature[c1].id==kSandDragon))
			if (creature[c2].attackTarget!=c1)
				if (in_ItemEquipedServer(creature[c2].playerIndex,kDragonStoneRing))
				{
					creature[c1].attackTarget			=	0;
					creature[c1].nextStrikeTime		=	0;
					creature[c1].numberOfStrikes	=	0;
					creature[c1].trackRow					= 0;
					creature[c1].trackCol					= 0;
					creature[c1].tracking					= false;
					return (false);
				}

	if ((creature[c1].hiding) || (creature[c2].hiding))	// either hiding then stop attack
	{
		if ((creature[c1].playerIndex==0) && (creature[c2].playerIndex!=0))
			tx_CreatureTalkText(c1, c2, kJustHidTalk);

		if ((creature[c1].playerIndex!=0) || (creatureInfo[creature[c1].id].maxMagicPoints==0))
		{
			creature[c1].attackTarget			=	0;
			creature[c1].nextStrikeTime		=	0;
			creature[c1].numberOfStrikes	=	0;
			creature[c1].trackRow					= 0;
			creature[c1].trackCol					= 0;
			creature[c1].tracking					= false;
			return (false);
		}
	}

	if ((creature[c2].playerIndex!=0 && pl_AuthorizedTo(c2, kRegularPlayer) && player[creature[c2].playerIndex].stealth))	// Opponent stealthy then stop attack (disabled for GMs to let them lure)
	{
		if ((creature[c1].playerIndex==0))
			tx_CreatureTalkText(c1, c2, kJustHidTalk);

		if ((creature[c1].playerIndex!=0) || (creatureInfo[creature[c1].id].maxMagicPoints==0))
		{
			creature[c1].attackTarget			=	0;
			creature[c1].nextStrikeTime		=	0;
			creature[c1].numberOfStrikes	=	0;
			creature[c1].trackRow					= 0;
			creature[c1].trackCol					= 0;
			creature[c1].tracking					= false;
			return (false);
		}
	}

	if (creature[c2].dieTime!=0)  // going to die soon
		return (true);

	if (creatureInfo[creature[c1].id].attackRange<1)	// mod 8/12 structures - shelters, towers, etc
		return (false);

	if (creature[c2].playerIndex!=0)
		if (creature[c1].master!=0)
			if (in_ItemEquipedServer(creature[c2].playerIndex,kChaosRing))
				if (tb_Rnd(1,4))
				{
					for (i=0;i<kMaxPets;i++)
						if (player[creature[creature[c1].master].playerIndex].pet[i]==c1)
						{
							player[creature[creature[c1].master].playerIndex].pet[i]											= 0;
							player[creature[creature[c1].master].playerIndex].petInStable[i]									= false;
							player[creature[creature[c1].master].playerIndex].petType[i]										= 0;
							player[creature[creature[c1].master].playerIndex].petExperience[i]									= 0;
							player[creature[creature[c1].master].playerIndex].petHealth[i]										= 0;
							strcpy(player[creature[creature[c1].master].playerIndex].petName[i],"");
							nw_SendPetNameRefresh(creature[creature[c1].master].playerIndex);
							break;
						}

					creature[c1].attackTarget			=	creature[c1].master;

					creature[c1].master				    =	0;
					creature[c1].possibleTarget	  =	0;
					creature[c1].nextStrikeTime		=	0;
					creature[c1].numberOfStrikes	=	0;
					creature[c1].trackRow					= 0;
					creature[c1].trackCol					= 0;
					creature[c1].tracking					= false;
					return (false);
				}

	if (creature[c2].playerIndex!=0)  // check for pets guarding
		if (player[c2].dead==false)
			for (i=0;i<kMaxPets;i++)
				if (player[creature[c2].playerIndex].pet[i]!=0)
					if (player[creature[c2].playerIndex].petInStable[i]==false)
						if (creature[player[creature[c2].playerIndex].pet[i]].command==kGuard)
							if (creature[player[creature[c2].playerIndex].pet[i]].attackTarget==0)
								if (creature[player[creature[c2].playerIndex].pet[i]].possibleTarget==0)
								{
									creature[player[creature[c2].playerIndex].pet[i]].possibleTarget	=	c1;
								}

	if (creature[c1].playerIndex==0)
		if (cr_LowMorale(c1))
			return (false);

	if (creature[c1].disorientation!=0)	// attacker is disoriented
	{
		if (creature[c1].disorientation>TickCount())
		{
			if (tb_Rnd(1,2)==1)
				return (false);
		}
		else
			creature[c1].disorientation=0;
	}

	if (creature[c1].paralyzed!=0)	// attacker is paralyzed
	{
		if (creature[c1].paralyzed>TickCount())
			return (false);
		else
			creature[c1].paralyzed=0;
	}

	
	// merge 7/15
	if (creature[c1].master!=0) // chance of going wild during attack
		//if (player[creature[creature[c1].master].playerIndex].category==0)
		if (pl_AuthorizedTo(creature[c1].master,kCanLosePets)) // put in carbon 11/27
			if (in_ItemEquipedServer(creature[creature[c1].master].playerIndex,kDruidRing)==false)
				//if (tb_Rnd(1,175000)<(creatureInfo[creature[c1].id].tamingLevel*15))  // go wild
				if (creature[c2].level>4+creature[c1].level) //Dave .level>3  03-26-2011
				{
					defyChance=creature[c1].hunger - 1000 * ((creature[c2].level - creature[c1].level)-3);

					if (creature[creature[c1].master].playerIndex!=0)
						if (in_ItemEquipedServer(creature[creature[c1].master].playerIndex, kCharmOfLoyalty))
							defyChance+=5000; //Dave 3000 03-26-2011
						
					if (defyChance<0)
						defyChance=0;

					if (abs(creature[c1].level - creature[c2].level)>3)
						if (tb_Rnd(1,7000)>defyChance)
						{
							nw_SendTextToClient(creature[creature[c1].master].playerIndex, "Your pet defies you and goes wild!", 0);
							master=creature[c1].master;

							//pl_BanishCreature(-1,c1); // merge 7/27

							if (player[creature[creature[c1].master].playerIndex].skill[kTaming].level>425)
								player[creature[creature[c1].master].playerIndex].skill[kTaming].level=player[creature[creature[c1].master].playerIndex].skill[kTaming].level-tb_Rnd(5,10);

							creature[c1].master				  =	0;

							creature[c1].attackTarget	  =	0;

							creature[c1].possibleTarget	=	0;

							creature[c1].trackRow			  =	0;

							creature[c1].trackCol			  =	0;

							creature[c1].tracking			  =	false;

							//if (creatureInfo[creature[c1].id].alignment==kEvil)	// free evil creatures in town will attack former master
							//  if ((creature[c1].row>2075) && (creature[c1].row<2310) && (creature[c1].col>1749) && (creature[c1].col<2001))
							creature[c1].attackTarget	=	master;

							creature[c1].alignment=kEvil;




							for (j=0;j<kMaxPets;j++)
								if (player[creature[master].playerIndex].pet[j]==c1)
								{
									player[creature[master].playerIndex].pet[j]											= 0;
									player[creature[master].playerIndex].petInStable[j]									= false;
									player[creature[master].playerIndex].petType[j]										= 0;
									player[creature[master].playerIndex].petExperience[j]								= 0;
									player[creature[master].playerIndex].petHealth[j]									= 0;
									strcpy(player[creature[master].playerIndex].petName[j],"");
									nw_SendPetNameRefresh(creature[master].playerIndex);
								}
						}
				}

	// merge 7/15

	rowDist=fabs(creature[c1].row-creature[c2].row);

	colDist=fabs(creature[c1].col-creature[c2].col);

	if (rowDist>colDist)
		dist=rowDist;
	else
		dist=colDist;

	if (dist>=25)	// target too far away ... break off attack
	{

		creature[c1].attackTarget			=	0;
		creature[c1].nextStrikeTime		=	0;
		creature[c1].numberOfStrikes	=	0;
		creature[c1].trackRow					= 0;
		creature[c1].trackCol					= 0;
		creature[c1].tracking					= false;
		return (false);

	}
	else
	{
		if (creature[c1].master!=0)
		{
			if (creature[c1].master==creature[c2].master) // tamed creatures with same master can't fight
			{
				creature[c1].attackTarget			=	0;
				creature[c1].nextStrikeTime		=	0;
				creature[c1].numberOfStrikes	=	0;
				creature[c2].attackTarget			=	0;
				creature[c2].nextStrikeTime		=	0;
				creature[c2].numberOfStrikes	=	0;
				return (false);
			}

			if (creature[c1].attackTarget==creature[c1].master)
			{
				creature[c1].attackTarget			=	0;
				creature[c1].nextStrikeTime		=	0;
				creature[c1].numberOfStrikes	=	0;
				return (false);
			}
		}

		currentTime=TickCount();

		if (creature[c1].playerIndex!=0)	// get weapon range
		{
			if (player[creature[c1].playerIndex].arm[kRightHand]==0)
				weaponRange=1;
			else
				weaponRange	=	itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].weaponRange;
		}
		else
		{
			weaponRange	=	creatureInfo[creature[c1].id].attackRange;
		}

		if (dist>weaponRange)
		{
			//out of weapon range... break off attack
			return (false); 
		}

		if (currentTime<creature[c1].nextAttackTime) // is it time to attack again?
		{
			return (true);
		}

		if (dist>1)
			if (cr_ViewBlocked(creature[c1].row, creature[c1].col, creature[c2].row, creature[c2].col))
				if (cr_ViewBlocked(creature[c2].row, creature[c2].col, creature[c1].row, creature[c1].col))  //JS line of sight bug
					return (false);	// obstacle between foes


		// --------------- interrupt harvest -------------------

		if (creature[c1].playerIndex!=0)
		{
			if (player[creature[c1].playerIndex].gatherType==255) // player interrupted making item
			{
				it_FailedMake(creature[c1].playerIndex);
				creature[c1].nextStrikeTime=0;
				creature[c1].numberOfStrikes=0;
				creature[c1].strikePose=0;
			}
			else if (player[creature[c1].playerIndex].gathering!=0) // player interrupted resource harvest
			{
				rs_FailedHarvest(creature[c1].playerIndex);
				creature[c1].nextStrikeTime=0;
				creature[c1].numberOfStrikes=0;
				creature[c1].strikePose=0;
			}
			else if (player[creature[c1].playerIndex].meditating) // player interrupted meditating
			{
				player[creature[c1].playerIndex].meditating	=	false;
				nw_SendDisplayMessage(creature[c1].playerIndex,kConcentrationDisturbed);
			}
			else if (player[creature[c1].playerIndex].stealth) // player interrupted stealth
			{
				player[creature[c1].playerIndex].possibleBackstab	=	true;

				if (pl_AuthorizedTo(c1, kRegularPlayer))
				{
					player[creature[c1].playerIndex].stealth		=	false;
					player[creature[c1].playerIndex].stealthCool	=	TickCount()+3*60;
					nw_SendDisplayMessage(creature[c1].playerIndex,kLeavingStealth);
				}
			}
		}

		if (creature[c2].playerIndex!=0)
		{
			if (player[creature[c2].playerIndex].gatherType==255) // player interrupted making item
			{
				it_FailedMake(creature[c2].playerIndex);
				creature[c1].nextStrikeTime=0;
				creature[c1].numberOfStrikes=0;
				creature[c1].strikePose=0;
			}
			else if (player[creature[c2].playerIndex].gathering!=0) // player interrupted resource harvest
			{
				rs_FailedHarvest(creature[c2].playerIndex);
				creature[c1].nextStrikeTime=0;
				creature[c1].numberOfStrikes=0;
				creature[c1].strikePose=0;
			}
			else if (player[creature[c2].playerIndex].meditating) // player interrupted meditating
			{
				player[creature[c2].playerIndex].meditating	=	false;
				nw_SendDisplayMessage(creature[c2].playerIndex,kConcentrationDisturbed);
			}

		}

		// --------------- check for bow ---------------

		if (creature[c1].playerIndex!=0)
			if (player[creature[c1].playerIndex].arm[kRightHand]!=0)
				if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].itemType==kArrowWeaponType)
				{
					slot=-1;

					for (i=1;i<kMaxInventorySize;i++)
						if (creature[c1].inventory[i].itemID==kArrow)
						{
							slot=i;
							break;
						}

					if (slot<0) // no arrows
						return (false);

					in_RemoveFromInventorySlot(c1, slot, 1);

					nw_SendToClientDropFromInventorySlot(creature[c1].playerIndex, kNoMessage, slot, 1, 0);

					//creature[c1].soundFX					      = kArrowSnd;
					// creature[c1].soundFXTime			      = currentTime;
					creature[c1].finalStrikeSound       = kArrowSnd;
				}

		// ----------------------------------------------------------------- // random target switch

		if (creature[c2].playerIndex==0)
		{
			if (creature[c2].attackTarget==0)
			{
				if (creature[c2].possibleTarget==0)
				{
					creature[c2].attackTarget=0;
					creature[c2].possibleTarget=c1;
				}
			}
			else if (creature[c2].attackTarget!=c1 && creature[c2].taunted<currentTime)
			{
				if (creature[creature[c2].attackTarget].opportunityTime==0)
				{
					if (tb_Rnd(1,10)==1)
					{
						creature[c2].attackTarget=0;
						creature[c2].possibleTarget=c1;
					}
					else if (creatureInfo[creature[c2].id].maxMagicPoints>0)
					{
						if (tb_Rnd(1,10)<4)
						{
							creature[c2].attackTarget=0;
							creature[c2].possibleTarget=c1;
						}
					}
				}
				else
				{
					opport=6;

					if (creature[creature[c2].attackTarget].opportunityTime>TickCount()+(4*60))
						opport=8;

					if (tb_Rnd(1,10)<opport)
					{
						creature[c2].attackTarget=0;
						creature[c2].possibleTarget=c1;
					}
				}
			}
		}

		// -----------------------------------------------------------------

		if (creature[c2].playerIndex!=0)  // check for switch target
			if (creature[c2].attackTarget!=c1)
			{
				if (player[creature[c2].playerIndex].arm[kRightHand]==0)
					weaponRange=1;
				else
					weaponRange	=	itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[kRightHand]].itemID].weaponRange;

				if (cr_DistanceBetween(c2,creature[c2].attackTarget)>weaponRange)
					if (cr_DistanceBetween(c1,c2)<=weaponRange)
						creature[c2].attackTarget=c1;
			}

		// --------------- make monster immediately hit back --------------- // 0.1.4

		if (creature[c1].playerIndex!=0)
			if (creature[c2].playerIndex==0)
				if (currentTime>=creature[c2].nextAttackTime) // is it time to attack again?
					if (creature[c2].attackTarget==c1)
						if (!cr_LowMorale(c2))
							creature[c2].nextMoveTime=currentTime;

		// --------------- check for interrupted harvest ---------------

		if (creature[c1].playerIndex!=0)
			if (player[creature[c1].playerIndex].gathering!=0)
				sk_SendFailedHarvestMessage(creature[c1].playerIndex);

		if (creature[c2].playerIndex!=0)
			if (player[creature[c2].playerIndex].gathering!=0)
				sk_SendFailedHarvestMessage(creature[c2].playerIndex);

		// --------------- Robe restriction test 03-28-2012---------------
		/*
		if (in_ItemEquipedServer(creature[c1].playerIndex,kMagicRobeType))
			if (player[creature[c1].playerIndex].characterClass!=3)
				if (player[creature[c1].playerIndex].characterClass!=4)
					if (player[creature[c1].playerIndex].characterClass!=5)
						
						creature[c1].nextStrikeTime     = currentTime+3000;
		
		*/
		
		// --------------- face direction ---------------

		rowDelta=creature[c2].row-creature[c1].row;

		colDelta=creature[c2].col-creature[c1].col;

		if ((rowDelta<=-1) && (colDelta<=-1)) 				creature[c1].facing=0;
		else if ((rowDelta<=-1) && (colDelta==0)) 		creature[c1].facing=1;
		else if ((rowDelta<=-1) && (colDelta>=1)) 		creature[c1].facing=2;
		else if ((rowDelta==0) && (colDelta<=-1)) 		creature[c1].facing=7;
		else if ((rowDelta==0) && (colDelta>=1)) 			creature[c1].facing=3;
		else if ((rowDelta>=1) && (colDelta<=-1)) 		creature[c1].facing=6;
		else if ((rowDelta>=1) && (colDelta==0)) 			creature[c1].facing=5;
		else if ((rowDelta>=1) && (colDelta>=1)) 			creature[c1].facing=4;

		//Rogues - Backstab (daggers only)

		/*if (creature[c1].playerIndex!=0)
			if (player[creature[c1].playerIndex].possibleBackstab)
			{
				if (player[creature[c1].playerIndex].lastBackstabTarget!=c2)
					if (creature[c1].facing==creature[c2].facing)
						if (player[creature[c1].playerIndex].characterClass==5 || pl_AuthorizedTo(c1, kIsAnyGM) || pl_AuthorizedTo(c1, kIsQuestCharacter))
							if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].useSkill==kDaggers)
							{
								if (creature[c2].paralyzed>currentTime || tb_Rnd(0, 100)<(sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)*10))
									backstab=1;
								else
									backstab=2;
							}

				player[creature[c1].playerIndex].lastBackstabTarget=c2;

				player[creature[c1].playerIndex].possibleBackstab=false;
			}
		*/



		//Miss and Critical hit

		if (creature[c1].playerIndex!=0)
		{
			if (player[creature[c1].playerIndex].arm[kRightHand]!=0)
				//if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].useSkill!=kArchery)
				{
					crit=tb_Rnd(1,20);

					if (crit==1)
						missed=true;

					//if (player[creature[c1].playerIndex].characterClass==3 || pl_AuthorizedTo(c1, kIsAnyGM))//|| pl_AuthorizedTo(c1, kIsQuestCharacter))
					//if (creature[c1].noCrit==false)
					{
						if (crit>=21-(sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)/2))
							critical=true;
						else
						{
							if (currentCycle==kFightingMoon && fullMoon)
								if (crit>=20-(sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)/2))
									critical=true;

							if (creature[c2].paralyzed>currentTime)
								if (crit>=20-(sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)/2))
									critical=true;
						}
					}
				}
			
			if (critical && itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].useSkill==kArcheryFighting) //check for archery crits
				
			{	
					
				if (pl_AuthorizedTo(c1, kIsAnyGM)==false && player[creature[c1].playerIndex].characterClass!=4) //non ranger/gm
					critical=false;
						
				if (critical && creature[c1].nextMoveTime+60>currentTime) //moved less than a second ago, no crit
					critical=false;	
				
			}
			
			if (critical && itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].useSkill==kArcheryFighting)
				
				{creature[c1].finalStrikeSound       = kArrowCritSnd;}
			//else
			//	{creature[c1].finalStrikeSound       = kArrowSnd;}
			
			
			if (critical && pl_AuthorizedTo(c1, kRegularPlayer))
				if (player[creature[c1].playerIndex].characterClass!=3)
					if (player[creature[c1].playerIndex].characterClass!=4)
						if (player[creature[c1].playerIndex].characterClass!=5)
							critical=false;
					
		}


		// ---------- calculate next attack time ---------

		if (creature[c1].playerIndex!=0)	// get weapon speed
		{
			if (player[creature[c1].playerIndex].arm[kRightHand]==0)
				creature[c1].nextAttackTime = currentTime + 250;
			else
				creature[c1].nextAttackTime	= currentTime + itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].weaponSpeed;
			
			if (creature[c1].specialAttackArg1==kSpatAttackSpeed)
				creature[c1].nextAttackTime= currentTime + cr_SpecialAttack(c1, c2, creature[c1].nextAttackTime-currentTime);
		}
		else
			creature[c1].nextAttackTime	= currentTime + 200;

		// -------- check for magic creature attack  --------

		if (creature[c1].playerIndex==0)
		{
			if (creatureInfo[creature[c1].id].spellcastChance>0)	// magical creature attacking
			{
				if (tb_Rnd(1,100)<creatureInfo[creature[c1].id].spellcastChance)
				{
					/*if ((creature[c1].hitPoints<20) && (tb_Rnd(1,3)>1))
						sk_CastSpellOnCreature(c1, c1, kGreaterHealSpell, false);
					else if ((creature[c1].poison>0) && (tb_Rnd(1,3)>1))
						sk_CastSpellOnCreature(c1, c1, kGreaterDetoxifySpell, false);
					else if (creature[c2].hiding)
					{
						sk_CastSpellOnTerrain(c1,0,0,kRevealSpell);
						creature[c2].hiding=false;
					}
					else
					{
						magicSuccess=false;
						spell=0;

						if (creature[c2].paralyzed==0)
							if (tb_Rnd(1,5)==1)
								spell=1;

						if (spell==0)
							spell=tb_Rnd(2,9);

						if (creature[c2].playerIndex==0)
							spell=tb_Rnd(2,4);
						else if (tb_Rnd(1,3)>1)
							spell=tb_Rnd(3,4);

						if (creature[c1].skipSpell==0)
						{
							if (spell==1) magicSuccess=sk_CastSpellOnCreature(c1, c2, kParalyzeSpell, false);

							if (spell==2) magicSuccess=sk_CastSpellOnCreature(c1, c2, kPoisonSpell, false);

							if (spell==3) magicSuccess=sk_CastSpellOnCreature(c1, c2, kExplosionSpell, false);

							if (spell==4) magicSuccess=sk_CastSpellOnCreature(c1, c2, kLightningBoltSpell, false);

							if (spell==5) magicSuccess=sk_CastSpellOnCreature(c1, c2, kDisorientationSpell, false);

							if (spell==6) magicSuccess=sk_CastSpellOnCreature(c1, c2, kClumsySpell, false);

							if (spell==7) magicSuccess=sk_CastSpellOnCreature(c1, c2, kWeakenSpell, false);

							if (spell==8) magicSuccess=sk_CastSpellOnCreature(c1, c2, kSimpleMindSpell, false);
						}
						else
						{
							creature[c1].skipSpell--;
							magicSuccess=false;
						}*/
						
						if ((creature[c1].hitPoints<creatureInfo[creature[c1].id].selfhealTrigger) && (tb_Rnd(1,100)<creatureInfo[creature[c1].id].selfhealChance))
							sk_CastSpellOnCreature(c1, c1, kCreatureHealSpell, false);
						else if ((creature[c1].poison>0) && (tb_Rnd(1,100)<33))
							sk_CastSpellOnCreature(c1, c1, kGreaterDetoxifySpell, false);
						else if (creature[c1].skipSpell==0)
						{
							magicSuccess=cr_CastCreatureSpell(c1, c2);
						}
						else
						{
							creature[c1].skipSpell--;
							magicSuccess=false;
						}
						
					//}

					//-----
					if (magicSuccess)
					{
						if (creature[c2].hiding)	creature[c2].hiding=false;
						
						
						creature[c1].nextStrikeTime=currentTime;
						creature[c1].numberOfStrikes=4;
						creature[c1].strikePose											=	1;
						
						creature[c1].nextAttackTime	= currentTime + 600 - creatureInfo[creature[c1].id].mageryLevel*10;
					}

					//-----
					if (magicSuccess || creatureInfo[creature[c1].id].spellcastChance==100) //no melee
					{
						return (true);
					}
				}

				//return (true);
			}
		}

		// --------------- attack sound ---------------

		if (creature[c1].playerIndex!=0 && player[creature[c1].playerIndex].morph==0)
		{
			if (creature[c1].finalStrikeSound!=kArrowSnd)
			{
				attackerMetal                 = false;
				targetMetal                   = false;
				creature[c1].finalStrikeSound = kNonMetalSnd;

				if (player[creature[c1].playerIndex].arm[kRightHand]!=0)
					if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].metal)
						attackerMetal=true;

				if (creature[c2].playerIndex==0)
					targetMetal=creatureInfo[creature[c2].id].metal;
				else
				{
					if (itemList[creature[c2].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].metal)  targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c1].playerIndex].arm[kHead]].itemID].metal)       targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c1].playerIndex].arm[kTorso]].itemID].metal)      targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c1].playerIndex].arm[kLegs]].itemID].metal)       targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c1].playerIndex].arm[kArms]].itemID].metal)       targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c1].playerIndex].arm[kHands]].itemID].metal)      targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c1].playerIndex].arm[kLeftHand]].itemID].metal)   targetMetal=true;
				}

				if (!missed && !critical)
				{
					if ((attackerMetal) && (targetMetal))
						creature[c1].finalStrikeSound       = kMetalMetalSnd;

					if ((attackerMetal) && (!targetMetal))
						creature[c1].finalStrikeSound       = kMetalNonMetalSnd;
				}

				else if (missed)
					creature[c1].finalStrikeSound       = kMissedSnd;

				else if (critical)
				{
					if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].useSkill==kArcheryFighting) //check for archery crits
						creature[c1].finalStrikeSound       = kArrowCritSnd;
					
					if ((attackerMetal) && (targetMetal))
						creature[c1].finalStrikeSound       = kMetalMetalCritSnd;

					if ((attackerMetal) && (!targetMetal))
						creature[c1].finalStrikeSound       = kMetalNonMetalCritSnd;

					if (!attackerMetal && itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].useSkill!=kArcheryFighting)
						creature[c1].finalStrikeSound		= kNonMetalCritSnd;
				}

			}
		}
		else
		{
			if (creature[c1].playerIndex!=0) 		crit=player[creature[c1].playerIndex].morph;
			else									crit=creature[c1].id;
			
			
			if (creatureInfo[crit].blunt!=0)
			{
				creature[c1].finalStrikeSound = creatureInfo[crit].blunt;
			}
			else if (creatureInfo[crit].metal || (creature[c1].npcIndex!=0 && (npc[creature[c1].npcIndex].type!=kNPCRegular)))
			{
				targetMetal                   = false;

				if (creature[c2].playerIndex==0)
					targetMetal=creatureInfo[creature[c2].id].metal;
				else if (player[creature[c2].playerIndex].morph!=0)
				{
					targetMetal=creatureInfo[player[creature[c2].playerIndex].morph].metal;	
				}
				else
				{
					if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[kRightHand]].itemID].metal)  targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[kHead]].itemID].metal)       targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[kTorso]].itemID].metal)      targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[kLegs]].itemID].metal)       targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[kArms]].itemID].metal)       targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[kHands]].itemID].metal)      targetMetal=true;

					if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[kLeftHand]].itemID].metal)   targetMetal=true;
				}

				if (targetMetal)
					creature[c1].finalStrikeSound       = kMetalMetalSnd;

				if (!targetMetal)
					creature[c1].finalStrikeSound       = kMetalNonMetalSnd;
			}
			else if (creatureInfo[crit].attackSound!=0)
			{
				cr_AddSound(c1, creatureInfo[crit].attackSound);
				//creature[c1].soundFX							= creatureInfo[creature[c1].id].attackSound;
				//creature[c1].soundFXTime					= currentTime;
			}
		}

		// --------------- calculate damage resist ---------------

		if (creature[c2].playerIndex!=0)
		{
			resist=(float)creature[c2].armorRating;

			if (player[creature[c2].playerIndex].arm[kLeftHand]!=0)
			{
				resist=resist+(float)(sk_GetSkillLevel(player[creature[c2].playerIndex].skill[kParrying].level))*0.10;
				sk_TrainProof(c2, c1, 0, kParrying, 1);//sk_Parrying(creature[c2].playerIndex,c1);
			}
			
			if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[kRightHand]].itemID].twoHandItem && itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[kRightHand]].itemID].useSkill!=kArcheryFighting) //poles raise parrying
			{
				resist=resist+(float)(sk_GetSkillLevel(player[creature[c2].playerIndex].skill[kParrying].level))*0.07;
				sk_TrainProof(c2, c1, 0, kParrying, 1);//sk_Parrying(creature[c2].playerIndex,c1);
			}

			if (creatureInfo[creature[c1].id].undead) // bonus for armed silver
				for (i=1;i<13;i++)
					if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID].silver)
						resist++;

			if (pl_AuthorizedTo(c2,kExtraResistance)) // put in carbon 11/27
				resist=resist*1.5;
			
			
		}
		else if (creature[c2].npcIndex==0)
			resist=(float)creatureInfo[creature[c2].id].armorRating;
		else
		{
			if (npc[creature[c2].npcIndex].type==kNPCBrigand)
				resist=30;
			else if (npc[creature[c2].npcIndex].type==kNPCMercenary)
				resist=40;
			//Dave
			else if (npc[creature[c2].npcIndex].type==kNPCPrivateer)
				resist=resist+95;
			else if (npc[creature[c2].npcIndex].type==kNPCDwarf1)
				resist=50;
			else if (creature[c2].master!=0) //Tamed guards/humans
				resist=72;
			else
				resist=40;
		}

		if (creature[c2].master!=0) //Extra resist for pets
			for (i=creatureInfo[creature[c2].id].creatureLevel; i<creature[c2].level; i++)
				resist++;

		if (resist>=100)
			resist=99;
		
		if (creature[c2].godMode)
			resist=90;
		
		resist=100-resist;

		if (resist<0)
			resist=0;

		resist=resist/100;

		weaponModifier=1.0;

		// --------------- calculate health damge ---------------

		if (creature[c1].playerIndex!=0)	// get damage from weapon
		{
			if (player[creature[c1].playerIndex].morph!=0) //morphed druids   && pl_AuthorizedTo(c1, kRegularPlayer) || pl_AuthorizedTo(c1, kIsQuestScoundrel)
			{
				damage	=	tb_Rnd(creatureInfo[player[creature[c1].playerIndex].morph].attackDamageMin,creatureInfo[player[creature[c1].playerIndex].morph].attackDamageMax); //use creature melee
			}
			//if (creatureInfo[i].poisonous!=0) //poison  creature
			else
			{
				if (player[creature[c1].playerIndex].arm[kRightHand]==0)
				{
					damage			=	(float)(sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kWrestling].level))*0.9;//1.5;

					anatomy 		= (float)sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)*0.05;
					proficiency		= 0;//(float)sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kWrestling].level)*0.4;
					strength		= (float)(creature[c1].strength+creature[c1].strengthOffset)*0.3;
					dexterity		= (float)(creature[c1].dexterity+creature[c1].dexterityOffset)*0.25;

					total				= (anatomy+proficiency+strength+dexterity)/100;

					damage			= damage * total;

					sk_Anatomy(creature[c1].playerIndex, 0, false);
					sk_TrainProof(c1, c2, 0, kWrestling, 1);
				}
				else
				{
					for (i=0;i<kMaxWeaponModifiers;i++)
					{
						if (creatureInfo[creature[c2].id].weaponID[i]==0)
							break;

						if (creatureInfo[creature[c2].id].weaponID[i]==itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].parent)
						{
							weaponModifier=creatureInfo[creature[c2].id].weaponModifier[i];
							break;
						}
					}

					//itemList[item].parent

					damage			=	itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].weaponDamage;

					useSkill		= (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].useSkill==kArcheryFighting)?kArchery:kMelee;//itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].useSkill;

					if (!in_CanGroup(creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID))
					{
						condition		= (float)creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount;

						if (condition>127)
							condition=condition-128;
					}
					else condition=127;

					condition		= condition * 0.78;

					condition		= (float)condition*0.15;

					anatomy 		= (float)sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)*0.15;

					proficiency	= (float)sk_GetSkillLevel(player[creature[c1].playerIndex].skill[useSkill].level)*0.4;

					strength		= (float)(creature[c1].strength+creature[c1].strengthOffset)*0.15;

					dexterity		= (float)(creature[c1].dexterity+creature[c1].dexterityOffset)*0.15;

					total				= (condition+anatomy+proficiency+strength+dexterity)/100;

					damage			= damage * total;

					if ((float)creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount>127) // bonus for GM weapon
						damage=damage*1.5;

					//if (player[creature[c1].playerIndex].category==5) // merge 7/29 extra damage for brigand
					//if (pl_GMLevel(c1)>=4) // put in carbon 11/27
					//if (pl_AuthorizedTo(c1,kExtraDamage))
						//damage=damage*2;

					/*
					if (creatureInfo[creature[c2].id].fire) // bonus for using frost axe against fire creatures
						{
						  if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID==kFrostAxe)
						  	{
						    	damage=damage*5;
						    }
						}
						
					if (creatureInfo[creature[c2].id].frost) // bonus for using flaming sword against frost creatures
						{
						  if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID==kFireSword)
						  	damage=damage*5;
						}

					if (creatureInfo[creature[c2].id].domain==kWater) // bonus for using trident against water creatures
						{
						  if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID==kNeptunesTrident)
						  	{
						    	damage=damage*6;
						    }
						}
					*/

					if (creatureInfo[creature[c2].id].undead) // bonus for using silver against undead
					{
						if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].silver)
							damage=damage*2;
						
						if (creature[c2].playerIndex!=0)
							if (in_ItemEquipedServer(creature[c2].playerIndex, kCharmOfUndeath))
								damage=damage*1.3;
					}

					// check for poison weapon

					if (creatureInfo[creature[c2].id].immuneToPoison==false && !missed)
						if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].poison)
							if (tb_Rnd(1,2)==1)
								if (creature[c2].poison<5)
								{
									creature[c2].poison=creature[c2].poison+1;

									if (creature[c2].playerIndex!=0)
										nw_SendDisplayMessage(creature[c2].playerIndex,kYouHaveBeenPoisoned);
								}



					sk_Anatomy(creature[c1].playerIndex, 0, false);
					
					if (useSkill==kMelee)
						/*//Dave 03-28-2012
						if (player[creature[c1].playerIndex].characterClass==3)
							if (player[creature[c1].playerIndex].characterClass==4)
								if (player[creature[c1].playerIndex].characterClass==5)
						if (player[creature[c1].playerIndex].arm[kTorso]==kMagicRobeType)
						{
							gr_AddText("Your robe is interfering with your melee.",false,true);
							return;
						}
						
						//Dave^*/
						sk_TrainProof(c1, c2, 0, kMelee, 1);
					
					/*if (useSkill==kMaceFighting)
						sk_MaceFighting(creature[c1].playerIndex,c2);
					else if (useSkill==kDaggers)
						sk_SmallWeapons(creature[c1].playerIndex,c2);
					else if (useSkill==kSwordsmanship)
						sk_Swordsmanship(creature[c1].playerIndex,c2);
					else if (useSkill==kPoleFighting)
						sk_PoleFighting(creature[c1].playerIndex,c2);

					else*/ 
					
					if (useSkill==kArchery)
					{
						//Dave add diso can't shoot arrows 02-04-2011					
								
						if (creature[c1].disorientation!=0)// attacker is disoriented
							{
							if (creatureInfo[creature[c1].id].mobile==true)	
								{
								damage=0;
								nw_SendDisplayMessage(creature[c1].playerIndex, kHitMissed);
								}
							else 
								{
								nw_SendDisplayMessage(c1, kYouAreDisoriented);
								creature[c1].nextAttackTime = currentTime + 600;
								}
							}
					//	if (creature[c1].disorientation!=0)
					//		if (tb_Rnd(1,5)<3)
					//			{
					//			damage=0;
					//			nw_SendDisplayMessage(creature[c1].playerIndex, kHitMissed);
					//			}
							
						sk_TrainProof(c1, c2, 0, kArchery, 1);//sk_Archery(creature[c1].playerIndex,c2);
						//damage=damage*0.85;	// version 0.1.2 - lessen archery damage

						if ((tb_Rnd(0,100)>sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kArchery].level)) && (tb_Rnd(1,10)<dist))
						{
							damage=0;
						}
						else if (tb_Rnd(1,2)==1)
							if (in_AddtoInventory(c2, kArrow, 1)>0)
								if (creature[c2].playerIndex!=0)
									nw_SendInventoryAdd(creature[c2].playerIndex, kArrow, 1, kNoMessage, 0);
					}
				}
			
			}
		}
		else
		{
			damage	=	tb_Rnd(creatureInfo[creature[c1].id].attackDamageMin,creatureInfo[creature[c1].id].attackDamageMax);

			for (i=1;i< ( (creature[c1].level) - (creatureInfo[creature[c1].id].creatureLevel) +1 );i++)
				damage=damage*1.06;

			if (creature[c1].npcIndex!=0 && !np_IsRegularNPC(c1)) //Hit by a Protector? (guard, acolyte, etc)
				if (creature[c2].playerIndex==0 && creature[c2].npcIndex==0) //Monster?
					creature[c2].noDrop=true; //Will not drop loot on death
		}
		
		if (creature[c1].godMode)
			damage=damage*5;

		if (creature[c2].reflectDamage>0)	// ------------ reflective armor ---------------
		{
			// --- calculate damage reflected to attacker

			attackerDamage	=	tb_Round(damage*((float)creature[c2].reflectDamage/100));
			damage					=	tb_Round(damage*((100-(float)creature[c2].reflectDamage)/100));

			// --------------- calculate damge resist of attacker ---------------

			if (creature[c1].playerIndex!=0)
			{
				attackerResist=(float)creature[c1].armorRating;

				if (player[creature[c1].playerIndex].arm[kLeftHand]!=0)
					attackerResist=attackerResist+(float)(sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kParrying].level))*0.10;
			}
			else
				attackerResist=(float)creatureInfo[creature[c1].id].armorRating;

			if (attackerResist>=100)
				attackerResist=99;

			attackerResist=100-attackerResist;

			if (attackerResist<0)
				attackerResist=0;

			attackerResist=attackerResist/100;

			attackerDamage=tb_Round(attackerDamage*attackerResist);
			
			if (creature[c1].damageReduc<1.0)
				attackerDamage=tb_Round(attackerDamage*creature[c1].damageReduc);

			if (creature[c1].npcIndex!=0 && npc[creature[c1].npcIndex].spawned==false && (npc[creature[c1].npcIndex].type==kNPCRegular || npc[creature[c1].npcIndex].type==kNPCGuard))	// can't harm non-spawned NPCs
				if (creature[c1].master==0)
					attackerDamage=0;
				
					

			if (attackerDamage>=creature[c1].hitPoints) // damage attacker
				cr_setDieTime(c1,(currentTime+100));

			//cr_Dead(c1);
			else
				creature[c1].hitPoints=creature[c1].hitPoints-attackerDamage;

			creature[c2].reflectDamage=0;
		}

		// ------- end reflective armor ---------------------------------------------------

		damage=tb_Round(damage*weaponModifier);
		
		damage=tb_Round(damage*resist);

		/*if (backstab==1)
		{
			damage+=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)*3;
			nw_SendDisplayMessage(creature[c1].playerIndex, kBackstabDone);

			if (creature[c2].playerIndex!=0)
				nw_SendDisplayMessage(creature[c2].playerIndex, kBackstabbed);


			//Backstab + poison = para
			if (creatureInfo[creature[c2].id].immuneToPoison==false)
				if (creature[c2].paralyzed==0)
					if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].poison)
					{
						creature[c2].paralyzed=currentTime+60*tb_Rnd(sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)-1,sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)+1);

						if (creature[c2].playerIndex!=0)
							nw_SendDisplayMessage(creature[c2].playerIndex, kYouAreParalyzed);
					}
		}
		else if (backstab==2)
		{
			nw_SendDisplayMessage(creature[c1].playerIndex, kBackstabFailed);
		}*/

		if (critical)
		
		{	
			//Dave change crit damage 
			
		/*	//Original code start
			if (critical)
		{
			if (player[creature[c1].playerIndex].characterClass==3 || pl_AuthorizedTo(c1, kIsAnyGM))
			{
				damage+=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level);
			}
			
			damage+=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[useSkill].level);
			
			nw_SendDisplayMessage(creature[c1].playerIndex, kCriticalDone);
		}
			//Original code end
			*/
			if (player[creature[c1].playerIndex].characterClass==3 || pl_AuthorizedTo(c1, kIsAnyGM)) //Fighter
				{
					//damage+=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level);
					anatLevel=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level);
					meleeLevel=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kMelee].level);
					if (meleeLevel<=1)	{	return (false);	}
					damage+=(anatLevel/2)+tb_Rnd(anatLevel/4,anatLevel/2)+(meleeLevel/2)+tb_Rnd(1,meleeLevel/2);
					//damage=damage*0.75;
					//damage=tb_Round(damage);
				}
			else if (player[creature[c1].playerIndex].characterClass==4)//Ranger
					if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].useSkill==kArcheryFighting)
				
					{
						anatLevel=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level);
						archLevel=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kArchery].level);
						
						damage+=(anatLevel/2)+tb_Rnd(anatLevel/4,anatLevel/2)+(archLevel/2)+tb_Rnd(1,archLevel/2);
						//damage=tb_Round(damage);
						if (damage<=1) { damage=1; } //Add ranger crit fix for melee 1
					}
					else 
					{
						anatLevel=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level);
						meleeLevel=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kMelee].level);
						if (meleeLevel<=1)	{	return (false);	}
						damage+=(anatLevel/2)+tb_Rnd(anatLevel/4,anatLevel/2)+(meleeLevel/2)+tb_Rnd(1,meleeLevel/2);
						//damage=tb_Round(damage);
						if (damage<=1) { damage=1; }
					}
					
			else if (player[creature[c1].playerIndex].characterClass==5)//Rogue
				if (!pl_BehindTargetServer(c1,c2))
					{
					anatLevel=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level);
					meleeLevel=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kMelee].level);
					if (meleeLevel<=1)	{	return (false);	}
					damage+=(anatLevel/2)+tb_Rnd(anatLevel/4,anatLevel/2)+(meleeLevel/2)+tb_Rnd(1,4);
					
					if (damage<=1) { damage=1; }
					}
				else
					{
					
					anatLevel=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level);
					meleeLevel=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kMelee].level);
					if (meleeLevel<=1)	{	return (false);	}
					damage+=(anatLevel/2)+tb_Rnd(anatLevel/4,anatLevel/2)+(meleeLevel/2)+tb_Rnd(1,2);
					//damage+=(anatLevel/2)+tb_Rnd(anatLevel/4,anatLevel/2)+(meleeLevel/2)+tb_Rnd(1,ceil(meleeLevel/3));  // ceil = Lia test
					//damage=damage*0.5;
					//damage=tb_Round(damage);
					if (damage<=1) { damage=1; }
					}
			//damage+=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[useSkill].level);
			
			//damage+=sk_GetSkillLevel(player[creature[c1].playerIndex].skill[useSkill].level);
			
			
			nw_SendDisplayMessage(creature[c1].playerIndex, kCriticalDone);
		}

		if (missed)
		{
			if (creature[c1].specialAttackArg2==0 || creature[c1].specialAttackArg2==3) //mis spat, pay the price still
			{
				creature[c1].stamina-=itemList[creature[c1].specialAttackType].price;
				cr_ClearSpat(c1, true);
			}
			damage=0;
			nw_SendDisplayMessage(creature[c1].playerIndex, kHitMissed);
		}
		else if (creature[c1].specialAttackArg1==kSpatDirectDamage)
			damage=cr_SpecialAttack(c1, c2, damage);
		
		if (creature[c2].specialAttackArg1==kSpatDamageAbsorb)
			damage=cr_SpecialAttack(c2, c1, damage);


		if (creature[c2].npcIndex!=0 && npc[creature[c2].npcIndex].spawned==false && (npc[creature[c2].npcIndex].type==kNPCRegular || npc[creature[c2].npcIndex].type==kNPCGuard))	// can't harm non-spawned NPCs
			if (creature[c2].master==0)
				damage=0;
			
		if (creature[c2].damageReduc<1.0)
			damage=tb_Round(damage*creature[c2].damageReduc);

		if (damage>0)
		{
			if (tb_Rnd(1,3)==1)
				cr_DropBloodStain(creature[c2].id,creature[c2].row+tb_Rnd(0,2)-1,creature[c2].col+tb_Rnd(0,2)-1);

			if (creatureInfo[creature[c1].id].poisonous>0) // poison attack
				if (creatureInfo[creature[c2].id].immuneToPoison==false)
					if (tb_Rnd(0,100)<creatureInfo[creature[c1].id].poisonous)
						if (creature[c2].poison<100)
						{
							creature[c2].poison=creature[c2].poison+1;

							if ((creatureInfo[creature[c1].id].poisonous>75) && (creature[c2].poison<100))  // very poisonous
								creature[c2].poison=creature[c2].poison+1;
							
						
							if (creature[c2].playerIndex!=0)
								nw_SendDisplayMessage(creature[c2].playerIndex,kYouHaveBeenPoisoned);
						}

			if (creatureInfo[creature[c1].id].curse>0) // curse
				if (creature[c2].playerIndex!=0)
					if (tb_Rnd(0,100)<creatureInfo[creature[c1].id].curse)
						if (!in_ItemEquipedServer(creature[c2].playerIndex, kCharmOfPurity) || tb_Rnd(1,100)>30)
							pl_CursePlayer(creature[c2].playerIndex);
			
			
			
		}


		if (damage>=creature[c2].hitPoints)
		{
			if (creature[c1].playerIndex!=0)
			{
				pl_AddToTitle(creature[c1].playerIndex,creatureInfo[creature[c2].id].creatureLevel);  // made a kill
			}

			cr_setDieTime(c2,(currentTime+120));

			if (creature[c2].playerIndex!=0 && creature[c2].godMode==false && creature[c2].noDeath==false)
				tx_CreatureTalkText(c1,c2,kKillingTalk);

			if (creature[c1].master!=0)
				if (creature[c2].playerIndex==0)
					if (creature[c2].master==0)
						if (abs(creature[c1].level - creature[c2].level)<4)
							if (creature[c1].level < 2 * creatureInfo[creature[c1].id].creatureLevel)
							{
								creature[c1].petExp+=creature[c2].level * 10;

								if (sk_GetSkillLevel(creature[c1].petExp)>creature[c1].level)
									creature[c1].level++;
							}
		}
		else
		{
			tx_CreatureTalkText(c1,c2,kAttackingTalk);
			creature[c2].hitPoints=creature[c2].hitPoints-damage;

			if (creature[c2].master!=0)
				//if (player[creature[creature[c2].master].playerIndex].category==0)
				if (pl_AuthorizedTo(creature[c2].master,kCanLosePets)) // put in carbon 11/27
				{
					loyaltyDamage=damage*8;

					if (creature[c2].hunger<loyaltyDamage)
						loyaltyDamage=0;
					else
						creature[c2].hunger=creature[c2].hunger-loyaltyDamage;

					player[creature[creature[c2].master].playerIndex].sendPetsRefresh=true;
				}
		}

//-----
		creature[c1].nextStrikeTime     = currentTime;

		creature[c1].numberOfStrikes    = 2;

		creature[c1].strikePose					=	1;

		if (creature[c1].playerIndex!=0)
		{
			//creature[c1].strikePose											=	1;
			player[creature[c1].playerIndex].gatherType	= 0;
			player[creature[c1].playerIndex].gathering	=	0;
			player[creature[c1].playerIndex].gatherRow	=	0;
			player[creature[c1].playerIndex].gatherCol	=	0;
		}

//-----

		if (creature[c1].master!=0) //Taming leveling
		{

			expGained=0;

			if ((sk_GetSkillLevel(player[creature[creature[c1].master].playerIndex].skill[kTaming].level)-creatureInfo[creature[c1].id].creatureLevel)>3)
				if (tb_Rnd(0,10)<1)
					expGained=1;
				else
					expGained=-1;

			if (abs(creatureInfo[creature[c1].id].creatureLevel-creatureInfo[creature[c2].id].creatureLevel)>3)
				if (tb_Rnd(0,10)<1)
					expGained=1;
				else
					expGained=-1;

			if (expGained==0)
			{
				tmpExp=30-creature[c1].attackCounter;

				if (tmpExp<1)
					tmpExp=1;

				tmpExp=0.5 * (1/tmpExp);

				tmpExp2=(60)-((TickCount()-creature[c1].outOfStable)/3600.0);

				if (tmpExp2<1)
					tmpExp2=1;

				tmpExp2=0.5 * (1/tmpExp2);

				tmpExp+=tmpExp2;

				tmpExp2=(creature[c1].hunger)/5000.0;

				tmpExp+=tmpExp2;

				tmpExp+=1;

				tmpExp2=sk_GetSkillLevel(player[creature[creature[c1].master].playerIndex].skill[kTaming].level)-creatureInfo[creature[c1].id].creatureLevel;

				tmpExp2+=abs(creatureInfo[creature[c1].id].creatureLevel-creatureInfo[creature[c2].id].creatureLevel);

				if (tmpExp2==0)
					tmpExp2=1;


				tmpExp=tmpExp/tmpExp2;

				if (tmpExp > 0.49)
					expGained++;

				if (tmpExp > 1.49)
					expGained++;
			}

			if (expGained<0)
				expGained=0;


			// -------------------- anti macroing code -----------------
			macroing=false;

			totalExp=0;

			for (s=0;s<kMaxMacroTrace;s++)
				if (creature[c1].attackTargets[s][0]==c2)
				{
					macroing=true;
					totalExp+=creature[c1].attackTargets[s][1];
					creature[c1].attackTargets[s][1]+=expGained;
				}

			if (macroing==false)
			{
				found=false;

				for (s=0;s<kMaxMacroTrace;s++)
					if (creature[c1].attackTargets[s][0]==0)
					{
						creature[c1].attackTargets[s][0]=c2;
						creature[c1].attackTargets[s][1]=expGained;
						found=true;
						break;
					}

				if (found==false)
				{
					for (s=1;s<kMaxMacroTrace;s++)
						creature[c1].attackTargets[s-1][0]=creature[c1].attackTargets[s][0];

					creature[c1].attackTargets[s-1][1]=creature[c1].attackTargets[s][1];

					creature[c1].attackTargets[kMaxMacroTrace-1][0]=c2;

					creature[c1].attackTargets[kMaxMacroTrace-1][1]=expGained;
				}
			}

			// ----------------------------------------------------------
			if (!macroing)
				creature[c1].attackCounter++;

			if (totalExp<2 && expGained>0)
			{
				if (creature[c1].level < 2 * creatureInfo[creature[c1].id].creatureLevel)
				{
					creature[c1].petExp++;

					if (sk_GetSkillLevel(creature[c1].petExp)>creature[c1].level)
						creature[c1].level++;
				}


				if (tb_Rnd(1,10)<3)
					sk_RaiseSkill(creature[creature[c1].master].playerIndex,0,kTaming,expGained);
			}


		}

		if (creature[c2].playerIndex!=0)
			player[creature[c2].playerIndex].sendStatsRefresh=true;

		// --------------- calculate weapon damge ---------------

		//    if (cr_RustDamage(c2,creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID))
		//      rustDamage=10;
		//    else
		//      rustDamage=1;

		//    for (j=0;j<rustDamage;j++)
		if (creature[c1].playerIndex!=0)  // player controlled
		{
			if (player[creature[c1].playerIndex].arm[kRightHand]!=0) // holding weapon
			{

				if (in_CanGroup(creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID)) // not weapon .. fighting with tool
				{
					if (tb_Rnd(1,8)==1)  // odds of armed non weapon being destroyed during combat - modified 0.1.3
					{

						nw_SendToClientDropFromInventorySlot(creature[c1].playerIndex,kDamageItem,player[creature[c1].playerIndex].arm[kRightHand],1, 0);

						if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount>0)
							creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount--;

						if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].desiredCount>0)
							in_ItemDestroyed(creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID, 1);

						if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount==0)
							creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID=0;

						player[creature[c1].playerIndex].arm[kRightHand]=0;
					}
				}
				else if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount>127)	// it's a GM weapon
				{
					if ((tb_Rnd(0,(itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].durability*2))==1) || (cr_RustDamage(c2,creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID)))
					{

						if (cr_RustDamage(c2,creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID))
							itemDamage=10;
						else
							itemDamage=1;

						if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount-itemDamage<127)
							itemDamage=creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount-127;

						nw_SendToClientDropFromInventorySlot(creature[c1].playerIndex,kDamageItem,player[creature[c1].playerIndex].arm[kRightHand],itemDamage, 0);

						if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount>0)
							creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount=creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount-itemDamage;

						if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount<=127)
						{
							if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].desiredCount>0)
								in_ItemDestroyed(creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID, 1);

							creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID=0;

							creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount=0;

							player[creature[c1].playerIndex].arm[kRightHand]=0;
						}

						it_CalculateArmorRating(creature[c1].playerIndex);
					}
				}
				else	// it's a regular weapon
				{

					if (cr_RustDamage(c2,creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID))
						itemDamage=10;
					else
						itemDamage=1;

					if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount-itemDamage<0)
						itemDamage=creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount;

					if ((tb_Rnd(0,itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].durability)==1) || (cr_RustDamage(c2,creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID)))
					{
						nw_SendToClientDropFromInventorySlot(creature[c1].playerIndex,kDamageItem,player[creature[c1].playerIndex].arm[kRightHand],itemDamage, 0);

						if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount>0)
							creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount=creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount-itemDamage;

						if (creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemCount==0)
						{
							if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].desiredCount>0)
								in_ItemDestroyed(creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID, 1);

							creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID=0;

							player[creature[c1].playerIndex].arm[kRightHand]=0;
						}

						it_CalculateArmorRating(creature[c1].playerIndex);
					}
				}

			}
		}

		// --------------- calculate armor damage ---------------

		//  if (creature[c1].id==kRustBeast)
		//    rustDamage=10;
		//  else
		//    rustDamage=1;

		//  for (j=0;j<rustDamage;j++)
		if (creature[c2].playerIndex!=0)
		{

			for (i=1;i<13;i++)
				if (player[creature[c2].playerIndex].arm[i]!=0)
					if (it_IsArmor(creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID, false))
					{

						if (creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount>127)	// it's a GM armor item - disappearing armor fix 0.1.3
						{
							if (((tb_Rnd(0,(itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID].durability*2))==1)) || (cr_RustDamage(c1,creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID)))
							{

								if (cr_RustDamage(c1,creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID))
									itemDamage=10;
								else
									itemDamage=1;

								if (creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount-itemDamage<127)
									itemDamage=creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount-127;

								nw_SendToClientDropFromInventorySlot(creature[c2].playerIndex,kDamageItem,player[creature[c2].playerIndex].arm[i],itemDamage, 0);

								if (creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount>itemDamage)
									creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount=creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount-itemDamage;

								if (creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount<=127)
								{
									if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID].desiredCount>0)
										in_ItemDestroyed(creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID, 1);

									creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID=0;

									creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount=0;

									player[creature[c2].playerIndex].arm[i]=0;
								}

								it_CalculateArmorRating(creature[c2].playerIndex);
							}
						}
						else	// it's a regular armor item
						{
							if ((tb_Rnd(0,itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID].durability)==1) || (cr_RustDamage(c1,creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID)))
							{

								if (cr_RustDamage(c1,creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID))
									itemDamage=10;
								else
									itemDamage=1;

								if (creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount-itemDamage<0)
									itemDamage=creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount;

								nw_SendToClientDropFromInventorySlot(creature[c2].playerIndex,kDamageItem,player[creature[c2].playerIndex].arm[i],itemDamage, 0);

								if (creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount>0)
									creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount=creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount-itemDamage;

								if (creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemCount==0)
								{
									if (itemList[creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID].desiredCount>0)
										in_ItemDestroyed(creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID, 1);

									creature[c2].inventory[player[creature[c2].playerIndex].arm[i]].itemID=0;

									player[creature[c2].playerIndex].arm[i]=0;
								}

								it_CalculateArmorRating(creature[c2].playerIndex);
							}
						}

					}

		}

		return (true);
	}

}

// ----------------------------------------------------------------- //
int cr_SpecialAttack(int c1, int c2, int param)
// ----------------------------------------------------------------- //
{
	
	
	
	//By calling this function we assume everything was (double) checked - we just need to deal with the attack and remove stamina
	unsigned long	currentTime=TickCount();
	int 			row, col, absorb;
		
				
	//Let's still paranoid double check stamina
	if (creature[c1].specialAttackArg2==0 || creature[c1].specialAttackArg2==3) //0 or 3 = pay on using
	{
		if (creature[c1].stamina<itemList[creature[c1].specialAttackType].price)
		{
			if (creature[c1].playerIndex!=0)
				nw_SendDisplayMessage(creature[c1].playerIndex, kNotEnoughStaminaMsg);
			
			return (0);
		}
		
		creature[c1].stamina-=itemList[creature[c1].specialAttackType].price;
		
		if (creature[c1].playerIndex!=0)
			player[creature[c1].playerIndex].sendStatsRefresh=true;
	}
	
	if (creature[c1].playerIndex!=0 && c2!=0)
	{
		switch (player[creature[c1].playerIndex].characterClass)
		{
			case 3:
				sk_TrainProof(c1, c2, 0, kSpecialFighter, 2);
			break;
			
			case 4:
				sk_TrainProof(c1, c2, 0, kSpecialRanger, 2);
			break;
			
			case 5:
				sk_TrainProof(c1, c2, 0, kSpecialRogue, 2);
			break;
		}
	}

	switch (creature[c1].specialAttackType)
	{
		case kAttackRage: //direct damage
			//Dave added
			if ((creature[c1].stamina<=0) || (creature[c1].stamina>=120))
				{
				creature[c1].stamina=0;
				}
			if (creature[c1].stamina<=5)
			{
				creature[c1].stamina<0; //Dave =0
				creature[c1].paralyzed=currentTime+60*5;
				nw_SendDisplayMessage(creature[c1].playerIndex, kExhausted);
				cr_ClearSpat(c1, true);
			}
			else
			{
				if (creature[c1].stamina<=5)
					{
						return;
					}
					else
						{
						creature[c1].stamina-=5;	
						}
					
			}
			
			
			
			player[creature[c1].playerIndex].sendStatsRefresh=true;
			
			return param*3;
		break;
		
		case kAttackKnockback: //direct damage
		
			cr_ClearSpat(c1, true);
			
			if (c2==0)
				return;
			
			fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
			
			switch (creature[c1].facing)
			{
				case 4://4
					row=creature[c2].row+1;
					col=creature[c2].col+1;
				break;
				
				case 5://5
					row=creature[c2].row+1;
					col=creature[c2].col;
				break;
				
				case 6://6
					row=creature[c2].row+1;
					col=creature[c2].col-1;
				break;
				
				case 7://7
					row=creature[c2].row;
					col=creature[c2].col-1;
				break;
				
				case 0://0
					row=creature[c2].row-1;
					col=creature[c2].col-1;
				break;
				
				case 1://1
					row=creature[c2].row-1;
					col=creature[c2].col;
				break;
				
				case 2://2
					row=creature[c2].row-1;
					col=creature[c2].col+1;
				break;
				
				case 3://3
					row=creature[c2].row;
					col=creature[c2].col+1;
				break;
			}
			
			
			
			
			if (creatureInfo[creature[c2].id].mobile && creatureInfo[creature[c2].id].size<3 && creatureMap[row][col]==0 && cr_InDomain(c2,row,col))
			{
				absorb=creature[c2].attackTarget;
				cr_CreatureMoveTo(c2, row, col);
				creature[c2].attackTarget=absorb;
				creature[c2].paralyzed=currentTime+150;
				
				if (creature[c1].playerIndex!=0)
					nw_SendDisplayMessage(creature[c1].playerIndex, kKnockback);
				
				if (creature[c2].playerIndex!=0)
					nw_SendDisplayMessage(creature[c2].playerIndex, kYouAreParalyzed);
			}
			else
			{
				nw_SendDisplayMessage(creature[c1].playerIndex, kHitMissed);
			}
				
			return (param+5);
			
		break;
		
		case kAttackSting: //direct damage
			cr_ClearSpat(c1, true);
			
			if (c2==0)
				return;
			
			if (!pl_BehindTargetServer(c1,c2))
			{
				nw_SendDisplayMessage(creature[c1].playerIndex, kMustBeBehindTarget);
				return;
			}
			
			if (creatureInfo[creature[c2].id].immuneToPoison==false)
				if (itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].poison)
				{
					if (creature[c2].poison+3<6)
						creature[c2].poison=creature[c2].poison+3;
					else
						creature[c2].poison=6;

						if (creature[c2].playerIndex!=0)
							nw_SendDisplayMessage(creature[c2].playerIndex,kYouHaveBeenPoisoned);
				}
						
			
			fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
			
			if (creature[c1].playerIndex!=0)
				nw_SendDisplayMessage(creature[c1].playerIndex, kBackstabDone0);
			
			return (param);
		break;
		
		  //Original
		case kAttackBackstab: //direct damage
			cr_ClearSpat(c1, true);
			
			if (c2==0)
				return;
					//Dave add stealth
				/*	//if (player[c1].stealth==false)
					if (player[creature[c1].playerIndex].stealth==false)
					
					{
					nw_SendDisplayMessage(creature[c1].playerIndex, kMustBeStealthed);
						cr_ClearSpat(c1, true);
						return;
					}
				
			if (pl_AuthorizedTo(c1, kRegularPlayer))
				{
					player[creature[c1].playerIndex].stealth		=	false;
					player[creature[c1].playerIndex].stealthCool	=	TickCount()+3*60;
					nw_SendDisplayMessage(creature[c1].playerIndex,kMustBeStealthed);
					cr_ClearSpat(c1, true);
					return;
				}
			*/
			if (!pl_BehindTargetServer(c1,c2))
			{
				nw_SendDisplayMessage(creature[c1].playerIndex, kMustBeBehindTarget);
				return;
			}
			
		
			if (creatureInfo[creature[c2].id].size<3)
				creature[c2].paralyzed=currentTime+120+60*(sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)/2);
			
			fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
			
			if (creature[c1].playerIndex!=0)
				nw_SendDisplayMessage(creature[c1].playerIndex, kBackstabDone1);
			
			if (creature[c2].playerIndex!=0)
				nw_SendDisplayMessage(creature[c2].playerIndex, kYouAreParalyzed);
			//Dave edit - level)*2.5f));
			return (param+tb_Round((float)sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)*1.5f));
		break;
	/*	
		
		case kAttackBackstab: //direct damage
			
			if (player[creature[c1].playerIndex].stealth)
				{
					cr_ClearSpat(c1, true);
					
					if (c2==0)
						return;
					
					if (!pl_BehindTargetServer(c1,c2))
					{
						nw_SendDisplayMessage(creature[c1].playerIndex, kMustBeBehindTarget);
						return;
					}
					
					if (creatureInfo[creature[c2].id].size<3)
						creature[c2].paralyzed=currentTime+100+60*(sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)/2);// Dave  currentTime+120+60*  - level)/2);
					
					fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
					
					if (creature[c1].playerIndex!=0)
						nw_SendDisplayMessage(creature[c1].playerIndex, kBackstabDone1);
					
					if (creature[c2].playerIndex!=0)
						nw_SendDisplayMessage(creature[c2].playerIndex, kYouAreParalyzed);
					//Dave edit - level)*2.5f));
					return (param+tb_Round((float)sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)*1.5f));
			
				}
				
			else
					{
						cr_ClearSpat(c1, true);
						nw_SendDisplayMessage(c1,kMustBeStealthed);
						return;
					}
		break;
		*/
		case kAttackDecimate: //direct damage
			cr_ClearSpat(c1, true);
			
			if (c2==0)
				return;
			
			if (!pl_BehindTargetServer(c1,c2))
			{
				nw_SendDisplayMessage(creature[c1].playerIndex, kMustBeBehindTarget);
				return;
			}
			/*
			if (player[c1].stealth==false)
			{
				//nw_SendDisplayMessage(creature[c1].playerIndex, kMustBeStealthed);
				gr_AddText("You must be stealthy before using this special attack.", false, true);
				return;
			}
			*/
			
			
			fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
			
			if (creatureInfo[creature[c2].id].size<3)
				creature[c2].paralyzed=currentTime+60+(sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)/3);
			
			if (creature[c1].playerIndex!=0)
				nw_SendDisplayMessage(creature[c1].playerIndex, kBackstabDone2);
			
			if (creature[c2].playerIndex!=0)
				nw_SendDisplayMessage(creature[c2].playerIndex, kYouAreParalyzed);
			
			return (param+tb_Round((float)sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kAnatomy].level)*3.5f));
		break;
		
		case kAttackTaunt: //direct damage
			cr_ClearSpat(c1, true);
			
			if (c2==0)
				return;
			
			fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
			creature[c2].attackTarget=c1;
			creature[c2].taunted=currentTime+120;
			
			if (creature[c1].playerIndex!=0)
				nw_SendDisplayMessage(creature[c1].playerIndex, kTaunt);
			
			return param;
		break;
		
		case kAttackBoomingBlow: //direct damage
			cr_ClearSpat(c1, true);
			if (c2==0)
				return;
			cr_AddSound(c2, kExplosionSnd);
			fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
			
			if (creature[c1].playerIndex!=0)
				nw_SendDisplayMessage(creature[c1].playerIndex, kBoomingBlow);
			
			return (param*2);
		break;
		
		case kAttackEscapeMove: //direct damage
			cr_ClearSpat(c1, true);
			if (c2==0)
				return;
			fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
			if (creatureInfo[creature[c2].id].mobile && creatureInfo[creature[c2].id].size<3)
			{
				creature[c2].paralyzed=currentTime+(tb_Rnd(3,5)*60);
				
				if (creature[c1].playerIndex!=0)
					nw_SendDisplayMessage(creature[c1].playerIndex, kEscapeMove);
				
				if (creature[c2].playerIndex!=0)
					nw_SendDisplayMessage(creature[c2].playerIndex, kYouAreParalyzed);
					
				return 2;
			}
			else
			{
				nw_SendDisplayMessage(creature[c1].playerIndex, kHitMissed);
				return 0;
			}
			
		break;
		
		case kAttackAimedShot: //direct damage
			cr_ClearSpat(c1, true);
			if (c2==0)
				return;
			fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
			
			if (creature[c1].playerIndex!=0)
				nw_SendDisplayMessage(creature[c1].playerIndex, kAimedShot);
			
			return (param*2+sk_GetSkillLevel(player[creature[c1].playerIndex].skill[kArchery].level));
		break;
		
		case kAttackGrapplingShot: //direct damage

			cr_ClearSpat(c1, true);
			if (c2==0)
				return;
			if (creatureInfo[creature[c2].id].mobile && creatureInfo[creature[c2].id].size<2)
			{
				switch (creature[c1].facing)
				{
					case 0:
						row=creature[c1].row-2;
						col=creature[c1].col-2;
					break;
					
					case 1:
						row=creature[c1].row-2;
						col=creature[c1].col;
					break;
					
					case 2:
						row=creature[c1].row-2;
						col=creature[c1].col+2;
					break;
					
					case 3:
						row=creature[c1].row;
						col=creature[c1].col+2;
					break;
					
					case 4:
						row=creature[c1].row+2;
						col=creature[c1].col+2;
					break;
					
					case 5:
						row=creature[c1].row+2;
						col=creature[c1].col;
					break;
					
					case 6:
						row=creature[c1].row+2;
						col=creature[c1].col-2;
					break;
					
					case 7:
						row=creature[c1].row;
						col=creature[c1].col-2;
					break;
				}
				
				if (creatureMap[row][col]==0 && cr_InDomain(c2,row,col))
				{
					cr_CreatureMoveTo(c2, row, col);
					
					if (creature[c1].playerIndex!=0)
						nw_SendDisplayMessage(creature[c1].playerIndex, kGrapplingShot);
					
					return 0;
				}
			}
			
			nw_SendDisplayMessage(creature[c1].playerIndex, kHitMissed);
			return 0;
		break;
		
		case kAttackDoubleStrike: //attack speed
		case kAttackDoubleShot:
			cr_ClearSpat(c1, true);
			fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
			return 80;
		break;
		
		case kAttackRangerSwiftness: //attack speed
			if (creature[c1].stamina<=5)
			{
				creature[c1].stamina=0;
				cr_ClearSpat(c1, true);
				nw_SendDisplayMessage(creature[c1].playerIndex, kExhausted);
			}
			else
			{
				creature[c1].stamina-=5;
			}
			
			player[creature[c1].playerIndex].sendStatsRefresh=true;
			return param/2;			
		break;
		
		case kAttackLesserDefense: //damage absorb
			absorb=param/4;
			
			if (creature[c1].stamina<absorb)
			{
				absorb=creature[c1].stamina;
				creature[c1].stamina=0;
				cr_ClearSpat(c1, true);
				nw_SendDisplayMessage(creature[c1].playerIndex, kDefenseEnd);
			}
			else
			{
				creature[c1].stamina-=absorb;
			}
			
			player[creature[c1].playerIndex].sendStatsRefresh=true;
			return param-absorb;
		break;
		
		case kAttackGreaterDefense:
			absorb=param/2;
			
			if (creature[c1].stamina<absorb/2)
			{
				absorb=creature[c1].stamina*2;
				creature[c1].stamina=0;
				cr_ClearSpat(c1, true);
				nw_SendDisplayMessage(creature[c1].playerIndex, kDefenseEnd);
			}
			else
			{
				creature[c1].stamina-=absorb/2;
			}
			
			player[creature[c1].playerIndex].sendStatsRefresh=true;
			return param-absorb;
		break;
	}
	
	return 0;
}

// ----------------------------------------------------------------- //
void cr_ClearSpat(int c, TBoolean send)
// ----------------------------------------------------------------- //
{
	creature[c].specialAttackType=-1;
	creature[c].specialAttackArg1=-1;
	creature[c].specialAttackArg2=-1;
	
	if (send && creature[c].playerIndex!=0)	nw_SendClearSpat(creature[c].playerIndex);
}

// ----------------------------------------------------------------- //
void cr_CheckForPetGuarding(int c2, int c1)
// ----------------------------------------------------------------- //

{
	int         i;

	if (creature[c2].playerIndex!=0)
		if (player[c2].dead==false)
			for (i=0;i<kMaxPets;i++) // 0.1.4
				if (player[creature[c2].playerIndex].pet[i]!=0)
					if (player[creature[c2].playerIndex].petInStable[i]==false)
						if (creature[player[creature[c2].playerIndex].pet[i]].command==kGuard)
							if (creature[player[creature[c2].playerIndex].pet[i]].attackTarget==0)
								creature[player[creature[c2].playerIndex].pet[i]].possibleTarget	=	c1;

}

// ----------------------------------------------------------------- //
int cr_MinimumDistanceToHuman(int r, int c)
// ----------------------------------------------------------------- //

{
	int         firstRow;
	int         lastRow;
	int         firstCol;
	int         lastCol;
	int         row;
	int         col;
	int         dist;
	int         minDist;

	firstRow  = r-10;
	lastRow   = r+10;
	firstCol  = c-10;
	lastCol   = c+10;

	if (firstRow<0)            firstRow=0;

	if (lastRow>=kMapWidth)    lastRow=kMapWidth-1;

	if (firstCol<0)            firstCol=0;

	if (lastCol>=kMapHeight)   lastCol=kMapHeight-1;

	dist=0;

	minDist=9999;

	for (row=firstRow;row<=lastRow;row++)
		for (col=firstCol;col<=lastCol;col++)
			if ((row>0) && (row<kMapWidth) && (col>0) && (col<kMapHeight))
				if (creatureMap[row][col]!=0)
					if ((creature[creatureMap[row][col]].playerIndex!=0) || (creature[creatureMap[row][col]].master!=0))
					{
						dist=cr_RealDistanceFrom(creatureMap[row][col], r, c);

						if (dist<minDist)
							minDist=dist;
					}

	return (minDist);

}

// ----------------------------------------------------------------- //
void cr_GetCreatureRetreat(int c, int *fleeRow, int *fleeCol)
// ----------------------------------------------------------------- //

{
	int         firstRow;
	int         lastRow;
	int         firstCol;
	int         lastCol;
	int         row;
	int         col;
	int         minDist=0;
	//int         maxDist;
	int         dist;

	firstRow=creature[c].row-1;
	lastRow=creature[c].row+1;
	firstCol=creature[c].col-1;
	lastCol=creature[c].col+1;

	if (firstRow<0)            firstRow=0;

	if (lastRow>=kMapWidth)    lastRow=kMapWidth-1;

	if (firstCol<0)            firstCol=0;

	if (lastCol>=kMapHeight)   lastCol=kMapHeight-1;

	minDist=cr_MinimumDistanceToHuman(creature[c].row,creature[c].col);

	if (minDist<9999999)
		for (row=firstRow;row<=lastRow;row++)
			for (col=firstCol;col<=lastCol;col++)
				if ((row>0) && (row<kMapWidth) && (col>0) && (col<kMapHeight))
					if (cr_InDomain(c, row, col))
					{
						dist=cr_MinimumDistanceToHuman(row,col);

						if (dist>minDist)
						{
							*fleeRow=row;
							*fleeCol=col;
							return;
						}
					}

	*fleeRow=0;

	*fleeCol=0;

}

// ----------------------------------------------------------------- //
void cr_GetSpotFarthestAway(int c1, int c2, int *fleeRow, int *fleeCol)
// ----------------------------------------------------------------- //

{
	int         firstRow;
	int         lastRow;
	int         firstCol;
	int         lastCol;
	int         maxDist;
	int         r;
	int         c;
	int         row;
	int         col;
	//int         d1;
	int         d2;
	int         d;
	//int         currentDistance;

	firstRow=creature[c1].row-4;
	lastRow=creature[c1].row+4;
	firstCol=creature[c1].col-4;
	lastCol=creature[c1].col+4;

	if (firstRow<0)            firstRow=0;

	if (lastRow>=kMapWidth)    lastRow=kMapWidth-1;

	if (firstCol<0)            firstCol=0;

	if (lastCol>=kMapHeight)   lastCol=kMapHeight-1;

	maxDist=cr_RealDistanceFrom(c2,creature[c1].row,creature[c1].col);

	r=0;

	c=0;

	//currentDistance=cr_RealDistanceFrom(c2,creature[c1].row,creature[c1].col);
	for (row=firstRow;row<=lastRow;row++)
		for (col=firstCol;col<=lastCol;col++)
			if (cr_ObstacleBetween2(row, col, creature[c1].row, creature[c1].col, creatureInfo[creature[c1].id].domain, &d, c1, c2, (cr_ImmuneToFF(creature[c1].id) || creatureInfo[creature[c1].id].fire))==false)
			{
				//d1=cr_RealDistanceFrom(c1,row, col);
				d2=cr_RealDistanceFrom(c2,row, col);

				if (d2>maxDist)
				{
					maxDist=d2;
					r=row;
					c=col;
				}
			}

	*fleeRow=r;

	*fleeCol=c;

}

// ----------------------------------------------------------------- //
void cr_CheckForGuardingPets(int  c2, int c1)
// ----------------------------------------------------------------- //

{
	int                 i;

	if (creature[c2].playerIndex!=0)  // check for pets guarding
		if (player[c2].dead==false)
			for (i=0;i<kMaxPets;i++)
				if (player[creature[c2].playerIndex].pet[i]!=0)
					if (player[creature[c2].playerIndex].petInStable[i]==false)
						if (creature[player[creature[c2].playerIndex].pet[i]].command==kGuard)
							if (creature[player[creature[c2].playerIndex].pet[i]].attackTarget==0)
								if (creature[player[creature[c2].playerIndex].pet[i]].possibleTarget==0)
									creature[player[creature[c2].playerIndex].pet[i]].possibleTarget	=	c1;

}

// ----------------------------------------------------------------- //
void cr_MoveCreatures(void)
// ----------------------------------------------------------------- //

{
	int						i;
	int						j;
	//int						k;
	UInt16					row;
	UInt16					col;
	unsigned long			currentTime=tb_CurrentTick()+1;
	unsigned long			talkExpireTime=currentTime-(8*60);
	int						rowDir;
	int						colDir;
	TBoolean				doMove;
	int						modifier=0;
	TBoolean				forceMove;	// position same, but state change ... ex: turtle surfacing
	TBoolean				inCombat=false;
	//TBoolean				onTrail;
	//int						dRow;
	//int						dCol;
	//TBoolean				foundShortcut;
	//TBoolean				sibling;
	TBoolean				stepOnLatter;
	int						raiseChance;
	int						poison;
	//int                 	dropped;
	//int                 	tryRow;
	//int                 	tryCol;
	int						dist;
	int                 	crow;
	int                 	ccol;
	int                 	nrow;
	int                 	ncol;
	int                 	rowDist;
	int                 	colDist;
	int                 	rowDist2;
	int                 	colDist2;
	int                 	dist2;
	TBoolean            	outOfRange;
	TBoolean            	clericHoldingWeapon;
	int                 	theRow;
	int                 	theCol;
	TBoolean            	tracking;
	int                 	stepRow;
	int                 	stepCol;
	TBoolean            	attacking;
	TBoolean            	running;
	TBoolean				spellcasterFlying;
	int                 	fleeRow;
	int                 	fleeCol;
	TBoolean				followingMaster;
	TBoolean            	followMaster;
	UInt16              	petRow;
	UInt16              	petCol;
	int                 	firstRow;
	int                 	lastRow;
	int                 	firstCol;
	int                 	lastCol;
	int                 	scanRow;
	int                 	scanCol;
	int						humansInArea;
	TBoolean				farFromHome;
	TBoolean				canReveal;
	TBoolean				stinks;
	int						staminaRegen;
	int						staminaStep;
	int						ringdamage;
//	int						checkTime;
//	int 					wearingPlate;


#ifdef _FUNCTIONTRACEDEBUGGING
	fi_WriteToErrorLog("cr_MoveCreatures Entry");
#endif

	for (i=0;i<kMaxBloodStains;i++)
		if (bloodStain[i].id!=0)
			if (currentTime>bloodStain[i].expireTime)
				cr_ClearBloodStain(i);

	for (i=1;i<kMaxPlayers;i++)	// check for players attacking --> 1..5000 reserved for players
		if (creature[i].playerIndex!=0)
			creature[i].beingAttacked=0;


	for (i=1;i<kMaxPlayers;i++)	// check for players attacking --> 1..5000 reserved for players
		if (creature[i].playerIndex!=0)
		{
			if (creature[i].talkTime>0)	// ------ check for talk erase time
				if (talkExpireTime>creature[i].talkTime)
				{
					creature[i].talkWhen			= 0;
					creature[i].talkTime			= 0;
					creature[i].talkID			  = 0;
					strcpy(creature[i].talk,"");
				}

			humansInArea=0;

			firstRow=creature[i].row-6;
			lastRow=creature[i].row+6;
			firstCol=creature[i].col-6;
			lastCol=creature[i].col+6;

			if (firstRow<0)            			firstRow=1;
			else if (lastRow>=kMapWidth) 	lastRow=kMapWidth-1;

			if (firstCol<0)            			firstCol=1;
			else if (lastCol>=kMapHeight) lastCol=kMapHeight-1;

			for (scanRow=firstRow;scanRow<lastRow;scanRow++)
				for (scanCol=firstCol;scanCol<lastCol;scanCol++)
					if (creatureMap[scanRow][scanCol]!=0)
						if (creature[creatureMap[scanRow][scanCol]].id==kHuman)
							if (pl_AuthorizedTo(creatureMap[scanRow][scanCol], kRegularPlayer))
								humansInArea++;

			if (humansInArea==1)
				player[creature[i].playerIndex].maxAttackers=5;
			else if ((humansInArea==2) || (humansInArea==3))
				player[creature[i].playerIndex].maxAttackers=3;
			else
				player[creature[i].playerIndex].maxAttackers=2;
			
			player[creature[i].playerIndex].maxAttackers=5; //change 018.4

			/*if ((player[creature[i].playerIndex].lastMove+(3600*40))<currentTime)	// ------ check for move inactivity
			  if (pl_AuthorizedTo(i,kRegularPlayer)) // put in carbon 11/27
				pl_SendMacroWarning(creature[i].playerIndex);*/

			pl_CalculatePetLoyalty(creature[i].playerIndex);

			if (creature[i].soundFX!=0)
				if ((creature[i].soundFXTime+300)<currentTime)	// kill sound after 2 seconds
				{
					creature[i].soundFX			        =	0;
					creature[i].soundFXTime	        =	0;
					creature[i].soundID             = 0;
				}

			if (player[creature[i].playerIndex].turnUntil!=0)
				if (currentTime>player[creature[i].playerIndex].turnUntil)
					player[creature[i].playerIndex].turnUntil=0;

			if (creature[i].dieTime!=0)
				if (currentTime>creature[i].dieTime)
				{
					cr_Dead(i);
					continue;
				}

			if ((creature[i].pathRow[creature[i].pathPtr]!=0) && (currentTime>creature[i].nextMoveTime))
			{
				crow=creature[i].row;
				ccol=creature[i].col;
				nrow=creature[i].pathRow[creature[i].pathPtr];
				ncol=creature[i].pathCol[creature[i].pathPtr];

				if ((crow==nrow) && (ccol==ncol))
				{
					creature[i].pathPtr=0;

					for (j=0;j<kMaxPath;j++)
					{
						creature[i].pathRow[j]	=	0;
						creature[i].pathCol[j]	=	0;
					}
				}
				else
				{
					if ((crow<nrow) && (ccol==ncol))
						if (pl_PlayerMove(creature[i].playerIndex, 1))
							creature[i].pathPtr++;

					if ((crow>nrow) && (ccol==ncol))
						if (pl_PlayerMove(creature[i].playerIndex, 2))
							creature[i].pathPtr++;

					if ((crow==nrow) && (ccol>ncol))
						if (pl_PlayerMove(creature[i].playerIndex, 3))
							creature[i].pathPtr++;

					if ((crow==nrow) && (ccol<ncol))
						if (pl_PlayerMove(creature[i].playerIndex, 4))
							creature[i].pathPtr++;

					if ((crow<nrow) && (ccol<ncol))
						if (pl_PlayerMove(creature[i].playerIndex, 5))
							creature[i].pathPtr++;

					if ((crow<nrow) && (ccol>ncol))
						if (pl_PlayerMove(creature[i].playerIndex, 7))
							creature[i].pathPtr++;

					if ((crow>nrow) && (ccol<ncol))
						if (pl_PlayerMove(creature[i].playerIndex, 6))
							creature[i].pathPtr++;

					if ((crow>nrow) && (ccol>ncol))
						if (pl_PlayerMove(creature[i].playerIndex, 8))
							creature[i].pathPtr++;

			
					
					creature[i].nextMoveTime	=	currentTime+30;

					if (player[creature[i].playerIndex].speed>0)
						creature[i].nextMoveTime -=	player[creature[i].playerIndex].speed;
					
					if (in_ItemEquipedServer(creature[i].playerIndex, kCharmOfSwiftness))
						creature[i].nextMoveTime -= 5;

					if (player[creature[i].playerIndex].meditating)
					{
						player[creature[i].playerIndex].meditating=false;
						nw_SendDisplayMessage(creature[i].playerIndex, kConcentrationDisturbed);
					}
					
//-----------------------ARMOR RESTRICTIONS--------------------------------------------------
	/*
					if (player[creature[i].playerIndex].characterClass!=1)
						if (in_ItemEquipedServer(creature[i].playerIndex, (kBrownBreastLeather || kBrownLeatherLegs)))
							//creature[i].nextMoveTime -= 5;
						creature[i].nextMoveTime += 10;
					
					if (player[creature[i].playerIndex].characterClass!=1)
						if (in_ItemEquipedServer(creature[i].playerIndex, (kChainGauntlets || kChainArms || kChainLegs || kBreastChain || kChainMailHelmet)))
							creature[i].nextMoveTime	=	currentTime+2*60;
							//creature[i].nextMoveTime += 15;
						
					if (player[creature[i].playerIndex].characterClass!=1)
						if (in_ItemEquipedServer(creature[i].playerIndex, (kClosedPlateHelmet || kSlottedPlateHelmet || kPlateGauntlets || kPlateArms ||
								 kLegsPlate || kBreastPlate || kOpenPlateHelmet || kVikingHelmet)))
							creature[i].nextMoveTime	=	currentTime+3*60;
						 
*/
//-------------------------------------------------------------------------------------------		
					
					
					/* //Dave
					if (pl_AuthorizedTo(i, kRegularPlayer) && player[creature[i].playerIndex].stealth)
					{
						dist=0;
						
						if (in_ItemEquipedServer(creature[i].playerIndex, kRingOfShadows))			dist=3;
						else if (in_ItemEquipedServer(creature[i].playerIndex, kRingOfStealth))		dist=2;
						else if (in_ItemEquipedServer(creature[i].playerIndex, kDiscretionRing))	dist=1;
						
						if (player[creature[i].playerIndex].stealthMoves+1>=sk_GetSkillLevel(player[creature[i].playerIndex].skill[kStealth].level)+dist) //Dave  if (player[creature[i].playerIndex].stealthMoves+1>=sk_GetSkillLevel(player[creature[i].playerIndex].skill[kStealth].level)+dist)  
						{
							player[creature[i].playerIndex].stealth=false;
							player[creature[i].playerIndex].stealthCool=TickCount()+3*60;
							nw_SendDisplayMessage(i, kLeavingStealth);
							player[creature[i].playerIndex].stealthMoves=0;
						}
						else
							player[creature[i].playerIndex].stealthMoves++;
					}
					*/ //Dave^ edited below, original above
			
			
				
				/* Above unedited ^
				
				int total_num_moves = sk_GetSkillLevel((player[creature[i].playerIndex].skill[kStealth].level)<3)	dist=tb_Rnd(0,0) && (player[creature[i].playerIndex].skill[kStealth].level)<6)	count=tb_Rnd(1,4) &&  (player[creature[i].playerIndex].skill[kStealth].level)<10) count=tb_Rnd(2,5));
				
				int player_stealth_moves = player[creature[i].playerIndex].stealthMoves-count;
				//try one ^
				
				int total_num_moves = sk_GetSkillLevel(player[creature[i].playerIndex].skill[kStealth].level);
				
				int player_stealth_moves = player[creature[i].playerIndex].stealthMoves;
				
				dist=0;
				
				
				//Dave class level restrictions
				if (sk_GetSkillLevel(player[i].skill[kStealth].level)<3)			dist=0;
					else if (sk_GetSkillLevel(player[i].skill[kStealth].level)<6)	dist=dist-tb_Rnd(1,3);
					else if (sk_GetSkillLevel(player[i].skill[kStealth].level)<10)	dist=dist-tb_Rnd(2,5);
					else 
						dist=0;	
							
							//Dave^ 
				
					
					placeholder*/	

//------------------------------------STEALTH---------------------------------------------------------------
		
			if (pl_AuthorizedTo(i, kRegularPlayer) && player[creature[i].playerIndex].stealth)
			{
				//all declaration top of block, variable unset when block exists, for both
				int total_num_moves = sk_GetSkillLevel(player[creature[i].playerIndex].skill[kStealth].level);
				
				int player_stealth_moves = player[creature[i].playerIndex].stealthMoves;
				
				dist=0;
					//Dave armor restriction for rogues
					if 	   (in_ItemEquipedServer(creature[i].playerIndex, (kGrayRobeProtection || kLightBlueRobeProtection || kRedRobeProtection || kBlueRobeProtection ||
							kBrownRobeProtection || kBlackRobeProtection || kGreenRobeProtection || kPurpleRobeProtection || kYellowRobeProtection || kGrayRobeResistance ||
							kLightBlueRobeResistance || kRedRobeResistance || kBlueRobeResistance || kBrownRobeResistance || KBlackRobeResistance || kGreenRobeResistance ||
							kPurpleRobeResistance || kYellowRobeResistance || kRoyalRobe)))
							dist=dist-2;
							
							
							
					
					if 	    (in_ItemEquipedServer(creature[i].playerIndex, (kBrownBreastLeather || kBrownLeatherLegs || kBrownLeatherArms)))
							//(in_ItemEquipedServer(creature[i].playerIndex, kWoodenShield)) ||
							//(in_ItemEquipedServer(creature[i].playerIndex, kMedalShield)) ||
							//(in_ItemEquipedServer(creature[i].playerIndex, kBlackMetalShield))
							dist=dist-1;
					
					if 	   ((in_ItemEquipedServer(creature[i].playerIndex, kChainGauntlets)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kChainArms)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kChainLegs)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBreastChain)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kChainMailHelmet)))
							dist=dist-2;
					
					if 	   ((in_ItemEquipedServer(creature[i].playerIndex, kBlackBreastPlate)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackLegsPlate)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackVikingHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackClosedPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackSlottedPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackOpenPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackPlateGauntlets)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackPlateArms)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kVikingHelmet)))
							dist=dist-7;
					
					if 	   ((in_ItemEquipedServer(creature[i].playerIndex, kClosedPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kSlottedPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kPlateGauntlets)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kPlateArms)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kLegsPlate)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBreastPlate)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kOpenPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kVikingHelmet)))
							dist=dist-10;
							
					
							
					if (in_ItemEquipedServer(creature[i].playerIndex, kRingOfShadows))					dist=dist+3;
						else if (in_ItemEquipedServer(creature[i].playerIndex, kRingOfStealth))			dist=dist+2;
						 	else if (in_ItemEquipedServer(creature[i].playerIndex, kDiscretionRing))	dist=dist+1;
						
					
					//if (dist<0) dist=1;
						
						//Dave^
					//Dave
					total_num_moves+=dist; //add dist
					if (player_stealth_moves+1+tb_Rnd(0,2)-1 >= total_num_moves)
					//if (player_stealth_moves+(tb_Rnd(0,2)-1) >= total_num_moves)   
					//if (player_stealth_moves+3 >= total_num_moves)   
					//if (player[creature[i].playerIndex].stealthMoves+1>=sk_GetSkillLevel(player[creature[i].playerIndex].skill[kStealth].level)+dist)  
						{
							//int tmp_shog_rnd = tb_Rnd(1,10);
							//char tmp_thing [150];
							//sprintf (tmp_thing,"orig random was: %d and player stealth is: %d and total_num_moves is %d",tmp_shog_rnd,player_stealth_moves+5,total_num_moves);
							//fi_WriteToErrorLog(tmp_thing);

							//check if this is the move we _should_ be unstealthed regardless
							//if (player_stealth_moves+1>=total_num_moves)
							if (player_stealth_moves+1>=total_num_moves)
								{
								//we are unstealthed
								player[creature[i].playerIndex].stealth=false;
								player[creature[i].playerIndex].stealthCool=TickCount()+3*60;
								nw_SendDisplayMessage(i, kLeavingStealth);
								player[creature[i].playerIndex].stealthMoves=0;
								}
							  	//half chance last X moves
							else if (tb_Rnd(0,10)>=6) //1,2 = 50% chance
								//we are ok
								{
								 
								player[creature[i].playerIndex].stealthMoves++;
							//	fi_WriteToErrorLog("Stealthed ok");
								
								}
							else
							{
								//sprintf (tmp_thing,"Mean random was: %d",tmp_shog_rnd);
								//fi_WriteToErrorLog(tmp_thing);
								//we are unstealthed
								player[creature[i].playerIndex].stealth=false;
								player[creature[i].playerIndex].stealthCool=TickCount()+3*60;
								nw_SendDisplayMessage(i, kLeavingStealth);
								player[creature[i].playerIndex].stealthMoves=0;
							}
						}
						else
							player[creature[i].playerIndex].stealthMoves++;
					}
			
					//creature[i].nextMoveTime	=	currentTime+29; // small move step test

					if (creature[i].pathPtr>=kMaxPath)
					{
						creature[i].pathPtr=0;

						for (j=0;j<kMaxPath;j++)
						{
							creature[i].pathRow[j]	=	0;
							creature[i].pathCol[j]	=	0;
						}
					}
				}
			}

//--------------------------------------------------------------------------
			if (currentTime>player[creature[i].playerIndex].saveAt)
			{
				fi_SavePlayerData(creature[i].playerIndex);
				player[creature[i].playerIndex].saveAt = TickCount()+(3600*kPlayerSaveTime);
			}

			if (player[creature[i].playerIndex].dead==false)
			{
				if (creature[i].strengthTime!=0)
					if (currentTime>creature[i].strengthTime)
					{
						creature[i].strengthTime=0;
						creature[i].strengthOffset=0;
						player[creature[i].playerIndex].sendStatsRefresh=true;
					}

				if (creature[i].intelligenceTime!=0)
					if (currentTime>creature[i].intelligenceTime)
					{
						creature[i].intelligenceTime=0;
						creature[i].intelligenceOffset=0;
						player[creature[i].playerIndex].sendStatsRefresh=true;
					}

				if (creature[i].dexterityTime!=0)
					if (currentTime>creature[i].dexterityTime)
					{
						creature[i].dexterityTime=0;
						creature[i].dexterityOffset=0;
						player[creature[i].playerIndex].sendStatsRefresh=true;
					}

				if (creature[i].opportunityTime!=0)
					if (currentTime>creature[i].opportunityTime)
						creature[i].opportunityTime=0;
				
				if (player[creature[i].playerIndex].morphTime!=0)
					if (currentTime>player[creature[i].playerIndex].morphTime)
					{
						player[creature[i].playerIndex].morphTime=0;
						player[creature[i].playerIndex].morph=0;
						
						nw_SendClearName(creature[i].playerIndex);
					}
					
				if (currentTime>creature[i].nextStatTime)
				{
		// ------------------------------ raise magic points
					clericHoldingWeapon=false;

					if (player[creature[i].playerIndex].characterClass==1)
					{
						if (player[creature[i].playerIndex].arm[kRightHand]!=0)
							if (it_IsWeapon(creature[i].inventory[player[creature[i].playerIndex].arm[kRightHand]].itemID))
								clericHoldingWeapon=true;

						if (player[creature[i].playerIndex].arm[kLeftHand]!=0)
							if (it_IsWeapon(creature[i].inventory[player[creature[i].playerIndex].arm[kLeftHand]].itemID))
								clericHoldingWeapon=true;
					}

	//----------------------------------stamina regen
					
	/*			
	//Dave 03-29-2012
	//Dave armor restriction  
	wearingPlate=0;
		if (player[creature[i].playerIndex].characterClass==4)
				if (player[creature[i].playerIndex].characterClass==5)
					
					if 	   ((in_ItemEquipedServer(creature[i].playerIndex, kClosedPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kSlottedPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kPlateGauntlets)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kPlateArms)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kLegsPlate)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBreastPlate)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kOpenPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kVikingHelmet)) ||
							
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackBreastPlate)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackLegsPlate)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackVikingHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackClosedPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackSlottedPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackOpenPlateHelmet)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackPlateGauntlets)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kBlackPlateArms)) || 
							(in_ItemEquipedServer(creature[i].playerIndex, kVikingHelmet)))
							wearingPlate=1;
							
*/
					
					
					staminaRegen=0;
					staminaStep=creature[i].dexterity/60;
					
					//Dave
					//if (wearingPlate==1)
						//staminaStep=staminaStep-10;
					
					if (staminaStep<=0)	staminaStep=1;
					
						
					if (creature[i].nextMoveTime<currentTime+60*3) //not running
						staminaRegen+=staminaStep;
					
					if (creature[i].attackTarget==0) //not attacking
					{
						staminaRegen+=staminaStep;
					}
					else
					{
						staminaRegen=0;	
						
						if (player[creature[i].playerIndex].characterClass==5 || pl_AuthorizedTo(i, kIsAnyGM)) //rogue edge
							if (tb_Rnd(1,3)<3)
								staminaRegen+=staminaStep;
					}
					
					if (creature[i].specialAttackType!=-1)
					{
						staminaRegen=0;
						
						//deal with stamina over time consuming spats..
						if (creature[i].specialAttackType==kAttackRage && tb_Rnd(1,2)==1)
						{
							if (creature[i].stamina==1) 
							{
								creature[i].stamina<5; //Dave stamina=0;
								creature[i].paralyzed=TickCount()+60*5;
								nw_SendDisplayMessage(creature[i].playerIndex, kExhausted);
								cr_ClearSpat(i, true);
							}
							else
							{
								creature[i].stamina--;
							}
						}
					}
					
					if (creature[i].stamina<creature[i].dexterity)
					{
						if (creature[i].godMode)	staminaRegen+=15;
						
						creature[i].stamina+=staminaRegen;
						
						if (creature[i].stamina+staminaRegen>creature[i].dexterity) 
							creature[i].stamina=creature[i].dexterity;
						
						player[creature[i].playerIndex].sendStatsRefresh=true;
					}
					
					//Dave  ring damage
					/* timer test
					if (creature[i].playerIndex!=0)
						{
					
						player[creature[i].playerIndex].wearTime=currentTime+60*5;
						}
						
						
					if (player[creature[i].playerIndex].wearTime==0)
					*/
					if ((creature[i].stamina==1) ||  (creature[i].stamina==10) ||  (creature[i].stamina==20) ||  (creature[i].stamina==30)) 
							if (player[creature[i].playerIndex].arm[kRightFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID==kRingOfBruteStrength)
							{
								
								ringdamage=1;
							
								if (tb_Rnd(1,10)<5)
								{
									
									
									nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kRightFinger],ringdamage, 0);

									if (creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount<=ringdamage)
									{
										if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID].desiredCount>0)
											in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID, 1);

										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID=0;

										player[creature[i].playerIndex].arm[kRightFinger]=0;
										
									}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount-ringdamage;
								}
								
								player[creature[i].playerIndex].sendStatsRefresh=true;
								creature[i].stamina-1;
							}
							
						if ((creature[i].stamina==1) ||  (creature[i].stamina==10) ||  (creature[i].stamina==20) ||  (creature[i].stamina==30)) 
							if (player[creature[i].playerIndex].arm[kLeftFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID==kRingOfBruteStrength)
							{
								ringdamage=1;
							
								if (tb_Rnd(1,10)<5)
								{
									
									
									nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kLeftFinger],ringdamage, 0);

									if (creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount<=ringdamage)
									{
										if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID].desiredCount>0)
											in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID, 1);

										creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID=0;

										player[creature[i].playerIndex].arm[kLeftFinger]=0;
									}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount-ringdamage;
								}

								player[creature[i].playerIndex].sendStatsRefresh=true;
								creature[i].stamina-1;
							}
							
							//strength ring
							if ((creature[i].stamina==1) ||  (creature[i].stamina==10) ||  (creature[i].stamina==20) ||  (creature[i].stamina==30))  
							if (player[creature[i].playerIndex].arm[kRightFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID==kRingOfStrength)
							{
								
								ringdamage=1;
							
								if (tb_Rnd(1,20)<2)
								{
									
									
									nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kRightFinger],ringdamage, 0);

									if (creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount<=ringdamage)
									{
										if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID].desiredCount>0)
											in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID, 1);

										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID=0;

										player[creature[i].playerIndex].arm[kRightFinger]=0;
									}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount-ringdamage;
								}
								
								player[creature[i].playerIndex].sendStatsRefresh=true;
								creature[i].stamina-1;
							}
							
						if ((creature[i].stamina==1) ||  (creature[i].stamina==10) ||  (creature[i].stamina==20) ||  (creature[i].stamina==30)) 
							if (player[creature[i].playerIndex].arm[kLeftFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID==kRingOfStrength)
							{
								ringdamage=1;
							
								if (tb_Rnd(1,20)<2)
								{
									
									
									nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kLeftFinger],ringdamage, 0);

									if (creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount<=ringdamage)
									{
										if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID].desiredCount>0)
											in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID, 1);

										creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID=0;

										player[creature[i].playerIndex].arm[kLeftFinger]=0;
									}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount-ringdamage;
								}

								player[creature[i].playerIndex].sendStatsRefresh=true;
								creature[i].stamina-1;
							}

//Stealth rings----------------------------------------------------------------------------------
					if (player[creature[i].playerIndex].arm[kRightFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID==kRingOfShadows)
						if (pl_AuthorizedTo(i, kRegularPlayer) && player[creature[i].playerIndex].stealth)
							if (creature[i].nextMoveTime>currentTime) 
								if (tb_Rnd(1,20)<=2)
									{
								
									if (tb_Rnd(1,10)<=2)
										{
										ringdamage=1;
										nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kRightFinger],ringdamage, 0);

											if (creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount<=ringdamage)
												{
													if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID].desiredCount>0)
													in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID, 1);
													
													creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID=0;
													
													player[creature[i].playerIndex].arm[kRightFinger]=0;
												}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount-ringdamage;
										}
								
								player[creature[i].playerIndex].sendStatsRefresh=true;
									}
							
					if (player[creature[i].playerIndex].arm[kLeftFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID==kRingOfShadows)
						if (pl_AuthorizedTo(i, kRegularPlayer) && player[creature[i].playerIndex].stealth)
							if (creature[i].nextMoveTime>currentTime) 
								if (tb_Rnd(1,20)<=2)
									{
								
									if (tb_Rnd(1,10)<=2)
										{
										ringdamage=1;
										nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kLeftFinger],ringdamage, 0);

										if (creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount<=ringdamage)
											{
												if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID].desiredCount>0)
													in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID, 1);

														creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID=0;

														player[creature[i].playerIndex].arm[kLeftFinger]=0;
											}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount-ringdamage;
										}

								player[creature[i].playerIndex].sendStatsRefresh=true;
									}
						//Next stealth ring
					if (player[creature[i].playerIndex].arm[kRightFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID==kRingOfStealth)
						if (pl_AuthorizedTo(i, kRegularPlayer) && player[creature[i].playerIndex].stealth)
							if (creature[i].nextMoveTime>currentTime) 
								if (tb_Rnd(1,20)<=2)
									{
								
									if (tb_Rnd(1,10)<=2)
										{
										ringdamage=1;
										nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kRightFinger],ringdamage, 0);

									if (creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount<=ringdamage)
									{
										if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID].desiredCount>0)
											in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID, 1);

										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID=0;

										player[creature[i].playerIndex].arm[kRightFinger]=0;
									}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount-ringdamage;
								}
								
								player[creature[i].playerIndex].sendStatsRefresh=true;
							}
							
					if (player[creature[i].playerIndex].arm[kLeftFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID==kRingOfStealth)
						if (pl_AuthorizedTo(i, kRegularPlayer) && player[creature[i].playerIndex].stealth)
							if (creature[i].nextMoveTime>currentTime) 
								if (tb_Rnd(1,20)<=2)
									{
								
									if (tb_Rnd(1,10)<=2)
										{
										ringdamage=1;
										nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kLeftFinger],ringdamage, 0);

										if (creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount<=ringdamage)
											{
												if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID].desiredCount>0)
												in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID, 1);

												creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID=0;

												player[creature[i].playerIndex].arm[kLeftFinger]=0;
											}
											else
										creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount-ringdamage;
										}

								player[creature[i].playerIndex].sendStatsRefresh=true;
									}		
						//Next stealth ring kDiscretionRing
					if (player[creature[i].playerIndex].arm[kRightFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID==kDiscretionRing)
						if (pl_AuthorizedTo(i, kRegularPlayer) && player[creature[i].playerIndex].stealth)
							if (creature[i].nextMoveTime>currentTime) 
								if (tb_Rnd(1,20)<=2)
									{
								
									if (tb_Rnd(1,10)<=2)
										{
										ringdamage=1;
										nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kRightFinger],ringdamage, 0);

									if (creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount<=ringdamage)
										{
										if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID].desiredCount>0)
											in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID, 1);

										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID=0;

										player[creature[i].playerIndex].arm[kRightFinger]=0;
										}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount-ringdamage;
								}
								
								player[creature[i].playerIndex].sendStatsRefresh=true;
							}
							
					if (player[creature[i].playerIndex].arm[kLeftFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID==kDiscretionRing)
						if (pl_AuthorizedTo(i, kRegularPlayer) && player[creature[i].playerIndex].stealth)
							if (creature[i].nextMoveTime>currentTime) 
								if (tb_Rnd(1,20)<=2)
									{
								
									if (tb_Rnd(1,10)<=2)
										{
										ringdamage=1;
										nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kLeftFinger],ringdamage, 0);

									if (creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount<=ringdamage)
									{
										if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID].desiredCount>0)
											in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID, 1);

										creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID=0;

										player[creature[i].playerIndex].arm[kLeftFinger]=0;
									}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount-ringdamage;
								}

								player[creature[i].playerIndex].sendStatsRefresh=true;
							}
					
		/*		
	//talisman of speed---------------------------------------------------------------------------------

					if (in_ItemEquipedServer(creature[i].playerIndex, kHorse))
						if (creature[i].nextMoveTime>currentTime) 
							{
							if (tb_Rnd(1,10)<=5)
								{
										ringdamage=1;
										nw_SendToClientDropFromInventorySlot(i,kDamageItem, (creature[i].playerIndex, kHorse),ringdamage);

									if (kHorse<=ringdamage)
										{
										if ((creature[i].playerIndex, kHorse)>0)
											in_ItemDestroyed((creature[i].playerIndex, kHorse), 1);

										(creature[i].playerIndex, kHorse)==0;

									
										}
									else
										(creature[i].playerIndex, kHorse)-ringdamage;
								}
								
								player[creature[i].playerIndex].sendStatsRefresh=true;
							}
							
					

	//charm of speed---------------------------------------------------------------------------
	
					

				if (in_ItemEquipedServer(creature[i].playerIndex, kSickHorse))
						if (creature[i].nextMoveTime>currentTime) 
							{
							if (tb_Rnd(1,10)<=5)
								{
										ringdamage=1;
										nw_SendToClientDropFromInventorySlot(i,kDamageItem,(creature[i].playerIndex, kSickHorse),ringdamage);

									if (creature[i].inventory[player[creature[i].playerIndex, kSickHorse].itemCount<=ringdamage)
										{
										if (itemList[creature[i].inventory[player[creature[i].playerIndex, kSickHorse].itemID].desiredCount>0)
											in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex, kSickHorse].itemID, 1);

										creature[i].inventory[player[creature[i].playerIndex, kSickHorse].itemID=0;

										creature[i].playerIndex, kSickHorse=0;
										}
									else
										creature[i].inventory[player[creature[i].playerIndex, kHorse].itemCount=creature[i].inventory[player[creature[i].playerIndex, kSickHorse].itemCount-ringdamage;
								}
								
								player[creature[i].playerIndex].sendStatsRefresh=true;
							}
							
				*/			
				


				//Dave^
		
					if (clericHoldingWeapon)
					{
						if ((creature[i].magicPoints>0) && (tb_Rnd(1,2)==1))
						{
							creature[i].magicPoints--;
							player[creature[i].playerIndex].sendStatsRefresh=true;
						}
					}
					else if (creature[i].magicPoints<creature[i].intelligence)
					{
						//raiseChance=10-sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMeditation].level);
						//if (raiseChance<1)
						//raiseChance=1;
						
						//Dave add Fighting Moon meditation 03-24-2012
						if (creature[i].magicPoints<creature[i].intelligence)
							if (currentCycle==kFightingMoon && fullMoon)
								if (tb_Rnd(1,10)<=sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMeditation].level))
								{
									creature[i].magicPoints++;

									if (player[creature[i].playerIndex].meditating)
										creature[i].magicPoints++;

									if (creature[i].magicPoints>creature[i].intelligence)
										creature[i].magicPoints=creature[i].intelligence;

									player[creature[i].playerIndex].sendStatsRefresh=true;
								}
									
									
						if (tb_Rnd(1,20)<=sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMeditation].level))
						{
							creature[i].magicPoints++;

							if (player[creature[i].playerIndex].meditating)
								creature[i].magicPoints++;

							if (creature[i].magicPoints>creature[i].intelligence)
								creature[i].magicPoints=creature[i].intelligence;

							player[creature[i].playerIndex].sendStatsRefresh=true;
						}

						if (creature[i].magicPoints<creature[i].intelligence)
							if (in_ItemEquipedServer(creature[i].playerIndex,kManaRing))
							{
								creature[i].magicPoints+=2;

								if (player[creature[i].playerIndex].meditating)
									creature[i].magicPoints+=2;

								if (creature[i].magicPoints>creature[i].intelligence)
									creature[i].magicPoints=creature[i].intelligence;//

								player[creature[i].playerIndex].sendStatsRefresh=true;
							}
							
						if (creature[i].magicPoints<creature[i].intelligence)
							if (creature[i].godMode)
							{
								creature[i].magicPoints+=10;

								if (creature[i].magicPoints>creature[i].intelligence)
									creature[i].magicPoints=creature[i].intelligence;//

								player[creature[i].playerIndex].sendStatsRefresh=true;
							}
						//Lesser mana ring
						if (creature[i].magicPoints<creature[i].intelligence)
							if (player[creature[i].playerIndex].arm[kRightFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID==kLesserManaRing)
							{
								creature[i].magicPoints++;

								if (creature[i].magicPoints>creature[i].intelligence)
									creature[i].magicPoints=creature[i].intelligence;//
									
								if ((tb_Rnd(0,15)<1))
								{
									staminaRegen=tb_Rnd(1,3);
									
									nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kRightFinger],staminaRegen, 0);

									if (creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount<=staminaRegen)
									{
										if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID].desiredCount>0)
											in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID, 1);

										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemID=0;

										player[creature[i].playerIndex].arm[kRightFinger]=0;
									}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kRightFinger]].itemCount-staminaRegen;
								}
								
								player[creature[i].playerIndex].sendStatsRefresh=true;
							}
							
						if (creature[i].magicPoints<creature[i].intelligence)
							if (player[creature[i].playerIndex].arm[kLeftFinger]!=0 && creature[creature[i].playerIndex].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID==kLesserManaRing)
							{
								creature[i].magicPoints++;

								if (creature[i].magicPoints>creature[i].intelligence)
									creature[i].magicPoints=creature[i].intelligence;//
									
								if ((tb_Rnd(0,10)<1))
								{
									staminaRegen=tb_Rnd(1,3);
									
									nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[creature[i].playerIndex].arm[kLeftFinger],staminaRegen, 0);

									if (creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount<=staminaRegen)
									{
										if (itemList[creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID].desiredCount>0)
											in_ItemDestroyed(creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID, 1);

										creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemID=0;

										player[creature[i].playerIndex].arm[kLeftFinger]=0;
									}
									else
										creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount=creature[i].inventory[player[creature[i].playerIndex].arm[kLeftFinger]].itemCount-staminaRegen;
								}

								player[creature[i].playerIndex].sendStatsRefresh=true;
							}
					
					}


					// Wizards med
					if (player[creature[i].playerIndex].characterClass==6 || pl_AuthorizedTo(i, kIsAnyGM))
						if ((creature[i].hitPoints<creature[i].strength))
							if (player[creature[i].playerIndex].meditating)
								if (tb_Rnd(1, 20)<=sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMeditation].level))
									creature[i].hitPoints++;
	
	
	

					// ------- raise health points
					if ((creature[i].hitPoints<creature[i].strength) && (tb_Rnd(1,2)==1) && (creature[i].poison==0))//
					{
						raiseChance=10-sk_GetSkillLevel(player[creature[i].playerIndex].skill[kHealing].level);

						if (raiseChance<1)
							raiseChance=1;

						if (creature[i].attackTarget!=0)
							raiseChance=15;

						if (tb_Rnd(1,(raiseChance*2))==1)
						{
							creature[i].hitPoints++;

							if (player[creature[i].playerIndex].healing)
								creature[i].hitPoints++;

							if (creature[i].hitPoints>creature[i].strength)
								creature[i].hitPoints=creature[i].strength;

							player[creature[i].playerIndex].sendStatsRefresh=true;
						}
					}
					
					if (creature[i].hitPoints<creature[i].strength && creature[i].godMode)
					{
						creature[i].hitPoints+=20;

						if (creature[i].hitPoints>creature[i].strength)
							creature[i].hitPoints=creature[i].strength;

						player[creature[i].playerIndex].sendStatsRefresh=true;
					}

					// ------- regeneration

					if (creature[i].regeneratingTime!=0)
					{
						if (creature[i].regeneratingTime<currentTime)
						{
							creature[i].regeneratingTime=0;
							creature[i].regeneratingPower=0;
						}
						else
						{
							if (creature[i].hitPoints<creature[i].strength)
							{
								for (j=0; j<creature[i].regeneratingPower; j++)
									if (tb_Rnd(0,10)<1)
										if (creature[i].hitPoints<creature[i].strength)
											creature[i].hitPoints++;
							}
							else
							{
								creature[i].regeneratingTime=0;
								creature[i].regeneratingPower=0;
							}
						}
					}



					// ------- poison
					if (creature[i].poison>0)
						if (in_ItemEquipedServer(creature[i].playerIndex,kPoisonRing))
							if (creature[i].poison>6)
								creature[i].poison=creature[i].poison-6;
							else
								creature[i].poison=0;

					if ((creature[i].poison>0) && (tb_Rnd(1,4)==1))
					{
						raiseChance=sk_GetSkillLevel(player[creature[i].playerIndex].skill[kHealing].level);

						if (tb_Rnd(1,raiseChance)==1)
						{
							poison=tb_Rnd(1,creature[i].poison);
							
							if (creature[i].damageReduc<1.0)
								poison=tb_Round((float)poison*creature[i].damageReduc);

							if (poison>=creature[i].hitPoints)
							{
								cr_Dead(i);
								continue;
							}
							else
								creature[i].hitPoints=creature[i].hitPoints-poison;

							player[creature[i].playerIndex].sendStatsRefresh=true;
						}

						if (tb_Rnd(1,50)==1)	// poison dissipation rate
							creature[i].poison--;
					}

					creature[i].nextStatTime=currentTime+30;
				}

				//	if (fxMap[RC(creature[i].row,kMapWidth)][RC(creature[i].col,kMapHeight)]!=0)
				//		{
				//			if (serverFX[fxMap[RC(creature[i].row,kMapWidth)][RC(creature[i].col,kMapHeight)]]==kFireField)	 // standing in fire
				//				if (tb_Rnd(1,10)==1)
				//					{
				//						player[creature[i].playerIndex].sendStatsRefresh=true;
				//						if (creature[i].hitPoints>2)
				//							creature[i].hitPoints=creature[i].hitPoints-2;
				//						else
				//							creature[i].hitPoints=0;
				//						if (creature[i].hitPoints==0)
				//							{
				//								cr_Dead(i);
				//								continue;
				//							}
				//					}
				//		}

			}

			if (creature[i].activeFX) // gs ?
			{
				creature[i].activeFX=false;

				for (j=0;j<kMaxCreatureEffects;j++)
					if (creature[i].fx[j]!=0)
						if (currentTime>creature[i].fxKillTime[j])
						{
							serverFX[RC(creature[i].fx[j],kMaxFX)]	=	0;
							creature[i].fx[j]						=	0;
							creature[i].fxKillTime[j]		=	0;
							creature[i].fxTarget[j]			=	0;
							creature[i].fxTargetRow[j]	=	0;
							creature[i].fxTargetCol[j]	=	0;
						}
						else
							creature[i].activeFX=true;
			}

			if (creature[i].attackTarget!=0)
				if (currentTime>creature[i].nextAttackTime)
				{
					#ifdef _FUNCTIONTRACEDEBUGGING
					fi_WriteToErrorLog("cr_attack Entry");
					#endif
	
					cr_Attack(i,creature[i].attackTarget);
				
					#ifdef _FUNCTIONTRACEDEBUGGING
					fi_WriteToErrorLog("cr_attack Exit");
					#endif
				}
					

			if ((currentTime>creature[i].nextStrikeTime) && (creature[i].numberOfStrikes>0))
			{

				if (creature[i].strikePose==1)
				{
					creature[i].strikePose          = 2;
				}
				else
				{
					creature[i].strikePose=1;

					if (player[creature[i].playerIndex].gatherType==kLumberjacking)
					{
						cr_AddSound(i, kWoodChop);
						//creature[i].soundFX					= kWoodChop;
						//creature[i].soundFXTime			= currentTime;
					}
					else if (player[creature[i].playerIndex].gatherType==kMining)
					{
						cr_AddSound(i, kMineSnd);
						//creature[i].soundFX					= kMineSnd;
						//creature[i].soundFXTime			= currentTime;
					}
					else if (player[creature[i].playerIndex].gatherType==255)
						if (itemList[player[creature[i].playerIndex].gatherRow].makeSkill==kBlacksmithing)
						{
							cr_AddSound(i, kBlacksmithySnd);
							//creature[i].soundFX					= kBlacksmithySnd;
							//creature[i].soundFXTime			= currentTime;
						}
				}

				creature[i].numberOfStrikes--;

				if (creature[i].numberOfStrikes==0) //0)
				{
					creature[i].nextStrikeTime=0;
					creature[i].strikePose=0;

					if (creature[i].finalStrikeSound!=0)
					{
						cr_AddSound(i, creature[i].finalStrikeSound);
						//creature[i].soundFX					      = creature[i].finalStrikeSound;
						//creature[i].soundFXTime			      = currentTime;
						creature[i].finalStrikeSound      = 0;
					}
				}
				else
				{
					creature[i].nextStrikeTime=currentTime+45;
				}
			}

		}

	currentCreatures=0;

	for (i=5000;i<kMaxCreatures;i++)	// ----------------------- monsters --------------------------------------------------------------------------
		if (creature[i].id!=0)
		{
			if (creature[i].talkTime>0)	// ------ check for talk erase time
				if (talkExpireTime>creature[i].talkTime)
				{
					creature[i].talkWhen			= 0;
					creature[i].talkTime			= 0;
					creature[i].talkID			  = 0;
					strcpy(creature[i].talk,"");
				}



			// gs questionable 2/26/03 //  nw_GetMessages();

			if (creature[i].home!=0) // if creature is out of home range and not in sight then clear it and spawn another
			{
				farFromHome=false;

				if (creature[i].row<spawnRegion[creature[i].home].theRegion.left-creature[i].range) 				farFromHome=true;
				else if (creature[i].row>spawnRegion[creature[i].home].theRegion.right+creature[i].range) 	farFromHome=true;
				else if (creature[i].col<spawnRegion[creature[i].home].theRegion.top-creature[i].range) 		farFromHome=true;
				else if (creature[i].col>spawnRegion[creature[i].home].theRegion.bottom+creature[i].range) 	farFromHome=true;

				if (farFromHome)
					if (cr_NearPlayer(i)==false)
					{
						pl_BanishCreature(-1,i);
						continue;
					}
			}

			currentCreatures++;

			if (creature[i].dieTime!=0)
			{
				if (currentTime>creature[i].dieTime)
					cr_Dead(i);

				continue;
			}

			if (creature[i].lifespan>0)
			{
				if (currentTime>creature[i].lifespan)
				{
					cr_Dead(i);
					continue;
				}

				if (creature[i].attackTarget==0)
				{
					for (theRow=(creature[i].row-3);theRow<(creature[i].row+3);theRow++)
						for (theCol=(creature[i].col-3);theCol<(creature[i].col+3);theCol++)
							if (theCol>0 && theCol<kMapHeight && theRow>0 && theRow<kMapWidth)
								if (creatureMap[theRow][theCol]!=0)
									if (creatureMap[theRow][theCol]!=i)
										if (!((creature[creatureMap[theRow][theCol]].npcIndex!=0) || (creature[creatureMap[theRow][theCol]].id==kHuman)))
											if (creature[creatureMap[theRow][theCol]].master==0)
												if (creature[creatureMap[theRow][theCol]].lifespan==0)
													creature[i].attackTarget=creatureMap[theRow][theCol];
				}
			}

			if (creature[i].calmUntil>0)
				if (currentTime>creature[i].calmUntil)
					creature[i].calmUntil=0;

			if (currentTime>creature[i].nextStatTime)
			{
				// ------- raise magic points
				if ((creature[i].magicPoints<creatureInfo[creature[i].id].maxMagicPoints) && (tb_Rnd(1,5)==1))//
				{
					creature[i].magicPoints=creature[i].magicPoints+(creatureInfo[creature[i].id].maxMagicPoints/10);

					if (creature[i].magicPoints>creatureInfo[creature[i].id].maxMagicPoints)
						creature[i].magicPoints=creatureInfo[creature[i].id].maxMagicPoints;
				}

				// ------- raise health points
				if (creature[i].hitPoints<creatureInfo[creature[i].id].maxHitPoints)
					//if (tb_Rnd(1,5)==1)
					if (tb_Rnd(1,350)<creatureInfo[creature[i].id].maxHitPoints) // put in carbon 11/24
					{
						if (creature[i].attackTarget==0)
						{
							creature[i].hitPoints++;//

							if (creature[i].hitPoints>creatureInfo[creature[i].id].maxHitPoints)
								creature[i].hitPoints=creatureInfo[creature[i].id].maxHitPoints;

							if (creature[i].wounded)
								if (creature[i].hitPoints*1.35>creatureInfo[creature[i].id].maxHitPoints)
								{
									creature[i].wounded=false;
									creature[i].cornered=false;
								}
						}
						else if (cr_DistanceBetween(i,creature[i].attackTarget)>creatureInfo[creature[i].id].attackRange) // 11/14
						{
							creature[i].hitPoints++;//

							if (creature[i].hitPoints>creatureInfo[creature[i].id].maxHitPoints)
								creature[i].hitPoints=creatureInfo[creature[i].id].maxHitPoints;

							if (creature[i].wounded)
								if (creature[i].hitPoints*1.35>creatureInfo[creature[i].id].maxHitPoints)
								{
									creature[i].wounded=false;
									creature[i].cornered=false;
								}
						}
					}

				// ------- regeneration

				if (creature[i].regeneratingTime!=0)
				{
					if (creature[i].regeneratingTime<currentTime)
					{
						creature[i].regeneratingTime=0;
						creature[i].regeneratingPower=0;
					}
					else
					{
						if (creature[i].hitPoints<creatureInfo[creature[i].id].maxHitPoints)
						{
							for (j=0; j<creature[i].regeneratingPower; j++)
								if (tb_Rnd(0,10)<1)
									if (creature[i].hitPoints<creatureInfo[creature[i].id].maxHitPoints)
										creature[i].hitPoints++;
						}
						else
						{
							creature[i].regeneratingTime=0;
							creature[i].regeneratingPower=0;
						}
					}
				}

				// ------- poison
				if (creature[i].poison>0)
					if (tb_Rnd(1,4)==1)
					{
						if (tb_Rnd(1,2)==1)
						{
							poison=tb_Rnd(1,creature[i].poison);
							
							if (creature[i].damageReduc<1.0)
								poison=tb_Round((float)poison*creature[i].damageReduc);

							if (poison>=creature[i].hitPoints)
							{
								cr_Dead(i);
								continue;
							}
							else
								creature[i].hitPoints=creature[i].hitPoints-poison;
						}

						if (tb_Rnd(1,10)==1)
							creature[i].poison--;
					}

				creature[i].nextStatTime=currentTime+30;
			}

			if (creature[i].soundFX!=0)
				if ((creature[i].soundFXTime+200)<currentTime)	// kill sound after 2 seconds
				{
					creature[i].soundFX			        =	0;
					creature[i].soundFXTime	        =	0;
					creature[i].soundID             = 0; //11/11
				}

			//if (fxMap[RC(creature[i].row,kMapWidth)][RC(creature[i].col,kMapHeight)]!=0)
			if (fxMap[creature[i].row][creature[i].col]!=0)
			{
				//if (serverFX[fxMap[RC(creature[i].row,kMapWidth)][RC(creature[i].col,kMapHeight)]]==kFireField)	 // standing in fire
				if (serverFX[fxMap[creature[i].row][creature[i].col]]==kFireField)	 // standing in fire (will not hurt fire creatures, but hurt ice creatures more)
				{
					if (creatureInfo[creature[i].id].fire==false && cr_ImmuneToFF(creature[i].id)==false)
					{
						if (tb_Rnd(1,15)==1)
						{
							creature[i].hitPoints--;

							if (creatureInfo[creature[i].id].frost)
								creature[i].hitPoints--;
						}

						if (creature[i].hitPoints<=0)
						{
							cr_Dead(i);
							continue;
						}
					}
				}
			}


			if (creature[i].activeFX)
			{
				creature[i].activeFX=false;

				for (j=0;j<kMaxCreatureEffects;j++)
					if (creature[i].fx[j]!=0)
						if (currentTime>creature[i].fxKillTime[j])
						{
							serverFX[RC(creature[i].fx[j],kMaxFX)]	=	0;
							creature[i].fx[j]						=	0;
							creature[i].fxKillTime[j]		=	0;
							creature[i].fxTarget[j]			=	0;
							creature[i].fxTargetRow[j]	=	0;
							creature[i].fxTargetCol[j]	=	0;
						}
						else
							creature[i].activeFX=true;
			}

			if (creature[i].paralyzed!=0)	// check for creature paralyzed
			{
				if (creature[i].paralyzed>currentTime)
				{
					continue;
				}
				else
					creature[i].paralyzed=0;
			}

			if (creature[i].attackTarget!=0) // --- attacking something?
			{
				if (creature[creature[i].attackTarget].playerIndex!=0) //... used to make hiding harder
				{
					dist=cr_DistanceBetween(i,creature[i].attackTarget);

					if (dist>255)
						dist=255;

					if ((dist<creature[creature[i].attackTarget].beingAttacked) || (creature[creature[i].attackTarget].beingAttacked==0))
						creature[creature[i].attackTarget].beingAttacked=dist;
				}
			}

			if ((currentTime>creature[i].nextMoveTime) || ((currentTime>creature[i].nextStrikeTime) && (creature[i].numberOfStrikes>0)))
			{
				forceMove	=	false;
				inCombat	=	false;
				modifier	=	0;


				if (creature[i].strengthTime!=0)
					if (currentTime>creature[i].strengthTime)
					{
						creature[i].strengthTime=0;
						creature[i].strengthOffset=0;
					}

				if (creature[i].intelligenceTime!=0)
					if (currentTime>creature[i].intelligenceTime)
					{
						creature[i].intelligenceTime=0;
						creature[i].intelligenceOffset=0;
					}

				if (creature[i].dexterityTime!=0)
					if (currentTime>creature[i].dexterityTime)
					{
						creature[i].dexterityTime=0;
						creature[i].dexterityOffset=0;
					}

				if ((currentTime>creature[i].nextStrikeTime) && (creature[i].numberOfStrikes>0))
				{
					forceMove=true;

					if (creature[i].strikePose==1)
					{
						creature[i].strikePose=2;
						modifier=10;
					}
					else
					{
						creature[i].strikePose=1;

						if (creature[i].facing>9)
							creature[i].facing=creature[i].facing-10;
					}

					creature[i].numberOfStrikes--;

					if ((creature[i].numberOfStrikes==1) && (creature[i].finalStrikeSound==kBluntAttackSnd)) //0)
					{
						if (creature[i].finalStrikeSound!=0)
						{
							cr_AddSound(i, creature[i].finalStrikeSound);
							//creature[i].soundFX					      = creature[i].finalStrikeSound;
							//creature[i].soundFXTime			      = currentTime;
							creature[i].finalStrikeSound      = 0;
						}
					}

					if (creature[i].numberOfStrikes==0)
					{
						creature[i].nextStrikeTime=0;
						creature[i].strikePose=0;

						if ((creature[i].finalStrikeSound!=0) && (creature[i].finalStrikeSound!=kBluntAttackSnd))
						{
							cr_AddSound(i, creature[i].finalStrikeSound);
							//creature[i].soundFX					      = creature[i].finalStrikeSound;
							//creature[i].soundFXTime			      = currentTime;
							creature[i].finalStrikeSound      = 0;
						}
					}
					else
					{
						creature[i].nextStrikeTime=currentTime+45;
					}
				}


				//-------------------------------------------------------------------------------------------

				tracking    							= false;

				attacking   							= false;

				running     							= false;

				followingMaster							= false;

				spellcasterFlying						= false;

				creature[i].nextTrackRow  = 0;

				creature[i].nextTrackCol  = 0;


				//--------- pet s--------
				if (creature[i].master!=0) // tamed
				{

					firstRow=creature[i].row-3;
					lastRow=creature[i].row+3;
					firstCol=creature[i].col-3;
					lastCol=creature[i].col+3;

					if (firstRow<0)            firstRow=1;

					if (lastRow>=kMapWidth)    lastRow=kMapWidth-1;

					if (firstCol<0)            firstCol=1;

					if (lastCol>=kMapHeight)   lastCol=kMapHeight-1;

					if (creature[i].npcIndex==0)
					{
						for (scanRow=firstRow;scanRow<lastRow;scanRow++)
							for (scanCol=firstCol;scanCol<lastCol;scanCol++)
								if (creatureMap[scanRow][scanCol]!=0)
									if (creature[creatureMap[scanRow][scanCol]].alignment==kEvil)
										if (creature[creatureMap[scanRow][scanCol]].master==0)
											if (creature[creatureMap[scanRow][scanCol]].lifespan==0)
												if (creature[creatureMap[scanRow][scanCol]].calmUntil==0)
													if (creature[creatureMap[scanRow][scanCol]].attackTarget==0)
														if (creature[creatureMap[scanRow][scanCol]].possibleTarget==0)
															if (tb_Rnd(1,7)==1)
																if (pl_AuthorizedTo(creature[i].master, kRegularPlayer))
																	creature[creatureMap[scanRow][scanCol]].possibleTarget=i;
					}

					if (creature[i].command==kStay)
					{
						continue;
					}

					if (creature[i].command==kMove)
					{
						if (cr_DistanceBetween(creature[i].master, i)>9)
							creature[i].command=kHeel;

						cr_GetSpotFarthestAway(i,creature[i].master,&fleeRow, &fleeCol);

						if (!((fleeRow==0) && (fleeCol==0)))
							if (cr_BuildPathTo(i,fleeRow,fleeCol, &stepRow, &stepCol))
							{
								if ((stepRow==0) && (stepCol==0))
								{
									tracking=true;
									running=false;
								}
								else
								{
									creature[i].nextTrackRow    = stepRow;
									creature[i].nextTrackCol    = stepCol;
									tracking=true;
									running=true;
								}
							}
					}

					followMaster=false;

					if (creature[i].command==kHeel)
						followMaster=true;

					if ((creature[i].command==kGuard) && (creature[i].attackTarget==0) && (creature[i].possibleTarget==0))
						followMaster=true;

					if ((creature[i].command==kAttack) && (creature[i].attackTarget==0) && (creature[i].possibleTarget==0))
						followMaster=true;

					if (followMaster)
					{
						if (cr_DistanceBetween(creature[i].master, i)<4)
							continue;

						pl_GetPetPosition(creature[i].master,i,&petRow,&petCol);

						if ((petRow==0) && (petCol==0))
						{
							followingMaster=true;
							tracking=false;
							running=false;
						}
						else
						{
							cr_BuildPathTo(i, petRow, petCol, &stepRow, &stepCol);
							creature[i].nextTrackRow    = stepRow;
							creature[i].nextTrackCol    = stepCol;
							followingMaster=true;
							tracking=true;
						}
					}

					//if ((creature[i].command==kGuard) && (creature[i].attackTarget==0) && (creature[i].possibleTarget==0))
					//	{
					//	  if (cr_DistanceBetween(creature[i].master, i)<5)
					//			continue;
					//		pl_GetPetPosition(creature[i].master,i,&creature[i].nextTrackRow,&creature[i].nextTrackCol);
					//		followingMaster=true;
					//		tracking=true;
					//	}

				}


				if (followingMaster==false)
				{
					if (creature[i].attackTarget!=0)  // already attacking something
					{
						if (cr_LowMorale(i))
						{
							creature[i].wounded=true;
							cr_GetSpotFarthestAway(i,creature[i].attackTarget,&fleeRow, &fleeCol);

							if (!((fleeRow==0) && (fleeCol==0)))
							{
								if (cr_BuildPathTo(i,fleeRow,fleeCol, &stepRow, &stepCol)) // if can get to target
								{
									if ((stepRow==0) && (stepCol==0))
									{
										tracking=false;
										running=false;
									}
									else
									{
										creature[i].nextTrackRow    = stepRow;
										creature[i].nextTrackCol    = stepCol;
										tracking=true;
										running=true;
										tx_CreatureTalkText(i,creature[i].attackTarget,kRetreatingTalk);
									}
								}
							}
							else
								creature[i].cornered=true;

							if (cr_DistanceBetween(i,creature[i].attackTarget)>8)
							{
								creature[i].attackTarget			=	0;
								creature[i].possibleTarget		=	0;
								creature[i].nextStrikeTime		=	0;
								creature[i].numberOfStrikes		=	0;
								creature[i].trackRow					= 0;
								creature[i].trackCol					= 0;
								creature[i].tracking					= false;
								creature[i].cornered					= false;
							}




						}

						//Spellcasters try to stay away from attacker
						if (creatureInfo[creature[i].id].spellcastChance==100 && creatureInfo[creature[i].id].mobile)
						{
							cr_GetSpotFarthestAway(i,creature[i].attackTarget,&fleeRow, &fleeCol);

							if (!((fleeRow==0) && (fleeCol==0)))
							{
								if (cr_BuildPathTo(i,fleeRow,fleeCol, &stepRow, &stepCol))
								{
									if ((stepRow!=0) || (stepCol!=0))
										if (!cr_ViewBlocked(stepRow, stepCol, creature[creature[i].attackTarget].row, creature[creature[i].attackTarget].col))
										{

											creature[i].nextTrackRow    = stepRow;
											creature[i].nextTrackCol    = stepCol;
											spellcasterFlying=true;
											tracking=true;
										}
								}
							}
						}

						if (running==false)
							if (cr_AttackRange(i)>=cr_DistanceBetween(i,creature[i].attackTarget))  // if in attack range
								if (cr_ViewBlocked(creature[i].row, creature[i].col, creature[creature[i].attackTarget].row, creature[creature[i].attackTarget].col)==false)
								{
									cr_Attack(i,creature[i].attackTarget);  // attack creature

									if (!spellcasterFlying)
										tracking=false;

									attacking=true;

									if (creature[creature[i].attackTarget].playerIndex!=0)  // human retailiate - disabled
									{
										//if (creature[creature[i].attackTarget].attackTarget==0)
											//creature[creature[i].attackTarget].attackTarget=i;
									}
									else  // monster retailiate
									{
										if (creature[creature[i].attackTarget].attackTarget==0)
											if (creature[creature[i].attackTarget].possibleTarget==0)
												creature[creature[i].attackTarget].possibleTarget=i;
									}

								}


					}


					if (running==false)
						//if (tracking==true)
						if (creature[i].attackTarget==0) // no target yet
						{


							if (creature[i].possibleTarget==0) // serpents and wolves will kill sheep
								if ((creature[i].id==kSerpent) || (creature[i].id==kGreyWolf) || (creature[i].id==kBlackWolf) || (creature[i].id==kBrownWolf) || (creature[i].id==kPhaseSerpent)) //Dave add PS
								{
									firstRow=creature[i].row-creatureInfo[creature[i].id].attackRange;
									lastRow=creature[i].row+creatureInfo[creature[i].id].attackRange;
									firstCol=creature[i].col-creatureInfo[creature[i].id].attackRange;
									lastCol=creature[i].col+creatureInfo[creature[i].id].attackRange;

									if (firstRow<0)            firstRow=1;

									if (lastRow>=kMapWidth)    lastRow=kMapWidth-1;

									if (firstCol<0)            firstCol=1;

									if (lastCol>=kMapHeight)   lastCol=kMapHeight-1;

									for (scanRow=firstRow;scanRow<lastRow;scanRow++)
										for (scanCol=firstCol;scanCol<lastCol;scanCol++)
											if (creatureMap[scanRow][scanCol]!=0)
												if (creature[creatureMap[scanRow][scanCol]].id==kSheep)
													if (creature[creatureMap[scanRow][scanCol]].master==0)
													{
														creature[i].attackTarget=creatureMap[scanRow][scanCol];
														break;
													}
								}

							if (creature[i].possibleTarget!=0)  // if there is a possible target
							{
								if (creature[creature[i].possibleTarget].playerIndex!=0)
									if (player[creature[creature[i].possibleTarget].playerIndex].currentlyAttacking>=player[creature[creature[i].possibleTarget].playerIndex].maxAttackers)
										creature[i].possibleTarget=0;

								if (cr_AttackRange(i)>=cr_DistanceBetween(i,creature[i].possibleTarget))  // if in attack range
									if (cr_ViewBlocked(creature[i].row, creature[i].col, creature[creature[i].possibleTarget].row, creature[creature[i].possibleTarget].col)==false)
									{
										creature[i].attackTarget    = creature[i].possibleTarget; // set target
										creature[i].possibleTarget  = 0;
										creature[i].nextTrackRow    = 0;
										creature[i].nextTrackCol    = 0;
										tracking=true;
										cr_CheckForGuardingPets(creature[i].attackTarget, i);
									}

								if (creature[i].attackTarget==0)
								{
									if (cr_BuildAttackPath(i,creature[i].possibleTarget, &stepRow, &stepCol)) // if can get to target
									{
										creature[i].attackTarget    = creature[i].possibleTarget; // set target
										creature[i].possibleTarget  = 0;
										creature[i].nextTrackRow    = stepRow;
										creature[i].nextTrackCol    = stepCol;
										tracking=true;
										cr_CheckForGuardingPets(creature[i].attackTarget, i);
									}
									else
									{
										creature[i].attackTarget    = 0;  // can't get to possible target, break off attack
										creature[i].possibleTarget  = 0;
										tracking=false;
										attacking=false;
									}
								}
							}
						}
						else  // already have target
						{
							if (creature[creature[i].attackTarget].playerIndex!=0 && player[creature[creature[i].attackTarget].playerIndex].dead)  // target died, stop chasing
							{
								creature[i].attackTarget			=	0;
								creature[i].possibleTarget		=	0;
								tracking=false;
							}

							if ((cr_AttackRange(i)>=cr_DistanceBetween(i,creature[i].attackTarget)) && (cr_ViewBlocked(creature[i].row, creature[i].col, creature[creature[i].attackTarget].row, creature[creature[i].attackTarget].col)==false))
							{
								if (!spellcasterFlying)
								{
									creature[i].nextTrackRow    = 0;
									creature[i].nextTrackCol    = 0;
								}

								tracking=true;
							}
							else if (cr_BuildAttackPath(i,creature[i].attackTarget, &stepRow, &stepCol))  // if possible, move toward target
							{
								creature[i].nextTrackRow    = stepRow;
								creature[i].nextTrackCol    = stepCol;
								tracking=true;
								tx_CreatureTalkText(i,creature[i].attackTarget,kChasingTalk);
								
								if (cr_DistanceBetween(i,creature[i].attackTarget)<=7 && !cr_ViewBlocked(creature[i].row, creature[i].col, creature[creature[i].attackTarget].row, creature[creature[i].attackTarget].col) && currentTime>creature[i].nextAttackTime && tb_Rnd(0,100)<creatureInfo[creature[i].id].spellcastChance) // out of weapon range... cast spell?
								{
									if (cr_CastCreatureSpell(i, creature[i].attackTarget))
									{
										if (creature[creature[i].attackTarget].hiding)	creature[creature[i].attackTarget].hiding=false;
										
										creature[i].nextStrikeTime=currentTime;
										creature[i].numberOfStrikes=4;
										creature[i].strikePose=1;
						
										creature[i].nextAttackTime	= currentTime + 600 - creatureInfo[creature[i].id].mageryLevel*10;
									}
								}
							}
							else  // can't get to target, break off attack
							{
								creature[i].attackTarget    = 0;
								creature[i].possibleTarget  = 0;
								tracking=false;
								attacking=false;
							}
						}


				}


				rowDir=0;

				colDir=0;
				row=(int)(creature[i].row);
				col=(int)(creature[i].col);

				//   if ((creature[i].attackTarget==0) && (creature[i].pathPtr>0))
				//   	{
				/*
					for (j=0;j<creature[i].pathPtr;j++)
						{
				    				rowDir=fabs(creature[i].pathRow[j]-creature[i].row);
				    				colDir=fabs(creature[i].pathCol[j]-creature[i].col);
				    				if ((rowDir<=1) && (colDir<=1))
				    					{
				    						creature[i].pathPtr=j;
				    						break;
				    					}
						}
				    		rowDir=creature[i].pathRow[creature[i].pathPtr]-creature[i].row;
				    		colDir=creature[i].pathCol[creature[i].pathPtr]-creature[i].col;
				    		if (creature[i].pathPtr>0)
				    			creature[i].pathPtr--;
				*/
				//  	}
				// 	else



				if (tracking)
				{
					cr_GetTrackingMove(i,&rowDir,&colDir);  // get tracking direction

					if ((fabs(rowDir)>1) || (fabs(colDir)>1))
					{
						rowDir=0;
						colDir=0;
					}

					if (spellcasterFlying)
						if (cr_AttackRange(i)==cr_DistanceBetween(i,creature[i].attackTarget))
						{
							rowDir=0;
							colDir=0;
						}
				}
				else
				{
					if (creature[i].npcIndex!=0) //------- NPC Human ... 25% chance of moving
					{
						if (npc[creature[i].npcIndex].forceMove==false && tb_Rnd(1,4)>1)
							continue;

						//------------ default random move ------------
						rowDir=tb_Rnd(0,2)-1;

						colDir=tb_Rnd(0,2)-1;

						if (tb_Rnd(0,10)>6)
						{
							rowDir=0;
							colDir=0;
						}

						//---------------------------------------------
						if (rowDir!=0 || colDir!=0)
						{
							if (npc[creature[i].npcIndex].range>0 && creature[i].master==0)// npc out of range
							{
								if (fabs((creature[i].row+rowDir)-npc[creature[i].npcIndex].startRow)>npc[creature[i].npcIndex].range)
									rowDir=0;

								if (fabs((creature[i].col+colDir)-npc[creature[i].npcIndex].startCol)>npc[creature[i].npcIndex].range)
									colDir=0;

								if (npc[creature[i].npcIndex].spawned==false && cr_DistanceFrom(i, npc[creature[i].npcIndex].startRow, npc[creature[i].npcIndex].startCol)>npc[creature[i].npcIndex].range) //Out of range, come back home
								{
									if (np_IsRegularNPC(i) || npc[creature[i].npcIndex].type==kNPCGuard)
										cr_CreatureMoveTo(i, npc[creature[i].npcIndex].startRow, npc[creature[i].npcIndex].startCol);
									else
									{
										npc[creature[i].npcIndex].startRow=creature[i].row;
										npc[creature[i].npcIndex].startCol=creature[i].col;
									}
								}


							}
						}
					}

					//-------------------------------------------------------------------------------------------
					else if (creature[i].id==kSeaTurtle) //------- sea turtle
					{
						if (tb_Rnd(1,25)!=1)
						{
							if (tb_Rnd(1,15)==1)
							{
								rowDir=0;
								colDir=0;
								modifier=10;

								if (creature[i].facing<8)	forceMove=true;
							}
							else
							{
								modifier=0;

								if (creature[i].facing==0)				{	rowDir=-1;		colDir=-1;}

								if (creature[i].facing==1)				{	rowDir=-1;		colDir=0;	}

								if (creature[i].facing==2)				{	rowDir=-1;		colDir=1;	}

								if (creature[i].facing==3)				{	rowDir=0;			colDir=1;	}

								if (creature[i].facing==4)				{	rowDir=1;			colDir=1;	}

								if (creature[i].facing==5)				{	rowDir=1;			colDir=0;	}

								if (creature[i].facing==6)				{	rowDir=1;			colDir=-1;}

								if (creature[i].facing==7)				{	rowDir=0;			colDir=-1;}
							}
						}
						else
						{
							modifier=10;
							rowDir=tb_Rnd(0,2)-1;
							colDir=tb_Rnd(0,2)-1;
						}
					}
					else  //------------ default random move ------------
					{
						{
							j=tb_Rnd(0,10);

							//if ((creature[i].hitPoints*2)<creatureInfo[creature[i].id].maxHitPoints)
							//  j=4;

							if (j<3)
							{
								switch (creature[i].facing)
								{
									case 0: {	rowDir=-1;		colDir=-1;		break; }

									case 1: {	rowDir=-1;		colDir=0;			break; }

									case 2: {	rowDir=-1;		colDir=1;			break; }

									case 3: {	rowDir=0;			colDir=1;			break; }

									case 4: {	rowDir=1;			colDir=1;			break; }

									case 5: {	rowDir=1;			colDir=0;			break; }

									case 6: {	rowDir=1;			colDir=-1;		break; }

									case 7: {	rowDir=0;			colDir=-1;		break; }

									default:	break;
								}
							}
							else if (j<7)
							{
								rowDir=tb_Rnd(0,2)-1;
								colDir=tb_Rnd(0,2)-1;
								//if (cr_AdjacentSpotHasHuman(creature[i].row+rowDir,creature[i].col+colDir))
								//  {
								//    rowDir=0;
								//   colDir=0;
								// }
							}
							else
							{
								rowDir=0;
								colDir=0;
							}

						}
					}

					if (creature[i].master==0)
						if (creature[i].attackTarget==0)
							if (creature[i].range>0)
							{
								outOfRange=false;
								rowDist=fabs(creature[i].row+rowDir-creature[i].startRow);
								colDist=fabs(creature[i].col+colDir-creature[i].startCol);

								if (rowDist>colDist)
									dist=rowDist;
								else
									dist=colDist;

								if (dist>creature[i].range)
								{
									outOfRange=true;
									rowDist2=fabs(creature[i].row-creature[i].startRow);
									colDist2=fabs(creature[i].col-creature[i].startCol);

									if (rowDist2>colDist2)
										dist2=rowDist2;
									else
										dist2=colDist2;

									if (dist<dist2)
										outOfRange=false;
								}

								if (outOfRange)
								{
									rowDir=0;
									colDir=0;

									if (dist2>creature[i].range)
									{
										creature[i].startRow=creature[i].row;
										creature[i].startCol=creature[i].col;
									}

									//if (cr_BuildPathTo(i,creature[i].startRow,creature[i].startCol, &stepRow, &stepCol)) // if can get to target
									//	{
									//    creature[i].nextTrackRow    = stepRow;
									//    creature[i].nextTrackCol    = stepCol;
									//    tracking=true;
									//	}
								}
							}

				}

				//-------------------------------------------------------------------------------------------

				if (creature[i].disorientation!=0)	// check for creature disorientated
				{
					if (creature[i].disorientation<=currentTime)
						creature[i].disorientation=0;
					else
					{
						rowDir=tb_Rnd(0,2)-1;
						colDir=tb_Rnd(0,2)-1;
					}
				}

				if (creature[i].npcIndex!=0 && np_IsRegularNPC(i))  //Only regular NPCs will stop if a player is around
					if (cr_AdjacentSpotHasHuman(creature[i].row,creature[i].col))
						if (npc[creature[i].npcIndex].forceMove==false)
						{
							rowDir=0;
							colDir=0;
						}
						else
							npc[creature[i].npcIndex].forceMove=false;

				if (creatureInfo[creature[i].id].mobile==false || attacking)
				{
					if (!spellcasterFlying)
					{
						rowDir=0;
						colDir=0;
					}
					else
					{
						row=(int)(creature[i].row+rowDir);
						col=(int)(creature[i].col+colDir);

						if (row<0) 								{ row	=	0; rowDir=0; }
						else if (row>=kMapWidth) 	{ row	=	kMapWidth-1; rowDir=0; }

						if (col<0) 								{ col	=	0; colDir=0; }
						else if (col>=kMapHeight)	{ col	=	kMapHeight-1; colDir=0; }
					}

					if (creatureInfo[creature[i].id].mobile)
						if (fxMap[creature[i].row][creature[i].col]!=0)
							if (fxMap[creature[i].row][creature[i].col]<kMaxFX)
							{
								if (serverFX[fxMap[creature[i].row][creature[i].col]]==kFireField)	 // standing in fire (don't run if immune)
								{
									if (creatureInfo[creature[i].id].fire==false && cr_ImmuneToFF(creature[i].id)==false)
									{
										rowDir=tb_Rnd(0,2)-1;
										colDir=tb_Rnd(0,2)-1;
										doMove=true;
										row=(int)(creature[i].row+rowDir);
										col=(int)(creature[i].col+colDir);

										if (row<0) 								{ row	=	0; rowDir=0; }
										else if (row>=kMapWidth) 	{ row	=	kMapWidth-1; rowDir=0; }

										if (col<0) 								{ col	=	0; colDir=0; }
										else if (col>=kMapHeight)	{ col	=	kMapHeight-1; colDir=0; }
									}
								}
							}
				}
				else
				{
					row=(int)(creature[i].row+rowDir);
					col=(int)(creature[i].col+colDir);

					if (row<0) 								{ row	=	0; rowDir=0; }
					else if (row>=kMapWidth) 	{ row	=	kMapWidth-1; rowDir=0; }

					if (col<0) 								{ col	=	0; colDir=0; }
					else if (col>=kMapHeight)	{ col	=	kMapHeight-1; colDir=0; }
				}

				//-------------------------------------------------------------------------------------------

				if (creatureInfo[creature[i].id].domain==kLand)
				{
					if ((creatureMap[row][col]	== 0) && (canWalkOn[map[row][col]]))
						doMove=true;
					else
					{
						doMove=false;
						row=(int)(creature[i].row);
						col=(int)(creature[i].col);
					}

					if (containerMap[row][col]!=0)
						if (gmProp[containerID[containerMap[row][col]]])
						{
							doMove=false;
							row=(int)(creature[i].row);
							col=(int)(creature[i].col);
						}
				}
				else if (creatureInfo[creature[i].id].domain==kAir)
				{
					if ((creatureMap[row][col]	== 0) && (canFlyOver[map[row][col]]))
						doMove=true;
					else
					{
						doMove=false;
						row=(int)(creature[i].row);
						col=(int)(creature[i].col);
					}

					if (containerMap[row][col]!=0)
						if (gmProp[containerID[containerMap[row][col]]])
						{
							doMove=false;
							row=(int)(creature[i].row);
							col=(int)(creature[i].col);
						}
				}
				else if (creatureInfo[creature[i].id].domain==kWater)
				{
					if ((creatureMap[row][col]	== 0) && (canSwimIn[map[row][col]]))
						doMove=true;
					else
					{
						doMove=false;
						row=(int)(creature[i].row);
						col=(int)(creature[i].col);
					}

					if (containerMap[row][col]!=0)
						if (gmProp[containerID[containerMap[row][col]]])
						{
							doMove=false;
							row=(int)(creature[i].row);
							col=(int)(creature[i].col);
						}
				}
				else if (creatureInfo[creature[i].id].domain==kNullDomain)
				{
					doMove=false;
					row=(int)(creature[i].row);
					col=(int)(creature[i].col);
				}

				//------------------------------------------------------------------------

				/*if (creature[i].home!=0)
					if (creature[i].attackTarget==0)
						{
							farFromHome=false;
							if (row<=spawnRegion[creature[i].home].theRegion.left-creature[i].range) 					farFromHome=true;
							else if (row>=spawnRegion[creature[i].home].theRegion.right+creature[i].range) 		farFromHome=true;
							else if (col<=spawnRegion[creature[i].home].theRegion.top-creature[i].range) 			farFromHome=true;
							else if (col>=spawnRegion[creature[i].home].theRegion.bottom+creature[i].range) 	farFromHome=true;
							if (farFromHome)
								doMove=false;
						}*/  //07/02/2005 - Would make creatures freeze if hid on

				//------------------------------------------------------------------------

				if (fxMap[RC(row,kMapWidth)][RC(col,kMapHeight)]!=0)
					if (fxMap[row][col]<kMaxFX)
					{
						if (serverFX[fxMap[RC(row,kMapWidth)][RC(col,kMapHeight)]]==kFireField)	 // walking in fire
							if (creatureInfo[creature[i].id].fire==false && cr_ImmuneToFF(creature[i].id)==false)
								doMove=false;
					}

				if ((map[row][col]==395) || 
						(map[row][col]==908) || 
							(map[row][col]==900) || 
								(map[row][col]==901) ||	
									(map[row][col]==1009) || 
										(map[row][col]==1010) ||
											(map[row][col]==1109))	// stepped on latter
				{
					if ((creature[i].master!=0) || (creature[i].attackTarget!=0)) // 11/5
					{
						stepOnLatter=true;

						if (creature[i].master!=0)
							if ((creature[i].trackRow!=row) || (creature[i].trackCol!=col))
							{
								stepOnLatter=false;
								doMove=false;
							}

						if (stepOnLatter)
						{
							cr_SteppedOnLatter(i,&row,&col);
							creature[i].attackTarget=	0;
							creature[i].trackRow		=	0;
							creature[i].trackCol		=	0;
							creature[i].tracking		=	false;

							if ((row<0) || (col<0))
							{
								doMove=false;
								rowDir	=	0;
								colDir	=	0;
								row=(int)(creature[i].row);
								col=(int)(creature[i].col);
							}
							else
							{
								creature[i].startRow	=	row;
								creature[i].startCol	=	col;
								creature[i].range			=	12;
							}
						}
					}
					else
					{
						doMove=false;
						creature[i].attackTarget=	0;  // 11/4
						creature[i].trackRow		=	0;  // 11/4
						creature[i].trackCol		=	0;  // 11/4
						creature[i].tracking		=	false;  // 11/4
					}
				}

				if ((doMove) || (forceMove))
				{

					if (creature[i].hitPoints<((float)creatureInfo[creature[i].id].maxHitPoints*0.10))
					{
						cr_DropBloodStain(creature[i].id,creature[i].row,creature[i].col);
					}
					else if (creature[i].hitPoints<((float)creatureInfo[creature[i].id].maxHitPoints*0.30))
					{
						if (tb_Rnd(1,2)==1)
							cr_DropBloodStain(creature[i].id,creature[i].row,creature[i].col);
					}
					else if (creature[i].hitPoints<((float)creatureInfo[creature[i].id].maxHitPoints*0.50))
					{
						if (tb_Rnd(1,3)==1)
							cr_DropBloodStain(creature[i].id,creature[i].row,creature[i].col);
					}
					else if (creature[i].hitPoints<((float)creatureInfo[creature[i].id].maxHitPoints*0.75))
					{
						if (tb_Rnd(1,4)==1)
							cr_DropBloodStain(creature[i].id,creature[i].row,creature[i].col);
					}

					creatureMap[creature[i].row][creature[i].col]	= 0;

					creature[i].row						=	(UInt16)row;
					creature[i].col						=	(UInt16)col;

					if (tb_Rnd(1,3)==1)
					{
						if ((tb_Rnd(1,400)<creatureInfo[creature[i].id].maxIntelligence))
							canReveal=true;
						else
							canReveal=false;

						if (creature[i].master==0)
						{
							if (row>0 && row<kMapWidth && col>0 && col<kMapHeight)
							{
								if (creatureMap[row-1][col-1]!=0)
								{
									if (creature[creatureMap[row-1][col-1]].playerIndex!=0)
										stinks=player[creature[creatureMap[row-1][col-1]].playerIndex].curse&kStenchCurse;

									if ((creature[creatureMap[row-1][col-1]].hiding) && (stinks || canReveal))
									{
										creature[creatureMap[row-1][col-1]].hiding=false;

										if (creature[creatureMap[row-1][col-1]].playerIndex!=0)
											if (pl_AuthorizedTo(creatureMap[row-1][col-1],kCanBeRevealed)==false) // put in carbon 11/27
												creature[creatureMap[row-1][col-1]].hiding=true;
									}

									/*if (creature[creatureMap[row-1][col-1]].playerIndex!=0)
									{
										j=creature[creatureMap[row-1][col-1]].playerIndex;

										if (player[j].stealth && pl_AuthorizedTo(player[j].creatureIndex, kRegularPlayer) && tb_Rnd(1,4)==1)
										{
											player[j].stealth		=	false;
											player[j].stealthCool	=	TickCount()+3*60;
											nw_SendTextToClient(j, "Something bumped into you.", 0);
											nw_SendDisplayMessage(j,kLeavingStealth);
										}
									}*/ //-remove random bumps
								}

								if (creatureMap[row][col-1]!=0)
								{
									if (creature[creatureMap[row][col-1]].playerIndex!=0)
										stinks=player[creature[creatureMap[row][col-1]].playerIndex].curse&kStenchCurse;

									if ((creature[creatureMap[row][col-1]].hiding) && (stinks || canReveal))
									{
										creature[creatureMap[row][col-1]].hiding=false;

										if (creature[creatureMap[row][col-1]].playerIndex!=0)
											if (pl_AuthorizedTo(creatureMap[row][col-1],kCanBeRevealed)==false) // put in carbon 11/27
												creature[creatureMap[row][col-1]].hiding=true;
									}

									/*if (creature[creatureMap[row][col-1]].playerIndex!=0)
									{
										j=creature[creatureMap[row][col-1]].playerIndex;

										if (player[j].stealth && pl_AuthorizedTo(player[j].creatureIndex, kRegularPlayer) && tb_Rnd(1,4)==1)
										{
											player[j].stealth		=	false;
											player[j].stealthCool	=	TickCount()+3*60;
											nw_SendTextToClient(j, "Something bumped into you.", 0);
											nw_SendDisplayMessage(j,kLeavingStealth);
										}
									}*/
								}

								if (creatureMap[row+1][col-1]!=0)
								{
									if (creature[creatureMap[row+1][col-1]].playerIndex!=0)
										stinks=player[creature[creatureMap[row+1][col-1]].playerIndex].curse&kStenchCurse;

									if ((creature[creatureMap[row+1][col-1]].hiding) && (stinks || canReveal))
									{
										creature[creatureMap[row+1][col-1]].hiding=false;

										if (creature[creatureMap[row+1][col-1]].playerIndex!=0)
											if (pl_AuthorizedTo(creatureMap[row+1][col-1],kCanBeRevealed)==false) // put in carbon 11/27
												creature[creatureMap[row+1][col-1]].hiding=true;
									}

									/*if (creature[creatureMap[row+1][col-1]].playerIndex!=0)
									{
										j=creature[creatureMap[row+1][col-1]].playerIndex;

										if (player[j].stealth && pl_AuthorizedTo(player[j].creatureIndex, kRegularPlayer) && tb_Rnd(1,4)==1)
										{
											player[j].stealth		=	false;
											player[j].stealthCool	=	TickCount()+3*60;
											nw_SendTextToClient(j, "Something bumped into you.", 0);
											nw_SendDisplayMessage(j,kLeavingStealth);
										}
									}*/
								}

								if (creatureMap[row-1][col]!=0)
								{
									if (creature[creatureMap[row-1][col]].playerIndex!=0)
										stinks=player[creature[creatureMap[row-1][col]].playerIndex].curse&kStenchCurse;

									if ((creature[creatureMap[row-1][col]].hiding) && (stinks || canReveal))
									{
										creature[creatureMap[row-1][col]].hiding=false;

										if (creature[creatureMap[row-1][col]].playerIndex!=0)
											if (pl_AuthorizedTo(creatureMap[row-1][col],kCanBeRevealed)==false) // put in carbon 11/27
												creature[creatureMap[row-1][col]].hiding=true;
									}

									/*if (creature[creatureMap[row-1][col]].playerIndex!=0)
									{
										j=creature[creatureMap[row-1][col]].playerIndex;

										if (player[j].stealth && pl_AuthorizedTo(player[j].creatureIndex, kRegularPlayer) && tb_Rnd(1,4)==1)
										{
											player[j].stealth		=	false;
											player[j].stealthCool	=	TickCount()+3*60;
											nw_SendTextToClient(j, "Something bumped into you.", 0);
											nw_SendDisplayMessage(j,kLeavingStealth);
										}
									}*/
								}

								if (creatureMap[row+1][col]!=0)
								{
									if (creature[creatureMap[row+1][col]].playerIndex!=0)
										stinks=player[creature[creatureMap[row+1][col]].playerIndex].curse&kStenchCurse;

									if ((creature[creatureMap[row+1][col]].hiding) && (stinks || canReveal))
									{
										creature[creatureMap[row+1][col]].hiding=false;

										if (creature[creatureMap[row+1][col]].playerIndex!=0)
											if (pl_AuthorizedTo(creatureMap[row+1][col],kCanBeRevealed)==false) // put in carbon 11/27
												creature[creatureMap[row+1][col]].hiding=true;
									}

									/*if (creature[creatureMap[row+1][col]].playerIndex!=0)
									{
										j=creature[creatureMap[row+1][col]].playerIndex;

										if (player[j].stealth && pl_AuthorizedTo(player[j].creatureIndex, kRegularPlayer) && tb_Rnd(1,4)==1)
										{
											player[j].stealth		=	false;
											player[j].stealthCool	=	TickCount()+3*60;
											nw_SendTextToClient(j, "Something bumped into you.", 0);
											nw_SendDisplayMessage(j,kLeavingStealth);
										}
									}*/
								}

								if (creatureMap[row-1][col+1]!=0)
								{
									if (creature[creatureMap[row-1][col+1]].playerIndex!=0)
										stinks=player[creature[creatureMap[row-1][col+1]].playerIndex].curse&kStenchCurse;

									if ((creature[creatureMap[row-1][col+1]].hiding) && (stinks || canReveal))
									{
										creature[creatureMap[row-1][col+1]].hiding=false;

										if (creature[creatureMap[row-1][col+1]].playerIndex!=0)
											if (pl_AuthorizedTo(creatureMap[row-1][col+1],kCanBeRevealed)==false) // put in carbon 11/27
												creature[creatureMap[row-1][col+1]].hiding=true;
									}

									/*if (creature[creatureMap[row-1][col+1]].playerIndex!=0)
									{
										j=creature[creatureMap[row-1][col+1]].playerIndex;

										if (player[j].stealth && pl_AuthorizedTo(player[j].creatureIndex, kRegularPlayer) && tb_Rnd(1,4)==1)
										{
											player[j].stealth		=	false;
											player[j].stealthCool	=	TickCount()+3*60;
											nw_SendTextToClient(j, "Something bumped into you.", 0);
											nw_SendDisplayMessage(j,kLeavingStealth);
										}
									}*/
								}

								if (creatureMap[row][col+1]!=0)
								{
									if (creature[creatureMap[row][col+1]].playerIndex!=0)
										stinks=player[creature[creatureMap[row][col+1]].playerIndex].curse&kStenchCurse;

									if ((creature[creatureMap[row][col+1]].hiding) && (stinks || canReveal))
									{
										creature[creatureMap[row][col+1]].hiding=false;

										if (creature[creatureMap[row][col+1]].playerIndex!=0)
											if (pl_AuthorizedTo(creatureMap[row][col+1],kCanBeRevealed)==false) // put in carbon 11/27
												creature[creatureMap[row][col+1]].hiding=true;
									}

									/*if (creature[creatureMap[row][col+1]].playerIndex!=0)
									{
										j=creature[creatureMap[row][col+1]].playerIndex;

										if (player[j].stealth && pl_AuthorizedTo(player[j].creatureIndex, kRegularPlayer) && tb_Rnd(1,4)==1)
										{
											player[j].stealth		=	false;
											player[j].stealthCool	=	TickCount()+3*60;
											nw_SendTextToClient(j, "Something bumped into you.", 0);
											nw_SendDisplayMessage(j,kLeavingStealth);
										}
									}*/
								}

								if (creatureMap[row+1][col+1]!=0)
								{
									if (creature[creatureMap[row+1][col+1]].playerIndex!=0)
										stinks=player[creature[creatureMap[row+1][col+1]].playerIndex].curse&kStenchCurse;

									if ((creature[creatureMap[row+1][col+1]].hiding) && (stinks || canReveal))
									{
										creature[creatureMap[row+1][col+1]].hiding=false;

										if (creature[creatureMap[row+1][col+1]].playerIndex!=0)
											if (pl_AuthorizedTo(creatureMap[row+1][col+1],kCanBeRevealed)==false) // put in carbon 11/27
												creature[creatureMap[row+1][col+1]].hiding=true;
									}

									/*if (creature[creatureMap[row+1][col+1]].playerIndex!=0)
									{
										j=creature[creatureMap[row+1][col+1]].playerIndex;

										if (player[j].stealth && pl_AuthorizedTo(player[j].creatureIndex, kRegularPlayer) && tb_Rnd(1,4)==1)
										{
											player[j].stealth		=	false;
											player[j].stealthCool	=	TickCount()+3*60;
											nw_SendTextToClient(j, "Something bumped into you.", 0);
											nw_SendDisplayMessage(j,kLeavingStealth);
										}
									}*/
								}
							}
						}
					}

					creatureMap[row][col]			= i; // creature[i].id;

					creature[i].hiding				= false;

					if ((creature[i].npcIndex!=0) && (creature[i].attackTarget!=0))
					{
						if (npc[creature[i].npcIndex].type==kNPCGuard)
							creature[i].nextMoveTime	=	currentTime + 32;  // guard attacking  //Dave 35;
						else creature[i].nextMoveTime	=	currentTime + 50; // other (brigand, etc)
					}
					else
					{
						if (creature[i].master==0)
							creature[i].nextMoveTime	=	currentTime+creatureInfo[creature[i].id].speed;
						else
						{
							creature[i].nextMoveTime	=	currentTime+creatureInfo[creature[i].id].speed;

							for (j=creatureInfo[creature[i].id].creatureLevel; j<=creature[i].level; j++)
							{
								creature[i].nextMoveTime-=2;
							}

							if (in_ItemEquipedServer(creature[creature[i].master].playerIndex, kCharmOfSwiftness))
								creature[i].nextMoveTime -= 5;
						}

						//if (creature[i].npcIndex!=0)
						//if (npc[creature[i].npcIndex].type!=kNPCGuard)
						//creature[i].nextMoveTime = creature[i].nextMoveTime+100; // make guards faster than everything
					}

					creature[i].timeStamp			= currentTime;

					if ((rowDir==-1) && (colDir==-1)) 	      { creature[i].moveDirection=0;     creature[i].facing=0; }
					else if ((rowDir==-1) && (colDir==0)) 		{ creature[i].moveDirection=1;     creature[i].facing=1; }
					else if ((rowDir==-1) && (colDir==1)) 		{ creature[i].moveDirection=2;     creature[i].facing=2; }
					else if ((rowDir==0) && (colDir==-1)) 		{ creature[i].moveDirection=3;     creature[i].facing=7; }

					else if ((rowDir==0) && (colDir==0))
					{
						creature[i].moveDirection=4;

						if (creature[i].facing<8)
							creature[i].facing=creature[i].facing+modifier;
					}

					else if ((rowDir==0) && (colDir==1)) 		  { creature[i].moveDirection=5;     creature[i].facing=3; }
					else if ((rowDir==1) && (colDir==-1)) 		{ creature[i].moveDirection=6;     creature[i].facing=6; }
					else if ((rowDir==1) && (colDir==0)) 		  { creature[i].moveDirection=7;     creature[i].facing=5; }

					if ((rowDir==1) && (colDir==1)) 		      { creature[i].moveDirection=8;     creature[i].facing=4; }

					//merge 7/17
					if (creature[i].master!=0)
					{
						if (pl_AuthorizedTo(creature[i].master,kCanLosePets)) // put in carbon 11/27
							if (creatureInfo[creature[i].id].tamingLevel>20)
							{
								if (creature[i].hunger>0)
									creature[i].hunger--;

								if (creature[i].hunger>0)
									creature[i].hunger--;
							}
							else
							{
								for (j=0;j<creatureInfo[creature[i].id].tamingLevel;j++)
									if (creature[i].hunger>0)
										creature[i].hunger--;
							}

						//merge 7/17
						if (creature[i].hunger<1750)
							if (creature[i].hungerfeedback==false)
							{
								nw_SendDisplayMessage(creature[creature[i].master].playerIndex, kPetStarving);
								creature[i].hungerfeedback=true;
							}
					}

					if (creature[i].master!=0)
					{
						if ((creature[i].command==kHeel) || ((creature[i].command==kGuard) && (creature[i].attackTarget==0)))
							creature[i].nextMoveTime	=	currentTime+30; // keep up with master

						//if (player[creature[creature[i].master].playerIndex].category==0)
						if (pl_AuthorizedTo(creature[i].master,kCanLosePets)) // put in carbon 11/27
							if (creature[i].hunger==0)  //starvation
							{
								for (j=0;j<kMaxPets;j++)
									if (player[creature[creature[i].master].playerIndex].pet[j]==i)
									{
										/*player[creature[creature[i].master].playerIndex].pet[j]					  =	0;
										  		player[creature[creature[i].master].playerIndex].petInStable[j]		= false;
										  		player[creature[creature[i].master].playerIndex].petType[j]				= 0;
										  		creature[i].master				                  =	0;
										  		creature[i].attackTarget	                  =	0;
										  		creature[i].trackRow			                  =	0;
										  		creature[i].trackCol			                  =	0;
										  		creature[i].tracking			                  =	false;
										  		creature[i].alignment							=kEvil;*/
										cr_setDieTime(i, TickCount());
										nw_SendDisplayMessage(creature[creature[i].master].playerIndex, kPetDies);
										//nw_SendTextToClient(creature[i].master, "Your pet dies from starvation!", 0);

										if (pl_AuthorizedTo(creature[i].master, kRegularPlayer))
											if (player[creature[creature[i].master].playerIndex].skill[kTaming].level>425)
												player[creature[creature[i].master].playerIndex].skill[kTaming].level=player[creature[creature[i].master].playerIndex].skill[kTaming].level-tb_Rnd(20,30);

										break;
									}
							}
					}
					else
					{
						if (containerMap[creature[i].row][creature[i].col]!=0)
							if (in_ContainerLocked(i)==false)
							{
								if (in_ContainerTrapped(i))
								{
									if (creature[i].intelligence>25 && creature[i].intelligence<70)
										in_CreaturePickUp(i);
								}
								else
									in_CreaturePickUp(i);
							}
					}

				}
			}

		}

#ifdef _FUNCTIONTRACEDEBUGGING
	fi_WriteToErrorLog("cr_MoveCreatures Exit");

#endif

}

// ----------------------------------------------------------------- //
TBoolean cr_CreatureWantsItem(int	c, int item)
// ----------------------------------------------------------------- //

{

	if (creature[c].npcIndex>0) // npcs check in quest
		return (false);

	if (creatureInfo[creature[c].id].seekWeapons)
		if (it_IsWeapon(item) || (it_IsArmor(item, false)))
			return (true);

	if (creatureInfo[creature[c].id].seekMagic)
		if (it_IsWeapon(item) || (it_IsArmor(item, false)))
			return (true);

	return (false);

}

// ----------------------------------------------------------------- //
void cr_ClearCreatureRecord(int	i)
// ----------------------------------------------------------------- //

{
	// Note:  remember to add fields to FileIO.C - fi_Logon
	int				j;

	if ((i<0) || (i>=kMaxCreatures))
	{
		fi_WriteToErrorLog("Out of Range - cr_ClearCreatureRecord");
		return;
	}

	creature[i].id																=	0;

	creature[i].row																=	0;
	creature[i].col																=	0;
	creature[i].nextMoveTime											=	0;
	creature[i].dieTime     											=	0;
	creature[i].moveDirection											= 0;
	creature[i].hiding														= false;
	creature[i].strength													= 0;
	creature[i].intelligence											= 0;
	creature[i].dexterity													= 0;
	creature[i].strengthOffset										= 0;
	creature[i].intelligenceOffset								= 0;
	creature[i].regeneratingTime		=0;
	creature[i].regeneratingPower		=0;
	creature[i].dexterityOffset										= 0;
	creature[i].strengthTime											= 0;
	creature[i].intelligenceTime									= 0;
	creature[i].dexterityTime											= 0;
	creature[i].hitPoints													= 0;
	creature[i].magicPoints												= 0;
	creature[i].armorRating												= 0;
	creature[i].stamina														= 0;
	creature[i].specialAttackType										= -1;
	creature[i].specialAttackArg1										= -1;
	creature[i].specialAttackArg2										= -1;
	creature[i].playerIndex												= 0;
	creature[i].npcIndex													= 0;
	creature[i].facing														= 0;
	creature[i].strikePose												= 0;
	creature[i].nextStrikeTime										= 0;
	creature[i].numberOfStrikes										= 0;
	creature[i].attackTarget											= 0;
	creature[i].nextAttackTime										= 0;
	creature[i].missedLastHit											= true;
	creature[i].activeFX													= false;
	creature[i].finalStrikeSound                  = 0;
	creature[i].soundFX														= 0;
	creature[i].soundFXTime												= 0;
	creature[i].soundID												    = 0;
	creature[i].trackRow													= 0;
	creature[i].trackCol													= 0;
	creature[i].tracking													= false;
	creature[i].master														= 0;
	creature[i].command														= 0;
	creature[i].poison														= 0;
	creature[i].nextStatTime											= 0;
	creature[i].paralyzed													= 0;
	creature[i].reflectDamage											= 0;
	creature[i].disorientation										= 0;
	creature[i].lifespan										      = 0;
	creature[i].calmUntil										      = 0;
	creature[i].magicReflection										= false;
	creature[i].hunger										        = 0;
	creature[i].hungerfeedback										= false;
	creature[i].beingAttacked										  = 0;
	creature[i].range										          = 0;
	creature[i].startRow										      = 0;
	creature[i].startCol										      = 0;
	creature[i].cornered										      = false;
	creature[i].wounded										        = false;
	creature[i].respawn										        = false;
	creature[i].level										          = 1;
	creature[i].goodTarget												= 0;
	creature[i].respawnTime										    = 0;
	creature[i].taunted												= 0;
	creature[i].home										          = 0;
	creature[i].skipSpell										      = 0;
	creature[i].nextTrackRow                      = 0;
	creature[i].nextTrackCol                      = 0;
	creature[i].possibleTarget                    = 0;
	strcpy(creature[i].talk, "");
	creature[i].talkWhen								          =	0;
	creature[i].talkTime								          =	0;
	creature[i].talkID                            = 0;
	creature[i].previousSituation									= 0;
	creature[i].mute								=false;
	creature[i].noDrop					 		 =false;
	creature[i].godMode							=false;
	creature[i].noDeath							=false;
	creature[i].autoLock							=false;
	creature[i].opportunityTime					=0;
	creature[i].damageReduc						=1.0f;

	for (j=0;j<kMaxCreatureEffects;j++)
	{
		creature[i].fx[j]													=	0;
		creature[i].fxTarget[j]										=	0;
		creature[i].fxTargetRow[j]								=	0;
		creature[i].fxTargetCol[j]								=	0;
	}

	for (j=0;j<kMaxInventorySize;j++)
	{
		creature[i].inventory[j].itemID						=	0;
		creature[i].inventory[j].itemCount				=	0;
	}

	creature[i].pathPtr=0;

	for (j=0;j<kMaxPath;j++)
	{
		creature[i].pathRow[j]										=	0;
		creature[i].pathRow[j]										=	0;
	}

}

// ----------------------------------------------------------------- //
void cr_ClearBloodStainRecord(int	i)
// ----------------------------------------------------------------- //

{

	if ((i<0) || (i>=kMaxBloodStains))
	{
		fi_WriteToErrorLog("Out of Range - cr_ClearBloodStainRecord");
		return;
	}

	bloodStain[i].id															=	0;

	bloodStain[i].row															=	0;
	bloodStain[i].col															=	0;
	bloodStain[i].expireTime											=	0;

}

// ----------------------------------------------------------------- //
void cr_ClearBloodStain(int	i)
// ----------------------------------------------------------------- //

{

	if ((i<0) || (i>=kMaxBloodStains))
	{
		fi_WriteToErrorLog("Out of Range - cr_ClearBloodStain");
		return;
	}

	if (bloodStain[i].row>=0)
		if (bloodStain[i].row<kMapWidth)
			if (bloodStain[i].col>=0)
				if (bloodStain[i].col<kMapHeight)
					bloodStainMap[bloodStain[i].row][bloodStain[i].col]=0;

	cr_ClearBloodStainRecord(i);

}

// ----------------------------------------------------------------- //
void cr_DropBloodStain(int theCreatureID, int row, int col)
// ----------------------------------------------------------------- //

{
	int						i;

	if (row<0)
	{
		fi_WriteToErrorLog("row<0 - cr_DropBloodStain");
		return;
	}

	if (row>=kMapWidth)
	{
		fi_WriteToErrorLog("row>kMapWidth - cr_DropBloodStain");
		return;
	}

	if (col<0)
	{
		fi_WriteToErrorLog("col<0 - cr_DropBloodStain");
		return;
	}

	if (col>=kMapHeight)
	{
		fi_WriteToErrorLog("col>kMapHeight - cr_DropBloodStain");
		return;
	}

	if (creatureInfo[theCreatureID].bleeds==false)
		return;

	if (bloodStainMap[row][col]!=0)
		return;

	for (i=0;i<kMaxBloodStains;i++)
		if (bloodStain[i].id==0)
		{
			bloodStain[i].id						=	tb_Rnd(kBloodStart,kBloodEnd);
			bloodStain[i].row						=	row;
			bloodStain[i].col						=	col;
			bloodStain[i].expireTime		=	TickCount()+(60*180);
			bloodStainMap[row][col]=i;
			return;
		}

}

// ----------------------------------------------------------------- //
void cr_ReloadCreatures(void)
// ----------------------------------------------------------------- //

{
	int					i;

	nextSpawnTime=TickCount();

	for (i=5000;i<kMaxCreatures;i++)
		if (creature[i].npcIndex==0)
			if (creature[i].master==0)
				pl_BanishCreature(-1, i);

	creatureSpawnPtr            = 5000;

	for (i=0;i<kMaxSpawnRegions;i++)
	{
		spawnRegion[i].creatureID=0;
		tb_SetRect(&spawnRegion[i].theRegion,0,0,0,0);
		spawnRegion[i].currentPopulation=0;
		spawnRegion[i].maxPopulation=0;
	}

	cr_CreatureControls();

}

// ----------------------------------------------------------------- //
void cr_InitCreatures(void)
// ----------------------------------------------------------------- //

{
	// NOTE:  map must be initialized before calling this function

	int					x;
	int					y;
	int					i;


	nextSpawnTime=TickCount();

	creatureSpawnPtr            = 5000;

	for (x=0;x<kMapWidth;x++)
		for (y=0;y<kMapHeight;y++)
		{
			creatureMap[x][y]=0;
			bloodStainMap[x][y]=0;
		}

	for (i=0;i<kMaxCreatures;i++)
		cr_ClearCreatureRecord(i);

	for (i=0;i<kMaxBloodStains;i++)
		cr_ClearBloodStainRecord(i);

	for (i=0;i<kMaxSpawnRegions;i++)
	{
		spawnRegion[i].creatureID=0;
		tb_SetRect(&spawnRegion[i].theRegion,0,0,0,0);
		spawnRegion[i].currentPopulation=0;
		spawnRegion[i].maxPopulation=0;
	}

	cr_CreatureControls();

}

// ----------------------------------------------------------------- //
TBoolean cr_CastCreatureSpell(int c1, int c2)
// ----------------------------------------------------------------- //
{
	int spellcast=tb_Rnd(1,100);
	int spellchance=0;
	int cpt;
						
	for (cpt=0; cpt<kMaxCreatureSpells; cpt++)
	{								
		if (spellchance<100)
		{
			spellchance=spellchance+creatureInfo[creature[c1].id].spellList[cpt][2];
		
			if (spellcast<spellchance)
			{
				return(sk_CastSpellOnCreature(c1, c2, creatureInfo[creature[c1].id].spellList[cpt][0], false));
			}
		}
			
	}
}

#endif

