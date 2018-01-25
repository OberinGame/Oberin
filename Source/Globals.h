#ifndef _GLOBALS  
#define _GLOBALS 

//  GM COMMANDS
// /nogain - player will have no skill game until logoff/logon
// /kills - shows previous X kills
// /resetkills - resets all kills
// /summon - brings player to GM
// /gmhome - olympus
// /addwrap x - adds to wrap sheet
// /showwrap - dumps wrap file


//	Version 0.1.1
//      - added demo mode
//      - demo players disappear when they die
//      - demo players can't drop items
//      - demo players can't give to other players
//      - demo players can't loot permanent players
//      - demo players will be attacked by monsters outside of town
//	     - demo can't loot bodies
//      - shows gold total in buy & sell displays
//      - creatures will not attack in town unless attacked first
//      - demo players cant steal from non demo players
//      - harmful spells can't be casted in town
//      - no evil creatures will spawn in town
//      - new temple
//      - name of corpse
//      - /bug
//      - fixed bank bug
//			- larger point in map view
//      - meditative trance bug fixed
//      - can no longer press dim buttons
//
//	Version 0.1.2
//      - added instant log off at inn *
//      - can no longer give more than you have *
//      - fixed grammatical errors
//      - demo can't drop by giving to NPCs *
//      - creatures with spawn spots will stay in general area*
//      - thieves steal less gold*
//      - stats don't drop when you enter new temple*
//      - added interrupt action when moving with path*
//
//	Version 0.1.3
//
//      - clear networking variables (player names) at each join

#include <MacTypes.h>

