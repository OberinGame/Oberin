#ifndef _CREATURES
#define _CREATURES

#include "Keys.h"
#include "Remap.h"
#include "Inventory.h"

	//-----------------------------------------------
	#define							kMaxBloodStains						2000
	#define							kBloodStart							1
	#define							kBloodEnd							12

	#define							kMaxCreatures						50000 //30000
	#define							kMaxSpawnSpots						20
	#define							kMaxCreatureSpells					20
	
	#define							kMaxTalkScripts						50
	#define							kMaxWeaponModifiers					50 //Dave 11-24-2012 20
	
	#define							kEnglish							1

	#define							kMaxSpawnRegions					6000
	
	#define							kMaxLadders							200

	#define							kSpawnInterval						5

	#define							kLizardStrongHoldRegion				1
	#define							kDungeonRegion						2
	#define							kCemetaryRegion						3
	#define							kSewerRegion					  	4
	#define							kMeadowRegion						5
	#define							kIceDungeonRegion       			6
	#define							kCaveRegion             			7
	#define							kCarthCemeteryRegion				8
	#define							kCarthRegion						9
	#define							kFireDungeonRegion					10
	#define							kCarthCemetaryRegion				11
	
	#define							kNullDomain							0
	#define							kLand								1
	#define							kWater								2
	#define							kAir								3

	#define							kAnyTerrain							1
	#define							kGrass								2
	#define							kDesert								3
	#define							kSnowTerrain						4

	
	#define							kHuman								1
	#define							kNPCHuman							2
	#define							kNPCHumanFemale						3
	#define							kGazer								4
	#define							kAirElemental						5
	#define							kSeaTurtle							6
	#define							kSkeleton							7
	#define							kSlug								8
	#define							kHellHound							9
	#define							kGreyWolf							10
	#define							kRedDragon							11
	#define							kIceDragon							12
	#define							kReaper								13
	#define							kSheep								14
	#define							kScorpion							15
	#define							kHarpie								16
	#define							kBoneMage							17
	#define							kWaterElemental						18
	#define							kPoisonElemental					19
	#define							kBrownWolf							20
	#define							kBlackWolf							21
	#define							kFireElemental						22
	#define							kRat					          	23
	#define							kLizardMan					   	 	24
	#define							kForestDragon						25
	#define							kBat					          	26
  #define							kSpider					        	27
  #define							kFrostSpider					  	28
  #define							kLavaSpider					    	29
  #define							kSnail					    		30
  #define							kAnt					    		31
  #define							kTroll					    		32
  #define							kGiant					    		33
  #define							kCentaur					    	34
  #define							kSerpent					    	35
  #define							kZombie					    	  	36
  #define							kRustBeast					    	37
  #define							kBrigandShelter1					38
  #define							kBrigandShelter2					39
  #define							kBrigandShelter3					40
  #define							kBlackDragon				    	41
  #define							kSandSpider				      		42
  #define							kHillGiant				      		43
  #define							kFrostGiant				      		44
  #define							kBrownDog				      		45
  #define							kBlackDog				      		46
  #define							kGreyDog				      		47
  #define							kForestTroll				    	48
  #define							kPoisonBeast				    	49
  #define							kPinkDragon							50
  #define							kQueenAnt							51
  #define							kSnake								52
  #define							kWorkAnt							53
  #define							kWarriorAnt							54
  #define							kForagerAnt							55
  #define							kElderCentaur						56
  #define							kBlinkHound							57
  #define							kCube								58
  #define							kRedDraco							59
  #define							kIceDraco							60
  #define							kForestDraco						61
  #define							kBlackDraco				    		62
  #define							kTusker								63
  #define							kForestDragonAncient				64
  #define							kIceDragonAncient					65
  #define							kRedDragonAncient					66
  #define							kBlackDragonAncient					67
  #define							kLich								68
  #define							kDwarf								69
  #define							kLion								70
  #define							kGolem								71
  //Dave
  #define							kIceGolem							72
  #define							kMagmaGolem							73
  #define							kEarthElemental						74
  #define							kPixie								75
  #define							kSandGolem							76
  #define							kSandDragon							77
  #define							kSandDraco							78
  #define							kPhaseSerpent						79
  #define							kBlackBear							80
  #define							kBrownBear							81
  #define							kShark								82
  #define							kLizardManShaman					83
  #define							kHellMage							84
  #define							kWrath								85
  #define							kSkeletonArcher						86
    
  #define							kMaxCreatureTypes					87
	
	//-----------------------------------------------
	
	typedef struct creatureType {
		UInt16									id;
		UInt16 									row;	// saved
		UInt16 									col;	// saved
		UInt16									playerIndex;
		UInt16									npcIndex;
		TBoolean								hiding;	//
		unsigned long							nextMoveTime;
		unsigned long    	     				dieTime;
		UInt8									moveDirection;
		UInt8									facing;
		UInt8									strength;	// saved
		UInt8									intelligence;	// saved
		UInt8									dexterity;	// saved

		short									strengthOffset;	
		short									intelligenceOffset;	
		short									dexterityOffset;	

		unsigned long							strengthTime;
		unsigned long							intelligenceTime;
		unsigned long							dexterityTime;
		unsigned long         					lifespan;
		unsigned long         					calmUntil;
		unsigned long		  					outOfStable;
		UInt16					  				attackCounter;
		UInt8					  				attackTargets[kMaxMacroTrace][2];
		float									hitPoints;	// saved
		UInt8									magicPoints;	// saved
		UInt8									stamina; //saved
		int										specialAttackType;
		int										specialAttackArg1;
		int										specialAttackArg2;
		UInt8									armorRating;
		UInt8									alignment;
	  	UInt16									hunger;	// saved
	  	TBoolean								hungerfeedback;
		unsigned long							timeStamp;
		UInt8									strikePose;
		unsigned long							nextStrikeTime;
		UInt8									numberOfStrikes;
		UInt32									attackTarget;
		unsigned long							nextAttackTime;
		TBoolean								missedLastHit; // 0.1.4
		inventoryType							inventory[kMaxInventorySize];	// saved
		//TBoolean      	        			checkForUnwantedItems;
		UInt16									fx[kMaxCreatureEffects];
		UInt32									fxTarget[kMaxCreatureEffects];
		UInt16									fxTargetRow[kMaxCreatureEffects];
		UInt16									fxTargetCol[kMaxCreatureEffects];
		unsigned long							fxKillTime[kMaxCreatureEffects];
		UInt16									soundFX;
		unsigned long							soundFXTime;
		unsigned long       	  				soundID;
		UInt16                					finalStrikeSound;
		TBoolean								activeFX;
		UInt16									trackRow;
		UInt16									trackCol;
		TBoolean								tracking;
		UInt16									nextTrackRow;
		UInt16									nextTrackCol;
		UInt32              	  				possibleTarget;
		UInt32									master;
		UInt8									command;
		UInt8									poison;	//
		unsigned long							nextStatTime;
		unsigned long							paralyzed;	//
		unsigned long							disorientation;	//
		UInt8									reflectDamage;	//
		TBoolean								magicReflection;	//
		UInt8                 					beingAttacked;
		UInt16                					pathRow[kMaxPath];
		UInt16                					pathCol[kMaxPath];
		UInt8     	            				pathPtr;
		UInt8       	          				range;
		UInt16          	      				startRow;
		UInt16              	  				startCol;
		TBoolean              					respawn;
		unsigned long         					respawnTime;
		unsigned long							taunted;
		TBoolean              					cornered;
		TBoolean              					wounded;
		UInt16    	             				home;
		UInt8       	          				skipSpell;
		UInt8           	      				level;
		UInt32									petExp; //Experience points for pets
		TBoolean								godMode; //godmode (unkillable, huge damage, huge regen, etc)
		unsigned long							regeneratingTime;
		UInt8									regeneratingPower;
		short									goodTarget;
  		char									talk[kStringLength];
  		unsigned long							talkWhen;
  		unsigned long							talkTime;
  		unsigned long         					talkID;
  		UInt8									previousSituation;
  		TBoolean								mute;
  		TBoolean								noDrop; //Don't drop inventory if hit by guard
  		TBoolean								autoLock; //Body auto locked on death
  		unsigned long							opportunityTime;
  		TBoolean								noDeath; //No death
  		float									damageReduc;
  		float									magicResist; //Dave add 11-30-2010
	} creatureType;	

	typedef struct creatureTalk {
		UInt8									state;
		UInt8									language;
		UInt8									target;
		char									say[kStringLength];
	} creatureTalkType;

	typedef struct spawnRegionType {
		UInt8									creatureID;
		TRect									theRegion;
		int										currentPopulation;
		int										maxPopulation;
		int										range;
	} spawnRegionType;
	
	typedef struct ladderType {
		UInt16									sRow;
		UInt16									sCol;
		UInt16									dRow;
		UInt16									dCol;
	} latterType;

	typedef struct creatureControls {
		UInt8									bmp;
		UInt16									speed;
		UInt8									domain;
		UInt8									terrain;
		TBoolean								mobile;
		UInt16									maxHitPoints;
		UInt16									maxStrength;
		UInt16									maxIntelligence;
		UInt16									maxDexterity;
		UInt16									maxMagicPoints;
		UInt16									maxStaminaPoints;
		UInt8									alignment;
		TBoolean								directionalGraphics;
		TBoolean								pauseGraphics;
		TBoolean            				  	canMorph;
		int										graphicsIndex;
		UInt16									minGold;
		UInt16									maxGold;
		UInt8									corpseID;
		TBoolean								seekGold;
		TBoolean								seekMagic;
		TBoolean								bleeds;
		TBoolean								seekWeapons;
		float									magicResist; // put in carbon
		UInt8									tamingLevel;
		UInt16									inventoryItem[kMaxInventorySize];
		UInt8									inventoryMinCount[kMaxInventorySize];
		UInt8									inventoryMaxCount[kMaxInventorySize];
		UInt8									lockStrength;
		int										population;
		int										desiredPopulation;
		UInt16									randomSound;	// 1..254
		UInt16									attackSound;
		UInt8									attackRange;
		UInt16									attackDamageMin;
		UInt16									attackDamageMax;
		UInt16									armorRating;
		UInt16                 					poisonous;
		UInt8                 					curse;
		TBoolean              					immuneToPoison;
		TBoolean								meatEater;
		TBoolean								vegEater;
		TBoolean								frost;
		TBoolean								fire;
		UInt8                 					points;
		UInt8                 					undead;
		UInt16                					rareLevel;
		UInt16                					rareChance;
		UInt16                					color;
		TBoolean              					metal;
		TBoolean              					blunt;
		creatureTalkType						talkScript[kMaxTalkScripts];
		int										weaponID[kMaxWeaponModifiers];
		float									weaponModifier[kMaxWeaponModifiers];
		int										creatureLevel;
		int										size;
		int										trackingLevel;
		UInt16									mageryLevel; //Spellpower
		UInt8									spellcastChance;
  		int										spellList[kMaxCreatureSpells][3];
  		UInt8									selfhealChance;
  		UInt16									selfhealTrigger;
	} creatureControls;	

	typedef struct bloodStainType {
		UInt8									id;
		UInt16 									row;
		UInt16 									col;
		unsigned long							expireTime;
	} bloodStainType;	
	
