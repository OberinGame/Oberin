#include <OpenTransport.h>

#include "Toolbox.h"
#include "Globals.h"
#include "Common.h"
#include "Players.h"
#include "Network.h"
#include "FileIO.h"
#include "Graphics.h"
#include "Inventory.h"
#include "Creatures.h"
#include "Skills.h"
#include "Resource.h"
#include "Items.h"
#include "FX.h"
#include "GameText.h"
#include "Shops.h"
#include "Npc.h"

//-----------------------------------------------

extern TBoolean							doorway[kMaxIcons];
extern TBoolean							sign[kMaxIcons];

//-----------------------------------------------

#ifdef _SERVER
playerType							player						[kMaxPlayers];
int									playerState				[kMaxPlayers][2][kMaxScreenElements];
char								ban								[1000][18];

gmJumpType							gmJump						[kMaxGMJumps];
#endif

playerLocalSkillsType				localSkills				[kMaxSkills];

UInt8								localStrength;
UInt8								localIntelligence;
UInt8								localDexterity;
UInt8								localArmorRating;
UInt8								localHitPoints;
UInt8								localMagicPoints;
UInt8								localStamina;
UInt16								localWeight;
UInt8								localLevel;
UInt8								localClass;
UInt16								localHunger;

UInt8								localRightHand;
UInt8								localLeftHand;
UInt8								localLegs;
UInt8								localTorso;
UInt8								localFeet;
UInt8								localArms;
UInt8								localRightFinger;
UInt8								localLeftFinger;
UInt8								localHead;
UInt8								localNeck;
UInt8								localHands;
UInt8								localWaist;
UInt8								localMount;
int									localSpat;
extern int							localTarget;
extern UInt8						creatureFacing[kMaxScreenElements];

#ifdef _SERVER
extern UInt16						creatureMap				[kMapWidth][kMapHeight];
extern creatureType					creature				[kMaxCreatures];
extern UInt8						playerData				[kMaxPlayers][kDataSize];
#endif

#ifdef _SERVER
extern npcType						npc						[kMaxNPCs];
extern guildHallType		guildHalls[kMaxGuildHalls];
extern int					guildMap[kMapWidth][kMapHeight];
#endif

extern creatureControls				creatureInfo			[kMaxCreatureTypes];
extern TBoolean						canWalkOn				[kMaxIcons];
extern TBoolean						canFlyOver				[kMaxIcons];
extern TBoolean						canSwimIn				[kMaxIcons];
extern UInt16						map						[kMapWidth][kMapHeight];
extern int							targetMode;
extern unsigned long        		chatMessageID;
extern unsigned long       			soundMessageID;
extern TBoolean           		 	gmProp					[kMaxContainerTypes];
extern spawnRegionType				spawnRegion				[kMaxSpawnRegions];
extern UInt16						tracksMap				[kMapWidth][kMapHeight];
extern char							tracksNote				[kMaxTracks][100];


extern TBoolean						wasStandingInShop;
extern TBoolean						wasStandingInBank;
extern TBoolean						wasStandingOnActionSpot;
extern TBoolean						sign					[kMaxIcons];

//-----------------------------------------------

#ifdef _SERVER

// ----------------------------------------------------------------- //
TBoolean pl_AuthorizedTo(int c, int command)	// put in carbon 11/27
// ----------------------------------------------------------------- //

{
	int             p;

	if (command==kCanBeRevealed)
		return (true);

	p=creature[c].playerIndex;

	if (p==0)
		return (false);

	if (command==kRegularPlayer)
		if (player[p].category==0  || player[p].category==8)
			return (true);

	if (command==kCanLosePets)
		if (player[p].category==0 || player[p].category==8)  //02/14/2005 - New category : 8 = /makeevil PCs
			return (true);

	if (command==kCanBeRevealed)
		if (player[p].category==0)
			return (true);

	if (command==kAttackPets)
		if ((player[p].category==1) || (player[p].category==4) || (player[p].category==5) || (player[p].category==8))
			return (true);

	if (command==kAttackPlayers)
		if ((player[p].category==1) || (player[p].category==4) || (player[p].category==5) || (player[p].category==8))
			return (true);

	if (command==kPetCanAttackPlayers)
		if ((player[p].category==1) || (player[p].category==4) || (player[p].category==5) || (player[p].category==8))
			return (true);

	if (command==kExtraResistance)
		if (player[p].category!=0 && player[p].category!=8 && player[p].category!=4 && player[p].category!=5)
			return (true);

	if (command==kExtraDamage)
		if (player[p].category!=0 && player[p].category!=8 && player[p].category!=4 && player[p].category!=5)
			return (true);

	if (command==kCanBan)
		if (player[p].category==1)
			return (true);

	if (command==kCanAKA)
		if (player[p].category==1 || player[p].category==7 || player[p].category==4 || player[p].category==5)
			return (true);

	if (command==kIsApprenticeGM)
		if (player[p].category==6)
			return (true);

	if (command==kIsRegularGM)
		if (player[p].category==2)
			return (true);

	if (command==kIsSeniorGM)
		if (player[p].category==7)
			return (true);

	if (command==kIsDeveloper)
	{
		if (player[p].category==1)
			return (true);
	}

	if (command==kIsAnyGM)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanSeeDemoNames)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kIsQuestCharacter)
		if ((player[p].category==4) || (player[p].category==5))
			return (true);

	if (command==kCanSeeQuestCharacters)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanBroadcast)
		if ((player[p].category==1) || (player[p].category==7) || (player[p].category==4) || (player[p].category==5))
			return (true);

	if (command==kCanGetIP)
		if ((player[p].category==1) || (player[p].category==7))
			return (true);

	if (command==kSetLogonMessage)
		if ((player[p].category==1) || (player[p].category==7))
			return (true);

	if (command==kCanUseGMChannel)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanUseDeveloperChannel)
		if (player[p].category==1)
			return (true);

	if (command==kMeetPlayers)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanSeeMacroSuspects)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kIsQuestScoundrel)
		if (player[p].category==5 || player[p].category==8)
			return (true);

	if (command==kCanSummonPlayers)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanSetGuildTags)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanSetLogonMessage)
		if ((player[p].category==1) || (player[p].category==7))
			return (true);

	if (command==kCanSetSigns)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanSetHalls)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanGrantExperience)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanSetOwner)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanGoto)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanSpawnCreatures)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanMorph)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanSetOwner)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanMakeContainer)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanSpawnItems)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanTalkToGhosts)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kPermanentNightVision)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanGetPosition)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanControlCharacterCreator)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanSeeHelpRequestsWithFeedback)
		if ((player[p].category==2) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanSeeHelpRequestsWithNoFeedback)
		if ((player[p].category==1) || (player[p].category==4) || (player[p].category==5))
			return (true);

	if (command==kCanBanishCreature)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanPitPlayers)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanJailPlayers)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanKickPlayers)
		if (player[p].category==1)
			return (true);

	if (command==kCanControlSkillGain)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanResPlayers)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==7))
			return (true);

	if (command==kCanChangeNames)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanResetPlayers)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanSendHome)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanActivate)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kNotifiedOfNewPlayers)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanMute)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanMsay)
		if ((player[p].category==1) || (player[p].category==7) || (player[p].category==4) || (player[p].category==5))
			return (true);

	if (command==kCanWhisper)
		if ((player[p].category==1) || (player[p].category==2) || (player[p].category==4) || (player[p].category==5) || (player[p].category==6) || (player[p].category==7))
			return (true);

	if (command==kCanMuteAll)
		if ((player[p].category==1) || (player[p].category==7))
			return (true);

	if (command==kCanKillPlayers)
		if ((player[p].category==1) || (player[p].category==7) || (player[p].category==4) || (player[p].category==5))
			return (true);


	return (false);
}

// ----------------------------------------------------------------- //
TBoolean pl_OnTeleporter(int /*pRow*/, int /*pCol*/, int /*row*/, int /*col*/)	// server mod 8/4 new function
// ----------------------------------------------------------------- //

{

	//if ((pRow>=(row-2)) && (pRow<=(row+2)) && (pCol>=(col-2)) && (pCol<=(col+2)))
	//  return (true);

	return (false);
}

// ----------------------------------------------------------------- //
void pl_HairDying(int i, int color)	// server
// ----------------------------------------------------------------- //

{
	player[i].hairColor=color;
}

// ----------------------------------------------------------------- //
void pl_SetSpeed(int i, UInt8 speed)	// server
// ----------------------------------------------------------------- //

{
	player[i].speed=speed;
	
	if (player[i].speed>15) //paranoid error checking
		player[i].speed=15;
	
	nw_SendSpeedRefresh(i);
}

// ----------------------------------------------------------------- //
TBoolean pl_Teleporting(int i, int crystal)	// server mod 8/4 new function
// ----------------------------------------------------------------- //

{
	int           row;
	int           col;
	TBoolean      moved;
	int           j;
	UInt16        prow;
	UInt16        pcol;
	int           k;


	moved=false;
	row=0;
	col=0;

	for (j=0;j<kMaxPortals;j++)
		if (portalItem[j]==0)
			return (false);
		else
		{
			if (portalItem[j]==crystal)
				if ((creature[player[i].creatureIndex].row>=portalRect[j].left) && (creature[player[i].creatureIndex].row<=portalRect[j].right))
					if ((creature[player[i].creatureIndex].col>=portalRect[j].top) && (creature[player[i].creatureIndex].col<=portalRect[j].bottom))
					{
						moved=true;
						row=portalRow[j];
						col=portalCol[j];
						break;
					}
		}

	if (moved)
	{

		if ((row!=0) && (col!=0))
		{
			for (k=0;k<kMaxCreatureEffects;k++)
				if (creature[player[i].creatureIndex].fx[k]>0)
				{
					serverFX[RC(creature[player[i].creatureIndex].fx[k],kMaxFX)]	=	0;
					creature[player[i].creatureIndex].fx[k]						=	0;
					creature[player[i].creatureIndex].fxKillTime[k]		=	0;
					creature[player[i].creatureIndex].fxTarget[k]			=	0;
					creature[player[i].creatureIndex].fxTargetRow[k]	=	0;
					creature[player[i].creatureIndex].fxTargetCol[k]	=	0;
					creature[player[i].creatureIndex].activeFX				= false;
				}

			for (j=0;j<kMaxPets;j++)
				if (player[i].pet[j]!=0)
					if (player[i].petInStable[j]==false)
					{
						prow=row+tb_Rnd(0,4)-2;
						pcol=col+tb_Rnd(0,4)-2;
						cr_GetNearestFreeLocation(player[i].pet[j], &prow, &pcol,"pl_Teleporting");

						if ((prow!=0) && (pcol!=0))
						{
							creatureMap[creature[player[i].pet[j]].row][creature[player[i].pet[j]].col]	= 0;
							creature[player[i].pet[j]].row					=	prow;
							creature[player[i].pet[j]].col					=	pcol;
							creatureMap[prow][pcol]									= player[i].pet[j];
							creature[player[i].pet[j]].nextMoveTime	=	TickCount()+creatureInfo[creature[player[i].pet[j]].id].speed;
							creature[player[i].pet[j]].timeStamp		= TickCount();
							creature[player[i].pet[j]].attackTarget	=	0;
							creature[player[i].pet[j]].trackRow			=	0;
							creature[player[i].pet[j]].trackCol			=	0;
							creature[player[i].pet[j]].tracking			=	false;

							for (k=0;k<kMaxCreatureEffects;k++)
								if (creature[player[i].pet[j]].fx[k]>0)
								{
									serverFX[RC(creature[player[i].pet[j]].fx[k],kMaxFX)]	=	0;
									creature[player[i].pet[j]].fx[k]						=	0;
									creature[player[i].pet[j]].fxKillTime[k]		=	0;
									creature[player[i].pet[j]].fxTarget[k]			=	0;
									creature[player[i].pet[j]].fxTargetRow[k]	  =	0;
									creature[player[i].pet[j]].fxTargetCol[k]	  =	0;
									creature[player[i].pet[j]].activeFX				  = false;
								}

							if (creature[player[i].pet[j]].hiding==false)
								fx_CreatureServerFX(player[i].pet[j], player[i].pet[j], kSparkle, (5*60), 0);
						}
					}

			pl_PlayerMoveTo(i,row,col);

			if (player[i].stealth==false)
				if (creature[player[i].creatureIndex].hiding==false)
					fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (2*60), 0);

			return (true);
		}

	}

	return (false);

}

// ----------------------------------------------------------------- //
void pl_GetPetPosition(int i, int p, UInt16 *row, UInt16 *col)	// server
// ----------------------------------------------------------------- //

{
	UInt16					r=0;
	UInt16					c=0;
	int							j;
	int							petPosition=0;
	int							petCount=0;


	if ((player[i].rowTrail[2]==0) || (player[i].colTrail[2]==0))
	{
		r=creature[player[i].creatureIndex].row;
		c=creature[player[i].creatureIndex].col;
	}
	else
	{
		r=player[i].rowTrail[2];
		c=player[i].colTrail[2];
	}

	cr_GetNearestFreeLocation(p, &r, &c,"pl_GetPetPosition");

	*row=r;
	*col=c;

	return;

	for (j=0;j<kMaxPets;j++)
		if (player[i].pet[j]!=0)
			if (player[i].petInStable[j]==false)
			{
				petCount++;

				if (player[i].pet[j]==p)
					petPosition++;
			}

	if (petCount==1)
		petPosition=2;

	if (petCount==2)
	{
		if (petPosition==1)
			petPosition=1;
		else
			petPosition=3;
	}

	if (petPosition==1)
	{
		if (creature[player[i].creatureIndex].moveDirection==8)
		{
			r=creature[player[i].creatureIndex].row-1;
			c=creature[player[i].creatureIndex].col+1;
		}

		if (creature[player[i].creatureIndex].moveDirection==5)
		{
			r=creature[player[i].creatureIndex].row+1;
			c=creature[player[i].creatureIndex].col-2;
		}

		if (creature[player[i].creatureIndex].moveDirection==2)
		{
			r=creature[player[i].creatureIndex].row+3;
			c=creature[player[i].creatureIndex].col-1;
		}

		if (creature[player[i].creatureIndex].moveDirection==1)
		{
			r=creature[player[i].creatureIndex].row+2;
			c=creature[player[i].creatureIndex].col+1;
		}

		if ((creature[player[i].creatureIndex].moveDirection==0) || (creature[player[i].creatureIndex].moveDirection==4))
		{
			r=creature[player[i].creatureIndex].row+1;
			c=creature[player[i].creatureIndex].col+3;
		}

		if (creature[player[i].creatureIndex].moveDirection==3)
		{
			r=creature[player[i].creatureIndex].row-1;
			c=creature[player[i].creatureIndex].col+2;
		}

		if (creature[player[i].creatureIndex].moveDirection==6)
		{
			r=creature[player[i].creatureIndex].row-3;
			c=creature[player[i].creatureIndex].col+1;
		}

		if (creature[player[i].creatureIndex].moveDirection==7)
		{
			r=creature[player[i].creatureIndex].row-3;
			c=creature[player[i].creatureIndex].col-1;
		}
	}
	else if (petPosition==2)
	{
		if (creature[player[i].creatureIndex].moveDirection==8)
		{
			r=creature[player[i].creatureIndex].row-2;
			c=creature[player[i].creatureIndex].col-1;
		}

		if (creature[player[i].creatureIndex].moveDirection==5)
		{
			r=creature[player[i].creatureIndex].row+0;
			c=creature[player[i].creatureIndex].col-2;
		}

		if (creature[player[i].creatureIndex].moveDirection==2)
		{
			r=creature[player[i].creatureIndex].row+2;
			c=creature[player[i].creatureIndex].col-2;
		}

		if (creature[player[i].creatureIndex].moveDirection==1)
		{
			r=creature[player[i].creatureIndex].row+2;
			c=creature[player[i].creatureIndex].col+0;
		}

		if ((creature[player[i].creatureIndex].moveDirection==0) || (creature[player[i].creatureIndex].moveDirection==4))
		{
			r=creature[player[i].creatureIndex].row+2;
			c=creature[player[i].creatureIndex].col+2;
		}

		if (creature[player[i].creatureIndex].moveDirection==3)
		{
			r=creature[player[i].creatureIndex].row;
			c=creature[player[i].creatureIndex].col+2;
		}

		if (creature[player[i].creatureIndex].moveDirection==6)
		{
			r=creature[player[i].creatureIndex].row-2;
			c=creature[player[i].creatureIndex].col+2;
		}

		if (creature[player[i].creatureIndex].moveDirection==7)
		{
			r=creature[player[i].creatureIndex].row-3;
			c=creature[player[i].creatureIndex].col;
		}
	}
	else if (petPosition==3)
	{
		if (creature[player[i].creatureIndex].moveDirection==8)
		{
			r=creature[player[i].creatureIndex].row-3;
			c=creature[player[i].creatureIndex].col-1;
		}

		if (creature[player[i].creatureIndex].moveDirection==5)
		{
			r=creature[player[i].creatureIndex].row-1;
			c=creature[player[i].creatureIndex].col-2;
		}

		if (creature[player[i].creatureIndex].moveDirection==2)
		{
			r=creature[player[i].creatureIndex].row+1;
			c=creature[player[i].creatureIndex].col-3;
		}

		if (creature[player[i].creatureIndex].moveDirection==1)
		{
			r=creature[player[i].creatureIndex].row+2;
			c=creature[player[i].creatureIndex].col-1;
		}

		if ((creature[player[i].creatureIndex].moveDirection==0) || (creature[player[i].creatureIndex].moveDirection==4))
		{
			r=creature[player[i].creatureIndex].row+3;
			c=creature[player[i].creatureIndex].col+1;
		}

		if (creature[player[i].creatureIndex].moveDirection==3)
		{
			r=creature[player[i].creatureIndex].row+1;
			c=creature[player[i].creatureIndex].col+2;
		}

		if (creature[player[i].creatureIndex].moveDirection==6)
		{
			r=creature[player[i].creatureIndex].row-1;
			c=creature[player[i].creatureIndex].col+3;
		}

		if (creature[player[i].creatureIndex].moveDirection==7)
		{
			r=creature[player[i].creatureIndex].row-3;
			c=creature[player[i].creatureIndex].col+1;
		}
	}

	if (creatureMap[r][c]==p)
	{
		*row=0;
		*col=0;
	}
	else
	{
		cr_GetNearestFreeLocation(p, &r, &c,"pl_GetPetPosition 2");
		*row=r;
		*col=c;
	}

}

#endif

// ----------------------------------------------------------------- //
TBoolean pl_IsPetCommand(char	s[kStringLength])
// ----------------------------------------------------------------- //

{
	if (strcmp(s,"claim")==0)   return true;

	if (strcmp(s,"stable")==0)  return true;

	if (strcmp(s,"attack!")==0)  return true;

	if (strcmp(s,"heel!")==0)    return true;

	if (strcmp(s,"stay!")==0)    return true;

	if (strcmp(s,"move!")==0)    return true;

	if (strcmp(s,"speak!")==0)   return true;

	if (strcmp(s,"guard!")==0)   return true;

	if (strcmp(s,"release!")==0)    return true;

	return (false);

}

#ifdef _SERVER
// ----------------------------------------------------------------- //
int pl_GetCharacterLevel(int p)	// server
// ----------------------------------------------------------------- //

