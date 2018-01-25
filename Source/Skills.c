#include "Toolbox.h"
#include "Keys.h"
#include "Remap.h"
#include "Network.h"
#include "Inventory.h"
#include "Resource.h"
#include "Globals.h"
#include "Skills.h"
#include "Common.h"
#include "Creatures.h"
#include "Graphics.h"
#include "Players.h"
#include "Items.h"
#include "FX.h"
#include "FileIO.h"
#include "Async Sound.h"
#include "GameText.h"

int                                 maxSkillLevel[7][kMaxSkills];


extern	UInt8												resources					[kMapWidth][kMapHeight];
extern int													targetMode;
extern creatureType									creature					[kMaxCreatures];
extern UInt16												creatureMap				[kMapWidth][kMapHeight];
extern playerLocalSkillsType				localSkills				[kMaxSkills];
extern UInt16												map								[kMapWidth][kMapHeight];
extern UInt16												playerRow;
extern UInt16												playerCol;
extern TBoolean											canSwimIn					[kMaxIcons];
extern UInt8												localRightHand;
extern UInt8												localLeftHand;
extern UInt8												localLegs;
extern UInt8												localTorso;
extern UInt8												localFeet;
extern UInt8												localRightFinger;
extern UInt8												localLeftFinger;
extern UInt8												localHead;
extern UInt8												localNeck;
extern UInt8												localHands;
extern UInt8												localWaist;
extern UInt8												localClass;
extern int                          ginsengSpawn;
extern int                          lotusFlowerSpawn;
extern int                          mandrakeRootSpawn;
extern int                          garlicSpawn;
extern int                          volcanicAshSpawn;
extern int                          mushroomSpawn;//Dave
//extern int                          eggSpawn;//Dave

extern unsigned long                chatMessageID;
extern unsigned long                soundMessageID;
extern spawnRegionType				spawnRegion[kMaxSpawnRegions];

extern WindowPtr                    makeWindow;
extern int                          tbMakeWindow;
extern TBoolean                     windowVisible[kMaxWindows];
extern UInt16				        containerState[kMaxScreenElements];
extern UInt16				        bloodStainState[kMaxScreenElements];
extern UInt16						state[kMaxScreenElements];
extern UInt8						creatureFacing[kMaxScreenElements];

extern unsigned long				tracksDecayTime	      [kMaxTracks];

extern guildHallType				guildHalls[kMaxGuildHalls];
extern int							guildMap[kMapWidth][kMapHeight];

extern int							tracksMap[kMapWidth][kMapHeight];

extern TBoolean						questChar[kMaxScreenElements];

extern UInt8						currentCycle;

extern TBoolean						fullMoon;

extern unsigned long				currentlyTracking;

int mana; //Dave
															//Dave add 11-29-2010
//extern npcType						npc[kMaxNPCs]; 

// ----------------------------------------------------------------- //
void sk_CalculateStats(int p)	// server
// ----------------------------------------------------------------- //

{
	int           i;
	int           level=sk_GetCharacterLevel(p);
	float         total=60;

	for (i=1;i<level;i++)
		total=total+30;

	if (player[p].characterClass==1) // cleric
	{
		creature[player[p].creatureIndex].dexterity     = tb_Round(total * 0.25);
		creature[player[p].creatureIndex].intelligence  = tb_Round(total * 0.50);
		creature[player[p].creatureIndex].strength      = tb_Round(total * 0.25);
	}
	else if (player[p].characterClass==2) // druid
	{
		creature[player[p].creatureIndex].dexterity     = tb_Round(total * 0.25);
		creature[player[p].creatureIndex].intelligence  = tb_Round(total * 0.40);
		creature[player[p].creatureIndex].strength      = tb_Round(total * 0.35);
	}
	else if (player[p].characterClass==3) // fighter
	{
		creature[player[p].creatureIndex].dexterity     = tb_Round(total * 0.40);
		creature[player[p].creatureIndex].intelligence  = tb_Round(total * 0.20);
		creature[player[p].creatureIndex].strength      = tb_Round(total * 0.40);
	}
	else if (player[p].characterClass==4) // ranger
	{
		creature[player[p].creatureIndex].dexterity     = tb_Round(total * 0.25);
		creature[player[p].creatureIndex].intelligence  = tb_Round(total * 0.30);
		creature[player[p].creatureIndex].strength      = tb_Round(total * 0.45);
	}
	else if (player[p].characterClass==5) // rogue
	{
		creature[player[p].creatureIndex].dexterity     = tb_Round(total * 0.35);
		creature[player[p].creatureIndex].intelligence  = tb_Round(total * 0.35);
		creature[player[p].creatureIndex].strength      = tb_Round(total * 0.30);
	}
	else if (player[p].characterClass==6) // wizard
	{
		creature[player[p].creatureIndex].dexterity     = tb_Round(total * 0.25);
		creature[player[p].creatureIndex].intelligence  = tb_Round(total * 0.45);
		creature[player[p].creatureIndex].strength      = tb_Round(total * 0.30);
	}
	else if (player[p].characterClass==7) // generic
	{
		creature[player[p].creatureIndex].dexterity     = tb_Round(total * 0.33);
		creature[player[p].creatureIndex].intelligence  = tb_Round(total * 0.33);
		creature[player[p].creatureIndex].strength      = tb_Round(total * 0.34);
	}


}

// ----------------------------------------------------------------- //
UInt8 sk_GetCharacterLevel(int p)	// server
// ----------------------------------------------------------------- //

{
	UInt32			start=100;
	UInt32      points=0;
	int					level=1;
	//int					i;
	//float       totalStatPoints;

	//if ((player[p].category==1) || (player[p].category==2))

	if (pl_AuthorizedTo(player[p].creatureIndex,kIsAnyGM)) // add to carbon 11/27
	{
		if (player[p].god)
			return (100);
		else
			return (sk_GetSkillLevel(player[p].skill[kAlchemyCleric].level));
	}
	else return (pl_GetCharacterLevel(p));

}

// ----------------------------------------------------------------- //
void sk_InitializeSkills(void)
// ----------------------------------------------------------------- //

{
	//Cleric

	maxSkillLevel[1][kAlchemyDruid]               =0;
	maxSkillLevel[1][kAlchemyCleric]              =200;
	maxSkillLevel[1][kPoisoning]              	=0;
	maxSkillLevel[1][kAnatomy]                    =0;
	maxSkillLevel[1][kArchery]                    =0;
	maxSkillLevel[1][kBlacksmithing]              =200;
	maxSkillLevel[1][kCooking]                    =200;
	maxSkillLevel[1][kDetectingHidden]            =1;
	maxSkillLevel[1][kEnchanting]					=0;
	maxSkillLevel[1][kFishing]                    =200;
	maxSkillLevel[1][kHealing]                    =200;
	maxSkillLevel[1][kHiding]                     =3;
	maxSkillLevel[1][kLockpicking]                =1;
	maxSkillLevel[1][kLumberjacking]              =200;
	//maxSkillLevel[1][kMaceFighting]               =0;
	maxSkillLevel[1][kMageryDruid]                =0;
	maxSkillLevel[1][kMageryCleric]               =200;
	maxSkillLevel[1][kMageryWizard]               =0;
	maxSkillLevel[1][kMeditation]                 =200;
	maxSkillLevel[1][kMelee]                		=0;
	maxSkillLevel[1][kMining]                     =200;
	maxSkillLevel[1][kTracking]                 =0;
	maxSkillLevel[1][kParrying]                   =0;
	//maxSkillLevel[1][kPoleFighting]               =0;
	maxSkillLevel[1][kRemoveTrap]                 =0;
	maxSkillLevel[1][kResistingMagic]             =200;
	maxSkillLevel[1][kSpecialFighter]				=0;
	maxSkillLevel[1][kSpecialRanger]				=0;
	maxSkillLevel[1][kSpecialRogue]					=0;
	//maxSkillLevel[1][kSwordsmanship]              =0;
	//maxSkillLevel[1][kDaggers]               		=0;
	maxSkillLevel[1][kTailoringLeatherworking]    =200;
	maxSkillLevel[1][kTaming]                     =2;
	maxSkillLevel[1][kStealth]                   	=0;
	maxSkillLevel[1][kTinkering]                  =200;
	maxSkillLevel[1][kWoodworking]                =200;
	maxSkillLevel[1][kWrestling]                  =200;

	//Druid

	maxSkillLevel[2][kAlchemyDruid]               =200;
	maxSkillLevel[2][kAlchemyCleric]              =0;
	maxSkillLevel[2][kPoisoning]              	=0;
	maxSkillLevel[2][kAnatomy]                    =0;
	maxSkillLevel[2][kArchery]                    =0;
	maxSkillLevel[2][kBlacksmithing]              =200;
	maxSkillLevel[2][kCooking]                    =200;
	maxSkillLevel[2][kDetectingHidden]            =1;
	maxSkillLevel[2][kEnchanting]					=0;
	maxSkillLevel[2][kFishing]                    =200;
	maxSkillLevel[2][kHealing]                    =3;
	maxSkillLevel[2][kHiding]                     =3;
	maxSkillLevel[2][kLockpicking]                =1;
	maxSkillLevel[2][kLumberjacking]              =200;
	//maxSkillLevel[2][kMaceFighting]               =0;
	maxSkillLevel[2][kMageryDruid]                =200;
	maxSkillLevel[2][kMageryCleric]               =0;
	maxSkillLevel[2][kMageryWizard]               =0;
	maxSkillLevel[2][kMeditation]                 =200;
	maxSkillLevel[2][kMelee]                		=0;
	maxSkillLevel[2][kSpecialFighter]				=0;
	maxSkillLevel[2][kSpecialRanger]				=0;
	maxSkillLevel[2][kSpecialRogue]					=0;
	maxSkillLevel[2][kMining]                     =200;
	maxSkillLevel[2][kTracking]                 =0;
	maxSkillLevel[2][kParrying]                   =0;
	//maxSkillLevel[2][kPoleFighting]               =0;
	maxSkillLevel[2][kRemoveTrap]                 =0;
	maxSkillLevel[2][kResistingMagic]             =200;
	//maxSkillLevel[2][kSwordsmanship]              =0;
	//maxSkillLevel[2][kDaggers]               		=0;
	maxSkillLevel[2][kTailoringLeatherworking]    =200;
	maxSkillLevel[2][kTaming]                     =200;
	maxSkillLevel[2][kStealth]                   	=0;
	maxSkillLevel[2][kTinkering]                  =200;
	maxSkillLevel[2][kWoodworking]                =200;
	maxSkillLevel[2][kWrestling]                  =200;

	//Fighter

	maxSkillLevel[3][kAlchemyDruid]               =0;
	maxSkillLevel[3][kAlchemyCleric]              =0;
	maxSkillLevel[3][kPoisoning]              	=0;
	maxSkillLevel[3][kAnatomy]                    =200;
	maxSkillLevel[3][kArchery]                    =0;
	maxSkillLevel[3][kBlacksmithing]              =200;
	maxSkillLevel[3][kCooking]                    =200;
	maxSkillLevel[3][kDetectingHidden]            =1;
	maxSkillLevel[3][kEnchanting]					=0;
	maxSkillLevel[3][kFishing]                    =200;
	maxSkillLevel[3][kHealing]                    =5;
	maxSkillLevel[3][kHiding]                     =1;
	maxSkillLevel[3][kLockpicking]                =1;
	maxSkillLevel[3][kLumberjacking]              =200;
	//maxSkillLevel[3][kMaceFighting]               =200;
	maxSkillLevel[3][kMageryDruid]                =0;
	maxSkillLevel[3][kMageryCleric]               =0;
	maxSkillLevel[3][kMageryWizard]               =0;
	maxSkillLevel[3][kMeditation]                 =0;
	maxSkillLevel[3][kMelee]                		=200;
	maxSkillLevel[3][kSpecialFighter]				=200;
	maxSkillLevel[3][kSpecialRanger]				=0;
	maxSkillLevel[3][kSpecialRogue]					=0;
	maxSkillLevel[3][kMining]                     =200;
	maxSkillLevel[3][kTracking]                 	=0;
	maxSkillLevel[3][kParrying]                   =200;
	//maxSkillLevel[3][kPoleFighting]               =200;
	maxSkillLevel[3][kRemoveTrap]                 =0;
	maxSkillLevel[3][kResistingMagic]             =5; //Dave =4 changed to 5 - 03-07-2012
	//maxSkillLevel[3][kSwordsmanship]              =200;
	//maxSkillLevel[3][kDaggers]               		=200;
	maxSkillLevel[3][kTailoringLeatherworking]    =200;
	maxSkillLevel[3][kTaming]                     =0;
	maxSkillLevel[3][kStealth]                   	=0;
	maxSkillLevel[3][kTinkering]                  =200;
	maxSkillLevel[3][kWoodworking]                =200;
	maxSkillLevel[3][kWrestling]                  =200;

	//Ranger

	maxSkillLevel[4][kAlchemyDruid]               =0;
	maxSkillLevel[4][kAlchemyCleric]              =0;
	maxSkillLevel[4][kPoisoning]              	=0;
	maxSkillLevel[4][kAnatomy]                    =6;
	maxSkillLevel[4][kArchery]                    =200;
	maxSkillLevel[4][kBlacksmithing]              =200;
	maxSkillLevel[4][kCooking]                    =200;
	maxSkillLevel[4][kDetectingHidden]            =2;
	maxSkillLevel[4][kEnchanting]					=0;
	maxSkillLevel[4][kFishing]                    =200;
	maxSkillLevel[4][kHealing]                    =5;
	maxSkillLevel[4][kHiding]                     =4;
	maxSkillLevel[4][kLockpicking]                =2;
	maxSkillLevel[4][kLumberjacking]              =200;
	//maxSkillLevel[4][kMaceFighting]               =2;
	maxSkillLevel[4][kMageryDruid]                =0;
	maxSkillLevel[4][kMageryCleric]               =0;
	maxSkillLevel[4][kMageryWizard]               =0;
	maxSkillLevel[4][kMeditation]                 =0;
	maxSkillLevel[4][kMelee]                		=200;
	maxSkillLevel[4][kSpecialFighter]				=0;
	maxSkillLevel[4][kSpecialRanger]				=200;
	maxSkillLevel[4][kSpecialRogue]					=0;
	maxSkillLevel[4][kMining]                     =200;
	maxSkillLevel[4][kTracking]                 =200;
	maxSkillLevel[4][kParrying]                   =5;
	//maxSkillLevel[4][kPoleFighting]               =2;
	maxSkillLevel[4][kRemoveTrap]                 =0;
	maxSkillLevel[4][kResistingMagic]             =5;
	//maxSkillLevel[4][kSwordsmanship]              =200;
	//maxSkillLevel[4][kDaggers]               		=200;
	maxSkillLevel[4][kTailoringLeatherworking]    =200;
	maxSkillLevel[4][kTaming]                     =3;
	maxSkillLevel[4][kStealth]                   	=0;
	maxSkillLevel[4][kTinkering]                  =200;
	maxSkillLevel[4][kWoodworking]                =200;
	maxSkillLevel[4][kWrestling]                  =200;

	//Rogue

	maxSkillLevel[5][kAlchemyDruid]               =0;
	maxSkillLevel[5][kAlchemyCleric]              =0;
	maxSkillLevel[5][kPoisoning]              	=200;
	maxSkillLevel[5][kAnatomy]                    =200;
	maxSkillLevel[5][kArchery]                    =0;
	maxSkillLevel[5][kBlacksmithing]              =200;
	maxSkillLevel[5][kCooking]                    =200;
	maxSkillLevel[5][kDetectingHidden]            =200;
	maxSkillLevel[5][kEnchanting]					=0;
	maxSkillLevel[5][kFishing]                    =200;
	maxSkillLevel[5][kHealing]                    =5;
	maxSkillLevel[5][kHiding]                     =200;
	maxSkillLevel[5][kLockpicking]                =200;
	maxSkillLevel[5][kLumberjacking]              =200;
	//maxSkillLevel[5][kMaceFighting]               =0;
	maxSkillLevel[5][kMageryDruid]                =0;
	maxSkillLevel[5][kMageryCleric]               =0;
	maxSkillLevel[5][kMageryWizard]               =0;
	maxSkillLevel[5][kMeditation]                 =0;
	maxSkillLevel[5][kMelee]                		=200;
	maxSkillLevel[5][kMining]                     =200;
	maxSkillLevel[5][kTracking]                 =2;
	maxSkillLevel[5][kParrying]                   =0;
	//maxSkillLevel[5][kPoleFighting]               =0;
	maxSkillLevel[5][kRemoveTrap]                 =200;
	maxSkillLevel[5][kResistingMagic]             =5;
	//maxSkillLevel[5][kSwordsmanship]              =2;
	//maxSkillLevel[5][kDaggers]               		=200;
	maxSkillLevel[5][kSpecialFighter]				=0;
	maxSkillLevel[5][kSpecialRanger]				=0;
	maxSkillLevel[5][kSpecialRogue]				=200;
	maxSkillLevel[5][kTailoringLeatherworking]    =200;
	maxSkillLevel[5][kTaming]                     =0;
	maxSkillLevel[5][kStealth]                   	=200;
	maxSkillLevel[5][kTinkering]                  =200;
	maxSkillLevel[5][kWoodworking]                =200;
	maxSkillLevel[5][kWrestling]                  =200;

	//Wizard

	maxSkillLevel[6][kAlchemyDruid]               =0;
	maxSkillLevel[6][kAlchemyCleric]              =0;
	maxSkillLevel[6][kPoisoning]              	=0;
	maxSkillLevel[6][kAnatomy]                    =0;
	maxSkillLevel[6][kArchery]                    =0;
	maxSkillLevel[6][kBlacksmithing]              =200;
	maxSkillLevel[6][kCooking]                    =200;
	maxSkillLevel[6][kDetectingHidden]            =1;
	maxSkillLevel[6][kEnchanting]					=200;
	maxSkillLevel[6][kFishing]                    =200;
	maxSkillLevel[6][kHealing]                    =3;
	maxSkillLevel[6][kHiding]                     =3;
	maxSkillLevel[6][kLockpicking]                =1;
	maxSkillLevel[6][kLumberjacking]              =200;
	//maxSkillLevel[6][kMaceFighting]               =0;
	maxSkillLevel[6][kMageryDruid]                =0;
	maxSkillLevel[6][kMageryCleric]               =0;
	maxSkillLevel[6][kMageryWizard]               =200;
	maxSkillLevel[6][kMeditation]                 =200;
	maxSkillLevel[6][kMelee]                		=0;
	maxSkillLevel[6][kMining]                     =200;
	maxSkillLevel[6][kTracking]                	=0;
	maxSkillLevel[6][kParrying]                   =0;
	//maxSkillLevel[6][kPoleFighting]               =0;
	maxSkillLevel[6][kRemoveTrap]                 =0;
	maxSkillLevel[6][kResistingMagic]             =200;
	//maxSkillLevel[6][kSwordsmanship]              =0;
	//maxSkillLevel[6][kDaggers]               		=0;
	maxSkillLevel[6][kSpecialFighter]				=0;
	maxSkillLevel[6][kSpecialRanger]				=0;
	maxSkillLevel[6][kSpecialRogue]				=0;
	maxSkillLevel[6][kTailoringLeatherworking]    =200;
	maxSkillLevel[6][kTaming]                     =2;
	maxSkillLevel[6][kStealth]                   	=0;
	maxSkillLevel[6][kTinkering]                  =200;
	maxSkillLevel[6][kWoodworking]                =200;
	maxSkillLevel[6][kWrestling]                  =200;

}

// ----------------------------------------------------------------- //
void sk_RaiseSkill(int i, int attacker, int skill, int amount) // server
// ----------------------------------------------------------------- //

{
	UInt32        currentLevel;


	if (player[i].noGain) // mod 8/4
		return;

	if (maxSkillLevel[player[i].characterClass][skill]==0)
		return;

	if ((creature[player[i].creatureIndex].row>1076) && (creature[player[i].creatureIndex].row<1150))  // no skill gain in pvp
		if ((creature[player[i].creatureIndex].col>1681) && (creature[player[i].creatureIndex].col<=1765))
			return;

	if (pl_IsThief(player[i].creatureIndex))	// 11/2/01
		return;

	if (attacker!=0)
	{
		if (player[i].normalAttacker==attacker)
			if (player[i].fightingSkillGained>18)
				return;

		if (player[i].normalAttacker!=attacker)
			player[i].fightingSkillGained=0;

		player[i].normalAttacker=attacker;

		player[i].fightingSkillGained++;
	}

	if (amount>2)
		amount=2;
	else if (amount<1)
		amount=1;

	if (player[i].category!=0 && player[i].category!=8) // merge 7/27
		//if (pl_AuthorizedTo(player[i].creatureIndex,kIsAnyGM)) // add to carbon 11/27
		return;

	if (creature[player[i].creatureIndex].hunger==0)
		return;

	currentLevel=sk_GetSkillLevel(player[i].skill[skill].level);

	if (currentLevel>=maxSkillLevel[player[i].characterClass][skill])
		return;

	player[i].skill[skill].level=player[i].skill[skill].level+amount;

	player[i].lastSkillGain=TickCount();

	if ((sk_GetSkillLevel(player[i].skill[skill].level)>currentLevel))
	{
		skillGain++;
		nw_SendSkillChange(i, skill, 1);
		nw_SendSkillsRefresh(i);
	}

	pl_CalculateStats(i);

}

// ----------------------------------------------------------------- //
TBoolean sk_StandingNearFire(void)
// ----------------------------------------------------------------- //

{
	int i=creatureFacing[90];

	if (containerState[108]==8)
		if (i==6)
			return (true);

	if (containerState[89]==8)
		if (i==5)
			return (true);

	if (containerState[71]==8)
		if (i==4)
			return (true);

	if (containerState[109]==8)
		if (i==7)
			return (true);

	if (containerState[90]==8)     return (true);

	if (containerState[72]==8)
		if (i==3)
			return (true);

	if (containerState[110]==8)
		if (i==0)
			return (true);

	if (containerState[91]==8)
		if (i==1)
			return (true);

	if (containerState[73]==8)
		if (i==2)
			return (true);



	if (containerState[108]==50)
		if (i==6)
			return (true);

	if (containerState[89]==50)
		if (i==5)
			return (true);

	if (containerState[71]==50)
		if (i==4)
			return (true);

	if (containerState[109]==50)
		if (i==7)
			return (true);

	if (containerState[90]==50)     return (true);

	if (containerState[72]==50)
		if (i==3)
			return (true);

	if (containerState[110]==50)
		if (i==0)
			return (true);

	if (containerState[91]==50)
		if (i==1)
			return (true);

	if (containerState[73]==50)
		if (i==2)
			return (true);



	if (clientMap[playerRow+13-1][playerCol+13-1]==738)
		if (i==0)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13]==738)
		if (i==1)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13+1]==738)
		if (i==2)
			return (true);

	if (clientMap[playerRow+13][playerCol+13-1]==738)
		if (i==7)
			return(true);

	if (clientMap[playerRow+13][playerCol+13+1]==738)
		if (i==3)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13-1]==738)
		if (i==6)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13]==738)
		if (i==5)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13+1]==738)
		if (i==4)
			return (true);

	/*if (clientMap[playerRow+13-1][playerCol+13-1]==644) 			return (true);
	else if (clientMap[playerRow+13-1][playerCol+13]==644) 	  return (true);
	else if (clientMap[playerRow+13-1][playerCol+13+1]==644)  return (true);
	else if (clientMap[playerRow+13][playerCol+13-1]==644) 	  return (true);*/
	if (clientMap[playerRow+13][playerCol+13+1]==644)
	{
		if (i==2 || i==3)
			return (true);
	}

	/*else if (clientMap[playerRow+13+1][playerCol+13-1]==644)  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13]==644) 	  return (true);*/
	else if (clientMap[playerRow+13+1][playerCol+13+1]==644)
	{
		if (i==4 || i==3)
			return (true);
	}


	/*if (clientMap[playerRow+13-1][playerCol+13-1]==645) 			return (true);
	else if (clientMap[playerRow+13-1][playerCol+13]==645) 	  return (true);*/
	if (clientMap[playerRow+13-1][playerCol+13+1]==645)
	{
		if (i==2)
			return (true);
	}

	//else if (clientMap[playerRow+13][playerCol+13-1]==645) 	  return (true);
	else if (clientMap[playerRow+13][playerCol+13+1]==645)
	{
		if (i==2 || i==3)
			return (true);
	}

	/*else if (clientMap[playerRow+13+1][playerCol+13-1]==645)  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13]==645) 	  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13+1]==645)  return (true);*/


	/*if (clientMap[playerRow+13-1][playerCol+13-1]==683) 			return (true);
	else if (clientMap[playerRow+13-1][playerCol+13]==683) 	  return (true);
	else if (clientMap[playerRow+13-1][playerCol+13+1]==683)  return (true);*/
	if (clientMap[playerRow+13][playerCol+13+1]==683)
		if (i==3)
			return (true);

	/*else if (clientMap[playerRow+13][playerCol+13+1]==683) 	  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13-1]==683)  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13]==683) 	  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13+1]==683)  return (true);*/

	//if (clientMap[playerRow+13-1][playerCol+13-1]==692) 			return (true);
	if (clientMap[playerRow+13+1][playerCol+13]==692)
		if (i==5)
			return (true);

	/*else if (clientMap[playerRow+13-1][playerCol+13+1]==692)  return (true);
	else if (clientMap[playerRow+13][playerCol+13-1]==692) 	  return (true);
	else if (clientMap[playerRow+13][playerCol+13+1]==692) 	  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13-1]==692)  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13]==692) 	  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13+1]==692)  return (true);*/

	if (clientMap[playerRow+13-1][playerCol+13-1]==933)
		if (i==0)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13]==933)
		if (i==1)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13+1]==933)
		if (i==2)
			return (true);

	if (clientMap[playerRow+13][playerCol+13-1]==933)
		if (i==7)
			return(true);

	if (clientMap[playerRow+13][playerCol+13+1]==933)
		if (i==3)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13-1]==933)
		if (i==6)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13]==933)
		if (i==5)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13+1]==933)
		if (i==4)
			return (true);

	//if (clientMap[playerRow+13-1][playerCol+13-1]==784) 			return (true);
	if (clientMap[playerRow+13+1][playerCol+13]==784)
		if (i==5)
			return (true);

	/*else if (clientMap[playerRow+13-1][playerCol+13+1]==784)  return (true);
	else if (clientMap[playerRow+13][playerCol+13-1]==784) 	  return (true);
	else if (clientMap[playerRow+13][playerCol+13+1]==784) 	  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13-1]==784)  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13]==784) 	  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13+1]==784)  return (true);*/

	if (clientMap[playerRow+13-1][playerCol+13-1]==972)
		if (i==0)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13]==972)
		if (i==1)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13+1]==972)
		if (i==2)
			return (true);

	if (clientMap[playerRow+13][playerCol+13-1]==972)
		if (i==7)
			return(true);

	if (clientMap[playerRow+13][playerCol+13+1]==972)
		if (i==3)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13-1]==972)
		if (i==6)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13]==972)
		if (i==5)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13+1]==972)
		if (i==4)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13-1]==983)
		if (i==0)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13]==983)
		if (i==1)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13+1]==983)
		if (i==2)
			return (true);

	if (clientMap[playerRow+13][playerCol+13-1]==983)
		if (i==7)
			return(true);

	if (clientMap[playerRow+13][playerCol+13+1]==983)
		if (i==3)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13-1]==983)
		if (i==6)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13]==983)
		if (i==5)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13+1]==983)
		if (i==4)
			return (true);

	/*if (clientMap[playerRow+13-1][playerCol+13-1]==1180) 			return (true);
	else if (clientMap[playerRow+13-1][playerCol+13]==1180) 	  return (true);
	else if (clientMap[playerRow+13-1][playerCol+13+1]==1180)  return (true);
	else if (clientMap[playerRow+13][playerCol+13-1]==1180) 	  return (true);
	else if (clientMap[playerRow+13][playerCol+13+1]==1180) 	  return (true);*/
	if (clientMap[playerRow+13+1][playerCol+13-1]==1180)
		if (i==6)
			return (true);

	if (clientMap[playerRow+13+1][playerCol+13]==1180)
		if (i==5)
			return(true);

	//else if (clientMap[playerRow+13+1][playerCol+13+1]==1180)  return (true);

	/*if (clientMap[playerRow+13-1][playerCol+13-1]==1181) 			return (true);
	else if (clientMap[playerRow+13-1][playerCol+13]==1181) 	  return (true);
	else if (clientMap[playerRow+13-1][playerCol+13+1]==1181)  return (true);
	else if (clientMap[playerRow+13][playerCol+13-1]==1181) 	  return (true);
	else if (clientMap[playerRow+13][playerCol+13+1]==1181) 	  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13-1]==1181)  return (true);*/
	if (clientMap[playerRow+13+1][playerCol+13]==1181)
		if (i==5)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13+1]==1181)
		if (i==4)
			return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean sk_StandingNearForge(void)