//-----------------------------------------------
extern creatureType								creature			[kMaxCreatures];
extern creatureControls							creatureInfo		[kMaxCreatureTypes];

extern UInt8									bloodStainMap		[kMapWidth][kMapHeight];
extern bloodStainType							bloodStain			[kMaxBloodStains];

//--------------------------------------------------------------

void 		cr_InitCreatures					(void);
void 		cr_CreatureControls					(void);
int 		cr_SpawnCreature					(UInt16 id, UInt16 row, UInt16 col);
void 		cr_SpawnNPC							(UInt16 row, UInt16 col, UInt8 torso, UInt8 legs);
void 		cr_MoveCreatures					(void);
void 		cr_ClearCreatureRecord				(int	i);
TBoolean 	cr_InDomain							(int	i, int row, int col);
void 		cr_Dead								(int c);
int 		cr_DistanceBetween					(int c1, int c2);
void 		cr_GetNearestFreeLocation			(int i, UInt16 *row, UInt16 *col, char callingFunction[kStringLength]);
void 		cr_AddToCreatureControlInventory	(int creature, int item, int min, int max);
void 		cr_EmptySpotNear					(int i, UInt16 *row, UInt16 *col, TBoolean includeOrigin);
void 		cr_SteppedOnLatter					(int i, UInt16 *row, UInt16 *col);
int 		cr_DistanceFrom						(int c, int row, int col);
TBoolean 	cr_ObstacleBetween					(int Ax, int Ay, int Bx, int By, int domain);
TBoolean    cr_ViewBlocked                  	(int Ax, int Ay, int Bx, int By);
int         cr_TamedCreatureBetween   	      	(int Ax, int Ay, int Bx, int By);
void 		cr_CheckCreaturePopulation			(void);
TBoolean    cr_CreatureWantsItem  	    	    (int c, int item);
void        cr_setDieTime     	              	(int c, unsigned long theTime);
void        cr_CreatePath       	            (int c, int Ax, int Ay, int Bx, int By);
TBoolean    cr_LowMorale            	        (int c);
void        cr_ClearAttackTarget        	    (int i);
void        cr_GetTrackingMove              	(int i, int *row, int *col);
TBoolean    cr_Attack        	                (int c1, int c2);
int         cr_AttackRange      	            (int c);
void        cr_CreatePath2          	        (int x0, int y0, int x1, int y1);
TBoolean    cr_BuildAttackPath          	    (int s, int d, int *stepX, int *stepY);
void        cr_MoveCreatures2               	(void);
TBoolean    cr_ObstacleBetween2   	            (int x0, int y0, int x1, int y1, int domain, int *d, int c1, int c2, TBoolean fire);
void        cr_CheckForPetGuarding  	        (int c2, int c1);
TBoolean    cr_BuildPathTo              	    (int s, int drow, int dcol, int *stepX, int *stepY);
void        cr_GetSpotFarthestAway	         	(int c1, int c2, int *fleeRow, int *fleeCol);
void        cr_CheckForGuardingPets         	(int  c2, int c1);
TBoolean    cr_RustDamage                   	(int c, int item);
TBoolean    cr_BothInPVPZone                	(int c1, int c2);
TBoolean    cr_InPVPZone       	             	(int c);
TBoolean	cr_InTown							(int c);
void        cr_AddSound                     	(int c, int theSound);
TBoolean 	cr_AdjacentSpotInDomain				(int row, int col, int domain);
void        cr_GetCreatureRetreat           	(int c, int *fleeRow, int *fleeCol);
int         cr_MinimumDistanceToHuman       	(int r, int c);
int         cr_RealDistanceFrom             	(int c, int row, int col);
TBoolean    cr_AdjacentSpotHasHuman         	(int row, int col);
void        cr_GetSpotAwayFromHuman         	(int c, int *stepX, int *stepY);
TBoolean 	cr_AdjacentSpotHasNPCHuman			(int row, int col);
TBoolean 	cr_NearPlayer						(int c);
void 		cr_ReloadCreatures					(void);
void 		cr_ClearBloodStainRecord			(int	i);
void 		cr_DropBloodStain					(int theCreatureID, int row, int col);
void 		cr_ClearBloodStain					(int	i);
void		cr_CreatureMoveTo					(int i, UInt16 row, UInt16 col);
void 		cr_SpeakSpell						(int i, int t, int spell);
int			cr_SpecialAttack					(int c1, int c2, int param);
void		cr_ClearSpat						(int c, TBoolean send);
TBoolean	cr_CastCreatureSpell				(int c1, int c2);
TBoolean	cr_ImmuneToFF						(int type);

//--------------------------------------------------------------

#endif