extern void sh_EndSession(void);

    //#define 						_SERVER
    //#define 						_SERVERONLY
  	//#define						_TESTSERVER
  //#define 						_USEAPPLETALK
  //#define 						_DEBUGGING
  	#define 						_USEIP
  //#define 						_LEVEL2DEBUGGING
  //#define 						_FUNCTIONTRACEDEBUGGING
 	#define 						_DONTATTACKGMS

	#define							kClericTarget											1
	#define							kDruidTarget											2
	#define							kFighterTarget											3
	#define							kRangerTarget											4
	#define							kRogueTarget											5
	#define							kWizardTarget											6
	#define							kHumanTarget											7
	#define							kPetTarget												8
	#define							kMorphedTarget											9
	
	#define             kMapWindow			              1			// map menu
	#define             kInventoryWindow                  2			// window menu
	#define             kPackContentsWindow               3			// window menu
	#define             kShopContentsWindow               4			// window menu
	#define             kSellListWindow                   5			// window menu
	#define             kIDWindow                         6			// window menu
	#define             kSkillsWindow                     7			// window menu
	#define             kStatsWindow                      8			// window menu
	#define             kChatWindow                       9			// window menu
	#define             kCharacterWindow                  10		// window menu
	#define             kMakeWindow                       11		// window menu
	#define             kChessWindow                      12		// window menu
	#define             kBankWindow                       13		// window menu
	#define             kPartyWindow                      14
	#define             kQuestWindow                      15
	#define             kPetsWindow                       16
	#define             kAboutWindow                      17
	#define				kCalendarWindow					  18

	#define             kMaxWindows                       20

	#define							kStringLength										  128
	#define							kMaxChatSlots                     200  //Dave 100
	
	#define							kMaxMacroTrace                    3

	#define							kServerVersion								43
	#define							kClientVersion								43
	#define							kClientType									0			// >0 can always log on

  #define 						kChatMessage				              0
  #define 						kPlayerMessage				            1
  #define 						kSystemMessage				            2

	#define							kMaxPlayers						200		// array range 1..500
	#define							kLogOnLimit						100		// only allow this number of regular players on ... save one spot for client type >0
	#define							kMaxCreatureEffects								3   //Dave 11-23-2012   kMaxCreatureEffects 3 
 	#define							kMaxLocalFX												25
	#define							kDataSize													10000 

	#define							kMaxIcons													1500 //Dave 1200
	
	#define							kStatMax													225
	#define							kWeightLimit                      18    // * strength for max carry weight
	#define							kThiefTime                        15 // can attack thief for x minutes after item stolen

	#define							kStayInWorldTime                  1
	#define							kPlayerSaveTime                   10

	#define							kMaxPath                          25

	#define							kMaxTrail													20
	#define							kMaxPets													5

  #define							kDropping										      1
  #define							kGiving											      2
  #define							kTaking											      3
  #define							kBuying											      4
  #define							kSelling										      5
  #define							kDepositing									      6
  #define							kWithdrawing								      7
  #define							kLocate								            8

	#define							kHeel															1
	#define							kStay															2
	#define							kGuard														3
	#define							kAttack														4
	#define							kFree															5
	#define							kMove															6
	#define							kSpeak														7
	
	#define							kTempleFire												90	// add new effects to parse okay check or they won't work
	#define							kLightEllipse											91  //93
	#define							kLightExplosion										92
	#define							kLightRing												93
	#define							kLightningBoltX										94
	#define							kLightningBoltReverse							95
	#define							kFireField												96
	#define							kSmallFire												97
	#define							kLightningBolt										98
	#define							kSparkle											99
	#define							kWaterFountain										100
	#define							kFireballX											101
	//#define							kExplosion2											102
	
	//--------------------102-110 RESTRICTED for fireball travel use---------------------//
	#define							kFireballX2											111
	#define							kFireblastX											112
	#define							kMMX												113
	//--------------------114-122 RESTRICTED for magic missile travel use----------------//
	#define							kMMX2												123
	#define							kFireblast3X										124
	#define							kFireblast4X										125
	#define							kFireblast5X										126
	#define							kMassHealX											127
	#define							kBuffStrX											128
	#define							kBuffAgiX											129
	#define							kBuffMindX											130
	#define							kBuffMRX											131
	#define							kBuffRDX											132
	#define							kResX												133
	#define							kExplosionX											134
	#define							kDomeX												135
	#define							kDome2X												136
	#define							kCreatureSummonX									137
	#define							kFBFieldX											138
	#define							kSpatSwordX											139
	#define							kBrackenRootX										140
	#define							kParaFX												141
	#define							kSparkle2											142
	#define							kSparkle3											143
	#define							kSparkle4											144
	#define							kHealFountain										145
	#define							kSteamFX											146

	#define							kCastleFountainFX									2500
	#define							kFireTestFX												2501

	#define							kTempleFire1FX										2502
	#define							kTempleFire2FX										2503
	#define							kTempleFire3FX										2504
	#define							kTempleFire4FX										2505
	#define							kTempleFire5FX										2506
	#define							kTempleFire6FX										2507
	#define							kTempleFire7FX										2508
	#define							kTempleFire8FX										2509
	#define							kTempleFire9FX										2510
	#define							kTempleFire10FX										2511
	#define							kTempleFire11FX										2512
	#define							kTempleFire12FX										2513
	#define							kTempleFire13FX										2514
	#define							kTempleFire14FX										2515
	
	#define							kMaxFX														1500 //Dave 1500
	
	#define 						kMapWidth													2000
	#define 						kMapHeight												2000

	#define 						kMaxItems													7000 //Dave 5000
	#define 						kMaxRareItems											2000 //Dave 200

	#define 						kMaxScreenElements									252
	#define 						kMaxSparkles										25
	#define 						kMaxContainerTypes									350 //Dave 150
	#define							kMaxTracksTypes										70
	#define							kMaxTracked											20
	
	#define 						kCharacterButton									0
	#define 						kInventoryButton									1
	#define 						kSkillsButton										2
	#define 						kPickUpButton										3
	#define 						kBankButton											4
	#define 						kBuyButton											5
	#define 						kSellButton											6
	#define 						kWarButton											7
	#define 						kIDButton											8 // remember to increase button array
	#define 						kLogOffButton										9
	#define 						kActionButton										10

	#define 						kActiveButton										0
	#define 						kPressedButton										1
	#define 						kDimButton											2

	#define 						kNoCreatureColor                  					0
	#define 						kRedDragonColor                   					1
	#define 						kWaterElementalColor              					2
	#define 						kIceDragonColor                   					3
	#define 						kPoisonElementalColor             					4
	#define 						kBrownWolfColor             						5
	#define 						kBlackWolfColor             						6
	#define 						kFireElementalColor              					7
	#define 						kForestDragonColor                					8
  	#define 						kBlackDragonColor                 					9
  	#define 						kSandSpiderColor                  					10
  	#define							kPinkDragonColor				  					11
  	#define							kCreatureColorBlue
  		
  	#define							kMaxMacros											100
  
	// ----- Player Skills ---------------------------------
	
	#define							kAlchemyCleric										0
	#define							kAlchemyDruid										1
	#define							kAnatomy											2
	#define							kArchery											3
	#define							kBlacksmithing										4
	#define							kCooking											5
	#define							kDetectingHidden									6
	#define							kEnchanting											7
	#define							kFishing											8
	#define							kHealing											9
	#define							kHiding												10
	#define							kLockpicking										11
	#define							kLumberjacking										12
	#define							kMageryCleric										13
	#define							kMageryDruid										14
	#define							kMageryWizard										15
	#define							kMeditation											16
	#define							kMelee												17
	#define							kMining												18
	#define							kParrying											19
	#define							kPoisoning											20
	#define							kRemoveTrap											21
	#define							kResistingMagic										22
	#define							kSpecialFighter										23
	#define							kSpecialRanger										24
	#define							kSpecialRogue										25
	#define							kStealth											26
	#define							kTailoringLeatherworking							27
	#define							kTaming												28
	#define							kTinkering											29
	#define							kTracking											30
	#define							kWoodworking										31
	#define							kWrestling											32
	
	#define							kMaxSkills											33
	
	#define							kSkillCap											700
	
	#define							kMaxSkillLevel										1000000000

	// ----- Weapon Types ----------------------------------
	
	#define							kMaceFighting										0
	#define							kSwordsmanship										1
	#define							kDaggers											2
	#define							kPoleFighting										3
	#define							kArcheryFighting									4
	
	// ----- Special Attack Types --------------------------
	
	#define							kSpatDirectDamage									1
	#define							kSpatAttackSpeed									2
	#define							kSpatDamageAbsorb									3
	
	// ----- Player Skills ---------------------------------