// ----------------------------------------------------------------- //

{
	int i=creatureFacing[90];

	if (clientMap[playerRow+13-1][playerCol+13-1]==738)
		if (i==0)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13]==738)
		if (i==1)
			return (true);

	if (clientMap[playerRow+13-1][playerCol+13+1]==738)
		if (i==2)
			return (true);

	if (clientMap[playerRow+13][playerCol+13-1]==738)
		if (i==7)
			return(true);

	if (clientMap[playerRow+13][playerCol+13+1]==738)
		if (i==3)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13-1]==738)
		if (i==6)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13]==738)
		if (i==5)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13+1]==738)
		if (i==4)
			return (true);

	/*if (clientMap[playerRow+13-1][playerCol+13-1]==644) 			return (true);
	else if (clientMap[playerRow+13-1][playerCol+13]==644) 	  return (true);
	else if (clientMap[playerRow+13-1][playerCol+13+1]==644)  return (true);
	else if (clientMap[playerRow+13][playerCol+13-1]==644) 	  return (true);*/
	if (clientMap[playerRow+13][playerCol+13+1]==644)
	{
		if (i==2 || i==3)
			return (true);
	}

	/*else if (clientMap[playerRow+13+1][playerCol+13-1]==644)  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13]==644) 	  return (true);*/
	else if (clientMap[playerRow+13+1][playerCol+13+1]==644)
	{
		if (i==4 || i==3)
			return (true);
	}


	/*if (clientMap[playerRow+13-1][playerCol+13-1]==645) 			return (true);
	else if (clientMap[playerRow+13-1][playerCol+13]==645) 	  return (true);*/
	if (clientMap[playerRow+13-1][playerCol+13+1]==645)
	{
		if (i==2)
			return (true);
	}

	//else if (clientMap[playerRow+13][playerCol+13-1]==645) 	  return (true);
	else if (clientMap[playerRow+13][playerCol+13+1]==645)
	{
		if (i==2 || i==3)
			return (true);
	}

	/*else if (clientMap[playerRow+13+1][playerCol+13-1]==645)  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13]==645) 	  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13+1]==645)  return (true);*/



	/*if (clientMap[playerRow+13-1][playerCol+13-1]==1180) 			return (true);
	else if (clientMap[playerRow+13-1][playerCol+13]==1180) 	  return (true);
	else if (clientMap[playerRow+13-1][playerCol+13+1]==1180)  return (true);
	else if (clientMap[playerRow+13][playerCol+13-1]==1180) 	  return (true);
	else if (clientMap[playerRow+13][playerCol+13+1]==1180) 	  return (true);*/
	if (clientMap[playerRow+13+1][playerCol+13-1]==1180)
		if (i==6)
			return (true);

	if (clientMap[playerRow+13+1][playerCol+13]==1180)
		if (i==5)
			return(true);

	//else if (clientMap[playerRow+13+1][playerCol+13+1]==1180)  return (true);



	/*if (clientMap[playerRow+13-1][playerCol+13-1]==1181) 			return (true);
	else if (clientMap[playerRow+13-1][playerCol+13]==1181) 	  return (true);
	else if (clientMap[playerRow+13-1][playerCol+13+1]==1181)  return (true);
	else if (clientMap[playerRow+13][playerCol+13-1]==1181) 	  return (true);
	else if (clientMap[playerRow+13][playerCol+13+1]==1181) 	  return (true);
	else if (clientMap[playerRow+13+1][playerCol+13-1]==1181)  return (true);*/
	if (clientMap[playerRow+13+1][playerCol+13]==1181)
		if (i==5)
			return(true);

	if (clientMap[playerRow+13+1][playerCol+13+1]==1181)
		if (i==4)
			return (true);


	return (false);

}

// ----------------------------------------------------------------- //
UInt8 sk_GetSkillLevel(UInt32 points)	// server
// ----------------------------------------------------------------- //

{
	UInt32			start=100;
	int					level=1;
	int					i;

	for (i=0;i<25;i++)
		if (points<start)
			return (level);
		else
		{
			level++;

			if (level>=200)
				return (level);

			start=start*2;
		}

	return (level);
}

// ----------------------------------------------------------------- //
UInt32 sk_GetSkillNextLevel(UInt32 points)	// server
// ----------------------------------------------------------------- //

{
	UInt32			start=100;

	while (start<=points)
		start*=2;

	return (start);
}

// ----------------------------------------------------------------- //
void sk_GetPositionOf(int	p, int t, int *row, int *col)	// server
// ----------------------------------------------------------------- //

{
	int           missRow;
	int           missCol;
	int           range;

	if ((creature[player[t].creatureIndex].row<218) || (creature[player[t].creatureIndex].col<221))
	{
		*row=0;
		*col=0;
		return;
	}

	range=sk_GetSkillLevel(player[p].skill[kTracking].level);

	if (range>10)
		range=10;

	missRow=tb_Rnd(0,10-range);

	missCol=tb_Rnd(0,10-range);

	if (missRow>0)
		if (tb_Rnd(1,2)==1)
			missRow=-1*missRow;

	if (missCol>0)
		if (tb_Rnd(1,2)==1)
			missCol=-1*missCol;

	missRow=missRow/10;

	missCol=missCol/10;

	*row=creature[player[t].creatureIndex].row+missRow-218;

	*col=creature[player[t].creatureIndex].col+missCol-221;

}

#ifdef _SERVER
// ----------------------------------------------------------------- //
void sk_SendFailedHarvestMessage(int	i)	// server
// ----------------------------------------------------------------- //

{

	if (player[i].gatherType==255)
		nw_SendDisplayMessage(i,kFailedToMake);

	else if (player[i].gatherType==kLumberjacking)
		nw_SendDisplayMessage(i,kNoLumberHarvest);

	else if (player[i].gatherType==kMining)
		nw_SendDisplayMessage(i,kNoOreHarvest);

	else if (player[i].gatherType==kFishing)
		nw_SendDisplayMessage(i,kNoFishCaught);

	player[i].gathering								=	0;

	player[i].gatherType							= 0;

	player[i].gatherRow								=	0;

	player[i].gatherCol								=	0;

	creature[player[i].creatureIndex].strikePose				= 0;

	creature[player[i].creatureIndex].nextStrikeTime		= 0;

	creature[player[i].creatureIndex].numberOfStrikes		= 0;

}

#endif

// ----------------------------------------------------------------- //
void sk_UseSkill(int skill)	// client
// ----------------------------------------------------------------- //

{
#ifndef _SERVERONLY
	TBoolean							canUseSkill=false;
	//int                   missRow;
	//int                   missCol;
	//char									navStr[kStringLength];
	//char									temp[kStringLength];
	//int                   range;
	int						i=creatureFacing[90];
//	TBoolean							attackMode=false;

//	if (targetMode==kAttackTarget)
//		attackMode=true;


	if (playerDead)
		return;

	if (localClass<=6 && !questChar[90])
		if (maxSkillLevel[localClass][skill]==0)
			if ((localRightFinger==0 || itemList[itemList[localInventory[localRightFinger].itemID].spell].makeSkill!=skill) && (localLeftFinger==0 || itemList[itemList[localInventory[localLeftFinger].itemID].spell].makeSkill!=skill))
				return;

	ClearPlayerAction();

	//------------------------------------------------------------
	if (skill==kTailoringLeatherworking)
	{
		if (!in_ItemTypeEquiped(kSewingToolType))
		{
			if (in_PlayerHasItemType(kSewingToolType)>0)
			{
				gr_AddText("You remove your shears from your bag.",false,true);
				in_UseItem(in_PlayerHasItemType(kSewingToolType));
			}
			else
			{
				gr_AddText("You must have a pair of shears.",false,true);
				return;
			}
		}

		gr_OpenFloatingWindow(tbMakeWindow);

		UpdateEvent(tbMakeWindow);
		windowVisible[kMakeWindow]=true;
		gr_RenderMakeListWindow(kTailoringLeatherworking, 0);
		return;
	}

	//------------------------------------------------------------

	if ((skill==kAlchemyDruid) || (skill==kAlchemyCleric) || (skill==kPoisoning) || (skill==kEnchanting))
	{
		if (!in_ItemTypeEquiped(kEmptyHands))
		{
			gr_AddText("You free your hands.",false,true);

			if (localRightHand!=0)
				in_UnEquipItem(localRightHand);

			if (localLeftHand!=0)
				in_UnEquipItem(localLeftHand);
		}

		currentInfoDisplay=kMakeDisplay;

		gr_OpenFloatingWindow(tbMakeWindow);
		UpdateEvent(tbMakeWindow);
		windowVisible[kMakeWindow]=true;
		
		if (skill==kEnchanting)
			gr_RenderMakeListWindow(skill, 0);
		else
			gr_RenderMakeListWindow(skill, kAllCategories);
	}

	if (skill==kCooking)
	{
		if (!in_ItemTypeEquiped(kEmptyHands))
		{
			gr_AddText("You free your hands.",false,true);

			if (localRightHand!=0)
				in_UnEquipItem(localRightHand);

			if (localLeftHand!=0)
				in_UnEquipItem(localLeftHand);
		}

		if (sk_StandingNearFire())
		{
			currentInfoDisplay=kMakeDisplay;
			gr_OpenFloatingWindow(tbMakeWindow);
			UpdateEvent(tbMakeWindow);
			windowVisible[kMakeWindow]=true;
			gr_RenderMakeListWindow(skill, kAllCategories);
		}
		else
			gr_AddText("You must be facing a fire to cook.",false,true);
	}

	if ((skill==kMageryCleric) || (skill==kMageryDruid) || (skill==kMageryWizard))
	{
		currentInfoDisplay=kMakeDisplay;
		gr_OpenFloatingWindow(tbMakeWindow);
		UpdateEvent(tbMakeWindow);
		windowVisible[kMakeWindow]=true;
		gr_RenderMakeListWindow(skill, kAllCategories);
	}

	if ((skill==kSpecialFighter) || (skill==kSpecialRogue) || skill==kSpecialRanger)
	{
		currentInfoDisplay=kMakeDisplay;
		gr_OpenFloatingWindow(tbMakeWindow);
		UpdateEvent(tbMakeWindow);
		windowVisible[kMakeWindow]=true;
		gr_RenderMakeListWindow(skill, kAllCategories);
	}

	//------------------------------------------------------------

	if ((skill==kMeditation) && (nw_CanSend()))
	{
		gr_AddText("You enter a meditative trance...",false,true);
		nw_SendAction(kMeditate,kNormalPriority,kSendNow,true);
	}

	if ((skill==kLockpicking) && (nw_CanSend()))
	{
		nw_SendAction(kPickLock,kNormalPriority,kSendNow,true);
	}

	if ((skill==kRemoveTrap) && (nw_CanSend()))
	{
		nw_SendAction(kDisarmTrap,kNormalPriority,kSendNow,true);
	}

	if (skill==kLumberjacking)
	{
		if (!(in_ItemTypeEquiped(kAxeWeaponType) || in_ItemTypeEquiped(kLumberjackToolType)))
		{
			if (in_PlayerHasItemType(kLumberjackToolType)>0)
			{
				gr_AddText("You arm your axe.",false,true);
				in_UseItem(in_PlayerHasItemType(kLumberjackToolType));
			}
			else if (in_PlayerHasItemType(kAxeWeaponType)>0)
			{
				gr_AddText("You arm your axe.",false,true);
				in_UseItem(in_PlayerHasItemType(kAxeWeaponType));
			}
			else
			{
				gr_AddText("You must have a wood chopping tool.",false,true);
				return;
			}
		}

		gr_AddText("Target a tree...",false,false);

		targetMode=kLumberjacking;
	}

	//------------------------------------------------------------

	if (skill==kTracking)
	{

		if (currentlyTracking!=0)  //Cooldown
			return;

		if ((playerRow>1842) || (playerCol>1842))
		{
			gr_AddText("It is too dark for you to track anything.",false,true);
			return;
		}
		else
			nw_SendAction(kTrack,kNormalPriority,kSendNow,true);

	}

	//------------------------------------------------------------

	if (skill==kWoodworking)
	{
		if (!in_ItemTypeEquiped(kWoodToolType))
		{
			if (in_PlayerHasItemType(kWoodToolType)>0)
			{
				gr_AddText("You remove your woodworking tools from your bag.",false,true);
				in_UseItem(in_PlayerHasItemType(kWoodToolType));
			}
			else
			{
				gr_AddText("You must have woodworking tools.",false,true);
				return;
			}
		}

		currentInfoDisplay=kMakeDisplay;

		//gr_RenderMakeList(true, skill, kAllCategories);
		gr_OpenFloatingWindow(tbMakeWindow);
		//ShowWindow(makeWindow);
		//SelectWindow(makeWindow);
		UpdateEvent(tbMakeWindow);
		windowVisible[kMakeWindow]=true;
		gr_RenderMakeListWindow(skill, kAllCategories);
	}

	//------------------------------------------------------------

	if (skill==kTinkering)
	{
		if (!in_ItemTypeEquiped(kTinkerToolType))
		{
			if (in_PlayerHasItemType(kTinkerToolType)>0)
			{
				gr_AddText("You remove your tinkering tools from your bag.",false,true);
				in_UseItem(in_PlayerHasItemType(kTinkerToolType));
			}
			else
			{
				gr_AddText("You must have tinkering tools.",false,true);
				return;
			}
		}

		currentInfoDisplay=kMakeDisplay;

		//gr_RenderMakeList(true, skill, kAllCategories);
		gr_OpenFloatingWindow(tbMakeWindow);
		//ShowWindow(makeWindow);
		//SelectWindow(makeWindow);
		UpdateEvent(tbMakeWindow);
		windowVisible[kMakeWindow]=true;
		gr_RenderMakeListWindow(skill, kAllCategories);
	}

	//------------------------------------------------------------

	if (skill==kBlacksmithing)
	{
		if (!in_ItemTypeEquiped(kBlacksmithToolType))
		{
			if (in_PlayerHasItemType(kBlacksmithToolType)>0)
			{
				gr_AddText("You remove your blacksmithing hammer from your bag.",false,true);
				in_UseItem(in_PlayerHasItemType(kBlacksmithToolType));
			}
			else
			{
				gr_AddText("You must have a blacksmithing hammer.",false,true);
				return;
			}
		}

		if (sk_StandingNearForge())
		{
			currentInfoDisplay=kMakeDisplay;
			gr_OpenFloatingWindow(tbMakeWindow);
			//ShowWindow(makeWindow);
			//SelectWindow(makeWindow);
			UpdateEvent(tbMakeWindow);
			windowVisible[kMakeWindow]=true;
			gr_RenderMakeListWindow(skill, 0);
			//gr_RenderMakeList(true, skill, kAllCategories);
		}
		else
			gr_AddText("You must be facing a forge to blacksmith.",false,true);
	}

	//------------------------------------------------------------

	if (skill==kMining)
	{
		if (!in_ItemTypeEquiped(kMiningToolType))
		{
			if (in_PlayerHasItemType(kMiningToolType)>0)
			{
				gr_AddText("You remove your mining axe from your bag.",false,true);
				in_UseItem(in_PlayerHasItemType(kMiningToolType));
			}
			else
			{
				gr_AddText("You must have a mining axe.",false,true);
				return;
			}
		}

		gr_AddText("Target a rock...",false,false);

		targetMode=kMining;
	}

	//------------------------------------------------------------

	if (skill==kHealing)
	{
		if (in_PlayerHas(kBandage)==0)
			gr_AddText("You don't have any bandages.",false,true);
		else
		{
			gr_AddText("Target a creature...",false,false);
			targetMode=kHealing;
		}
	}

	//------------------------------------------------------------

	if (skill==kTaming)
	{
		gr_AddText("Target a creature...",false,false);
		targetMode=kTameTarget;
	}

	//------------------------------------------------------------

	if (skill==kStealth)
	{
		nw_SendAction(kEnterStealth,kNormalPriority,kCanWait,true);
	}

	if (skill==kAnatomy)
	{
		gr_AddText("Target a creature...",false,false);
		targetMode=kAnatomy;
	}

	//if (skill==kAnimalLore)
	//	{
	//		gr_AddText("Target a creature...",false,false);
	//		targetMode=kAnimalLore;
	//	}

	//if (skill==kEvaluatingIntelligence)
	//	{
	//		gr_AddText("Target a creature...",false,false);
	//		targetMode=kEvaluatingIntelligence;
	//	}

	if (skill==kHiding)
		nw_SendAction(kHide,kNormalPriority,kCanWait,true);

	if (skill==kDetectingHidden)
		nw_SendAction(kDetectHidden,kNormalPriority,kCanWait,true);

	if (skill==kFishing)
	{
		if (!in_ItemTypeEquiped(kFishingToolType))
		{
			if (in_PlayerHasItemType(kFishingToolType)>0)
			{
				gr_AddText("You remove your fishing pole from your bag.",false,true);
				in_UseItem(in_PlayerHasItemType(kFishingToolType));
			}
			else
			{
				gr_AddText("You must have a fishing pole.",false,true);
				return;
			}
		}

		canUseSkill=false;

		if (canSwimIn[clientMap[playerRow+13-1][playerCol+13-1]])
			if (i==0)
				canUseSkill=true;

		if (canSwimIn[clientMap[playerRow+13-1][playerCol+13]])
			if (i==1)
				canUseSkill=true;

		if (canSwimIn[clientMap[playerRow+13-1][playerCol+13+1]])
			if (i==2)
				canUseSkill=true;

		if (canSwimIn[clientMap[playerRow+13][playerCol+13-1]])
			if (i==7)
				canUseSkill=true;

		if (canSwimIn[clientMap[playerRow+13][playerCol+13+1]])
			if (i==3)
				canUseSkill=true;

		if (canSwimIn[clientMap[playerRow+13+1][playerCol+13-1]])
			if (i==6)
				canUseSkill=true;

		if (canSwimIn[clientMap[playerRow+13+1][playerCol+13]])
			if (i==5)
				canUseSkill=true;

		if (canSwimIn[clientMap[playerRow+13+1][playerCol+13+1]])
			if (i==4)
				canUseSkill=true;

		if (canUseSkill)
		{
			gr_AddText("You begin fishing...",false,true);
			nw_SendAction(kFish,kNormalPriority,kCanWait,true);
		}
		else
			gr_AddText("You must be standing near water to fish.",false,true);

	}

	//if ((attackMode==true) && (targetMode!=kAttackTarget))
	//	gr_DrawButtons(true);
#endif
}

// ----------------------------------------------------------------- //
TBoolean sk_IsUseableSkill(int skill)	// client
// ----------------------------------------------------------------- //

{

	if (skill==kAlchemyDruid)						  return (true);

	if (skill==kAlchemyCleric)						return (true);

	if (skill==kPoisoning)						return (true);

	if (skill==kAnatomy)									return (true);

	if (skill==kBlacksmithing)						return (true);

	if (skill==kCooking)									return (true);

	if (skill==kDetectingHidden)					return (true);
	
	if (skill==kEnchanting)								return(true);

	if (skill==kFishing)									return (true);

	if (skill==kHealing)									return (true);

	if (skill==kHiding)										return (true);

	if (skill==kLockpicking)							return (true);

	if (skill==kLumberjacking)						return (true);

	if (skill==kMageryDruid)							return (true);

	if (skill==kMageryCleric)							return (true);

	if (skill==kMageryWizard)							return (true);

	if (skill==kMeditation)								return (true);

	if (skill==kSpecialFighter)							return (true);

	if (skill==kSpecialRanger)							return (true);

	if (skill==kSpecialRogue)								return (true);

	if (skill==kMining)										return (true);

	if (skill==kTracking)								return (true);

	if (skill==kRemoveTrap)								return (true);

	if (skill==kTailoringLeatherworking)	return (true);

	if (skill==kTaming)										return (true);

	if (skill==kTinkering)								return (true);

	if (skill==kWoodworking)							return (true);

	if (skill==kStealth)									return (true);

	return (false);

}

// ----------------------------------------------------------------- //
void sk_ClearLocalSkills(void)	// client
// ----------------------------------------------------------------- //

{
	int					i;

	for (i=0;i<kMaxSkills;i++)
	{
		localSkills[i].level			=	0;
		//localSkills[i].direction	=	0;
	}

}

#ifdef _SERVER
// ----------------------------------------------------------------- //
void sk_Healing(int i, int c) // server
// ----------------------------------------------------------------- //

{
	TBoolean						gotLucky=false;
	//int									chance;


	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	if (c>0)
		if (creature[c].playerIndex!=0)
			if (player[creature[c].playerIndex].dead)
			{
				nw_SendDisplayMessage(i,kHealingFailed);
				return;
			}

// if (player[i].category!=5)  // merge 7/28
	if (pl_AuthorizedTo(player[i].creatureIndex,kIsQuestScoundrel)==false) // add to carbon 11/27
		if (player[i].creatureIndex!=c)
			if (pl_IsThief(c))	// 017
			{
				nw_SendDisplayMessage(i,kHealingFailed);	// can't heal a scoundrel
				return;
			}

	if (cr_DistanceBetween(player[i].creatureIndex, c)>1)
	{
		nw_SendDisplayMessage(i,kCreatureTooFarAway);
		return;
	}

	if ((creature[c].playerIndex!=0) && (tb_Rnd(1,10)<=sk_GetSkillLevel(player[i].skill[kHealing].level)))
	{
		nw_SendDisplayMessage(i,kApplyingBandages);
		player[creature[c].playerIndex].healing=true;
		nw_SendDisplayMessage(creature[c].playerIndex,kBeingBandaged);

		if (creature[c].hitPoints<creature[c].strength) // 018
			sk_RaiseSkill(i,0,kHealing,5);
	}
	else
		nw_SendDisplayMessage(i,kHealingFailed);

}

// ----------------------------------------------------------------- //
void sk_Taming(int i, int c) // server
// ----------------------------------------------------------------- //