{
	UInt32			start=100;
	UInt32      points=0;
	int					level=1;
	int					i;
	//float       totalStatPoints;

	if (player[p].characterClass==1) // cleric
	{
		points=points+player[p].skill[kAlchemyCleric].level;
		points=points+player[p].skill[kHealing].level;
		points=points+player[p].skill[kMageryCleric].level;
		points=points/3;
	}
	else if (player[p].characterClass==2) // druid
	{
		points=points+player[p].skill[kAlchemyDruid].level;
		points=points+player[p].skill[kMageryDruid].level;
		points=points+player[p].skill[kTaming].level;
		points=points/3;
	}
	else if (player[p].characterClass==3) // fighter
	{
		points=points+player[p].skill[kMelee].level;
		points=points+player[p].skill[kSpecialFighter].level;
		points=points+player[p].skill[kParrying].level;
		points=points/3;
	}
	else if (player[p].characterClass==4) // ranger
	{
		points=points+player[p].skill[kArchery].level;
		points=points+player[p].skill[kTracking].level;
		points=points+player[p].skill[kSpecialRanger].level;
		points=points/3;
	}
	else if (player[p].characterClass==5) // rogue
	{
		points=points+player[p].skill[kAnatomy].level;
		points=points+player[p].skill[kSpecialRogue].level;
		points=points+player[p].skill[kStealth].level;
		points=points/3;
	}
	else if (player[p].characterClass==6) // wizard
	{
		points=points+player[p].skill[kEnchanting].level;
		points=points+player[p].skill[kMageryWizard].level;
		points=points+player[p].skill[kMeditation].level;
		points=points/3;
	}
	else  // na
	{
		return (0);
	}

	//---------- determine level ----------
	for (i=0;i<25;i++)
		if (points>start)
		{
			level++;

			if (level>=200)
				break;

			start=start*2;
		}

	if (level>200)
		level=200;

	return (level);

}

// ----------------------------------------------------------------- //
void pl_CursePlayer(int p)	// server
// ----------------------------------------------------------------- //

{
	int						c;

	c=tb_Rnd(1,2);

	if (c==1) // babble curse
	{
		if (player[p].curse&kBabbleCurse) return; // already cursed

		player[p].curse=player[p].curse|kBabbleCurse;

		nw_SendDisplayMessage(p,kYouHaveBeenCursed);
		
	}

	if (c==2) // stench curse
	{
		if (player[p].curse&kStenchCurse) return; // already cursed

		player[p].curse=player[p].curse|kStenchCurse;

		nw_SendDisplayMessage(p,kYouHaveBeenCursed);
		//Dave 12-15-2012
		fx_CreatureServerFX(player[p].creatureIndex, player[p].creatureIndex, kSparkle2, (15*60), 0);
	}


}

// ----------------------------------------------------------------- //
void pl_CalculatePetLoyalty(int p)	// server
// ----------------------------------------------------------------- //

{
	int			    loyalty;
	int         i;

	//if (player[creature[creature[p].master].playerIndex].category==0 || player[creature[creature[p].master].playerIndex].category==8)

	for (i=0;i<kMaxPets;i++)
		if (player[p].pet[i]!=0)
		{
			if (player[p].category!=0 && player[p].category!=8)
				creature[player[p].pet[i]].hunger=5000;

			if (creature[player[p].pet[i]].hunger>4500)        loyalty=7;  // extremely loyal
			else if (creature[player[p].pet[i]].hunger>3750)   loyalty=6;  // very loyal
			else if (creature[player[p].pet[i]].hunger>3000)   loyalty=5;  // loyal
			else if (creature[player[p].pet[i]].hunger>2250)   loyalty=4;  // average
			else if (creature[player[p].pet[i]].hunger>1500)   loyalty=3;  // poor
			else if (creature[player[p].pet[i]].hunger>750)    loyalty=2;  // very poor
			else loyalty=1;                                                // extrememly poor

			if (player[p].petLoyalty[i]!=loyalty)
			{
				player[p].petLoyalty[i]=loyalty;
				player[p].sendPetsRefresh=true;
			}
		}

}

// ----------------------------------------------------------------- //
void pl_CalculateStats(int p)	// server
// ----------------------------------------------------------------- //

{
	UInt32			start=100;
	UInt32      points=0;
	int					level=1;
	int					i;
	float       totalStatPoints;
	float       strength;
	float       dexterity;
	float       intelligence;
	//char		message[kStringLength];
	//char 		levelStr[kStringLength];

	level=pl_GetCharacterLevel(p);
	
	/*if (level!=localLevel) //Level up
	{
		strcpy(message, "Congratulations, you reached level ");
		tb_IntegerToString(level, levelStr);
		strcat(message, levelStr);
		strcat(message, " !");
		gr_AddText(message, true, true);
	}
	//Dave
	if (level!=localLevel) //Level up
		{
		cr_AddSound(player[i].creatureIndex, kLevelUpSnd);
		}*/
	
	totalStatPoints=100;

	for (i=1;i<level;i++)
		totalStatPoints=totalStatPoints+13;

	if (totalStatPoints>500)
		totalStatPoints=500;

	if (player[p].characterClass==1) // cleric
	{
		strength      = tb_Round(totalStatPoints*0.20);
		dexterity     = tb_Round(totalStatPoints*0.40);
		intelligence  = tb_Round(totalStatPoints*0.40);
	}
	else if (player[p].characterClass==2) // druid
	{
		strength      = tb_Round(totalStatPoints*0.35);
		dexterity     = tb_Round(totalStatPoints*0.25);
		intelligence  = tb_Round(totalStatPoints*0.40);
	}
	else if (player[p].characterClass==3) // fighter
	{
		strength      = tb_Round(totalStatPoints*0.40);
		dexterity     = tb_Round(totalStatPoints*0.40);
		intelligence  = tb_Round(totalStatPoints*0.20);
	}
	else if (player[p].characterClass==4) // ranger
	{
		strength      = tb_Round(totalStatPoints*0.40);
		dexterity     = tb_Round(totalStatPoints*0.30);
		intelligence  = tb_Round(totalStatPoints*0.30);
	}
	else if (player[p].characterClass==5) // rogue
	{
		strength      = tb_Round(totalStatPoints*0.30);
		dexterity     = tb_Round(totalStatPoints*0.35);
		intelligence  = tb_Round(totalStatPoints*0.35);
	}
	else if (player[p].characterClass==6) // wizard
	{
		strength      = tb_Round(totalStatPoints*0.30);
		dexterity     = tb_Round(totalStatPoints*0.30);
		intelligence  = tb_Round(totalStatPoints*0.40);
	}
	else // gm
	{
		strength      = 150;
		dexterity     = 150;
		intelligence  = 150;
	}

	if (strength>150)     strength      = 150;

	if (dexterity>150)    dexterity     = 150;

	if (intelligence>150) intelligence  = 150;

	if (strength<0)       strength      = 10;

	if (dexterity<0)      dexterity     = 10;

	if (intelligence<0)   intelligence  = 10;

	creature[player[p].creatureIndex].strength      = (UInt8)strength;

	creature[player[p].creatureIndex].dexterity     = (UInt8)dexterity;

	creature[player[p].creatureIndex].intelligence  = (UInt8)intelligence;

}

// ----------------------------------------------------------------- //
void pl_BanishCreature(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];
	char              creatureStr[kStringLength];
	int               j;
	int               SendRefresh;
	int               master=0;


	if (i>0)  // merge 7/27
		// if (!((player[i].category==1) || (player[i].category==2) || (player[i].category==4) || (player[i].category==5))) // merge 7/27
		if (pl_AuthorizedTo(player[i].creatureIndex,kCanBanishCreature)==false) // add to carbon 11/27
			return;

	if (creature[p].playerIndex!=0)
		return;

	if (creature[p].npcIndex!=0)
	{
		npc[creature[p].npcIndex].startRow								= 0;
		npc[creature[p].npcIndex].startCol								= 0;
		npc[creature[p].npcIndex].torso									= 0;
		npc[creature[p].npcIndex].legs										= 0;
		npc[creature[p].npcIndex].range									= 0;
		strcpy(npc[creature[p].npcIndex].talk, "");
		npc[creature[p].npcIndex].talkTime								=	0;

		for (j=0;j<kMaxNPCQuestions;j++)
			if (npcQuestion[j].id==creature[p].npcIndex)
			{
				npcQuestion[j].id=0;
				strcpy(npcQuestion[j].question, "");
				strcpy(npcQuestion[j].answer, "");
			}

		for (j=0;j<kMaxNPCTrades;j++)
			if (npcTrade[j].id==creature[p].npcIndex)
			{
				npcTrade[j].id							= 0;
				npcTrade[j].giveItem					= 0;
				npcTrade[j].giveCount					= 0;
				npcTrade[j].takeItem					= 0;
				npcTrade[j].takeCount					= 0;
				npcTrade[j].experience				= 0;
				strcpy(npcTrade[j].response,"");
			}
	}

	if (creature[p].id==0)
		return;

	if (creature[p].home>0)
	{
		spawnRegion[creature[p].home].currentPopulation--;

		if (spawnRegion[creature[p].home].currentPopulation<0)
			spawnRegion[creature[p].home].currentPopulation++;
	}

	for (j=0;j<kMaxCreatureEffects;j++)
		if (creature[p].fx[j]>0)
		{
			serverFX[RC(creature[p].fx[j],kMaxFX)]	=	0;
			creature[p].fx[j]						=	0;
			creature[p].fxKillTime[j]		=	0;
			creature[p].fxTarget[j]			=	0;
			creature[p].fxTargetRow[j]	=	0;
			creature[p].fxTargetCol[j]	=	0;
			creature[p].activeFX				= false;
		}

	creatureInfo[creature[p].id].population--;

	creatureMap[creature[p].row][creature[p].col]	= 0;

	SendRefresh=false;

	if (creature[p].master!=0)
	{
		master=creature[creature[p].master].playerIndex;

		for (j=0;j<kMaxPets;j++)
			if (player[creature[creature[p].master].playerIndex].pet[j]==p)
			{
				player[creature[creature[p].master].playerIndex].sendPetsRefresh	=	true;
				player[creature[creature[p].master].playerIndex].pet[j]					  =	0;
				player[creature[creature[p].master].playerIndex].petInStable[j]	  = false;
				player[creature[creature[p].master].playerIndex].petType[j]			  = 0;
				player[creature[creature[p].master].playerIndex].petExperience[j]			  = 0;
				player[creature[creature[p].master].playerIndex].petHealth[j]			  = 0;
				strcpy(player[creature[creature[p].master].playerIndex].petName[j],"");
				SendRefresh=true;
				break;
			}
	}

	if (SendRefresh)
		nw_SendPetNameRefresh(master);

	if (i>0) // merge 7/27
	{
		strcpy(logStr,tb_getTimeAndDate("date&time"));
		strcat(logStr, ": ");
		strcat(logStr, player[i].playerName);
		strcat(logStr, " banish ");
		tx_GetCreatureName(creature[p].id, creatureStr);
		strcat(logStr, creatureStr);
		fi_WriteToGMLog(i, logStr);
	}

	cr_ClearCreatureRecord(p);

}

// ----------------------------------------------------------------- //
void pl_PitPlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];
	int pit;

	//if (!((player[i].category==1) || (player[i].category==2) || (player[i].category==4) || (player[i].category==5))) // merge 7/27

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanPitPlayers)==false) // add to carbon 11/27
		return;

	if (creature[p].playerIndex==0)
		return;
	
	if (pl_AuthorizedTo(player[p].creatureIndex,kIsDeveloper)) //Pit a dev => backlash
		pit=player[i].creatureIndex;
	else
		pit=p;
	
	strcpy(logStr,tb_getTimeAndDate("date&time"));
	strcat(logStr, ": ");
	strcat(logStr, player[i].playerName);

	strcat(logStr, " put in pit ");

	strcat(logStr, player[creature[pit].playerIndex].playerName);

	fi_WriteToGMLog(i, logStr);

	pl_PlayerMoveTo(creature[pit].playerIndex,316,1582);

}

// ----------------------------------------------------------------- //
void pl_JailPlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];
	int jail;

// if (!((player[i].category==1) || (player[i].category==2) || (player[i].category==4) || (player[i].category==5))) // merge 7/27

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanJailPlayers)==false) // add to carbon 11/27
		return;

	if (creature[p].playerIndex==0)
		return;
	
	if (pl_AuthorizedTo(player[p].creatureIndex,kIsDeveloper)) //Jailing a dev => backlash
		jail=player[i].creatureIndex;
	else
		jail=p;
		
	strcpy(logStr,tb_getTimeAndDate("date&time"));
	strcat(logStr, ": ");
	strcat(logStr, player[i].playerName);

	strcat(logStr, " jailed ");

	strcat(logStr, player[creature[jail].playerIndex].playerName);

	fi_WriteToGMLog(i, logStr);

	pl_PlayerMoveTo(creature[jail].playerIndex,964,654);

}

// ----------------------------------------------------------------- //
void pl_DumpPlayerClass(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              classStr[kStringLength];
	char			  levelStr[kStringLength];

	if (pl_AuthorizedTo(player[i].creatureIndex,kIsAnyGM)==false)
		if (pl_AuthorizedTo(player[i].creatureIndex,kIsQuestCharacter)==false)
			return;

	if (creature[p].playerIndex==0)  //Not a player
		return;
	else //Player
	{
		if (player[p].characterClass==1)
			strcpy(classStr,"Class:  Cleric");
		else if (player[p].characterClass==2)
			strcpy(classStr,"Class:  Druid");
		else if (player[p].characterClass==3)
			strcpy(classStr,"Class:  Fighter");
		else if (player[p].characterClass==4)
			strcpy(classStr,"Class:  Ranger");
		else if (player[p].characterClass==5)
			strcpy(classStr,"Class:  Rogue");
		else if (player[p].characterClass==6)
			strcpy(classStr,"Class:  Wizard");
		else if (pl_AuthorizedTo(player[p].creatureIndex,kIsApprenticeGM))
			strcpy(classStr,"Class:  Counselor");
		else if (pl_AuthorizedTo(player[p].creatureIndex,kIsRegularGM))
			strcpy(classStr,"Class:  Counselor");
		else if (pl_AuthorizedTo(player[p].creatureIndex,kIsSeniorGM))
			strcpy(classStr,"Class:  Game Master");
		else if (pl_AuthorizedTo(player[p].creatureIndex,kIsDeveloper))
			strcpy(classStr,"Class:  Developer");
		else
			strcpy(classStr,"Class:  N/A");

		strcat(classStr, " Level ");

		tb_IntegerToString(sk_GetCharacterLevel(p), levelStr);
	
		strcat(classStr, levelStr);

		nw_SendSystemTextToClient(i, classStr);
	}
}

// ----------------------------------------------------------------- //
void pl_MakeEvil(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char            logStr[kStringLength];
	char			evilStr[kStringLength];

	if (pl_AuthorizedTo(player[i].creatureIndex,kIsSeniorGM)==false)
		if (pl_AuthorizedTo(player[i].creatureIndex,kIsDeveloper)==false)
			if (pl_AuthorizedTo(player[i].creatureIndex,kIsQuestCharacter)==false)
				return;

	if (creature[p].playerIndex==0)  //Not a player
		return;
	else //Player
	{
		
		strcpy(logStr,tb_getTimeAndDate("date&time"));
		strcat(logStr, ": ");	
		strcat(logStr, player[i].playerName);
		strcat(logStr, " made evil ");
		strcat(logStr, player[p].playerName);
		fi_WriteToGMLog(i, logStr);

		if (player[p].category==0)
		{
			strcpy(evilStr, player[p].playerName);
			strcat(evilStr, "'s alignment shifted to evil.");
			player[p].category=8;
			nw_SendTextToClient(i, evilStr, 0);
			nw_SendTextToClient(p, "Alignment shifted to evil.", 0);
		}
	}
}

// ----------------------------------------------------------------- //
void pl_MakeGood(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	int				j;
	char            logStr[kStringLength];
	char			goodStr[kStringLength];

	if (pl_AuthorizedTo(player[i].creatureIndex,kIsSeniorGM)==false)
		if (pl_AuthorizedTo(player[i].creatureIndex,kIsDeveloper)==false)
			if (pl_AuthorizedTo(player[i].creatureIndex,kIsQuestCharacter)==false)
				return;

	if (creature[p].playerIndex==0)  //Not a player
		return;
	else //Player
	{
		strcpy(logStr,tb_getTimeAndDate("date&time"));
		strcat(logStr, ": ");
		strcat(logStr, player[i].playerName);
		strcat(logStr, " made good ");
		strcat(logStr, player[p].playerName);
		fi_WriteToGMLog(i, logStr);

		if (player[p].category==8)
		{
			strcpy(goodStr, player[p].playerName);
			strcat(goodStr, "'s alignment shifted to good.");
			player[p].category=0;

			for (j=0; j<kMaxPlayers; j++)
				if (creature[j].playerIndex!=0 && creature[j].attackTarget==i && player[creature[j].playerIndex].category==1) //clear target
				{
					creature[j].attackTarget			=	0;
					creature[j].nextStrikeTime		=	0;
					creature[j].numberOfStrikes	=	0;
					creature[j].trackRow					= 0;
					creature[j].trackCol					= 0;
					creature[j].tracking					= false;
				}

			cr_ClearAttackTarget(player[p].creatureIndex);

			nw_SendTextToClient(i, goodStr, 0);
			nw_SendTextToClient(p, "Alignment shifted to good.", 0);
		}
	}
}

// ----------------------------------------------------------------- //
void pl_MakeSay(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char	logStr[kStringLength];
	char	theStr[kStringLength];

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanMsay)==false)
		return;

	if (creature[p].playerIndex==0)  //Can't use Msay on players
	{
		creature[p].talkWhen  = TickCount();
		creature[p].talkTime  = TickCount();
		creature[p].talkID	  = chatMessageID;
		chatMessageID++;
		
		strcpy(logStr,tb_getTimeAndDate("date&time"));
		strcat(logStr, ": ");
		strcat(logStr, player[i].playerName);
		strcat(logStr, " msay ");
		strcat(logStr, player[i].msay);
		strcat(logStr, " to ");
		tx_GetCreatureName(creature[p].id, theStr);
		strcat(logStr, theStr);
		fi_WriteToGMLog(i, logStr);

		strcpy(creature[p].talk,player[i].msay);
		strcpy(player[i].msay, "");

		creature[p].hiding=false;

	}


}

// ----------------------------------------------------------------- //
void pl_MutePlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanMute)==false)
		return;

	if (creature[p].playerIndex!=0)  //Player, add to gm log
	{
		strcpy(logStr,tb_getTimeAndDate("date&time"));
		strcat(logStr, ": ");
		strcat(logStr, player[i].playerName);
		strcat(logStr, " muted ");
		strcat(logStr, player[creature[p].playerIndex].playerName);
		fi_WriteToGMLog(i, logStr);

		strcpy(logStr, player[creature[p].playerIndex].playerName);
		strcat(logStr, " ");
	}
	else
		strcpy(logStr, "");

	strcat(logStr, "muted.");

	nw_SendTextToClient(i, logStr, 0);


	creature[p].mute=true;

}

