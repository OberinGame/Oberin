#ifndef _SKILLS
#define _SKILLS

#include "Keys.h"
#include "Remap.h"
#include "Network.h"
#include "Inventory.h"
#include "Globals.h"

void 											sk_IncreaseStrengthCounter(int i); // server
void 											sk_IncreaseIntelligenceCounter(int i); // server
void 											sk_IncreaseDexterityCounter(int i); // server
UInt8 										sk_GetSkillLevel(UInt32 points);	// server
UInt32 										sk_GetSkillNextLevel(UInt32 points);	// server
void                      sk_RaiseSkill(int i, int attacker, int skill, int amount); // server

void 											sk_SendFailedHarvestMessage(int	i);	// server

void											sk_Melee							(int, int); //server
void 											sk_Lumberjacking					(int i); // server
void 											sk_Mining									(int i); // server
void 											sk_Hiding									(int i);
void 											sk_Anatomy								(int i, int c, TBoolean active); // server
void 											sk_AnimalLore							(int i, int c); // server
void 											sk_EvaluatingIntelligence	(int i, int c); // server
void 											sk_Fishing								(int i); // server
void 											sk_MagicLore							(int i, int w); // server
void 											sk_Meditation							(int i); // server
void 											sk_Healing								(int i, int c); // server
void 											sk_Thievery								(int i, int c); // server
void 											sk_MageryOnCreature				(int i, int c, int spell); // server
void 											sk_MageryOnTerrain				(int i, int row, int col, int spell); // server
void 											sk_ResistingSpells				(int c, int i); // server
void 											sk_Taming									(int i, int c); // server
void 											sk_DetectHidden						(int i); // server
void 											sk_PoleFighting						(int i, int theEnemy); // server
void 											sk_Swordsmanship					(int i, int theEnemy); // server
void 											sk_MaceFighting						(int i, int theEnemy); // server
void 											sk_Wrestling							(int i, int theEnemy); // server
void                      						sk_Archery                (int i, int theEnemy); // server
void                      						sk_Tracking             (int i); // server
void                      						sk_Lockpicking            (int i); // server
void                      						sk_RemoveTrap             (int i); // server
void                      						sk_Parrying               (int i, int theEnemy); // server
void											sk_Stealth				(int i);
void 											sk_SmallWeapons						(int i, int theEnemy); // server
void 											sk_DamageMagicRobe				(int p); // server
void											sk_TrainProof			(int, int, int, int, int);

TBoolean 									sk_CraftingSkill					(int skill, int i, int item, TBoolean raiseStr, TBoolean raiseInt, TBoolean raiseDex); // server

TBoolean 								  sk_CastSpellOnCreature		(int cc, int ct, int spell, TBoolean fieldSpell); // server
TBoolean 											sk_CastSpellOnTerrain			(int cc, int row, int col, int spell); // server
void 											sk_ClearLocalSkills				(void);
TBoolean 									sk_IsUseableSkill					(int skill);
void 											sk_UseSkill								(int skill);	// client
void 											sk_IncreaseSkill					(int i, int skill);
void                      sk_GetPositionOf          (int	p, int t, int *row, int *col);	// server
TBoolean                  sk_StandingNearFire       (void);
TBoolean                  sk_StandingNearForge      (void);
void                      sk_InitializeSkills       (void); // server
UInt8                     sk_GetCharacterLevel      (int p);	// server
float                     sk_GetMagicResistance     (int p); // server
void                      sk_CalculateStats         (int p);	// server
void					  sk_CalculateDamage		(int what, int cc, int ct); //server

#endif