{
	int									j, cpt;
	TBoolean						foundPetSpot;
	int                 currentlyControlling;
	//int                 maxPets;
	TBoolean            found;
	int									row;	// merge 7/17
	int									col;	// merge 7/17
	TBoolean						trapped;	// merge 7/17

	//char debugStr[kStringLength]; //CS

	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	if (cr_ViewBlocked(creature[player[i].creatureIndex].row, creature[player[i].creatureIndex].col, creature[c].row, creature[c].col))
	{
		nw_SendDisplayMessage(i,kCantSeeTarget);
		return;
	}

	if (creature[c].playerIndex!=0)
	{
		nw_SendDisplayMessage(i,kTamingFailed);
		return;
	}

	if (creature[c].master!=0)	// already tame
	{
		nw_SendDisplayMessage(i,kAlreadyTame);
		return;
	}

	if (creature[c].npcIndex!=0 && pl_AuthorizedTo(i, kRegularPlayer)) //taming a player or NPC
	{
		nw_SendDisplayMessage(i,kTamingFailed);
		return;
	}

	if (pl_AuthorizedTo(player[i].creatureIndex, kRegularPlayer)==false)
		goto AutoGM;

	if (creatureInfo[creature[c].id].tamingLevel>100)  //if 101 = non tamable 
	{
		nw_SendDisplayMessage(i,kTamingFailed);
		return;
	}

	if ((creatureInfo[creature[c].id].tamingLevel>sk_GetSkillLevel(player[i].skill[kTaming].level)) && (player[i].category==0 || player[i].category==8)) //creature level too high
	{
		nw_SendDisplayMessage(i,kTamingFailed);
		return;
	}

	/*if (((creatureInfo[creature[c].id].tamingLevel>=8) && (tb_Rnd(1,15)>1)) && (player[i].category==0 || player[i].category==8))// dragons really hard // merge 7/15
		{
			nw_SendDisplayMessage(i,kTamingFailed);
			return;
		}*/


	if ((creature[player[i].creatureIndex].attackTarget==c) && (player[i].category==0 || player[i].category==8))	// can't tame attack target
	{
		nw_SendDisplayMessage(i,kTamingFailed);
		return;
	}

	if ((creature[c].attackTarget!=0) && (player[i].category==0 || player[i].category==8))	// unlikely to tame attacking creature
		if (tb_Rnd(1,5)>1)
		{
			nw_SendDisplayMessage(i,kTamingFailed);
			return;
		}

	for (j=1;j<creature[c].level;j++)
		if (tb_Rnd(1,10)==1)
		{
			nw_SendDisplayMessage(i,kTamingFailed);
			return;
		}


	// merge 7/15
	row=creature[c].row;

	col=creature[c].col;

	trapped=true;

	if (creatureMap[row-1][col-1]==0)
		if (cr_InDomain(c,row-1,col-1))
			trapped=false;

	if (trapped)
		if (creatureMap[row-1][col]==0)
			if (cr_InDomain(c,row-1,col))
				trapped=false;

	if (trapped)
		if (creatureMap[row-1][col+1]==0)
			if (cr_InDomain(c,row-1,col+1))
				trapped=false;

	if (trapped)
		if (creatureMap[row][col-1]==0)
			if (cr_InDomain(c,row,col-1))
				trapped=false;

	if (trapped)
		if (creatureMap[row][col+1]==0)
			if (cr_InDomain(c,row,col+1))
				trapped=false;

	if (trapped)
		if (creatureMap[row+1][col-1]==0)
			if (cr_InDomain(c,row+1,col-1))
				trapped=false;

	if (trapped)
		if (creatureMap[row+1][col]==0)
			if (cr_InDomain(c,row+1,col))
				trapped=false;

	if (trapped)
		if (creatureMap[row+1][col+1]==0)
			if (cr_InDomain(c,row+1,col+1))
				trapped=false;

	if ((trapped) && (player[i].category==0 || player[i].category==8))	// can't tame trapped creature
	{
		nw_SendDisplayMessage(i,kTamingFailed);
		return;
	}

	// merge 7/15


	currentlyControlling=0;

	for (j=0;j<kMaxPets;j++)
		if (player[i].pet[j]!=0)
			if (player[i].petInStable[j]==false)
				currentlyControlling++;

	/*maxPets=sk_GetSkillLevel(player[i].skill[kTaming].level);
	if (maxPets>3)
	  maxPets=3;
	if (currentlyControlling>=maxPets)*/
	if (currentlyControlling>0)
	{
		nw_SendDisplayMessage(i,kTooManyCreatures);
		return;
	}

	if (tb_Rnd(1,5)==1)
AutoGM:
	{

		foundPetSpot=false;

		for (j=0;j<kMaxPets;j++)
			if ((player[i].pet[j]==0) && (player[i].petInStable[j]==false))
			{
				player[i].pet[j] 				= c;
				player[i].petInStable[j]		= false;
				player[i].petType[j]			= creature[c].id;
				foundPetSpot					= true;

				if (creature[c].level>1)
				{
					creature[c].petExp				= 100;
					cpt=2;

					while (cpt<creature[c].level)
					{
						cpt++;
						creature[c].petExp*=2;
					}

					creature[c].petExp+=tb_Rnd(10,20);
				}
				else
					creature[c].petExp				= 0;

				break;

			}

		if (foundPetSpot==false)
		{
			nw_SendDisplayMessage(i,kTamingFailed);
			return;
		}

		nw_SendDisplayMessage(i,kBeastHasBeenTamed);

		// sk_RaiseSkill(i,kTaming,creatureInfo[creature[c].id].tamingLevel);

		//if (creature[player[i].creatureIndex].attackTarget==c)
		//creature[player[i].creatureIndex].attackTarget=0;

		cr_ClearAttackTarget(c);

		if (creature[c].home>0)
		{
			spawnRegion[creature[c].home].currentPopulation--;

			if (spawnRegion[creature[c].home].currentPopulation<0)
				spawnRegion[creature[c].home].currentPopulation++;

			creature[c].home =	0;

		}

		creature[c].master					=	player[i].creatureIndex;

		creature[c].command					=	kHeel;
		creature[c].attackTarget		=	0;
		creature[c].nextStrikeTime	=	0;
		creature[c].numberOfStrikes	=	0;
		creature[c].strikePose			=	0;
		creature[c].trackRow				= 0;
		creature[c].trackCol				= 0;
		creature[c].tracking				= false;
		creature[c].hunger          = 5000;
		creature[c].hungerfeedback	= false;
		creature[c].level           = creatureInfo[creature[c].id].creatureLevel;
		creature[c].alignment		= kGood;
		creature[c].outOfStable		= TickCount();
		creature[c].attackCounter	= 0;
		//creature[c].timeStamp				= TickCount();

		if (creature[c].facing>=10)
			creature[c].facing=creature[c].facing-10;

		for (cpt=0; cpt<kMaxMacroTrace; cpt++)
		{
			creature[i].attackTargets[cpt][0]=0;
			creature[i].attackTargets[cpt][1]=0;
		}

		player[i].sendPetsRefresh=true;

		// -------------------- anti macroing code -----------------

		for (j=0;j<kMaxMacroTrace;j++)
			if (player[i].tamingTarget[j]==c)
				return;

		found=false;

		for (j=0;j<kMaxMacroTrace;j++)
			if (player[i].tamingTarget[j]==0)
			{
				player[i].tamingTarget[j]=c;
				found=true;
				break;
			}

		if (found==false)
		{
			for (j=1;j<kMaxMacroTrace;j++)
				player[i].tamingTarget[j-1]=player[i].tamingTarget[j];

			player[i].tamingTarget[kMaxMacroTrace-1]=c;
		}

		// ----------------------------------------------------------

		//sk_RaiseSkill(i,0,kTaming,creatureInfo[creature[c].id].tamingLevel); No more raising on taming

	}
	else
		nw_SendDisplayMessage(i,kTamingFailed);

}

// ----------------------------------------------------------------- //
float sk_GetMagicResistance(int p) // server
// ----------------------------------------------------------------- //

{
	float       resist;
	float       low;        // merge 6/29/01
	float       high;       // merge 6/29/01
	TBoolean		wearingRobeOfResistance=false;
	int					c1;
	//int					itemDamage;

	resist=(float)sk_GetSkillLevel(player[p].skill[kResistingMagic].level)*10;

	if (in_ItemEquipedServer(p,kGrayRobeResistance))							wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kLightBlueRobeResistance))		wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kRedRobeResistance))					wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kBlueRobeResistance))					wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kBrownRobeResistance))				wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,KBlackRobeResistance))				wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kGreenRobeResistance))				wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kPurpleRobeResistance))				wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kYellowRobeResistance))				wearingRobeOfResistance=true;

	c1=player[p].creatureIndex;
	
	if (wearingRobeOfResistance)  // lowers resistance as robe wears
	{
		if (creature[c1].inventory[player[p].arm[kTorso]].itemCount>100) resist=resist*2;
		else if (creature[c1].inventory[player[p].arm[kTorso]].itemCount>80) resist=resist*1.9;
		else if (creature[c1].inventory[player[p].arm[kTorso]].itemCount>60) resist=resist*1.8;
		else if (creature[c1].inventory[player[p].arm[kTorso]].itemCount>40) resist=resist*1.7;
		else if (creature[c1].inventory[player[p].arm[kTorso]].itemCount>20) resist=resist*1.5;
		else if (creature[c1].inventory[player[p].arm[kTorso]].itemCount>0) resist=resist*1.3;
	}

	if (resist>99)
		resist=99;

	high=resist;                                    // merge 6/29/01

	low=0.60*resist;                                // merge 6/29/01

	resist=tb_Rnd(tb_Round(low),tb_Round(high));    // merge 6/29/01

	if (in_ItemEquipedServer(p,kGMRobe))
		resist=100;
	
	if (creature[c1].godMode)
		resist=99;

	resist=(100-resist)/100;

	return (resist);

}

// ----------------------------------------------------------------- //
void sk_DamageMagicRobe(int p) // server
// ----------------------------------------------------------------- //

{
	TBoolean		wearingRobeOfResistance=false;
	int					c1;
	int					itemDamage;

	if (in_ItemEquipedServer(p,kGrayRobeResistance))							wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kLightBlueRobeResistance))		wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kRedRobeResistance))					wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kBlueRobeResistance))					wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kBrownRobeResistance))				wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,KBlackRobeResistance))				wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kGreenRobeResistance))				wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kPurpleRobeResistance))				wearingRobeOfResistance=true;
	else if (in_ItemEquipedServer(p,kYellowRobeResistance))				wearingRobeOfResistance=true;

	if (wearingRobeOfResistance)
	{
		c1=player[p].creatureIndex;
		itemDamage=1;//tb_Rnd(3,6);//15;

		if (creature[c1].inventory[player[p].arm[kTorso]].itemCount-itemDamage<0)
			itemDamage=creature[c1].inventory[player[p].arm[kTorso]].itemCount;

		nw_SendToClientDropFromInventorySlot(creature[c1].playerIndex,kDamageItem,player[p].arm[kTorso],itemDamage, 0);

		if (creature[c1].inventory[player[p].arm[kTorso]].itemCount>0)
			creature[c1].inventory[player[p].arm[kTorso]].itemCount=creature[c1].inventory[player[p].arm[kTorso]].itemCount-itemDamage;

		if (creature[c1].inventory[player[p].arm[kTorso]].itemCount==0)
		{
			if (itemList[creature[c1].inventory[player[p].arm[kTorso]].itemID].desiredCount>0)
				in_ItemDestroyed(creature[c1].inventory[player[p].arm[kTorso]].itemID, 1);

			creature[c1].inventory[player[p].arm[kTorso]].itemID=0;

			player[p].arm[kTorso]=0;
		}
	}

}

// ----------------------------------------------------------------- //
TBoolean sk_CastSpellOnCreature(int cc, int ct, int spell, TBoolean fieldSpell) // server
// ----------------------------------------------------------------- //

{
	float									damage;
	float									heal;
	float									poison;
	float									resist=0;
	//int										fxIndex;
	int										j;
	unsigned long					currentTime;
	int										reflect;
	float                 bonus;
	int                   effect;
	TBoolean              retailiate;
	TBoolean			  usingRing;
	int					  ringDamage;
	int						level;


	usingRing=false;
	//Check for Ring & damage it

	if (creature[cc].npcIndex!=0) //npc casting <=> ring
	{
		usingRing=true;
		cr_SpeakSpell(cc, ct, spell);
	}

	if (creature[cc].playerIndex!=0)
	{

		if (player[creature[cc].playerIndex].arm[kRightFinger]!=0 && itemList[creature[cc].inventory[player[creature[cc].playerIndex].arm[kRightFinger]].itemID].spell==spell)
		{
			usingRing=true;
			ringDamage=tb_Rnd(10,15);
			nw_SendToClientDropFromInventorySlot(cc,kDamageItem,player[creature[cc].playerIndex].arm[kRightFinger],ringDamage, 0);

			if (creature[cc].inventory[player[creature[cc].playerIndex].arm[kRightFinger]].itemCount<=ringDamage)
			{
				if (itemList[creature[cc].inventory[player[creature[cc].playerIndex].arm[kRightFinger]].itemID].desiredCount>0)
					in_ItemDestroyed(creature[cc].inventory[player[creature[cc].playerIndex].arm[kRightFinger]].itemID, 1);

				creature[cc].inventory[player[creature[cc].playerIndex].arm[kRightFinger]].itemID=0;

				player[creature[cc].playerIndex].arm[kRightFinger]=0;
			}
			else
				creature[cc].inventory[player[creature[cc].playerIndex].arm[kRightFinger]].itemCount=creature[cc].inventory[player[creature[cc].playerIndex].arm[kRightFinger]].itemCount-ringDamage;


		}

		if (!usingRing)
			if (player[creature[cc].playerIndex].arm[kLeftFinger]!=0 && itemList[creature[cc].inventory[player[creature[cc].playerIndex].arm[kLeftFinger]].itemID].spell==spell)
			{
				usingRing=true;
				ringDamage=tb_Rnd(10,15);
				nw_SendToClientDropFromInventorySlot(cc,kDamageItem,player[creature[cc].playerIndex].arm[kLeftFinger],ringDamage, 0);

				if (creature[cc].inventory[player[creature[cc].playerIndex].arm[kLeftFinger]].itemCount<=ringDamage)
				{
					if (itemList[creature[cc].inventory[player[creature[cc].playerIndex].arm[kLeftFinger]].itemID].desiredCount>0)
						in_ItemDestroyed(creature[cc].inventory[player[creature[cc].playerIndex].arm[kLeftFinger]].itemID, 1);

					creature[cc].inventory[player[creature[cc].playerIndex].arm[kLeftFinger]].itemID=0;

					player[creature[cc].playerIndex].arm[kLeftFinger]=0;
				}
				else
					creature[cc].inventory[player[creature[cc].playerIndex].arm[kLeftFinger]].itemCount=creature[cc].inventory[player[creature[cc].playerIndex].arm[kLeftFinger]].itemCount-ringDamage;
			}
	}

	currentTime=TickCount();

	//-- get magic bonus -------------------------------------------------------------

	bonus = 1.0;

	if (creature[cc].playerIndex!=0)
	{
		if (itemList[creature[cc].inventory[player[creature[cc].playerIndex].arm[kHead]].itemID].itemType==kWizardHatType)
			bonus = bonus + 0.15;
			
		if (creatureInfo[creature[ct].id].undead)
			if (in_ItemEquipedServer(creature[cc].playerIndex, kCharmOfUndeath))
				bonus+=0.3;
	}
	else
	{
		//for (j=1;j<creature[cc].level;j++)
			//bonus = bonus + 0.05;
			
		bonus = 1+creatureInfo[creature[cc].id].mageryLevel*0.05;
	}

	if (usingRing) bonus=1.65; //Equivalent to a level 10 spellcaster
	
	if (creature[cc].godMode)
		bonus=5;
	
	

	//-- get magic resistance -------------------------------------------------------------

	if (creature[ct].npcIndex!=0)
		resist=0.39;  //JS bourrin  //Dave resist=0.5; npc RM
/*	if (creature[npc[ct].creatureIndex].type=3)
		resist=0.6;
	if (creature[npc[ct].creatureIndex].type=4)
		resist=0.5;
	if (creature[npc[ct].creatureIndex].type=6)
		resist=0.35;
	
	if (creature[ct].npcIndex==3)
		resist=0.5;  //JS bourrin
	if (creature[ct].npcIndex==4)
		resist=0.4;
	if (creature[ct].npcIndex==5)
		resist=0.4;
	if (creature[ct].npcIndex==6)
		resist=0.2;
	*/
	else if (creature[ct].playerIndex!=0)
		resist=sk_GetMagicResistance(creature[ct].playerIndex);
	else
	{
		//resist=(float)((151-((float)creatureInfo[creature[ct].id].magicResist+((float)creatureInfo[creature[ct].id].maxIntelligence/2)))/100);
		//for (j=1;j<creature[ct].level;j++)
		//  resist=resist*1.06;

		resist=creatureInfo[creature[ct].id].magicResist;

		for (j=1;j<(creatureInfo[creature[ct].id].creatureLevel - creature[ct].level)+1;j++)
			resist=resist*0.9;
	}

	//--------------------------------------------------------------------------------------

	//if (creature[cc].playerIndex!=0)
		if (creature[cc].godMode)
			creature[ct].magicReflection=false;

	//--------------------------------------------------------------------------------------

	if (fieldSpell==false && !usingRing &&creature[cc].playerIndex!=0)
	{
		if (creature[cc].magicPoints>=itemList[spell].price)
		{
			creature[cc].magicPoints=creature[cc].magicPoints-itemList[spell].price;

			if (creature[cc].playerIndex!=0)
				if (player[creature[cc].playerIndex].god)
					creature[cc].magicPoints=100;
		}
		else
		{
			return (false);
		}
	}

	//-- lightning bolt spell --------------------------------------------------------------

	if (spell==kLightningBoltSpell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		cr_AddSound(cc, kBoltSnd);

		//creature[cc].soundFX					      = kBoltSnd;
		//creature[cc].soundFXTime			      = currentTime;

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;
		
		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=creatureInfo[creature[ct].id].magicResist;

				for (j=1;j<creature[ct].level;j++)
					resist=resist*0.9;
			}
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		fx_CreatureServerFX(cc, ct, kLightningBolt, (3*60), 0);
		fx_CreatureServerFX(ct, cc, kLightningBoltReverse, (3*60), 0);

		if (creature[cc].playerIndex!=0)
		{
			damage=tb_Rnd(9,20)+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)*2);
		}
		else
			damage=tb_Rnd(9,20)+creatureInfo[creature[cc].id].mageryLevel*2;//tb_Round(creatureInfo[creature[cc].id].maxMagicPoints/10);

		if (usingRing)
			damage=tb_Rnd(9,20)+20;

		damage=damage*bonus;

		damage=damage*resist;
		
		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);

		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;

			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			if (player[creature[cc].playerIndex].morph!=0) //morphed druids  
					{
					tx_CreatureTalkText(cc,ct,kMorphedTalk);
					}
			else
				
				tx_CreatureTalkText(cc,ct,kAttackingTalk);
				
				
			//cr_Dead(ct);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
				sk_ResistingSpells(cc,creature[ct].playerIndex);
			}
		}
	}


	//-- Fireball spell --------------------------------------------------------------

	if (spell==kFireballSpell)
	{
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;
		}

		cr_AddSound(cc, kFireballSnd);

		gr_CreateDistanceSpell(cc, ct, kFireballSpell);
	}

	//-- Fireblast spell --------------------------------------------------------------

	if (spell==kFireblastSpell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		//cr_AddSound(cc, kBoltSnd);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);
			else
				resist=0; //reflection sent to the ancient dragon, 0 damage
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		fx_CreatureServerFX(ct, ct, kFireblastX, (2*60), 0);

		if (creature[cc].playerIndex!=0)
		{
			damage=tb_Rnd(60,75)+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)*2);
		}
		else
			damage=tb_Rnd(50,65)+creatureInfo[creature[cc].id].mageryLevel*2;//tb_Round(creatureInfo[creature[cc].id].maxMagicPoints/10);

		if (usingRing)
			damage=tb_Rnd(60,75)+20;

		damage=damage*bonus;

		damage=damage*resist;
		
		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);

		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;
			
			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			else
				tx_CreatureTalkText(cc,ct,kAttackingTalk);

			//cr_Dead(ct);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
				sk_ResistingSpells(cc,creature[ct].playerIndex);
			}
		}
	}

	//-- Magic Missile spell --------------------------------------------------------------

	if (spell==kMagicMissileSpell)
	{
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;
		}

		//cr_AddSound(cc, kFireballSnd);
		gr_CreateDistanceSpell(cc, ct, kMagicMissileSpell);
	}

	//-- Fireblast3 spell --------------------------------------------------------------

	if (spell==kFireblast3Spell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		//cr_AddSound(cc, kBoltSnd);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=creatureInfo[creature[ct].id].magicResist;

				for (j=1;j<creature[ct].level;j++)
					resist=resist*0.9;
			}
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		fx_CreatureServerFX(cc, ct, kFireblast3X, (3*60), 0);

		if (creature[cc].playerIndex!=0)
		{
			if (player[creature[cc].playerIndex].god)
			{
				damage=1000;
				resist=1;
			}
			else
				damage=tb_Rnd(9,20)+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)*2);
		}
		else
			damage=tb_Rnd(8,15)+creatureInfo[creature[cc].id].mageryLevel*2;//tb_Round(creatureInfo[creature[cc].id].maxMagicPoints/10);

		if (usingRing)
			damage=tb_Rnd(9,20)+20;

		damage=damage*bonus;

		damage=damage*resist;

		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);

		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;

			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			else
				tx_CreatureTalkText(cc,ct,kAttackingTalk);

			//cr_Dead(ct);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
				sk_ResistingSpells(cc,creature[ct].playerIndex);
			}
		}
	}

	//-- Fireblast4 spell --------------------------------------------------------------

	if (spell==kFireblast4Spell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		//cr_AddSound(cc, kBoltSnd);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=creatureInfo[creature[ct].id].magicResist;

				for (j=1;j<creature[ct].level;j++)
					resist=resist*0.9;
			}
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(cc, ct, kSparkle, (3*60), 0);

		fx_CreatureServerFX(cc, ct, kFireblast4X, (3*60), 0);

		if (creature[cc].playerIndex!=0)
		{
			if (player[creature[cc].playerIndex].god)
			{
				damage=1000;
				resist=1;
			}
			else
				damage=tb_Rnd(9,20)+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)*2);
		}
		else
			damage=tb_Rnd(8,15)+creatureInfo[creature[cc].id].mageryLevel*2;//tb_Round(creatureInfo[creature[cc].id].maxMagicPoints/10);

		if (usingRing)
			damage=tb_Rnd(9,20)+20;

		damage=damage*bonus;

		damage=damage*resist;

		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);

		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;

			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			else
				tx_CreatureTalkText(cc,ct,kAttackingTalk);

			//cr_Dead(ct);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
				sk_ResistingSpells(cc,creature[ct].playerIndex);
			}
		}
	}

	//-- Fireblast5 spell --------------------------------------------------------------

	if (spell==kFireblast5Spell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		//cr_AddSound(cc, kBoltSnd);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=creatureInfo[creature[ct].id].magicResist;

				for (j=1;j<creature[ct].level;j++)
					resist=resist*0.9;
			}
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		fx_CreatureServerFX(cc, ct, kFireblast5X, (3*60), 0);

		if (creature[cc].playerIndex!=0)
		{
			if (player[creature[cc].playerIndex].god)
			{
				damage=1000;
				resist=1;
			}
			else
				damage=tb_Rnd(9,20)+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)*2);
		}
		else
			damage=tb_Rnd(9,20)+creatureInfo[creature[cc].id].mageryLevel*2;//tb_Round(creatureInfo[creature[cc].id].maxMagicPoints/10);

		if (usingRing)
			damage=tb_Rnd(9,20)+20;

		damage=damage*bonus;

		damage=damage*resist;

		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);

		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;

			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			else
				tx_CreatureTalkText(cc,ct,kAttackingTalk);

			//cr_Dead(ct);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
				sk_ResistingSpells(cc,creature[ct].playerIndex);
			}
		}
	}

	//-- resurrect spell --------------------------------------------------------------

	if (spell==kResurrectSpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kResX, (5*60), 0);

		player[creature[ct].playerIndex].dead=false;
		player[creature[ct].playerIndex].sendStatsRefresh=true;
		creature[player[ct].creatureIndex].hitPoints=25;

		if (creature[ct].strength<creature[ct].hitPoints)
			creature[ct].hitPoints=creature[ct].strength;

		creature[ct].magicPoints=25;

		if (creature[ct].intelligence<creature[ct].magicPoints)
			creature[ct].magicPoints=creature[ct].intelligence;

	}

	//-- lesser heal spell --------------------------------------------------------------

	if (spell==kLesserHealSpell)
	{
		if (fieldSpell==false)
		{
			fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
			fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
		}

		level=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryCleric].level);

		if (level>4)
			level=4;

		if (fieldSpell==false)
		{
			if (creature[cc].playerIndex!=0)
				heal=tb_Rnd(5,20)+level;
			else
				heal=tb_Rnd(5,20)+creatureInfo[creature[cc].id].mageryLevel;//tb_Round(creatureInfo[creature[cc].id].maxMagicPoints/10);

			if (usingRing)
				heal=tb_Rnd(5,20)+10;
		}
		else //casting mass heal
		{
			if (creature[cc].playerIndex!=0)
				heal=tb_Rnd(16,31)+level;
			else
				heal=tb_Rnd(16,31)+creatureInfo[creature[cc].id].mageryLevel;//tb_Round(creatureInfo[creature[cc].id].maxMagicPoints/10);

			if (usingRing)
				heal=tb_Rnd(16,31)+10;
		}

		heal=heal*bonus;

		if (creature[ct].playerIndex!=0)
		{
			if (creature[ct].hitPoints==creature[ct].strength)	// merge 7/28
				return (false);

			if (heal+creature[ct].hitPoints>creature[ct].strength)
				heal=creature[ct].strength-creature[ct].hitPoints;
		}
		else
		{
			if (creature[ct].hitPoints==creatureInfo[creature[ct].id].maxHitPoints)	// merge 7/28
				return (false);

			if (heal+creature[ct].hitPoints>creatureInfo[creature[ct].id].maxHitPoints)
				heal=creatureInfo[creature[ct].id].maxHitPoints-creature[ct].hitPoints;
		}

		if (heal>0)
		{
			creature[ct].hitPoints=creature[ct].hitPoints+heal;

			if (creature[ct].playerIndex!=0)
				player[creature[ct].playerIndex].sendStatsRefresh=true;

			if (creature[ct].master!=0)			// 02/07/05 Pets have to stop running once healed
				creature[ct].wounded=false;
		}
	}

	//-- greater heal spell --------------------------------------------------------------

	if (spell==kGreaterHealSpell)
	{
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			heal=tb_Rnd(25,50)+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryCleric].level);
		else
			heal=tb_Rnd(25,50)+creatureInfo[creature[cc].id].mageryLevel;

		if (usingRing)
			heal=tb_Rnd(25,50)+10;

		heal=heal*bonus;

		if (creature[ct].playerIndex!=0)
		{
			if (creature[ct].hitPoints==creature[ct].strength)	// merge 7/28
				return (false);

			if (heal+creature[ct].hitPoints>creature[ct].strength)
				heal=creature[ct].strength-creature[ct].hitPoints;
		}
		else
		{
			if (creature[ct].hitPoints==creatureInfo[creature[ct].id].maxHitPoints)	// merge 7/28
				return (false);

			if (heal+creature[ct].hitPoints>creatureInfo[creature[ct].id].maxHitPoints)
				heal=creatureInfo[creature[ct].id].maxHitPoints-creature[ct].hitPoints;
		}

		if (heal>0)
		{
			creature[ct].hitPoints=creature[ct].hitPoints+heal;

			if (creature[ct].playerIndex!=0)
				player[creature[ct].playerIndex].sendStatsRefresh=true;
		}
	}

	//-- remove curse spell --------------------------------------------------------------

	if (spell==kRemoveCurseSpell)
	{
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
		

		if (creature[ct].playerIndex>0)
		{
			if (player[creature[ct].playerIndex].curse&kStenchCurse || player[creature[ct].playerIndex].curse&kBabbleCurse)
			{
				player[creature[ct].playerIndex].curse=0;
				nw_SendDisplayMessage(creature[ct].playerIndex,kCurseRemoved);
				
			}
			else
				return(false);
		}
		
	}