// ----------------------------------------------------------------- //
void pl_UnmutePlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanMute)==false)
		return;

	if (creature[p].playerIndex!=0)  //Player, add to gm log
	{
		strcpy(logStr,tb_getTimeAndDate("date&time"));
		strcat(logStr, ": ");
		strcat(logStr, player[i].playerName);
		strcat(logStr, " unmuted ");
		strcat(logStr, player[creature[p].playerIndex].playerName);
		fi_WriteToGMLog(i, logStr);

		strcpy(logStr, player[creature[p].playerIndex].playerName);
		strcat(logStr, " ");
	}
	else
		strcpy(logStr, "");

	strcat(logStr, "unmuted.");

	nw_SendTextToClient(i, logStr, 0);

	creature[p].mute=false;

}

// ----------------------------------------------------------------- //
void pl_DumpPlayerSkills(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              	skillStr[kStringLength], temp[kStringLength];
	int					j=creature[p].playerIndex, c;

	if (pl_AuthorizedTo(player[i].creatureIndex,kIsAnyGM)==false && pl_AuthorizedTo(player[i].creatureIndex,kIsQuestCharacter)==false)
		return;

	if (j==0)
		return;

	nw_SendTextToClient(i, player[j].playerName,0);

	if (player[j].noGain)
		nw_SendTextToClient(i, "Skill Gain Off",0);

	else nw_SendTextToClient(i, "Skill Gain On",0);


	for (c=0; c<kMaxSkills; c++)
	{
		strcpy(skillStr, "");
		tx_GetSkillName(c, skillStr);
		tb_NumToString(sk_GetSkillLevel(player[j].skill[c].level), temp);
		strcat(skillStr, ": ");
		strcat(skillStr, temp);
		tb_NumToString(player[j].skill[c].level, temp);
		strcat(skillStr, " (");
		strcat(skillStr, temp);
		tb_NumToString(sk_GetSkillNextLevel(player[j].skill[c].level), temp);
		strcat(skillStr, "/");
		strcat(skillStr, temp);
		strcat(skillStr, ")");
		nw_SendTextToClient(i, skillStr,0);
	}


}

// ----------------------------------------------------------------- //
void pl_BanPlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];

	if (pl_AuthorizedTo(player[i].creatureIndex, kCanBan)==false)
		return;

	if (creature[p].playerIndex==0)
		return;
	
	if (pl_AuthorizedTo(p, kIsAnyGM))
		if (pl_AuthorizedTo(player[i].creatureIndex, kIsDeveloper)==false)
			return;
		
	if (pl_AuthorizedTo(p, kIsDeveloper))
		return;

	strcpy(logStr,tb_getTimeAndDate("date&time"));
	strcat(logStr, ": ");
	strcat(logStr,player[i].playerName);

	strcat(logStr," ban ");

	strcat(logStr,player[creature[p].playerIndex].playerName);

	fi_WriteToGMLog(i, logStr);

	fi_AddToBanFile(i,p);

	player[creature[p].playerIndex].online=false;

	nw_RemovePlayer(player[creature[p].playerIndex].id);

	/*cr_ClearAttackTarget(p);
	fi_Logout(creature[p].playerIndex);
	nw_SendPlayerLeft(player[creature[p].playerIndex].id);
	nw_RemovePlayer(player[creature[p].playerIndex].id);
	nw_SendClearName(creature[p].playerIndex);
	creatureMap[creature[p].row][creature[p].col]=0;
	cr_ClearCreatureRecord(p);
	pl_ClearPlayerRecord(creature[p].playerIndex);
	#ifdef _SERVERONLY
	gr_DrawServerNameAt(creature[p].playerIndex);
	#endif*/

}

// ----------------------------------------------------------------- //
void pl_KickPlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];

	//if (player[i].category!=1)

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanKickPlayers)==false) // add to carbon 11/27
		return;

	if (creature[p].playerIndex==0)
		return;
	
	if (pl_AuthorizedTo(p, kIsAnyGM))
		if (pl_AuthorizedTo(player[i].creatureIndex, kIsDeveloper)==false)
			return;
		
	if (pl_AuthorizedTo(p, kIsDeveloper))
		return;

	//fi_WriteToErrorLog("Kick Player - Remove Player:");
	//fi_WriteToErrorLog(player[creature[p].playerIndex].playerName);


	strcpy(logStr,tb_getTimeAndDate("date&time"));
	strcat(logStr, ": ");
	strcat(logStr,player[i].playerName);

	strcat(logStr," kicks ");

	strcat(logStr,player[creature[p].playerIndex].playerName);

	fi_WriteToGMLog(i, logStr);

	player[creature[p].playerIndex].online=false;

	nw_RemovePlayer(player[creature[p].playerIndex].id);

// cr_ClearAttackTarget(creature[p].playerIndex);
// nw_SendPlayerLeft(player[creature[p].playerIndex].id);
//	nw_RemovePlayer(player[creature[p].playerIndex].id);
//	nw_SendClearName(creature[p].playerIndex);
//	fi_Logout(creature[p].playerIndex);
//	creatureMap[creature[player[creature[p].playerIndex].creatureIndex].row][creature[player[creature[p].playerIndex].creatureIndex].col]=0;
//	cr_ClearCreatureRecord(player[creature[p].playerIndex].creatureIndex);
//	pl_ClearPlayerRecord(creature[p].playerIndex);
//	#ifdef _SERVERONLY
//	gr_DrawServerNameAt(creature[p].playerIndex);
//	#endif

}

// ----------------------------------------------------------------- //
void pl_GainPlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];

	//if (!((player[i].category==1) || (player[i].category==2) || (player[i].category==4) || (player[i].category==5))) // merge 7/27

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanControlSkillGain)==false) // add to carbon 11/27
		return;

	if (creature[p].playerIndex==0)
		return;
	
	strcpy(logStr,tb_getTimeAndDate("date&time"));
	strcat(logStr, ": ");

	strcat(logStr, player[i].playerName);

	strcat(logStr, " - skill gain on ");

	strcat(logStr, player[creature[p].playerIndex].playerName);

	fi_WriteToGMLog(i, logStr);

	player[creature[p].playerIndex].noGain=false;

	nw_SendTextToClient(i, "Skill Gain On", 0);

}

// ----------------------------------------------------------------- //
void pl_NoGainPlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];

	//if (!((player[i].category==1) || (player[i].category==2) || (player[i].category==4) || (player[i].category==5))) // merge 7/27

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanControlSkillGain)==false) // add to carbon 11/27
		return;

	if (creature[p].playerIndex==0)
		return;

	strcpy(logStr,tb_getTimeAndDate("date&time"));
	strcat(logStr, ": ");
	strcat(logStr, player[i].playerName);

	strcat(logStr, " - skill gain off ");

	strcat(logStr, player[creature[p].playerIndex].playerName);

	fi_WriteToGMLog(i, logStr);

	player[creature[p].playerIndex].noGain=true;

	nw_SendTextToClient(i, "Skill Gain Off", 0);

}

// ----------------------------------------------------------------- //
void pl_ResPlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];

	//if (!((player[i].category==1) || (player[i].category==2) || (player[i].category==4) || (player[i].category==5))) // merge 7/27

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanResPlayers)==false) // add to carbon 11/27
		return;

	if (creature[p].playerIndex==0)
		return;

	if (player[creature[p].playerIndex].dead==false)
		return;
	
	
	strcpy(logStr,tb_getTimeAndDate("date&time"));
	strcat(logStr, ": ");
	strcat(logStr, player[i].playerName);

	strcat(logStr, " res ");

	strcat(logStr, player[creature[p].playerIndex].playerName);

	fi_WriteToGMLog(i, logStr);

	player[creature[p].playerIndex].dead=false;

	player[creature[p].playerIndex].sendStatsRefresh=true;

	creature[player[p].creatureIndex].hitPoints=100;

	if (creature[p].strength<creature[p].hitPoints)
		creature[p].hitPoints=creature[p].strength;

	creature[p].magicPoints=100;

	if (creature[p].intelligence<creature[p].magicPoints)
		creature[p].magicPoints=creature[p].intelligence;

}

// ----------------------------------------------------------------- //
void pl_KillPlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanKillPlayers)==false) // add to carbon 11/27
		return;

	if (pl_AuthorizedTo(p, kIsAnyGM)==true)  //Can't kill a GM (unless you're a dev >:) ).
		if (pl_AuthorizedTo(player[i].creatureIndex, kIsDeveloper)==false)
			return;
		
	if (pl_AuthorizedTo(p, kIsDeveloper)==true) //Can't kill a dev whatsoever
		return;

	if (creature[p].npcIndex!=0) //Can't kill a non spawned NPC
	{
		if (npc[creature[p].npcIndex].type==kNPCRegular || npc[creature[p].npcIndex].type==kNPCGuard)
			if (creature[p].master==0)
				return;
	}

	if (creature[p].playerIndex!=0) //Only log players kill
	{
		if (player[creature[p].playerIndex].dead==true)
			return;

		strcpy(logStr,tb_getTimeAndDate("date&time"));
		strcat(logStr, ": ");
		strcat(logStr, player[i].playerName);

		strcat(logStr, " kill ");

		strcat(logStr, player[creature[p].playerIndex].playerName);

		fi_WriteToGMLog(i, logStr);
	}

	if (!player[i].stealth)
		cr_AddSound(i, kBoltSnd);

	fx_CreatureServerFX(i, p, kLightEllipse, (3*60), 10);

	fx_CreatureServerFX(i, p, kLightningBolt, (3*60), 0);

	fx_CreatureServerFX(i, p, kLightningBoltReverse, (3*60), 0);
	
	creature[p].godMode=false;
	creature[p].noDeath=false;

	creature[p].hitPoints=0;

	cr_setDieTime(p,(TickCount()+20));

}

// ----------------------------------------------------------------- //
void pl_ResetPlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];
	int               j;

	//if (!((player[i].category==1) || (player[i].category==2) || (player[i].category==4) || (player[i].category==5))) // merge 7/27

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanResetPlayers)==false) // add to carbon 11/27
		return;

	if (creature[p].playerIndex==0)
		return;
	
	
	strcpy(logStr,tb_getTimeAndDate("date&time"));
	strcat(logStr, ": ");
	strcat(logStr, player[i].playerName);

	strcat(logStr, " reset ");

	strcat(logStr, player[p].playerName);

	fi_WriteToGMLog(i, logStr);

	it_CalculateArmorRating(p);

	pl_CalculateStats(p);

	player[p].sendStatsRefresh=true;

	player[p].sendPetsRefresh=true;

	nw_SendInventoryRefresh(p);

	nw_SendSkillsRefresh(p);

	nw_SendArmRefresh(p);

	nw_SendClientCommand(p,kClearNamesCommand);

	pl_CalculatePetLoyalty(p);

	for (j=0;j<kMaxPets;j++)
		if (player[p].petType[j]!=0)
		{
			nw_SendPetNameRefresh(p);
			break;
		}

}

// ----------------------------------------------------------------- //
void pl_SendPlayerHome(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	char              logStr[kStringLength];

	//if (!((player[i].category==1) || (player[i].category==2) || (player[i].category==4) || (player[i].category==5))) // merge 7/27

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanSendHome)==false) // add to carbon 11/27
		return;

	if (creature[p].playerIndex==0)
		return;
	
	strcpy(logStr,tb_getTimeAndDate("date&time"));
	strcat(logStr, ": ");

	strcat(logStr, player[i].playerName);

	strcat(logStr, " home ");

	strcat(logStr, player[p].playerName);

	fi_WriteToGMLog(i, logStr);

	pl_PlayerMoveTo(creature[p].playerIndex,788,1600);

}

// ----------------------------------------------------------------- //
void pl_DumpPlayerInv(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	int 		j;
	char		itemName[kStringLength];
	UInt32		gold;

	if (pl_AuthorizedTo(player[i].creatureIndex,kIsSeniorGM)==false && pl_AuthorizedTo(player[i].creatureIndex,kIsDeveloper)==false && pl_AuthorizedTo(player[i].creatureIndex,kIsQuestCharacter)==false) // add to carbon 11/27
		return;

	if (p==0)
		return;
	
	gold=in_GetGoldTotal(creature[p].inventory[0].itemID, creature[p].inventory[0].itemCount);
	
	sprintf(itemName, "%d gold piece(s)", gold);
	nw_SendTextToClient(i, itemName, 0);

	for (j=1; j<kMaxInventorySize; j++)
	{
		if (creature[p].inventory[j].itemID!=0)
		{
			in_GetItemName(creature[p].inventory[j].itemID, itemName, creature[p].inventory[j].itemCount);
			nw_SendTextToClient(i, itemName, 0);
		}
	}

}

// ----------------------------------------------------------------- //
void pl_DumpPlayerBank(int i, int p)		// server
// ----------------------------------------------------------------- //

{
	int 		j;
	char		itemName[kStringLength];
	UInt32		gold;

	if (pl_AuthorizedTo(player[i].creatureIndex,kIsSeniorGM)==false && pl_AuthorizedTo(player[i].creatureIndex,kIsDeveloper)==false && pl_AuthorizedTo(player[i].creatureIndex,kIsQuestCharacter)==false) // add to carbon 11/27
		return;

	if (creature[p].playerIndex==0)
		return;
	
	gold=in_GetGoldTotal(player[creature[p].playerIndex].bank[0].itemID, player[creature[p].playerIndex].bank[0].itemCount);
	
	sprintf(itemName, "%d gold piece(s)", gold);
	nw_SendTextToClient(i, itemName, 0);

	for (j=1; j<kMaxInventorySize; j++)
	{
		if (player[creature[p].playerIndex].bank[j].itemID!=0)
		{
			in_GetItemName(player[creature[p].playerIndex].bank[j].itemID, itemName, player[creature[p].playerIndex].bank[j].itemCount);
			nw_SendTextToClient(i, itemName, 0);
		}
	}

}

// ----------------------------------------------------------------- //
void pl_ActivatePlayer(int i, int p)		// server
// ----------------------------------------------------------------- //

{

	//if (!((player[i].category==1) || (player[i].category==2) || (player[i].category==4) || (player[i].category==5) || (player[i].helper))) // merge 7/27
	if (pl_AuthorizedTo(player[i].creatureIndex,kCanActivate)==false) // add to carbon 11/27
		return;

	if (creature[p].playerIndex==0)
		return;

// if (player[creature[p].playerIndex].demo==false)
//   return;

	player[creature[p].playerIndex].demo=false;

	nw_SendClearName(creature[p].playerIndex);

}

// ----------------------------------------------------------------- //
void pl_AddToTitle(int i, int amount)		// server
// ----------------------------------------------------------------- //

{

	player[i].title=player[i].title+amount;

	if (player[i].title>500000)
		player[i].title=500000;

}

// ----------------------------------------------------------------- //
void pl_GetLocationName(char location[kStringLength], int /*row*/, int /*col*/)		// server
// ----------------------------------------------------------------- //

{

	strcpy(location,"");

	//if ((row>2221) && (row<2337) && (col>1865) && (col<1880))
		//strcpy(location,"Blacksmith Shop");

}

#endif

// ----------------------------------------------------------------- //
void pl_InitPlayers(void)
// ----------------------------------------------------------------- //

{
	int					i;

	sk_ClearLocalSkills();

#ifdef _SERVER

	for (i=0;i<kMaxPlayers;i++)
		pl_ClearPlayerRecord(i);

#endif

	wasStandingInShop=false;

	wasStandingInBank=false;

	wasStandingOnActionSpot=false;

	localStrength			=	0;

	localIntelligence	=	0;

	localDexterity		=	0;

	localArmorRating	=	0;

	localHitPoints		=	0;

	localMagicPoints	=	0;

	localStamina			=	0;

	localWeight				=	0;

	localLeftHand			= 0;

	localRightHand		= 0;

	localLegs					= 0;

	localTorso				= 0;

	localFeet					= 0;

	localRightFinger	= 0;

	localLeftFinger		= 0;

	localHead					= 0;

	localNeck					= 0;

	localHands				= 0;

	localWaist				= 0;
	
	localSpat				= 0;

}

// ----------------------------------------------------------------- //
TBoolean pl_CheckSpecialAttack(int spat) //client
// ----------------------------------------------------------------- //

{
	//TBoolean bOk=true;
	//int targetFacing;
		
	//char deb[255];
	//Dave
	int gold=in_GetGoldTotal(localInventory[0].itemID, localInventory[0].itemCount);	
	//
	if (gold==8192)
		{
			return;
		}
	if (spat==localSpat) //attempt to cancel current spat
	{
		localSpat=0;
	}
	else
	{
		switch (spat)
		{
			case kAttackBackstab:
			case kAttackDecimate:
			/*//if (player[i].stealth==false)
			if (pl_AuthorizedTo(i, kRegularPlayer) && player[creature[i].playerIndex].stealth==false)
				{
					gr_AddText("You must be stealthy before you can attack.", false, true);
					return (false);
				}
			*/
			case kAttackSting:
						
				//if (localTarget==255) //must have target
				//{
					//tx_AddText(kMustHaveTargetMsg);
					//return (false);
				//}
				
				if (in_ItemTypeEquiped(kDaggers))
				{
					tx_AddText(kMustWearDaggerMsg);
					return (false);
				}
				
				//if (!pl_BehindTarget())
				//{
					//tx_AddText(kMustBeBehindTargetMsg);
					//return (false);
				//}
				
			break;
			
			case kAttackRage:
				if ((localRightFinger==0 || localInventory[localRightFinger].itemID!=kRingOfBruteStrength) && (localLeftFinger==0 || localInventory[localLeftFinger].itemID!=kRingOfBruteStrength))
					{
					gr_AddText("You must have Ring of Brute Strength equipped.", false, true);
					return (false);
					}
		//Dave	rage must have melee weapon
				if (itemList[localInventory[localRightHand].itemID].useSkill != kMaceFighting && itemList[localInventory[localRightHand].itemID].useSkill != kPoleFighting && itemList[localInventory[localRightHand].itemID].useSkill != kSwordsmanship) //must have melee weap equipped
					{
					tx_AddText(kMustHaveMeleeWeaponMsg);
					return (false);
					}
			
			break;
			
			case kAttackKnockback:
				if ((localRightFinger==0 || localInventory[localRightFinger].itemID!=kRingOfStrength) && (localLeftFinger==0 || localInventory[localLeftFinger].itemID!=kRingOfStrength))
					if ((localRightFinger==0 || localInventory[localRightFinger].itemID!=kRingOfBruteStrength) && (localLeftFinger==0 || localInventory[localLeftFinger].itemID!=kRingOfBruteStrength))
					{
						gr_AddText("You must have any Strength ring equipped.", false, true);
						return (false);
					}
				
				if (itemList[localInventory[localRightHand].itemID].useSkill != kMaceFighting && itemList[localInventory[localRightHand].itemID].useSkill != kPoleFighting && itemList[localInventory[localRightHand].itemID].useSkill != kSwordsmanship) //must have melee weap equipped
					{
					tx_AddText(kMustHaveMeleeWeaponMsg);
					return (false);
					}
			
			//Dave
			if (itemList[localInventory[localRightHand].itemID].useSkill != kMaceFighting && itemList[localInventory[localRightHand].itemID].useSkill != kPoleFighting && itemList[localInventory[localRightHand].itemID].useSkill != kSwordsmanship) //must have melee weap equipped
				{
					tx_AddText(kMustHaveMeleeWeaponMsg);
					return (false);
				}
			
			break;
			
			case kAttackInstantStrike:
			
				if (localTarget==255) //must have target
				{
					tx_AddText(kMustHaveTargetMsg);
					return (false);
				}
				
				if (itemList[localRightHand].useSkill != kDaggers)
				{
					tx_AddText(kMustWearDaggerMsg);
					return (false);
				}
				
			break;
			
			case kAttackVial:
			
			 
				
				if (in_PlayerHas(kVialOfAcid)==0)
				{
					gr_AddText("You must have explosive dust.", false, true);
					return false;
				}
				
				if (localTarget==255) //must have target
				{
					tx_AddText(kMustHaveTargetMsg);
					return (false);
				}
				
			break;
			
			case kAttackBoomingBlow:
			case kAttackDoubleStrike:
			case kAttackEscapeMove:
			case kAttackTaunt:
			
				//if (localTarget==255) //must have target
				//{
					//tx_AddText(kMustHaveTargetMsg);
					//return (false);
				//}
				
				if (localRightHand == 0) //must have weapon equipped
				{
					tx_AddText(kMustHaveWeaponMsg);
					return (false);
				}
				
				if (itemList[localRightHand].useSkill != kMaceFighting && itemList[localInventory[localRightHand].itemID].useSkill != kPoleFighting && itemList[localInventory[localRightHand].itemID].useSkill != kSwordsmanship) //must have melee weap equipped
				{
					tx_AddText(kMustHaveMeleeWeaponMsg);
					return (false);
				}
				
				//if (!pl_NearTarget()) //must be near target
				//{
					//tx_AddText(kMustBeNearTargetMsg);
					//return (false);
				//}
			
			break;
			
			case kAttackQuickDraw:
				
				if (localRightHand == 0) //must have weapon equipped
				{
					tx_AddText(kMustHaveWeaponMsg);
					return (false);
				}
				
			break;
			
			case kAttackGrapplingShot:
			case kAttackDoubleShot:
			case kAttackAimedShot:
			{
				if (localTarget==255) //must have target
				{
					tx_AddText(kMustHaveTargetMsg);
					return (false);
				}
				
				if (itemList[localInventory[localRightHand].itemID].useSkill != kArcheryFighting) //must have bow equipped
				{
					tx_AddText(kMustHaveBowMsg);
					return (false);
				}
				
				//distance will be checked server-wise
				
				break;
			}
			
			case kAttackLesserDefense:
			case kAttackGreaterDefense:
			{
				if (itemList[localInventory[localLeftHand].itemID].itemType!=kArmorType) //must be weilding shield
				{
					tx_AddText(kMustHaveShieldMsg);
					return (false);
				}
				
				break;
			}
			
			case kAttackRangerSwiftness:
			{
				if (itemList[localInventory[localRightHand].itemID].useSkill != kArcheryFighting) //must have bow equipped
				{
					tx_AddText(kMustHaveBowMsg);
					return (false);
				}
				
				break;
			}
		}
		
		localSpat=spat;	
	}
		
	return (true);
}