/*	
//	#define							kAgriculture											0//
	#define							kPoisoning													0
	#define							kAnatomy													1
	#define							kAnimalLore												2
	#define							kArchery													3
	#define							kBlacksmithing											4
	#define							kBowcraftFletching								5
	#define							kCooking													6
	#define							kDetectingHidden									7
	#define							kEvaluatingIntelligence						8
	#define							kFishing													9
	#define							kHealing													10
	#define							kHiding														11
//	#define							kLanguage													13//
	#define							kLockpicking											12
	#define							kLumberjacking										13
	#define							kMaceFighting											14
	#define							kMageryWizard														15

	#define							kMagicLore												16
	
	#define							kMeditation												17
	#define							kMining														18
	#define							kTracking												19
	#define							kParrying													20
	#define							kPoleFighting											21
	#define							kRemoveTrap												22
	#define							kResistingMagic										23
	#define							kSwordsmanship										24
	#define							kTailoringLeatherworking					25
	#define							kTaming														26
	#define							kStealth													27
	#define							kTinkering												28
//	#define							kTracking													30//
	#define							kVoodoo														29
//	#define							kWeaponLore												32
	#define							kWoodworking											30
	#define							kWrestling												31
	
	#define							kMaxSkills												32
	
	#define							kSkillCap													700
*/	
	// ------------------------------------------------------

	#define							kEvil															0
	#define							kGood															1
	#define							kNeutral													2

	// ------------------------------------------------------

	#define							kNoTarget													1000
	#define							kCreatureTarget										1001
	#define							kInventoryTarget									1002
	#define							kGiveTarget												1003
	
	#define							kRemoveCurseTarget								214
	#define             kIPTarget                   	    215
	#define             kPitTarget                   	    216
	#define             kGainTarget                   	  217
	#define             kNoGainTarget                   	218
	#define             kResetTarget                   	  219
	#define             kKickTarget                   	  220
	#define             kJailTarget                   	  221
	#define             kBanTarget                   	    222
	#define             kAKATarget                   	    223
	#define             kBanishTarget                   	224
	#define             kResTarget                   			225
	#define             kHomeTarget                   		226
	#define             kActivateTarget                   227
	#define							kResurrectTarget									228
	#define							kFullHealTarget										229
	#define							kFullDetoxifyTarget								230
	#define							kClumsySpellTarget						    231
	#define							kAgilitySpellTarget						    232
	#define							kWeakenSpellTarget						    233
	#define							kStrengthSpellTarget						  234
	#define							kSimpleMindSpellTarget						235
	#define							kElevateMindTarget						    236
	#define							kMagicReflectionTarget						237
	#define							kDisorientationTarget							238
	#define							kReflectiveArmorTarget						239
	#define							kDrainManaTarget									240
	#define							kTeleportTarget										241
	#define							kParalyzeTarget										242
	#define							kNightVisionTarget								243
	#define							kInvisibilityTarget								244
	#define							kExplosionTarget									245
	#define							kGreaterDetoxifyTarget						246
	#define							kLesserDetoxifyTarget							247
	#define							kPoisonTarget											248
	#define							kGreaterHealTarget								249
	#define							kTameTarget												250
	#define							kIDTarget													251
	#define							kFireFieldTarget									252
	#define							kLesserHealTarget									253
	#define							kLightningBoltTarget							254
	#define							kAttackTarget											255
	#define							kMuteTarget									256
	#define							kUnmuteTarget								257
	#define							kKillTarget									258
	#define							kClassTarget								259
	#define							kMakeEvilTarget								260
	#define							kMakeGoodTarget								261
	#define							kMsayTarget									262
	#define							kSkillsTarget									263
	#define							kNPCITarget									264
	#define							kFireballTarget								265
	#define							kMagicMissileTarget							266
	#define							kFireblastTarget							267
	#define							kFireblast3Target							268
	#define							kFireblast4Target							269
	#define							kFireblast5Target							270
	#define							kOpportunityTarget							271
	#define							kDressupTarget								272
	#define							kDumpInvTarget								273
	#define							kDumpBankTarget								274
	
	#define							kMaxActions									274

	#define							kTargetPlayArea										1
	#define							kTargetInventoryArea							2
	
	// ------------------------------------------------------
	
	// ----- Messages ---------------------------------------
	#define							kNoLumberHarvest									1
	#define							kHidingFailed											2
	#define							kYouAreHidden											3
	#define							kVeryWeak													4
	#define							kWeak															5
	#define							kAverageStrength									6
	#define							kStrong														7
	#define							kVeryStrong												8
	#define							kExtremelyStrong									9
	#define							kAnatomyFailed										10
	#define							kAnimalLoreFailed									11
	#define							kLooksAggressive									12
	#define							kEvaluatingIntelligenceFailed			13
	#define							kVeryStupid												14
	#define							kStupid														15
	#define							kAverageIntelligence							16
	#define							kIntelligent											17
	#define							kVeryIntelligent									18
	#define							kExtremelyIntelligent							19
	#define							kNoFishCaught											20
	#define							kStillHungry											21
	#define							kLessHungry												22
	#define							kFeelingFull											23
	#define							kVeryFull													24
	#define							kCantEatMore											25

	#define							kVeryPoorCondition								26
	#define							kPoorCondition										27
	#define							kFairCondition										28
	#define							kGoodCondition										29
	#define							kVeryGoodCondition								30
	#define							kLikeNewCondition									31
	#define							kMagicLoreFailed									32
	#define							kConcentrationDisturbed						33
	
	#define							kApplyingBandages									34
	#define							kBeingBandaged										35
	#define							kHealingFailed										36
	#define							kYouHaveBeenPoisoned							37
	
	#define							kNoOreHarvest											38
	
	#define							kHisPackFull											39
	#define							kCantDropHere											40
	#define							kNotEnoughGold										41
	#define							kCantBankHere											42

	#define							kFailedToMake											43
	#define							kSuccessfulMake										44
	#define							kYourPackFull											45
	#define							kCatchThief												46
	#define							kStealthFailed										47

	#define							kSpellFizzles											48
	#define							kNotEnoughReagents								49

	#define							kAlreadyTame											50
	#define							kTamingFailed											51

	#define							kHiddenRevealed										52
	#define							kHiddenFailed											53

	#define							kCantSeeTarget										54
	#define							kCantGiveToGhost									55
	
	#define							kYouAreParalyzed									56
	#define							kYouArePoisoned										57
	#define							kYouAreDisoriented								58

	#define							kVeryHappy								        59
	#define							kSomewhatHappy								    60
	#define							kSomwhatUnhappy								    61
	#define							kVeryUnhappy								      62
	#define							kContainerLocked								  63
	#define							kBeastHasBeenTamed								64
	#define							kOrdinaryItem								      65

	#define							kNoContainer								      66
	#define							kCantPickMagicLock								67
	#define							kNotLocked								        68
	#define							kContainerUnlocked								69
	#define							kLockpickingFailed								70

	#define							kNoContainerToDisarm							71
	#define							kCantDisarmMagicTrap							72
	#define							kNotTrappeed								      73
	#define							kContainerDisarmed								74
	#define							kRemoveTrapFailed								  75
	
	#define							kMagicLoreSuccess								  76
	#define							kCantAttackInTown								  77
	#define							kCreatureTooFarAway               78
	#define							kPersonCantCarryThatMuch          79
	#define							kSomeoneTryingToSteal             80
	#define							kHungry                           81
	#define							kCantCastInTown								    82
	#define							kLooksNonAggressive							  83
	
	#define							kCantAttackCreature								85
	#define							kNoMoreFerrite								    86
	#define							kNoMoreLogs								        87
	#define							kGMOnline								          88
	#define							kNoGMOnline								        89
  #define							kPlayerNotOnline								  90
  #define							kQuestFinished                    91
  #define             kTooManyCreatures                 92
  #define             kNoManaForSpell                   93
  #define             kNoManaForPotion                  94
  
  #define             kNoOwner                          95
  #define             kEnterPVP                         96
  #define             kExitPVP                          97
  #define             kEnterBadlands                    98
  #define             kExitBadlands                     99
  #define             kBeware                           100
  #define							kYouHaveBeenCursed								101
  #define					kLeavingStealth										102
  #define				kBackstabDone0											103
  #define				kBackstabbed											104
  #define				kBackstabFailed											105
  #define				kEnteringStealth										106
  #define				kCriticalDone											107
  #define				kHitMissed												108
  #define				kTrackingFailed											109
  #define				kPetStarving											110
  #define				kPetDies												111
  #define				kCurseRemoved											112
  #define				kNoManaForEnchant										113
  #define				kNotEnoughStaminaMsg									114
  #define				kRewardPackFull											115
  #define				kDefenseBegin											116
  #define				kDefenseEnd												117
  #define				kExhausted												118
  #define				kVialOfAcidMsg											119
  #define				kKnockback												120
  #define				kBackstabDone1											121
  #define				kBackstabDone2											122
  #define				kTaunt													123
  #define				kEscapeMove												124
  #define				kAimedShot												125
  #define				kGrapplingShot											126
  #define				kBoomingBlow											127
  #define				kRangerSwiftness										128	
  #define				kQuickDrawMsg											129
  #define				kNoQuickDrawMsg											130
  #define				kMustBeBehindTarget										131
  #define				kMustBeStealthed										132
  #define				kYouRolled												133
	
	// ------------------------------------------------------
	
	
	#define 						PrefsName    											"\pOberin Prefs (cb)"
	#define 						Creator														'bCVI'
	#define 						FileType													'obDC'

	#define 						kScreenDepth											16
	#define 						kMaskDepth												1
	
#endif