/*
//Dave
//-- Clense spell --------------------------------------------------------------

	if (spell==kClenseSpell)
	{
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[ct].playerIndex>0)
		{
			if (player[creature[ct].playerIndex].paralyzed&kYouAreParalyzed || player[creature[ct].playerIndex].disorientation&kYouAreDisoriented)
			{
				player[creature[ct].playerIndex].disorientation=0;
				player[creature[ct].playerIndex].paralyzed=0;
				nw_SendDisplayMessage(creature[ct].playerIndex,kYouHaveBeenClensed);
			}
			else
				return(false);
		}
	}
	*/
	//-- full heal spell --------------------------------------------------------------

	if (spell==kFullHealSpell)
	{
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[ct].playerIndex!=0)
		{
			if (creature[ct].hitPoints==creature[ct].strength)	// merge 7/28
				return (false);

			creature[ct].hitPoints=creature[ct].strength;

			player[creature[ct].playerIndex].sendStatsRefresh=true;
		}
		else
		{
			if (creature[ct].hitPoints==creatureInfo[creature[ct].id].maxHitPoints)	// merge 7/28
				return (false);

			creature[ct].hitPoints=creatureInfo[creature[ct].id].maxHitPoints;
		}
	}

	//-- poison spell --------------------------------------------------------------

	if (spell==kPoisonSpell)
	{
		int	wmLevel=(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level));
		int	range=0;
		
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=creatureInfo[creature[ct].id].magicResist;

				for (j=1;j<creature[ct].level;j++)
					resist=resist*0.9;
			}
		}

		if (fieldSpell==false)
		{
			fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
			fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
		}
		//Dave add 11-06-2010
		//if (pl_AuthorizedTo(cc, kRegularPlayer))
			
				if (wmLevel<=4)
					(range=(tb_Rnd(wmLevel-1,wmLevel+1)));
					
				if (wmLevel>=10)
					(range=(tb_Rnd(wmLevel-1,wmLevel+3)));
				
										
				
				else range=(tb_Rnd(wmLevel-1,wmLevel+2));

		
		
		if (creature[cc].playerIndex!=0)
			//poison=1+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)/2);
			poison=1+range/2;
		else
			poison=1+creatureInfo[creature[cc].id].mageryLevel/2;

		poison=poison*bonus;

		poison=poison*resist;

		if (creatureInfo[creature[ct].id].immuneToPoison==false)
			if (creature[ct].poison<100)
				creature[ct].poison=creature[ct].poison+poison;

		if (creature[ct].playerIndex!=0)
		{
			sk_ResistingSpells(cc,creature[ct].playerIndex);
			nw_SendDisplayMessage(creature[ct].playerIndex,kYouArePoisoned);
		}

	}

	//-- reflective armor spell --------------------------------------------------------------

	if (spell==kReflectiveArmorSpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		//fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
		fx_CreatureServerFX(ct, ct, kDome2X, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			reflect=8+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryCleric].level)*2);
		else
			reflect=(tb_Rnd(0,75)/2);//Dave tb_Rnd(0,100)/2

		reflect=reflect*bonus;

		if (creature[ct].reflectDamage<reflect)
			creature[ct].reflectDamage=reflect;
		
		return false;

	}

	//-- clumsy spell --------------------------------------------------------------

	if (spell==kClumsySpell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			effect=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)*10;
		else
			effect=creatureInfo[creature[cc].id].mageryLevel*10;

		creature[ct].dexterityOffset	=	-1*tb_Round((float)creature[ct].dexterity*0.3);

		bonus                         = (60*effect);

		bonus                         = bonus * resist;

		creature[ct].dexterityTime		=	TickCount()+bonus;

		if (creature[ct].playerIndex!=0)
			player[creature[ct].playerIndex].sendStatsRefresh=true;

	}

	//-- weaken spell --------------------------------------------------------------

	if (spell==kWeakenSpell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			effect=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)*10;
		else
			effect=creatureInfo[creature[cc].id].mageryLevel*10;

		creature[ct].strengthOffset	=	-1*tb_Round((float)creature[ct].strength*0.3);

		bonus                         = (60*effect);

		bonus                         = bonus * resist;

		creature[ct].strengthTime		  =	TickCount()+bonus;

		if (creature[ct].playerIndex!=0)
			player[creature[ct].playerIndex].sendStatsRefresh=true;

	}

	//-- simple mind spell --------------------------------------------------------------

	if (spell==kSimpleMindSpell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			effect=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)*10;
		else
			effect=creatureInfo[creature[cc].id].mageryLevel*10;

		creature[ct].intelligenceOffset	=	-1*tb_Round((float)creature[ct].intelligence*0.3);

		bonus                           = (60*effect);

		bonus                           = bonus * resist;

		creature[ct].intelligenceTime		=	TickCount()+bonus;

		if (creature[ct].playerIndex!=0)
			player[creature[ct].playerIndex].sendStatsRefresh=true;

	}

	//-- agility spell --------------------------------------------------------------

	if (spell==kAgilitySpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		//fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
		fx_CreatureServerFX(ct, ct, kBuffAgiX, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			effect=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryCleric].level)*10;
		else
			effect=creatureInfo[creature[cc].id].mageryLevel*10;

		creature[ct].dexterityOffset	=	tb_Round((float)creature[ct].dexterity*0.3);

		creature[ct].dexterityTime		=	TickCount()+(60*effect);

		if (creature[ct].playerIndex!=0)
			player[creature[ct].playerIndex].sendStatsRefresh=true;
		
		return false;

	}

	//-- strength spell --------------------------------------------------------------

	if (spell==kStrengthSpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		//fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
		fx_CreatureServerFX(ct, ct, kBuffStrX, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			effect=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryCleric].level)*10;
		else
			effect=creatureInfo[creature[cc].id].mageryLevel*10;

		creature[ct].strengthOffset	=	tb_Round((float)creature[ct].strength*0.3);

		creature[ct].strengthTime		=	TickCount()+(60*effect);

		if (creature[ct].playerIndex!=0)
			player[creature[ct].playerIndex].sendStatsRefresh=true;
		
		return false;

	}

	//-- elevate mind spell --------------------------------------------------------------

	if (spell==kElevateMindSpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		//fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
		fx_CreatureServerFX(ct, ct, kBuffMindX, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			effect=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryCleric].level)*10;
		else
			effect=creatureInfo[creature[cc].id].mageryLevel*10;

		creature[ct].intelligenceOffset	=	tb_Round((float)creature[ct].intelligence*0.3);

		creature[ct].intelligenceTime		=	TickCount()+(60*effect);

		if (creature[ct].playerIndex!=0)
			player[creature[ct].playerIndex].sendStatsRefresh=true;
		
		return false;

	}

	//-- lesser cure spell --------------------------------------------------------------

	if (spell==kLesserDetoxifySpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			heal=tb_Rnd(1,2);
		else
			heal=tb_Rnd(1,5);

		heal=heal*bonus;

		if (heal>creature[ct].poison)
			heal=creature[ct].poison;

		if (creature[ct].poison==0)	// merge 7/28
			return (false);

		creature[ct].poison=creature[ct].poison-heal;

	}

	//-- greater cure spell --------------------------------------------------------------

	if (spell==kGreaterDetoxifySpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			heal=2+tb_Round(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryCleric].level));
		else
			heal=2+creatureInfo[creature[cc].id].mageryLevel;

		heal=heal*bonus;

		if (heal>creature[ct].poison)
			heal=creature[ct].poison;

		if (creature[ct].poison==0)	// merge 7/28
			return (false);

		creature[ct].poison=creature[ct].poison-heal;

	}

	//-- full cure spell --------------------------------------------------------------

	if (spell==kFullDetoxifySpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[ct].poison==0)	// merge 7/28
			return (false);

		creature[ct].poison=0;

	}

	//-- explosion spell --------------------------------------------------------------

	if (spell==kExplosionSpell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=creatureInfo[creature[ct].id].magicResist;

				for (j=1;j<creature[ct].level;j++)
					resist=resist*0.9;
			}
		}

		cr_AddSound(ct, kExplosionSnd);

		//creature[ct].soundFX					      = kExplosionSnd;
		//creature[ct].soundFXTime			      = currentTime;

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kLightExplosion, (2*60), 0);
		fx_CreatureServerFX(ct, ct, kExplosionX, (3*60), 10);

		if (creature[cc].playerIndex!=0)
			damage=tb_Rnd(5,9)+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level);
		else
			damage=tb_Rnd(5,9)+creatureInfo[creature[cc].id].mageryLevel;

		damage=damage*bonus;

		damage=damage*resist;
		
		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);

		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;

			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			else
				tx_CreatureTalkText(cc,ct,kAttackingTalk);

			//cr_Dead(ct);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
				sk_ResistingSpells(cc,creature[ct].playerIndex);
			}
		}

	}

	//-- magic reflection spell --------------------------------------------------------------

	if (spell==kMagicReflectionSpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		//fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
		fx_CreatureServerFX(ct, ct, kDomeX, (5*60), 0);
		//fx_AddServerFX(kSteamFX, (2*60), row, col,0, 0);
		fx_CreatureServerFX(ct, ct, kSteamFX, (1*60), 10);
		if (creature[ct].magicReflection)	// merge 7/28
			return (false);

		creature[ct].magicReflection=true;
		
		return false;

	}

	//-- paralyze spell --------------------------------------------------------------

	if (spell==kParalyzeSpell)
	{
		//Dave add 11-06-2010
		int	wmLevel=(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level));
		int	range=0;
		
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=creatureInfo[creature[ct].id].magicResist;

				for (j=1;j<creature[ct].level;j++)
					resist=resist*0.9;
			}
		}
		
		
		
		//if (fieldSpell==false)
		//{
			//fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
			fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
			fx_CreatureServerFX(ct, ct, kCreatureSummonX, (1*60), 0);
			
			
		//}

		//Dave
		//if (pl_AuthorizedTo(cc, kRegularPlayer))
			//{
				if (wmLevel<=4)
					(range=(tb_Rnd(wmLevel-1,wmLevel+1)));
				if (wmLevel>=10)
					(range=(tb_Rnd(wmLevel-1,wmLevel+3)));
		//	}							
				
				else range=(tb_Rnd(wmLevel-1,wmLevel+2));

		
		if (creature[cc].playerIndex!=0)
			//damage=tb_Rnd(6,16+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level));
			damage=tb_Rnd(6,16+range);
		else
			//damage=tb_Rnd(6,16)+creatureInfo[creature[cc].id].mageryLevel;
			damage=tb_Rnd(6,16)+range;
		
		
			
		//Dave^
		
		damage=damage*60;

		damage=damage*bonus;

		damage=damage*resist;

		if (damage>0)
		{
			creature[ct].paralyzed=currentTime+damage;

			if (creature[ct].playerIndex!=0)
			{
				sk_ResistingSpells(cc,creature[ct].playerIndex);
				nw_SendDisplayMessage(creature[ct].playerIndex,kYouAreParalyzed);
				
			}
		}

	}

	//-- disorientation spell --------------------------------------------------------------

	if (spell==kDisorientationSpell)
	{
		int	wmLevel=(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level));
		int	range=1;
		
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=creatureInfo[creature[ct].id].magicResist;

				for (j=1;j<creature[ct].level;j++)
					resist=resist*0.9;
			}
		}



		if (fieldSpell==false)
		{
			fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
			fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
			fx_CreatureServerFX(ct, ct, kSparkle4, (7*60), 0);
			
			
			if (pl_AuthorizedTo(cc, kRegularPlayer))
			{
				if (wmLevel==8)
					(range=(tb_Rnd(5,8)));
				
				if (wmLevel==9)
					(range=(tb_Rnd(6,9)));
				
				if (wmLevel==10)
					(range=(tb_Rnd(7,10)));
				
				if (wmLevel==11)
					(range=(tb_Rnd(8,11)));
				
				if (wmLevel==12)
					(range=(tb_Rnd(9,12)));
				
				if (wmLevel>=13)
					(range=(tb_Rnd(10,13)));
			}							
				
				else range=(tb_Rnd(20,30));

			
			if (creature[cc].playerIndex!=0)
				//damage=tb_Rnd(10,20)+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level);
				damage=(tb_Rnd(10,20)+range*2);
			else
				damage=tb_Rnd(10,20)+creatureInfo[creature[cc].id].mageryLevel*2;
		}
		else //casting disorient fiel - reduce effect
		{
			if (creature[cc].playerIndex!=0)
				damage=tb_Rnd(3,5)+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)/2;
			else
				damage=tb_Rnd(3,5)+creatureInfo[creature[cc].id].mageryLevel/2;
		}

		damage=damage*60;

		damage=damage*bonus;

		damage=damage*resist;

		if (damage>0)
		{
			creature[ct].disorientation=currentTime+damage;

			if (creature[ct].playerIndex!=0)
			{
				sk_ResistingSpells(cc,creature[ct].playerIndex);
				nw_SendDisplayMessage(creature[ct].playerIndex,kYouAreDisoriented);
			}
		}

	}

	//-- drain mana spell --------------------------------------------------------------

	if (spell==kDrainManaSpell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=creatureInfo[creature[ct].id].magicResist;

				for (j=1;j<creature[ct].level;j++)
					resist=resist*0.9;
			}
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			damage=tb_Rnd(5,20)+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level);
		else
			damage=tb_Rnd(5,20)+creatureInfo[creature[cc].id].mageryLevel;

		damage=damage*bonus;

		damage=damage*resist;

		if (damage>creature[ct].magicPoints)
			damage=creature[ct].magicPoints;

		if (damage>0)
		{
			if (creature[ct].playerIndex==0)
				if (creature[ct].skipSpell<100)
					creature[ct].skipSpell=creature[ct].skipSpell+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)/2);

			creature[ct].magicPoints=creature[ct].magicPoints-damage;

			if (creature[ct].playerIndex!=0)
				player[creature[ct].playerIndex].sendStatsRefresh=true;
		}

	}

	//-- invisibility spell --------------------------------------------------------------

	if (spell==kInvisibilitySpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		if (creature[ct].hiding)	// merge 7/28
			return (false);

		creature[ct].hiding=true;
		
		return false;

	}

	//-- opportunity spell --------------------------------------------------------------

	if (spell==kOpportunitySpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
		creature[ct].opportunityTime=TickCount()+8*60;
		
		return false;

	}

	//-- night Vision spell --------------------------------------------------------------

	if (spell==kNightVisionSpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		if (creature[ct].playerIndex!=0)
			nw_SendNightVision(creature[ct].playerIndex);
		
		return false;

	}
	
	//MONSTERS ONLY SPELLS
	
	//-- summon black draco --------------------------------------------------------------
	
	if (spell==kSummonBlackDracoSpell)
	{
		int creat;
		int cpt;
		int tracks;
		
		fx_CreatureServerFX(cc, cc, kFBFieldX, (1.5*60), 10);
		
		damage=creatureInfo[creature[cc].id].mageryLevel/3;
				
		for (cpt=0; cpt<damage; cpt++)
		{
			creat=cr_SpawnCreature(kBlackDraco, creature[cc].row, creature[cc].col);
			fx_CreatureServerFX(creat, creat, kCreatureSummonX, (1.5*60), 0);
			if (containerMap[creature[creat].row][creature[creat].col] == 0 && tracksMap[creature[creat].row][creature[creat].col] == 0)
			{
				tracks=in_MakeTracks(creature[creat].row, creature[creat].col, tb_Rnd(21, 26));
				
				if (tracks!=-1)
					tracksDecayTime[tracks]	= TickCount()+(180);
			}
		}		
	}
	
	//-- summon red draco --------------------------------------------------------------
	
	if (spell==kSummonRedDracoSpell)
	{
		int creat;
		int cpt;
		int tracks;
		
		fx_CreatureServerFX(cc, cc, kFBFieldX, (1.5*60), 10);
		
		damage=creatureInfo[creature[cc].id].mageryLevel/3;
				
		for (cpt=0; cpt<damage; cpt++)
		{
			creat=cr_SpawnCreature(kRedDraco, creature[cc].row, creature[cc].col);
			fx_CreatureServerFX(creat, creat, kCreatureSummonX, (1.5*60), 0);
			if (containerMap[creature[creat].row][creature[creat].col] == 0 && tracksMap[creature[creat].row][creature[creat].col] == 0)
			{
				tracks=in_MakeTracks(creature[creat].row, creature[creat].col, tb_Rnd(21, 26));
				
				if (tracks!=-1)
					tracksDecayTime[tracks]	= TickCount()+(180);
			}
		}		
	}
	
	//-- summon ice draco --------------------------------------------------------------
	
	if (spell==kSummonIceDracoSpell)
	{
		int creat;
		int cpt;
		int tracks;
		
		fx_CreatureServerFX(cc, cc, kFBFieldX, (1.5*60), 10);
		
		damage=creatureInfo[creature[cc].id].mageryLevel/3;
				
		for (cpt=0; cpt<damage; cpt++)
		{
			creat=cr_SpawnCreature(kIceDraco, creature[cc].row, creature[cc].col);
			fx_CreatureServerFX(creat, creat, kCreatureSummonX, (1.5*60), 0);
			if (containerMap[creature[creat].row][creature[creat].col] == 0 && tracksMap[creature[creat].row][creature[creat].col] == 0)
			{
				tracks=in_MakeTracks(creature[creat].row, creature[creat].col, tb_Rnd(21, 26));
				
				if (tracks!=-1)
					tracksDecayTime[tracks]	= TickCount()+(180);
			}
		}		
	}
	
	//-- summon green draco --------------------------------------------------------------
	
	if (spell==kSummonGreenDracoSpell)
	{
		int creat;
		int cpt;
		int tracks;
		
		fx_CreatureServerFX(cc, cc, kFBFieldX, (1.5*60), 10);
		
		damage=creatureInfo[creature[cc].id].mageryLevel/3;
				
		for (cpt=0; cpt<damage; cpt++)
		{
			creat=cr_SpawnCreature(kForestDraco, creature[cc].row, creature[cc].col);
			fx_CreatureServerFX(creat, creat, kCreatureSummonX, (1.5*60), 0);
			if (containerMap[creature[creat].row][creature[creat].col] == 0 && tracksMap[creature[creat].row][creature[creat].col] == 0)
			{
				tracks=in_MakeTracks(creature[creat].row, creature[creat].col, tb_Rnd(21, 26));
				
				if (tracks!=-1)
					tracksDecayTime[tracks]	= TickCount()+(180);
			}
		}		
	}
	
	//-- summon skels --------------------------------------------------------------
	
	if (spell==kSummonSkelsSpell)
	{
		int creat;
		int cpt;
		int tracks;
		
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		
		damage=creatureInfo[creature[cc].id].mageryLevel/3;
		
		for (cpt=0; cpt<damage; cpt++)
		{
			creat=cr_SpawnCreature(kSkeleton, creature[cc].row, creature[cc].col);
			fx_CreatureServerFX(creat, creat, kCreatureSummonX, (1.5*60), 0);
			if (containerMap[creature[creat].row][creature[creat].col] == 0 && tracksMap[creature[creat].row][creature[creat].col] == 0)
			{
				tracks=in_MakeTracks(creature[creat].row, creature[creat].col, tb_Rnd(21, 26));
				
				if (tracks!=-1)
					tracksDecayTime[tracks]	= TickCount()+(180);
			}
		}
	}
	
	//-- field spell --------------------------------------------------------------
	
	if (spell>=kFirstFieldSpell && spell<=kLastFieldSpell) //creature field
	{
		if (fieldSpell)
		{
			if (creature[ct].playerIndex!=0 && pl_AuthorizedTo(ct, kIsAnyGM))			return false;
			if (creature[ct].playerIndex!=0 && player[creature[ct].playerIndex].dead)	return false;
			
			if (spell==kChaosFieldSpell)		return (sk_CastSpellOnCreature(cc, ct, kChaosSpell, true));
			if (spell==kFireblastFieldSpell)	return (sk_CastSpellOnCreature(cc, ct, kFireblastSpell, true));
			if (spell==kIceFieldSpell)			return (sk_CastSpellOnCreature(cc, ct, kIceSplashSpell, true));
			if (spell==kNatureFieldSpell)		return (sk_CastSpellOnCreature(cc, ct, kNatureSplashSpell, true));
			//etc
		}
		else
		{
			if (creature[ct].playerIndex==0)	return false;
		
			player[creature[ct].playerIndex].creatureField=spell;
			player[creature[ct].playerIndex].fieldFrom=cc;
		
			sk_CastSpellOnCreature(cc, ct, spell, true);
		}
		
	}
	
	//-- chaos spell --------------------------------------------------------------
	
	if (spell==kChaosSpell)
	{
		int row, col;
		int tries=0;
		
		if (creatureInfo[creature[ct].id].mobile==false)
			return (0);
		
		do
		{
			row=creature[cc].row+tb_Rnd(0,8)-4;
			col=creature[cc].col+tb_Rnd(0,8)-4;
			
			tries++;
		}
		while ((!cr_InDomain(ct,row,col) || cr_ViewBlocked(creature[ct].row, creature[ct].col, row, col) || guildMap[row][col]!=0) && tries<255);
		
		if (tries==255)	return false;
		
		if (creature[ct].playerIndex==0)
		{
			cr_CreatureMoveTo(ct, row, col);
		}
		else
		{
			pl_PlayerMoveTo(creature[ct].playerIndex,row,col);
		}
		
		if (creature[ct].hiding==false)
			fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);
		
		//break attack
		creature[player[creature[ct].playerIndex].fieldFrom].attackTarget=0;
		creature[player[creature[ct].playerIndex].fieldFrom].possibleTarget=0;
	}
	
	//-- nature splash --------------------------------------------------------------

	if (spell==kNatureSplashSpell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=0;
			}
		}

		fx_CreatureServerFX(cc, cc, kFBFieldX, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (3*60), 0);

		if (creature[cc].playerIndex!=0)
			poison=1+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)/2);
		else
			poison=5+creatureInfo[creature[cc].id].mageryLevel;

		poison=poison*bonus;

		poison=poison*resist;

		if (creatureInfo[creature[ct].id].immuneToPoison==false)
			if (creature[ct].poison<100)
				creature[ct].poison=creature[ct].poison+poison;
		
			
		if (creature[cc].playerIndex!=0)
			damage=tb_Rnd(6,16+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level));
		else
			damage=tb_Rnd(25,40)+creatureInfo[creature[cc].id].mageryLevel;

		damage=damage*bonus;

		damage=damage*resist;
		
		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);

		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;

			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			else
				tx_CreatureTalkText(cc,ct,kAttackingTalk);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
			}
		}

		if (creature[ct].playerIndex!=0)
		{
			sk_ResistingSpells(cc,creature[ct].playerIndex);
			nw_SendDisplayMessage(creature[ct].playerIndex,kYouArePoisoned);
		}
	}
	
	//-- creature heal spell --------------------------------------------------------------

	if (spell==kCreatureHealSpell)
	{
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (5*60), 0);

		heal=creatureInfo[creature[ct].id].maxHitPoints/10;
		
		heal*=creatureInfo[creature[ct].id].mageryLevel/5;

		if (creature[ct].playerIndex!=0)
		{
			if (creature[ct].hitPoints==creature[ct].strength)	// merge 7/28
				return (false);

			if (heal+creature[ct].hitPoints>creature[ct].strength)
				heal=creature[ct].strength-creature[ct].hitPoints;
		}
		else
		{
			if (creature[ct].hitPoints==creatureInfo[creature[ct].id].maxHitPoints)	// merge 7/28
				return (false);

			if (heal+creature[ct].hitPoints>creatureInfo[creature[ct].id].maxHitPoints)
				heal=creatureInfo[creature[ct].id].maxHitPoints-creature[ct].hitPoints;
		}

		if (heal>0)
		{
			creature[ct].hitPoints=creature[ct].hitPoints+heal;

			if (creature[ct].playerIndex!=0)
				player[creature[ct].playerIndex].sendStatsRefresh=true;
		}
	}
	
	//-- ice splash --------------------------------------------------------------

	if (spell==kIceSplashSpell)
	{
		if (creature[ct].playerIndex!=0)
			sk_DamageMagicRobe(creature[ct].playerIndex);

		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}

		if (creature[ct].magicReflection)
		{
			creature[ct].magicReflection=false;
			ct=cc;

			if (creature[ct].playerIndex!=0)
				resist=sk_GetMagicResistance(creature[ct].playerIndex);

			//resist=(float)((151-((float)player[creature[ct].playerIndex].skill[kResistingMagic].level+tb_Round((float)(creature[ct].intelligence+creature[ct].intelligenceOffset)/2)))/100);
			else
			{
				resist=0;
			}
		}

		fx_CreatureServerFX(cc, cc, kFBFieldX, (3*60), 10);
		fx_CreatureServerFX(ct, ct, kSparkle, (3*60), 0);

		if (creature[cc].playerIndex!=0)
			damage=tb_Rnd(6,16+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level));
		else
			damage=tb_Rnd(10,20)+creatureInfo[creature[cc].id].mageryLevel;

		damage=damage*60;

		damage=damage*bonus;

		damage=damage*resist;

		if (damage>0)
		{
			creature[ct].paralyzed=currentTime+damage;

			if (creature[ct].playerIndex!=0)
			{
				sk_ResistingSpells(cc,creature[ct].playerIndex);
				nw_SendDisplayMessage(creature[ct].playerIndex,kYouAreParalyzed);
			}
		}
		
			
		if (creature[cc].playerIndex!=0)
			damage=tb_Rnd(6,16+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level));
		else
			damage=tb_Rnd(25,40)+creatureInfo[creature[cc].id].mageryLevel;

		damage=damage*bonus;

		damage=damage*resist;
		
		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);

		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;

			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			else
				tx_CreatureTalkText(cc,ct,kAttackingTalk);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
			}
		}
	}
		
	//-- root attack --------------------------------------------------------------

	if (spell==kRootAttackSpell)
	{
		int tracks; 
		
		retailiate=false;

		if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
			retailiate=true;
		else
			if (creature[ct].playerIndex==0)
				if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
					retailiate=true;
				
		if (creature[ct].playerIndex!=0)	retailiate=false;

		if (retailiate)
		{
			if (pl_IsThief(ct))
			{
				if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
				{
					creature[ct].attackTarget=cc;
					creature[ct].nextAttackTime	= currentTime;
				}
			}
			else
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}
		
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		if (tracksMap[creature[ct].row][creature[ct].col]==0 && containerMap[creature[ct].row][creature[ct].col]==0)
		{
			tracks=in_MakeTracks(creature[ct].row, creature[ct].col, 51);
			
			if (tracks!=-1)
					tracksDecayTime[tracks]	= TickCount()+(90);
		}
		

			
		if (creature[cc].playerIndex!=0)
			damage=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMelee].level);
		else
			damage=tb_Rnd(creatureInfo[creature[cc].id].attackDamageMin,creatureInfo[creature[cc].id].attackDamageMax);
		
		
		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);
		
		
		
		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;

			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			else
				tx_CreatureTalkText(cc,ct,kAttackingTalk);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
			}
		
		
		
		}
	}
	
	return (true);

}