// ----------------------------------------------------------------- //
TBoolean pl_NearTarget() //client
// ----------------------------------------------------------------- //
{
	if (localTarget==71)	return (true);
	if (localTarget==72)	return (true);
	if (localTarget==73)	return (true);
	if (localTarget==89)	return (true);
	if (localTarget==91)	return (true);
	if (localTarget==108)	return (true);
	if (localTarget==109)	return (true);
	if (localTarget==110)	return (true);
	
	return (false);
}

// ----------------------------------------------------------------- //
TBoolean pl_BehindTarget() //client
// ----------------------------------------------------------------- //
{
	int targetFacing;
	
	targetFacing=(creatureFacing[localTarget]>9)?creatureFacing[localTarget]-10:creatureFacing[localTarget];
	
	switch (localTarget)
	{
		case 71:
			if (targetFacing==3)	return true;
			if (targetFacing==4)	return true;
			if (targetFacing==5)	return true;
		break;
		
		case 72:
			if (targetFacing==2)	return true;
			if (targetFacing==3)	return true;
			if (targetFacing==4)	return true;
		break;
		
		case 73:
			if (targetFacing==1)	return true;
			if (targetFacing==2)	return true;
			if (targetFacing==3)	return true;
		break;
		
		case 89:
			if (targetFacing==4)	return true;
			if (targetFacing==5)	return true;
			if (targetFacing==6)	return true;
		break;
		
		case 91:
			if (targetFacing==1)	return true;
			if (targetFacing==2)	return true;
			if (targetFacing==0)	return true;
		break;
		
		case 108:
			if (targetFacing==5)	return true;
			if (targetFacing==6)	return true;
			if (targetFacing==7)	return true;
		break;
		
		case 109:
			if (targetFacing==6)	return true;
			if (targetFacing==7)	return true;
			if (targetFacing==0)	return true;
		break;
		
		case 110:
			if (targetFacing==7)	return true;
			if (targetFacing==0)	return true;
			if (targetFacing==1)	return true;
		break;
	}
	
	return (false);
}

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean pl_BehindTargetServer(int c1, int c2) //server
// ----------------------------------------------------------------- //
{
	int facing1, facing2;
		
	facing1=(creature[c1].facing>9)?creature[c1].facing-10:creature[c1].facing;
	facing2=(creature[c2].facing>9)?creature[c2].facing-10:creature[c2].facing;
		
	if (facing2==0 && facing1==7)	return true;
	if (facing2==7 && facing1==0)	return true;
	
	if (facing1==facing2-1)		return true;
	if (facing1==facing2)		return true;
	if (facing1==facing2+1)		return true;
	
	return false;
}

// ----------------------------------------------------------------- //
void pl_PlayerMoveTo(int i, UInt16 row, UInt16 col) // server
// ----------------------------------------------------------------- //

{
	int														oRow;
	int														oCol;
	int														pRow;
	int														pCol;
	TBoolean												isInID=false, jumpsInID=false;


	pl_CheckForActionInterrupted(i, false);

	if (player[i].category==0 || player[i].category==8)
		player[i].lastMove=TickCount();

	oRow=creature[player[i].creatureIndex].row;

	oCol=creature[player[i].creatureIndex].col;

	if (oRow>1864 && oRow<1978 && oCol>1010 && oCol<1111)
		isInID=true;

	if (row>1864 && row<1978 && col>1010 && col<1111)
		jumpsInID=true;

	creatureMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col]	=	0;

	if ((map[row][col]==395) || 
			(map[row][col]==908) || 
				(map[row][col]==900) || 
					(map[row][col]==901) || 
						(map[row][col]==1009) || 
							(map[row][col]==1010) ||
								(map[row][col]==1109))	// stepped on latter
	{
		pRow=row;
		pCol=col;
		cr_SteppedOnLatter(player[i].creatureIndex,&row,&col);

		if ((row<0) || (col<0))
		{
			row=pRow;
			col=pCol;
			cr_EmptySpotNear(player[i].creatureIndex,&row, &col,false);

			if ((row<0) || (col<0))
			{
				row=oRow;
				col=oCol;
			}
		}
		else
		{
			creature[player[i].creatureIndex].hiding=true;
			nw_SendRefresh(i,true);
			creature[player[i].creatureIndex].hiding=false;
		}
	}

	cr_GetNearestFreeLocation(player[i].creatureIndex, &row, &col,"pl_PlayerMoveTo");

	if ((row<=0) && (col<=0))
	{
		row=oRow;
		col=oCol;
	}

	creature[player[i].creatureIndex].row							=	row;

	creature[player[i].creatureIndex].col							=	col;
	creatureMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col]	=	player[i].creatureIndex;
	creature[player[i].creatureIndex].moveDirection		=	4;
	creature[player[i].creatureIndex].timeStamp				= TickCount();
	creature[player[i].creatureIndex].strikePose			= 0;
	creature[player[i].creatureIndex].nextStrikeTime	= 0;
	creature[player[i].creatureIndex].numberOfStrikes	= 0;
	creature[player[i].creatureIndex].attackTarget		= 0;

	//strcpy(player[i].talk, "");
	//player[i].talkTime=0;
	//player[i].talkID=0;

	strcpy(creature[player[i].creatureIndex].talk, "");
	creature[player[i].creatureIndex].talkWhen=0;
	creature[player[i].creatureIndex].talkTime=0;
	creature[player[i].creatureIndex].talkID=0;

	if (pl_AuthorizedTo(player[i].creatureIndex, kRegularPlayer))
		if (player[i].stealth)
		{
			nw_SendDisplayMessage(i, kLeavingStealth);
			player[i].stealth = false;
			player[i].stealthCool = TickCount()+3*60;
		}

	nw_SendRefresh(i,true);

	player[i].skipNextRefresh	= true;


}

// ----------------------------------------------------------------- //
TBoolean pl_IsThief(int c) // server
// ----------------------------------------------------------------- //

{

	if (creature[c].playerIndex==0)
		return (false);

	//if (player[creature[c].playerIndex].category==5)	// merge 7/28
	if (pl_AuthorizedTo(c,kIsQuestScoundrel)) // add to carbon 11/27
		return (true);

	if (player[creature[c].playerIndex].thiefUntil==0)
		return (false);

	if (player[creature[c].playerIndex].thiefUntil>TickCount())
		return (true);

	player[creature[c].playerIndex].thiefUntil=0;

	return (false);

}

// ----------------------------------------------------------------- //
void pl_AddSpecialAttack(int i, int spat) //server
// ----------------------------------------------------------------- //
{
	int c1=player[i].creatureIndex;
	int c2=creature[c1].attackTarget;
	
	//int										rowDist;
	//int										colDist;
	//int										dist;
	
	int j;
	
	float damage=0.0;
	float resist=0.0;
	
	int attackType1=0;
	int attackType2=0;
	TBoolean bOk=true;
	
	int currentTime=TickCount();
		
	//char deb[255];
	
	//------------------ Attack type 2 : ------------------
	//
	//0 : pay on using - doesn't break on moving
	//1 : pay on activation - doesn't break on moving
	//2 : pay on activation - breaks on moving
	//3 : pay on using - breaks on moving
	//
	//-----------------------------------------------------
	
	
	if (player[i].dead) //check again
		return;
	
	switch (spat)
	{
		case kAttackGrapplingShot: //check archery distance & los
			if (c2==0)
			{
				cr_ClearSpat(c1, true);
				return;	
			}
			
			if (cr_DistanceBetween(c1,c2)>itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].weaponRange)
			{
				nw_SendDisplayMessage(i, kCreatureTooFarAway);
				cr_ClearSpat(c1, true);
				return;
			}
			
			if (cr_ViewBlocked(creature[c1].row, creature[c1].col, creature[c2].row, creature[c2].col))
			{
				cr_ClearSpat(c1, true);
				nw_SendDisplayMessage(i, kCantSeeTarget);
				return;
			}
			
		case kAttackRage:
			attackType2=1;
			attackType1=kSpatDirectDamage;
		break;
		
		case kAttackInstantStrike:
		
			if (creature[c1].nextAttackTime>currentTime)
			{
				if (creature[c1].stamina<itemList[spat].price)
				{
					nw_SendDisplayMessage(i, kNotEnoughStaminaMsg);
				}
				else
				{
					creature[c1].stamina-=itemList[spat].price;
					creature[c1].nextAttackTime=currentTime;
					
					fx_CreatureServerFX(c1, c1, kSpatSwordX, (1*60), 0);
					
					sk_TrainProof(c1, c2, 0, kSpecialRogue, 1);
					
					player[i].sendStatsRefresh=true;
				}
			}
			
			nw_SendClearSpat(i);
			return;

		break;
		
		case kAttackVial:
		
			//Dave can't use while paralyzed
			if (creature[c1].paralyzed!=0)
				{
				cr_ClearSpat(c1, true);
				nw_SendDisplayMessage(i, kYouAreParalyzed);
				return;
				}
			if (creature[c1].disorientation!=0)
				{
				cr_ClearSpat(c1, true);
				nw_SendDisplayMessage(i, kYouAreDisoriented);
				return;
				}
			/**/
			//Dave - can't use through walls
			if (cr_ViewBlocked(creature[c1].row, creature[c1].col, creature[c2].row, creature[c2].col))
				if (cr_ViewBlocked(creature[c2].row, creature[c2].col, creature[c1].row, creature[c1].col))  //JS line of sight bug
						{
						nw_SendDisplayMessage(i, kCantSeeTarget);
						cr_ClearSpat(c1, true);
						return;
						}
			
			//Dave^
			
			attackType1=in_GetSlotOf_s(c1, kVialOfAcid);
			
			if (attackType1==0)
			{
				cr_ClearSpat(c1, true);
				return;
			}
			
			if (cr_DistanceBetween(c1,c2)>5)
			{
				nw_SendDisplayMessage(i, kCreatureTooFarAway);
				cr_ClearSpat(c1, true);
				return;
			}
			/*
			if (player[i].stealth==true)
			{
				(player[i].stealth==false);
				cr_ClearSpat(c1, true);
				return;
			}
			*/
			if (creature[c1].stamina<itemList[spat].price)
			{
				nw_SendDisplayMessage(i, kNotEnoughStaminaMsg);
			}
			else
			{
				creature[c1].stamina-=itemList[spat].price;					
				player[i].sendStatsRefresh=true;
				
				in_RemoveFromInventorySlot(c1, attackType1, 1);
				nw_SendInventoryRefresh(i);
				fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
				
				damage=tb_Rnd(10,20); //Dave (15,20);
				
				if (creature[c2].damageReduc<1.0)
					damage=tb_Round((float)damage*creature[c2].damageReduc);
				
				if (creature[c2].hitPoints<damage)
				{
					creature[c2].hitPoints=0;
					cr_setDieTime(c2, currentTime+120);
				}
				else
					creature[c2].hitPoints-=damage;
				
				damage=tb_Rnd(9,12);
				damage*=60;
				
				if (creature[c2].playerIndex!=0)
					resist=sk_GetMagicResistance(creature[c2].playerIndex);
				else
				{
					resist=creatureInfo[creature[c2].id].magicResist;

					for (j=1;j<creature[c2].level;j++)
						resist*=0.9;
				}
				
				damage=damage*resist;
				
				if (damage>0)
				{
					creature[c2].paralyzed=currentTime+damage;
				}
				
				sk_TrainProof(c1, c2, 0, kSpecialRogue, 2);
				
				if (creature[c2].playerIndex!=0)
				{
					nw_SendDisplayMessage(creature[c2].playerIndex, kVialOfAcidMsg);
					player[creature[c2].playerIndex].sendStatsRefresh=true;
				}
					
			
				if (pl_AuthorizedTo(player[i].creatureIndex,kRegularPlayer)) 
					if (player[i].stealth)
						{
						nw_SendDisplayMessage(creature[i].playerIndex, kLeavingStealth);
						player[creature[i].playerIndex].stealth = false;
						player[creature[i].playerIndex].stealthCool = TickCount()+3*60;
						}
				}	
				nw_SendClearSpat(i);
				return;
		
		break;
		
		case kAttackBackstab:
		case kAttackDecimate:
		if (player[i].stealth==false)
			{
			nw_SendDisplayMessage(creature[i].playerIndex, kMustBeStealthed);
				cr_ClearSpat(c1, true);
				return;
			}
		case kAttackBoomingBlow:
		case kAttackEscapeMove:
		case kAttackSting:
		case kAttackKnockback:
		case kAttackTaunt:
			attackType1=kSpatDirectDamage;
			//if (c2==0)
			//{
				//cr_ClearSpat(c1, true);
				//return;	
			//}
		break;
		
		case kAttackAimedShot:
			attackType2=3;
			if (c2==0)
			{
				cr_ClearSpat(c1, true);
				return;	
			}
			
			if (cr_DistanceBetween(c1,c2)>itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].weaponRange)
			{
				nw_SendDisplayMessage(i, kCreatureTooFarAway);
				cr_ClearSpat(c1, true);
				return;
			}
			
			if (cr_ViewBlocked(creature[c1].row, creature[c1].col, creature[c2].row, creature[c2].col))
			{
				nw_SendDisplayMessage(i, kCantSeeTarget);
				cr_ClearSpat(c1, true);
				return;
			}
			
			creature[c1].nextAttackTime+=60;
			
			attackType1=kSpatDirectDamage;
		break;
		
		case kAttackDoubleShot: //check archery distance & los
			if (c2==0)
			{
				cr_ClearSpat(c1, true);
				return;	
			}
			
			if (cr_DistanceBetween(c1,c2)>itemList[creature[c1].inventory[player[creature[c1].playerIndex].arm[kRightHand]].itemID].weaponRange)
			{
				nw_SendDisplayMessage(i, kCreatureTooFarAway);
				cr_ClearSpat(c1, true);
				return;
			}
			
			if (cr_ViewBlocked(creature[c1].row, creature[c1].col, creature[c2].row, creature[c2].col))
			{
				nw_SendDisplayMessage(i, kCantSeeTarget);
				cr_ClearSpat(c1, true);
				return;
			}
		
		case kAttackDoubleStrike:
			//if (c2==0)
			//{
				//cr_ClearSpat(c1, true);
				//return;	
			//}
			
			attackType1=kSpatAttackSpeed;
		break;
		
		case kAttackQuickDraw:
			if (creature[c1].stamina<itemList[spat].price)
			{
				nw_SendDisplayMessage(i, kNotEnoughStaminaMsg);
			}
			else if (creature[c1].nextAttackTime>currentTime+180)
			{
				creature[c1].stamina-=itemList[spat].price;					
				player[i].sendStatsRefresh=true;
				
				fx_CreatureServerFX(c1, c1, kSpatSwordX, (2*60), 0);
				
				creature[c1].nextAttackTime=currentTime+180;
				nw_SendDisplayMessage(i, kQuickDrawMsg);
			}
			else
			{
				nw_SendDisplayMessage(i, kNoQuickDrawMsg);
			}
			
			cr_ClearSpat(c1, true);
			return;
		break;
		
		case kAttackLesserDefense:
		case kAttackGreaterDefense:
			nw_SendDisplayMessage(i, kDefenseBegin);
			attackType1=kSpatDamageAbsorb;
			attackType2=2;
		break;
		
		case kAttackRangerSwiftness:
			nw_SendDisplayMessage(i, kRangerSwiftness);
			attackType1=kSpatAttackSpeed;
			attackType2=2;
		break;
	}
	
	if (bOk) //spat passed requirements, check for enough stamina
	{
		if (creature[c1].stamina>=itemList[spat].price)
		{
			if (attackType2==1 || attackType2==2) //pay on activation
			{
				creature[c1].stamina-=itemList[spat].price;
				player[i].sendStatsRefresh=true;	
			}
			
			creature[c1].specialAttackType=spat;
			creature[c1].specialAttackArg1=attackType1;
			creature[c1].specialAttackArg2=attackType2;
		}
		else
		{
			nw_SendDisplayMessage(i, kNotEnoughStaminaMsg);
			nw_SendClearSpat(i);
		}
	}
}

// ----------------------------------------------------------------- //
void pl_Attack(int i, int c) // server
// ----------------------------------------------------------------- //

