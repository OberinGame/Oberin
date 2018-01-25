#ifndef _QUEST
#define _QUEST

#include "Keys.h"
#include "Remap.h"


#define kQuestStringLength                      512
#define kMaxQuests                		          100

#define kRatHuntQuest                           1
#define kLobsterHuntQuest                       2
#define kScorpionStingerQuest                   3
#define kGuardSteakRunQuest                     4
#define kLeatherHideQuest                       5
#define kQueensRubyQuest                        6
#define kHolyDaggerQuest                        7
#define kFrebbleNailsQuest                      8
#define kFrebbleMiningAxeQuest                  9
#define kFrebbleWoodworkingToolsQuest           10
#define kTrinceMiningAxeQuest                   11
#define kTrinceCoalQuest                        12
#define kSkeletonHuntQuest                      13
#define kDragonHuntQuest                        14
#define kGreaterHealPotionQuest                 15
#define kGreaterDetoxifyPotionQuest             16
#define kGaperStalkQuest                        17
#define kFurQuest                               18
#define kSpiceRunQuest                          19
#define kMinceSpiceRunQuest                     20
#define kTomeQuest                              21
#define kIceCrystalQuest                        22

typedef struct questType {
	TBoolean								active;
	int											gold;
	int											itemID;
	int											itemCount;
	int											experience;
	int											npcGoal;
	int											itemGoal;
	int											itemCountGoal;
	int											description;
	int											id;
} questType;	

extern int  currentQuest;

void  qu_InitializeQuests					(void);
void  qu_SendQuestDescription    	(int p, int q);
void  qu_GetQuestDescription     	(int q);	// client
int   qu_QuestComplete           	(int p, int theNPC, int item, int count);	// client
void  qu_InitializeQuests         (void);
int   qu_GetQuestForSpawn         (void);
void  qu_SpawnQuest               (void);

#endif