// ----------------------------------------------------------------- //
TBoolean sk_CastSpellOnTerrain(int cc, int row, int col, int spell) // server
// ----------------------------------------------------------------- //

{
#define								kMaxProvocation						25

	int										fxIndex;
	int										j;
	int										i;
	unsigned long					currentTime=TickCount();
	float									time;
	float                 amount;
	int										theContainer=0;
	//float									theLock;
	//int										lockRange;
	float									theTrap;
	//int										trapRange;
	int										spawn;
	int										creatureType;
	float                 bonus;
	int                   theRow;
	int                   theCol;
	int										provocationList[kMaxProvocation];
	int										tempPtr;
	int										theTarget;
	float						damage;
	float						resist;
	//int							heal;
	//int 						ct;
	//int							level;
	//TBoolean					retailiate;
	//int 					mana; //Dave 06-11-2012
	int 					wolfColor; //Dave

	if ((row<0) || (row>=kMapWidth) || (col<0) || (col>=kMapHeight))			// beta debug
		fi_WriteToErrorLog("Out of Range - sk_CastSpellOnTerrain");

	//-- get magic bonus -------------------------------------------------------------

	bonus = 1.0;

	if (creature[cc].playerIndex!=0)
	{
		if (itemList[creature[cc].inventory[player[creature[cc].playerIndex].arm[kHead]].itemID].itemType==kWizardHatType)
			bonus = bonus + 0.15;
	}
	else
	{
		for (j=1;j<creature[cc].level;j++)
			bonus = bonus + 0.05;
	}

	//---------------------------------------------------------------------------------

	if (creature[cc].magicPoints>=itemList[spell].price)
		creature[cc].magicPoints=creature[cc].magicPoints-itemList[spell].price;
	else
		creature[cc].magicPoints=0;

	//-- teleport spell --------------------------------------------------------------

	if (spell==kTeleportSpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		fx_CreatureServerFX(cc, cc, kSparkle, (5*60), 0);

		if (creature[cc].playerIndex!=0)
			pl_PlayerMoveTo(creature[cc].playerIndex, row, col);
		else
		{
			creature[cc].row							=	row;
			creature[cc].col							=	col;
			creature[cc].moveDirection		= 4;
			creature[cc].timeStamp				= TickCount();
			creature[cc].strikePose				= 0;
			creature[cc].nextStrikeTime		= 0;
			creature[cc].numberOfStrikes	= 0;
		}
		
		return false;

	}

	//-- fire field spell --------------------------------------------------------------

	if (spell==kFireFieldSpell)
	{
		//Dave added time dependant on magery level 07-24-2012
		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)==5)
			time=10;
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)==6)
			time=15;
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)==7)
			time=20;
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)==8)
			time=25;
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)==9)
			time=30;
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)==10)
			time=35;
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)>=11)
			time=40;
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)>=16)
			time=100;
			
		//Original code			
		//if (creature[cc].playerIndex!=0)
		//	time=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*15;
		
		//else
		//	time=30;

		//time=30;

		//time=time*bonus;

		fxIndex=fx_AddServerFX(kFireField, (60*((tb_Rnd(5,30))+tb_Round(time))), row, col,0, 0);

		/*if (fxIndex>0)
		{
			if ((fxMap[row][col]>0) && (fxMap[row][col]<kMaxFX))  // mod 7/31
			{
				serverFX[fxMap[row][col]]						=0;
				serverFXKillTime[fxMap[row][col]]		=0;
				serverFXRow[fxMap[row][col]]				=0;
				serverFXCol[fxMap[row][col]]				=0;
			}

			fxMap[RC(row,kMapWidth)][RC(col,kMapHeight)]=fxIndex;
		}*/

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		if (creatureMap[row][col]!=0)
			if (creature[creatureMap[row][col]].playerIndex!=0)
				if (!(creature[creatureMap[row][col]].npcIndex!=0 && creature[creatureMap[row][col]].alignment!=kEvil))
					if ((creature[creatureMap[row][col]].master==0) || (cr_BothInPVPZone(cc,creatureMap[row][col])))
						if (creature[creatureMap[row][col]].alignment==kEvil)
							creature[creatureMap[row][col]].attackTarget=cc;

	}

	//-- summon creature spell --------------------------------------------------------------

	if (spell==kSummonCreatureSpell)
	{
		theRow=creature[cc].row+tb_Rnd(0,6)-3;
		theCol=creature[cc].col+tb_Rnd(0,6)-3;

		if (canSwimIn[map[theRow][theCol]])
		{
			creatureType=kWaterElemental;
		}
		else
		{
			creatureType=kAirElemental;
		}

		spawn=cr_SpawnCreature(creatureType,theRow,theCol);

		if (spawn>=0)
		{
			for (i=0;i<kMaxInventorySize;i++)
			{
				creature[spawn].inventory[i].itemID=0;
				creature[spawn].inventory[i].itemCount=0;
			}

			fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

			fx_CreatureServerFX(spawn, spawn, kSparkle, (5*60), 0);
			creature[spawn].attackTarget=creature[cc].attackTarget;
			creature[spawn].lifespan=TickCount()+(60*(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*5));
			//tb_SetForeColor(playBMP,0,39321,65535);
		}
	}

	//-- Greater summon creature spell -------//Dave-02-06-2011-------------------------------------------------------

		if (spell==kGreaterSummonCreatureSpell)
		{
			theRow=creature[cc].row+tb_Rnd(0,6)-3;
			theCol=creature[cc].col+tb_Rnd(0,6)-3;
			
			if (canSwimIn[map[theRow][theCol]])
				{
				creatureType=kWaterElemental;
				}
			
			else
				{
					if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)<=11)
						{
							creatureType=kEarthElemental;
						}
					else if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==12)
						{
							if (tb_Rnd(1,4)==1)
								{
								creatureType=kFireElemental;
								}
							else 
								{
								creatureType=kEarthElemental;
								}
						}
					else if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)>=13) 
						{
							if (tb_Rnd(1,15)==1)
								{
								creatureType=kPoisonElemental;
								}
							else if (tb_Rnd(1,3)==1)
								{
								creatureType=kFireElemental;
								}
							else 
								{
								creatureType=kEarthElemental;
								}
						}
					
				}

			spawn=cr_SpawnCreature(creatureType,theRow,theCol);

			if (spawn>=0)
			{
				for (i=0;i<kMaxInventorySize;i++)
				{
					creature[spawn].inventory[i].itemID=0;
					creature[spawn].inventory[i].itemCount=0;
				}

				fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

				fx_CreatureServerFX(spawn, spawn, kSparkle, (5*60), 0);
				creature[spawn].attackTarget=creature[cc].attackTarget;
				if (creatureType==kWaterElemental)
					{
					creature[spawn].lifespan=TickCount()+(60*(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*10));
					}
				else 
					{
					creature[spawn].lifespan=TickCount()+(60*(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*7));	
					}
			}
		}
	//-- provocation spell --------------------------------------------------------------

	if (spell==kProvocationSpell)
	{	
		//Dave level adjustment for provo
		int dmLevel=(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level));
	
		if (dmLevel<=8)
			(dmLevel=1);
		if (dmLevel>=10)
			(dmLevel=4);
		else (dmLevel=2);
		
		//theRow=creature[cc].row-6; //Dave changed range, cut in half 10-25-2010
		//theCol=creature[cc].col-6;
		theRow=creature[cc].row-3;
		theCol=creature[cc].col-3;
		tempPtr=0;

		for (i=0;i<kMaxProvocation;i++)
			provocationList[i]=0;
		
		for (theRow=(creature[cc].row-dmLevel);theRow<(creature[cc].row+dmLevel+1);theRow++)
			for (theCol=(creature[cc].col-dmLevel);theCol<(creature[cc].col+dmLevel+1);theCol++)
		
		/*
		//for (theRow=(creature[cc].row-6);theRow<(creature[cc].row+6);theRow++)
			//for (theCol=(creature[cc].col-6);theCol<(creature[cc].col+6);theCol++)
		//for (theRow=(creature[cc].row-3);theRow<(creature[cc].row+4);theRow++)			first change 10-25-2010
			//for (theCol=(creature[cc].col-3);theCol<(creature[cc].col+4);theCol++)
		
		*/
			
			
			{
				if (tempPtr>=kMaxProvocation)     continue;

				if (theRow<0)						          continue;

				if (theCol<0)						          continue;

				if (theRow>=kMapWidth)	          continue;

				if (theCol>=kMapHeight)	          continue;

				if (creatureMap[theRow][theCol]!=0)
					if (!((creature[creatureMap[theRow][theCol]].npcIndex!=0) || (creature[creatureMap[theRow][theCol]].id==kHuman)))
						if ((creature[creatureMap[theRow][theCol]].master==0) || (cr_BothInPVPZone(cc,creatureMap[theRow][theCol])))
							if (cr_ViewBlocked(creature[cc].row, creature[cc].col, theRow, theCol)==false)
								if (tb_Rnd(5,100)<(creatureInfo[creature[creatureMap[theRow][theCol]].id].magicResist*100))
								{
									provocationList[tempPtr]=creatureMap[theRow][theCol];
									tempPtr++;
								}
			}

		for (i=0;i<tempPtr;i++)
		{
			theTarget=tb_Rnd(0,(tempPtr-1));

			if (theTarget==i)
				theTarget=tb_Rnd(0,(tempPtr-1));

			if (theTarget==i)
				theTarget=tb_Rnd(0,(tempPtr-1));

			if (theTarget==i)
				theTarget=tb_Rnd(0,(tempPtr-1));

			if (theTarget==i)
				continue;

			creature[provocationList[i]].attackTarget=provocationList[theTarget];
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
	}

	//-- calm spell --------------------------------------------------------------

	if (spell==kCalmSpell)
	{	
		int dmLevel2=(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level));
		int range=1;
		
		//theRow=creature[cc].row-6; //Dave changed calm range 10-25-2010 
		//theCol=creature[cc].col-6;
		theRow=creature[cc].row-3;
		theCol=creature[cc].col-3;

		if (pl_AuthorizedTo(cc, kRegularPlayer))
			{
			if (dmLevel2==4)
				(range=1);
			
			if (dmLevel2==5)
				(range=1);
			
			if (dmLevel2==6)
				(range=2);
			
			if (dmLevel2==7)
				(range=2);
			
			if (dmLevel2==8)
				(range=3);
			
			if (dmLevel2==9)
				(range=3);
			
			if (dmLevel2>=10)
				(range=4);
			}
			//else (range=6);
		else (range=6);	
		
		
		//for (theRow=(creature[cc].row-6);theRow<(creature[cc].row+6);theRow++)
			//for (theCol=(creature[cc].col-6);theCol<(creature[cc].col+6);theCol++)
		
		for (theRow=(creature[cc].row-range);theRow<(creature[cc].row+range+1);theRow++)
			for (theCol=(creature[cc].col-range);theCol<(creature[cc].col+range+1);theCol++)
			{
				if (theRow<0)						          continue;

				if (theCol<0)						          continue;

				if (theRow>=kMapWidth)	          continue;

				if (theCol>=kMapHeight)	          continue;

				if (creatureMap[theRow][theCol]!=0)
					if (!((creature[creatureMap[theRow][theCol]].npcIndex!=0) || (creature[creatureMap[theRow][theCol]].id==kHuman)))
						if ((creature[creatureMap[theRow][theCol]].master==0) || (cr_BothInPVPZone(cc,creatureMap[theRow][theCol])))
							if (cr_ViewBlocked(creature[cc].row, creature[cc].col, theRow, theCol)==false)
								if (tb_Rnd(5,100)<(creatureInfo[creature[creatureMap[theRow][theCol]].id].magicResist*100))
								{
									creature[creatureMap[theRow][theCol]].calmUntil         = currentTime+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*100);
									creature[creatureMap[theRow][theCol]].attackTarget      = 0;
									creature[creatureMap[theRow][theCol]].attackTarget			=	0;
									creature[creatureMap[theRow][theCol]].nextStrikeTime		=	0;
									creature[creatureMap[theRow][theCol]].numberOfStrikes	  =	0;
									creature[creatureMap[theRow][theCol]].trackRow					= 0;
									creature[creatureMap[theRow][theCol]].trackCol					= 0;
									creature[creatureMap[theRow][theCol]].tracking					= false;
								}
			}

		creature[cc].attackTarget=0;

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		//fxIndex=fx_AddServerFX(kSteamFX, (30*60), row, col,0, 0);
	}

	//-- reveal spell --------------------------------------------------------------

	if (spell==kRevealSpell)
	{

		if (creature[cc].playerIndex!=0)
			player[creature[cc].playerIndex].revealHidden=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*15;
		else
		{
			theRow=creature[cc].row-6;
			theCol=creature[cc].col-6;

			for (theRow=(creature[cc].row-6);theRow<(creature[cc].row+6);theRow++)
				for (theCol=(creature[cc].col-6);theCol<(creature[cc].col+6);theCol++)
				{
					if (theRow<0)						          continue;

					if (theCol<0)						          continue;

					if (theRow>=kMapWidth)	          continue;

					if (theCol>=kMapHeight)	          continue;

					if (creatureMap[theRow][theCol]!=0)
						if (creature[creatureMap[theRow][theCol]].hiding)
							if (tb_Rnd(1,4)==1)
								creature[creatureMap[theRow][theCol]].hiding=false;
				}
		}

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

	}

	//-- mass heal spell --------------------------------------------------------------

	if (spell==kMassHealSpell)
	{
		//fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		//fx_CreatureServerFX(cc, cc, kMassHealX, (3*60),0);
		fx_CreatureServerFX(cc, cc, kHealFountain, (3*60),0);
		if (creature[cc].playerIndex!=0)
		{
			player[creature[cc].playerIndex].massheal=1+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryCleric].level)/2);
			sk_CastSpellOnCreature(cc, cc, kLesserHealSpell, true);
		}
	}
	
	//-- shapeshift Lesser spell --------------------------------------------------------------

	if (spell==kShapeshiftLesserSpell)
	{	
		
		spawn=tb_Rnd(1,3);
		wolfColor=tb_Rnd(1,2);
		
		//Dave
			
			
		
			//----------------Mana per level---------------------------------------------------
			/*					
			
			//if (sk_GetSkillLevel(player[i].skill[kMageryDruid].level)<=4)
				//mana=56;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)<=4)
				mana=56;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)==5)
				mana=61;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)==6)
				mana=66;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)==7)
				mana=71;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)==8)
				mana=76;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)==9)
				mana=82;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)==10)
				mana=87;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)==11)
				mana=92;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)==12)
				mana=97;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)==13)
				mana=102;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)>=14)
				mana=108;
			if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kMageryDruid].level)>=20)
				mana=5;*/
			//-------------------------------------------------------------------
		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)<=5)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=(tb_Rnd(44,46));} //dog
			if (spawn==2){player[creature[cc].playerIndex].morph=13;} //sheep
			if (spawn==3){player[creature[cc].playerIndex].morph=22;} //rat
			}	
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==6)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=30;} //giant ant
			if (spawn==2){player[creature[cc].playerIndex].morph=25;} //bat
			if (spawn==3){player[creature[cc].playerIndex].morph=29;} //snail
			}	
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)>=7)
			{
			if (spawn==1)
				{
				if (wolfColor==1) player[creature[cc].playerIndex].morph=9;
				if (wolfColor==2) player[creature[cc].playerIndex].morph=tb_Rnd(19,20);//wolves	
				}
			if (spawn==2){player[creature[cc].playerIndex].morph=30;} //giant ant
			if (spawn==3){player[creature[cc].playerIndex].morph=4;} //AE
			}	
		
		/*
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==7)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=14;} //scorpion
			if (spawn==2){player[creature[cc].playerIndex].morph=15;} //harpie
			if (spawn==3){player[creature[cc].playerIndex].morph=4;} //Air Ele
			}	
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==8)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=7;} //sludge
			if (spawn==2){player[creature[cc].playerIndex].morph=26;} //spider
			if (spawn==3){player[creature[cc].playerIndex].morph=73;} //Earth Ele
			}		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==9)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=51;} //snake
			if (spawn==2){player[creature[cc].playerIndex].morph=36;} //Rust beast
			if (spawn==3){player[creature[cc].playerIndex].morph=53;} //warrior ant
			
			//if ((spawn==1) && (wolfColor==1)){player[creature[cc].playerIndex].morph=8;} //Hell Hound
			//if ((spawn==1) && (wolfColor==2)){player[creature[cc].playerIndex].morph=17;} //poison Ele
			//if ((spawn==2) && (wolfColor==1)){player[creature[cc].playerIndex].morph=51;} //snake
			//if ((spawn==2) && (wolfColor==2)){player[creature[cc].playerIndex].morph=27;} //frost spider
			//if ((spawn==3) && (wolfColor==1)){player[creature[cc].playerIndex].morph=53;} //warrior ant
			//if ((spawn==3) && (wolfColor==2)){player[creature[cc].playerIndex].morph=36} //rust
		
			}		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==10)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=27;} //frost spider
			if (spawn==2){player[creature[cc].playerIndex].morph=18;} //poison Ele
			if (spawn==3){player[creature[cc].playerIndex].morph=8;} //Hell Hound
			}		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==11)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=56;} //blink
			if (spawn==2){player[creature[cc].playerIndex].morph=50;} //queen
			if (spawn==3){player[creature[cc].playerIndex].morph=28;} //lava spider
			}		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==12)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=69;} //lion
			if (spawn==2){player[creature[cc].playerIndex].morph=48;} //poison beast
			if (spawn==3){player[creature[cc].playerIndex].morph=62;} //tusker
			}		
		
		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==13)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=60;} //forest draco
			if (spawn==2){player[creature[cc].playerIndex].morph=59;} //ice draco
			if (spawn==3){player[creature[cc].playerIndex].morph=77;} //sand draco
			}		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)>=16)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=24;} //FD
			if (spawn==2){player[creature[cc].playerIndex].morph=10;} //RD
			if (spawn==3){player[creature[cc].playerIndex].morph=11;} //ID
			}		
		*/
		
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
		
		
		if (creature[cc].playerIndex!=0)
		{
			/*if (spawn==1)
				wolfColor=tb_Rnd(1,2);
				
				if (wolfColor==1)
				player[creature[cc].playerIndex].morph=9;
				if (wolfColor==2)
				player[creature[cc].playerIndex].morph=tb_Rnd(19,20);
			else if (spawn==2)
				player[creature[cc].playerIndex].morph=15;
			else if (spawn==3)
				player[creature[cc].playerIndex].morph=4;
			else if (spawn==4)
				player[creature[cc].playerIndex].morph=80;
			else if (spawn==5)
				player[creature[cc].playerIndex].morph=14;
			else if (spawn==6)
				player[creature[cc].playerIndex].morph=25;
			else if (spawn==7)
				player[creature[cc].playerIndex].morph=22;
			else if (spawn==8)
				player[creature[cc].playerIndex].morph=29;
			else if (spawn==9)
				player[creature[cc].playerIndex].morph=51;
			else if (spawn==10)
				player[creature[cc].playerIndex].morph=tb_Rnd(52,54);
			*/
			
			player[creature[cc].playerIndex].morphTime=currentTime+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*tb_Rnd(130,160);
			
			nw_SendClearName(creature[cc].playerIndex);
		}
	}
	/*  //Dave Original spell
	if (spell==kShapeshiftWolfSpell)
	{
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		if (creature[cc].playerIndex!=0)
		{
			if (tb_Rnd(1,3)==1)
				player[creature[cc].playerIndex].morph=9;
			else
				player[creature[cc].playerIndex].morph=tb_Rnd(19,20);
			
			player[creature[cc].playerIndex].morphTime=currentTime+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*120;
			
			nw_SendClearName(creature[cc].playerIndex);
		}
	}
	*/
	
	//-- shapeshift  spell --------------------------------------------------------------

	if (spell==kShapeshiftSpell)
	{
		spawn=tb_Rnd(1,3);
		wolfColor=tb_Rnd(1,2);
		
		fx_CreatureServerFX(cc, cc, kLightEllipse, (1*60), 10);

		if (creature[cc].playerIndex!=0)
		{
			if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)<=8)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=14;} //scorpion
			if (spawn==2){player[creature[cc].playerIndex].morph=15;} //harpie
			if (spawn==3){player[creature[cc].playerIndex].morph=73;} //Earth Ele
			}	
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==9)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=7;} //sludge
			if (spawn==2){player[creature[cc].playerIndex].morph=26;} //spider
			if (spawn==3){player[creature[cc].playerIndex].morph=73;} //Earth Ele
			}		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)>=10)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=80;} //Brown Bear
			if (spawn==2){player[creature[cc].playerIndex].morph=36;} //Rust beast
			if (spawn==3){player[creature[cc].playerIndex].morph=53;} //warrior ant
			}
			player[creature[cc].playerIndex].morphTime=currentTime+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*tb_Rnd(130,160);
			
			nw_SendClearName(creature[cc].playerIndex);
		}
	}
	
	
	
	//-- greater shapeshift spell --------------------------------------------------------------

	if (spell==kShapeshiftGreaterSpell)
	{
		spawn=tb_Rnd(1,3);
		wolfColor=tb_Rnd(1,2);
		
		fx_CreatureServerFX(cc, cc, kLightEllipse, (1*60), 10);

		if (creature[cc].playerIndex!=0)
		{
			if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)<=11)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=27;} //frost spider
			if (spawn==2){player[creature[cc].playerIndex].morph=33;} //Centaur
			if (spawn==3){player[creature[cc].playerIndex].morph=8;} //Hell Hound
			}		
		/*if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==12)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=56;} //blink
			if (spawn==2){player[creature[cc].playerIndex].morph=50;} //queen
			if (spawn==3){player[creature[cc].playerIndex].morph=28;} //lava spider
			}	*/	
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)==12)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=69;} //lion
			if (spawn==2){player[creature[cc].playerIndex].morph=48;} //poison beast
			if (spawn==3){player[creature[cc].playerIndex].morph=62;} //tusker
			}		
		
		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)>=13)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=60;} //forest draco
			if (spawn==2){player[creature[cc].playerIndex].morph=59;} //ice draco
			if (spawn==3){player[creature[cc].playerIndex].morph=77;} //sand draco
			}		
		if (sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)>=16)
			{
			if (spawn==1){player[creature[cc].playerIndex].morph=24;} //FD
			if (spawn==2){player[creature[cc].playerIndex].morph=10;} //RD
			if (spawn==3){player[creature[cc].playerIndex].morph=11;} //ID
			}		
			
			player[creature[cc].playerIndex].morphTime=currentTime+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*tb_Rnd(130,160);
			
			nw_SendClearName(creature[cc].playerIndex);
		}
	}
	
	//-- shapeshift all spell --------------------------------------------------------------

	if (spell==kShapeshiftAllSpell)
	{
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		if (creature[cc].playerIndex!=0)
		{
			player[creature[cc].playerIndex].morph=tb_Rnd(3,82);
			
			player[creature[cc].playerIndex].morphTime=currentTime+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*300;
		
			nw_SendClearName(creature[cc].playerIndex);
		}
	}
	
	//-- shapeshift dragon spell --------------------------------------------------------------

	if (spell==kShapeshiftDragonSpell)
	{
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		if (creature[cc].playerIndex!=0)
		{
			player[creature[cc].playerIndex].morph=10;
			
			player[creature[cc].playerIndex].morphTime=currentTime+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level)*60;
		
			nw_SendClearName(creature[cc].playerIndex);
		}
	}

	//-- turn undead spell --------------------------------------------------------------

	if (spell==kTurnUndeadSpell)
	{
		int cmLevel=(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryCleric].level));
		int range=1;
		
		theRow=creature[cc].row-6;
		theCol=creature[cc].col-6;

			if (pl_AuthorizedTo(cc, kRegularPlayer))
			{
			if (cmLevel==7)
				(range=1);
			
			if (cmLevel==8)
				(range=2);
			
			if (cmLevel==9)
				(range=3);
			
			if (cmLevel==10)
				(range=4);
			
			if (cmLevel==11)
				(range=5);
			
			if (cmLevel>=12)
				(range=6);
			
	
			}
			//else (range=6);
			else (range=6);	


		for (theRow=(creature[cc].row-range);theRow<(creature[cc].row+range+1);theRow++)
			for (theCol=(creature[cc].col-range);theCol<(creature[cc].col+range+1);theCol++)
			{
				if (theRow<0)						          continue;

				if (theCol<0)						          continue;

				if (theRow>=kMapWidth)	          continue;

				if (theCol>=kMapHeight)	          continue;

				if (creatureMap[theRow][theCol]!=0)
				{
					theTarget=creatureMap[theRow][theCol];
					if (cr_ViewBlocked(creature[cc].row, creature[cc].col, theRow, theCol)==false)
					if (creature[theTarget].id==kSkeleton || creature[theTarget].id==kSkeletonArcher || creature[theTarget].id==kBoneMage || creature[theTarget].id==kZombie || creature[theTarget].id==kHellMage || creature[theTarget].id==kWrath)
					{
						//Stop attacking cleric

						if (creature[theTarget].attackTarget==cc)
						{
							creature[theTarget].attackTarget				= 0;
							creature[theTarget].possibleTarget				= 0;
							creature[theTarget].nextStrikeTime				= 0;
							creature[theTarget].numberOfStrikes				= 0;
							creature[theTarget].trackRow					= 0;
							creature[theTarget].trackCol					= 0;
							creature[theTarget].tracking					= false;
						}

						//Actual damage
						resist=creatureInfo[creature[theTarget].id].magicResist;

						for (j=1;j<creature[theTarget].level;j++)
							resist=resist*0.9;

						damage=tb_Rnd(15,20)+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryCleric].level)-7)*5;

						damage=damage*bonus;

						damage=damage*resist;
						
						if (creature[theTarget].damageReduc<1.0)
							damage=tb_Round((float)damage*creature[theTarget].damageReduc);

						if (damage>0)
						{
							//cr_AddSound(theTarget, kExplosionSnd);
							//fx_CreatureServerFX(theTarget, theTarget, kLightExplosion, (2*60), 0);
							if (damage>creature[theTarget].hitPoints)
								damage=creature[theTarget].hitPoints;

							creature[theTarget].hitPoints=creature[creatureMap[theRow][theCol]].hitPoints-damage;

							if (creature[theTarget].hitPoints==0)
								cr_setDieTime(theTarget,(currentTime+120));
						}
					}
				}
			}



		fx_CreatureServerFX(cc, cc, kLightEllipse, (5*60), 10);

		player[creature[cc].playerIndex].turnUntil=TickCount()+(60*20);

	}

	//-- create food spell --------------------------------------------------------------

	if (spell==kCreateFoodSpell)
	{

		amount=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level);

		amount=amount*bonus;

		in_DropItem(0,1,kLoafOfBread, tb_Round(amount), row, col, kThreeMinuteDecay, false, "");

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		return(false);

	}

	//-- magic disarm spell --------------------------------------------------------------

	if (spell==kMagicDisarmSpell)
	{


		theContainer=containerMap[creature[cc].row][creature[cc].col];

		if (theContainer!=0)
		{
			if (containerMagicTrap[theContainer]==0) //Not trapped, can't disarm
				nw_SendDisplayMessage(cc, kSpellFizzles);
			else
			{
				fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
				theTrap=tb_Rnd(1,sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level));
				theTrap=theTrap*bonus;

				if (containerMagicTrap[theContainer]<=tb_Round(theTrap))
					containerMagicTrap[theContainer]=0;
			}
		}
		
		return false;

	}

	//-- paralyze field spell --------------------------------------------------------------

	if (spell==kParalyzeFieldSpell)
	//if (cr_ViewBlocked(creature[cc].row, creature[cc].col, theRow, theCol)==false)
	{
		
		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		if (creature[cc].playerIndex!=0)
			player[creature[cc].playerIndex].paralyzeField=tb_Rnd(10,20)+sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level);

	}

	//-- disorient field spell --------------------------------------------------------------

	if (spell==kDisorientFieldSpell)
		//if (cr_ViewBlocked(creature[cc].row, creature[cc].col, theRow, theCol)==false)
			{
				fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

				if (creature[cc].playerIndex!=0)
					player[creature[cc].playerIndex].disorientfield=1+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)/2);

			}

	//-- poison field spell --------------------------------------------------------------

	if (spell==kPoisonFieldSpell)
		//if (cr_ViewBlocked(creature[cc].row, creature[cc].col, theRow, theCol)==false)
			{

				fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

				if (creature[cc].playerIndex!=0)
					player[creature[cc].playerIndex].poisonField=1+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)/2);

			}

	//-- magic trap spell --------------------------------------------------------------

	if (spell==kMagicTrapSpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		theContainer=containerMap[creature[cc].row][creature[cc].col];

		if (theContainer!=0)
		{
			strcpy(containerMagicTrapName[theContainer],"");

			if (creature[cc].playerIndex!=0)
				if (strlen(player[creature[cc].playerIndex].playerNameLC)<15)
					strcpy(containerMagicTrapName[theContainer],player[creature[cc].playerIndex].playerNameLC);

			theTrap=tb_Rnd(1,sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level));

			theTrap=tb_Round(theTrap*bonus);

			if (theTrap<1)
				theTrap=1;

			if (theTrap>19)
				theTrap=19;

			if (containerMagicTrap[theContainer]<theTrap)
				containerMagicTrap[theContainer]=theTrap;
		}
		
		return false;

	}

	//-- Summon pet spell (formerly magic lock spell) --------------------------------------------------------------

	if (spell==kSummonPetSpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);

		for (j=0; j<kMaxPets; j++)
		{
			if (player[cc].petInStable[j]==false)
			{
				cr_CreatureMoveTo(player[cc].pet[j] , creature[player[cc].creatureIndex].row, creature[player[cc].creatureIndex].col);
				fx_CreatureServerFX(player[cc].pet[j], player[cc].pet[j], kSparkle, (5*60), 0);
			}
		}

		return (false);

		/*theContainer=containerMap[creature[cc].row][creature[cc].col];
		if (theContainer!=0)
			{
						theLock=tb_Rnd(1,sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level));
						theLock=tb_Round(theLock*bonus);
						if (theLock<1)
							theLock=1;
						if (theLock>99)
							theLock=99;
						if (containerMagicLock[theContainer]<theLock)
							containerMagicLock[theContainer]=theLock;
			}*/


	}

	//-- heal pet spell --------------------------------------------------------------

	if (spell==kHealPetSpell)
	{

		fx_CreatureServerFX(cc, cc, kLightEllipse, (1*60), 10);

		for (j=0; j<kMaxPets; j++)
		{
			if (player[cc].petInStable[j]==false)
			{
				if (creature[player[cc].pet[j]].regeneratingTime==0)
				{
					creature[player[cc].pet[j]].regeneratingTime=TickCount()+60*10;
					creature[player[cc].pet[j]].regeneratingPower=sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level);
					fx_CreatureServerFX(player[cc].pet[j], player[cc].pet[j], kSparkle, (5*60), 0);
				}
			}
		}

		/*theContainer=containerMap[creature[cc].row][creature[cc].col];
		if (theContainer!=0)
			{
				if (containerMagicLock[theContainer]==0) //Not locked, can't unlock
					nw_SendDisplayMessage(cc, kSpellFizzles);
				else
					{
						fx_CreatureServerFX(cc, cc, kLightEllipse, (3*60), 10);
						theLock=tb_Rnd(1,sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryDruid].level));
						theLock=tb_Round(theLock*bonus);
						if (containerMagicLock[theContainer]<=theLock)
							containerMagicLock[theContainer]=0;
					}
			}*/

	}

	return(true);
}