{
	unsigned long			currentTime     = TickCount();
	int                     j;
	TBoolean                targetInTown    = false;
	TBoolean                attackerInTown  = false;
	TBoolean                canAttackPet;// merge 7/28
	TBoolean                inPVPZone;


	if (player[i].dead)
	{
		creature[player[i].creatureIndex].attackTarget		=	0;
		creature[player[i].creatureIndex].nextAttackTime	= 0;
		return;
	}

	if (player[i].creatureIndex==c)
	{
		creature[player[i].creatureIndex].attackTarget		=	0;
		//creature[player[i].creatureIndex].nextAttackTime	= 0; // merge   6/29/01
		creature[player[i].creatureIndex].nextAttackTime	= currentTime + 250; // merge   6/29/01
		return;
	}

	if (creature[player[i].creatureIndex].attackTarget==c)
	{
		for (j=0;j<kMaxPets;j++)
			if (player[i].pet[j]!=0)
				if (creature[player[i].pet[j]].command==kAttack)
					creature[player[i].pet[j]].attackTarget	 =	c;

		return;
	}

	if (creature[c].master==player[i].creatureIndex) // can't attack pet
		return;

	if (creature[c].playerIndex!=0)
		if (player[creature[c].playerIndex].dead) // can't attack ghost
			return;

	inPVPZone=false;

	if ((creature[player[i].creatureIndex].row>1076) && (creature[player[i].creatureIndex].row<1150))
		if ((creature[player[i].creatureIndex].col>1681) && (creature[player[i].creatureIndex].col<=1765))
			if ((creature[c].row>1076) && (creature[c].row<1150))
				if ((creature[c].col>1681) && (creature[c].col<=1765))
					if ((creature[player[i].creatureIndex].row>1076) && (creature[player[i].creatureIndex].row<1150))
						inPVPZone=true;
						//Mirith Jail
						if ((creature[player[i].creatureIndex].col>960) && (creature[player[i].creatureIndex].col<=967))
							if ((creature[player[i].creatureIndex].col>647) && (creature[player[i].creatureIndex].col<=659))
								if ((creature[c].row>1076) && (creature[c].row<1150))					
									if ((creature[c].row>960) && (creature[c].row<967))
										if ((creature[c].col>647) && (creature[c].col<659))
					inPVPZone=true;

	if (inPVPZone==false)
		if (player[i].god==false)
			//if (player[i].category!=5)	// merge 7/28
			if (pl_AuthorizedTo(player[i].creatureIndex,kIsQuestScoundrel)==false) // add to carbon 11/27
				if (((creature[c].master!=0) || (creature[c].alignment==kGood)) && (pl_IsThief(c)==false)) // pvp off 0.1.3
				{
					canAttackPet=false;// merge 7/28

					if (creature[c].master!=0)
						if (player[creature[creature[c].master].playerIndex].category==5)
						//if (pl_AuthorizedTo(creature[c].master,kIsQuestScoundrel)) // add to carbon 11/27
							canAttackPet=true;

					if (canAttackPet==false)
					{
						nw_SendDisplayMessage(i,kCantAttackCreature);
						return;
					}
				}// merge 7/28

	if ( player[i].category!=5 && (creature[c].id==kCentaur || creature[c].id==kElderCentaur) ) //02/10/2005 - can't attack centaurs?
	{
		nw_SendDisplayMessage(i,kCantAttackCreature);
		return;
	}

//	if (((creature[c].master!=0) || (creatureInfo[creature[c].id].alignment==kGood)) && (pl_IsThief(c)==false))
//	  {
//	    if ((creature[c].row>2075) && (creature[c].row<2310) && (creature[c].col>1749) && (creature[c].col<2001))
//	      targetInTown=true;
//	    if ((creature[player[i].creatureIndex].row>2075) && (creature[player[i].creatureIndex].row<2310) && (creature[player[i].creatureIndex].col>1749) && (creature[player[i].creatureIndex].col<2001))
//	      attackerInTown=true;
//	    if ((targetInTown) || (attackerInTown))
//	      {
//		      nw_SendDisplayMessage(i,kCantAttackInTown);
//		      return;
//		    }
//	  }

	if (creature[player[i].creatureIndex].attackTarget==0)
	{
		if (creature[player[i].creatureIndex].nextAttackTime==0)
			creature[player[i].creatureIndex].nextAttackTime	= currentTime + 30;
	}
	else
	{
		if (player[i].arm[kRightHand]==0)
			creature[player[i].creatureIndex].nextAttackTime = currentTime + 250;
		else
			creature[player[i].creatureIndex].nextAttackTime	= currentTime + itemList[creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID].weaponSpeed;
	}

	creature[player[i].creatureIndex].attackTarget=c;

	if (creature[c].attackTarget==0)  // auto retailiate
		if (player[i].currentlyAttacking<player[i].maxAttackers)
			if (creatureInfo[creature[c].id].attackRange>0)	// mod 8/12 structures - shelters, towers, etc
				if (creature[c].playerIndex==0)// || inPVPZone || pl_AuthorizedTo(c,kIsQuestScoundrel) || pl_AuthorizedTo(player[i].creatureIndex,kIsQuestScoundrel))
					if (!player[i].stealth)
					{
						creature[c].attackTarget=player[i].creatureIndex;
						creature[c].nextAttackTime	= currentTime + 30;
					}

	//if (creature[player[i].creatureIndex].nextAttackTime==0)
	//	creature[player[i].creatureIndex].nextAttackTime	= currentTime + 60;
	//if (player[i].arm[kRightHand]==0)
	//	creature[player[i].creatureIndex].nextAttackTime = currentTime + 250;
	//else
	//	creature[player[i].creatureIndex].nextAttackTime	= currentTime + itemList[creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID].weaponSpeed;

	for (j=0;j<kMaxPets;j++)
		if (player[i].pet[j]!=0)
			if (creature[player[i].pet[j]].command==kAttack)
				creature[player[i].pet[j]].attackTarget	 =	c;

	for (j=0;j<kMaxPets;j++)
		if (player[i].pet[j]!=0)
			if ((creature[player[i].pet[j]].command==kGuard) && (creature[player[i].pet[j]].attackTarget==0))
				creature[player[i].pet[j]].attackTarget	 =	c;

}

// ----------------------------------------------------------------- //
void pl_CheckForActionInterrupted(int i, TBoolean move) // server
// ----------------------------------------------------------------- //

{

	if (creature[player[i].creatureIndex].finalStrikeSound!=0)
	{
		cr_AddSound(player[i].creatureIndex, creature[player[i].creatureIndex].finalStrikeSound);
		//creature[player[i].creatureIndex].soundFX				    = creature[player[i].creatureIndex].finalStrikeSound;
		//creature[player[i].creatureIndex].soundFXTime		    = TickCount();
		creature[player[i].creatureIndex].finalStrikeSound  = 0;
	}

	creature[player[i].creatureIndex].nextStrikeTime		=	0;

	creature[player[i].creatureIndex].strikePose				=	0;
	creature[player[i].creatureIndex].numberOfStrikes		=	0;
	creature[player[i].creatureIndex].hiding						=	false;

	if (pl_AuthorizedTo(player[i].creatureIndex, kRegularPlayer) && player[i].stealth)
		if (!move)
		{
			nw_SendDisplayMessage(i, kLeavingStealth);
			player[i].stealth = false;
			player[i].stealthCool = TickCount()+3*60;
		}


	if (player[i].gatherType==255) // player interrupted making item
		it_FailedMake(i);

	else if (player[i].gathering!=0) // player interrupted resource harvest
		rs_FailedHarvest(i);

	else if (player[i].meditating) // player interrupted meditating
	{
		player[i].meditating	=	false;
		nw_SendDisplayMessage(i,kConcentrationDisturbed);
	}

//	else if (player[i].healing) // player interrupted healing
//		{
//			player[i].healing	=	false;
//			nw_SendDisplayMessage(i,kBandagesFellOff);
//		}

}

// ----------------------------------------------------------------- //
void pl_PlayerEat(int i) // server
// ----------------------------------------------------------------- //

{

	if (creature[player[i].creatureIndex].hiding)
		creature[player[i].creatureIndex].hiding=false;

	if (pl_AuthorizedTo(player[i].creatureIndex, kRegularPlayer))
		if (player[i].stealth)
		{
			nw_SendDisplayMessage(i, kLeavingStealth);
			player[i].stealth = false;
			player[i].stealthCool = TickCount()+3*60;
		}

	creature[player[i].creatureIndex].hunger=creature[player[i].creatureIndex].hunger+1200;

	if (creature[player[i].creatureIndex].hunger>7000)
		creature[player[i].creatureIndex].hunger=7000;

	if (creature[player[i].creatureIndex].hunger<1750)
		nw_SendDisplayMessage(i,kStillHungry);
	else if (creature[player[i].creatureIndex].hunger<3500)
		nw_SendDisplayMessage(i,kLessHungry);
	else if (creature[player[i].creatureIndex].hunger<5250)
		nw_SendDisplayMessage(i,kFeelingFull);
	else if (creature[player[i].creatureIndex].hunger<7000)
		nw_SendDisplayMessage(i,kVeryFull);
	else
		nw_SendDisplayMessage(i,kCantEatMore);

	player[i].sendStatsRefresh=true;

}

// ----------------------------------------------------------------- //
void pl_PlayerDrinkPotion(int i, int potionType) // server
// ----------------------------------------------------------------- //

{
	//int						j;
	//int						fxIndex;
	int						effect;
	int						added;
	int 					skill;
	

	pl_CheckForActionInterrupted(i, false);

	if ((player[i].dead) || (creature[player[i].creatureIndex].dieTime!=0))
		return;

	if (potionType==kLesserHealPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);

		if (creature[player[i].creatureIndex].hitPoints<creature[player[i].creatureIndex].strength)
		{
			creature[player[i].creatureIndex].hitPoints=creature[player[i].creatureIndex].hitPoints+tb_Rnd(3,12);

			if (creature[player[i].creatureIndex].hitPoints>creature[player[i].creatureIndex].strength)
				creature[player[i].creatureIndex].hitPoints=creature[player[i].creatureIndex].strength;

			player[i].sendStatsRefresh=true;
		}
	}
	else if (potionType==kGreaterHealPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);

		if (creature[player[i].creatureIndex].hitPoints<creature[player[i].creatureIndex].strength)
		{
			creature[player[i].creatureIndex].hitPoints=creature[player[i].creatureIndex].hitPoints+tb_Rnd(12,35);

			if (creature[player[i].creatureIndex].hitPoints>creature[player[i].creatureIndex].strength)
				creature[player[i].creatureIndex].hitPoints=creature[player[i].creatureIndex].strength;

			player[i].sendStatsRefresh=true;
		}
	}
	else if (potionType==kFullHealPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);

		if (creature[player[i].creatureIndex].hitPoints<creature[player[i].creatureIndex].strength)
		{
			creature[player[i].creatureIndex].hitPoints=creature[player[i].creatureIndex].strength;
			player[i].sendStatsRefresh=true;
		}
	}
	else if (potionType==kManaPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);

		if (creature[player[i].creatureIndex].magicPoints<creature[player[i].creatureIndex].intelligence)
		{
			creature[player[i].creatureIndex].magicPoints=creature[player[i].creatureIndex].magicPoints+tb_Rnd(4,25);

			if (creature[player[i].creatureIndex].magicPoints>creature[player[i].creatureIndex].intelligence)
				creature[player[i].creatureIndex].magicPoints=creature[player[i].creatureIndex].intelligence;

			player[i].sendStatsRefresh=true;
		}
	}

	else if (potionType==kLesserDetoxifyPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);
		effect=tb_Rnd(1,3);

		if (effect>creature[player[i].creatureIndex].poison)
			effect=creature[player[i].creatureIndex].poison;

		creature[player[i].creatureIndex].poison=creature[player[i].creatureIndex].poison-effect;
	}

	else if (potionType==kGreaterDetoxifyPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);
		effect=tb_Rnd(3,7);

		if (effect>creature[player[i].creatureIndex].poison)
			effect=creature[player[i].creatureIndex].poison;

		creature[player[i].creatureIndex].poison=creature[player[i].creatureIndex].poison-effect;
	}

	else if (potionType==kFullDetoxifyPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);
		creature[player[i].creatureIndex].poison=0;
	}

	else if (potionType==kNightVisionPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);
		nw_SendNightVision(i);
	}
	else if (potionType==kInvisibilityPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);
		creature[player[i].creatureIndex].hiding=true;
		player[i].sendStatsRefresh=true;
	}
	else if (potionType==kOpportunityPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);
		creature[player[i].creatureIndex].opportunityTime=TickCount()+4*60;
	}
	else if (potionType==kNourishPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);
		creature[player[i].creatureIndex].hunger=7000;
		player[i].sendStatsRefresh=true;
	}
	else if (potionType==kStrengthPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);
		creature[player[i].creatureIndex].strengthOffset	=	tb_Round((float)creature[player[i].creatureIndex].strength*0.3);
		creature[player[i].creatureIndex].strengthTime		=	TickCount()+(3600);
		player[i].sendStatsRefresh=true;
	}
	else if (potionType==kElevateMindPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);
		creature[player[i].creatureIndex].intelligenceOffset	=	tb_Round((float)creature[player[i].creatureIndex].intelligence*0.3);
		creature[player[i].creatureIndex].intelligenceTime		=	TickCount()+(3600);
		player[i].sendStatsRefresh=true;
	}
	else if (potionType==kAgilityPotion)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle, (5*60), 0);
		creature[player[i].creatureIndex].dexterityOffset	=	tb_Round((float)creature[player[i].creatureIndex].dexterity*0.3);
		creature[player[i].creatureIndex].dexterityTime		=	TickCount()+(3600);
		player[i].sendStatsRefresh=true;
	}

	//Dave 01-04-2013
	/*
	else if (potionType==kMaraliWhiskey)	//-------------------------------------------------------------------------
	{
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle3, (5*60), 0);
		//nw_SendNightVision(i);
	}
	*/
	
	//Dave reward books
	/*
//Skill rewards
	
	
	//c1=player[p].creatureIndex;
	else if (potionType==kClericReward)	// Cleric
		if (localClass==1)
		if (player[i].characterClass==1) 
			{
			sk_RaiseSkill(i,0,kHealing,10);
			sk_RaiseSkill(i,0,kMageryCleric,10);
			sk_RaiseSkill(i,0,kAlchemyCleric,10);
			sk_TrainProof(i, 0, 0, kMageryCleric, 10);
			gr_AddText("You feel more experienced.",true,true);
			}
		else
			{
			gr_AddText("You must be a cleric to read this book.", true,true);
			}
	else if (potionType==kWizardReward)	// Wizard
		if (player[i].characterClass==6) 
			{
			sk_RaiseSkill(i,0,kEnchanting,10);
			sk_RaiseSkill(i,0,kMageryWizard,10);
			sk_RaiseSkill(i,0,kMeditation,10);
			gr_AddText("You feel more experienced.",true,true);
			}
		else
			{
			gr_AddText("You must be a wizard to read this book.", true,true);
			}
	else if (potionType==kDruidReward)	// Druid
		if (player[i].characterClass==2) 
			{
			sk_RaiseSkill(i,0,kHealing,10);
			sk_RaiseSkill(i,0,kMageryCleric,10);
			sk_RaiseSkill(i,0,kAlchemyCleric,10);
			gr_AddText("You feel more experienced.",true,true);
			}
		else
			{
			gr_AddText("You must be a druid to read this book.", true,true);
			}
	else if (potionType==kFighterReward)	// Fighter
		if (player[i].characterClass==3) 
			{
			sk_RaiseSkill(i,0,kMelee,10);
			sk_RaiseSkill(i,0,kParrying,10);
			sk_RaiseSkill(i,0,kSpecialFighter,10);
			gr_AddText("You feel more experienced.",true,true);
			}
		else
			{
			gr_AddText("You must be a fighter to read this book.", true,true);
			}
	else if (potionType==kRangerReward)	// Ranger
		if (player[i].characterClass==4) 
			{
			sk_RaiseSkill(i,0,kArchery,10);
			sk_RaiseSkill(i,0,kTracking,10);
			sk_RaiseSkill(i,0,kSpecialRanger,10);
			gr_AddText("You feel more experienced.",true,true);
			}
		else
			{
			gr_AddText("You must be a ranger to read this book.", true,true);
			}
	else if (potionType==kRogueReward)	// Rogue
		if (player[i].characterClass==5) 
			{
			sk_RaiseSkill(i,0,kAnatomy,10);
			sk_RaiseSkill(i,0,kStealth,10);
			sk_RaiseSkill(i,0,kSpecialRogue,10);
			gr_AddText("You feel more experienced.",true,true);
			}
		else
			{
			gr_AddText("You must be a rogue to read this book.", true,true);
			}

// might work
//sk_RaiseSkill(i,0,skill,itemList[item].makeSkillLevel);
		//return (true);
*/


	added=in_AddtoInventory(player[i].creatureIndex, kEmptyBottle, 1);

	if (added>0)
		nw_SendInventoryAdd(player[i].creatureIndex, kEmptyBottle, added, kNoMessage, 0);
	else
		in_DropItem(0,1,kEmptyBottle, 1, creature[player[i].creatureIndex].row, creature[player[i].creatureIndex].col, 10, false, "");

}

// ----------------------------------------------------------------- //
void pl_SendMacroWarning(int i) // server
// ----------------------------------------------------------------- //

{
	int               k;
	char              gmText[kStringLength];


	if (player[i].lastSkillGain!=0)
		if ((player[i].lastSkillGain+(3600*10))<TickCount())
			return;

	player[i].lastMove=TickCount()-(3600*30);

	for (k=0;k<kMaxPlayers;k++)
		// if (player[k].category!=0)
		if (pl_AuthorizedTo(player[k].creatureIndex,kCanSeeMacroSuspects)) // add to carbon 11/27
		{
			player[i].lastMove=TickCount();
			strcpy(gmText, "Macro Suspect:  ");
			strcat(gmText, player[i].playerName);
			nw_SendSystemTextToClient(k, gmText);
		}

}

// ----------------------------------------------------------------- //
TBoolean pl_PlayerMove(int i, int direction) // server
// ----------------------------------------------------------------- //

{
	UInt16												row;
	UInt16												col;
	TBoolean											doMove;
	int													rowDir;
	int													colDir;
	int													j;//, k;
	UInt16												pRow;
	UInt16												pCol;
	unsigned long										currentTime;
	TBoolean											steppedOnLatter;
	char                          						signStr[kStringLength], nameStr[kStringLength];
	int													bumped=0;
	int													c;



	if (player[i].online==false)
		return (false);

	currentTime=TickCount();

	pl_CheckForActionInterrupted(i, true);
	c=player[i].creatureIndex;
	
	//if (creature[player[i].creatureIndex].attackTarget==0)
		if (creature[c].specialAttackType!=-1 && creature[c].specialAttackArg2>1) //only spats that break on movement
			cr_ClearSpat(c, true);

	if (player[i].category==0 || player[i].category==8)
		player[i].lastMove=currentTime;

	if (creature[player[i].creatureIndex].disorientation!=0)
	{
		if (creature[player[i].creatureIndex].disorientation>currentTime)
			direction=tb_Rnd(1,8);
		else
			creature[player[i].creatureIndex].disorientation=0;
	}

	if (creature[player[i].creatureIndex].paralyzed!=0)
	{
		if (creature[player[i].creatureIndex].paralyzed>currentTime)
			return (false);
		else
			creature[player[i].creatureIndex].paralyzed=0;
	}

	row=creature[player[i].creatureIndex].row;

	col=creature[player[i].creatureIndex].col;

	if (direction==1) 		 {	row++;	creature[player[i].creatureIndex].facing=5; }
	else if (direction==2) {	row--;	creature[player[i].creatureIndex].facing=1; }
	else if (direction==3) {	col--;	creature[player[i].creatureIndex].facing=7; }
	else if (direction==4) {	col++;	creature[player[i].creatureIndex].facing=3; }
	else if (direction==5) {	row++; col++; creature[player[i].creatureIndex].facing=4; }
	else if (direction==7) {	row++; col--; creature[player[i].creatureIndex].facing=6; }
	else if (direction==6) {	row--; col++; creature[player[i].creatureIndex].facing=2; }
	else if (direction==8) {	row--; col--; creature[player[i].creatureIndex].facing=0; }

	rowDir=row-creature[player[i].creatureIndex].row;

	colDir=col-creature[player[i].creatureIndex].col;

	if (player[i].dead)
		if (doorway[map[row][col]])
		{
			if (direction==1) 		 {	row++;	creature[player[i].creatureIndex].facing=5; }
			else if (direction==2) {	row--;	creature[player[i].creatureIndex].facing=1; }
			else if (direction==3) {	col--;	creature[player[i].creatureIndex].facing=7; }
			else if (direction==4) {	col++;	creature[player[i].creatureIndex].facing=3; }
			else if (direction==5) {	row++; col++; creature[player[i].creatureIndex].facing=4; }
			else if (direction==7) {	row++; col--; creature[player[i].creatureIndex].facing=6; }
			else if (direction==6) {	row--; col++; creature[player[i].creatureIndex].facing=2; }
			else if (direction==8) {	row--; col--; creature[player[i].creatureIndex].facing=0; }
		}

	doMove=false; // put in carbon 11/27

//#ifdef _DEBUGGING
//  doMove=true;
//#endif

	if (player[i].stealth) // put in carbon 11/27
		if (pl_AuthorizedTo(player[i].creatureIndex, kRegularPlayer)==false)
		{
			doMove=true;

			if (creatureMap[row][col]!=0)
				doMove=false;
		}

	if (row<0) 								row	=	0; // put in carbon 11/27
	else if (row>=kMapWidth) 	row	=	kMapWidth-1;

	if (col<0) 								col	=	0;
	else if (col>=kMapHeight)	col	=	kMapHeight-1;


	//if (player[i].category==1)	// type 1 player ... can walk through obstacles
	//	doMove=true;

	if (creatureInfo[creature[player[i].creatureIndex].id].domain==kLand)
	{
		if ((creatureMap[row][col]	== 0) && (canWalkOn[map[row][col]]))
			doMove=true;
	}
	else if (creatureInfo[creature[player[i].creatureIndex].id].domain==kAir)
	{
		if ((creatureMap[row][col]	== 0) && (canFlyOver[map[row][col]]))
			doMove=true;
	}

	if ((creatureMap[row][col]	== 0) && (canSwimIn[map[row][col]]))
		if (in_QuantityOf_s(player[i].creatureIndex,kRaft)>0)
			doMove=true;

	if (player[i].category==0 || player[i].category==8)
		if (containerMap[row][col]!=0)
			if (gmProp[containerID[containerMap[row][col]]])
				doMove=false;

	steppedOnLatter=false;

	if ((map[row][col]==395) || 
			(map[row][col]==908) || 
				(map[row][col]==900) || 
					(map[row][col]==901) || 
						(map[row][col]==1009) || 
							(map[row][col]==1010)  || 
								(map[row][col]==1109))	// stepped on latter
	
	
	
	{
		pRow=row;
		pCol=col;
		cr_SteppedOnLatter(player[i].creatureIndex,&row,&col);

		if ((row<0) || (col<0))
			doMove=false;
		//Dave
		if (map[row][col]==1109)
			nw_SendDisplayMessage(i,kBeware);
		else
		{
			creature[player[i].creatureIndex].attackTarget=0;
			steppedOnLatter=true;
			creature[player[i].creatureIndex].hiding=true;
			nw_SendRefresh(i,true);
			creature[player[i].creatureIndex].hiding=false;
		
	
		
		}
	
	
	}

	if (player[i].category==0 || player[i].category==8)
		if (guildMap[row][col]!=0)
			if (strcmp(guildHalls[guildMap[row][col]].guild, "all")!=0)
				if (strcmp(guildHalls[guildMap[row][col]].guild, "All")!=0)
					if (strcmp(player[i].guildName, guildHalls[guildMap[row][col]].guild)!=0)
						doMove=false;

	if (doMove)
	{

		if (player[i].dead==false)
		{
			if (creature[player[i].creatureIndex].hitPoints<((float)creature[player[i].creatureIndex].strength*0.10))
			{
				cr_DropBloodStain(creature[player[i].creatureIndex].id,creature[player[i].creatureIndex].row,creature[player[i].creatureIndex].col);
			}
			else if (creature[player[i].creatureIndex].hitPoints<((float)creature[player[i].creatureIndex].strength*0.30))
			{
				if (tb_Rnd(1,2)==1)
					cr_DropBloodStain(creature[player[i].creatureIndex].id,creature[player[i].creatureIndex].row,creature[player[i].creatureIndex].col);
			}
			else if (creature[player[i].creatureIndex].hitPoints<((float)creature[player[i].creatureIndex].strength*0.50))
			{
				if (tb_Rnd(1,3)==1)
					cr_DropBloodStain(creature[player[i].creatureIndex].id,creature[player[i].creatureIndex].row,creature[player[i].creatureIndex].col);
			}
			else if (creature[player[i].creatureIndex].hitPoints<((float)creature[player[i].creatureIndex].strength*0.75))
			{
				if (tb_Rnd(1,4)==1)
					cr_DropBloodStain(creature[player[i].creatureIndex].id,creature[player[i].creatureIndex].row,creature[player[i].creatureIndex].col);
			}
		}

		for (j=(kMaxTrail-1);j>0;j--)
		{
			player[i].rowTrail[j]	=	player[i].rowTrail[j-1];
			player[i].colTrail[j]	=	player[i].colTrail[j-1];
		}

		player[i].rowTrail[0]	=	row;

		player[i].colTrail[0]	=	col;

		creature[player[i].creatureIndex].hiding	=	false;

		if ((rowDir==-1) && (colDir==-1)) 			creature[player[i].creatureIndex].moveDirection=0;
		else if ((rowDir==-1) && (colDir==0)) 	creature[player[i].creatureIndex].moveDirection=1;
		else if ((rowDir==-1) && (colDir==1)) 	creature[player[i].creatureIndex].moveDirection=2;
		else if ((rowDir==0) && (colDir==-1)) 	creature[player[i].creatureIndex].moveDirection=3;
		else if ((rowDir==0) && (colDir==0)) 		creature[player[i].creatureIndex].moveDirection=4;
		else if ((rowDir==0) && (colDir==1)) 		creature[player[i].creatureIndex].moveDirection=5;
		else if ((rowDir==1) && (colDir==-1)) 	creature[player[i].creatureIndex].moveDirection=6;
		else if ((rowDir==1) && (colDir==0)) 		creature[player[i].creatureIndex].moveDirection=7;
		else if ((rowDir==1) && (colDir==1)) 		creature[player[i].creatureIndex].moveDirection=8;

		creatureMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col]	=	0;

		creature[player[i].creatureIndex].row=row;

		creature[player[i].creatureIndex].col=col;

		creatureMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col]	=	player[i].creatureIndex;

		creature[player[i].creatureIndex].timeStamp			= TickCount();

		if (sign[map[row][col]])	// standing on sign
		{
			fi_LoadSignMessage(row, col, signStr);

			if ((strcmp(signStr,"")==0) && (map[row][col]==1048))
				nw_SendDisplayMessage(i,kNoOwner);
			else
				nw_SendTextToClient(i, signStr, 0);
		}

		if (tracksMap[row][col]!=0)	// standing on track
		{
			if (strcmp(tracksNote[tracksMap[row][col]], "")!=0) // track has a note
				if (!player[i].dead)	// ghosts can't see notes
					if (in_TracksOwner(i))
						nw_SendTextToClient(i, tracksNote[tracksMap[row][col]], 0);
		}

		if (player[i].dead || player[i].curse!=0)
		{
			for (j=0;j<kMaxTemples;j++) // standing in temple
				if (templeRect[j].right!=0)
					if ((creature[player[i].creatureIndex].row>=templeRect[j].left) && (creature[player[i].creatureIndex].row<=templeRect[j].right))
						if ((creature[player[i].creatureIndex].col>=templeRect[j].top) && (creature[player[i].creatureIndex].col<=templeRect[j].bottom))
						{
							if (player[i].dead)
							{
								player[i].dead=false;
								player[i].sendStatsRefresh=true;
								creature[player[i].creatureIndex].hitPoints=25;

								if (creature[player[i].creatureIndex].strength<creature[player[i].creatureIndex].hitPoints)
									creature[player[i].creatureIndex].hitPoints=creature[player[i].creatureIndex].strength;

								creature[player[i].creatureIndex].magicPoints=25;

								if (creature[player[i].creatureIndex].intelligence<creature[player[i].creatureIndex].magicPoints)
									creature[player[i].creatureIndex].magicPoints=creature[player[i].creatureIndex].intelligence;
							}

							//if (player[i].curse!=0) //Temple decurse
							//player[i].curse=0;

							break;
						}
		}


		if (steppedOnLatter)
		{

			creature[player[i].creatureIndex].pathPtr=0;

			for (j=0;j<kMaxPath;j++)
			{
				creature[player[i].creatureIndex].pathRow[j]	=	0;
				creature[player[i].creatureIndex].pathRow[j]	=	0;
			}

			for (j=0;j<kMaxPets;j++)
				if (player[i].pet[j]!=0)
				{
					if (cr_DistanceFrom(player[i].pet[j], pRow, pCol)<20)
					{
						row=pRow;
						col=pCol;
						cr_SteppedOnLatter(player[i].pet[j],&row,&col);

						if ((row>0) && (col>0))
						{
							creatureMap[creature[player[i].pet[j]].row][creature[player[i].pet[j]].col]	= 0;
							creature[player[i].pet[j]].row					=	(UInt16)row;
							creature[player[i].pet[j]].col					=	(UInt16)col;
							creatureMap[row][col]										= player[i].pet[j];
							creature[player[i].pet[j]].nextMoveTime	=	currentTime+creatureInfo[creature[player[i].pet[j]].id].speed;
							creature[player[i].pet[j]].timeStamp		= currentTime;
							creature[player[i].pet[j]].attackTarget	=	0;
							creature[player[i].pet[j]].trackRow			=	0;
							creature[player[i].pet[j]].trackCol			=	0;
							creature[player[i].pet[j]].tracking			=	false;
						}
						else
						{
							creature[player[i].pet[j]].attackTarget=0;
							creature[player[i].pet[j]].trackRow=pRow;
							creature[player[i].pet[j]].trackCol=pCol;
							creature[player[i].pet[j]].tracking=true;
						}
					}
					else
					{
						creature[player[i].pet[j]].attackTarget=0;
						creature[player[i].pet[j]].trackRow=pRow;
						creature[player[i].pet[j]].trackCol=pCol;
						creature[player[i].pet[j]].tracking=true;
					}
				}
		}


		nw_SendRefresh(i,true);

		player[i].skipNextRefresh	= true;

		return (true);
	}
	else
	{
		if (creatureMap[row][col]!=0)
		{
			if (creature[creatureMap[row][col]].playerIndex!=0)
			{
				if (!(player[i].stealth && pl_AuthorizedTo(player[i].creatureIndex, kRegularPlayer)==false))
				{
					j=creature[creatureMap[row][col]].playerIndex;

					if (player[i].stealth && pl_AuthorizedTo(player[i].creatureIndex, kRegularPlayer) && pl_AuthorizedTo(creatureMap[row][col], kRegularPlayer))
					{
						player[i].stealth		=	false;
						player[i].stealthCool	=	TickCount()+3*60;
						bumped=i;
					}

					if (player[j].stealth && pl_AuthorizedTo(creatureMap[row][col], kRegularPlayer) && pl_AuthorizedTo(player[i].creatureIndex, kRegularPlayer))
					{
						player[j].stealth		=	false;
						player[j].stealthCool	=	TickCount()+3*60;
						bumped=j;
					}

					if (bumped!=0)
					{
						strcpy(signStr, "You bumped into ");

						strcpy(nameStr, player[j].playerName);

						if (strcmp(player[j].playerNameTemp, "none")!=0)
							if (strcmp(player[j].playerNameTemp, "None")!=0)
								strcpy(nameStr, player[j].playerNameTemp);

						strcat(signStr, nameStr);

						strcat(signStr, ".");

						nw_SendTextToClient(i, signStr, 0);

						strcpy(signStr, "");

						strcpy(nameStr, player[i].playerName);

						if (strcmp(player[i].playerNameTemp, "none")!=0)
							if (strcmp(player[i].playerNameTemp, "None")!=0)
								strcpy(nameStr, player[j].playerNameTemp);

						strcat(signStr, nameStr);

						strcat(signStr, " bumped into you.");

						nw_SendTextToClient(j, signStr, 0);

						nw_SendDisplayMessage(bumped,kLeavingStealth);
					}
				}
			}
			else if (creature[creatureMap[row][col]].npcIndex!=0)
				npc[creature[creatureMap[row][col]].npcIndex].forceMove=true;
		}
	}

	return (false);

}

// ----------------------------------------------------------------- //
void pl_PlayerFaceDirection(int i, int direction) // server
// ----------------------------------------------------------------- //

{

	if (direction==kFace1) 		  creature[player[i].creatureIndex].facing=5;
	else if (direction==kFace2) creature[player[i].creatureIndex].facing=1;
	else if (direction==kFace3) creature[player[i].creatureIndex].facing=7;
	else if (direction==kFace4) creature[player[i].creatureIndex].facing=3;
	else if (direction==kFace5) creature[player[i].creatureIndex].facing=4;
	else if (direction==kFace6) creature[player[i].creatureIndex].facing=6;
	else if (direction==kFace7) creature[player[i].creatureIndex].facing=2;
	else if (direction==kFace8) creature[player[i].creatureIndex].facing=0;

}

// ----------------------------------------------------------------- //
void pl_StablePets(int i)
// ----------------------------------------------------------------- //

{
	int					j;

	for (j=0;j<kMaxPets;j++)
		pl_StablePet(i,j);

}

// ----------------------------------------------------------------- //
void pl_StablePet(int i, int j)
// ----------------------------------------------------------------- //

{
	int					c;
	int					k;
	
	TBoolean 				canStable=true;
	int						type, exp;
	char					name[kStringLength];

	if ((player[i].pet[j]!=0) && (player[i].petInStable[j]==false))
	{
		if (cr_InPVPZone(player[i].pet[j])==false) //bypass the following checks in pvp
		{
			if (creature[player[i].pet[j]].attackTarget!=0)		canStable=false; //can't stable if your pet is attacking something
		
			for (c=creature[player[i].pet[j]].row-4; c<creature[player[i].pet[j]].row+4; c++)
			{
				for (k=creature[player[i].pet[j]].col-4;k<creature[player[i].pet[j]].col+4;k++)
				{
					if (c<0 || k<0 || c>kMapWidth || k>kMapHeight)	continue;
					
					if (creatureMap[c][k]!=0)
					{
						if (creature[creatureMap[c][k]].attackTarget==player[i].pet[j]) //can't stable pet if under attack
						{
							canStable=false;
						}
					}
				}
			}
		}		
		
		if (creatureInfo[creature[player[i].pet[j]].id].domain!=kWater && creature[player[i].pet[j]].lifespan==0)  //Can't stable water and summoned creatures
		{
			if (!canStable)
			{
				if (player[i].online==false) //handle pet banishing when master disconnects
				{
					type=player[i].petType[j];
					exp=creature[player[i].pet[j]].petExp;
					strcpy(name, player[i].petName[j]);
					
					pl_BanishCreature(-1, player[i].pet[j]);
					
					player[i].petInStable[j]=true;
					player[i].petType[j]=type;
					player[i].petExperience[j]=exp;
					player[i].petHealth[j]=1;
					strcpy(player[i].petName[j], name);
				}
				
				return;
			}
			
			c=player[i].pet[j];
			player[i].petInStable[j]=true;
			player[i].petType[j]=creature[c].id;
			player[i].pet[j]=0;
			player[i].petExperience[j]=creature[c].petExp;
			player[i].petHealth[j]=creature[c].hitPoints;

			for (k=0;k<kMaxCreatureEffects;k++)
				if (creature[c].fx[k]>0)
				{
					serverFX[creature[c].fx[k]]	=	0;
					creature[c].fx[k]						=	0;
					creature[c].fxKillTime[k]		=	0;
					creature[c].fxTarget[k]			=	0;
					creature[c].fxTargetRow[k]	=	0;
					creature[c].fxTargetCol[k]	=	0;
					creature[c].activeFX				= false;
				}

			creatureInfo[creature[c].id].population--;

			creatureMap[creature[c].row][creature[c].col]	= 0;
			cr_ClearCreatureRecord(c);
		}
		else
		{
			c=player[i].pet[j];
			player[creature[c].master].pet[j]					  =	0;
			player[creature[c].master].petInStable[j]		= false;
			player[creature[c].master].petType[j]				= 0;
			player[creature[c].master].petExperience[j]				= 0;
			player[creature[c].master].petHealth[j]				=0;
			creature[c].master				                  =	0;
			creature[c].attackTarget	                  =	0;
			creature[c].trackRow			                  =	0;
			creature[c].trackCol			                  =	0;
			creature[c].tracking			                  =	false;
			creature[c].alignment							= kEvil;
		}	
	}
}

// ----------------------------------------------------------------- //
TBoolean pl_PlayerInStable(int i) // server
// ----------------------------------------------------------------- //

{
	int								j;

	for (j=0;j<kMaxStables;j++)
		if (stableRect[j].right!=0)
		{
			if ((creature[player[i].creatureIndex].row>=stableRect[j].left) && (creature[player[i].creatureIndex].row<=stableRect[j].right))
				if ((creature[player[i].creatureIndex].col>=stableRect[j].top) && (creature[player[i].creatureIndex].col<=stableRect[j].bottom))
					return (true);
		}
		else
			break;

	return (false);

	//if (strcmp(player[i].guildName,"MariRangers")==0)
	//  if ((creature[player[i].creatureIndex].row>2278) && (creature[player[i].creatureIndex].row<2297)) // mariranger guild hall
	//    if ((creature[player[i].creatureIndex].col>1915) && (creature[player[i].creatureIndex].col<1942))
	//         return (true);

	if (creature[player[i].creatureIndex].row<2159)
		return (false);

	if (creature[player[i].creatureIndex].row>2171)
		return (false);

	if (creature[player[i].creatureIndex].col<1775)
		return (false);

	if (creature[player[i].creatureIndex].col>1807)
		return (false);

	return (true);

}

// ----------------------------------------------------------------- //
void pl_PetCommand(int	i, char	playerText[kStringLength])
// ----------------------------------------------------------------- //