// ----------------------------------------------------------------- //
void sk_ResistingSpells(int c,int i) // server
// ----------------------------------------------------------------- //

{
	TBoolean        wearingRobe=false;

	if (player[i].magicAttacker==c)
		if (player[i].resistanceGained>12)
			return;

	if (player[i].magicAttacker!=c)
		player[i].resistanceGained=0;

	player[i].magicAttacker=c;

	player[i].resistanceGained++;

	if (in_ItemEquipedServer(i,kGrayRobeResistance))							wearingRobe=true;
	else if (in_ItemEquipedServer(i,kLightBlueRobeResistance))		wearingRobe=true;
	else if (in_ItemEquipedServer(i,kRedRobeResistance))					wearingRobe=true;
	else if (in_ItemEquipedServer(i,kBlueRobeResistance))					wearingRobe=true;
	else if (in_ItemEquipedServer(i,kBrownRobeResistance))				wearingRobe=true;
	else if (in_ItemEquipedServer(i,KBlackRobeResistance))				wearingRobe=true;
	else if (in_ItemEquipedServer(i,kGreenRobeResistance))				wearingRobe=true;
	else if (in_ItemEquipedServer(i,kPurpleRobeResistance))				wearingRobe=true;
	else if (in_ItemEquipedServer(i,kYellowRobeResistance))				wearingRobe=true;

	if (wearingRobe)
		if (tb_Rnd(1,2)==1)
			return;
	if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kResistingMagic].level)<=2)
		{
		sk_RaiseSkill(i,0,kResistingMagic,5);  //Dave sk_RaiseSkill(i,0,kResistingMagic,1); 	
		}
	if (sk_GetSkillLevel(player[creature[i].playerIndex].skill[kResistingMagic].level)>2)
		{
		sk_RaiseSkill(i,0,kResistingMagic,2);  //Dave sk_RaiseSkill(i,0,kResistingMagic,1); 	
		}

}

// ----------------------------------------------------------------- //
void sk_MageryOnCreature(int i, int c, int spell) // server
// ----------------------------------------------------------------- //

{
	int							item;
	int							j;
	int							k;
	int							rowDelta;
	int							colDelta;
	TBoolean        harmfulSpell;
	//TBoolean        targetInTown;
	//TBoolean        attackerInTown;
	char						magicWords[kStringLength];
	float           success=50;
	float           inc=25;
	int             chance;
	int             loop;
	TBoolean				skillGain;	// merge 7/28
	TBoolean        inPVPZone;
	TBoolean		usingRing;


	pl_CheckForActionInterrupted(i, false);

	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	if (c==0)
	{
		nw_SendDisplayMessage(i,kSpellFizzles);
		return;
	}

	if (creature[c].disorientation>0)
		if (tb_Rnd(1,100)>25)
		{
			nw_SendDisplayMessage(i,kSpellFizzles);
			return;
		}

	if (creature[c].dieTime!=0)
	{
		nw_SendDisplayMessage(i,kSpellFizzles);
		return;
	}

	if (creature[c].playerIndex!=0)
		if (player[creature[c].playerIndex].dead)
			if (spell!=kResurrectTarget)
			{
				nw_SendDisplayMessage(i,kSpellFizzles);
				return;
			}

	if (cr_ViewBlocked(creature[player[i].creatureIndex].row, creature[player[i].creatureIndex].col, creature[c].row, creature[c].col))
	{
		nw_SendDisplayMessage(i,kCantSeeTarget);
		return;
	}

	if (spell==kResurrectTarget)						{ item=kResurrectSpell;        	harmfulSpell=false; }

	if (spell==kLightningBoltTarget)				{ item=kLightningBoltSpell;     harmfulSpell=true;  }

	if (spell==kLesserHealTarget)						{ item=kLesserHealSpell;        harmfulSpell=false; }

	if (spell==kGreaterHealTarget)					{ item=kGreaterHealSpell;       harmfulSpell=false; }

	if (spell==kRemoveCurseTarget)					{ item=kRemoveCurseSpell;       harmfulSpell=false; }

	if (spell==kFullHealTarget)							{ item=kFullHealSpell;       		harmfulSpell=false; }

	if (spell==kPoisonTarget)								{ item=kPoisonSpell;            harmfulSpell=true;  }

	if (spell==kLesserDetoxifyTarget)				{ item=kLesserDetoxifySpell;    harmfulSpell=false; }

	if (spell==kGreaterDetoxifyTarget)			{ item=kGreaterDetoxifySpell;   harmfulSpell=false; }

	if (spell==kFullDetoxifyTarget)					{ item=kFullDetoxifySpell;   		harmfulSpell=false; }

	if (spell==kExplosionTarget)						{ item=kExplosionSpell;         harmfulSpell=true;  }

	if (spell==kInvisibilityTarget)					{ item=kInvisibilitySpell;      harmfulSpell=false; }

	if (spell==kOpportunityTarget)					{ item=kOpportunitySpell;      harmfulSpell=false; }

	if (spell==kNightVisionTarget)					{ item=kNightVisionSpell;       harmfulSpell=false; }

	if (spell==kParalyzeTarget)							{ item=kParalyzeSpell;          harmfulSpell=true;  }

	if (spell==kDrainManaTarget)						{ item=kDrainManaSpell;         harmfulSpell=true;  }

	if (spell==kReflectiveArmorTarget)			{ item=kReflectiveArmorSpell;   harmfulSpell=false; }

	if (spell==kDisorientationTarget)				{ item=kDisorientationSpell;    harmfulSpell=true;  }

	if (spell==kMagicReflectionTarget)			{ item=kMagicReflectionSpell;   harmfulSpell=false; }

	if (spell==kClumsySpellTarget)			    { item=kClumsySpell;            harmfulSpell=true;  }

	if (spell==kAgilitySpellTarget)			    { item=kAgilitySpell;           harmfulSpell=false; }

	if (spell==kWeakenSpellTarget)			    { item=kWeakenSpell;            harmfulSpell=true;  }

	if (spell==kStrengthSpellTarget)			  { item=kStrengthSpell;          harmfulSpell=false; }

	if (spell==kSimpleMindSpellTarget)			{ item=kSimpleMindSpell;        harmfulSpell=true;  }

	if (spell==kElevateMindTarget)			    { item=kElevateMindSpell;       harmfulSpell=false; }

	if (spell==kFireballTarget)			    { item=kFireballSpell;       harmfulSpell=true; }

	if (spell==kFireblastTarget)			    { item=kFireblastSpell;       harmfulSpell=true; }

	if (spell==kMagicMissileTarget)			    { item=kMagicMissileSpell;       harmfulSpell=true; }

	if (spell==kFireblast3Target)			    { item=kFireblast3Spell;       harmfulSpell=true; }

	if (spell==kFireblast4Target)			    { item=kFireblast4Spell;       harmfulSpell=true; }

	if (spell==kFireblast5Target)			    { item=kFireblast5Spell;       harmfulSpell=true; }

	inPVPZone=false;

	if ((creature[player[i].creatureIndex].row>1076) && (creature[player[i].creatureIndex].row<1150))
		if ((creature[player[i].creatureIndex].col>1681) && (creature[player[i].creatureIndex].col<=1765))
			if ((creature[c].row>1076) && (creature[c].row<1150))
				if ((creature[c].col>1681) && (creature[c].col<=1765))
					inPVPZone=true;

	if (harmfulSpell)
	{

		if ( player[i].category!=5 && (creature[c].id==kCentaur || creature[c].id==kElderCentaur) )
		{
			nw_SendDisplayMessage(i,kSpellFizzles);
			return;
		}

		if (player[i].creatureIndex==c) // can't cast harmful spell on self 0.1.3
		{

			nw_SendDisplayMessage(i,kSpellFizzles);
			return;
		}

		if (inPVPZone==false)
			if (player[i].god==false)
				if (player[i].category!=5 && player[i].category!=8)	// merge 7/28
					if (((creature[c].master!=0) || (creature[c].alignment==kGood)) && (pl_IsThief(c)==false))
					{
						nw_SendDisplayMessage(i,kCantAttackCreature);
						return;
					}
	}
	else
	{
		if (inPVPZone==false)
		{
			if ((creature[c].master==0) && (creatureInfo[creature[c].id].alignment==kEvil))	// 0.1.4
		//	if ((creature[c].master!=0) && (creatureInfo[creature[c].id].alignment==kEvil)) //Dave 01-13-2013
			{
				nw_SendDisplayMessage(i,kSpellFizzles);	// can't cast helpful spell on evil creature
				return;
			}

			if (player[i].category!=5 && player[i].category!=8)  // merge 7/28
				if (player[i].creatureIndex!=c)
					
						if (pl_IsThief(c))	// 017
						{
							nw_SendDisplayMessage(i,kSpellFizzles);	// can't cast helful spell on scoundrel
							return;
						}
		}
	}

	if (item==kResurrectSpell)
	{
		if (creature[c].playerIndex==0)
		{
			nw_SendDisplayMessage(i,kSpellFizzles);	// can't cast res on non human
			return;
		}

		if (player[creature[c].playerIndex].dead==false)
		{
			nw_SendDisplayMessage(i,kSpellFizzles);	// can't cast res on living
			return;
		}
	}

	usingRing=false;

	if (player[i].arm[kRightFinger]!=0 && itemList[creature[player[i].creatureIndex].inventory[player[i].arm[kRightFinger]].itemID].spell==item)
		usingRing=true;

	if (player[i].arm[kLeftFinger]!=0 && itemList[creature[player[i].creatureIndex].inventory[player[i].arm[kLeftFinger]].itemID].spell==item)
		usingRing=true;


	if (!usingRing)
	{
		if (sk_GetSkillLevel(player[i].skill[itemList[item].makeSkill].level)<itemList[item].makeSkillLevel)
		{
			nw_SendDisplayMessage(i,kSpellFizzles);
			return;
		}

		if (creature[player[i].creatureIndex].magicPoints<itemList[item].price)
		{
			nw_SendDisplayMessage(i,kNoManaForSpell);
			return;
		}

		for (j=0;j<5;j++)
			if (itemList[item].ingredientCount[j]>0)
				if (in_QuantityOf_s(player[i].creatureIndex,itemList[item].ingredientID[j])<itemList[item].ingredientCount[j])	// not enough reagents
				{
					nw_SendDisplayMessage(i,kNotEnoughReagents);
					return;
				}
	}

	// --------------- face direction ---------------

	rowDelta=creature[c].row-creature[player[i].creatureIndex].row;

	colDelta=creature[c].col-creature[player[i].creatureIndex].col;

	if ((rowDelta<=-1) && (colDelta<=-1)) 				creature[player[i].creatureIndex].facing=0;
	else if ((rowDelta<=-1) && (colDelta==0)) 		creature[player[i].creatureIndex].facing=1;
	else if ((rowDelta<=-1) && (colDelta>=1)) 		creature[player[i].creatureIndex].facing=2;
	else if ((rowDelta==0) && (colDelta<=-1)) 		creature[player[i].creatureIndex].facing=7;
	else if ((rowDelta==0) && (colDelta>=1)) 			creature[player[i].creatureIndex].facing=3;
	else if ((rowDelta>=1) && (colDelta<=-1)) 		creature[player[i].creatureIndex].facing=6;
	else if ((rowDelta>=1) && (colDelta==0)) 			creature[player[i].creatureIndex].facing=5;
	else if ((rowDelta>=1) && (colDelta>=1)) 			creature[player[i].creatureIndex].facing=4;

	strcpy(magicWords,"* ");

	if (!usingRing)
		for (j=0;j<5;j++)
			if (itemList[item].ingredientCount[j]>0)
			{
				for (k=1;k<kMaxInventorySize;k++)
					if (creature[player[i].creatureIndex].inventory[k].itemID==itemList[item].ingredientID[j])
					{

						if (creature[player[i].creatureIndex].inventory[k].itemID==kGinseng)
						{
							ginsengSpawn        = ginsengSpawn       + itemList[item].ingredientCount[j];
							strcat(magicWords,"pur ");
						}
						else if (creature[player[i].creatureIndex].inventory[k].itemID==kLotusFlower)
						{
							lotusFlowerSpawn    = lotusFlowerSpawn   + itemList[item].ingredientCount[j];
							strcat(magicWords,"clar ");
						}
						else if (creature[player[i].creatureIndex].inventory[k].itemID==kMandrakeRoot)
						{
							mandrakeRootSpawn   = mandrakeRootSpawn  + itemList[item].ingredientCount[j];
							strcat(magicWords,"shel ");
						}
						else if (creature[player[i].creatureIndex].inventory[k].itemID==kGarlic)
						{
							garlicSpawn         = garlicSpawn        + itemList[item].ingredientCount[j];
							strcat(magicWords,"rel ");
						}
						else if (creature[player[i].creatureIndex].inventory[k].itemID==kVolcanicAsh)
						{
							volcanicAshSpawn    = volcanicAshSpawn   + itemList[item].ingredientCount[j];
							strcat(magicWords,"flas ");
						}
						else if (creature[player[i].creatureIndex].inventory[k].itemID==kPoisonGland)
						{
							strcat(magicWords,"pos ");
						}
						else if (creature[player[i].creatureIndex].inventory[k].itemID==kElectricEel)
						{
							strcat(magicWords,"bol ");
						}
						else if (creature[player[i].creatureIndex].inventory[k].itemID==kAmber)
						{
							strcat(magicWords,"ber ");
						}
						else if (creature[player[i].creatureIndex].inventory[k].itemID==kCoal)
						{
							strcat(magicWords,"bur ");
						}
						else if (creature[player[i].creatureIndex].inventory[k].itemID==kDragonTooth)
						{
							strcat(magicWords,"des ");
						}

						in_RemoveFromInventorySlot(player[i].creatureIndex,k,itemList[item].ingredientCount[j]);

						break;
					}
			}

	strcat(magicWords,"*");

	if (usingRing)
		strcpy(magicWords, "* Uses Magic Item *");

	if (player[i].curse&kBabbleCurse && !usingRing)
		strcpy(magicWords,"* babble babble babble *");

	if (creature[player[i].creatureIndex].mute) //018, muted players can't cast.
	{
		nw_SendTextToClient(i, "You can't cast any spell. You have been muted.", 0);
		return;
	}
	
	if (player[i].morph!=0)
		strcpy(magicWords, "");

	creature[player[i].creatureIndex].talkWhen	= TickCount();

	creature[player[i].creatureIndex].talkTime	= TickCount();
	strcpy(creature[player[i].creatureIndex].talk,magicWords);
	creature[player[i].creatureIndex].talkID	  = chatMessageID;
	chatMessageID++;

	nw_SendInventoryRefresh(i);

	loop=sk_GetSkillLevel(player[i].skill[itemList[item].makeSkill].level);  // determine success chance

	for (k=0;k<loop;k++)
	{
		success=success+inc;
		inc=inc/2;
	}

	chance=tb_Round(success);

	if (player[i].curse&kBabbleCurse && !usingRing)
		chance=0;

	if (tb_Rnd(1,100)<chance || usingRing)
	{
		skillGain=sk_CastSpellOnCreature(player[i].creatureIndex,c,item, false);	// merge 7/28
		player[i].sendStatsRefresh=true;

		if (skillGain && !usingRing)	// merge 7/28
		{
			if (harmfulSpell)
				sk_TrainProof(i,c,item, itemList[item].makeSkill, itemList[item].makeSkillLevel);
			else
				sk_TrainProof(i,0,item, itemList[item].makeSkill, itemList[item].makeSkillLevel);
			
		}
			
			//sk_RaiseSkill(i,0,itemList[item].makeSkill,itemList[item].makeSkillLevel);

		return;
	}

	nw_SendDisplayMessage(i,kSpellFizzles);

}

// ----------------------------------------------------------------- //
void sk_MageryOnTerrain(int i, int row, int col, int spell) // server
// ----------------------------------------------------------------- //