{
	int								j;
	int								k;
	int								command;
	//int								c;
	char							commandText[kStringLength];
	TBoolean          commanding[kMaxPets];
	char              word1[kStringLength];
	char              word2[kStringLength];
	char              word3[kStringLength];
	TBoolean          spaceFound=false;
	char              petName[kStringLength];
	TBoolean          found;
	int               level=sk_GetSkillLevel(player[i].skill[kTaming].level);
	int               currentlyControlling;
	TBoolean  			  SendRefresh=false;
	int				cpt;

	if (level>3)
		level=3;

	for (j=0;j<kMaxPets;j++)
		commanding[j]=true;

	command=-1;

	strcpy(commandText,playerText);

	strcpy(word1,"");

	strcpy(word2,"");

	k=0;

	for (j=0;j<strlen(commandText);j++)
	{
		if (((int)commandText[j]>=65) && ((int)commandText[j]<=90)) // make command lower case
			commandText[j]=(char)((int)commandText[j]+32);

		if (commandText[j]==' ')
			spaceFound=true;

		if (spaceFound==false)
		{
			word1[j]=commandText[j];
			word1[j+1]='\0';
		}
		else
		{
			word2[k]=commandText[j];
			word2[k+1]='\0';
			k++;
		}
	}

	if (strcmp(word1,"")!=0)
		tb_DeleteStringPadding(word1);

	if (strcmp(word1,"")!=0)
		tb_DeleteStringPadding(word2);

	if (strcmp(word2,"")==0)
		strcpy(word2,word1);
	else
	{
		if (pl_IsPetCommand(word1))
		{
			strcpy(word3,word1);
			strcpy(word1,word2);
			strcpy(word2,word3);
		}
		else
			return; //disallowing "none /free", etc.

		for (j=0;j<kMaxPets;j++)
			commanding[j]=false;

		for (j=0;j<kMaxPets;j++)
		{
			strcpy(petName,player[i].petName[j]);
			tb_MakeLowerCase(petName);

			if (strcmp(petName,word1)==0)
				commanding[j]=true;
		}
	}

	found=false;

	for (j=0;j<kMaxPets;j++)
		if (commanding[j])
		{
			found=true;
			break;
		}

	if (found==false)
		return;

	player[i].sendPetsRefresh=true;

	currentlyControlling=0;

	for (j=0;j<kMaxPets;j++)
		if (player[i].pet[j]!=0)
			if (player[i].petInStable[j]==false)
				currentlyControlling++;

	if (strcmp(word2,"stable")==0)	// place pets in stable
	{
		if (pl_PlayerInStable(i))
			for (j=0;j<kMaxPets;j++)
				if (commanding[j])
					pl_StablePet(i,j);

		return;
	}

	if (strcmp(word2,"claim")==0)	// claim pets from stable
		if (pl_PlayerInStable(i))
		{
			for (j=0;j<kMaxPets;j++)
				if (commanding[j])
					if (player[i].petInStable[j])
						if (currentlyControlling==0 || pl_AuthorizedTo(player[i].creatureIndex, kRegularPlayer)==false)
						{
							player[i].pet[j]=cr_SpawnCreature(player[i].petType[j], creature[player[i].creatureIndex].row+tb_Rnd(0,5)-3, creature[player[i].creatureIndex].col+tb_Rnd(0,5)-3);

							if (player[i].pet[j]==-1)
							{
								player[i].pet[j]=0;
								continue;
							}

							player[i].petInStable[j]							=	false;

							creature[player[i].pet[j]].master			=	player[i].creatureIndex;
							creature[player[i].pet[j]].command		 =	kHeel;
							creature[player[i].pet[j]].alignment	 =	kGood;
							creature[player[i].pet[j]].attackCounter =	0;
							creature[player[i].pet[j]].outOfStable   =  TickCount();
							creature[player[i].pet[j]].petExp		 =  player[i].petExperience[j];
							if (player[i].petHealth[j]<creatureInfo[creature[player[i].pet[j]].id].maxHitPoints)
							{
								creature[player[i].pet[j]].hitPoints=player[i].petHealth[j];
							}
							creature[player[i].pet[j]].level		 =  sk_GetSkillLevel(creature[player[i].pet[j]].petExp);

							for (cpt=0; cpt<kMaxMacroTrace; cpt++)
							{
								creature[i].attackTargets[cpt][0]=0;
								creature[i].attackTargets[cpt][1]=0;
							}

							creature[player[i].pet[j]].hunger     = 5000;

							creature[player[i].pet[j]].hungerfeedback=false;
							currentlyControlling++;
						}

			return;
		}
	//Dave added exclamation to pet commands (!)
	if (strcmp(word2,"heel!")==0)
		command=kHeel;

	if (strcmp(word2,"guard!")==0)
		command=kGuard;

	if (strcmp(word2,"stay!")==0)
		command=kStay;

	if (strcmp(word2,"attack!")==0)
		command=kAttack;

	if (strcmp(word2,"release!")==0)
		command=kFree;

	if (strcmp(word2,"move!")==0)
		command=kMove;

	if (strcmp(word2,"speak!")==0)
		command=kSpeak;

	if (command<0)  // no command 0.1.3
		return;

	for (j=0;j<kMaxPets;j++)
		if (commanding[j])
			if (player[i].pet[j]!=0)
				//if ((player[i].pet[j]!=0) || ((player[i].petInStable[j]) && (command==kFree)))
				if ((cr_DistanceBetween(player[i].creatureIndex,player[i].pet[j]<10)) || (command==kFree))
				{

					// merge 7/15

					/*if (in_ItemEquipedServer(i,kDruidRing)==false)
					if (player[i].category==0 || player[i].category==8)
					if (creature[player[i].pet[j]].hunger<3000)
					if (tb_Rnd(1,12000)<(creatureInfo[creature[player[i].pet[j]].id].tamingLevel*15))  // go wild
					{
					    nw_SendTextToClient(i, "Your pet defies you and goes wild!", 0);
								if (!player[i].petInStable[j]) // 0.1.4
									{
					        pl_BanishCreature(-1,player[i].pet[j]); // merge 7/27
										//creature[player[i].pet[j]].master				  =	0;
										//creature[player[i].pet[j]].attackTarget	  =	0;
								    //creature[player[i].pet[j]].possibleTarget	=	0;
										//creature[player[i].pet[j]].trackRow			  =	0;
										//creature[player[i].pet[j]].trackCol			  =	0;
										//creature[player[i].pet[j]].tracking			  =	false;
										
					      //if (creatureInfo[creature[player[i].pet[j]].id].alignment==kEvil)	// free evil creatures in town will attack former master
					        //  if ((creature[player[i].pet[j]].row>2075) && (creature[player[i].pet[j]].row<2310) && (creature[player[i].pet[j]].col>1749) && (creature[player[i].pet[j]].col<2001))
										//     creature[player[i].pet[j]].attackTarget	=	player[i].creatureIndex;
									}
									
								player[i].pet[j]												=	0;
								player[i].petInStable[j]								= false;
								player[i].petType[j]										= 0;
								strcpy(player[i].petName[j],"");
								SendRefresh=true;
					continue;
					}*/
					// merge 7/15

					if ((command!=kSpeak) && (!player[i].petInStable[j])) // 0.1.4
						creature[player[i].pet[j]].command=command;

					if ((command==kStay) && (!player[i].petInStable[j])) // 0.1.4
						if (doorway[map[creature[player[i].pet[j]].row][creature[player[i].pet[j]].col]])
						{
							creature[player[i].pet[j]].command        = kHeel;
							creature[player[i].pet[j]].attackTarget	  =	0;
							creature[player[i].pet[j]].possibleTarget	=	0;
							creature[player[i].pet[j]].trackRow			  =	0;
							creature[player[i].pet[j]].trackCol			  =	0;
							creature[player[i].pet[j]].tracking			  =	false;
						}

					if ((command==kMove) && (!player[i].petInStable[j])) // 0.1.4
					{
						creature[player[i].pet[j]].command        = kMove;
						creature[player[i].pet[j]].attackTarget	  =	0;
						creature[player[i].pet[j]].possibleTarget	=	0;
						creature[player[i].pet[j]].trackRow			  =	0;
						creature[player[i].pet[j]].trackCol			  =	0;
						creature[player[i].pet[j]].tracking			  =	false;
					}

					if ((command==kSpeak) && (!player[i].petInStable[j])) // 0.1.4
					{
						cr_AddSound(player[i].pet[j], creatureInfo[creature[player[i].pet[j]].id].attackSound);
						//creature[player[i].pet[j]].soundFX			= creatureInfo[creature[player[i].pet[j]].id].attackSound;
						//creature[player[i].pet[j]].soundFXTime	= TickCount();
					}

					if ((command==kHeel) && (!player[i].petInStable[j])) // 0.1.4
					{
						creature[player[i].pet[j]].attackTarget	  =	0;
						creature[player[i].pet[j]].possibleTarget	=	0;
						creature[player[i].pet[j]].trackRow			  =	0;
						creature[player[i].pet[j]].trackCol			  =	0;
						creature[player[i].pet[j]].tracking			  =	false;
					}

					if (command==kFree)
					{
						if (!player[i].petInStable[j]) // 0.1.4
						{
							pl_BanishCreature(-1,player[i].pet[j]); // merge 7/27

							//creature[player[i].pet[j]].master				  =	0;
							//creature[player[i].pet[j]].attackTarget	  =	0;
							//creature[player[i].pet[j]].possibleTarget	=	0;
							//creature[player[i].pet[j]].trackRow			  =	0;
							//creature[player[i].pet[j]].trackCol			  =	0;
							//creature[player[i].pet[j]].tracking			  =	false;

							//if (creatureInfo[creature[player[i].pet[j]].id].alignment==kEvil)	// free evil creatures in town will attack former master
							//  if ((creature[player[i].pet[j]].row>2075) && (creature[player[i].pet[j]].row<2310) && (creature[player[i].pet[j]].col>1749) && (creature[player[i].pet[j]].col<2001))
							//     creature[player[i].pet[j]].attackTarget	=	player[i].creatureIndex;
						}

						player[i].pet[j]												=	0;

						player[i].petInStable[j]								= false;
						player[i].petType[j]										= 0;
						player[i].petExperience[j]										= 0;
						player[i].petHealth[j]									= 0;
						strcpy(player[i].petName[j],"");
						SendRefresh=true;
					}

				}

	if (SendRefresh)
		nw_SendPetNameRefresh(i);

}

// ----------------------------------------------------------------- //
void pl_Insult(int i)
// ----------------------------------------------------------------- //

{
	//int								j;
	//int								k;
	//int								command;
	//int								c;

	char							insult[kStringLength];
	int								word;


	strcpy(insult,"Thou ");

	word=tb_Rnd(1,50);

	switch (word)
	{
		case 1: 	{	strcat(insult,"artless ");				break; }

		case 2: 	{	strcat(insult,"bawdy ");					break; }

		case 3: 	{	strcat(insult,"beslubbering ");		break; }

		case 4: 	{	strcat(insult,"bootless ");				break; }

		case 5: 	{	strcat(insult,"churlish ");				break; }

		case 6: 	{	strcat(insult,"cockered ");				break; }

		case 7: 	{	strcat(insult,"clouted ");				break; }

		case 8: 	{	strcat(insult,"craven ");					break; }

		case 9: 	{	strcat(insult,"currish ");				break; }

		case 10: 	{	strcat(insult,"dankish ");				break; }

		case 11: 	{	strcat(insult,"dissembling ");		break; }

		case 12: 	{	strcat(insult,"droning ");				break; }

		case 13: 	{	strcat(insult,"errant ");					break; }

		case 14: 	{	strcat(insult,"fawning ");				break; }

		case 15: 	{	strcat(insult,"fobbing ");				break; }

		case 16: 	{	strcat(insult,"froward ");				break; }

		case 17: 	{	strcat(insult,"frothy ");					break; }

		case 18: 	{	strcat(insult,"gleeking ");				break; }

		case 19: 	{	strcat(insult,"goatish ");				break; }

		case 20: 	{	strcat(insult,"gorbellied ");			break; }

		case 21: 	{	strcat(insult,"impertinent ");		break; }

		case 22: 	{	strcat(insult,"infectious ");			break; }

		case 23: 	{	strcat(insult,"jarring ");				break; }

		case 24: 	{	strcat(insult,"loggerheaded ");		break; }

		case 25: 	{	strcat(insult,"lumpish ");				break; }

		case 26: 	{	strcat(insult,"mammering ");			break; }

		case 27: 	{	strcat(insult,"mangled ");				break; }

		case 28: 	{	strcat(insult,"mewling ");				break; }

		case 29: 	{	strcat(insult,"paunchy ");				break; }

		case 30: 	{	strcat(insult,"pribbling ");			break; }

		case 31: 	{	strcat(insult,"puking ");					break; }

		case 32: 	{	strcat(insult,"puny ");						break; }

		case 33: 	{	strcat(insult,"quailing ");				break; }

		case 34: 	{	strcat(insult,"rank ");						break; }

		case 35: 	{	strcat(insult,"reeky ");					break; }

		case 36: 	{	strcat(insult,"roguish ");				break; }

		case 37: 	{	strcat(insult,"ruttish ");				break; }

		case 38: 	{	strcat(insult,"saucy ");					break; }

		case 39: 	{	strcat(insult,"spleeny ");				break; }

		case 40: 	{	strcat(insult,"spongy ");					break; }

		case 41: 	{	strcat(insult,"surly ");					break; }

		case 42: 	{	strcat(insult,"tottering ");			break; }

		case 43: 	{	strcat(insult,"unmuzzled ");			break; }

		case 44: 	{	strcat(insult,"vain ");						break; }

		case 45: 	{	strcat(insult,"venomed ");				break; }

		case 46: 	{	strcat(insult,"villainous ");			break; }

		case 47: 	{	strcat(insult,"warped ");					break; }

		case 48: 	{	strcat(insult,"wayward ");				break; }

		case 49: 	{	strcat(insult,"weedy ");					break; }

		case 50: 	{	strcat(insult,"yeasty ");					break; }

		default:	break;
	}

	word=tb_Rnd(1,50);

	switch (word)
	{
		case 1: 	{	strcat(insult,"base-court ");			break; }

		case 2: 	{	strcat(insult,"bat-fowling ");		break; }

		case 3: 	{	strcat(insult,"beef-witted ");		break; }

		case 4: 	{	strcat(insult,"beetle-headed ");	break; }

		case 5: 	{	strcat(insult,"boil-brained ");		break; }

		case 6: 	{	strcat(insult,"clapper-clawed ");	break; }

		case 7: 	{	strcat(insult,"clay-brained ");		break; }

		case 8: 	{	strcat(insult,"common-kissing ");	break; }

		case 9: 	{	strcat(insult,"crook-pated ");		break; }

		case 10: 	{	strcat(insult,"dismal-dreaming ");break; }

		case 11: 	{	strcat(insult,"dizzy-eyed ");			break; }

		case 12: 	{	strcat(insult,"doghearted ");			break; }

		case 13: 	{	strcat(insult,"dread-bolted ");		break; }

		case 14: 	{	strcat(insult,"earth-vexing ");		break; }

		case 15: 	{	strcat(insult,"elf-skinned ");		break; }

		case 16: 	{	strcat(insult,"fat-kidneyed ");		break; }

		case 17: 	{	strcat(insult,"fen-sucked ");			break; }

		case 18: 	{	strcat(insult,"flap-mouthed ");		break; }

		case 19: 	{	strcat(insult,"fly-bitten ");			break; }

		case 20: 	{	strcat(insult,"folly-fallen ");		break; }

		case 21: 	{	strcat(insult,"fool-born ");			break; }

		case 22: 	{	strcat(insult,"full-gorged ");		break; }

		case 23: 	{	strcat(insult,"guts-griping ");		break; }

		case 24: 	{	strcat(insult,"half-faced ");			break; }

		case 25: 	{	strcat(insult,"hasty-witted ");		break; }

		case 26: 	{	strcat(insult,"hedge-born ");			break; }

		case 27: 	{	strcat(insult,"hell-hated ");			break; }

		case 28: 	{	strcat(insult,"idle-headed ");		break; }

		case 29: 	{	strcat(insult,"ill-breeding ");		break; }

		case 30: 	{	strcat(insult,"ill-nurtured ");		break; }

		case 31: 	{	strcat(insult,"knotty-pated ");		break; }

		case 32: 	{	strcat(insult,"milk-livered ");		break; }

		case 33: 	{	strcat(insult,"motley-minded ");	break; }

		case 34: 	{	strcat(insult,"onion-eyed ");			break; }

		case 35: 	{	strcat(insult,"plume-plucked ");	break; }

		case 36: 	{	strcat(insult,"pottle-deep ");		break; }

		case 37: 	{	strcat(insult,"pox-marked ");			break; }

		case 38: 	{	strcat(insult,"reeling-ripe ");		break; }

		case 39: 	{	strcat(insult,"rough-hewn ");			break; }

		case 40: 	{	strcat(insult,"rude-growing ");		break; }

		case 41: 	{	strcat(insult,"rump-fed ");				break; }

		case 42: 	{	strcat(insult,"shard-borne ");		break; }

		case 43: 	{	strcat(insult,"sheep-biting ");		break; }

		case 44: 	{	strcat(insult,"spur-galled ");		break; }

		case 45: 	{	strcat(insult,"swag-bellied ");		break; }

		case 46: 	{	strcat(insult,"tardy-gaited ");		break; }

		case 47: 	{	strcat(insult,"tickle-brained ");	break; }

		case 48: 	{	strcat(insult,"toad-spotted ");		break; }

		case 49: 	{	strcat(insult,"urchin-snouted ");	break; }

		case 50: 	{	strcat(insult,"weather-bitten ");	break; }

		default:	break;
	}

	word=tb_Rnd(1,50);

	switch (word)
	{
		case 1: 	{	strcat(insult,"apple-john.");			break; }

		case 2: 	{	strcat(insult,"baggage.");				break; }

		case 3: 	{	strcat(insult,"barnacle.");				break; }

		case 4: 	{	strcat(insult,"bladder.");				break; }

		case 5: 	{	strcat(insult,"boar-pig.");				break; }

		case 6: 	{	strcat(insult,"bugbear.");				break; }

		case 7: 	{	strcat(insult,"bum-bailey.");			break; }

		case 8: 	{	strcat(insult,"canker-blossom.");	break; }

		case 9: 	{	strcat(insult,"clack-dish.");			break; }

		case 10: 	{	strcat(insult,"clotpole.");				break; }

		case 11: 	{	strcat(insult,"coxcomb.");				break; }

		case 12: 	{	strcat(insult,"codpiece.");				break; }

		case 13: 	{	strcat(insult,"death-token.");		break; }

		case 14: 	{	strcat(insult,"dewberry.");				break; }

		case 15: 	{	strcat(insult,"flap-dragon.");		break; }

		case 16: 	{	strcat(insult,"flax-wench.");			break; }

		case 17: 	{	strcat(insult,"flirt-gill.");			break; }

		case 18: 	{	strcat(insult,"foot-licker.");		break; }

		case 19: 	{	strcat(insult,"fustilarian.");		break; }

		case 20: 	{	strcat(insult,"giglet.");					break; }

		case 21: 	{	strcat(insult,"gudgeon.");				break; }

		case 22: 	{	strcat(insult,"haggard.");				break; }

		case 23: 	{	strcat(insult,"harpie.");					break; }

		case 24: 	{	strcat(insult,"hedge-pig.");			break; }

		case 25: 	{	strcat(insult,"horn-beast.");			break; }

		case 26: 	{	strcat(insult,"hugger-mugger.");	break; }

		case 27: 	{	strcat(insult,"jolthead.");				break; }

		case 28: 	{	strcat(insult,"lewdster.");				break; }

		case 29: 	{	strcat(insult,"lout.");						break; }

		case 30: 	{	strcat(insult,"maggot-pie.");			break; }

		case 31: 	{	strcat(insult,"malt-worm.");			break; }

		case 32: 	{	strcat(insult,"mammet.");					break; }

		case 33: 	{	strcat(insult,"measle.");					break; }

		case 34: 	{	strcat(insult,"minnow.");					break; }

		case 35: 	{	strcat(insult,"miscreant.");			break; }

		case 36: 	{	strcat(insult,"moldwarp.");				break; }

		case 37: 	{	strcat(insult,"mumble-news.");		break; }

		case 38: 	{	strcat(insult,"nut-hook.");				break; }

		case 39: 	{	strcat(insult,"pigeon-egg.");			break; }

		case 40: 	{	strcat(insult,"pignut.");					break; }

		case 41: 	{	strcat(insult,"puttock.");				break; }

		case 42: 	{	strcat(insult,"pumpion.");				break; }

		case 43: 	{	strcat(insult,"ratsbane.");				break; }

		case 44: 	{	strcat(insult,"scut.");						break; }

		case 45: 	{	strcat(insult,"skainsmate.");			break; }

		case 46: 	{	strcat(insult,"newbie.");				break; }

		case 47: 	{	strcat(insult,"varlet.");					break; }

		case 48: 	{	strcat(insult,"vassal.");					break; }

		case 49: 	{	strcat(insult,"whey-face.");			break; }

		case 50: 	{	strcat(insult,"wagtail.");				break; }

		default:	break;
	}

	if (player[i].curse&kBabbleCurse)
		strcpy(insult,"Babble babble babble.");

	if (creature[player[i].creatureIndex].mute)
		strcpy(insult, "");

	creature[player[i].creatureIndex].talkWhen  = TickCount();

	creature[player[i].creatureIndex].talkTime  = TickCount();

	creature[player[i].creatureIndex].talkID	  = chatMessageID;

	chatMessageID++;

	strcpy(creature[player[i].creatureIndex].talk,insult);

	creature[player[i].creatureIndex].hiding=false;

}