{
	int							item;
	int							j;
	int							k;
	int							rowDelta;
	int							colDelta;
	char						magicWords[kStringLength];
	TBoolean        harmfulSpell;
	TBoolean		skillGain;
	TBoolean		found;
	float           success=50;
	float           inc=25;
	int             chance;
	int             loop;

	pl_CheckForActionInterrupted(i, false);

	if ((row<0) || (row>=kMapWidth) || (col<0) || (col>=kMapHeight))			// beta debug
		fi_WriteToErrorLog("Out of Range - sk_MageryOnTerrain");

	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	if (cr_ViewBlocked(creature[player[i].creatureIndex].row, creature[player[i].creatureIndex].col, row, col))
	{
		nw_SendDisplayMessage(i,kCantSeeTarget);
		return;
	}

	if (creature[player[i].creatureIndex].disorientation>0)
		if (tb_Rnd(1,100)>25)
		{
			nw_SendDisplayMessage(i,kSpellFizzles);
			return;
		}

	item=spell;

	harmfulSpell=false;

	if (spell==kCastTurnUndeadSpell)				item=kTurnUndeadSpell;

	if (spell==kCastCreateFoodSpell)				item=kCreateFoodSpell;

	if (spell==kFireFieldTarget)						{ item=kFireFieldSpell; harmfulSpell=true; }

	if (spell==kTeleportTarget)							item=kTeleportSpell;

	if (spell==kCastSummonPetSpell)					item=kSummonPetSpell;

	if (spell==kCastHealPetSpell)				item=kHealPetSpell;

	if (spell==kCastMagicTrapSpell)					{ item=kMagicTrapSpell; harmfulSpell=true; }

	if (spell==kCastMagicDisarmSpell)				item=kMagicDisarmSpell;
	//Dave
	//if (spell==kCastChaosFieldSpell)			{	item=kChaosFieldSpell; harmfulSpell=true; }

	if (spell==kCastSummonCreatureSpell)		{ item=kSummonCreatureSpell; harmfulSpell=true; }
	//Dave 02-06-2011
	if (spell==kCastGreaterSummonCreatureSpell)		{ item=kGreaterSummonCreatureSpell; harmfulSpell=true; }

	if (spell==kCastRevealSpell)		        item=kRevealSpell;

	if (spell==kCastParalyzeFieldSpell)			{ item=kParalyzeFieldSpell; }

	if (spell==kCastPoisonFieldSpell)				{ item=kPoisonFieldSpell; }

	if (spell==kCastProvocationSpell)		      item=kProvocationSpell;

	if (spell==kCastCalmSpell)		            item=kCalmSpell;

	if (spell==kCastMassHealSpell)				item=kMassHealSpell;
	
	if (spell==kCastShapeshiftLesserSpell)		item=kShapeshiftLesserSpell;
	
	if (spell==kCastShapeshiftSpell)			item=kShapeshiftSpell;
	
	if (spell==kCastShapeshiftGreaterSpell)		item=kShapeshiftGreaterSpell;
	
	if (spell==kCastShapeshiftAllSpell)			item=kShapeshiftAllSpell;

	if (spell==kCastShapeshiftDragonSpell)		item=kShapeshiftDragonSpell;

	if (spell==kCastDisorientFieldSpell)			item=kDisorientFieldSpell;

	if (harmfulSpell)
		if (player[i].god==false)
			if (player[i].category!=5)	// merge 7/28
				if (cr_InTown(player[i].creatureIndex))
				{
					nw_SendDisplayMessage(i,kCantCastInTown);
					return;
				}

	if (item==kTeleportSpell)
	{
		if (!cr_InDomain(player[i].creatureIndex,row,col))
		{
			nw_SendDisplayMessage(i,kSpellFizzles);
			return;
		}

		if (guildMap[row][col]!=0)
			if (strcmp(guildHalls[guildMap[row][col]].guild, "All")!=0)
				if (strcmp(guildHalls[guildMap[row][col]].guild, "all")!=0)
					if (strcmp(guildHalls[guildMap[row][col]].guild, player[i].guildName)!=0)
					{
						nw_SendDisplayMessage(i,kSpellFizzles);
						return;
					}

	}


	if (item==kMagicDisarmSpell)
		if (containerMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col]==0)
		{
			nw_SendDisplayMessage(i,kSpellFizzles);
			return;
		}

	if (item==kMagicTrapSpell)
		if (containerMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col]==0)
		{
			nw_SendDisplayMessage(i,kSpellFizzles);
			return;
		}

	if (item==kSummonPetSpell)
		//if (containerMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col]==0)
	{
		found=false;

		for (j=0; j<kMaxPets; j++)
			if (creature[player[i].pet[j]].id!=0 && !player[i].petInStable[j])
				found=true;

		if (!found)
		{
			nw_SendDisplayMessage(i,kSpellFizzles);
			return;
		}
	}

	if (item==kHealPetSpell)
	{
		found=false;

		for (j=0; j<kMaxPets; j++)
			if (creature[player[i].pet[j]].id!=0 && !player[i].petInStable[j])
				found=true;

		if (!found)
		{
			nw_SendDisplayMessage(i,kSpellFizzles);
			return;
		}
	}

	if (sk_GetSkillLevel(player[i].skill[itemList[item].makeSkill].level)<itemList[item].makeSkillLevel) //018 druid cast bug
		//if (sk_GetSkillLevel(player[i].skill[kMageryWizard].level)<itemList[item].makeSkillLevel)
	{
		nw_SendDisplayMessage(i,kSpellFizzles);
		return;
	}
	
	
	
	if (creature[player[i].creatureIndex].magicPoints<itemList[item].price)
	
	{
		nw_SendDisplayMessage(i,kNoManaForSpell);
		return;
	}

	
	
	for (j=0;j<5;j++)
		if (itemList[item].ingredientCount[j]>0)
			if (in_QuantityOf_s(player[i].creatureIndex,itemList[item].ingredientID[j])<itemList[item].ingredientCount[j])	// not enough reagents
			{
				nw_SendDisplayMessage(i,kNotEnoughReagents);
				return;
			}

	// --------------- face direction ---------------

	rowDelta=row-creature[player[i].creatureIndex].row;

	colDelta=col-creature[player[i].creatureIndex].col;

	if ((rowDelta<=-1) && (colDelta<=-1)) 				creature[player[i].creatureIndex].facing=0;
	else if ((rowDelta<=-1) && (colDelta==0)) 		creature[player[i].creatureIndex].facing=1;
	else if ((rowDelta<=-1) && (colDelta>=1)) 		creature[player[i].creatureIndex].facing=2;
	else if ((rowDelta==0) && (colDelta<=-1)) 		creature[player[i].creatureIndex].facing=7;
	else if ((rowDelta==0) && (colDelta>=1)) 			creature[player[i].creatureIndex].facing=3;
	else if ((rowDelta>=1) && (colDelta<=-1)) 		creature[player[i].creatureIndex].facing=6;
	else if ((rowDelta>=1) && (colDelta==0)) 			creature[player[i].creatureIndex].facing=5;
	else if ((rowDelta>=1) && (colDelta>=1)) 			creature[player[i].creatureIndex].facing=4;


	strcpy(magicWords,"* ");

	for (j=0;j<5;j++)
		if (itemList[item].ingredientCount[j]>0)
			for (k=1;k<kMaxInventorySize;k++)
				if (creature[player[i].creatureIndex].inventory[k].itemID==itemList[item].ingredientID[j])
				{

					if (creature[player[i].creatureIndex].inventory[k].itemID==kGinseng)
					{
						ginsengSpawn        = ginsengSpawn       + itemList[item].ingredientCount[j];
						strcat(magicWords,"pur ");
					}
					else if (creature[player[i].creatureIndex].inventory[k].itemID==kLotusFlower)
					{
						lotusFlowerSpawn    = lotusFlowerSpawn   + itemList[item].ingredientCount[j];
						strcat(magicWords,"clar ");
					}
					else if (creature[player[i].creatureIndex].inventory[k].itemID==kMandrakeRoot)
					{
						mandrakeRootSpawn   = mandrakeRootSpawn  + itemList[item].ingredientCount[j];
						strcat(magicWords,"shel ");
					}
					else if (creature[player[i].creatureIndex].inventory[k].itemID==kGarlic)
					{
						garlicSpawn         = garlicSpawn        + itemList[item].ingredientCount[j];
						strcat(magicWords,"rel ");
					}
					else if (creature[player[i].creatureIndex].inventory[k].itemID==kVolcanicAsh)
					{
						volcanicAshSpawn    = volcanicAshSpawn   + itemList[item].ingredientCount[j];
						strcat(magicWords,"flas ");
					}
					else if (creature[player[i].creatureIndex].inventory[k].itemID==kPoisonGland)
					{
						strcat(magicWords,"pos ");
					}
					else if (creature[player[i].creatureIndex].inventory[k].itemID==kElectricEel)
					{
						strcat(magicWords,"bol ");
					}

					else if (creature[player[i].creatureIndex].inventory[k].itemID==kAmber)
					{
						strcat(magicWords,"ber ");
					}
					else if (creature[player[i].creatureIndex].inventory[k].itemID==kCoal)
					{
						strcat(magicWords,"bur ");
					}
					else if (creature[player[i].creatureIndex].inventory[k].itemID==kDragonTooth)
					{
						strcat(magicWords,"des ");
					}

					in_RemoveFromInventorySlot(player[i].creatureIndex,k,itemList[item].ingredientCount[j]);

					break;
				}

	strcat(magicWords,"*");

	if (player[i].curse&kBabbleCurse)
		strcpy(magicWords,"* babble babble babble *");

	if (creature[player[i].creatureIndex].mute) //018, muted players can't cast.
	{
		nw_SendTextToClient(i, "You can't cast any spell. You have been muted.", 0);
		return;
	}
	
	if (player[i].morph!=0)
		strcpy(magicWords, "");

	creature[player[i].creatureIndex].talkWhen	= TickCount();

	creature[player[i].creatureIndex].talkTime	= TickCount();
	strcpy(creature[player[i].creatureIndex].talk,magicWords);
	creature[player[i].creatureIndex].talkID		= chatMessageID;
	chatMessageID++;

	nw_SendInventoryRefresh(i);

	loop=sk_GetSkillLevel(player[i].skill[itemList[item].makeSkill].level);  // determine success chance

	for (k=0;k<loop;k++)
	{
		success=success+inc;
		inc=inc/2;
	}

	chance=tb_Round(success);

	if (player[i].curse&kBabbleCurse)
		chance=0;

	if (tb_Rnd(1,100)<chance)
	{
		skillGain=sk_CastSpellOnTerrain(player[i].creatureIndex, row, col, item);
		player[i].sendStatsRefresh=true;

		if (skillGain)
		{
			//sk_TrainProof(i,0, item, itemList[item].makeSkill, itemList[item].makeSkillLevel);  //Lia 12-3-2012
			sk_RaiseSkill(i,0,itemList[item].makeSkill,itemList[item].makeSkillLevel);  //Lia 12-3-2012
		}

		return;
	}

	nw_SendDisplayMessage(i,kSpellFizzles);

}

// ----------------------------------------------------------------- //
TBoolean sk_CraftingSkill(int skill, int i, int item, TBoolean /*raiseStr*/, TBoolean /*raiseInt*/, TBoolean /*raiseDex*/) // server
// ----------------------------------------------------------------- //

{
	float         success=50;
	float         inc=25;
	int           chance;
	int           loop;
	int           j;

	if (sk_GetSkillLevel(player[i].skill[skill].level)<itemList[item].makeSkillLevel) // not enough skill
		return (false);

	loop=sk_GetSkillLevel(player[i].skill[skill].level)-itemList[item].makeSkillLevel;  // determine success chance

	for (j=0;j<loop;j++)
	{
		success=success+inc;
		inc=inc/2;
	}

	chance=tb_Round(success);

	if (tb_Rnd(1,100)<chance)
	{
		sk_RaiseSkill(i,0,skill,itemList[item].makeSkillLevel);
		return (true);
	}

	if (currentCycle==kHarvestMoon && fullMoon)  //Harvest Moon - 1/3 of failures become successes
		if (tb_Rnd(0,2)<1)
		{
			sk_RaiseSkill(i,0,skill,itemList[item].makeSkillLevel);
			return (true);
		}

	return (false);

}

// ----------------------------------------------------------------- //
void sk_IncreaseStrengthCounter(int /*i*/) // server
// ----------------------------------------------------------------- //

{


}

// ----------------------------------------------------------------- //
void sk_IncreaseDexterityCounter(int /*i*/) // server
// ----------------------------------------------------------------- //

{

}

// ----------------------------------------------------------------- //
void sk_IncreaseIntelligenceCounter(int /*i*/) // server
// ----------------------------------------------------------------- //

{

}

// ----------------------------------------------------------------- //
void sk_Meditation(int i) // server
// ----------------------------------------------------------------- //

{

	if (tb_Rnd(1,4)==1)
		sk_RaiseSkill(i,0,kMeditation,1);

}

// ----------------------------------------------------------------- //
void sk_Tracking(int i) // server
// ----------------------------------------------------------------- //

{

	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}


	if (nw_SendTrackingInfosToClient(i)==false)
		return;

	if (cr_DistanceFrom(player[i].creatureIndex, player[i].lastTrackingRow, player[i].lastTrackingCol)>5)
		sk_RaiseSkill(i,0,kTracking,2);

	player[i].lastTrackingRow = creature[player[i].creatureIndex].row;

	player[i].lastTrackingCol = creature[player[i].creatureIndex].col;

}

// ----------------------------------------------------------------- //
void sk_Wrestling(int i, int theEnemy) // server
// ----------------------------------------------------------------- //

{
	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	sk_TrainProof(i, theEnemy, 0, kWrestling, 1);
}

// ----------------------------------------------------------------- //
void sk_PoleFighting(int i, int theEnemy) // server
// ----------------------------------------------------------------- //
{
	sk_TrainProof(i, theEnemy, 0, kMelee, 1);
}

// ----------------------------------------------------------------- //
void sk_Swordsmanship(int i, int theEnemy) // server
// ----------------------------------------------------------------- //
{
	sk_TrainProof(i, theEnemy, 0, kMelee, 1);
}

// ----------------------------------------------------------------- //
void sk_SmallWeapons(int i, int theEnemy) // server
// ----------------------------------------------------------------- //
{
	sk_TrainProof(i, theEnemy, 0, kMelee, 1);
}

// ----------------------------------------------------------------- //
void sk_MaceFighting(int i, int theEnemy) // server
// ----------------------------------------------------------------- //
{
	sk_TrainProof(i, theEnemy, 0, kMelee, 1);
}

// ----------------------------------------------------------------- //
void sk_TrainProof(int i, int theEnemy, int theSpell, int theSkill, int reward)
// ----------------------------------------------------------------- //
{
	int levelDelta;
	int chance=0;
	
	if (theEnemy!=0)
	{
		levelDelta=creatureInfo[creature[theEnemy].id].creatureLevel-sk_GetSkillLevel(player[creature[i].playerIndex].skill[theSkill].level);	
		switch (abs(levelDelta))
		{
			
			case 6:
				chance=25;  //Lia
			break;
				
			case 5:
				chance=30;  //Lia
			break;
				
			case 4:
				chance=35;  //Lia
			break;
				
			case 3:
				chance=35;  //10; //15;  /Lia
			break;
			
			case 2:
				chance=40;  //20; //30;  //Lia
			break;
			
			case 1:
				chance=40;  //30; //45; //Lia
			break;
			
			case 0:
				chance=40; //60;
			break;
			
			default:
				chance=20; //5;  //Lia
			break;
		}
		
		if (levelDelta>0) //Fighting stronger
		{
			chance=20; //10; Lia
			
			if (chance<=0)	
				return;
		}	
		
		//if (abs(levelDelta)>8)	chance=0;
	}
	else //benefical/terrain spell
	{
		levelDelta=itemList[theSpell].makeSkillLevel - sk_GetSkillLevel(player[creature[i].playerIndex].skill[theSkill].level);
		
		chance=100-5*levelDelta;
	}
	
	
	if (tb_Rnd(1,100)<chance)
		sk_RaiseSkill(i,theEnemy,theSkill,reward);
	
}

// ----------------------------------------------------------------- //
void sk_Melee(int i, int theEnemy) //sever
// ----------------------------------------------------------------- //
{
	sk_TrainProof(i, theEnemy, 0, kMelee, 1);
}

// ----------------------------------------------------------------- //
void sk_Parrying(int i, int theEnemy) // server
// ----------------------------------------------------------------- //

{
	sk_TrainProof(i, theEnemy, 0, kParrying, 1);
}

// ----------------------------------------------------------------- //
void sk_Archery(int i, int theEnemy) // server
// ----------------------------------------------------------------- //
{
	sk_TrainProof(i, theEnemy, 0, kArchery, 1);
}

// ----------------------------------------------------------------- //
void sk_Fishing(int i) // server
// ----------------------------------------------------------------- //

{
	int									caught;
	int									theCatch;
	int									rare;
	int                 itemCaught=0;
	int                 chance;
	int                 amount;
	unsigned long				currentTime=TickCount();
	TBoolean			success=false;


	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	//----------------------------------------------------------------------------------
	if ((player[i].lastMove+54000)<currentTime)	// ------ check for move inactivity
		if (tb_Rnd(1,2)==1)
		{
			nw_SendDisplayMessage(i,kNoFishCaught);
			return;
		}

	if ((player[i].lastMove+108000)<currentTime)	// ------ check for move inactivity
		if (tb_Rnd(1,2)==1)
		{
			nw_SendDisplayMessage(i,kNoFishCaught);
			return;
		}

	if ((player[i].lastMove+216000)<currentTime)	// ------ check for move inactivity
	{
		nw_SendDisplayMessage(i,kNoFishCaught);
		return;
	}

	//----------------------------------------------------------------------------------

	if (tb_Rnd(1,10)<=sk_GetSkillLevel(player[i].skill[kFishing].level))
		success=true;

	if (!success && currentCycle==kHarvestMoon && fullMoon)  //Harvest Moon...
		if (tb_Rnd(0,2)<1)
			success=true;

	if (success)
	{

		if (tb_Rnd(1,150)==1)  // slim chance of catching rare item
		{
			rare=it_GetRare(5);

			if (rare>0)
			{
				if (in_CanGroup(rare))
					caught=in_AddtoInventory(player[i].creatureIndex, rare, 1);
				else
					caught=in_AddtoInventory(player[i].creatureIndex, rare, 127);

				if (caught>0)
				{
					nw_SendInventoryAdd(i, rare, caught, kCaughtItem,0);
					itemList[rare].currentCount--;
					sk_RaiseSkill(i,0,kFishing,5);
				}
				else
				{
					nw_SendDisplayMessage(i,kNoFishCaught);
					return;
				}
			}
			else
			{
				nw_SendDisplayMessage(i,kNoFishCaught);
				return;
			}
		}

		//-----------------------

		theCatch=kTrout;

		chance=tb_Rnd(0,sk_GetSkillLevel(player[i].skill[kFishing].level));

		if (chance>10)  chance=10;

		theCatch=kTrout;

		if (chance>5)
		{
			chance=tb_Rnd(1,5);

			if (chance==1)                  theCatch=kTrout;

			if (chance==2)                  theCatch=kElectricEel;

			if (chance==3)                  theCatch=kLobster;
			//Dave
			if (chance==4)                  theCatch=kTuna;
			
			if (chance==5)                  theCatch=kSeaBass;
		}
		else if (chance>2)
		{
			chance=tb_Rnd(1,2);

			if (chance==1)                  theCatch=kTrout;

			if (chance==2)                  theCatch=kElectricEel;
		}

		//if (tb_Rnd(1,5)>1)  // special hot fishing spots
		//  if ((creature[player[i].creatureIndex].row>2194) && (creature[player[i].creatureIndex].row<2205)) // bridge good for eels
		//    if ((creature[player[i].creatureIndex].col>1950) && (creature[player[i].creatureIndex].col<1968))
		//      theCatch=kElectricEel;

		if (theCatch==kTrout)				        sk_RaiseSkill(i,0,kFishing,1);
		else if (theCatch==kElectricEel)		sk_RaiseSkill(i,0,kFishing,2);
		else if (theCatch==kLobster)				sk_RaiseSkill(i,0,kFishing,2);
		//Dave
		else if (theCatch==kTuna)				sk_RaiseSkill(i,0,kFishing,2);
		else if (theCatch==kSeaBass)				sk_RaiseSkill(i,0,kFishing,1);
		
		else 				                        sk_RaiseSkill(i,0,kFishing,1);

		amount=1;

		if (in_ItemEquipedServer(i,kLobsterPole)) theCatch=kLobster;

		if (in_ItemEquipedServer(i,kEelPole))     theCatch=kElectricEel;

		caught=in_AddtoInventory(player[i].creatureIndex, theCatch, amount);

		if (caught>0)
			nw_SendInventoryAdd(i, theCatch, caught, kCaughtItem,0);
		else
			nw_SendDisplayMessage(i,kYourPackFull);

		//-----------------------

		player[i].gathering	=	0;

		player[i].gatherRow	=	0;

		player[i].gatherCol	=	0;

	}
	else
	{
		in_DamageToolInHand(i);
		nw_SendDisplayMessage(i,kNoFishCaught);
	}

}

// ----------------------------------------------------------------- //
void sk_Lockpicking(int i) // server
// ----------------------------------------------------------------- //

{
	int						theContainer;


	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	theContainer=containerMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col];

	if (theContainer==0)
	{
		nw_SendDisplayMessage(i,kNoContainer);
		return;
	}

	if ((containerMagicLock[theContainer]>0) && (containerLock[theContainer]==0))
	{
		nw_SendDisplayMessage(i,kCantPickMagicLock);
		return;
	}

	if (containerLock[theContainer]==0)
	{
		nw_SendDisplayMessage(i,kNotLocked);
		return;
	}

	if ((sk_GetSkillLevel(player[i].skill[kLockpicking].level)+tb_Rnd(0,2))>=containerLock[theContainer])
	{
		sk_RaiseSkill(i,0,kLockpicking,containerLock[theContainer]);
		nw_SendDisplayMessage(i,kContainerUnlocked);
		cr_AddSound(player[i].creatureIndex, kLockPickSnd);
		containerLock[theContainer]=0;
	}
	else
	{
		nw_SendDisplayMessage(i,kLockpickingFailed);
		return;
	}

}

// ----------------------------------------------------------------- //
void sk_RemoveTrap(int i) // server
// ----------------------------------------------------------------- //

{
	int						theContainer;


	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	theContainer=containerMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col];

	if (theContainer==0)
	{
		nw_SendDisplayMessage(i,kNoContainerToDisarm);
		return;
	}

	if ((containerMagicTrap[theContainer]>0) && (containerTrap[theContainer]==0))
	{
		nw_SendDisplayMessage(i,kCantDisarmMagicTrap);
		return;
	}

	if (containerTrap[theContainer]==0)
	{
		nw_SendDisplayMessage(i,kNotTrappeed);
		return;
	}

	if ((sk_GetSkillLevel(player[i].skill[kRemoveTrap].level)+tb_Rnd(0,2))>=containerTrap[theContainer])
	{
		sk_RaiseSkill(i,0,kRemoveTrap,containerTrap[theContainer]);
		nw_SendDisplayMessage(i,kContainerDisarmed);
		containerTrap[theContainer]=0;
		strcpy(containerTrapName[theContainer],"");
	}
	else
	{
		nw_SendDisplayMessage(i,kRemoveTrapFailed);
		return;
	}

}

// ----------------------------------------------------------------- //
void sk_Lumberjacking(int i) // server
// ----------------------------------------------------------------- //

{
	int									harvest;
	int                 amount;
	unsigned long				currentTime=TickCount();
	TBoolean			success=false;

	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	//----------------------------------------------------------------------------------
	if ((player[i].lastMove+54000)<currentTime)	// ------ check for move inactivity
		if (tb_Rnd(1,2)==1)
		{
			nw_SendDisplayMessage(i,kNoLumberHarvest);
			return;
		}

	if ((player[i].lastMove+108000)<currentTime)	// ------ check for move inactivity
		if (tb_Rnd(1,2)==1)
		{
			nw_SendDisplayMessage(i,kNoLumberHarvest);
			return;
		}

	if ((player[i].lastMove+216000)<currentTime)	// ------ check for move inactivity
		if (tb_Rnd(1,2)==1)
		{
			nw_SendDisplayMessage(i,kNoLumberHarvest);
			return;
		}

	//----------------------------------------------------------------------------------

	if (resources[player[i].gatherRow][player[i].gatherCol]>100)
	{
		nw_SendDisplayMessage(i,kNoMoreLogs);
		return;
	}

	if (tb_Rnd(1,10)<=sk_GetSkillLevel(player[i].skill[kLumberjacking].level))
		success=true;

	if (!success && currentCycle==kHarvestMoon && fullMoon)  //Harvest Moon...
		if (tb_Rnd(0,2)<1)
			success=true;

	if (success)
	{
		amount=1;

		if (in_ItemEquipedServer(i,kDwarvenHandAxe))
			amount=2;

		resources[player[i].gatherRow][player[i].gatherCol]=resources[player[i].gatherRow][player[i].gatherCol]+10;

		if (resources[player[i].gatherRow][player[i].gatherCol]>100)
			resources[player[i].gatherRow][player[i].gatherCol]=110;

		if ((tb_Rnd(1,7)==1) || (in_ItemEquipedServer(i,kAmberAxe)))
		{
			harvest=in_AddtoInventory(player[i].creatureIndex, kAmber, amount);

			if (harvest>0)
				nw_SendInventoryAdd(i, kAmber, harvest, kTakeItem,0);
			else
				nw_SendDisplayMessage(i,kYourPackFull);
		}
		else
		{
			harvest=in_AddtoInventory(player[i].creatureIndex, kLog, amount);

			if (harvest>0)
				nw_SendInventoryAdd(i, kLog, harvest, kTakeItem,0);
			else
				nw_SendDisplayMessage(i,kYourPackFull);
		}

		player[i].gathering	=	0;

		player[i].gatherRow	=	0;
		player[i].gatherCol	=	0;

		sk_RaiseSkill(i,0,kLumberjacking,1);
	}
	else
	{
		in_DamageToolInHand(i);
		nw_SendDisplayMessage(i,kNoLumberHarvest);
	}

}

// ----------------------------------------------------------------- //
void sk_Mining(int i) // server
// ----------------------------------------------------------------- //

{
	int									harvest;
	unsigned long				currentTime=TickCount();
	int                 crystal;
	TBoolean			success=false;

	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	//----------------------------------------------------------------------------------
	if ((player[i].lastMove+54000)<currentTime)	// ------ check for move inactivity
		if (tb_Rnd(1,2)==1)
		{
			nw_SendDisplayMessage(i,kNoOreHarvest);
			return;
		}

	if ((player[i].lastMove+108000)<currentTime)	// ------ check for move inactivity
		if (tb_Rnd(1,2)==1)
		{
			nw_SendDisplayMessage(i,kNoOreHarvest);
			return;
		}

	if ((player[i].lastMove+216000)<currentTime)	// ------ check for move inactivity
		if (tb_Rnd(1,2)==1)
		{
			nw_SendDisplayMessage(i,kNoOreHarvest);
			return;
		}

	//----------------------------------------------------------------------------------

	if (resources[player[i].gatherRow][player[i].gatherCol]>100)
	{
		nw_SendDisplayMessage(i,kNoMoreFerrite);
		return;
	}

	if (tb_Rnd(1,10)<=sk_GetSkillLevel(player[i].skill[kMining].level))
		success=true;

	if (!success && currentCycle==kHarvestMoon && fullMoon)  //Harvest Moon...
		if (tb_Rnd(0,2)<1)
			success=true;

	if (success)
	{

		resources[player[i].gatherRow][player[i].gatherCol]=resources[player[i].gatherRow][player[i].gatherCol]+10;

		if (resources[player[i].gatherRow][player[i].gatherCol]>100)
			resources[player[i].gatherRow][player[i].gatherCol]=110;

		if ((tb_Rnd(1,40)==1)  || (in_ItemEquipedServer(i,kCentaurMiningAxe))) // mod 8/4 mining for crystals
		{
			crystal=kBlackCrystal;  //Dave kGreenCrystal

			if (tb_Rnd(1,15)==1)  crystal=kRedCrystal;

			if (tb_Rnd(1,15)==1)  crystal=kBlueCrystal;

			if (tb_Rnd(1,15)==1)  crystal=kPurpleCrystal;

			if (tb_Rnd(1,15)==1)  crystal=kOrangeCrystal;

			if (tb_Rnd(1,15)==1)  crystal=kYellowCrystal;

			if (tb_Rnd(1,15)==1)  crystal=kFireCrystal;

			if (tb_Rnd(1,15)==1)  crystal=kGreenCrystal;

			harvest=in_AddtoInventory(player[i].creatureIndex, crystal, 1);

			if (harvest>0)
				nw_SendInventoryAdd(i, crystal, harvest, kTakeItem,0);
			else
				nw_SendDisplayMessage(i,kYourPackFull);
		}
		else if ((tb_Rnd(1,7)==1) || (in_ItemEquipedServer(i,kCoalAxe)))
		{
			harvest=in_AddtoInventory(player[i].creatureIndex, kCoal, 1);

			if (harvest>0)
				nw_SendInventoryAdd(i, kCoal, harvest, kTakeItem,0);
			else
				nw_SendDisplayMessage(i,kYourPackFull);
		}
		//Dave
		else if ((tb_Rnd(1,40)==1))
		{
			harvest=in_AddtoInventory(player[i].creatureIndex, kBlackFerrite, 1);

			if (harvest>0)
				nw_SendInventoryAdd(i, kBlackFerrite, harvest, kTakeItem,0);
			else
				nw_SendDisplayMessage(i,kYourPackFull);
		}
		//Dave^
		else 
		{
			harvest=in_AddtoInventory(player[i].creatureIndex, kFerriteOre, 1);

			if (harvest>0)
				nw_SendInventoryAdd(i, kFerriteOre, harvest, kTakeItem,0);
			else
				nw_SendDisplayMessage(i,kYourPackFull);
		}
		
	
		

		player[i].gathering	=	0;

		player[i].gatherRow	=	0;
		player[i].gatherCol	=	0;

		sk_RaiseSkill(i,0,kMining,1);
	}
	else
	{
		in_DamageToolInHand(i);
		nw_SendDisplayMessage(i,kNoOreHarvest);
	}

}

// ----------------------------------------------------------------- //
void sk_Hiding(int i) // server
// ----------------------------------------------------------------- //

{
	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	if (creature[player[i].creatureIndex].hiding) // already hidden
		return;

	pl_CheckForActionInterrupted(i, true);

	if (player[i].stealth)
	{
		nw_SendDisplayMessage(i, kHidingFailed);
		return;
	}

	if (player[i].stealthCool != 0 && TickCount()<player[i].stealthCool)
	{
		nw_SendDisplayMessage(i, kHidingFailed);
		return;
	}
	else
		player[i].stealthCool=0;

	if (creature[player[i].creatureIndex].beingAttacked>0)
	{
		if (creature[player[i].creatureIndex].beingAttacked<3)
		{
			nw_SendDisplayMessage(i,kHidingFailed);
			return;
		}

		if (tb_Rnd(1,2)==1)
		{
			nw_SendDisplayMessage(i,kHidingFailed);
			return;
		}
	}

	if (tb_Rnd(1,10)<=sk_GetSkillLevel(player[i].skill[kHiding].level))
	{
		creature[player[i].creatureIndex].hiding=true;
		nw_SendDisplayMessage(i,kYouAreHidden);

		if (cr_DistanceFrom(player[i].creatureIndex, player[i].lastHideRow, player[i].lastHideCol)>5)
			sk_RaiseSkill(i,0,kHiding,2);

		player[i].lastHideRow = creature[player[i].creatureIndex].row;

		player[i].lastHideCol = creature[player[i].creatureIndex].col;
	}
	else
		nw_SendDisplayMessage(i,kHidingFailed);

}

// ----------------------------------------------------------------- //
void sk_Stealth(int i) // server
// ----------------------------------------------------------------- //

{
	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	pl_CheckForActionInterrupted(i, true);

	if (player[i].stealth==true) // already stealthed
	{
		player[i].stealth=false;
		player[i].stealthCool=TickCount()+3*60;
		nw_SendDisplayMessage(i, kLeavingStealth);
		return;
	}

	if (player[i].stealthCool!=0 && TickCount()<player[i].stealthCool)
	{
		nw_SendDisplayMessage(i, kStealthFailed);
		return;
	}
	else
		player[i].stealthCool=0;

	if (creature[player[i].creatureIndex].beingAttacked>0)
	{
		if (creature[player[i].creatureIndex].beingAttacked<3)
		{
			nw_SendDisplayMessage(i,kStealthFailed);
			return;
		}

		if (tb_Rnd(1,2)==1)
		{
			nw_SendDisplayMessage(i,kStealthFailed);
			return;
		}
	}

	if (tb_Rnd(1,10)<=sk_GetSkillLevel(player[i].skill[kStealth].level))
	{
		player[i].stealth=true;
		nw_SendDisplayMessage(i,kEnteringStealth);
		player[i].stealthMoves=0;

		if (cr_DistanceFrom(player[i].creatureIndex, player[i].lastStealthRow, player[i].lastStealthCol)>5)
			sk_RaiseSkill(i,0,kStealth,2);  //Dave  sk_RaiseSkill(i,0,kStealth,2);

		player[i].lastStealthRow = creature[player[i].creatureIndex].row;

		player[i].lastStealthCol = creature[player[i].creatureIndex].col;
	}
	else
		nw_SendDisplayMessage(i,kStealthFailed);

}

// ----------------------------------------------------------------- //
void sk_DetectHidden(int i) // server
// ----------------------------------------------------------------- //

{

	sk_RaiseSkill(i,0,kDetectingHidden,1);

}

// ----------------------------------------------------------------- //
void sk_MagicLore(int /*i*/, int /*w*/) // server
// ----------------------------------------------------------------- //

{

}

// ----------------------------------------------------------------- //
void sk_Anatomy(int i, int c, TBoolean active) // server
// ----------------------------------------------------------------- //

{
	TBoolean            found;
	int                 j;


	//player[i].lastCheckIn		= TickCount();
	
	
	
	if (c>0)
		if (creature[c].playerIndex!=0)
			if (player[creature[c].playerIndex].dead)
			{
				if (active)
					nw_SendDisplayMessage(i,kAnatomyFailed);

				return;
			}

	if (active)
	{
		if (cr_ViewBlocked(creature[player[i].creatureIndex].row, creature[player[i].creatureIndex].col, creature[c].row, creature[c].col))
		{
			nw_SendDisplayMessage(i,kCantSeeTarget);
			return;
		}
	}

	if (tb_Rnd(1,3)==1)
	{

		if (active)
		{
			if ((creature[c].strength+creature[c].strengthOffset)<=20)
				nw_SendDisplayMessage(i,kVeryWeak);
			else if ((creature[c].strength+creature[c].strengthOffset)<=35)
				nw_SendDisplayMessage(i,kWeak);
			else if ((creature[c].strength+creature[c].strengthOffset)<=50)
				nw_SendDisplayMessage(i,kAverageStrength);
			else if ((creature[c].strength+creature[c].strengthOffset)<=75)
				nw_SendDisplayMessage(i,kStrong);
			else if ((creature[c].strength+creature[c].strengthOffset)<=100)
				nw_SendDisplayMessage(i,kVeryStrong);
			else
				nw_SendDisplayMessage(i,kExtremelyStrong);
		}

		// -------------------- anti macroing code -----------------
		for (j=0;j<kMaxMacroTrace;j++)
			if (player[i].anatomyTarget[j]==c)
				return;

		found=false;

		for (j=0;j<kMaxMacroTrace;j++)
			if (player[i].anatomyTarget[j]==0)
			{
				player[i].anatomyTarget[j]=c;
				found=true;
				break;
			}

		if (found==false)
		{
			for (j=1;j<kMaxMacroTrace;j++)
				player[i].anatomyTarget[j-1]=player[i].anatomyTarget[j];

			player[i].anatomyTarget[kMaxMacroTrace-1]=c;
		}

		// ----------------------------------------------------------

		sk_RaiseSkill(i,0,kAnatomy,1);

	}
	else
	{
		if (active)
			nw_SendDisplayMessage(i,kAnatomyFailed);
	}
}

// ----------------------------------------------------------------- //
void sk_AnimalLore(int /*i*/, int /*c*/) // server
// ----------------------------------------------------------------- //

{

}

// ----------------------------------------------------------------- //
void sk_EvaluatingIntelligence(int /*i*/, int /*c*/) // server
// ----------------------------------------------------------------- //

{

}

// ----------------------------------------------------------------- //
void sk_Thievery(int i, int c) // server
// ----------------------------------------------------------------- //

{
	TBoolean						gotLucky=false;
	int									chance;
	int									item;
	int									j;
	int									added;
	int									count;
	int									targetValue;
	int									max;
	int									goldAdded;
	int									gold1;
	int									gold2;
	UInt32							gold;
	TBoolean            found;
	TBoolean            brigand;  // merge 7/29

	if (creature[player[i].creatureIndex].paralyzed!=0)	// check for player paralyzed
	{
		if (creature[player[i].creatureIndex].paralyzed>TickCount())
		{
			nw_SendDisplayMessage(i,kYouAreParalyzed);
			return;
		}
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	if (cr_DistanceBetween(player[i].creatureIndex, c)>1)
	{
		nw_SendDisplayMessage(i,kCreatureTooFarAway);
		return;
	}

	if (player[i].creatureIndex==c) // can't steal from self 0.1.3
	{
		nw_SendDisplayMessage(i,kStealthFailed);
		return;
	}

	if (creature[c].master!=0) // can't steal from pet
	{
		nw_SendDisplayMessage(i,kStealthFailed);
		return;
	}

	if (creature[c].playerIndex!=0) // can't steal from gm
		if ((player[creature[c].playerIndex].category==1) || (player[creature[c].playerIndex].category==2) || (player[creature[c].playerIndex].category==4) || (player[creature[c].playerIndex].category==5)) // merge 7/27
		{
			nw_SendDisplayMessage(creature[c].playerIndex, kSomeoneTryingToSteal);
			nw_SendDisplayMessage(i,kStealthFailed);
			return;
		}

	if (tb_Rnd(1,15)==1)
		gotLucky=true;

	if (in_ItemEquipedServer(i,kRingOfLife))
		if (tb_Rnd(1,3)==1)
			gotLucky=true;

	brigand=false;  // merge 7/29

	if (strcmp("brigand",player[i].playerNameLC)==0)
	{
		brigand=true;  // merge 7/29
		gotLucky=true;
	}

	chance=tb_Rnd(1,75)+(creature[c].intelligence+creature[c].intelligenceOffset);

	if ((chance<sk_GetSkillLevel(player[i].skill[kStealth].level)) || (gotLucky))
	{
		if (tb_Rnd(1,4)==1)	// odds of stealing gold ---------------------------
		{

			if (creature[c].playerIndex==0) // can only steal 1gp from npc 0.1.3
				targetValue=1;
			else
				targetValue=tb_Rnd(1,(sk_GetSkillLevel(player[i].skill[kStealth].level)));

			if (in_ItemEquipedServer(i,kRingOfLife))
				targetValue=targetValue*3;

			if (brigand) // merge 7/29
				targetValue=tb_Rnd(20,100);

			gold=in_GetGoldTotal(creature[c].inventory[0].itemID, creature[c].inventory[0].itemCount);

			if (targetValue>gold)
				targetValue=gold;

			if (targetValue==0)
			{
				nw_SendDisplayMessage(i,kStealthFailed);
				chance=tb_Rnd(1,100)+sk_GetSkillLevel(player[i].skill[kStealth].level);	// chance of being caught

				if (creature[c].playerIndex!=0)
					if (chance<(creature[c].intelligence+creature[c].intelligenceOffset))
						nw_SendDisplayMessage(creature[c].playerIndex, kSomeoneTryingToSteal);

				return;
			}

			player[i].thiefUntil=TickCount()+(kThiefTime*3600);

			//--remove gold from target creature

			if (creature[c].playerIndex!=0)
				nw_SendGoldRemove(creature[c].playerIndex, kNoMessage, targetValue, 0);

			gold=gold-targetValue;

			in_SetGoldTotal(gold, &gold1, &gold2);

			creature[c].inventory[0].itemID=gold1;

			creature[c].inventory[0].itemCount=gold2;

			//--give gold to thief
			goldAdded=in_AddGoldtoInventory(player[i].creatureIndex, targetValue);

			if (goldAdded>0)
			{
				in_SetGoldTotal(goldAdded, &gold1, &gold2);
				nw_SendGoldAdd(i, kStoleItem, 0, gold1, gold2);
			}

			chance=tb_Rnd(1,100)+sk_GetSkillLevel(player[i].skill[kStealth].level);	// chance of being caught

			if (creature[c].playerIndex!=0)
				if (chance<(creature[c].intelligence+creature[c].intelligenceOffset))
					nw_SendDisplayMessage(creature[c].playerIndex, kCatchThief);
		}
		else	// try to steal item ---------------------------
		{
			targetValue=sk_GetSkillLevel(player[i].skill[kStealth].level)*3;

			if (in_ItemEquipedServer(i,kRingOfLife))
				targetValue=targetValue*3;

			if (brigand) // merge 7/29
				targetValue=1000000;

			max=0;

			for (j=1;j<kMaxInventorySize;j++)
				if ((creature[c].inventory[j].itemID!=0) && (creature[c].inventory[j].itemCount!=0))
					if ((itemList[creature[c].inventory[j].itemID].price>max) && (itemList[creature[c].inventory[j].itemID].price<=targetValue))
						max=j;

			if (max==0)
			{
				for (j=0;j<10;j++)
				{
					item=tb_Rnd(1,kMaxInventorySize);

					if ((creature[c].inventory[item].itemID!=0) && (creature[c].inventory[item].itemCount!=0))
						break;
				}
			}
			else
				item=max;

			if ((creature[c].inventory[item].itemID==0) || (creature[c].inventory[item].itemCount==0))
			{
				nw_SendDisplayMessage(i,kStealthFailed);
				chance=tb_Rnd(1,100)+sk_GetSkillLevel(player[i].skill[kStealth].level);	// chance of being caught

				if (creature[c].playerIndex!=0)
					if (chance<(creature[c].intelligence+creature[c].intelligenceOffset))
						nw_SendDisplayMessage(creature[c].playerIndex, kSomeoneTryingToSteal);

				return;
			}

			if (sk_GetSkillLevel(player[i].skill[kStealth].level)<100)
				if (creature[c].playerIndex!=0)
					for (j=1;j<13;j++)
						if (player[creature[c].playerIndex].arm[j]==item)
						{
							nw_SendDisplayMessage(i,kStealthFailed);
							chance=tb_Rnd(1,100)+sk_GetSkillLevel(player[i].skill[kStealth].level);	// chance of being caught

							if (creature[c].playerIndex!=0)
								if (chance<(creature[c].intelligence+creature[c].intelligenceOffset))
									nw_SendDisplayMessage(creature[c].playerIndex, kSomeoneTryingToSteal);

							return;
						}

			if (itemList[creature[c].inventory[item].itemID].blessed)
			{
				nw_SendDisplayMessage(i,kStealthFailed);
				chance=tb_Rnd(1,100)+sk_GetSkillLevel(player[i].skill[kStealth].level);	// chance of being caught

				if (creature[c].playerIndex!=0)
					if (chance<(creature[c].intelligence+creature[c].intelligenceOffset))
						nw_SendDisplayMessage(creature[c].playerIndex, kSomeoneTryingToSteal);

				return;
			}

			// ----- can't steal meat from animal, wool from sheep, logs from reaper, etc -----
			if (itemList[creature[c].inventory[item].itemID].itemType==kRawMeatType)
			{
				nw_SendDisplayMessage(i,kStealthFailed);
				chance=tb_Rnd(1,100)+sk_GetSkillLevel(player[i].skill[kStealth].level);	// chance of being caught

				if (creature[c].playerIndex!=0)
					if (chance<(creature[c].intelligence+creature[c].intelligenceOffset))
						nw_SendDisplayMessage(creature[c].playerIndex, kSomeoneTryingToSteal);

				return;
			}

			if ((creature[c].inventory[item].itemID==kLog) && (creature[c].id==kReaper))
			{
				nw_SendDisplayMessage(i,kStealthFailed);
				chance=tb_Rnd(1,100)+sk_GetSkillLevel(player[i].skill[kStealth].level);	// chance of being caught

				if (creature[c].playerIndex!=0)
					if (chance<(creature[c].intelligence+creature[c].intelligenceOffset))
						nw_SendDisplayMessage(creature[c].playerIndex, kSomeoneTryingToSteal);

				return;
			}

			// --------------------------------------------------------------------------------

			if (in_CanGroup(creature[c].inventory[item].itemID))
				count=1;
			else
				count=creature[c].inventory[item].itemCount;

			added=in_AddtoInventory(player[i].creatureIndex, creature[c].inventory[item].itemID, count);

			if (added>0)
			{
				player[i].thiefUntil=TickCount()+(kThiefTime*3600);
				nw_SendInventoryAdd(i, creature[c].inventory[item].itemID, added, kStoleItem,0); // server
				in_RemoveFromInventorySlot(c, item, added);

				if (creature[c].playerIndex!=0)
				{
					nw_SendToClientDropFromInventorySlot(creature[c].playerIndex, kNoMessage, item, added, 0);

					chance=tb_Rnd(1,100)+sk_GetSkillLevel(player[i].skill[kStealth].level);	// chance of being caught

					if (creature[c].playerIndex!=0)
						if (chance<(creature[c].intelligence+creature[c].intelligenceOffset))
							nw_SendDisplayMessage(creature[c].playerIndex, kCatchThief);
				}
			}
			else
				nw_SendDisplayMessage(i, kYourPackFull);
		}

		// -------------------- anti macroing code -----------------
		for (j=0;j<kMaxMacroTrace;j++)
			if (player[i].thieveryTarget[j]==c)
				return;

		found=false;

		for (j=0;j<kMaxMacroTrace;j++)
			if (player[i].thieveryTarget[j]==0)
			{
				player[i].thieveryTarget[j]=c;
				found=true;
				break;
			}

		if (found==false)
		{
			for (j=1;j<kMaxMacroTrace;j++)
				player[i].thieveryTarget[j-1]=player[i].thieveryTarget[j];

			player[i].thieveryTarget[kMaxMacroTrace-1]=c;
		}

		// ----------------------------------------------------------

		sk_RaiseSkill(i,0,kStealth,1);

	}
	else
	{
		nw_SendDisplayMessage(i,kStealthFailed);
		chance=tb_Rnd(1,100)+sk_GetSkillLevel(player[i].skill[kStealth].level);	// chance of being caught

		if (creature[c].playerIndex!=0)
			if (chance<(creature[c].intelligence+creature[c].intelligenceOffset))
				nw_SendDisplayMessage(creature[c].playerIndex, kSomeoneTryingToSteal);
	}
}

// ----------------------------------------------------------------- //
void sk_IncreaseSkill(int /*i*/, int /*skill*/)
// ----------------------------------------------------------------- //

{

}

// ----------------------------------------------------------------- //
void sk_CalculateDamage(int what, int cc, int ct)
// ----------------------------------------------------------------- //

{
	TBoolean retailiate;
	float damage, resist, bonus, j;
	int currentTime=TickCount();
	TBoolean usingRing=false;

	if (creature[cc].playerIndex!=0)
	{

		if (player[creature[cc].playerIndex].arm[kRightFinger]!=0 && itemList[creature[cc].inventory[player[creature[cc].playerIndex].arm[kRightFinger]].itemID].spell==what)
			usingRing=true;

		if (!usingRing)
			if (player[creature[cc].playerIndex].arm[kLeftFinger] && itemList[creature[cc].inventory[player[creature[cc].playerIndex].arm[kLeftFinger]].itemID].spell==what)
				usingRing=true;
	}

	retailiate=false;

	if (creature[ct].attackTarget==0) // if no current target then automatically retaliate
		retailiate=true;
	else
		if (creature[ct].playerIndex==0)
			if (cr_DistanceBetween(ct,creature[ct].attackTarget)>creatureInfo[creature[ct].id].attackRange)
				retailiate=true;

	if (cc==ct)
		retailiate=false;
	
	if (creature[ct].playerIndex!=0)	retailiate=false;

	if (retailiate)
	{
		if (pl_IsThief(ct))
		{
			if (!((creature[cc].row>2075) && (creature[cc].row<2310) && (creature[cc].col>1749) && (creature[cc].col<2001)))
			{
				creature[ct].attackTarget=cc;
				creature[ct].nextAttackTime	= currentTime;
			}
		}
		else
		{
			creature[ct].attackTarget=cc;
			creature[ct].nextAttackTime	= currentTime;
		}
	}

	if (creature[ct].playerIndex!=0)
		sk_DamageMagicRobe(creature[ct].playerIndex);

	damage=bonus=resist=0;

	//-- get magic bonus -------------------------------------------------------------

	bonus = 1.0;

	if (creature[cc].playerIndex!=0)
	{
		if (itemList[creature[cc].inventory[player[creature[cc].playerIndex].arm[kHead]].itemID].itemType==kWizardHatType)
			bonus = bonus + 0.15;
	}
	else
	{
		//for (j=1;j<creature[cc].level;j++)
			//bonus = bonus + 0.05;
			
		bonus = 1+ creatureInfo[creature[cc].id].mageryLevel*0.05;//
	}

	if (usingRing) bonus=1.65;
	
	if (creature[cc].godMode)
		bonus=5;

	//-- get magic resistance -------------------------------------------------------------

	
	if (creature[ct].npcIndex!=0)
		resist=0.39;  //JS bourrin 
		
	//Dave npc RM
	/*if (creature[ct].npcIndex==1)
		resist=0.5; 
	if (creature[ct].npcIndex==2)
		resist=0.5; 
	if (creature[ct].npcIndex==3)
		resist=0.5;  
	if (creature[ct].npcIndex==4)
		resist=0.4;
	if (creature[ct].npcIndex==5)
		resist=0.4;
	if (creature[ct].npcIndex==6)
		resist=0.2;
	*/
	else if (creature[ct].playerIndex!=0)
		resist=sk_GetMagicResistance(creature[ct].playerIndex);
	else
	{
		resist=creatureInfo[creature[ct].id].magicResist;

		for (j=1;j<(creatureInfo[creature[ct].id].creatureLevel - creature[ct].level)+1;j++)
			resist=resist*0.9;
	}

	//--------------------------------------------------------------------------------------

	if (what==kFireballSpell)
	{
		if (creature[cc].playerIndex!=0)
		{
			if (player[creature[cc].playerIndex].god)
			{
				damage=1000;
				resist=1;
			}
			else
				damage=tb_Rnd(35,50)+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)*2);
		}
		else
			damage=tb_Rnd(35,58)+creatureInfo[creature[cc].id].mageryLevel*2; //Dave tweak 04-24-2012  damage=tb_Rnd(35,50)

		if (usingRing)
			damage=tb_Rnd(35,58)+20;   //Dave tweak 04-24-2012  damage=tb_Rnd(35,50)+20

		damage=damage*bonus;

		damage=damage*resist;
		
		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);

		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;

			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			else
				tx_CreatureTalkText(cc,ct,kAttackingTalk);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
				sk_ResistingSpells(cc,creature[ct].playerIndex);
			}
		}
	}
	else if (what==kMagicMissileSpell)
	{
		if (creature[cc].playerIndex!=0)
		{
			if (player[creature[cc].playerIndex].god)
			{
				damage=1000;
				resist=1;
			}
			else
				damage=tb_Rnd(1,3)+(sk_GetSkillLevel(player[creature[cc].playerIndex].skill[kMageryWizard].level)/2);
		}
		else
			damage=tb_Rnd(1,3)+creatureInfo[creature[cc].id].mageryLevel/2;

		if (usingRing)
			damage=tb_Rnd(1,3)+10;

		damage=damage*bonus;

		damage=damage*resist;
		
		if (creature[ct].damageReduc<1.0)
			damage=tb_Round((float)damage*creature[ct].damageReduc);

		if (damage>0)
		{
			if (damage>creature[ct].hitPoints)
				damage=creature[ct].hitPoints;

			creature[ct].hitPoints=creature[ct].hitPoints-damage;

			if (creature[ct].hitPoints==0)
			{
				cr_setDieTime(ct,(currentTime+120));

				if (creature[ct].godMode==false && creature[ct].noDeath==false)
					tx_CreatureTalkText(cc,ct,kKillingTalk);
			}
			else
				tx_CreatureTalkText(cc,ct,kAttackingTalk);

			if (creature[ct].playerIndex!=0)
			{
				player[creature[ct].playerIndex].sendStatsRefresh=true;
				sk_ResistingSpells(cc,creature[ct].playerIndex);
			}
		}
	}
}

#endif