// ----------------------------------------------------------------- //
TBoolean pl_PlayerHeard(int i, unsigned long talkID)
// ----------------------------------------------------------------- //

{
	int             j;
	int             min=player[i].heard[0];
	int             minIndex=0;

	for (j=0;j<kHeardSize;j++)
	{
		if (player[i].heard[j]==talkID)
			return (true);

		if (player[i].heard[j]<min)
		{
			min=player[i].heard[j];
			minIndex=j;
		}
	}

	player[i].heard[minIndex]=talkID;

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean pl_PlayerHeardSound(int i, unsigned long sndID, unsigned long sndTime)
// ----------------------------------------------------------------- //

{
	int             j;
	int             min=player[i].heardSound[0];
	int             minIndex=0;

	if (sndID==0)
		return (true);

	if ((sndTime+600)<TickCount())	// kill sound after 2 seconds
		return (true);

	for (j=0;j<kHeardSize;j++)
	{
		if (player[i].heardSound[j]==sndID)
			return (true);

		if (player[i].heardSound[j]<min)
		{
			min=player[i].heardSound[j];
			minIndex=j;
		}
	}

	player[i].heardSound[minIndex]=sndID;

	return (false);

}

// ----------------------------------------------------------------- //
void pl_ClearPlayerRecord(int i)
// ----------------------------------------------------------------- //

{
	int						j;

	player[i].active									=	false;
	player[i].online									= false;
	player[i].skipNextRefresh					= true;
	player[i].sendStatsRefresh				= false;
	player[i].sendPetsRefresh				  = false;
	strcpy(player[i].userName, "");
	strcpy(player[i].playerName, "");
	strcpy(player[i].password, "");
	strcpy(player[i].guildName, "None");
	strcpy(player[i].macID, "");
	player[i].creatureIndex						= 0;
	player[i].id											=	0;
	player[i].lastRefresh							=	0;
	player[i].thiefUntil							=	0;
	player[i].lastTalkRefresh					=	0;
	player[i].playerRefreshTime				= 0;
	player[i].previousStenchMessage		= 0;
	player[i].gathering								=	0;
	player[i].gatherType							= 0;
	player[i].gatherRow								=	0;
	player[i].gatherCol								=	0;
	player[i].strengthCounter					=	0;
	player[i].intelligenceCounter			=	0;
	player[i].dexterityCounter				=	0;
	player[i].meditating							=	false;
	player[i].healing									=	false;
	player[i].previousMessageID				=	9999999;
	//strcpy(player[i].talk, "");
	//player[i].talkTime								=	0;
	//player[i].talkID                  = 0;
	player[i].category								=	0;
	player[i].god								      =	false;
	player[i].revealHidden						=	0;
	player[i].dead										=	false;
	player[i].maxAttackers						= 3;
	player[i].currentlyAttacking			= 0;
	player[i].title										=	5000; //Dave 12-03-2012 was 0 for peasant
	player[i].accountNumber						=	0;
	player[i].router						      =	true;
	player[i].totalRefresh            = 0;
	player[i].stealth						      =	false;
	player[i].stealthCool							=0;
	player[i].meet						      	=	0;
	player[i].turnUntil						    =	0;
	player[i].characterClass					=	0;
	player[i].sharing					        =	false;
	player[i].demo					          =	true;
	player[i].helper					        =	false;
	player[i].noGain					        =	false;
	player[i].slow					          =	false;
	player[i].morph					          =	0;
	player[i].newPlayer					      =	true;
	player[i].poisonField					    =	0;
	player[i].creatureField						= 0;
	player[i].fieldFrom							= 0;
	player[i].speed								= 0;
	player[i].massheal							=0;
	player[i].disorientfield					=0;
	player[i].paralyzeField					  =	0;
	player[i].returnRow					      =	0;
	player[i].returnCol					      =	0;
	player[i].previousRow					    =	0;
	player[i].previousCol					    =	0;
	player[i].magicAttacker					  =	0;
	player[i].resistanceGained				=	0;
	player[i].normalAttacker					=	0;
	player[i].fightingSkillGained		  =	0;
	player[i].lastSkillGain		        =	0;
	player[i].language		        		=	0;
	player[i].curse		        				=	0;
	player[i].hairStyle						=0;
	player[i].hairColor						=0;
	player[i].possibleBackstab				=false;
	player[i].noTrack						=false;
	player[i].npcDressup					=0;
	player[i].morphTime					=0;
	strcpy(player[i].msay, "");

	for (j=0;j<kHeardSize;j++)
		player[i].heard[j]=0;

	for (j=0;j<kMaxPlayers;j++)
		player[i].sentName[j]=false;

	for (j=0;j<16;j++)
	{
		strcpy(player[i].friendName[j],"");
		player[i].friendID[j]=0;
	}

	for (j=0;j<kMaxMacroTrace;j++)
	{
		player[i].anatomyTarget[j]              = 0;
		player[i].tamingTarget[j]               = 0;
		player[i].detectingHiddenTarget[j]      = 0;
		player[i].thieveryTarget[j]             = 0;
	}

	player[i].lastHideRow                       = 0;

	player[i].lastHideCol                       = 0;

	player[i].lastTrackingRow                 = 0;
	player[i].lastTrackingCol                 = 0;

	for (j=0;j<kMaxPets;j++)
	{
		player[i].pet[j]							=	0;
		player[i].petInStable[j]			=	false;
		player[i].petType[j]					=	0;
		player[i].petExperience[j]					=	0;
		player[i].petHealth[j]					= 0;
		strcpy(player[i].petName[j],"");
		player[i].petLoyalty[j]       = 0;
	}

	for (j=0;j<13;j++)
		player[i].arm[j]								=	0;

	for (j=0;j<kMaxSkills;j++)
	{
		player[i].skill[j].level			=	0;
		//player[i].skill[j].direction	=	0;
		//player[i].skill[j].counter		=	0;
	}

	for (j=0;j<kMaxScreenElements;j++)
	{
		playerState[i][0][j]					=	0;
		playerState[i][1][j]					=	0;
	}

	for (j=0;j<kMaxInventorySize;j++)
	{
		player[i].bank[j].itemID			=	0;
		player[i].bank[j].itemCount		=	0;
	}

	for (j=0;j<kMaxTrail;j++)
	{
		player[i].rowTrail[j]					=	0;
		player[i].colTrail[j]					=	0;
	}

}

// ----------------------------------------------------------------- //
TBoolean pl_AddPlayer(char userName[275], char password[kStringLength], UInt32 playerID, int *playerIndex)		// server
// ----------------------------------------------------------------- //

{
	int						i;
	int						j;
	int						k;
	//int           			m;
	//int          			n;
	UInt16					row;
	UInt16					col;
	//UInt16					tryRow;
	//UInt16					tryCol;
	int						found=-1;
	//char					pass[kStringLength];
	creatureType 			theCreature;
	playerType				thePlayer;
	TBoolean				playerAdded=false;
	//int           			maxLoop;
	char          			textMessage[kStringLength];
	char					loggingOn[kStringLength];
	char          			userNameLC[kStringLength]; // merge 7/29

	*playerIndex=0;
	
	if (!fi_Logon(userName, password, &thePlayer, &theCreature, true))
		return (false);
	
	strcpy(loggingOn,userName);

	tb_MakeLowerCase(loggingOn);

	for (i=1;i<kMaxPlayers;i++) // merge 7/29 {
		if (player[i].id==playerID)
		{
			fi_WriteToDebuggerLog("Duplicate Login");
			nw_RemovePlayer(player[i].id);
			nw_SendClearName(i);
			fi_Logout(i);

			if ((player[i].creatureIndex>=0) && (player[i].creatureIndex<kMaxCreatures))
			{
				creatureMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col]=0;
				cr_ClearCreatureRecord(player[i].creatureIndex);
			}

			pl_ClearPlayerRecord(i);

#ifdef _SERVERONLY
			gr_DrawServerNameAt(i);
#endif
		} // merge 7/29 {

	for (i=1;i<kMaxPlayers;i++)
		if (player[i].active==true)
		{
			strcpy(userNameLC,player[i].userName); // merge 7/29
			tb_MakeLowerCase(userNameLC); // merge 7/29

			if (strcmp(loggingOn,userNameLC)==0) // merge 7/29
				//if (strcmp(loggingOn,player[i].playerNameLC)==0)
				//if (strcmp(userName,player[i].userName)==0)
			{
				if (strcmp(userName,"Host")==0)
					return (false);

				//fi_WriteToErrorLog("Duplicate Login - Remove Player:");
				//fi_WriteToErrorLog(player[i].playerName);
				nw_RemovePlayer(player[i].id);

				player[i].id			= playerID;

				player[i].lastCheckIn	= TickCount();

				player[i].lastMove		= TickCount();

				player[i].logon		    = TickCount();

				player[i].online		= true;

				//for (j=0;j<kDataSize;j++)
				//	playerData[i][j]=0;
				playerData[i][0]++;	// force refresh from previous frame

				*playerIndex=i;

				return (true);
			}
		}
		else if (found==-1)
			found=i;

	if (found>0)
	{
		//	  NSpPlayer_GetAddress (gNetGame,playerID,&outAddress);

		player[found].active				=	true;
		player[found].online				= true;
		strcpy(player[found].userName, userName);
		strcpy(player[found].password, password);
		strcpy(player[found].playerName, thePlayer.playerName);
		strcpy(player[found].playerNameTemp, thePlayer.playerNameTemp);
		strcpy(player[found].playerNameLC, thePlayer.playerName);
		strcpy(player[found].titleCustom, thePlayer.titleCustom);
		strcpy(player[found].guildName, thePlayer.guildName);

		for (j=0;j<strlen(player[found].playerNameLC);j++)
		{
			if (((int)player[found].playerNameLC[j]>=65) && ((int)player[found].playerNameLC[j]<=90)) // convert to lower case
				player[found].playerNameLC[j]=(char)((int)player[found].playerNameLC[j]+32);

			//else if (player[found].playerNameLC[j]=='*')
			//  player[found].playerNameLC[j]=' ';
		}

		for (j=1;j<kMaxPlayers;j++)	// 1..5000 reserved for players
			if (creature[j].id==0)
			{
				creature[j].id					=	kHuman;
				creature[j].alignment			=	kGood;
				row								=	theCreature.row;
				col								=	theCreature.col;

				playerAdded=true; // everything ok ... add to creature array
				creature[j].row						=	row;
				creature[j].col						=	col;
				creature[j].timeStamp				= TickCount();
				creature[j].moveDirection			= 4;
				creature[j].hitPoints				= theCreature.hitPoints;
				creature[j].strength				= theCreature.strength;
				creature[j].intelligence			= theCreature.intelligence;
				creature[j].dexterity				= theCreature.dexterity;
				creature[j].strengthOffset			= theCreature.strengthOffset;
				creature[j].intelligenceOffset		= theCreature.intelligenceOffset;
				creature[j].regeneratingTime		= theCreature.regeneratingTime;
				creature[j].regeneratingPower		= theCreature.regeneratingPower;
				creature[j].dexterityOffset			= theCreature.dexterityOffset;
				creature[j].strengthTime			= theCreature.strengthTime;
				creature[j].intelligenceTime		= theCreature.intelligenceTime;
				creature[j].dexterityTime			= theCreature.dexterityTime;
				creature[j].magicPoints				= theCreature.magicPoints;
				creature[j].playerIndex				= found;
				creature[j].hunger					= theCreature.hunger;
				creature[j].mute					= theCreature.mute;
				creature[j].poison					= theCreature.poison;
				creature[j].hungerfeedback			= theCreature.hungerfeedback;
				creature[j].stamina					= theCreature.stamina;

				//if (creature[j].hitPoints<creature[j].strength) // beta testing
				//  creature[j].hitPoints=creature[j].strength;

				for (k=0;k<kMaxInventorySize;k++)
				{
					creature[j].inventory[k].itemID			=	theCreature.inventory[k].itemID;
					creature[j].inventory[k].itemCount		=	theCreature.inventory[k].itemCount;
				}

				cr_GetNearestFreeLocation(j, &row, &col,"pl_AddPlayer");

				creature[j].row																	=	row;
				creature[j].col																	=	col;

				if ((row==0) && (col==0)) // can't find empty spot on map
				{
					pl_ClearPlayerRecord(found);
					cr_ClearCreatureRecord(j);
					return (false);
				}

				creatureMap[creature[j].row][creature[j].col]		=	j;

				for (k=0;k<kMaxSkills;k++)
				{
					player[found].skill[k].level			=	thePlayer.skill[k].level;
					//player[found].skill[k].direction	=	thePlayer.skill[k].direction;
				}

				for (k=0;k<13;k++)
					player[found].arm[k]								=	thePlayer.arm[k];

				player[found].noGain     =	false;

				player[found].slow       =	false;

				// -------- helpers------
				player[found].helper     =	false;

				//if (strcmp(player[found].playerNameLC,"helen")==0)            player[found].helper     =	true;
				//if (strcmp(player[found].playerNameLC,"helena")==0)           player[found].helper     =	true;
				// ----------------------

				if (thePlayer.category==3)  // new player ... notify gms ----------------
				{
					thePlayer.category=0;
					strcpy(textMessage,"New: ");
					strcat(textMessage,player[found].playerName);

					for (k=1;k<kMaxPlayers;k++)
						//if ((player[k].category==1) || (player[k].category==2) || (player[k].category==4) || (player[k].category==5) || (player[k].helper))  // merge 7/27
						if (pl_AuthorizedTo(player[k].creatureIndex,kNotifiedOfNewPlayers)) // add to carbon 11/27
							nw_SendSystemTextToClient(k, textMessage);
				} // new player ... notify gms ----------------

				player[found].category						= thePlayer.category;

				player[found].strengthCounter				= thePlayer.strengthCounter;

				player[found].intelligenceCounter			= thePlayer.intelligenceCounter;

				player[found].dexterityCounter				= thePlayer.dexterityCounter;

				player[found].category						= thePlayer.category;

				player[found].dead							= thePlayer.dead;

				player[found].title							= thePlayer.title;

				player[found].accountNumber					= thePlayer.accountNumber;

				player[found].gender						= thePlayer.gender;

				player[found].thiefUntil					= thePlayer.thiefUntil;

				player[found].saveAt                  		= TickCount()+(3600*kPlayerSaveTime);

				player[found].stealth                 		= thePlayer.stealth;

				player[found].stealthCool					= 0;

				player[found].meet                 			= 0;

				player[found].turnUntil               		= 0;

				player[found].characterClass				= thePlayer.characterClass;

				player[found].sharing					    = thePlayer.sharing;

				player[found].demo					        = thePlayer.demo;

				player[found].newPlayer					    = thePlayer.newPlayer;

				player[found].returnRow               		= 0;

				player[found].returnCol               		= 0;

				player[found].morph                   		= 0;

				player[found].maxAttackers					= 3;

				player[found].currentlyAttacking			= 0;

				player[found].hairColor						= thePlayer.hairColor;

				player[found].hairStyle						= thePlayer.hairStyle;

				player[found].curse							= thePlayer.curse;

				player[found].stealthMoves					= 0;

				player[found].lastBackstabTarget			= 0;

				player[found].possibleBackstab				= false;

				player[found].noTrack						= thePlayer.noTrack;
				
				player[found].npcDressup					= thePlayer.npcDressup;
				
				player[found].morphTime						= 0;

				strcpy(player[found].macID,"");

				strcpy(player[found].msay, "");

				for (k=0;k<kMaxInventorySize;k++)
				{
					player[found].bank[k].itemID		=	thePlayer.bank[k].itemID;
					player[found].bank[k].itemCount		=	thePlayer.bank[k].itemCount;
				}

				for (k=0;k<kMaxPets;k++)
				{
					player[found].pet[k]					= thePlayer.pet[k];
					player[found].petInStable[k]			= thePlayer.petInStable[k];
					player[found].petType[k]				= thePlayer.petType[k];
					player[found].petExperience[k]				= thePlayer.petExperience[k];
					player[found].petHealth[k]				= thePlayer.petHealth[k];
					strcpy(player[found].petName[k],thePlayer.petName[k]);
					player[found].petLoyalty[k]       		= thePlayer.petLoyalty[k];
				}

				// ------------------------------------------------
				player[found].creatureIndex	= j;

				//for (k=0;k<kDataSize;k++)
				//	playerData[i][k]=0;
				playerData[found][0]++;	// force refresh from previous frame

				//	player[found].rowTrail[0]					=	row;
				//	player[found].colTrail[0]					=	col;
				//	for (k=1;k<kMaxTrail;k++)
				//		{
				//		  maxLoop=0;
				//		  do {
				//		    tryRow=player[found].rowTrail[k-1]+tb_Rnd(0,2)-1;
				//		    tryCol=player[found].colTrail[k-1]+tb_Rnd(0,2)-1;
				//		    maxLoop++;
				//		    if (maxLoop>50) break;
				//		  } while (!cr_InDomain(j,tryRow,tryCol));
				//			player[found].rowTrail[k]					=	tryRow;
				// 			player[found].colTrail[k]					=	tryCol;
				//		}

				break;
			}

		if (playerAdded==false) // couldn't find space in creature array
		{
			pl_ClearPlayerRecord(found);
			return (false);
		}

		player[found].id						= playerID;

		player[found].lastCheckIn		= TickCount();
		player[found].lastMove		  = TickCount();
		player[found].logon		      = TickCount();

		*playerIndex=found;

		numberOfPlayersOnline++;
		
		fi_UpdatePlayerLog(found, 0);
		fi_UpdateActivityLog(found);

		return (true);
	}

	return (false);

}
#endif