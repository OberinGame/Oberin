#include "Inventory.h"
#include "Toolbox.h"
#include "Common.h"
#include "Creatures.h"
#include "Players.h"
#include "Graphics.h"
#include "Items.h"
#include "Shops.h"
#include "FX.h"
#include "Skills.h"
#include "FileIO.h"
#include "Quest.h"
#include "Async Sound.h"

#ifdef _SERVER

TRect								bankRect									[kMaxBanks];
TRect								templeRect								[kMaxTemples];
TRect								portalRect								[kMaxPortals];
int									portalItem								[kMaxPortals];
int									portalRow									[kMaxPortals];
int									portalCol									[kMaxPortals];

UInt16							containerMap				      [kMapWidth][kMapHeight];
UInt16							tracksMap				      [kMapWidth][kMapHeight];
inventoryType				container						      [kMaxContainers][kMaxInventorySize];
unsigned long				containerDecayTime	      [kMaxContainers];
unsigned long				tracksDecayTime	      [kMaxTracks];
char								tracksNote             [kMaxTracks][100];
TBoolean						containerEmpty			      [kMaxContainers];
int									containerRow				      [kMaxContainers];
int									containerCol				      [kMaxContainers];
UInt8								containerID					      [kMaxContainers];
int									tracksRow				      	  [kMaxTracks];
int									tracksCol				     	  [kMaxTracks];
UInt8								tracksID					      [kMaxTracks];
UInt8								containerLock				      [kMaxContainers];
UInt8								containerTrap				      [kMaxContainers];
char                containerTrapName         [kMaxContainers][20];
UInt8								containerMagicLock	      [kMaxContainers];
UInt8								containerMagicTrap	      [kMaxContainers];
char                containerMagicTrapName    [kMaxContainers][20];
UInt8								containerHidden	          [kMaxContainers];
char								containerName             [kMaxContainers][48]; //[15];
char								containerGift             [kMaxContainers][48]; //[15];
char								tracksGift             [kMaxContainers][48];
TBoolean						containerGM			          [kMaxContainers];

unsigned long       treasureSpawnTime;

extern creatureType	creature						[kMaxCreatures];
extern UInt16				creatureMap					[kMapWidth][kMapHeight];
extern playerType		player							[kMaxPlayers];
extern TBoolean			containerIsCorpse		[kMaxContainerTypes];
extern TBoolean     containerIsTreasure [kMaxContainerTypes];
extern TBoolean     gmCanSpawn          [kMaxContainerTypes];
extern TBoolean     gmProp          		[kMaxContainerTypes];

int                 ginsengSpawn;
int                 lotusFlowerSpawn;
int                 mandrakeRootSpawn;
int                 garlicSpawn;
int                 volcanicAshSpawn;
int                 mushroomSpawn;//Dave
//int                 eggSpawn;//Dave
int                 currentNumberOfContainers;
int                 currentNumberOfTracks;
int                 dungeonTreasureToSpawn;
int                 fireDungeonTreasureToSpawn;
int                 sewerTreasureToSpawn;
int                 lizardTreasureToSpawn;
int                 iceTreasureToSpawn;
#endif


inventoryType				makeList						[kMaxInventorySize];
inventoryType				localInventory			[kMaxInventorySize];
inventoryType				pickUpInventory			[kMaxInventorySize];
inventoryType				bankInventory				[kMaxInventorySize];
UInt16							containerState			[kMaxScreenElements];
UInt16							tracksState			[kMaxScreenElements];
UInt16							bloodStainState			[kMaxScreenElements];
UInt8								human								[kMaxScreenElements][19];
TBoolean						inventorySlotArmed	[kMaxInventorySize];
extern 							TBoolean						canWalkOn[kMaxIcons];
extern UInt16				map									[kMapWidth][kMapHeight];
extern UInt8				localRightHand;
extern UInt8				localLeftHand;
extern UInt8				localLegs;
extern UInt8				localTorso;
extern UInt8				localFeet;
extern UInt8				localArms;
extern UInt8				localRightFinger;
extern UInt8				localLeftFinger;
extern UInt8				localHead;
extern UInt8				localNeck;
extern UInt8				localHands;
extern UInt8				localWaist;
extern UInt8				localMount;
extern TBoolean			canSwimIn[kMaxIcons];						// specifies whether object is water
extern TBoolean			doorway[kMaxIcons];
extern TBoolean			sign[kMaxIcons];
extern char			    localContainerName[kStringLength]; //[15];

extern int					currentInfoDisplay;
extern itemType			itemList						[kMaxItems];
extern UInt16				localWeight;
extern int					playerAction;	// used for dropping multiple items
extern char					playerText[kStringLength];    //[kCommandStringMaxLength];
extern int          	useSlot;
extern int          	currentQuestSlot;
extern questType		quest[kMaxQuests];

extern UInt8				localClass;
extern TRect				skillsWindowRect;
extern WindowPtr			skillsWindow;
extern TPoint       		windowPosition[kMaxWindows];
extern int					maxSkillLevel[7][kMaxSkills];
extern TBoolean				questChar[kMaxScreenElements];



// ----------------------------------------------------------------- //
//void in_GetItemName(int id, char itemName[48], int count)
void in_GetItemName(int id, char itemName[kStringLength], int count)
// ----------------------------------------------------------------- //

{
	// count=0 - return just singular of name											ex:  Katana (+10)
	// count=1 - return count + singular of name									ex:  1 Katana
	// count>1 - return count + plural of name										ex:  2 Katanas
	// count=-1 - return just singular of name										ex:  Katana
	// count=-2 - return just singular of name/lower case					ex:  katana
	// count=kMaxInventoryItemCount+1 - return just singular of name	+ GM							ex:  GM Katana

	char									conditionStr[kStringLength];//[32];
	int										i;


	if (id==0)
	{
		strcpy(itemName,"zzzzzzzzzz");
		return;
	}

	strcpy(itemName,"");

	if (id<2000)  // not key, lock, or quest
	{
		if ((!in_CanGroup(id)) && (count>127))
		{
			if (count==(kMaxInventoryItemCount+1))
				count=-1;
			else
				count=count-128;

			strcpy(itemName,"GM ");
		}
		else if ((in_CanGroup(id)) && (count>1))
		{
			tb_IntegerToString(count,	itemName);
			strcat(itemName," ");
		}
	}

	if ((count>1) && (in_CanGroup(id)))
	{

		if ((id>=4000) && (id<5000))      strcat(itemName,"Scroll");
		else if (id==kKindling)					  strcat(itemName,"Bundles of Kindling");
		else if (id==kFishSteak)					strcat(itemName,"Fish Steaks");
		else if (id==kCookedSteak)				strcat(itemName,"Steaks");
		else if (id==kRawSteak)						strcat(itemName,"Raw Steaks");
		else if (id==kLoafOfBread)				strcat(itemName,"Loaves of Bread");
		else if (id==kCandy)				      strcat(itemName,"Pieces of Candy");

		else if (id==kRedEgg)				      strcat(itemName,"Red Eggs");
		else if (id==kPurpleEgg)				  strcat(itemName,"Purple Eggs");
		else if (id==kGreenEgg)				    strcat(itemName,"Green Eggs");
		else if (id==kBlueEgg)				    strcat(itemName,"Blue Eggs");
		else if (id==kBlackEgg)				    strcat(itemName,"Black Eggs");
		
		else if (id==kBlueFlag)				    strcat(itemName,"Blue Flags");
		else if (id==kRedFlag)				    strcat(itemName,"Red Flags");
		else if (id==kGreenFlag)				  strcat(itemName,"Green Flags");
		else if (id==kYellowFlag)				  strcat(itemName,"Yellow Flags");

		else if (id==kApple)							strcat(itemName,"Apples");
		else if (id==kBanana)							strcat(itemName,"Bananas");

		else if (id==kOrbOfSeeing)				strcat(itemName,"Orbs of Seeing");
		else if (id==kRedRose)						strcat(itemName,"Red Roses");
		else if (id==kYellowRose)					strcat(itemName,"Yellow Roses");
		else if (id==kBlackRose)					strcat(itemName,"Black Roses");
		//Dave
		else if (id==kBlueRose)					strcat(itemName,"Blue Roses");
		else if (id==kPurpleRose)					strcat(itemName,"Purple Roses");
		
		
		else if (id==kSpiderFang)		      strcat(itemName,"Spider Fangs");
		else if (id==kScorpionStinger)		strcat(itemName,"Scorpion Stingers");

		else if (id==kGrayWizardHat)			strcat(itemName,"Gray Wizard Hats");
		else if (id==kLightBlueWizardHat)	strcat(itemName,"Light Blue Wizard Hats");
		else if (id==kRedWizardHat)				strcat(itemName,"Red Wizard Hats");
		else if (id==kBlueWizardHat)			strcat(itemName,"Blue Wizard Hats");
		else if (id==kBrownWizardHat)			strcat(itemName,"Brown Wizard Hats");
		else if (id==kBlackWizardHat)			strcat(itemName,"Black Wizard Hats");
		else if (id==kGreenWizardHat)			strcat(itemName,"Green Wizard Hats");
		else if (id==kPurpleWizardHat)		strcat(itemName,"Purple Wizard Hats");
		else if (id==kYellowWizardHat)		strcat(itemName,"Yellow Wizard Hats");

		else if (id==kGrayJesterHat)			strcat(itemName,"Gray Jester Hats");
		else if (id==kLightBlueJesterHat)	strcat(itemName,"Light Blue Jester Hats");
		else if (id==kRedJesterHat)				strcat(itemName,"Red Jester Hats");
		else if (id==kBlueJesterHat)			strcat(itemName,"Blue Jester Hats");
		else if (id==kBrownJesterHat)			strcat(itemName,"Brown Jester Hats");
		else if (id==kBlackJesterHat)			strcat(itemName,"Black Jester Hats");
		else if (id==kGreenJesterHat)			strcat(itemName,"Green Jester Hats");
		else if (id==kPurpleJesterHat)		strcat(itemName,"Purple Jester Hats");
		else if (id==kYellowJesterHat)		strcat(itemName,"Yellow Jester Hats");

		else if (id==kGoldCrown)				  strcat(itemName,"Gold Crowns");

		else if (id==kEmptyBottle)				strcat(itemName,"Empty Bottles");
		else if (id==kSextant)				    strcat(itemName,"Sextants");
		else if (id==kShears)				      strcat(itemName,"Pairs of Shears");
		else if (id==kRatTail)				    strcat(itemName,"Rat Tails");
		else if (id==kLog)								strcat(itemName,"Logs");
		else if (id==kFerriteOre)					strcat(itemName,"Ferrite");
		else if (id==kEtherite)						strcat(itemName,"Etherite");
		else if (id==kEtheriteWood)					strcat(itemName,"Extracted Etherite from the Log");
		else if (id==kEtheriteFerrite)				strcat(itemName,"Extracted Etherite from the Ferrite");
		//Dave
		else if (id==kBlackEtherite)				strcat(itemName,"Extracted Etherite from the Black Ferrite");
		else if (id==kIngot)					    strcat(itemName,"Ingots");
		else if (id==kFishingPole)				strcat(itemName,"Fishing Poles");
		else if (id==kLobsterPole)				strcat(itemName,"Lobster Poles");
		else if (id==kEelPole)				    strcat(itemName,"Eel Poles");
		else if (id==kTrout)							strcat(itemName,"Trout");
		else if (id==kRaft)				        strcat(itemName,"Rafts");
		else if (id==kTrap)					      strcat(itemName,"Traps");
		else if (id==kLock)					      strcat(itemName,"Locks");
		else if (id==kBandage)						strcat(itemName,"Bandages");
		else if (id==kEelPie)							strcat(itemName,"Eel Pies");
		else if (id==kLobsterPie)					strcat(itemName,"Lobster Pies");
		else if (id==kDriedTrout)					strcat(itemName,"Dried Trout");
		else if (id==kApplePie)						strcat(itemName,"Apple Pies");
		else if (id==kGingerBread)				strcat(itemName,"Ginger Breads");
		else if (id==kOatMealCake)				strcat(itemName,"Oat Meal Cakes");
		else if (id==kLobsterSoup)				strcat(itemName,"Lobster Soups");
		else if (id==kMugOfAle)				    strcat(itemName,"Mugs Of Ale");
		else if (id==kBoiledLobster)			strcat(itemName,"Boiled Lobsters");
		else if (id==kBagOfFlour)					strcat(itemName,"Bags of Flour");
		else if (id==kBananaPie)					strcat(itemName,"Banana Pies");
		else if (id==kRoastedGaperStalk)	strcat(itemName,"Roasted Gaper Stalks");

		else if (id==kLeatherBelt)				strcat(itemName,"Leather Belts");
		else if (id==kAncientTome)				strcat(itemName,"Ancient Tomes");
		else if (id==kIceCrystal)				  strcat(itemName,"Ice Crystals");

		else if (id==kGoldMirror)				  strcat(itemName,"Gold Mirrors");
		else if (id==kBlueTicket)					strcat(itemName,"Blue Tickets");
		else if (id==kBookOfTheDead)			strcat(itemName,"Books of the Dead");
		else if (id==kMagicRune)				  strcat(itemName,"Magic Runes");
		else if (id==kSilverDragonClaw)		strcat(itemName,"Silver Dragon Claws");
		else if (id==kRubyHeart)				  strcat(itemName,"Ruby Hearts");
		else if (id==kBookOfTheStars)			strcat(itemName,"Books of the Stars");
		else if (id==kKingsSeal)				  strcat(itemName,"King's Seals");
		else if (id==kCrystalFlask)				strcat(itemName,"Crystal Flasks");
		else if (id==kSilverLeaf)				  strcat(itemName,"Silver Leaves");
		else if (id==kTomeOfTheDamned)	  strcat(itemName,"Tomes of the Damned");
		else if (id==kTomeOfLife)				  strcat(itemName,"Tomes of Life");
		else if (id==kGoldenFleece)				strcat(itemName,"Golden Fleeces");
		else if (id==kGuildHallDeed)			strcat(itemName,"Guild Hall Deeds");
		else if (id==kHouseDeed)			    strcat(itemName,"House Deeds");

		else if (id==kCrystalSkull)				strcat(itemName,"Shattered Crystal Skulls");
		else if (id==kCrystalSkullPure)			strcat(itemName,"Crystal Skull");
		else if (id==kKingsPortrait)			strcat(itemName,"King's Portraits");
		else if (id==kPersianRug)				  strcat(itemName,"Persian Rugs");

		else if (id==kGrayShirt)					strcat(itemName,"Gray Shirts");
		else if (id==kLightBlueShirt)			strcat(itemName,"Light Blue Shirts");
		else if (id==kRedShirt)						strcat(itemName,"Red Shirts");
		else if (id==kBlueShirt)					strcat(itemName,"Blue Shirts");
		else if (id==kBrownShirt)					strcat(itemName,"Brown Shirts");
		else if (id==KBlackShirt)					strcat(itemName,"Black Shirts");
		else if (id==kGreenShirt)					strcat(itemName,"Green Shirts");
		else if (id==kPurpleShirt)				strcat(itemName,"Purple Shirts");
		else if (id==kYellowShirt)				strcat(itemName,"Yellow Shirts");

		else if (id==kLightBlueBoots)			strcat(itemName,"Pairs of Light Blue Boots");
		else if (id==kRedBoots)						strcat(itemName,"Pairs of Red Boots");
		else if (id==kBlueBoots)					strcat(itemName,"Pairs of Blue Boots");
		else if (id==kBrownBoots)					strcat(itemName,"Pairs of Brown Boots");
		else if (id==kBlackBoots)					strcat(itemName,"Pairs of Black Boots");
		else if (id==kGreenBoots)					strcat(itemName,"Pairs of Green Boots");
		else if (id==kPurpleBoots)				strcat(itemName,"Pairs of Purple Boots");
		else if (id==kYellowBoots)				strcat(itemName,"Pairs of Yellow Boots");
		else if (id==kGrayBoots)					strcat(itemName,"Pairs of Gray Boots");

		else if (id==kGrayPants)					strcat(itemName,"Pairs of Gray Pants");
		else if (id==kLightBluePants)			strcat(itemName,"Pairs of Light Blue Pants");
		else if (id==kRedPants)						strcat(itemName,"Pairs of Red Pants");
		else if (id==kBluePants)					strcat(itemName,"Pairs of Blue Pants");
		else if (id==kBrownPants)					strcat(itemName,"Pairs of Brown Pants");
		else if (id==KBlackPants)					strcat(itemName,"Pairs of Black Pants");
		else if (id==kGreenPants)					strcat(itemName,"Pairs of Green Pants");
		else if (id==kPurplePants)				strcat(itemName,"Pairs of Purple Pants");
		else if (id==kYellowPants)				strcat(itemName,"Pairs of Yellow Pants");

		else if (id==kRoyalRobe)				  strcat(itemName,"Royal Robes");
		else if (id==kGMRobe)						  strcat(itemName,"GM Robes");
		else if (id==kGrayRobe)						strcat(itemName,"Gray Robes");
		else if (id==kLightBlueRobe)			strcat(itemName,"Light Blue Robes");
		else if (id==kRedRobe)						strcat(itemName,"Red Robes");
		else if (id==kBlueRobe)						strcat(itemName,"Blue Robes");
		else if (id==kBrownRobe)					strcat(itemName,"Brown Robes");
		else if (id==KBlackRobe)					strcat(itemName,"Black Robes");
		else if (id==kGreenRobe)					strcat(itemName,"Green Robes");
		else if (id==kPurpleRobe)					strcat(itemName,"Purple Robes");
		else if (id==kYellowRobe)					strcat(itemName,"Yellow Robes");
		else if (id==kOrangeRobe)					strcat(itemName,"Orange Robes");
		else if (id==kWhiteRobe)					strcat(itemName,"White Robes");

		else if (id==kGrayRobeProtection)						strcat(itemName,"Gray Robes of Protection");
		else if (id==kLightBlueRobeProtection)			strcat(itemName,"Light Blue Robes of Protection");
		else if (id==kRedRobeProtection)						strcat(itemName,"Red Robes of Protection");
		else if (id==kBlueRobeProtection)						strcat(itemName,"Blue Robes of Protection");
		else if (id==kBrownRobeProtection)					strcat(itemName,"Brown Robes of Protection");
		else if (id==kBlackRobeProtection)					strcat(itemName,"Black Robes of Protection");
		else if (id==kGreenRobeProtection)					strcat(itemName,"Green Robes of Protection");
		else if (id==kPurpleRobeProtection)					strcat(itemName,"Purple Robes of Protection");
		else if (id==kYellowRobeProtection)					strcat(itemName,"Yellow Robes of Protection");

		else if (id==kGrayRobeResistance)						strcat(itemName,"Gray Robes of Resistance");
		else if (id==kLightBlueRobeResistance)			strcat(itemName,"Light Blue Robes of Resistance");
		else if (id==kRedRobeResistance)						strcat(itemName,"Red Robes of Resistance");
		else if (id==kBlueRobeResistance)						strcat(itemName,"Blue Robes of Resistance");
		else if (id==kBrownRobeResistance)					strcat(itemName,"Brown Robes of Resistance");
		else if (id==KBlackRobeResistance)					strcat(itemName,"Black Robes of Resistance");
		else if (id==kGreenRobeResistance)					strcat(itemName,"Green Robes of Resistance");
		else if (id==kPurpleRobeResistance)					strcat(itemName,"Purple Robes of Resistance");
		else if (id==kYellowRobeResistance)					strcat(itemName,"Yellow Robes of Resistance");

		else if (id==kLobster)						strcat(itemName,"Lobsters");
		else if (id==kSkull)						  strcat(itemName,"Skulls");

		//else if (id==kMiningShovel)				strcat(itemName,"Mining Shovels");
		else if (id==kBlacksmithHammer)	  strcat(itemName,"Blacksmith Hammers");
		else if (id==kCarvingKnife)	      strcat(itemName,"Carving Knives");
		else if (id==kMiningAxe)					strcat(itemName,"Mining Axes");
		else if (id==kCoalAxe)					  strcat(itemName,"Coal Axes");

		else if (id==kHandAxe)					  strcat(itemName,"Hand Axes");
		else if (id==kHandAxeDurability)  strcat(itemName,"Hand Axes of Durability");
		else if (id==kHandAxePower)				strcat(itemName,"Hand Axes of Power");
		else if (id==kHandAxeSpeed)				strcat(itemName,"Hand Axes of Speed");
		else if (id==kHandAxeSilver)			strcat(itemName,"Silver Hand Axes");
		else if (id==kHandAxePoison)			strcat(itemName,"Poison Hand Axes");

		else if (id==kCentaurMiningAxe)		strcat(itemName,"Centaur Mining Axes");
		else if (id==kDwarvenHandAxe)			strcat(itemName,"Dwarven Hand Axes");
		else if (id==kAmberAxe)			      strcat(itemName,"Amber Axes");
		else if (id==kWoodworkingTools)		strcat(itemName,"Sets of Woodworking Tools");
		else if (id==kTinkeringTools)			strcat(itemName,"Sets of Tinkering Tools");
		else if (id==kPackOfNails)				strcat(itemName,"Packs of Nails");
		else if (id==kBoard)							strcat(itemName,"Boards");
		else if (id==kArrow)							strcat(itemName,"Arrows");
		else if (id==kBolt)								strcat(itemName,"Bolts");

		//else if (id==kBlankScroll)				strcat(itemName,"Blank Scrolls");

		else if (id==kLesserHealPotion)		strcat(itemName,"Lesser Heal Potions");
		else if (id==kGreaterHealPotion)	strcat(itemName,"Greater Heal Potions");
		else if (id==kFullHealPotion)			strcat(itemName,"Full Heal Potions");

		else if (id==kManaPotion)					strcat(itemName,"Mana Potions");

		else if (id==kLesserDetoxifyPotion)		strcat(itemName,"Lesser Detoxify Potions");
		else if (id==kGreaterDetoxifyPotion)	strcat(itemName,"Greater Detoxify Potions");
		else if (id==kFullDetoxifyPotion)			strcat(itemName,"Full Detoxify Potions");

		else if (id==kStrengthPotion)			strcat(itemName,"Strength Potions");
		else if (id==kAgilityPotion)			strcat(itemName,"Agility Potions");
		else if (id==kElevateMindPotion)	strcat(itemName,"Elevate Mind Potions");
		else if (id==kNightVisionPotion)	strcat(itemName,"Night Vision Potions");
		else if (id==kOpportunityPotion)	strcat(itemName,"Opportunity Potions");
		else if (id==kInvisibilityPotion)	strcat(itemName,"Invisibility Potions");
		else if (id==kNourishPotion)			strcat(itemName,"Nourish Potions");

		else if (id==kPoison)	            strcat(itemName,"Bottles of Poison");

		else if (id==kDiamond)						strcat(itemName,"Diamonds");

		else if (id==kGreenCrystal)				strcat(itemName,"Green Crystals");
		else if (id==kRedCrystal)					strcat(itemName,"Red Crystals");
		else if (id==kBlueCrystal)				strcat(itemName,"Blue Crystals");
		else if (id==kPurpleCrystal)			strcat(itemName,"Purple Crystals");
		else if (id==kOrangeCrystal)			strcat(itemName,"Orange Crystals");
		else if (id==kYellowCrystal)			strcat(itemName,"Yellow Crystals");
		else if (id==kFireCrystal)				strcat(itemName,"Fire Crystals");
		else if (id==kBlackCrystal)			  strcat(itemName,"Black Crystals");

		else if (id==kPureGreenCrystal)				strcat(itemName,"Pure Green Crystals");
		else if (id==kPureRedCrystal)					strcat(itemName,"Pure Red Crystals");
		else if (id==kPureBlueCrystal)				strcat(itemName,"Pure Blue Crystals");
		else if (id==kPurePurpleCrystal)			strcat(itemName,"Pure Purple Crystals");
		else if (id==kPureOrangeCrystal)			strcat(itemName,"Pure Orange Crystals");
		else if (id==kPureYellowCrystal)			strcat(itemName,"Pure Yellow Crystals");
		else if (id==kPureFireCrystal)				strcat(itemName,"Pure Fire Crystals");
		else if (id==kPureBlackCrystal)			  strcat(itemName,"Pure Black Crystals");
		else if (id==kPureIceCrystal)			  strcat(itemName,"Pure Ice Crystals");
		
		else if (id==kInfusedGreen)					strcat(itemName,"Infused Green Crystals");
		else if (id==kInfusedRed)					strcat(itemName,"Infused Red Crystals");
		else if (id==kInfusedBlue)					strcat(itemName,"Infused Blue Crystals");
		else if (id==kInfusedYellow)					strcat(itemName,"Infused Yellow Crystals");
		else if (id==kInfusedOrange)					strcat(itemName,"Infused Orange Crystals");
		else if (id==kInfusedPurple)					strcat(itemName,"Infused Purple Crystals");
		else if (id==kInfusedBlack)					strcat(itemName,"Infused Black Crystals");
		else if (id==kInfusedPGreen)					strcat(itemName,"Infused Pure Green Crystals");
		else if (id==kInfusedPRed)					strcat(itemName,"Infused Pure Red Crystals");
		else if (id==kInfusedPBlue)					strcat(itemName,"Infused Pure Blue Crystals");
		else if (id==kInfusedPYellow)					strcat(itemName,"Infused Pure Yellow Crystals");
		else if (id==kInfusedPOrange)					strcat(itemName,"Infused Pure Orange Crystals");
		else if (id==kInfusedPPurple)					strcat(itemName,"Infused Pure Purple Crystals");
		else if (id==kInfusedPBlack)					strcat(itemName,"Infused Pure Black Crystals");
		else if (id==kInfusedFire)					strcat(itemName,"Infused Fire Crystals");
		else if (id==kInfusedIce)					strcat(itemName,"Infused Ice Crystals");
		else if (id==kInfusedCS)					strcat(itemName,"Infused Crystal Skulls");

		else if (id==kBrigandHead)			  strcat(itemName,"Brigand Heads");
		else if (id==kSorcererHead)			  strcat(itemName,"Sorcerer Heads");
		else if (id==kPirateHead)			    strcat(itemName,"Pirate Heads");

		else if (id==kRuby)								strcat(itemName,"Rubies");
		else if (id==kQueensRuby)					strcat(itemName,"Queen's Rubies");

		else if (id==kCrystalRing)				strcat(itemName,"Crystal Rings");
		else if (id==kPlatinumRing)				strcat(itemName,"Platinum Rings");
		else if (id==kGoldRing)						strcat(itemName,"Gold Rings");
		else if (id==kSilverRing)					strcat(itemName,"Silver Rings");

		else if (id==kDragonStoneRing)		strcat(itemName,"Dragon Stone Rings");
		else if (id==kElementalRing)			strcat(itemName,"Elemental Rings");
		else if (id==kPoisonRing)					strcat(itemName,"Rings of Immunity");
		else if (id==kNeptunesRing)				strcat(itemName,"Neptune's Rings");
		else if (id==kHadesRing)					strcat(itemName,"Hades Rings");
		else if (id==kManaRing)					  strcat(itemName,"Mana Rings");
		else if (id==kDruidRing)					strcat(itemName,"Druid Rings");
		else if (id==kChaosRing)					strcat(itemName,"Chaos Rings");
		else if (id==kRingOfLife)				strcat(itemName,"Rings of Life");

		else if (id==kSilverNecklace)			strcat(itemName,"Silver Necklaces");

		else if (id==kShaft)							strcat(itemName,"Shafts");
		else if (id==kFeather)						strcat(itemName,"Feathers");
		else if (id==kRollOfCloth)				strcat(itemName,"Rolls of Cloth");
		else if (id==kTurtleShell)				strcat(itemName,"Turtle Shells");
		else if (id==kBundleOfWool)				strcat(itemName,"Bundles of Wool");

		else if (id==kBrackenSeed)				strcat(itemName,"Bracken Seeds");//
		else if (id==kGinseng)						strcat(itemName,"Ginsengs");//
		else if (id==kLotusFlower)				strcat(itemName,"Lotus Flowers");//
		else if (id==kMandrakeRoot)				strcat(itemName,"Mandrake Roots");//
		else if (id==kAmber)						  strcat(itemName,"Ambers");
		else if (id==kGarlic)							strcat(itemName,"Garlics");//
		else if (id==kVolcanicAsh)				strcat(itemName,"Volcanic Ashes");//
		else if (id==kPoisonGland)				strcat(itemName,"Poison Glands");//
		else if (id==kCoal)						    strcat(itemName,"Pieces of Coal");//
		else if (id==kElectricEel)				strcat(itemName,"Electric Eels");//
		else if (id==kDragonTooth)				strcat(itemName,"Dragon Teeth");//
		else if (id==kDragonScale)				strcat(itemName,"Dragon Scales");
		else if (id==kGaperStalk)				  strcat(itemName,"Gaper Stalks");
		else if (id==kSackOfSpice)				strcat(itemName,"Sacks of Spice");

		else if (id==kLightningBoltSpell)	strcat(itemName,"Lightning Bolt");
		else if (id==kResurrectSpell)			strcat(itemName,"Resurrect");

		else if (id==kLesserHealSpell)			strcat(itemName,"Lesser Heal");
		else if (id==kGreaterHealSpell)			strcat(itemName,"Greater Heal");
		else if (id==kRemoveCurseSpell)			strcat(itemName,"Remove Curse");
		else if (id==kFullHealSpell)				strcat(itemName,"Full Heal");

		else if (id==kFireFieldSpell)				strcat(itemName,"Fire Field");
		else if (id==kReflectiveArmorSpell)	strcat(itemName,"Reflective Armor");
		else if (id==kClumsySpell)					strcat(itemName,"Clumsy");
		else if (id==kCreateFoodSpell)			strcat(itemName,"Create Food");
		else if (id==kTurnUndeadSpell)			strcat(itemName,"Turn Undead");
		else if (id==kSimpleMindSpell)			strcat(itemName,"Simple Mind");
		else if (id==kNightVisionSpell)			strcat(itemName,"Night Vision");
		else if (id==kWeakenSpell)					strcat(itemName,"Weaken");
		else if (id==kAgilitySpell)					strcat(itemName,"Agility");
		else if (id==kElevateMindSpell)			strcat(itemName,"Elevate Mind");
		else if (id==kFireballSpell)			strcat(itemName,"Fireball");
		else if (id==kFireblastSpell)			strcat(itemName,"FX Test 1");
		else if (id==kMagicMissileSpell)			strcat(itemName,"Magic Arrow");
		else if (id==kFireblast3Spell)			strcat(itemName,"FX Test 3");
		else if (id==kFireblast4Spell)			strcat(itemName,"Blaze");
		else if (id==kFireblast5Spell)			strcat(itemName,"FX Test 5");
		else if (id==kChaosFieldSpell)			strcat(itemName,"Chaos Field");
		else if (id==kLesserDetoxifySpell)	strcat(itemName,"Lesser Detoxify");
		else if (id==kGreaterDetoxifySpell)	strcat(itemName,"Greater Detoxify");
		else if (id==kFullDetoxifySpell)		strcat(itemName,"Full Detoxify");
		else if (id==kMagicTrapSpell)				strcat(itemName,"Magic Trap");
		else if (id==kMagicDisarmSpell)			strcat(itemName,"Magic Disarm");
		else if (id==kStrengthSpell)				strcat(itemName,"Strength");
		else if (id==kSummonPetSpell)				strcat(itemName,"Summon Pet");
		else if (id==kHealPetSpell)			strcat(itemName,"Heal Pet");
		else if (id==kPoisonSpell)					strcat(itemName,"Poison");
		else if (id==kTeleportSpell)				strcat(itemName,"Teleport");
		else if (id==kDrainManaSpell)				strcat(itemName,"Drain Mana");
		else if (id==kMagicReflectionSpell)	strcat(itemName,"Magic Reflection");
		else if (id==kDisorientationSpell)	strcat(itemName,"Disorient");
		else if (id==kParalyzeSpell)				strcat(itemName,"Paralyze");
		else if (id==kSummonCreatureSpell)	strcat(itemName,"Summon Creature");
		//Dave
		else if (id==kGreaterSummonCreatureSpell)	strcat(itemName,"Greater Summon Creature");
		else if (id==kExplosionSpell)				strcat(itemName,"Explosion");
		else if (id==kInvisibilitySpell)		strcat(itemName,"Invisibility");
		else if (id==kRevealSpell)					strcat(itemName,"Reveal");
		else if (id==kParalyzeFieldSpell)		strcat(itemName,"Paralyze Field");
		else if (id==kPoisonFieldSpell)			strcat(itemName,"Poison Field");
		else if (id==kMassHealSpell)			strcat(itemName,"Mass Heal");
		else if (id==kShapeshiftLesserSpell)	strcat(itemName, "Lesser Shapeshift");
		else if (id==kShapeshiftSpell)			strcat(itemName, "Shapeshift");
		else if (id==kShapeshiftGreaterSpell)	strcat(itemName, "Greater Shapeshift");
		else if (id==kShapeshiftAllSpell)		strcat(itemName, "Shapeshift: All");
		else if (id==kShapeshiftDragonSpell)	strcat(itemName, "Shapeshift: Dragon");
		else if (id==kDisorientFieldSpell)		strcat(itemName,"Disorient Field");
		//else if (id==kManaStealSpell)				strcat(itemName,"Mana Steal");
		else if (id==kProvocationSpell)			strcat(itemName,"Provocation");
		else if (id==kCalmSpell)					  strcat(itemName,"Calm");
		else if (id==kOpportunitySpell)			strcat(itemName,"Opportunity");
		
		else if (id==kAttackBackstab)			strcat(itemName,"Backstab");
		else if (id==kAttackBoomingBlow)		strcat(itemName,"Booming Blow");
		else if (id==kAttackDecimate)			strcat(itemName,"Decimate");
		else if (id==kAttackInstantStrike)		strcat(itemName,"Instant Strike");
		else if (id==kAttackVial)				strcat(itemName,"Explosive Dust");
		else if (id==kAttackSting)				strcat(itemName,"Sting");
		else if (id==kAttackDoubleStrike)		strcat(itemName,"Double Strike");
		else if (id==kAttackDoubleShot)			strcat(itemName,"Double Shot");
		else if (id==kAttackGrapplingShot)		strcat(itemName,"Grappling Shot");
		else if (id==kAttackEscapeMove)			strcat(itemName,"Escape Move");
		else if (id==kAttackLesserDefense)		strcat(itemName,"Lesser Defensive Stance");
		else if (id==kAttackGreaterDefense)		strcat(itemName,"Greater Defensive Stance");
		else if (id==kAttackRage)				strcat(itemName,"Rage");
		else if (id==kAttackKnockback)			strcat(itemName,"Knockback");
		else if (id==kAttackAimedShot)			strcat(itemName,"Aimed Shot");
		else if (id==kAttackTaunt)				strcat(itemName,"Challenging Strike");
		else if (id==kAttackRangerSwiftness)	strcat(itemName,"Ranger's Swiftness");
		else if (id==kAttackQuickDraw)			strcat(itemName,"Battle Ready");

		else if (id==kLightBlueDye)				strcat(itemName,"Bottles of Light Blue Dye");
		else if (id==kRedDye)							strcat(itemName,"Bottles of Red Dye");
		else if (id==kBlueDye)						strcat(itemName,"Bottles of Blue Dye");
		else if (id==kBrownDye)						strcat(itemName,"Bottles of Brown Dye");
		else if (id==kBlackDye)						strcat(itemName,"Bottles of Black Dye");
		else if (id==kGreenDye)						strcat(itemName,"Bottles of Green Dye");
		else if (id==kPurpleDye)					strcat(itemName,"Bottles of Purple Dye");
		else if (id==kYellowDye)					strcat(itemName,"Bottles of Yellow Dye");
		//Dave
		else if (id==kGrayDye)						strcat(itemName,"Bottles of Gray Dye");
		else if (id==kOrangeDye)					strcat(itemName,"Bottles of Orange Dye");
		else if (id==kGoldenBox)					strcat(itemName, "Golden Boxes");
		else if (id==kBody)							strcat(itemName, "Bodies");

		else if (id==kLeatherHide)				strcat(itemName,"Leather Hides");
		else if (id==kFurSkin)						strcat(itemName,"Furs");
		else if (id==kCurio1)						strcat(itemName, "Mirith's Moon Curio Thirds");
		else if (id==kCurio2)						strcat(itemName, "Andris's Moon Curio Thirds");
		else if (id==kCurio3)						strcat(itemName, "Marali's Moon Curio Thirds");
		else if (id==kCurioBig)						strcat(itemName, "Moon Curios");
		else if (id==kCurioSmall)					strcat(itemName, "Moon Curio Fragments");
		else if (id==kSmallBag)					strcat(itemName, "Small Bags");
		else if (id==kLockbox)					strcat(itemName, "Lockboxes");
		else if (id==kPapers)					strcat(itemName, "Papers");
		else if (id==kMagicStone)					strcat(itemName, "Magic Stones");
		else if (id==kSmallKey)					strcat(itemName, "Small Keys");
		else if (id==kMessage)					strcat(itemName, "Messages");
		else if (id==kCrate)					strcat(itemName, "Crates");
		else if (id==kVialOfBlood)					strcat(itemName, "Vials of Blood");
		else if (id==kDocuments)					strcat(itemName, "Documents");
		else if (id==kLetter)					strcat(itemName, "Letters");
		else if (id==kRustyKey)					strcat(itemName, "Rusty Keys");
		else if (id==kIronKey)					strcat(itemName, "Iron Keys");
		else if (id==kBronzeKey)					strcat(itemName, "Bronze Keys");
		else if (id==kGoldenBolt)					strcat(itemName, "Golden Bolts");
		else if (id==kSilverBolt)					strcat(itemName, "Silver Bolts");
		else if (id==kDragonEggGreen)					strcat(itemName, "Green Dragon Eggs");
		else if (id==kDragonEggGold)					strcat(itemName, "Golden Dragon Eggs");
		else if (id==kDragonEggBlack)					strcat(itemName, "Black Dragon Eggs");
		else if (id==kDragonEggRed)					strcat(itemName, "Red Dragon Eggs");
		else if (id==kDragonEggBlue)					strcat(itemName, "Ice Dragon Eggs");
		else if (id==kDragonEggPurple)					strcat(itemName, "Abyss Dragon Eggs");
		else if (id==kKeyBow)						strcat(itemName, "Key Bows");
		else if (id==kKeyCollar)					strcat(itemName, "Key Collars");
		else if (id==kKeyShank)						strcat(itemName, "Key Shanks");
		else if (id==kKeyShoulder)					strcat(itemName, "Key Shoulders");
		else if (id==kKeyStem)						strcat(itemName, "Key Stems");
		else if (id==kKeyBit)						strcat(itemName, "Key Bits");
		else if (id==kSnakeSkin)					strcat(itemName, "Snake Skins");
		else if (id==kSunstoneRing)					strcat(itemName, "Sunstone Rings");
		else if (id==kFlaskOfPerfume)					strcat(itemName, "Flasks of Perfume");
		else if (id==kLittleStatue)					strcat(itemName, "Little Statues");
		else if (id==kLockOfHair)					strcat(itemName, "Locks of Hair");
		else if (id==kJar)					strcat(itemName, "Jars");
		else if (id==kParchment)					strcat(itemName, "Parchments");
		else if (id==kBoneKnife)					strcat(itemName, "Bone Knives");
		else if (id==kPearlNecklace)					strcat(itemName, "Pearl Necklaces");
		else if (id==kStatueOfElara)					strcat(itemName, "Statues of Elara");
		else if (id==kMysticSigil)					strcat(itemName, "Mystic Sigils");
		else if (id==kArcaneGrimoire)					strcat(itemName, "Arcane Grimoires");
		else if (id==kSilverChalice)					strcat(itemName, "Silver Chalices");
		else if (id==kInfernalTome)					strcat(itemName, "Infernal Tomes");
		else if (id==kEmeraldPrism)					strcat(itemName, "Emerald Prisms");
		else if (id==kSapphirePrism)					strcat(itemName, "Sapphire Prisms");
		else if (id==kAmethystPrism)					strcat(itemName, "Amethyst Prisms");
		else if (id==kRubyPrism)					strcat(itemName, "Ruby Prisms");
		else if (id==kTopazPrism)					strcat(itemName, "Topaz Prisms");
		else if (id==kDiamondPrism)					strcat(itemName, "Diamond Prisms");
		else if (id==kGoldCandelabrum)					strcat(itemName, "Gold Candelabra");
		else if (id==kBookofMoons)					strcat(itemName, "Books of Moons");
		else if (id==kBlinkHoundFur)					strcat(itemName, "Blink Hound Furs");
		else if (id==kZionidicCrystal)					strcat(itemName, "Zionidic Crystals");
		else if (id==kVermaStone)					strcat(itemName, "Verma Stones");
		else if (id==kSpiralCharm)					strcat(itemName, "Spiral Charms");
		else if (id==kGoldThread)					strcat(itemName, "Gold Threads");
		else if (id==kVialOfCentaurBlood)			strcat(itemName, "Vials of Centaur Blood");
		else if (id==kRoyalSceptre)					strcat(itemName, "Royal Sceptres");
		else if (id==kMirithToken)					strcat(itemName, "Mirith Tokens");
		else if (id==kMaraliToken)					strcat(itemName, "Marali Tokens");
		else if (id==kAndrisToken)					strcat(itemName, "Andris Tokens");
		else if (id==kMayorSeal)					strcat(itemName, "Mayor's Seals");
		else if (id==kSenateSeal)					strcat(itemName, "Senate's Seals");
		else if (id==kCommanderSeal)				strcat(itemName, "Commander's Seals");
		else if (id==kStoreToken)					strcat(itemName, "Store Tokens");
		else if (id==kIvoryTusk)					strcat(itemName, "Ivory Tusks");
		else if (id==kShevPortrait)					strcat(itemName, "Portraits of Shevarash");
		else if (id==kSickHorse)					strcat(itemName, "Charms of Speed");
		else if (id==kHorse)						strcat(itemName, "Talismans of Speed");
		else if (id==kFastHorse)					strcat(itemName, "Fast Horses");
		else if (id==kVeryFastHorse)				strcat(itemName, "Very Fast Horses");
		else if (id==kMagicMount1)					strcat(itemName, "Magic Mounts 1");
		else if (id==kMagicMount2)					strcat(itemName, "Magic Mounts 2");
		else if (id==kMagicMount3)					strcat(itemName, "Magic Mounts 3");
		else if (id==kMagicMount4)					strcat(itemName, "Magic Mounts 4");
		else if (id==kGMHorse)						strcat(itemName, "GM Horses");
		else if (id==kGaperEye)						strcat(itemName, "Gaper Eyes");
		else if (id==kElementalDust)				strcat(itemName, "Elemental Dusts");									
		else if (id==kAcidicGland)					strcat(itemName, "Acidic Glands");							
		else if (id==kBrackenRoots)					strcat(itemName, "Bracken Roots");
		else if (id==kBoneMageStaff)				strcat(itemName, "Bone Mage Staves");
		else if (id==kHarpieBeak)					strcat(itemName, "Harpie Beaks");
		else if (id==kLizardScale)					strcat(itemName, "Lizard Scales");	
		else if (id==kBatWing)						strcat(itemName, "Bat Wings");
		else if (id==kSpiderLeg)					strcat(itemName, "Spider Legs");	
		else if (id==kSnailSlime)					strcat(itemName, "Slime");									
		else if (id==kZombieBrain)					strcat(itemName, "Zombie Brains");	
		else if (id==kRustGland)					strcat(itemName, "Rust Glands");
		else if (id==kGiantHeart)					strcat(itemName, "Giant Hearts");	
		else if (id==kSnakeEgg)						strcat(itemName, "Snake Eggs");
		else if (id==kAntMandibles)					strcat(itemName, "Ant Mandibles");								
		else if (id==kCubeGoo)						strcat(itemName, "Goos");
		else if (id==kTuskerTail)					strcat(itemName, "Tusker Tails");
		else if (id==kAncientRune)					strcat(itemName, "Ancient Runes");
		else if (id==kNeviaFlower)					strcat(itemName, "Nevia Flowers");
		else if (id==kSmallMoonFragment)			strcat(itemName, "Small Moon Fragments");
		else if (id==kMerefolkHair)					strcat(itemName, "Merfolk Hair");
		else if (id==kRustyNecklace)				strcat(itemName, "Tarnished Necklaces");
		else if (id==kPureCrystalRing)				strcat(itemName, "Pure Crystal Rings");
		else if (id==kEnchantedCloth)				strcat(itemName, "Enchanted Cloth");
		else if (id==kDiamondDust)					strcat(itemName, "Diamond Dust");
		else if (id==kDiscretionRing)				strcat(itemName, "Discretion Rings");
		else if (id==kCharmOfUndeath)				strcat(itemName, "Charms of Undead Cleansing");
		else if (id==kVialOfAcid)					strcat(itemName, "Explosive Dusts");
		else if (id==kRingOfStrength)				strcat(itemName, "Rings of Strength");
		else if (id==kRingOfStealth)				strcat(itemName, "Rings of Stealth");
		else if (id==kCharmOfSwiftness)				strcat(itemName, "Charms of Swiftness");
		else if (id==kCharmOfLoyalty)				strcat(itemName, "Charms of Loyalty");
		else if (id==kCharmOfPurity)				strcat(itemName, "Charms of Purity");
		else if (id==kRingOfBruteStrength)			strcat(itemName, "Rings of Brute Strength");
		else if (id==kLesserManaRing)				strcat(itemName, "Lesser Mana Rings");
		else if (id==kRingOfShadows)				strcat(itemName, "Rings of Shadow");
		else if (id==kEssenceOfPower)				strcat(itemName, "Essences of Power");
		else if (id==kEssenceOfDurability)			strcat(itemName, "Essences of Durability");
		else if (id==kEssenceOfSpeed)				strcat(itemName, "Essences of Speed");
		else if (id==kBloodAmulet) 					strcat(itemName, "Blood Amulets");
		else if (id==kBloodStainedMoonstone) 		strcat(itemName, "Blood Stained Moonstones");
		else if (id==kBloodTome) 					strcat(itemName, "Blood Tomes");
		else if (id==kCrystalNecklace) 				strcat(itemName, "Crystal Necklaces");
		else if (id==kMoonstoneRing) 				strcat(itemName, "Moonstone Rings");
		else if (id==kJournal) 						strcat(itemName, "Journals");
		else if (id==kTornNote) 					strcat(itemName, "Torn Notes");
		else if (id==kCrystalChalice) 				strcat(itemName, "Crystal Chalices");
		else if (id==kBloodyJournal) 				strcat(itemName, "Bloody Journals");
		else if (id==kBloodyNote)			 		strcat(itemName, "Bloody Notes");
		else if (id==kSkeletonKey) 					strcat(itemName, "Skeleton Keys");
		else if (id==kNorthStone) 					strcat(itemName, "North Stones");
		else if (id==kSouthStone) 					strcat(itemName, "South Stones");	
		else if (id==kEastStone) 					strcat(itemName, "East Stones");
		else if (id==kWestStone) 					strcat(itemName, "West Stones");
		else if (id==kThread) 						strcat(itemName, "Threads");
		else if (id==kBead) 						strcat(itemName, "Beads");
		else if (id==kBrokenNecklace) 				strcat(itemName, "Broken Necklaces"); 					
		else if (id==kRuinedBook) 					strcat(itemName, "Ruined Books");	
		else if (id==kBloodStainedCloth) 			strcat(itemName, "Blood-Stained Cloths");	
		else if (id==kMuddyBoots) 					strcat(itemName, "Muddy Boots");
		else if (id==kOrnateStaff) 					strcat(itemName, "Ornate Staves");		
		else if (id==kMoldyBook) 					strcat(itemName, "Moldy Books"); 							
		else if (id==kLogbook) 						strcat(itemName, "Logbooks"); 							
		else if (id==kRegister) 					strcat(itemName, "Registers");				
		else if (id==kShreddedTome) 				strcat(itemName, "Shredded Tomes");			
		else if (id==kTornPage) 					strcat(itemName, "Torn Pages");		
		else if (id==kGlowingBottle) 				strcat(itemName, "Glowing Bottles");		
		else if (id==kCrystalDragonStatue) 			strcat(itemName, "Crystal Dragon Statues");
		else if (id==kSilverFigurine) 				strcat(itemName, "Silver Figurines"); 					
		else if (id==kStarDiamond) 					strcat(itemName, "Star Diamonds");	
		else if (id==kShadowedOrb) 					strcat(itemName, "Shadowed Orbs");		
		else if (id==kSilverTippedQuill) 			strcat(itemName, "Silver Tipped Quills");	
		else if (id==kCrystalGoddessCharm) 			strcat(itemName, "Crystal Goddess Charms");	
		else if (id==kJeweledHairPin) 				strcat(itemName, "Jeweled Hair Pins");
		else if (id==kMercenaryToken) 				strcat(itemName, "Mercenary Tokens");
		else if (id==kCrystalToken) 				strcat(itemName, "Crystal Tokens");
		else if (id==kDecorativeVase) 				strcat(itemName, "Decorative Vases");							
		else if (id==kWinningTicket) 				strcat(itemName, "Winning Tickets");
		else if (id==kBettingSlip) 					strcat(itemName, "Betting Slips");
		else if (id==kLeaderPin) 					strcat(itemName, "Leader Pins");
		else if (id==kOldSword) 					strcat(itemName, "Old Swords");
		else if (id==kWhiteWine) 					strcat(itemName, "White Wine");
		else if (id==kRedWine) 						strcat(itemName, "Red Wine");
		else if (id==kCopperMug) 					strcat(itemName, "Copper Mugs");
		else if (id==kCopperLantern) 				strcat(itemName, "Copper Lanterns");
		else if (id==kMirithPaleAle) 				strcat(itemName, "Mirith Pale Ales");
		else if (id==kMaraliWhiskey) 				strcat(itemName, "Marali Whiskeys");
		else if (id==kAndrisChampagne) 				strcat(itemName, "Andris Champagnes");
		else if (id==kBuckeyeBeer) 					strcat(itemName, "Buckeye Beers");
		else if (id==kLerilinCremeale) 				strcat(itemName, "Lerilin Creme Ales");
		else if (id==kRedNatesRotgutRum) 			strcat(itemName, "Red Nate's Rotgut Rums");
		else if (id==kGoldenScale) 					strcat(itemName, "Golden Scales");
		else if (id==kWoodenTrinket) 				strcat(itemName, "Wooden Trinkets");
		else if (id==kPetrifiedGaperEye) 			strcat(itemName, "Petrified Gaper Eyes");
		else if (id==kSilkPouch) 					strcat(itemName, "Silk Pouches");
		else if (id==kOrnateWalkingStick) 			strcat(itemName, "Ornate Walking Sticks");
		else if (id==kGoddessCharm)					strcat(itemName, "Goddess Charms");
		else if (id==kCrystalSpiderWeb) 			strcat(itemName, "Crystal Spider Webs");
		else if (id==kWoodenFlute) 					strcat(itemName, "Wooden Flutes");
		else if (id==kLute) 						strcat(itemName, "Lutes");
		else if (id==kSilverFlute) 					strcat(itemName, "Silver Flutes");
		else if (id==kGoldLadle) 					strcat(itemName, "Gold Ladles");
		else if (id==kPipe) 						strcat(itemName, "Pipes");
		else if (id==kDarkRuby) 					strcat(itemName, "Dark Rubies");
		else if (id==kMortarandPestle) 				strcat(itemName, "Pestles and Mortars");
		else if (id==kBookofFairyTales) 			strcat(itemName, "Books of Fairy Tales");
		else if (id==kJeweledDagger) 				strcat(itemName, "Jeweled Daggers");
		else if (id==kGarlicBread) 					strcat(itemName, "Garlic Breads");
		else if (id==kSheepsMilk) 					strcat(itemName, "Sheeps Milk");
		else if (id==kSteakFlambe) 					strcat(itemName, "Steaks FlambŽ");
		else if (id==kSushi) 						strcat(itemName, "Chirashizushis");
		else if (id==kRedOmelette) 					strcat(itemName, "Harpie Egg Omelettes");
		else if (id==kPurpleOmelette) 				strcat(itemName, "Sea Serpent Omelettes");
		else if (id==kWaterBottle) 					strcat(itemName, "Bottles of Water");
		else if (id==kRumCake) 						strcat(itemName, "Rum Cakes");
		else if (id==kCharStatue) 					strcat(itemName, "Statues of Charnath");
		else if (id==kBluePlateHelmet) 				strcat(itemName, "Blue Closed Plate Helmet");
		else if (id==kBluePlateBreast) 				strcat(itemName, "Blue Plate Breast");
		else if (id==kBluePlateArms) 				strcat(itemName, "Blue Plate Arms");
		else if (id==kBluePlateLegs) 				strcat(itemName, "Blue Plate Legs");
		else if (id==kBluePlateGauntlets) 			strcat(itemName, "Blue Plate Gauntlets");
		else if (id==kMushroom) 					strcat(itemName, "Mushrooms");
		else if (id==kCandiedApple) 				strcat(itemName, "Candied Apples");
		else if (id==kPizza) 						strcat(itemName, "Mushroom Pizzas");
		else if (id==kMead) 						strcat(itemName, "Mugs of Mead");
		else if (id==kBlueMoonGin) 					strcat(itemName, "Blue Moon Gins");
		else if (id==kHayateSake) 					strcat(itemName, "Hayate's Sakes");
		else if (id==kPumpkinPie) 					strcat(itemName, "Pumpkin Pies");
		else if (id==kPortGastMoonshine) 			strcat(itemName, "Port Gast's Moonshines");
		else if (id==kFoehanVodka) 					strcat(itemName, "Foehan Vodkas");
		else if (id==kWizardToken) 					strcat(itemName, "Wizard Tokens");
		else if (id==kDruidToken) 					strcat(itemName, "Druid Tokens");
		else if (id==kClericToken) 					strcat(itemName, "Cleric Tokens");
		else if (id==kFighterToken) 				strcat(itemName, "Fighter Tokens");
		else if (id==kRangerToken) 					strcat(itemName, "Ranger Tokens");
		else if (id==kRogueToken) 					strcat(itemName, "Rogue Tokens");
		else if (id==kBigJackKatana) 				strcat(itemName, "Big Jack's Katana");
		else if (id==kBlackFerrite) 				strcat(itemName, "Black Ferrite");
		else if (id==kBlackIngot) 					strcat(itemName, "Black Ingots");
		else if (id==kZombieBrainStew) 				strcat(itemName, "Zombie Brain Stews");
		else if (id==kMandrakeCookie) 				strcat(itemName, "Mandrake Cookies");
		else if (id==kJinkDoll) 					strcat(itemName, "Jinker Dolls");
		else if (id==kVoodooDoll) 					strcat(itemName, "Voodoo Dolls");
		else if (id==kNPCPrivateerHead) 			strcat(itemName, "Privateer Heads");
		else if (id==kTuna) 						strcat(itemName, "Tuna");
		else if (id==kSeaBass) 						strcat(itemName, "Sea Bass");
		else if (id==kCopperSpoon) 					strcat(itemName, "Copper Spoons");
		else if (id==kSilverButton) 				strcat(itemName, "Silver Buttons");
		else if (id==kTinPlate) 					strcat(itemName, "Tin Plates");
		else if (id==kSakeSeaBass) 					strcat(itemName, "Sake Sea Bass");
		else if (id==kCrystalBrooch) 				strcat(itemName, "Crystal Brooches");
		else if (id==kTornCloth) 					strcat(itemName, "Torn Cloths");
		else if (id==kBookOfBlood) 					strcat(itemName, "Books of Blood");
		else if (id==kRustySextant) 				strcat(itemName, "Rusty Sextants");
		else if (id==kAmuletOfStars) 				strcat(itemName, "Amulets of Stars");
		else if (id==kAmuletOfSky) 					strcat(itemName, "Amulets of Sky");
		else if (id==kAmuletOfTheMoon) 				strcat(itemName, "Amulets of the Moon");
		else if (id==kCeruleanDiamond) 				strcat(itemName, "Cerulean Diamonds");
		else if (id==kJeweledRing) 					strcat(itemName, "Jeweled Rings");
		else if (id==kBrokenRing) 					strcat(itemName, "Broken Rings");
		else if (id==kDriftWoodFigurine) 			strcat(itemName, "Drift Wood Figurines");
		else if (id==kSilverShell) 					strcat(itemName, "Silver Sea Shells");
		else if (id==kTinySeaShell) 				strcat(itemName, "Tiny Sea Shells");
		else if (id==kReed) 						strcat(itemName, "Reeds");
		else if (id==kKelp) 						strcat(itemName, "Kelp");
		else if (id==kCharmOfTheSea) 				strcat(itemName, "Charms of the Sea");
		else if (id==kOrnateShellHorn) 				strcat(itemName, "Ornate Shell Horns");
		else if (id==kBraidedKelpCharm) 			strcat(itemName, "Braided Kelp Charms");
		else if (id==kBookOfSky) 					strcat(itemName, "Books of Sky");
		else if (id==kStuffedGiantHeart) 			strcat(itemName, "Stuffed Giant Hearts");
		else if (id==kEggNog) 						strcat(itemName, "Egg Nogs");
		else if (id==kAndrisBananaCake) 			strcat(itemName, "Andris Banana Cakes");
		else if (id==kMorgansGingerbread) 			strcat(itemName, "Morgans' Gingerbread");
		else if (id==kWizardReward) 				strcat(itemName, "Books of The Wizard");
		else if (id==kClericReward) 				strcat(itemName, "Books of The Cleric");
		else if (id==kDruidReward) 					strcat(itemName, "Books of The Druid");
		else if (id==kFighterReward) 				strcat(itemName, "Books of The Fighter");
		else if (id==kRangerReward) 				strcat(itemName, "Books of The Ranger");
		else if (id==kRogueReward) 					strcat(itemName, "Books of The Rogue");
		else if (id==kPixieDust) 					strcat(itemName, "Pixie Dusts");
		else if (id==kCopperCoin) 					strcat(itemName, "Copper Coins");
		else if (id==kKingFigurine) 				strcat(itemName, "King Figurines");
		else if (id==kQueenFigurine) 				strcat(itemName, "Queen Figurines");
		else if (id==kBishopFigurine) 				strcat(itemName, "Bishop Figurines");
		else if (id==kKnightFigurine) 				strcat(itemName, "Knight Figurines");
		else if (id==kRookFigurine) 				strcat(itemName, "Rook Figurines");
		else if (id==kPawnFigurine) 				strcat(itemName, "Pawn Figurines");
		else if (id==kSilverCoin) 					strcat(itemName, "Silver Coins");
		else if (id==kRedToken) 					strcat(itemName, "Red Tokens");
		else if (id==kBlackToken) 					strcat(itemName, "Black Tokens");
		else if (id==kSilverToken) 					strcat(itemName, "Silver Tokens");
		else if (id==kGrainOfRedSand) 				strcat(itemName, "Grains of Red Sand");
		else if (id==kCandle) 						strcat(itemName, "Candles");
		else if (id==kBirthdayCake) 				strcat(itemName, "Birthday Cakes");
		else if (id==kGohoranChocolate) 			strcat(itemName, "Gohoran Chocolates");
		else if (id==kFirstCrown) 					strcat(itemName, "Champion's Crowns");
		else if (id==kSecondCrown) 					strcat(itemName, "Second Place Crowns");
		else if (id==kThirdCrown) 					strcat(itemName, "Third Place Crowns");
		else if (id==kSpiderEgg)					strcat(itemName, "Spider Egg Sacs");
		else if (id==kBearSkin)						strcat(itemName, "Bear Skins");
		else if (id==kSharkFin)						strcat(itemName, "Shark Fins");
		else if (id==kSharkFinSoup)					strcat(itemName, "Shark Fin Soups");
		else if (id==kTrollHouseCookie)				strcat(itemName, "Troll House Cookies");
		else if (id==kGoldMedal)					strcat(itemName, "Gold Medals");
		else if (id==kSilverMedal)					strcat(itemName, "Silver Medals");
		else if (id==kBronzeMedal)					strcat(itemName, "Bronze Medals");
		else if (id==kRuneSword)					strcat(itemName, "Rune Swords");
		else if (id==kNeedle)						strcat(itemName, "Needles");
		else if (id==kSLToken)						strcat(itemName, "Silver Leaf Tokens");	
		else if (id==kJadeNecklace)					strcat(itemName, "Jade Amulets");
		else if (id==kJadePin)						strcat(itemName, "Jade Pins");
		else if (id==kJadeStatue)					strcat(itemName, "Jade Statues");
		else if (id==kJadeBox)						strcat(itemName, "Jade Boxes");
		else if (id==kIridescentToken)				strcat(itemName, "Iridescent Tokens");
		else if (id==kBrigandInsignia)				strcat(itemName, "Brigand Insignias");
		else if (id==kBlueMetal)					strcat(itemName, "Blue Metals");
		else if (id==kPortraitLady)					strcat(itemName, "Portraits of a Lady");
		else if (id==kPortraitTirana)				strcat(itemName, "Portraits of Tirana");
		else if (id==kRingOfInsight)				strcat(itemName, "Rings of Insight");
		else if (id==kElixirHeart)					strcat(itemName, "Elixirs of Love");  //Elixirs
		else if (id==kElixirDuh)					strcat(itemName, "Elixirs of Forgetfulness");  //Elixirs
		else if (id==kElixirZimm)					strcat(itemName, "Elixirs of Zimm");  //Elixirs
		else if (id==kElixirKapow)					strcat(itemName, "Elixirs of Spirit");  //Elixirs
		else if (id==kElixirTingle)					strcat(itemName, "Elixirs");  //Elixirs
		else if (id==kTomeoftheLost)				strcat(itemName, "Tomes of the Lost");  //Tomes of the Lost
		
		else
		{
			tb_IntegerToString(id,	itemName);
			strcat(itemName," Unidentified Item");//Dave  strcat(itemName," <Unknown(s)>");
		}

	}
	else
	{
		if ((id>=4000) && (id<5000))      strcat(itemName,"Scroll");
		else if (id==kKindling)					  strcat(itemName,"Bundle of Kindling");
		else if (id==kKatana)							strcat(itemName,"Katana");
		else if (id==kKatanaDurability)		strcat(itemName,"Katana of Durability");
		else if (id==kKatanaPower)				strcat(itemName,"Katana of Power");
		else if (id==kKatanaSpeed)				strcat(itemName,"Katana of Speed");
		else if (id==kKatanaSilver)				strcat(itemName,"Silver Katana");
		else if (id==kKatanaPoison)				strcat(itemName,"Poison Katana");

		else if (id==kNeptunesTrident)  		strcat(itemName,"Neptune's Trident");
		else if (id==kFireSword)  					strcat(itemName,"Flaming Sword");
		else if (id==kLongSwordDurability)  strcat(itemName,"Long Sword of Durability");
		else if (id==kLongSwordPower)				strcat(itemName,"Long Sword of Power");
		else if (id==kLongSwordSpeed)				strcat(itemName,"Long Sword of Speed");
		else if (id==kLongSwordSilver)			strcat(itemName,"Silver Long Sword");
		else if (id==kLongSwordPoison)			strcat(itemName,"Poison Long Sword");

		else if (id==kBroadSwordDurability)  strcat(itemName,"Broad Sword of Durability");
		else if (id==kBroadSwordPower)				strcat(itemName,"Broad Sword of Power");
		else if (id==kBroadSwordSpeed)				strcat(itemName,"Broad Sword of Speed");
		else if (id==kBroadSwordSilver)			strcat(itemName,"Silver Broad Sword");
		else if (id==kBroadSwordPoison)			strcat(itemName,"Poison Broad Sword");

		else if (id==kScimitarDurability)  strcat(itemName,"Scimitar of Durability");
		else if (id==kScimitarPower)				strcat(itemName,"Scimitar of Power");
		else if (id==kScimitarSpeed)				strcat(itemName,"Scimitar of Speed");
		else if (id==kScimitarSilver)			strcat(itemName,"Silver Scimitar");
		else if (id==kScimitarPoison)			strcat(itemName,"Poison Scimitar");

		else if (id==kHolyDagger)  	strcat(itemName,"Holy Dagger");
		else if (id==kBloodDagger)  strcat(itemName,"Blood Dagger");

		else if (id==kKnifeDurability)  strcat(itemName,"Dagger of Durability");
		else if (id==kKnifePower)				strcat(itemName,"Dagger of Power");
		else if (id==kKnifeSpeed)				strcat(itemName,"Dagger of Speed");
		else if (id==kKnifeSilver)			strcat(itemName,"Silver Dagger");
		else if (id==kKnifePoison)			strcat(itemName,"Poison Dagger");

		else if (id==kStaveDurability)  strcat(itemName,"Stave of Durability");
		else if (id==kStavePower)				strcat(itemName,"Stave of Power");
		else if (id==kStaveSpeed)				strcat(itemName,"Stave of Speed");

		else if (id==kSickleDurability)  strcat(itemName,"Sickle of Durability");
		else if (id==kSicklePower)				strcat(itemName,"Sickle of Power");
		else if (id==kSickleSpeed)				strcat(itemName,"Sickle of Speed");
		else if (id==kSickleSilver)			strcat(itemName,"Silver Sickle");
		else if (id==kSicklePoison)			strcat(itemName,"Poison Sickle");

		else if (id==kClubDurability)  strcat(itemName,"Club of Durability");
		else if (id==kClubPower)				strcat(itemName,"Club of Power");
		else if (id==kClubSpeed)				strcat(itemName,"Club of Speed");

		else if (id==kFlailDurability)  strcat(itemName,"Flail of Durability");
		else if (id==kFlailPower)				strcat(itemName,"Flail of Power");
		else if (id==kFlailSpeed)				strcat(itemName,"Flail of Speed");

		else if (id==kShortBowDurability)  strcat(itemName,"Short Bow of Durability");
		else if (id==kShortBowPower)				strcat(itemName,"Short Bow of Power");
		else if (id==kShortBowSpeed)				strcat(itemName,"Short Bow of Speed");

		else if (id==kBattleAxeDurability)  strcat(itemName,"Battle Axe of Durability");
		else if (id==kBattleAxePower)				strcat(itemName,"Battle Axe of Power");
		else if (id==kBattleAxeSpeed)				strcat(itemName,"Battle Axe of Speed");
		else if (id==kBattleAxeSilver)			strcat(itemName,"Silver Battle Axe");
		else if (id==kBattleAxePoison)			strcat(itemName,"Poison Battle Axe");

		else if (id==kHandAxeDurability)  strcat(itemName,"Hand Axe of Durability");
		else if (id==kHandAxePower)				strcat(itemName,"Hand Axe of Power");
		else if (id==kHandAxeSpeed)				strcat(itemName,"Hand Axe of Speed");
		else if (id==kHandAxeSilver)			strcat(itemName,"Silver Hand Axe");
		else if (id==kHandAxePoison)			strcat(itemName,"Poison Hand Axe");

		else if (id==kGlaiveDurability)  strcat(itemName,"Glaive of Durability");
		else if (id==kGlaivePower)				strcat(itemName,"Glaive of Power");
		else if (id==kGlaiveSpeed)				strcat(itemName,"Glaive of Speed");
		else if (id==kGlaiveSilver)			strcat(itemName,"Silver Glaive");
		else if (id==kGlaivePoison)			strcat(itemName,"Poison Glaive");

		else if (id==kSpearDurability)  strcat(itemName,"Spear of Durability");
		else if (id==kSpearPower)				strcat(itemName,"Spear of Power");
		else if (id==kSpearSpeed)				strcat(itemName,"Spear of Speed");
		else if (id==kSpearSilver)			strcat(itemName,"Silver Spear");
		else if (id==kSpearPoison)			strcat(itemName,"Poison Spear");

		else if (id==kLongBowDurability)  strcat(itemName,"Long Bow of Durability");
		else if (id==kLongBowPower)				strcat(itemName,"Long Bow of Power");
		else if (id==kLongBowSpeed)				strcat(itemName,"Long Bow of Speed");

		else if (id==kWarHammerDurability)  strcat(itemName,"War Hammer of Durability");
		else if (id==kWarHammerPower)				strcat(itemName,"War Hammer of Power");
		else if (id==kWarHammerSpeed)				strcat(itemName,"War Hammer of Speed");

		else if (id==kMaceDurability)     strcat(itemName,"Mace of Durability");
		else if (id==kMacePower)				  strcat(itemName,"Mace of Power");
		else if (id==kMaceSpeed)				  strcat(itemName,"Mace of Speed");

		else if (id==kLongSword)					strcat(itemName,"Long Sword");
		else if (id==kBroadSword)					strcat(itemName,"Broad Sword");
		else if (id==kScimitar)						strcat(itemName,"Scimitar");
		else if (id==kKnife)							strcat(itemName,"Dagger");
		else if (id==kSextant)				    strcat(itemName,"Sextant");
		else if (id==kShears)				      strcat(itemName,"Shears");
		else if (id==kRatTail)				    strcat(itemName,"Rat Tail");
		else if (id==kStave)							strcat(itemName,"Stave");
		else if (id==kGMStaff)							strcat(itemName,"Poking Stick");
		else if (id==kSickle)							strcat(itemName,"Sickle");
		else if (id==kClub)								strcat(itemName,"Club");
		else if (id==kFlail)							strcat(itemName,"Flail");
		else if (id==kShortBow)					  strcat(itemName,"Short Bow");
		else if (id==kLongBow)					  strcat(itemName,"Long Bow");
		else if (id==kBattleAxe)					strcat(itemName,"Battle Axe");
		else if (id==kFrostAxe)					  strcat(itemName,"Frost Axe");
		else if (id==kHandAxe)						strcat(itemName,"Hand Axe");
		else if (id==kCentaurMiningAxe)		strcat(itemName,"Centaur Mining Axe");
		else if (id==kDwarvenHandAxe)			strcat(itemName,"Dwarven Hand Axe");
		else if (id==kAmberAxe)			      strcat(itemName,"Amber Axe");
		else if (id==kGlaive)							strcat(itemName,"Glaive");
		else if (id==kSpear)							strcat(itemName,"Spear");

		else if (id==kChainLegs)				          strcat(itemName,"Chain Mail Legs");
		else if (id==kChainArms)				          strcat(itemName,"Chain Mail Arms");
		else if (id==kPlateArms)				          strcat(itemName,"Plate Arms");
		else if (id==kBlackPlateArms)				      strcat(itemName,"Black Plate Arms");
		else if (id==kChainGauntlets)				      strcat(itemName,"Chain Mail Gloves");
		else if (id==kBlackPlateGauntlets)				strcat(itemName,"Black Plate Gauntlets");
		else if (id==kBlackOpenPlateHelmet)				strcat(itemName,"Black Open Plate Helmet");
		else if (id==kBlackSlottedPlateHelmet)		strcat(itemName,"Black Slotted Plate Helmet");
		else if (id==kBlackClosedPlateHelmet)			strcat(itemName,"Black Closed Plate Helmet");
		else if (id==kBlackVikingHelmet)				  strcat(itemName,"Black Viking Helmet");

		else if (id==kBreastPlate)				strcat(itemName,"Plate Breast");
		else if (id==kLegsPlate)					strcat(itemName,"Plate Legs");
		else if (id==kBlackBreastPlate)		strcat(itemName,"Black Plate Breast");
		else if (id==kBlackLegsPlate)			strcat(itemName,"Black Plate Legs");
		else if (id==kBreastChain)				strcat(itemName,"Chain Breast");

		else if (id==kMedalShield)				strcat(itemName,"Metal Shield");
		else if (id==kWoodenShield)				strcat(itemName,"Wooden Shield");

		else if (id==kSlottedPlateHelmet)	strcat(itemName,"Slotted Plate Helmet");
		else if (id==kClosedPlateHelmet)	strcat(itemName,"Closed Plate Helmet");
		else if (id==kVikingHelmet)		    strcat(itemName,"Viking Helmet");
		else if (id==kChainMailHelmet)		strcat(itemName,"Chain Mail Coif");

		else if (id==kOpenPlateHelmet)		strcat(itemName,"Open Plate Helmet");
		else if (id==kPlateGauntlets)			strcat(itemName,"Plate Gauntlets");
		
		else if (id==kLeatherBelt)				strcat(itemName,"Leather Belt");
		else if (id==kAncientTome)				strcat(itemName,"Ancient Tome");
		else if (id==kIceCrystal)				  strcat(itemName,"Ice Crystal");

		else if (id==kGoldMirror)				  strcat(itemName,"Gold Mirror");
		else if (id==kBlueTicket)				strcat(itemName,"Blue Ticket");
		else if (id==kBookOfTheDead)			strcat(itemName,"Book of the Dead");
		else if (id==kMagicRune)				  strcat(itemName,"Magic Rune");
		else if (id==kSilverDragonClaw)		strcat(itemName,"Silver Dragon Claw");
		else if (id==kRubyHeart)				  strcat(itemName,"Ruby Heart");
		else if (id==kBookOfTheStars)			strcat(itemName,"Book of the Stars");
		else if (id==kKingsSeal)				  strcat(itemName,"King's Seal");
		else if (id==kCrystalFlask)				strcat(itemName,"Crystal Flask");
		else if (id==kSilverLeaf)				  strcat(itemName,"Silver Leaf");
		else if (id==kTomeOfTheDamned)	  strcat(itemName,"Tome of the Damned");
		else if (id==kTomeOfLife)				  strcat(itemName,"Tome of Life");
		else if (id==kGoldenFleece)				strcat(itemName,"Golden Fleece");
		else if (id==kGuildHallDeed)			strcat(itemName,"Guild Hall Deed");
		else if (id==kHouseDeed)			    strcat(itemName,"House Deed");

		else if (id==kCrystalSkull)				strcat(itemName,"Shattered Crystal Skull");
		else if (id==kCrystalSkullPure)			strcat(itemName,"Crystal Skull");
		else if (id==kKingsPortrait)			strcat(itemName,"King's Portrait");
		else if (id==kPersianRug)				  strcat(itemName,"Persian Rug");

		//else if (id==kLightBlueBreastLeather)				strcat(itemName,"Light Blue Leather Breast");
		//else if (id==kRedBreastLeather)							strcat(itemName,"Red Leather Breast");
		//else if (id==kBlueBreastLeather)						strcat(itemName,"Blue Leather Breast");
		else if (id==kBrownBreastLeather)						strcat(itemName,"Leather Breast");
		else if (id==kBrownLeatherLegs)						  strcat(itemName,"Leather Legs");
		else if (id==kBrownLeatherArms)						  strcat(itemName,"Leather Arms");

		else if (id==kBlackBreastLeather)						strcat(itemName,"Black Leather Breast");
		else if (id==kGreenBreastLeather)						strcat(itemName,"Green Leather Breast");
		else if (id==kPurpleBreastLeather)					strcat(itemName,"Purple Leather Breast");
		else if (id==kYellowBreastLeather)					strcat(itemName,"Yellow Leather Breast");

		else if (id==kFishSteak)					strcat(itemName,"Fish Steak");
		else if (id==kCookedSteak)				strcat(itemName,"Steak");
		else if (id==kRawSteak)						strcat(itemName,"Raw Steak");
		else if (id==kLoafOfBread)				strcat(itemName,"Loaf of Bread");
		else if (id==kCandy)				      strcat(itemName,"Piece of Candy");
		else if (id==kApple)							strcat(itemName,"Apple");
		else if (id==kBanana)							strcat(itemName,"Banana");

		else if (id==kGoldenBox)					strcat(itemName, "Golden Box");
		else if (id==kBody)							strcat(itemName, "Body");

		else if (id==kRedEgg)				      strcat(itemName,"Red Egg");
		else if (id==kPurpleEgg)				  strcat(itemName,"Purple Egg");
		else if (id==kGreenEgg)				    strcat(itemName,"Green Egg");
		else if (id==kBlueEgg)				    strcat(itemName,"Blue Egg");
		else if (id==kBlackEgg)				    strcat(itemName,"Black Egg");
		
		else if (id==kBlueFlag)				    strcat(itemName,"Blue Flag");
		else if (id==kRedFlag)				    strcat(itemName,"Red Flag");
		else if (id==kGreenFlag)				  strcat(itemName,"Green Flag");
		else if (id==kYellowFlag)				  strcat(itemName,"Yellow Flag");

		else if (id==kOrbOfSeeing)				strcat(itemName,"Orb of Seeing");
		else if (id==kRedRose)						strcat(itemName,"Red Rose");
		else if (id==kYellowRose)					strcat(itemName,"Yellow Rose");
		else if (id==kBlackRose)					strcat(itemName,"Black Rose");
		//Dave
		else if (id==kBlueRose)					strcat(itemName,"Blue Rose");
		else if (id==kPurpleRose)					strcat(itemName,"Purple Rose");
		
		
		else if (id==kSpiderFang)		      strcat(itemName,"Spider Fang");
		else if (id==kScorpionStinger)		strcat(itemName,"Scorpion Stinger");

		else if (id==kGrayWizardHat)			strcat(itemName,"Gray Wizard Hat");
		else if (id==kLightBlueWizardHat)	strcat(itemName,"Light Blue Wizard Hat");
		else if (id==kRedWizardHat)				strcat(itemName,"Red Wizard Hat");
		else if (id==kBlueWizardHat)			strcat(itemName,"Blue Wizard Hat");
		else if (id==kBrownWizardHat)			strcat(itemName,"Brown Wizard Hat");
		else if (id==kBlackWizardHat)			strcat(itemName,"Black Wizard Hat");
		else if (id==kGreenWizardHat)			strcat(itemName,"Green Wizard Hat");
		else if (id==kPurpleWizardHat)		strcat(itemName,"Purple Wizard Hat");
		else if (id==kYellowWizardHat)		strcat(itemName,"Yellow Wizard Hat");

		else if (id==kGrayJesterHat)			strcat(itemName,"Gray Jester Hat");
		else if (id==kLightBlueJesterHat)	strcat(itemName,"Light Blue Jester Hat");
		else if (id==kRedJesterHat)				strcat(itemName,"Red Jester Hat");
		else if (id==kBlueJesterHat)			strcat(itemName,"Blue Jester Hat");
		else if (id==kBrownJesterHat)			strcat(itemName,"Brown Jester Hat");
		else if (id==kBlackJesterHat)			strcat(itemName,"Black Jester Hat");
		else if (id==kGreenJesterHat)			strcat(itemName,"Green Jester Hat");
		else if (id==kPurpleJesterHat)		strcat(itemName,"Purple Jester Hat");
		else if (id==kYellowJesterHat)		strcat(itemName,"Yellow Jester Hat");

		else if (id==kGoldCrown)				  strcat(itemName,"Gold Crown");

		else if (id==kEmptyBottle)				strcat(itemName,"Empty Bottle");
		else if (id==kLog)								strcat(itemName,"Log");
		else if (id==kFerriteOre)					strcat(itemName,"Ferrite");
		else if (id==kEtherite)						strcat(itemName,"Etherite");
		else if (id==kEtheriteWood)					strcat(itemName,"Extract Etherite (Log)");
		else if (id==kEtheriteFerrite)				strcat(itemName,"Extract Etherite (Ferrite)");
		//Dave
		else if (id==kBlackEtherite)				strcat(itemName,"Extract Etherite (Black Ferrite)");
		else if (id==kIngot)					    strcat(itemName,"Ingot");
		else if (id==kFishingPole)				strcat(itemName,"Fishing Pole");
		else if (id==kLobsterPole)				strcat(itemName,"Lobster Pole");
		else if (id==kEelPole)				    strcat(itemName,"Eel Pole");
		else if (id==kTrout)							strcat(itemName,"Trout");
		else if (id==kRaft)				        strcat(itemName,"Raft");
		else if (id==kLock)					      strcat(itemName,"Lock");
		else if (id==kTrap)					      strcat(itemName,"Trap");
		else if (id==kBandage)						strcat(itemName,"Bandage");
		else if (id==kEelPie)							strcat(itemName,"Eel Pie");
		else if (id==kLobsterPie)					strcat(itemName,"Lobster Pie");
		else if (id==kDriedTrout)					strcat(itemName,"Dried Trout");
		else if (id==kApplePie)						strcat(itemName,"Apple Pie");
		else if (id==kGingerBread)				strcat(itemName,"Ginger Bread");
		else if (id==kOatMealCake)				strcat(itemName,"Oat Meal Cake");
		else if (id==kLobsterSoup)				strcat(itemName,"Lobster Soup");
		else if (id==kMugOfAle)				    strcat(itemName,"Mug Of Ale");
		else if (id==kBoiledLobster)			strcat(itemName,"Boiled Lobster");
		else if (id==kBagOfFlour)					strcat(itemName,"Bag of Flour");
		else if (id==kBananaPie)					strcat(itemName,"Banana Pie");
		else if (id==kRoastedGaperStalk)	strcat(itemName,"Roasted Gaper Stalk");

		else if (id==kGrayShirt)					strcat(itemName,"Gray Shirt");
		else if (id==kLightBlueShirt)			strcat(itemName,"Light Blue Shirt");
		else if (id==kRedShirt)						strcat(itemName,"Red Shirt");
		else if (id==kBlueShirt)					strcat(itemName,"Blue Shirt");
		else if (id==kBrownShirt)					strcat(itemName,"Brown Shirt");
		else if (id==KBlackShirt)					strcat(itemName,"Black Shirt");
		else if (id==kGreenShirt)					strcat(itemName,"Green Shirt");
		else if (id==kPurpleShirt)				strcat(itemName,"Purple Shirt");
		else if (id==kYellowShirt)				strcat(itemName,"Yellow Shirt");

		else if (id==kGrayPants)					strcat(itemName,"Pair of Gray Pants");
		else if (id==kLightBluePants)			strcat(itemName,"Pair of Light Blue Pants");
		else if (id==kRedPants)						strcat(itemName,"Pair of Red Pants");
		else if (id==kBluePants)					strcat(itemName,"Pair of Blue Pants");
		else if (id==kBrownPants)					strcat(itemName,"Pair of Brown Pants");
		else if (id==KBlackPants)					strcat(itemName,"Pair of Black Pants");
		else if (id==kGreenPants)					strcat(itemName,"Pair of Green Pants");
		else if (id==kPurplePants)				strcat(itemName,"Pair of Purple Pants");
		else if (id==kYellowPants)				strcat(itemName,"Pair of Yellow Pants");

		else if (id==kLightBlueBoots)			strcat(itemName,"Pair of Light Blue Boots");
		else if (id==kRedBoots)						strcat(itemName,"Pair of Red Boots");
		else if (id==kBlueBoots)					strcat(itemName,"Pair of Blue Boots");
		else if (id==kBrownBoots)					strcat(itemName,"Pair of Brown Boots");
		else if (id==kBlackBoots)					strcat(itemName,"Pair of Black Boots");
		else if (id==kGreenBoots)					strcat(itemName,"Pair of Green Boots");
		else if (id==kPurpleBoots)				strcat(itemName,"Pair of Purple Boots");
		else if (id==kYellowBoots)				strcat(itemName,"Pair of Yellow Boots");
		else if (id==kGrayBoots)					strcat(itemName,"Pair of Gray Boots");

		else if (id==kRoyalRobe)					strcat(itemName,"Royal Robe");
		else if (id==kGMRobe)						  strcat(itemName,"GM Robe");
		else if (id==kGrayRobe)						strcat(itemName,"Gray Robe");
		else if (id==kLightBlueRobe)			strcat(itemName,"Light Blue Robe");
		else if (id==kRedRobe)						strcat(itemName,"Red Robe");
		else if (id==kBlueRobe)						strcat(itemName,"Blue Robe");
		else if (id==kBrownRobe)					strcat(itemName,"Brown Robe");
		else if (id==KBlackRobe)					strcat(itemName,"Black Robe");
		else if (id==kGreenRobe)					strcat(itemName,"Green Robe");
		else if (id==kPurpleRobe)					strcat(itemName,"Purple Robe");
		else if (id==kYellowRobe)					strcat(itemName,"Yellow Robe");
		else if (id==kOrangeRobe)					strcat(itemName,"Orange Robe");
		else if (id==kWhiteRobe)					strcat(itemName,"White Robe");

		else if (id==kGrayRobeProtection)						strcat(itemName,"Gray Robe of Protection");
		else if (id==kLightBlueRobeProtection)			strcat(itemName,"Light Blue Robe of Protection");
		else if (id==kRedRobeProtection)						strcat(itemName,"Red Robe of Protection");
		else if (id==kBlueRobeProtection)						strcat(itemName,"Blue Robe of Protection");
		else if (id==kBrownRobeProtection)					strcat(itemName,"Brown Robe of Protection");
		else if (id==kBlackRobeProtection)					strcat(itemName,"Black Robe of Protection");
		else if (id==kGreenRobeProtection)					strcat(itemName,"Green Robe of Protection");
		else if (id==kPurpleRobeProtection)					strcat(itemName,"Purple Robe of Protection");
		else if (id==kYellowRobeProtection)					strcat(itemName,"Yellow Robe of Protection");

		else if (id==kGrayRobeResistance)						strcat(itemName,"Gray Robe of Resistance");
		else if (id==kLightBlueRobeResistance)			strcat(itemName,"Light Blue Robe of Resistance");
		else if (id==kRedRobeResistance)						strcat(itemName,"Red Robe of Resistance");
		else if (id==kBlueRobeResistance)						strcat(itemName,"Blue Robe of Resistance");
		else if (id==kBrownRobeResistance)					strcat(itemName,"Brown Robe of Resistance");
		else if (id==KBlackRobeResistance)					strcat(itemName,"Black Robe of Resistance");
		else if (id==kGreenRobeResistance)					strcat(itemName,"Green Robe of Resistance");
		else if (id==kPurpleRobeResistance)					strcat(itemName,"Purple Robe of Resistance");
		else if (id==kYellowRobeResistance)					strcat(itemName,"Yellow Robe of Resistance");

		else if (id==kLobster)						strcat(itemName,"Lobster");
		else if (id==kSkull)						  strcat(itemName,"Skull");
		else if (id==kWarHammer)					strcat(itemName,"War Hammer");
		else if (id==kMace)								strcat(itemName,"Mace");
		//else if (id==kRoyalScepter)						strcat(itemName,"Royal Sceptre");  //Dave 11-15-2012
		//else if (id==kMiningShovel)				strcat(itemName,"Mining Shovel");
		else if (id==kBlacksmithHammer)		strcat(itemName,"Blacksmith Hammer");
		else if (id==kCarvingKnife)		    strcat(itemName,"Carving Knife");
		else if (id==kMiningAxe)					strcat(itemName,"Mining Axe");
		else if (id==kCoalAxe)					  strcat(itemName,"Coal Axe");
		else if (id==kWoodworkingTools)		strcat(itemName,"Set of Woodworking Tools");
		else if (id==kTinkeringTools)			strcat(itemName,"Set of Tinkering Tools");
		else if (id==kPackOfNails)				strcat(itemName,"Pack of Nails");
		else if (id==kBoard)							strcat(itemName,"Board");
		else if (id==kArrow)							strcat(itemName,"Arrow");
		else if (id==kBolt)								strcat(itemName,"Bolt");

		//else if (id==kBlankScroll)				strcat(itemName,"Blank Scroll");

		else if (id==kLesserHealPotion)		strcat(itemName,"Lesser Heal Potion");
		else if (id==kGreaterHealPotion)	strcat(itemName,"Greater Heal Potion");
		else if (id==kFullHealPotion)			strcat(itemName,"Full Heal Potion");

		else if (id==kManaPotion)					strcat(itemName,"Mana Potion");

		else if (id==kLesserDetoxifyPotion)			strcat(itemName,"Lesser Detoxify Potion");
		else if (id==kGreaterDetoxifyPotion)		strcat(itemName,"Greater Detoxify Potion");
		else if (id==kFullDetoxifyPotion)				strcat(itemName,"Full Detoxify Potion");

		else if (id==kStrengthPotion)			strcat(itemName,"Strength Potion");
		else if (id==kAgilityPotion)			strcat(itemName,"Agility Potion");
		else if (id==kElevateMindPotion)	strcat(itemName,"Elevate Mind Potion");
		else if (id==kOpportunityPotion)	strcat(itemName,"Opportunity Potion");
		else if (id==kNightVisionPotion)	strcat(itemName,"Night Vision Potion");
		else if (id==kInvisibilityPotion)	strcat(itemName,"Invisibility Potion");
		else if (id==kNourishPotion)			strcat(itemName,"Nourish Potion");

		else if (id==kPoison)	            strcat(itemName,"Bottle of Poison");

		else if (id==kDiamond)						strcat(itemName,"Diamond");

		else if (id==kGreenCrystal)				strcat(itemName,"Green Crystal");
		else if (id==kRedCrystal)					strcat(itemName,"Red Crystal");
		else if (id==kBlueCrystal)				strcat(itemName,"Blue Crystal");
		else if (id==kPurpleCrystal)			strcat(itemName,"Purple Crystal");
		else if (id==kOrangeCrystal)			strcat(itemName,"Orange Crystal");
		else if (id==kYellowCrystal)			strcat(itemName,"Yellow Crystal");
		else if (id==kFireCrystal)				strcat(itemName,"Fire Crystal");
		else if (id==kBlackCrystal)				strcat(itemName,"Black Crystal");

		else if (id==kPureGreenCrystal)				strcat(itemName,"Pure Green Crystal");
		else if (id==kPureRedCrystal)					strcat(itemName,"Pure Red Crystal");
		else if (id==kPureBlueCrystal)				strcat(itemName,"Pure Blue Crystal");
		else if (id==kPurePurpleCrystal)			strcat(itemName,"Pure Purple Crystal");
		else if (id==kPureOrangeCrystal)			strcat(itemName,"Pure Orange Crystal");
		else if (id==kPureYellowCrystal)			strcat(itemName,"Pure Yellow Crystal");
		else if (id==kPureFireCrystal)				strcat(itemName,"Pure Fire Crystal");
		else if (id==kPureBlackCrystal)			  strcat(itemName,"Pure Black Crystal");
		else if (id==kPureIceCrystal)			  strcat(itemName,"Pure Ice Crystal");
		
		else if (id==kInfusedGreen)					strcat(itemName,"Infused Green Crystal");
		else if (id==kInfusedRed)					strcat(itemName,"Infused Red Crystal");
		else if (id==kInfusedBlue)					strcat(itemName,"Infused Blue Crystal");
		else if (id==kInfusedYellow)					strcat(itemName,"Infused Yellow Crystal");
		else if (id==kInfusedOrange)					strcat(itemName,"Infused Orange Crystal");
		else if (id==kInfusedPurple)					strcat(itemName,"Infused Purple Crystal");
		else if (id==kInfusedBlack)					strcat(itemName,"Infused Black Crystal");
		else if (id==kInfusedPGreen)					strcat(itemName,"Infused Pure Green Crystal");
		else if (id==kInfusedPRed)					strcat(itemName,"Infused Pure Red Crystal");
		else if (id==kInfusedPBlue)					strcat(itemName,"Infused Pure Blue Crystal");
		else if (id==kInfusedPYellow)					strcat(itemName,"Infused Pure Yellow Crystal");
		else if (id==kInfusedPOrange)					strcat(itemName,"Infused Pure Orange Crystal");
		else if (id==kInfusedPPurple)					strcat(itemName,"Infused Pure Purple Crystal");
		else if (id==kInfusedPBlack)					strcat(itemName,"Infused Pure Black Crystal");
		else if (id==kInfusedFire)					strcat(itemName,"Infused Fire Crystal");
		else if (id==kInfusedIce)					strcat(itemName,"Infused Ice Crystal");
		else if (id==kInfusedCS)					strcat(itemName,"Infused Crystal Skull");

		else if (id==kBurningPlateBreast)		strcat(itemName,"Burning Plate Breast");
		else if (id==kBurningPlateLegs)			  strcat(itemName,"Burning Plate Legs");
		else if (id==kBurningPlateHelmet)		strcat(itemName,"Burning Plate Helmet");
		else if (id==kBurningPlateArms)			  strcat(itemName,"Burning Plate Arms");
		else if (id==kBurningPlateGauntlets)	strcat(itemName,"Burning Plate Gauntlets");

		else if (id==kBurningMetalShield)			  strcat(itemName,"Burning Metal Shield");
		else if (id==kBlackMetalShield)			  strcat(itemName,"Black Metal Shield");

		else if (id==kBrigandHead)			  strcat(itemName,"Brigand Head");
		else if (id==kSorcererHead)			  strcat(itemName,"Sorcerer Head");
		else if (id==kPirateHead)			    strcat(itemName,"Pirate Head");

		else if (id==kRuby)								strcat(itemName,"Ruby");
		else if (id==kQueensRuby)					strcat(itemName,"Queen's Ruby");

		else if (id==kCrystalRing)				strcat(itemName,"Crystal Ring");
		else if (id==kPlatinumRing)				strcat(itemName,"Platinum Ring");
		else if (id==kGoldRing)						strcat(itemName,"Gold Ring");
		else if (id==kSilverRing)					strcat(itemName,"Silver Ring");

		else if (id==kDragonStoneRing)		strcat(itemName,"Dragon Stone Ring");
		else if (id==kElementalRing)			strcat(itemName,"Elemental Ring");
		else if (id==kPoisonRing)					strcat(itemName,"Ring of Immunity");
		else if (id==kNeptunesRing)				strcat(itemName,"Neptune's Ring");
		else if (id==kHadesRing)					strcat(itemName,"Hades Ring");
		else if (id==kManaRing)					  strcat(itemName,"Mana Ring");
		else if (id==kDruidRing)					strcat(itemName,"Druid Ring");
		else if (id==kChaosRing)					strcat(itemName,"Chaos Ring");
		else if (id==kRingOfLife)				strcat(itemName,"Ring of Life");

		else if (id==kSilverNecklace)			strcat(itemName,"Silver Necklace");

		else if (id==kShaft)							strcat(itemName,"Shaft");
		else if (id==kFeather)						strcat(itemName,"Feather");
		else if (id==kRollOfCloth)				strcat(itemName,"Roll of Cloth");
		else if (id==kTurtleShell)				strcat(itemName,"Turtle Shell");
		else if (id==kBundleOfWool)				strcat(itemName,"Bundle of Wool");

		else if (id==kBrackenSeed)				strcat(itemName,"Bracken Seed");
		else if (id==kGinseng)						strcat(itemName,"Ginseng");
		else if (id==kLotusFlower)				strcat(itemName,"Lotus Flower");
		else if (id==kMandrakeRoot)				strcat(itemName,"Mandrake Root");
		else if (id==kAmber)						  strcat(itemName,"Amber");
		else if (id==kGarlic)							strcat(itemName,"Garlic");
		else if (id==kVolcanicAsh)				strcat(itemName,"Volcanic Ash");
		else if (id==kPoisonGland)				strcat(itemName,"Poison Gland");
		else if (id==kCoal)						    strcat(itemName,"Piece of Coal");
		else if (id==kElectricEel)				strcat(itemName,"Electric Eel");
		else if (id==kDragonTooth)				strcat(itemName,"Dragon Tooth");
		else if (id==kDragonScale)				strcat(itemName,"Dragon Scale");
		else if (id==kGaperStalk)				  strcat(itemName,"Gaper Stalk");
		else if (id==kSackOfSpice)				strcat(itemName,"Sack of Spice");

		else if (id==kLightningBoltSpell)		strcat(itemName,"Lightning Bolt");
		else if (id==kResurrectSpell)				strcat(itemName,"Resurrect");

		else if (id==kLesserHealSpell)			strcat(itemName,"Lesser Heal");
		else if (id==kGreaterHealSpell)			strcat(itemName,"Greater Heal");
		else if (id==kRemoveCurseSpell)			strcat(itemName,"Remove Curse");
		else if (id==kFullHealSpell)				strcat(itemName,"Full Heal");

		else if (id==kFireFieldSpell)				strcat(itemName,"Fire Field");

		else if (id==kReflectiveArmorSpell)	strcat(itemName,"Reflective Armor");
		else if (id==kClumsySpell)					strcat(itemName,"Clumsy");
		else if (id==kCreateFoodSpell)			strcat(itemName,"Create Food");
		else if (id==kTurnUndeadSpell)			strcat(itemName,"Turn Undead");
		else if (id==kSimpleMindSpell)			strcat(itemName,"Simple Mind");
		else if (id==kNightVisionSpell)			strcat(itemName,"Night Vision");
		else if (id==kWeakenSpell)					strcat(itemName,"Weaken");
		else if (id==kAgilitySpell)					strcat(itemName,"Agility");
		else if (id==kElevateMindSpell)			strcat(itemName,"Elevate Mind");
		else if (id==kFireballSpell)			strcat(itemName,"Fireball");
		else if (id==kFireblastSpell)			strcat(itemName,"FX Test 1");
		else if (id==kMagicMissileSpell)			strcat(itemName,"Magic Arrow");
		else if (id==kFireblast3Spell)			strcat(itemName,"FX Test 3");
		else if (id==kFireblast4Spell)			strcat(itemName,"Blaze");
		else if (id==kFireblast5Spell)			strcat(itemName,"FX Test 5");
		//Dave
		else if (id==kChaosFieldSpell)			strcat(itemName,"Chaos Field");
		else if (id==kLesserDetoxifySpell)	strcat(itemName,"Lesser Detoxify");
		else if (id==kGreaterDetoxifySpell)	strcat(itemName,"Greater Detoxify");
		else if (id==kFullDetoxifySpell)		strcat(itemName,"Full Detoxify");
		else if (id==kMagicTrapSpell)				strcat(itemName,"Magic Trap");
		else if (id==kMagicDisarmSpell)			strcat(itemName,"Magic Disarm");
		else if (id==kStrengthSpell)				strcat(itemName,"Strength");
		else if (id==kSummonPetSpell)				strcat(itemName,"Summon Pet");
		else if (id==kHealPetSpell)			strcat(itemName,"Heal Pet");
		else if (id==kPoisonSpell)					strcat(itemName,"Poison");
		else if (id==kTeleportSpell)				strcat(itemName,"Teleport");
		else if (id==kDrainManaSpell)				strcat(itemName,"Drain Mana");
		else if (id==kMagicReflectionSpell)	strcat(itemName,"Magic Reflection");
		else if (id==kDisorientationSpell)	strcat(itemName,"Disorient");
		else if (id==kParalyzeSpell)				strcat(itemName,"Paralyze");
		else if (id==kSummonCreatureSpell)	strcat(itemName,"Summon Creature");
		//Dave
		else if (id==kGreaterSummonCreatureSpell)	strcat(itemName,"Greater Summon Creature");
		else if (id==kExplosionSpell)				strcat(itemName,"Explosion");
		else if (id==kInvisibilitySpell)		strcat(itemName,"Invisibility");
		else if (id==kRevealSpell)					strcat(itemName,"Reveal");
		else if (id==kParalyzeFieldSpell)		strcat(itemName,"Paralyze Field");
		else if (id==kPoisonFieldSpell)			strcat(itemName,"Poison Field");
		else if (id==kMassHealSpell)			strcat(itemName,"Mass Heal");
		else if (id==kShapeshiftLesserSpell)	strcat(itemName, "Lesser Shapeshift");
		else if (id==kShapeshiftSpell)			strcat(itemName, "Shapeshift");
		else if (id==kShapeshiftGreaterSpell)	strcat(itemName, "Greater Shapeshift");
		else if (id==kShapeshiftAllSpell)		strcat(itemName, "Shapeshift: All");
		else if (id==kShapeshiftDragonSpell)	strcat(itemName, "Shapeshift: Dragon");
		else if (id==kDisorientFieldSpell)		strcat(itemName,"Disorient Field");
		else if (id==kProvocationSpell)			strcat(itemName,"Provocation");
		else if (id==kCalmSpell)					  strcat(itemName,"Calm");
		else if (id==kOpportunitySpell)				strcat(itemName,"Opportunity");

		//else if (id==kManaStealSpell)				strcat(itemName,"Mana Steal");
		
		else if (id==kAttackBackstab)			strcat(itemName,"Backstab");
		else if (id==kAttackBoomingBlow)		strcat(itemName,"Booming Blow");
		else if (id==kAttackDecimate)			strcat(itemName,"Decimate");
		else if (id==kAttackInstantStrike)		strcat(itemName,"Instant Strike");
		else if (id==kAttackVial)				strcat(itemName,"Explosive Dust");
		else if (id==kAttackDoubleStrike)		strcat(itemName,"Double Strike");
		else if (id==kAttackDoubleShot)			strcat(itemName,"Double Shot");
		else if (id==kAttackSting)				strcat(itemName,"Sting");
		else if (id==kAttackGrapplingShot)		strcat(itemName,"Grappling Shot");
		else if (id==kAttackEscapeMove)			strcat(itemName,"Escape Move");
		else if (id==kAttackLesserDefense)		strcat(itemName,"Lesser Defensive Stance");
		else if (id==kAttackGreaterDefense)		strcat(itemName,"Greater Defensive Stance");
		else if (id==kAttackRage)				strcat(itemName,"Rage");
		else if (id==kAttackKnockback)			strcat(itemName,"Knockback");
		else if (id==kAttackAimedShot)			strcat(itemName,"Aimed Shot");
		else if (id==kAttackTaunt)				strcat(itemName,"Challenging Strike");
		else if (id==kAttackRangerSwiftness)	strcat(itemName,"Ranger's Swiftness");
		else if (id==kAttackQuickDraw)			strcat(itemName,"Battle Ready");		

		else if (id==kLightBlueDye)				strcat(itemName,"Bottle of Light Blue Dye");
		else if (id==kRedDye)							strcat(itemName,"Bottle of Red Dye");
		else if (id==kBlueDye)						strcat(itemName,"Bottle of Blue Dye");
		else if (id==kBrownDye)						strcat(itemName,"Bottle of Brown Dye");
		else if (id==kBlackDye)						strcat(itemName,"Bottle of Black Dye");
		else if (id==kGreenDye)						strcat(itemName,"Bottle of Green Dye");
		else if (id==kPurpleDye)					strcat(itemName,"Bottle of Purple Dye");
		else if (id==kYellowDye)					strcat(itemName,"Bottle of Yellow Dye");
		//Dave
		else if (id==kGrayDye)						strcat(itemName,"Bottle of Gray Dye");
		else if (id==kOrangeDye)					strcat(itemName,"Bottle of Orange Dye");
		else if (id==kLeatherHide)					strcat(itemName,"Leather Hide");
		else if (id==kFurSkin)						strcat(itemName,"Fur");

		else if (id==kLesserHealRing)				strcat(itemName, "Ring of Lesser Heal");
		else if (id==kGreaterHealRing)				strcat(itemName, "Ring of Greater Heal");
		else if (id==kLightningBoltRing)			strcat(itemName, "Ring of Lightning Bolt");
		else if (id==kRemoveCurseRing)				strcat(itemName, "Ring of Remove Curse");
		else if (id==k5ARRing)						strcat(itemName, "Lesser Protection Ring");
		else if (id==k10ARRing)						strcat(itemName, "Medium Protection Ring");
		else if (id==k15ARRing)						strcat(itemName, "Greater Protection Ring");
		else if (id==kCurio1)						strcat(itemName, "Mirith's Moon Curio Third");
		else if (id==kCurio2)						strcat(itemName, "Andris's Moon Curio Third");
		else if (id==kCurio3)						strcat(itemName, "Marali's Moon Curio Third");
		else if (id==kCurioBig)						strcat(itemName, "Moon Curio");
		else if (id==kCurioSmall)					strcat(itemName, "Moon Curio Fragment");
		else if (id==kSmallBag)					strcat(itemName, "Small Bag");
		else if (id==kLockbox)					strcat(itemName, "Lockbox");
		else if (id==kPapers)					strcat(itemName, "Papers");
		else if (id==kMagicStone)					strcat(itemName, "Magic Stone");
		else if (id==kSmallKey)					strcat(itemName, "Small Key");
		else if (id==kMessage)					strcat(itemName, "Message");
		else if (id==kCrate)					strcat(itemName, "Crate");
		else if (id==kVialOfBlood)					strcat(itemName, "Vial of Blood");
		else if (id==kDocuments)					strcat(itemName, "Documents");
		else if (id==kLetter)					strcat(itemName, "Letter");
		else if (id==kRustyKey)					strcat(itemName, "Rusty Key");
		else if (id==kIronKey)					strcat(itemName, "Iron Key");
		else if (id==kBronzeKey)					strcat(itemName, "Bronze Key");
		else if (id==kGoldenBolt)					strcat(itemName, "Golden Bolt");
		else if (id==kSilverBolt)					strcat(itemName, "Silver Bolt");
		else if (id==kDragonEggGreen)					strcat(itemName, "Green Dragon Egg");
		else if (id==kDragonEggGold)					strcat(itemName, "Golden Dragon Egg");
		else if (id==kDragonEggBlack)					strcat(itemName, "Black Dragon Egg");
		else if (id==kDragonEggRed)					strcat(itemName, "Red Dragon Egg");
		else if (id==kDragonEggBlue)					strcat(itemName, "Ice Dragon Egg");
		else if (id==kDragonEggPurple)					strcat(itemName, "Abyss Dragon Egg");
		else if (id==kKeyBow)						strcat(itemName, "Key Bow");
		else if (id==kKeyCollar)					strcat(itemName, "Key Collar");
		else if (id==kKeyShank)						strcat(itemName, "Key Shank");
		else if (id==kKeyShoulder)					strcat(itemName, "Key Shoulder");
		else if (id==kKeyStem)						strcat(itemName, "Key Stem");
		else if (id==kKeyBit)						strcat(itemName, "Key Bit");
		else if (id==kSnakeSkin)					strcat(itemName, "Snake Skin");
		else if (id==kSunstoneRing)					strcat(itemName, "Sunstone Ring");
		else if (id==kFlaskOfPerfume)				strcat(itemName, "Flask of Perfume");
		else if (id==kLittleStatue)					strcat(itemName, "Little Statue");
		else if (id==kLockOfHair)					strcat(itemName, "Lock Of Hair");
		else if (id==kJar)							strcat(itemName, "Jar");
		else if (id==kParchment)					strcat(itemName, "Parchment");
		else if (id==kBoneKnife)					strcat(itemName, "Bone Knife");
		else if (id==kPearlNecklace)				strcat(itemName, "Pearl Necklace");
		else if (id==kStatueOfElara)				strcat(itemName, "Statue of Elara");
		else if (id==kMysticSigil)					strcat(itemName, "Mystic Sigil");
		else if (id==kArcaneGrimoire)				strcat(itemName, "Arcane Grimoire");
		else if (id==kSilverChalice)				strcat(itemName, "Silver Chalice");
		else if (id==kInfernalTome)					strcat(itemName, "Infernal Tome");
		else if (id==kEmeraldPrism)					strcat(itemName, "Emerald Prism");
		else if (id==kSapphirePrism)				strcat(itemName, "Sapphire Prism");
		else if (id==kAmethystPrism)				strcat(itemName, "Amethyst Prism");
		else if (id==kRubyPrism)					strcat(itemName, "Ruby Prism");
		else if (id==kTopazPrism)					strcat(itemName, "Topaz Prism");
		else if (id==kDiamondPrism)					strcat(itemName, "Diamond Prism");
		else if (id==kGoldCandelabrum)				strcat(itemName, "Gold Candelabrum");
		else if (id==kBookofMoons)					strcat(itemName, "Book of Moons");
		else if (id==kBlinkHoundFur)				strcat(itemName, "Blink Hound Fur");
		else if (id==kZionidicCrystal)				strcat(itemName, "Zionidic Crystal");
		else if (id==kVermaStone)					strcat(itemName, "Verma Stone");
		else if (id==kSpiralCharm)					strcat(itemName, "Spiral Charm");
		else if (id==kGoldThread)					strcat(itemName, "Gold Thread");
		else if (id==kVialOfCentaurBlood)			strcat(itemName, "Vial of Centaur Blood");
		else if (id==kRoyalSceptre)					strcat(itemName, "Royal Sceptre");
		else if (id==kMirithToken)					strcat(itemName, "Mirith Token");
		else if (id==kMaraliToken)					strcat(itemName, "Marali Token");
		else if (id==kAndrisToken)					strcat(itemName, "Andris Token");
		else if (id==kMayorSeal)					strcat(itemName, "Mayor's Seal");
		else if (id==kSenateSeal)					strcat(itemName, "Senate's Seal");
		else if (id==kCommanderSeal)				strcat(itemName, "Commander's Seal");
		else if (id==kStoreToken)					strcat(itemName, "Store Token");
		else if (id==kIvoryTusk)					strcat(itemName, "Ivory Tusk");
		else if (id==kShevPortrait)					strcat(itemName, "Portrait of Shevarash");
		else if (id==kSickHorse)					strcat(itemName, "Charm of Speed");
		else if (id==kHorse)						strcat(itemName, "Talisman of Speed");
		else if (id==kFastHorse)					strcat(itemName, "Fast Horse");
		else if (id==kVeryFastHorse)				strcat(itemName, "Very Fast Horse");
		else if (id==kMagicMount1)					strcat(itemName, "Magic Mount 1");
		else if (id==kMagicMount2)					strcat(itemName, "Magic Mount 2");
		else if (id==kMagicMount3)					strcat(itemName, "Magic Mount 3");
		else if (id==kMagicMount4)					strcat(itemName, "Magic Mount 4");
		else if (id==kGMHorse)						strcat(itemName, "GM Horse");
		else if (id==kGaperEye)						strcat(itemName, "Gaper Eye");
		else if (id==kElementalDust)				strcat(itemName, "Elemental Dust");									
		else if (id==kAcidicGland)					strcat(itemName, "Acidic Gland");							
		else if (id==kBrackenRoots)					strcat(itemName, "Bracken Root");
		else if (id==kBoneMageStaff)				strcat(itemName, "Bone Mage Stave");
		else if (id==kHarpieBeak)					strcat(itemName, "Harpie Beak");
		else if (id==kLizardScale)					strcat(itemName, "Lizard Scale");	
		else if (id==kBatWing)						strcat(itemName, "Bat Wing");
		else if (id==kSpiderLeg)					strcat(itemName, "Spider Leg");	
		else if (id==kSnailSlime)					strcat(itemName, "Slime");									
		else if (id==kZombieBrain)					strcat(itemName, "Zombie Brain");	
		else if (id==kRustGland)					strcat(itemName, "Rust Gland");
		else if (id==kGiantHeart)					strcat(itemName, "Giant Heart");	
		else if (id==kSnakeEgg)						strcat(itemName, "Snake Egg");
		else if (id==kAntMandibles)					strcat(itemName, "Ant Mandible");								
		else if (id==kCubeGoo)						strcat(itemName, "Goo");
		else if (id==kTuskerTail)					strcat(itemName, "Tusker Tail");
		else if (id==kAncientRune)					strcat(itemName, "Ancient Rune");
		else if (id==kNeviaFlower)					strcat(itemName, "Nevia Flower");
		else if (id==kSmallMoonFragment)			strcat(itemName, "Small Moon Fragment");
		else if (id==kMerefolkHair)					strcat(itemName, "Merfolk Hair");
		else if (id==kRustyNecklace)				strcat(itemName, "Tarnished Necklace");
		else if (id==kPureCrystalRing)				strcat(itemName, "Pure Crystal Ring");
		else if (id==kEnchantedCloth)				strcat(itemName, "Enchanted Cloth");
		else if (id==kDiamondDust)					strcat(itemName, "Diamond Dust");
		else if (id==kDiscretionRing)				strcat(itemName, "Discretion Ring");
		else if (id==kCharmOfUndeath)				strcat(itemName, "Charm of Undead Cleansing");
		else if (id==kVialOfAcid)					strcat(itemName, "Explosive Dust");
		else if (id==kRingOfStrength)				strcat(itemName, "Ring of Strength");
		else if (id==kRingOfStealth)				strcat(itemName, "Ring of Stealth");
		else if (id==kCharmOfSwiftness)				strcat(itemName, "Charm of Swiftness");
		else if (id==kCharmOfLoyalty)				strcat(itemName, "Charm of Loyalty");
		else if (id==kCharmOfPurity)				strcat(itemName, "Charm of Purity");
		else if (id==kRingOfBruteStrength)			strcat(itemName, "Ring of Brute Strength");
		else if (id==kLesserManaRing)				strcat(itemName, "Lesser Mana Ring");
		else if (id==kRingOfShadows)				strcat(itemName, "Ring of Shadow");
		else if (id==kEssenceOfPower)				strcat(itemName, "Essence of Power");
		else if (id==kEssenceOfDurability)			strcat(itemName, "Essence of Durability");
		else if (id==kEssenceOfSpeed)				strcat(itemName, "Essence of Speed");
		else if (id==kBloodAmulet) 					strcat(itemName, "Blood Amulet");
		else if (id==kBloodStainedMoonstone) 		strcat(itemName, "Blood Stained Moonstone");
		else if (id==kBloodTome) 					strcat(itemName, "Blood Tome");
		else if (id==kCrystalNecklace) 				strcat(itemName, "Crystal Necklace");
		else if (id==kMoonstoneRing) 				strcat(itemName, "Moonstone Ring");
		else if (id==kJournal) 						strcat(itemName, "Journal");
		else if (id==kTornNote) 					strcat(itemName, "Torn Note");
		else if (id==kCrystalChalice) 				strcat(itemName, "Crystal Chalice");
		else if (id==kBloodyJournal) 				strcat(itemName, "Bloody Journal");
		else if (id==kBloodyNote)			 		strcat(itemName, "Bloody Note");
		else if (id==kSkeletonKey) 					strcat(itemName, "Skeleton Key");
		else if (id==kNorthStone) 					strcat(itemName, "North Stone");
		else if (id==kSouthStone) 					strcat(itemName, "South Stone");	
		else if (id==kEastStone) 					strcat(itemName, "East Stone");
		else if (id==kWestStone) 					strcat(itemName, "West Stone");
		else if (id==kThread) 						strcat(itemName, "Thread");
		else if (id==kBead) 						strcat(itemName, "Bead");
		else if (id==kBrokenNecklace) 				strcat(itemName, "Broken Necklace"); 					
		else if (id==kRuinedBook) 					strcat(itemName, "Ruined Book");	
		else if (id==kBloodStainedCloth) 			strcat(itemName, "Blood-Stained Cloth");	
		else if (id==kMuddyBoots) 					strcat(itemName, "Muddy Boots");
		else if (id==kOrnateStaff) 					strcat(itemName, "Ornate Staff");		
		else if (id==kMoldyBook) 					strcat(itemName, "Moldy Book"); 							
		else if (id==kLogbook) 						strcat(itemName, "Logbook"); 							
		else if (id==kRegister) 					strcat(itemName, "Register");				
		else if (id==kShreddedTome) 				strcat(itemName, "Shredded Tome");			
		else if (id==kTornPage) 					strcat(itemName, "Torn Page");		
		else if (id==kGlowingBottle) 				strcat(itemName, "Glowing Bottle");		
		else if (id==kCrystalDragonStatue) 			strcat(itemName, "Crystal Dragon Statue");
		else if (id==kSilverFigurine) 				strcat(itemName, "Silver Figurine"); 					
		else if (id==kStarDiamond) 					strcat(itemName, "Star Diamond");	
		else if (id==kShadowedOrb) 					strcat(itemName, "Shadowed Orb");		
		else if (id==kSilverTippedQuill) 			strcat(itemName, "Silver Tipped Quill");	
		else if (id==kCrystalGoddessCharm) 			strcat(itemName, "Crystal Goddess Charm");	
		else if (id==kJeweledHairPin) 				strcat(itemName, "Jeweled Hair Pin");
		else if (id==kMercenaryToken) 				strcat(itemName, "Mercenary Token");
		else if (id==kCrystalToken) 				strcat(itemName, "Crystal Token");
		else if (id==kDecorativeVase) 				strcat(itemName, "Decorative Vase");
		else if (id==kWinningTicket) 				strcat(itemName, "Winning Ticket");
		else if (id==kBettingSlip) 					strcat(itemName, "Betting Slip");
		else if (id==kLeaderPin) 					strcat(itemName, "Leader Pin");
		else if (id==kOldSword) 					strcat(itemName, "Old Sword");
		else if (id==kRedWine) 						strcat(itemName, "Red Wine");
		else if (id==kWhiteWine) 					strcat(itemName, "White Wine");
		else if (id==kCopperMug) 					strcat(itemName, "Copper Mug");
		else if (id==kCopperLantern) 				strcat(itemName, "Copper Lantern");
		else if (id==kMirithPaleAle) 				strcat(itemName, "Mirith Pale Ale");
		else if (id==kMaraliWhiskey) 				strcat(itemName, "Marali Whiskey");
		else if (id==kAndrisChampagne) 				strcat(itemName, "Andris Champagne");
		else if (id==kBuckeyeBeer) 					strcat(itemName, "Buckeye Beer");
		else if (id==kLerilinCremeale) 				strcat(itemName, "Lerilin Creme Ale");
		else if (id==kRedNatesRotgutRum) 			strcat(itemName, "Red Nate's Rotgut Rum");
		else if (id==kGoldenScale) 					strcat(itemName, "Golden Scale");
		else if (id==kWoodenTrinket) 				strcat(itemName, "Wooden Trinket");
		else if (id==kPetrifiedGaperEye) 			strcat(itemName, "Petrified Gaper Eye");
		else if (id==kSilkPouch) 					strcat(itemName, "Silk Pouch");
		else if (id==kOrnateWalkingStick) 			strcat(itemName, "Ornate Walking Stick");
		else if (id==kGoddessCharm)					strcat(itemName, "Goddess Charm");
		else if (id==kCrystalSpiderWeb) 			strcat(itemName, "Crystal Spider Web");
		else if (id==kWoodenFlute) 					strcat(itemName, "Wooden Flute");
		else if (id==kLute) 						strcat(itemName, "Lute");
		else if (id==kSilverFlute) 					strcat(itemName, "Silver Flute");
		else if (id==kGoldLadle) 					strcat(itemName, "Gold Ladle");
		else if (id==kPipe) 						strcat(itemName, "Pipe");
		else if (id==kDarkRuby) 					strcat(itemName, "Dark Ruby");
		else if (id==kMortarandPestle) 				strcat(itemName, "Pestle and Mortar");
		else if (id==kBookofFairyTales) 			strcat(itemName, "Book of Fairy Tales");
		else if (id==kJeweledDagger) 				strcat(itemName, "Jeweled Dagger");
		else if (id==kGarlicBread) 					strcat(itemName, "Garlic Bread");
		else if (id==kSheepsMilk) 					strcat(itemName, "Sheep Milk");
		else if (id==kSteakFlambe) 					strcat(itemName, "Steak FlambŽ");
		else if (id==kSushi) 						strcat(itemName, "Chirashizushi");
		else if (id==kRedOmelette) 					strcat(itemName, "Harpie Egg Omelette");
		else if (id==kPurpleOmelette) 				strcat(itemName, "Sea Serpent Omelette");
		else if (id==kWaterBottle) 					strcat(itemName, "Bottle of Water");
		else if (id==kRumCake) 						strcat(itemName, "Rum Cake");
		else if (id==kCharStatue) 					strcat(itemName, "Statue of Charnath");
		else if (id==kBluePlateHelmet) 				strcat(itemName, "Blue Closed Plate Helmet");
		else if (id==kBluePlateBreast) 				strcat(itemName, "Blue Plate Breast");
		else if (id==kBluePlateArms) 				strcat(itemName, "Blue Plate Arms");
		else if (id==kBluePlateLegs) 				strcat(itemName, "Blue Plate Legs");
		else if (id==kBluePlateGauntlets) 			strcat(itemName, "Blue Plate Gauntlets");
		else if (id==kMushroom) 					strcat(itemName, "Mushroom");
		else if (id==kCandiedApple) 				strcat(itemName, "Candied Apple");
		else if (id==kPizza) 						strcat(itemName, "Mushroom Pizza");
		else if (id==kMead) 						strcat(itemName, "Mug of Mead");
		else if (id==kBlueMoonGin) 					strcat(itemName, "Blue Moon Gin");
		else if (id==kHayateSake) 					strcat(itemName, "Hayate's Sake");
		else if (id==kPumpkinPie) 					strcat(itemName, "Pumpkin Pie");
		else if (id==kPortGastMoonshine) 			strcat(itemName, "Port Gast's Moonshine");
		else if (id==kFoehanVodka) 					strcat(itemName, "Foehan Vodka");
		else if (id==kWizardToken) 					strcat(itemName, "Wizard Token");
		else if (id==kDruidToken) 					strcat(itemName, "Druid Token");
		else if (id==kClericToken) 					strcat(itemName, "Cleric Token");
		else if (id==kFighterToken) 				strcat(itemName, "Fighter Token");
		else if (id==kRangerToken) 					strcat(itemName, "Ranger Token");
		else if (id==kRogueToken) 					strcat(itemName, "Rogue Token");
		else if (id==kBigJackKatana) 				strcat(itemName, "Big Jack's Katana");
		else if (id==kBlackFerrite) 				strcat(itemName, "Black Ferrite");
		else if (id==kBlackIngot) 					strcat(itemName, "Black Ingot");
		else if (id==kZombieBrainStew) 				strcat(itemName, "Zombie Brain Stew");
		else if (id==kMandrakeCookie) 				strcat(itemName, "Mandrake Cookie");
		else if (id==kJinkDoll) 					strcat(itemName, "Jinker Doll");
		else if (id==kVoodooDoll) 					strcat(itemName, "Voodoo Doll");
		else if (id==kNPCPrivateerHead) 			strcat(itemName, "Privateer Head");
		else if (id==kTuna) 						strcat(itemName, "Tuna");
		else if (id==kSeaBass) 						strcat(itemName, "Sea Bass");
		else if (id==kCopperSpoon) 					strcat(itemName, "Copper Spoon");
		else if (id==kSilverButton) 				strcat(itemName, "Silver Button");
		else if (id==kTinPlate) 					strcat(itemName, "Tin Plate");
		else if (id==kSakeSeaBass) 					strcat(itemName, "Sake Sea Bass");
		else if (id==kCrystalBrooch) 				strcat(itemName, "Crystal Brooch");
		else if (id==kTornCloth) 					strcat(itemName, "Torn Cloth");
		else if (id==kBookOfBlood) 					strcat(itemName, "Book of Blood");
		else if (id==kRustySextant) 				strcat(itemName, "Rusty Sextant");
		else if (id==kAmuletOfStars) 				strcat(itemName, "Amulet of Stars");
		else if (id==kAmuletOfSky) 					strcat(itemName, "Amulet of Sky");
		else if (id==kAmuletOfTheMoon) 				strcat(itemName, "Amulet of the Moon");
		else if (id==kCeruleanDiamond) 				strcat(itemName, "Cerulean Diamond");
		else if (id==kJeweledRing) 					strcat(itemName, "Jeweled Ring");
		else if (id==kBrokenRing) 					strcat(itemName, "Broken Ring");
		else if (id==kDriftWoodFigurine) 			strcat(itemName, "Drift Wood Figurine");
		else if (id==kSilverShell) 					strcat(itemName, "Silver Sea Shell");
		else if (id==kTinySeaShell) 				strcat(itemName, "Tiny Sea Shell");
		else if (id==kReed) 						strcat(itemName, "Reed");
		else if (id==kKelp) 						strcat(itemName, "Kelp");
		else if (id==kCharmOfTheSea) 				strcat(itemName, "Charm of the Sea");
		else if (id==kOrnateShellHorn) 				strcat(itemName, "Ornate Shell Horn");
		else if (id==kBraidedKelpCharm) 			strcat(itemName, "Braided Kelp Charm");
		else if (id==kBookOfSky) 					strcat(itemName, "Book of Sky");
		else if (id==kStuffedGiantHeart) 			strcat(itemName, "Stuffed Giant Heart");
		else if (id==kEggNog) 						strcat(itemName, "Egg Nog");
		else if (id==kAndrisBananaCake) 			strcat(itemName, "Andris Banana Cake");
		else if (id==kMorgansGingerbread) 			strcat(itemName, "Morgans' Gingerbread");
		else if (id==kWizardReward) 				strcat(itemName, "Book of The Wizard");
		else if (id==kClericReward) 				strcat(itemName, "Book of The Cleric");
		else if (id==kDruidReward) 					strcat(itemName, "Book of The Druid");
		else if (id==kFighterReward) 				strcat(itemName, "Book of The Fighter");
		else if (id==kRangerReward) 				strcat(itemName, "Book of The Ranger");
		else if (id==kRogueReward) 					strcat(itemName, "Book of The Rogue");
		else if (id==kPixieDust) 					strcat(itemName, "Pixie Dust");
		else if (id==kCopperCoin) 					strcat(itemName, "Copper Coin");
		else if (id==kKingFigurine) 				strcat(itemName, "King Figurine");
		else if (id==kQueenFigurine) 				strcat(itemName, "Queen Figurine");
		else if (id==kBishopFigurine) 				strcat(itemName, "Bishop Figurine");
		else if (id==kKnightFigurine) 				strcat(itemName, "Knight Figurine");
		else if (id==kRookFigurine) 				strcat(itemName, "Rook Figurine");
		else if (id==kPawnFigurine) 				strcat(itemName, "Pawn Figurine");
		else if (id==kSilverCoin) 					strcat(itemName, "Silver Coin");
		else if (id==kRedToken) 					strcat(itemName, "Red Token");
		else if (id==kBlackToken) 					strcat(itemName, "Black Token");
		else if (id==kSilverToken) 					strcat(itemName, "Silver Token");
		else if (id==kGrainOfRedSand) 				strcat(itemName, "Grain of Red Sand");
		else if (id==kCandle) 						strcat(itemName, "Candle");
		else if (id==kBirthdayCake) 				strcat(itemName, "Birthday Cake");
		else if (id==kGohoranChocolate) 			strcat(itemName, "Gohoran Chocolate");
		else if (id==kFirstCrown) 					strcat(itemName, "Champion's Crown");
		else if (id==kSecondCrown) 					strcat(itemName, "Second Place Crown");
		else if (id==kThirdCrown) 					strcat(itemName, "Third Place Crown");
		else if (id==kSpiderEgg)					strcat(itemName, "Spider Egg Sac");
		else if (id==kBearSkin)						strcat(itemName, "Bear Skin");
		else if (id==kSharkFin)						strcat(itemName, "Shark Fin");
		else if (id==kSharkFinSoup)					strcat(itemName, "Shark Fin Soup");
		else if (id==kTrollHouseCookie)				strcat(itemName, "Troll House Cookie");
		else if (id==kGoldMedal)					strcat(itemName, "Gold Medal");
		else if (id==kSilverMedal)					strcat(itemName, "Silver Medal");
		else if (id==kBronzeMedal)					strcat(itemName, "Bronze Medal");
		else if (id==kRuneSword)					strcat(itemName, "Rune Sword");
		else if (id==kNeedle)						strcat(itemName, "Needle");
		else if (id==kSLToken)						strcat(itemName, "Silver Leaf Token");
		else if (id==kJadeNecklace)					strcat(itemName, "Jade Amulet");
		else if (id==kJadePin)						strcat(itemName, "Jade Pin");
		else if (id==kJadeStatue)					strcat(itemName, "Jade Statue");
		else if (id==kJadeBox)						strcat(itemName, "Jade Box");
		else if (id==kClenseSpell)					strcat(itemName, "Clense");	
		else if (id==kIridescentToken)				strcat(itemName, "Iridescent Token");
		else if (id==kBrigandInsignia)				strcat(itemName, "Brigand Insignia");
		else if (id==kBlueMetal)					strcat(itemName, "Blue Metal");
		else if (id==kPortraitLady)					strcat(itemName, "Portrait of a Lady");
		else if (id==kPortraitTirana)				strcat(itemName, "Portrait of Tirana");
		else if (id==kRingOfInsight)				strcat(itemName, "Ring of Insight");
		else if (id==kElixirHeart)					strcat(itemName, "Elixir of Love");  //Elixir
		else if (id==kElixirDuh)					strcat(itemName, "Elixir of Forgetfulness");  //Elixir
		else if (id==kElixirZimm)					strcat(itemName, "Elixir of Zimm");  //Elixir
		else if (id==kElixirKapow)					strcat(itemName, "Elixir of Spirit");  //Elixir
		else if (id==kElixirTingle)					strcat(itemName, "Elixir");  //Elixir
		else if (id==kTomeoftheLost)				strcat(itemName, "Tome of the Lost");  //Tome of the Lost
		
		
		else
		{
			tb_IntegerToString(id,	itemName);
			strcat(itemName," Unidentified Item");//Dave strcat(itemName," <Unknown(s)>");
		}

	}

	if (count==-2)
	{
		for (i=0;i<strlen(itemName);i++)
			if (((int)itemName[i]>=65) && ((int)itemName[i]<=90)) // make item name lower case
				itemName[i]=(char)((int)itemName[i]+32);
	}

	if (id<2000) // not lock, key, or quest
		if ((!in_CanGroup(id)) && (count>=0))
		{
			tb_IntegerToString((int)((float)count/12),	conditionStr);
			strcat(itemName," (+");
			strcat(itemName,conditionStr);
			strcat(itemName,")");
		}
}

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean in_ContainerOwner(int p)
// ----------------------------------------------------------------- //

{
	int					c;
	//int         i;
	char        s[kStringLength];
	char        name[kStringLength];

	c=containerMap[creature[player[p].creatureIndex].row][creature[player[p].creatureIndex].col];

	if (c==0)
		return (true);

	if (strcmp(containerGift[c],"")==0)
		return (true);

	if (pl_AuthorizedTo(player[p].creatureIndex, kRegularPlayer)==false)
		return (true);

	strcpy(name,player[p].playerName);  // check for player match

	//for (i=0;i<strlen(name);i++)
	//	if (name[i]=='*')
	//		name[i]=' ';
	if (strcmp(containerGift[c],name)==0)
		return (true);

	strcpy(name,player[p].guildName); // check for guild match

	//for (i=0;i<strlen(name);i++)
	//	if (name[i]=='*')
	//		name[i]=' ';
	if (strcmp(containerGift[c],name)==0)
		return (true);

	strcpy(s,"Only ");

	strcat(s,containerGift[c]);

	strcat(s," can take this item.");

	nw_SendTextToClient(p, s, 0);

	return (false);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean in_TracksOwner(int p)
// ----------------------------------------------------------------- //

{
	int					c;
	//int         i;
	//char        s[kStringLength];
	char        name[kStringLength];

	c=tracksMap[creature[player[p].creatureIndex].row][creature[player[p].creatureIndex].col];

	if (c==0)
		return (true);

	if (strcmp(tracksGift[c],"")==0)
		return (true);

	if (pl_AuthorizedTo(player[p].creatureIndex, kRegularPlayer)==false)
		return (true);

	strcpy(name,player[p].playerName);  // check for player match

	if (strcmp(tracksGift[c],name)==0)
		return (true);

	strcpy(name,player[p].guildName); // check for guild match

	if (strcmp(tracksGift[c],name)==0)
		return (true);


	return (false);

}

#endif

// ----------------------------------------------------------------- //
TBoolean in_AnythingInBankList(void)
// ----------------------------------------------------------------- //

{
	int					i;

	for (i=0;i<kMaxInventorySize;i++)
		if (bankInventory[i].itemID	!= 0)
			return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean in_AnythingInPickUpList(void)
// ----------------------------------------------------------------- //

{
	int					i;

	for (i=0;i<kMaxInventorySize;i++)
		if (pickUpInventory[i].itemID	!= 0)
			return (true);

	return (false);

}

#ifdef _SERVER
// ----------------------------------------------------------------- //
int in_WeightOf(int item, int count) // server
// ----------------------------------------------------------------- //

{

	if (in_CanGroup(item))
		return ((itemList[item].weight*count)/10);

	return (itemList[item].weight);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean in_PlayerCanCarryWeight(int i, int weight) // server
// ----------------------------------------------------------------- //

{
	int         carrying;
	int         j;

	carrying=0;

	for (j=1;j<kMaxInventorySize;j++)
		if (creature[player[i].creatureIndex].inventory[j].itemID!=0)
			if (in_CanGroup(creature[player[i].creatureIndex].inventory[j].itemID))
				carrying=carrying+itemList[creature[player[i].creatureIndex].inventory[j].itemID].weight+(itemList[creature[player[i].creatureIndex].inventory[j].itemID].weight*creature[player[i].creatureIndex].inventory[j].itemCount)/10;
			else
				carrying=carrying+itemList[creature[player[i].creatureIndex].inventory[j].itemID].weight;

	if ((carrying+weight)>(creature[player[i].creatureIndex].strength*kWeightLimit))
		return (false);

	return (true);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean in_ItemEquipedServer(int i, int item)
// ----------------------------------------------------------------- //

{

	if (player[i].arm[kRightFinger]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kRightFinger]].itemID==item)
			return (true);

	if (player[i].arm[kLeftFinger]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kLeftFinger]].itemID==item)
			return (true);

	if (player[i].arm[kTorso]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kTorso]].itemID==item)
			return (true);

	if (player[i].arm[kRightHand]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID==item)
			return (true);

	if (player[i].arm[kHead]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kHead]].itemID==item)
			return (true);

	if (player[i].arm[kLegs]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kLegs]].itemID==item)
			return (true);

	if (player[i].arm[kLeftHand]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kLeftHand]].itemID==item)
			return (true);

	if (player[i].arm[kFeet]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kFeet]].itemID==item)
			return (true);

	if (player[i].arm[kArms]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kArms]].itemID==item)
			return (true);

	if (player[i].arm[kNeck]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kNeck]].itemID==item)
			return (true);

	if (player[i].arm[kWaist]!=0)
		if (creature[player[i].creatureIndex].inventory[player[i].arm[kWaist]].itemID==item)
			return (true);

	return (false);

}

// ----------------------------------------------------------------- //
void in_SwapEquipServer(int i, int j, int k)
// ----------------------------------------------------------------- //
{
	if (player[i].arm[kRightFinger]==j)
		player[i].arm[kRightFinger]=k;

	if (player[i].arm[kLeftFinger]==j)
		player[i].arm[kLeftFinger]=k;

	if (player[i].arm[kTorso]==j)
		player[i].arm[kTorso]=k;

	if (player[i].arm[kRightHand]==j)
		player[i].arm[kRightHand]=k;

	if (player[i].arm[kHead]==j)
		player[i].arm[kHead]=k;

	if (player[i].arm[kLegs]==j)
		player[i].arm[kLegs]=k;

	if (player[i].arm[kLeftHand]==j)
		player[i].arm[kLeftHand]=k;

	if (player[i].arm[kFeet]==j)
		player[i].arm[kFeet]=k;

	if (player[i].arm[kArms]==j)
		player[i].arm[kArms]=k;

	if (player[i].arm[kNeck]==j)
		player[i].arm[kNeck]=k;

	if (player[i].arm[kWaist]==j)
		player[i].arm[kWaist]=k;
}

// ----------------------------------------------------------------- //
int in_GetEquipSlotServer(int i, int j)
// ----------------------------------------------------------------- //
{

	if (player[i].arm[kRightHand]==j)
		return(kRightHand);

	if (player[i].arm[kHead]==j)
		return(kHead);

	if (player[i].arm[kTorso]==j)
		return(kTorso);

	if (player[i].arm[kLegs]==j)
		return(kLegs);

	if (player[i].arm[kLeftHand]==j)
		return(kLeftHand);

	if (player[i].arm[kFeet]==j)
		return(kFeet);

	if (player[i].arm[kArms]==j)
		return(kArms);

	if (player[i].arm[kRightFinger]==j)
		return(kRightFinger);

	if (player[i].arm[kLeftFinger]==j)
		return(kLeftFinger);

	if (player[i].arm[kNeck]==j)
		return(kNeck);

	if (player[i].arm[kHands]==j)
		return(kHands);

	if (player[i].arm[kWaist]==j)
		return(kWaist);


	return(-1);
}

// ----------------------------------------------------------------- //
void in_EquipServer(int i, int j, int k)
// ----------------------------------------------------------------- //
{
	if (j==-1)
		return;

	player[i].arm[j]=k;
}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_DamageToolInHand(int i) // server
// ----------------------------------------------------------------- //

{
	if (player[i].arm[kRightHand]==0)
		return;

	if (in_CanGroup(creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID))
	{
		if (tb_Rnd(1,175)==1)
		{
			nw_SendToClientDropFromInventorySlot(i,kDamageItem,player[i].arm[kRightHand],1, 0);

			if (creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount>0)
				creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount--;

			if (itemList[creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID].desiredCount>0)
				in_ItemDestroyed(creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID, 1);

			if (creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount==0)
				creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID=0;

			player[i].arm[kRightHand]=0;
		}
	}
	else if (creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount>127)	// it's a GM weapon
	{
		if (tb_Rnd(0,(itemList[creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID].durability*2))==1)
		{
			nw_SendToClientDropFromInventorySlot(creature[player[i].creatureIndex].playerIndex,kDamageItem,player[i].arm[kRightHand],1, 0);

			if (creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount>0)
				creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount--;

			if (creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount<=127)
			{
				if (itemList[creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID].desiredCount>0)
					in_ItemDestroyed(creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID, 1);

				creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID=0;

				creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount=0;

				player[i].arm[kRightHand]=0;
			}

			it_CalculateArmorRating(i);
		}
	}
	else	// it's a regular weapon
	{
		if (tb_Rnd(0,itemList[creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID].durability)==1)
		{
			nw_SendToClientDropFromInventorySlot(creature[player[i].creatureIndex].playerIndex,kDamageItem,player[i].arm[kRightHand],1, 0);

			if (creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount>0)
				creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount--;

			if (creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemCount==0)
			{
				if (itemList[creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID].desiredCount>0)
					in_ItemDestroyed(creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID, 1);

				creature[player[i].creatureIndex].inventory[player[i].arm[kRightHand]].itemID=0;

				player[i].arm[kRightHand]=0;
			}

			it_CalculateArmorRating(i);
		}
	}
}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_CreaturePickUp(int c) // server
// ----------------------------------------------------------------- //

{
	int									j;
	int									moveCount;
	TBoolean						emptied=true;
	int									theContainer;
	UInt32							gold;
	UInt32							goldAdded;
	int									gold1;
	int									gold2;
	TBoolean						pickUp;

	theContainer=containerMap[creature[c].row][creature[c].col];


	if (theContainer==0)
		return;

	if (containerGM[theContainer])
		return;

	if (strcmp(containerGift[theContainer],"")!=0)
		return;


	if (containerTrap[theContainer]>0)
	{
		cr_AddSound(c, kExplosionSnd);
		fx_CreatureServerFX(c, c, kLightExplosion, (2*60), 0);

		if (creature[c].hitPoints<=containerTrap[theContainer])
		{
			creature[c].hitPoints=0;
			cr_setDieTime(c,(TickCount()+120));
			containerTrap[theContainer]=0;
			return;
		}

		creature[c].hitPoints=creature[c].hitPoints-containerTrap[theContainer];

		containerTrap[theContainer]=0;
		return;
	}

	if (containerMagicTrap[theContainer]>0)
	{
		cr_AddSound(c, kExplosionSnd);
		//creature[c].soundFX					      = kExplosionSnd;
		//creature[c].soundFXTime			      = TickCount();
		fx_CreatureServerFX(c, c, kLightExplosion, (2*60), 0);

		if (creature[c].hitPoints<=containerMagicTrap[theContainer])
		{
			creature[c].hitPoints=0;
			cr_setDieTime(c,(TickCount()+120));
			containerMagicTrap[theContainer]=0;
			return;
		}

		creature[c].hitPoints=creature[c].hitPoints-containerMagicTrap[theContainer];

		containerMagicTrap[theContainer]=0;
		return;
	}


	if (creatureInfo[creature[c].id].seekGold)
	{
		gold=in_GetGoldTotal(container[theContainer][0].itemID, container[theContainer][0].itemCount);

		if (gold>0)
		{
			goldAdded=in_AddGoldtoInventory(c, gold);

			if (goldAdded>0)
			{
				in_SetGoldTotal(goldAdded, &gold1, &gold2);
				gold=gold-goldAdded;
				in_SetGoldTotal(gold, &gold1, &gold2);
				container[theContainer][0].itemID=gold1;
				container[theContainer][0].itemCount=gold2;
			}
		}
	}

	for (j=1;j<kMaxInventorySize;j++)
		if (container[theContainer][j].itemID!=0)
		{

			pickUp=false;

			if (creatureInfo[creature[c].id].seekWeapons)
				if (it_IsWeapon(container[theContainer][j].itemID) || it_IsArmor(container[theContainer][j].itemID, false))
					pickUp=true;

			if (pickUp)
			{
				moveCount=in_AddtoInventory(c,container[theContainer][j].itemID,container[theContainer][j].itemCount);

				if (moveCount>0)
				{
					container[theContainer][j].itemCount=container[theContainer][j].itemCount-moveCount;

					if (container[theContainer][j].itemCount>0)
						emptied=false;
					else
						container[theContainer][j].itemID=0;
				}
				else
					emptied=false;
			}
			else
				emptied=false;

		}

	if (containerID[theContainer]==30)
		emptied=false;

	if ((emptied) && (!containerIsCorpse[containerID[theContainer]]))
	{
		if (containerID[theContainer]==55)
			fireDungeonTreasureToSpawn++;

		if (containerID[theContainer]==27)
			dungeonTreasureToSpawn++;

		if (containerID[theContainer]==28)
			sewerTreasureToSpawn++;

		if (containerID[theContainer]==29)
			lizardTreasureToSpawn++;

		if (containerID[theContainer]==32)
			iceTreasureToSpawn++;

		in_ClearContainer(theContainer);
	}

//	if ((emptied) && (containerID[theContainer]==1))
//		in_ClearContainer(theContainer);

}

#endif

// ----------------------------------------------------------------- //
UInt32 in_InventoryCheckSum_c(void) // client
// ----------------------------------------------------------------- //

{
	int					i;
	UInt32			sum=0;

	sum=sum+localInventory[0].itemID+localInventory[0].itemCount;

	for (i=1;i<kMaxInventorySize;i++)
	{
		sum=sum+localInventory[i].itemID;

		if (localInventory[i].itemID!=0)
			//if (in_CanGroup(localInventory[i].itemID))
			sum=sum+localInventory[i].itemCount;
	}

	return (sum);

}

#ifdef _SERVER
// ----------------------------------------------------------------- //
UInt32 in_InventoryCheckSum_s(int i) // server
// ----------------------------------------------------------------- //

{
	int					j;
	UInt32			sum=0;

	sum=sum+creature[i].inventory[0].itemID+creature[i].inventory[0].itemCount;

	for (j=1;j<kMaxInventorySize;j++)
	{
		sum=sum+creature[player[i].creatureIndex].inventory[j].itemID;

		if (creature[player[i].creatureIndex].inventory[j].itemID!=0)
			//if (in_CanGroup(creature[player[i].creatureIndex].inventory[j].itemID))
			sum=sum+creature[player[i].creatureIndex].inventory[j].itemCount;
	}

	return (sum);

}

#endif

// ----------------------------------------------------------------- //
int in_PlayerHas(int item) // client
// ----------------------------------------------------------------- //

{
	int						i;

	for (i=1;i<kMaxInventorySize;i++)
		if (localInventory[i].itemID==item)
			return (i);

	return (0);

}

// ----------------------------------------------------------------- //
int in_QuantityOf_c(int item) // client
// ----------------------------------------------------------------- //

{
	int						i;

	for (i=1;i<kMaxInventorySize;i++)
		if (localInventory[i].itemID==item)
			return (localInventory[i].itemCount);

	return (0);

}

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean in_IsItemInParty(int i, int item) // server
// ----------------------------------------------------------------- //

{
	int					j;
	int         k;
	int         m;
	TBoolean    mutual=false;

	for (j=1;j<kMaxInventorySize;j++)
		if (creature[player[i].creatureIndex].inventory[j].itemID==item)
			return (true);

	for (k=0;k<16;k++)
		if (player[i].friendID[k]!=0)
		{
			mutual=false;

			for (m=0;m<16;m++)
				if (player[player[i].friendID[k]].friendID[m]==i)
				{
					mutual=true;
					break;
				}

			if (mutual)
			{
				for (j=1;j<kMaxInventorySize;j++)
					if (creature[player[player[i].friendID[k]].creatureIndex].inventory[j].itemID==item)
						return (true);
			}
		}

	return (false);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean in_IsQuestInParty(int i, int theQuest) // server
// ----------------------------------------------------------------- //

{
	int					j;
	int         k;
	int         m;
	TBoolean    mutual=false;

	for (j=1;j<kMaxInventorySize;j++)
		if ((creature[player[i].creatureIndex].inventory[j].itemID>=4000) && (creature[player[i].creatureIndex].inventory[j].itemID<5000))
			if (quest[creature[player[i].creatureIndex].inventory[j].itemID-4000].id==theQuest)
				return (true);

	for (k=0;k<16;k++)
		if (player[i].friendID[k]!=0)
		{
			mutual=false;

			for (m=0;m<16;m++)
				if (player[player[i].friendID[k]].friendID[m]==i)
				{
					mutual=true;
					break;
				}

			if (mutual)
			{
				for (j=1;j<kMaxInventorySize;j++)
					if ((creature[player[player[i].friendID[k]].creatureIndex].inventory[j].itemID>=4000) && (creature[player[player[i].friendID[k]].creatureIndex].inventory[j].itemID<5000))
						if (quest[creature[player[player[i].friendID[k]].creatureIndex].inventory[j].itemID-4000].id==theQuest)
							return (true);
			}
		}

	return (false);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
int in_QuantityOf_s(int i, int item) // server
// ----------------------------------------------------------------- //

{
	int					j;

	for (j=1;j<kMaxInventorySize;j++)
		if (creature[i].inventory[j].itemID==item)
			return (creature[i].inventory[j].itemCount);

	return (0);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
int in_GetSlotOf_s(int i, int item) // server
// ----------------------------------------------------------------- //

{
	int					j;

	for (j=1;j<kMaxInventorySize;j++)
		if (creature[i].inventory[j].itemID==item)
			return (j);

	return (0);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
//UInt32 in_DropGold(int id, long amount, int row, int col, int decay, TBoolean dead, char name[15]) // server
UInt32 in_DropGold(int id, long amount, int row, int col, int decay, TBoolean dead, char name[kStringLength]) // server
// ----------------------------------------------------------------- //

{
	int							inContainer=containerMap[row][col];
	int							i;
	UInt32					total;
	int							gold1;
	int							gold2;
	TBoolean        permanentChest=false;

	//if ((row<0) || (row>=kMapWidth) || (col<0) || (col>=kMapHeight))			// beta debug
	//	fi_WriteToErrorLog("Out of Range - in_DropGold");

	if ((row<0) || (row>=kMapWidth) || (col<0) || (col>=kMapHeight))
	{
		fi_WriteToErrorLog("Out of Range - in_DropGold");
		return (0);
	}

	if (amount<=0)
		return (0);

	if (id==30)
		permanentChest=true;

	if (!dead)
		if ((map[row][col]>=747) && (map[row][col]<=750))	// can't drop in bank
			return (0);

	if (doorway[map[row][col]])	// can't drop in doorway
		return (0);

	if (inContainer!=0) // clear gm prop
		if (gmProp[containerID[inContainer]])
		{
			in_ClearContainer(inContainer);
			inContainer=0;
		}

	if (inContainer!=0) // destroyed in fire
		if (containerID[inContainer]==8)
			return (amount);

	if (inContainer!=0)
		if (containerID[inContainer]==30)
			permanentChest=true;

	if (inContainer!=0)
		if (containerIsTreasure[containerID[inContainer]])
		{
			if (containerIsCorpse[id])
			{
				if (containerID[inContainer]==55)
					fireDungeonTreasureToSpawn++;

				if (containerID[inContainer]==27)
					dungeonTreasureToSpawn++;

				if (containerID[inContainer]==28)
					sewerTreasureToSpawn++;

				if (containerID[inContainer]==29)
					lizardTreasureToSpawn++;

				if (containerID[inContainer]==32)
					iceTreasureToSpawn++;

				in_ClearContainer(inContainer);

				inContainer=0;
			}
		}

	if (inContainer==0)
	{
		if (currentNumberOfContainers>=(kMaxContainers-1)) // no empty containers
			return (0);

		for (i=currentNumberOfContainers;i<kMaxContainers;i++)
			if (containerEmpty[i])
			{
				inContainer=i;
				containerLock[inContainer]=0;
				containerTrap[inContainer]=0;
				containerMagicLock[inContainer]=0;
				containerMagicTrap[inContainer]=0;

				if (strlen(name)<48)
					strcpy(containerName[inContainer],name);

				break;
			}
	}

	if (inContainer==0)
		for (i=1;i<currentNumberOfContainers;i++)
			if (containerEmpty[i])
			{
				inContainer=i;
				containerLock[inContainer]=0;
				containerTrap[inContainer]=0;
				containerMagicLock[inContainer]=0;
				containerMagicTrap[inContainer]=0;

				if (strlen(name)<48)
					strcpy(containerName[inContainer],name);

				break;
			}

	if (inContainer==0)	// no empty containers
		return (0);

	//if ((containerID[inContainer]==0) || (containerID[inContainer]==1))
	if ((!containerIsCorpse[containerID[inContainer]]) && (permanentChest==false) && (containerGM[inContainer]==false))
		containerID[inContainer]=id;

	containerMap[row][col]		=	inContainer;

	if (strlen(name)<48)
		strcpy(containerName[inContainer],name);
	else
		strcpy(containerName[inContainer],"");

	total=in_GetGoldTotal(container[inContainer][0].itemID, container[inContainer][0].itemCount);

	if ((total+amount)>kMaxGold)
		amount=kMaxGold-total;

	total=total+amount;

	in_SetGoldTotal(total, &gold1, &gold2);

	container[inContainer][0].itemID		= gold1;

	container[inContainer][0].itemCount	= gold2;

	if (decay==kRandomDecay)
		decay=tb_Rnd(10,30);

	if (decay==0)
		containerDecayTime	[inContainer]	=	0;
	else
		containerDecayTime	[inContainer]	=	TickCount()+(decay*3600);

	if (containerEmpty[inContainer])
		currentNumberOfContainers++;

	containerEmpty			[inContainer]	=	false;

	containerRow				[inContainer]	= row;

	containerCol				[inContainer]	= col;

	if ((!((containerIsCorpse[containerID[inContainer]]) || containerIsTreasure[containerID[inContainer]])) && (permanentChest==false) && (containerGM[inContainer]==false))
		containerID[inContainer]=1;

	//for (i=1;i<kMaxInventorySize;i++)
	//	if (container[inContainer][i].itemID!=0)
	//		{
	//			containerID[inContainer]=1;
	//			break;
	//		}

	if (permanentChest)
		containerID[inContainer]=30;

	return (amount);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
UInt32 in_BankGold(int i, long amount) // server
// ----------------------------------------------------------------- //

{
	UInt32					total;
	int							gold1;
	int							gold2;


	total=in_GetGoldTotal(player[i].bank[0].itemID, player[i].bank[0].itemCount);

	if ((total+amount)>kMaxGold)
		amount=kMaxGold-total;

	total=total+amount;

	in_SetGoldTotal(total, &gold1, &gold2);

	player[i].bank[0].itemID		= gold1;

	player[i].bank[0].itemCount	= gold2;

	return (amount);

}

#endif

// ----------------------------------------------------------------- //
UInt32 in_GetGoldTotal(int gold1, int gold2)
// ----------------------------------------------------------------- //

{
	int					byte1=gold2;
	int					byte2=0;
	int					byte3=0;
	long				total;

	if (gold1>=32768)			{	byte3=byte3+128;		gold1=gold1-32768; }

	if (gold1>=16384)			{	byte3=byte3+64;			gold1=gold1-16384; }

	if (gold1>=8192)			{	byte3=byte3+32;			gold1=gold1-8192;	}

	if (gold1>=4096)			{	byte3=byte3+16;			gold1=gold1-4096;	}

	if (gold1>=2048)			{	byte3=byte3+8;			gold1=gold1-2048;	}

	if (gold1>=1024)			{	byte3=byte3+4;			gold1=gold1-1024;	}

	if (gold1>=512)				{	byte3=byte3+2;			gold1=gold1-512;	}

	if (gold1>=256)				{	byte3=byte3+1;			gold1=gold1-256;	}

	if (gold1>=128)				{	byte2=byte2+128;		gold1=gold1-128;	}

	if (gold1>=64)				{	byte2=byte2+64;			gold1=gold1-64;	}

	if (gold1>=32)				{	byte2=byte2+32;			gold1=gold1-32;	}

	if (gold1>=16)				{	byte2=byte2+16;			gold1=gold1-16;	}

	if (gold1>=8)					{	byte2=byte2+8;			gold1=gold1-8;	}

	if (gold1>=4)					{	byte2=byte2+4;			gold1=gold1-4;	}

	if (gold1>=2)					{	byte2=byte2+2;			gold1=gold1-2;	}

	if (gold1>=1)					{	byte2=byte2+1;	}

	total=byte1+(byte2*256)+(byte3*65536);

	return (total);

}

// ----------------------------------------------------------------- //
void in_SetGoldTotal(UInt32 total, int *gold1, int *gold2)
// ----------------------------------------------------------------- //

{
	int					byte1=0;
	int					byte2=0;
	int					byte3=0;

	if (total>kMaxGold)
		total=kMaxGold;

	if (total>65535)
	{
		byte3=(total/65536);
		total=total-(byte3*65536);
	}

	if (total>255)
	{
		byte2=(total/256);
		total=total-(byte2*256);
	}

	byte1=total;

	*gold1	=	(byte3*256)+byte2;
	*gold2	=	byte1;

}

// ----------------------------------------------------------------- //
void in_UnEquipItem(int i)
// ----------------------------------------------------------------- //

{

	int		j, cpt;

//	if (nw_CanSend())
//		{

	if (localRightHand==i)
	{
		localRightHand=0;
		nw_SendArm(kRightHand,0);
	}

	if (localLeftHand==i)
	{
		localLeftHand=0;
		nw_SendArm(kLeftHand,0);
	}

	if (localLegs==i)
	{
		localLegs=0;
		nw_SendArm(kLegs,0);
	}

	if (localTorso==i)
	{
		localTorso=0;
		nw_SendArm(kTorso,0);
	}

	if (localFeet==i)
	{
		localFeet=0;
		nw_SendArm(kFeet,0);
	}

	if (localArms==i)
	{
		localArms=0;
		nw_SendArm(kArms,0);
	}

	if (localRightFinger==i)
	{
		localRightFinger=0;
		nw_SendArm(kRightFinger,0);
	}

	if (localLeftFinger==i)
	{
		localLeftFinger=0;
		nw_SendArm(kLeftFinger,0);
	}

	if (localHead==i)
	{
		localHead=0;
		nw_SendArm(kHead,0);
	}

	if (localNeck==i)
	{
		localNeck=0;
		nw_SendArm(kNeck,0);
	}

	if (localHands==i)
	{
		localHands=0;
		nw_SendArm(kHands,0);
	}

	if (localWaist==i)
	{
		localWaist=0;
		nw_SendArm(kWaist,0);
	}
	
	if (localMount==i)
	{
		localMount=0;
		nw_SendArm(kMount,0);
	}

	gr_RenderPlayerDisplay(true);

	gr_RenderCharacterDisplayWindow();

	if (itemList[localInventory[i].itemID].spell!=0)  //Redraw skills window + close make window
	{
		cpt=0;

		for (j=0; j<kMaxSkills; j++)
		{
			if (localClass>6 || maxSkillLevel[localClass][j]>0 || questChar[90])
				cpt++;
			else if (localRightFinger !=0 && itemList[itemList[localInventory[localRightFinger].itemID].spell].makeSkill==j)
				cpt++;
			else if (localLeftFinger != 0 && itemList[itemList[localInventory[localLeftFinger].itemID].spell].makeSkill==j)
				cpt++;
		}

		tb_SetRect(&skillsWindowRect,0,0,152,42+(13.2*cpt));

		SetWindowBounds(skillsWindow, kWindowContentRgn, &skillsWindowRect);

		MoveWindowStructure(skillsWindow,windowPosition[kSkillsWindow].h,windowPosition[kSkillsWindow].v);

		gr_RenderSkillsWindow();

		if (localClass<=6 && maxSkillLevel[localClass][itemList[itemList[localInventory[i].itemID].spell].makeSkill]==0)
			gr_CloseFloatingWindowID(kMakeWindow);
	}

//		}

}

// ----------------------------------------------------------------- //
int in_PlayerHasItemType(int theType) // client
// ----------------------------------------------------------------- //

{
	int						i;

	for (i=1;i<kMaxInventorySize;i++)
		if (localInventory[i].itemID!=0)
			if (itemList[localInventory[i].itemID].itemType==theType)
				return (i);

	return (0);

}

// ----------------------------------------------------------------- //
TBoolean in_ItemTypeEquiped(int theType)
// ----------------------------------------------------------------- //

{

	if (theType==kEmptyHands)
	{
		if (localRightHand!=0)
			return (false);

		if (localLeftHand!=0)
			return (false);

		return (true);
	}

	if (localHead!=0)
		if (itemList[localInventory[localHead].itemID].itemType==theType)
			return (true);

	if (localNeck!=0)
		if (itemList[localInventory[localNeck].itemID].itemType==theType)
			return (true);

	if (localHands!=0)
		if (itemList[localInventory[localHands].itemID].itemType==theType)
			return (true);

	if (localWaist!=0)
		if (itemList[localInventory[localWaist].itemID].itemType==theType)
			return (true);

	if (localRightHand!=0)
		if (itemList[localInventory[localRightHand].itemID].itemType==theType)
			return (true);

	if (localLeftHand!=0)
		if (itemList[localInventory[localLeftHand].itemID].itemType==theType)
			return (true);

	if (localTorso!=0)
		if (itemList[localInventory[localTorso].itemID].itemType==theType)
			return (true);

	if (localLegs!=0)
		if (itemList[localInventory[localLegs].itemID].itemType==theType)
			return (true);

	if (localFeet!=0)
		if (itemList[localInventory[localFeet].itemID].itemType==theType)
			return (true);

	if (localRightFinger!=0)
		if (itemList[localInventory[localRightFinger].itemID].itemType==theType)
			return (true);

	if (localLeftFinger!=0)
		if (itemList[localInventory[localLeftFinger].itemID].itemType==theType)
			return (true);
		
	if (localMount!=0)
		if (itemList[localInventory[localMount].itemID].itemType==theType)
			return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean in_ItemEquiped(int i)
// ----------------------------------------------------------------- //

{

	if (localHead==i)
		return (true);

	if (localNeck==i)
		return (true);

	if (localHands==i)
		return (true);

	if (localWaist==i)
		return (true);

	if (localRightHand==i)
		return (true);

	if (localLeftHand==i)
		return (true);

	if (localLegs==i)
		return (true);

	if (localTorso==i)
		return (true);

	if (localFeet==i)
		return (true);

	if (localRightFinger==i)
		return (true);

	if (localLeftFinger==i)
		return (true);

	if (localArms==i)
		return (true);
	
	if (localMount==i)
		return (true);

	return (false);

}

// ----------------------------------------------------------------- //
int in_GetEquipSlot(int i)
// ----------------------------------------------------------------- //
{
	if (localHead==i)
		return (kHead);

	if (localNeck==i)
		return (kNeck);

	if (localHands==i)
		return (kHands);

	if (localWaist==i)
		return (kWaist);

	if (localRightHand==i)
		return (kRightHand);

	if (localLeftHand==i)
		return (kLeftHand);

	if (localLegs==i)
		return (kLegs);

	if (localTorso==i)
		return (kTorso);

	if (localFeet==i)
		return (kFeet);

	if (localRightFinger==i)
		return (kRightFinger);

	if (localLeftFinger==i)
		return(kLeftFinger);

	if (localArms==i)
		return(kArms);
	
	if (localMount==i)
		return(kMount);
	
	return (-1);
}

// ----------------------------------------------------------------- //
void in_EquipLocal(int i, int j)
// ----------------------------------------------------------------- //
{
	if (i==-1)
		return;

	switch (i)
	{
		case kHead:
			localHead=j;
			break;

		case kNeck:
			localNeck=j;
			break;

		case kHands:
			localHands=j;
			break;

		case kWaist:
			localWaist=j;
			break;

		case kRightHand:
			localRightHand=j;
			break;

		case kLeftHand:
			localLeftHand=j;
			break;

		case kLegs:
			localLegs=j;
			break;

		case kTorso:
			localTorso=j;
			break;

		case kFeet:
			localFeet=j;
			break;

		case kRightFinger:
			localRightFinger=j;
			break;

		case kLeftFinger:
			localLeftFinger=j;
			break;

		case kArms:
			localArms=j;
			break;
			
		case kMount:
			localMount=j;
			break;
	}
}

// ----------------------------------------------------------------- //
void in_EquipSwap(int i, int j)
// ----------------------------------------------------------------- //
{
	if (localHead==i)
		localHead=j;

	if (localNeck==i)
		localNeck=j;

	if (localHands==i)
		localHands=j;

	if (localWaist==i)
		localWaist=j;

	if (localRightHand==i)
		localRightHand=j;

	if (localLeftHand==i)
		localLeftHand=j;

	if (localLegs==i)
		localLegs=j;

	if (localTorso==i)
		localTorso=j;

	if (localFeet==i)
		localFeet=j;

	if (localRightFinger==i)
		localRightFinger=j;

	if (localLeftFinger==i)
		localLeftFinger=j;

	if (localArms==i)
		localArms=j;
	
	if (localMount==i)
		localMount=j;
}

// ----------------------------------------------------------------- //
void in_UseItem(int i)
// ----------------------------------------------------------------- //

{

	int		j, cpt;
	char	temp[kStringLength], navStr[kStringLength];

	//if (nw_CanSend())
	//	{

	if (itemList[localInventory[i].itemID].itemType==kQuestType)
	{
		currentQuestSlot=useSlot;
		qu_GetQuestDescription(localInventory[i].itemID);
		return;
	}

	if (itemList[localInventory[i].itemID].useLocation==kRightHand)	// armable in right hand
	{
		if (itemList[localInventory[i].itemID].twoHandItem)	// disarm left hand for two handed item
			if (localLeftHand!=0)
			{
				localLeftHand=0;
				//nw_SendArm(kLeftHand,0);
			}

		localRightHand=i;

		nw_SendArm(kRightHand,i);
	}

	if (itemList[localInventory[i].itemID].useLocation==kLeftHand)	// armable in left hand
	{
		if (localRightHand!=0)
			if (itemList[localInventory[localRightHand].itemID].twoHandItem)	// remove two handed item
			{
				localRightHand=0;
				//nw_SendArm(kRightHand,0);
			}

		localLeftHand=i;

		nw_SendArm(kLeftHand,i);
	}

	if (itemList[localInventory[i].itemID].useLocation==kLegs)	// armable on legs
	{
		if (localTorso!=0)
			if ((itemList[localInventory[localTorso].itemID].itemType==kMagicRobeType) || (itemList[localInventory[localTorso].itemID].itemType==kRobeType) || (itemList[localInventory[localTorso].itemID].itemType==kGMRobeType) || (itemList[localInventory[localTorso].itemID].itemType==kRoyalRobeType)) 	// remove robe
			{
				localTorso=0;
				//nw_SendArm(kTorso,0);
			}

		localLegs=i;

		nw_SendArm(kLegs,i);
	}

	if (itemList[localInventory[i].itemID].useLocation==kTorso)	// armable on torso
	{
		if ((itemList[localInventory[i].itemID].itemType==kMagicRobeType) || (itemList[localInventory[i].itemID].itemType==kRobeType) || (itemList[localInventory[i].itemID].itemType==kGMRobeType) || (itemList[localInventory[i].itemID].itemType==kRoyalRobeType))
			localLegs=0;

		localTorso=i;

		nw_SendArm(kTorso,i);
	}

	if (itemList[localInventory[i].itemID].useLocation==kFeet)	// armable on feet
	{
		localFeet=i;
		nw_SendArm(kFeet,i);
	}

	if (itemList[localInventory[i].itemID].useLocation==kRightFinger)	// armable on right finger
	{
		if (localRightFinger==0)
		{
			localRightFinger=i;
			nw_SendArm(kRightFinger,i);
		}
		else if (localLeftFinger==0)
		{
			localLeftFinger=i;
			nw_SendArm(kLeftFinger,i);
		}
		else
		{
			localRightFinger=i;
			nw_SendArm(kRightFinger,i);
		}
	}

	if (itemList[localInventory[i].itemID].useLocation==kArms)	// armable on arms
	{
		localArms=i;
		nw_SendArm(kArms,i);
	}

	if (itemList[localInventory[i].itemID].useLocation==kHead)	// armable on head
	{
		localHead=i;
		nw_SendArm(kHead,i);
	}

	if (itemList[localInventory[i].itemID].useLocation==kNeck)	// armable on neck
	{
		localNeck=i;
		nw_SendArm(kNeck,i);
	}

	if (itemList[localInventory[i].itemID].useLocation==kHands)	// armable on shoulders
	{
		localHands=i;
		nw_SendArm(kHands,i);
	}

	if (itemList[localInventory[i].itemID].useLocation==kWaist)	// armable on waist
	{
		localWaist=i;
		nw_SendArm(kWaist,i);
	}
	
	if (itemList[localInventory[i].itemID].useLocation==kMount)	// mount
	{
		localMount=i;
		nw_SendArm(kMount,i);
	}

	if (localInventory[i].itemID==kOrbOfSeeing)
	{
		gr_AddText("Who do you want to locate?",false,true);
		playerAction=kLocate;
		strcpy(playerText,"");
	}

	if (localInventory[i].itemID==kSextant)
	{
		if ((playerRow>1842) || (playerCol>1842))
			gr_AddText("You are unable to determine your position.",false,true);
		else
		{
			strcpy(navStr,"");
			tb_IntegerToString((playerRow),temp);
			strcat(navStr,temp);
			strcat(navStr," N by ");
			tb_IntegerToString((playerCol),temp);
			strcat(navStr,temp);
			strcat(navStr," E");
			gr_AddText(navStr,false,true);
		}
	}

/*
//Skill rewards
	//int c1;
	
	//c1=player[p].creatureIndex;
	else if (itemList[localInventory[i].itemID].itemType==kClericReward)	// Cleric
		if (localClass==1)
		if (player[i].characterClass==1) 
			{
			//sk_RaiseSkill(i,0,kHealing,10);
			//sk_RaiseSkill(i,0,kMageryCleric,10);
			//sk_RaiseSkill(i,0,kAlchemyCleric,10);
			sk_TrainProof(i, 0, 0, kMageryCleric, 10);
			gr_AddText("You feel more experienced.",true,true);
			}
		else
			{
			gr_AddText("You must be a cleric to read this book.", true,true);
			}
	else if (itemList[localInventory[i].itemID].itemType==kWizardReward)	// Wizard
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
	else if (itemList[localInventory[i].itemID].itemType==kDruidReward)	// Druid
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
	else if (itemList[localInventory[i].itemID].itemType==kFighterReward)	// Fighter
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
	else if (itemList[localInventory[i].itemID].itemType==kRangerReward)	// Ranger
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
	else if (itemList[localInventory[i].itemID].itemType==kRogueReward)	// Rogue
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
sk_RaiseSkill(i,0,skill,itemList[item].makeSkillLevel);
		return (true);


*/

//Dave^
	else if ((itemList[localInventory[i].itemID].itemType==kDyeType)	|| (itemList[localInventory[i].itemID].itemType==kCookedMeatType)	|| (itemList[localInventory[i].itemID].itemType==kCookedVegType))	      // --------- eat food
		in_ConsumeInventorySlot(i);
	else if (itemList[localInventory[i].itemID].itemType==kBeverageType)		// --------- drink beverage
		in_ConsumeInventorySlot(i);
	
//Dave
	else if (itemList[localInventory[i].itemID].itemType==kBeverageType2)		// --------- drink beverage
		in_ConsumeInventorySlot(i);
	
	else if (itemList[localInventory[i].itemID].itemType==kBeverageType3)		// --------- drink water
		in_ConsumeInventorySlot(i);
	//Dave
	else if (itemList[localInventory[i].itemID].itemType==kBeverageType4)		// --------- buckeye beer
		in_ConsumeInventorySlot(i);
	//Dave add 11-25-2012 kElixirType
	else if (itemList[localInventory[i].itemID].itemType==kElixirType1)		// --------- Elixir 
		in_ConsumeInventorySlot(i);
	else if (itemList[localInventory[i].itemID].itemType==kElixirType2)		// ---------Elixir
		in_ConsumeInventorySlot(i);
	else if (itemList[localInventory[i].itemID].itemType==kElixirType3)		// --------Elixir
		in_ConsumeInventorySlot(i);
	else if (itemList[localInventory[i].itemID].itemType==kElixirType4)		// --------- Elixir
		in_ConsumeInventorySlot(i);
	else if (itemList[localInventory[i].itemID].itemType==kElixirType5)		// --------- Elixir
		in_ConsumeInventorySlot(i);
	
	else if (itemList[localInventory[i].itemID].itemType==kPotionType)			// --------- drink potion
	{
		in_ConsumeInventorySlot(i);
	}
	else
	{
		gr_RenderPlayerDisplay(true);
		gr_RenderCharacterDisplayWindow();

		if (itemList[localInventory[i].itemID].spell!=0)  //Redraw skills window
		{
			cpt=0;

			for (j=0; j<kMaxSkills; j++)
			{
				if (localClass>6 || maxSkillLevel[localClass][j]>0 || questChar[90])
					cpt++;
				else if (localRightFinger !=0 && itemList[itemList[localInventory[localRightFinger].itemID].spell].makeSkill==j)
					cpt++;
				else if (localLeftFinger != 0 && itemList[itemList[localInventory[localLeftFinger].itemID].spell].makeSkill==j)
					cpt++;
			}

			tb_SetRect(&skillsWindowRect,0,0,152,42+(13.2*cpt));

			SetWindowBounds(skillsWindow, kWindowContentRgn, &skillsWindowRect);

			MoveWindowStructure(skillsWindow,windowPosition[kSkillsWindow].h,windowPosition[kSkillsWindow].v);

			gr_RenderSkillsWindow();
		}

	}

	//	}
}

#ifdef _SERVER
// ----------------------------------------------------------------- //
int in_AddtoInventory(int i, int item, int count) // server
// ----------------------------------------------------------------- //

{
	int						j;
	int						maxCount;
	int						realItem; //Etherite

	if ((item==0) || (count==0) || (i==0))
		return (0);

	if (itemList[item].itemType==kPotionType)
		maxCount=kMaxPotionInventoryItemCount;
	else
		maxCount=kMaxInventoryItemCount;

	if (creature[i].playerIndex!=0)
		if (player[creature[i].playerIndex].dead)
			return (0);

	if (in_CanGroup(item)==false) //018
		if (count>255)
			count=255;
		
	if (item==kEtheriteWood || item==kEtheriteFerrite)
	{
		realItem=kEtherite;
		if (sk_GetSkillLevel(player[i].skill[kEnchanting].level)<3)			count=tb_Rnd(8,12);
		else if (sk_GetSkillLevel(player[i].skill[kEnchanting].level)<6)	count=tb_Rnd(12,17);
		else if (sk_GetSkillLevel(player[i].skill[kEnchanting].level)<8)	count=tb_Rnd(17,22);
		else																count=tb_Rnd(22,30);
	}
			
	//Dave
	else if (item==kBlackEtherite) 
	{
		realItem=kEtherite;
		if (sk_GetSkillLevel(player[i].skill[kEnchanting].level)<3)			count=tb_Rnd(12,18);
		else if (sk_GetSkillLevel(player[i].skill[kEnchanting].level)<6)	count=tb_Rnd(18,25);
		else if (sk_GetSkillLevel(player[i].skill[kEnchanting].level)<8)	count=tb_Rnd(25,32);
		else																count=tb_Rnd(32,50);
	}
	else
	{
		realItem=item;
	}

	if (in_CanGroup(realItem))
		for (j=1;j<kMaxInventorySize;j++)
			if (creature[i].inventory[j].itemID==realItem)
			{
				int numberAdded=count;

				if (creature[i].inventory[j].itemCount+numberAdded>maxCount)
					numberAdded=maxCount-creature[i].inventory[j].itemCount;

				creature[i].inventory[j].itemCount=creature[i].inventory[j].itemCount+numberAdded;

				return (numberAdded);
			}

	if (count>maxCount)
	{
		count=maxCount;
		//fi_WriteToErrorLog("Add to Inv Out of Range");	// beta debug
	}

	for (j=1;j<kMaxInventorySize;j++)
		if (creature[i].inventory[j].itemID==0)
		{
			creature[i].inventory[j].itemID			=	realItem;
			creature[i].inventory[j].itemCount	=	count;
			
			return (count);
		}

	return (0);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_RemoveFromInventorySlot(int i, int slot, int count) // server
// ----------------------------------------------------------------- //

{

	if (slot>=kMaxInventorySize)
		return;

	if ((creature[i].inventory[slot].itemID==0) || (creature[i].inventory[slot].itemCount==0))  //018
		return;

	if (in_CanGroup(creature[i].inventory[slot].itemID))
	{
		if (creature[i].inventory[slot].itemCount<count)
			creature[i].inventory[slot].itemCount=0;
		else
			creature[i].inventory[slot].itemCount=creature[i].inventory[slot].itemCount-count;

		if (creature[i].inventory[slot].itemCount==0)
			creature[i].inventory[slot].itemID=0;
	}
	else
	{
		creature[i].inventory[slot].itemID		=	0;
		creature[i].inventory[slot].itemCount	=	0;
	}

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
UInt32 in_AddGoldtoInventory(int i, UInt32 gold) // server
// ----------------------------------------------------------------- //

{
	int						gold1;
	int						gold2;
	UInt32				added=gold;

	if (i==0)
		return (0);

	if (creature[i].playerIndex!=0)
		if (player[creature[i].playerIndex].dead)
			return (0);

	if (creature[i].id==0)
	{
		fi_WriteToErrorLog("Uninitialized creature in in_AddGoldtoInventory");
		return (0);
	}

	if ((gold+in_GetGoldTotal(creature[i].inventory[0].itemID, creature[i].inventory[0].itemCount))>kMaxGold)
		added=kMaxGold-in_GetGoldTotal(creature[i].inventory[0].itemID, creature[i].inventory[0].itemCount);

	gold=added+in_GetGoldTotal(creature[i].inventory[0].itemID, creature[i].inventory[0].itemCount);

	in_SetGoldTotal(gold, &gold1, &gold2);

	creature[i].inventory[0].itemID		=gold1;

	creature[i].inventory[0].itemCount=gold2;

	return (added);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_PickUp(int i) // server
// ----------------------------------------------------------------- //

{
	int									j;
	int									moveCount;
	TBoolean						emptied=true;
	int									theContainer;
	UInt32							gold;
	UInt32							goldAdded;
	int									gold1;
	int									gold2;
	TBoolean            addedItems=false;


	if (player[i].creatureIndex==0)
	{
		fi_WriteToErrorLog("Zero creature index in in_PickUp");
		return;
	}

	if (creature[player[i].creatureIndex].row<0)
	{
		fi_WriteToErrorLog("row<0 in in_PickUp");
		return;
	}

	if (creature[player[i].creatureIndex].row>=kMapWidth)
	{
		fi_WriteToErrorLog("row>kMapWidth in in_PickUp");
		return;
	}

	if (creature[player[i].creatureIndex].col<0)
	{
		fi_WriteToErrorLog("col<0 in in_PickUp");
		return;
	}

	if (creature[player[i].creatureIndex].col>=kMapHeight)
	{
		fi_WriteToErrorLog("col>kMapWidth in in_PickUp");
		return;
	}

	theContainer=containerMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col];

	if (theContainer==0)
		return;

	if ((containerTrap[theContainer]>0) && (strcmp(containerTrapName[theContainer],player[i].playerNameLC)!=0))
	{
		cr_AddSound(player[i].creatureIndex, kExplosionSnd);
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kLightExplosion, (2*60), 0);

		if (creature[player[i].creatureIndex].hitPoints<=(containerTrap[theContainer]*5))
		{
			creature[player[i].creatureIndex].hitPoints=0;
			cr_setDieTime(player[i].creatureIndex,(TickCount()+120));
			containerTrap[theContainer]=0;
			player[i].sendStatsRefresh=true;
			return;
		}

		creature[player[i].creatureIndex].hitPoints=creature[player[i].creatureIndex].hitPoints-(containerTrap[theContainer]*5);

		containerTrap[theContainer]=0;
		player[i].sendStatsRefresh=true;
		return;
	}

	if ((containerMagicTrap[theContainer]>0) && (strcmp(containerMagicTrapName[theContainer],player[i].playerNameLC)!=0))
	{
		cr_AddSound(player[i].creatureIndex, kExplosionSnd);
		fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kLightExplosion, (2*60), 0);

		if (creature[player[i].creatureIndex].hitPoints<=(containerMagicTrap[theContainer]*5))
		{
			creature[player[i].creatureIndex].hitPoints=0;
			cr_setDieTime(player[i].creatureIndex,(TickCount()+120));
			containerMagicTrap[theContainer]=0;
			player[i].sendStatsRefresh=true;
			return;
		}

		creature[player[i].creatureIndex].hitPoints=creature[player[i].creatureIndex].hitPoints-(containerMagicTrap[theContainer]*5);

		containerMagicTrap[theContainer]=0;
		player[i].sendStatsRefresh=true;
		return;
	}

	gold=in_GetGoldTotal(container[theContainer][0].itemID, container[theContainer][0].itemCount);

	if (gold>0)
	{
		goldAdded=in_AddGoldtoInventory(player[i].creatureIndex, gold);

		if (goldAdded>0)
		{
			in_SetGoldTotal(goldAdded, &gold1, &gold2);
			nw_SendGoldAdd(i, kTakeItem, 0, gold1, gold2);	// version 0.1.2
			addedItems=true;
			gold=gold-goldAdded;
			in_SetGoldTotal(gold, &gold1, &gold2);
			container[theContainer][0].itemID=gold1;
			container[theContainer][0].itemCount=gold2;
		}
	}

	for (j=1;j<kMaxInventorySize;j++)
		if (container[theContainer][j].itemID!=0)
		{
			moveCount=in_AddtoInventory(player[i].creatureIndex,container[theContainer][j].itemID,container[theContainer][j].itemCount);

			if (moveCount>0)
			{
				nw_SendInventoryAdd(i, container[theContainer][j].itemID, moveCount,kTakeItem,0);	// version 0.1.2
				addedItems=true;
				container[theContainer][j].itemCount=container[theContainer][j].itemCount-moveCount;

				if (container[theContainer][j].itemCount>0)
					emptied=false;
				else
					container[theContainer][j].itemID=0;
			}
			else
				emptied=false;
		}

	if (containerID[theContainer]==30)
		emptied=false;

	if ((emptied) && (!containerIsCorpse[containerID[theContainer]]))
	{
		if (containerID[theContainer]==55)
			fireDungeonTreasureToSpawn++;

		if (containerID[theContainer]==27)
			dungeonTreasureToSpawn++;

		if (containerID[theContainer]==28)
			sewerTreasureToSpawn++;

		if (containerID[theContainer]==29)
			lizardTreasureToSpawn++;

		if (containerID[theContainer]==32)
			iceTreasureToSpawn++;

		in_ClearContainer(theContainer);
	}
	else if ((emptied) && (containerGM[theContainer]))
	{
		containerDecayTime[theContainer]=TickCount()+(1*3600);
	}

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean in_ContainerLocked(int c) // server
// ----------------------------------------------------------------- //

{
	int					theContainer;
	//int         j;


	theContainer=containerMap[creature[c].row][creature[c].col];

	if (theContainer==0)
		return (false);

	if (containerLock[theContainer]>0)
		return (true);

	if (containerMagicLock[theContainer]>0)
		return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean in_ContainerTrapped(int c) // server
// ----------------------------------------------------------------- //

{
	int					theContainer;
	//int         j;


	theContainer=containerMap[creature[c].row][creature[c].col];

	if (theContainer==0)
		return (false);

	if (containerTrap[theContainer]>0)
		return (true);

	if (containerMagicTrap[theContainer]>0)
		return (true);

	return (false);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_PickUpFromList(int i, int item, int count) // server
// ----------------------------------------------------------------- //

{
	int									j;
	int									moveCount;
	TBoolean						emptied=true;
	int									theContainer;
	UInt32							gold;
	UInt32							goldAdded;
	int									gold1;
	int									gold2;

	if (player[i].creatureIndex==0)
	{
		fi_WriteToErrorLog("Zero creature index in in_PickUpFromList");
		return;
	}

	if (creature[player[i].creatureIndex].row<0)
	{
		fi_WriteToErrorLog("row<0 in in_PickUpFromList");
		return;
	}

	if (creature[player[i].creatureIndex].row>=kMapWidth)
	{
		fi_WriteToErrorLog("row>kMapWidth in in_PickUpFromList");
		return;
	}

	if (creature[player[i].creatureIndex].col<0)
	{
		fi_WriteToErrorLog("col<0 in in_PickUpFromList");
		return;
	}

	if (creature[player[i].creatureIndex].col>=kMapHeight)
	{
		fi_WriteToErrorLog("col>kMapWidth in in_PickUpFromList");
		return;
	}

	theContainer=containerMap[creature[player[i].creatureIndex].row][creature[player[i].creatureIndex].col];

	if (theContainer==0)
		return;

	if (item==0)
	{
		gold=in_GetGoldTotal(container[theContainer][0].itemID, container[theContainer][0].itemCount); // add to carbon 11/27

		if (count>gold)	count=gold;

		if (count>0)
		{
			goldAdded=in_AddGoldtoInventory(player[i].creatureIndex, count);

			if (goldAdded>0)
			{
				in_SetGoldTotal(goldAdded, &gold1, &gold2);
				nw_SendGoldAdd(i, kTakeItem, 0, gold1, gold2);
				gold=gold-goldAdded;
				in_SetGoldTotal(gold, &gold1, &gold2);
				container[theContainer][0].itemID=gold1;
				container[theContainer][0].itemCount=gold2;
			}
		}
	}
	else
	{
		for (j=1;j<kMaxInventorySize;j++)
			if ((container[theContainer][j].itemID!=0) && (item==j))
			{
				if (in_CanGroup(container[theContainer][j].itemID))
				{
					if (count>container[theContainer][j].itemCount)  //018
					{
						fi_WriteToErrorLog("Pick Up Count Error");	// beta debug
						count=container[theContainer][j].itemCount;
					}

					moveCount=in_AddtoInventory(player[i].creatureIndex,container[theContainer][j].itemID,count);
				}
				else
					moveCount=in_AddtoInventory(player[i].creatureIndex,container[theContainer][j].itemID,container[theContainer][j].itemCount);

				if (moveCount>0)
				{
					if (in_CanGroup(container[theContainer][j].itemID))
					{
						nw_SendInventoryAdd(i, container[theContainer][j].itemID, moveCount,kTakeItem,0);
						container[theContainer][j].itemCount=container[theContainer][j].itemCount-moveCount;

						if (container[theContainer][j].itemCount==0)
							container[theContainer][j].itemID=0;
					}
					else
					{
						nw_SendInventoryAdd(i, container[theContainer][j].itemID, container[theContainer][j].itemCount,kTakeItem,0);
						container[theContainer][j].itemCount=0;
						container[theContainer][j].itemID=0;
					}
				}

				break;
			}
	}

	emptied=true;

	for (j=0;j<kMaxInventorySize;j++)
		if ((container[theContainer][j].itemID!=0) || (container[theContainer][j].itemCount!=0))
			emptied=false;

	if (containerID[theContainer]==30)
		emptied=false;

	if ((emptied) && (!containerIsCorpse[containerID[theContainer]]))
	{
		if (containerID[theContainer]==55)
			fireDungeonTreasureToSpawn++;

		if (containerID[theContainer]==27)
			dungeonTreasureToSpawn++;

		if (containerID[theContainer]==28)
			sewerTreasureToSpawn++;

		if (containerID[theContainer]==29)
			lizardTreasureToSpawn++;

		if (containerID[theContainer]==32)
			iceTreasureToSpawn++;

		in_ClearContainer(theContainer);
	}
	else if ((emptied) && (containerGM[theContainer]))
	{
		containerDecayTime[theContainer]=TickCount()+(1*3600);
	}

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_PickUpFromBank(int i, int item, int count) // server
// ----------------------------------------------------------------- //

{
	int									j;
	int									moveCount;
	UInt32							gold;
	UInt32							goldAdded;
	int									gold1;
	int									gold2;


	if (player[i].creatureIndex==0)
	{
		fi_WriteToErrorLog("Zero creature index in in_PickUpFromBank");
		return;
	}

	if (item==0)
	{
		gold=in_GetGoldTotal(player[i].bank[0].itemID, player[i].bank[0].itemCount);

		if (count>gold)	count=gold;	// version 0.1.1

		if (gold>0)
		{
			goldAdded=in_AddGoldtoInventory(player[i].creatureIndex, count);

			if (goldAdded>0)
			{
				in_SetGoldTotal(goldAdded, &gold1, &gold2);
				nw_SendGoldAdd(i, kWithdrawItem, 0, gold1, gold2);
				gold=gold-goldAdded;
				in_SetGoldTotal(gold, &gold1, &gold2);
				player[i].bank[0].itemID=gold1;
				player[i].bank[0].itemCount=gold2;
			}
		}
	}
	else
	{
		for (j=1;j<kMaxInventorySize;j++)
			if ((player[i].bank[j].itemID!=0) && (item==j))
			{

				if (in_CanGroup(player[i].bank[j].itemID))
				{
					if (count>player[i].bank[j].itemCount)  //018
					{
						fi_WriteToErrorLog("Bank Count Error");	// beta debug
						count=player[i].bank[j].itemCount;
					}

					moveCount=in_AddtoInventory(player[i].creatureIndex,player[i].bank[j].itemID,count);
				}
				else
					moveCount=in_AddtoInventory(player[i].creatureIndex,player[i].bank[j].itemID,player[i].bank[j].itemCount);

				if (moveCount>0)
				{
					if (in_CanGroup(player[i].bank[j].itemID))
					{
						nw_SendInventoryAdd(i, player[i].bank[j].itemID, moveCount, kWithdrawItem,0);
						player[i].bank[j].itemCount=player[i].bank[j].itemCount-moveCount;

						if (player[i].bank[j].itemCount==0)
							player[i].bank[j].itemID	 =0;
					}
					else
					{
						nw_SendInventoryAdd(i, player[i].bank[j].itemID, player[i].bank[j].itemCount,kWithdrawItem,0);
						player[i].bank[j].itemCount	=	0;
						player[i].bank[j].itemID		=	0;
					}
				}

				break;
			}
	}

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_BuyFromList(int i, int item, int count) // server
// ----------------------------------------------------------------- //

{
	int									j;
	int									moveCount;
	int									trader;
	UInt32							gold;
	UInt32							goldRemoved;
	int									gold1;
	int									gold2;
	int									theShop;


	if (player[i].creatureIndex==0)
	{
		fi_WriteToErrorLog("Zero creature index in in_BuyFromList");
		return;
	}

	theShop=sh_GetShopID(i);

	if (theShop==0)
		return;

	for (j=0;j<kMaxInventorySize;j++)
		if ((shop[theShop][j].itemID!=0) && (item==j))
		{

			gold=in_GetGoldTotal(creature[player[i].creatureIndex].inventory[0].itemID, creature[player[i].creatureIndex].inventory[0].itemCount);

			if (shop[theShop][j].cost>gold)
			{
				nw_SendDisplayMessage(i, kNotEnoughGold);
				return;
			}

			if (in_CanGroup(shop[theShop][j].itemID))
				while ((shop[theShop][j].cost*count)>gold)
				{
					count--;

					if (count<=0)
					{
						nw_SendDisplayMessage(i, kNotEnoughGold);
						return;
					}
				}

			if (itemList[shop[theShop][j].itemID].itemType==kSpellType) //Bought a spell from trader
			{
				trader=sh_GetTrader(i);
				moveCount=1;

				if (trader==0)
					return;

				sk_CastSpellOnCreature(trader, i, shop[theShop][j].itemID, false);
			}
			else
			{
				if (!in_CanGroup(shop[theShop][j].itemID))	// make sure item is in good condition
					count=127;

				moveCount=in_AddtoInventory(player[i].creatureIndex,shop[theShop][j].itemID,count);

				if (moveCount>0)
				{
					nw_SendInventoryAdd(i, shop[theShop][j].itemID, moveCount,kBoughtItem,0);

					if (shop[theShop][j].itemID==kGinseng)
						ginsengSpawn        = ginsengSpawn       - moveCount;
					else if (shop[theShop][j].itemID==kLotusFlower)
						lotusFlowerSpawn    = lotusFlowerSpawn   - moveCount;
					else if (shop[theShop][j].itemID==kMandrakeRoot)
						mandrakeRootSpawn   = mandrakeRootSpawn  - moveCount;
					else if (shop[theShop][j].itemID==kGarlic)
						garlicSpawn         = garlicSpawn        - moveCount;
					else if (shop[theShop][j].itemID==kVolcanicAsh)
						volcanicAshSpawn    = volcanicAshSpawn   - moveCount;
					//Dave
					else if (shop[theShop][j].itemID==kMushroom)
						mushroomSpawn    = mushroomSpawn   - moveCount;

				}
			}

			break;
		}

	gold=in_GetGoldTotal(creature[player[i].creatureIndex].inventory[0].itemID, creature[player[i].creatureIndex].inventory[0].itemCount);

	if (!in_CanGroup(shop[theShop][j].itemID))
		moveCount=1;

	goldRemoved=shop[theShop][j].cost*moveCount;

	if (goldRemoved>gold)
		goldRemoved=gold;

	if (goldRemoved>0)
	{
		nw_SendGoldRemove(i, kPaid, goldRemoved, 0);
		gold=gold-goldRemoved;
		in_SetGoldTotal(gold, &gold1, &gold2);
		creature[player[i].creatureIndex].inventory[0].itemID=gold1;
		creature[player[i].creatureIndex].inventory[0].itemCount=gold2;
	}

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_ClearContainer(int	i) // server
// ----------------------------------------------------------------- //

{
	int				j;


	if ((i<0) || (i>=kMaxContainers))
	{
		fi_WriteToErrorLog("Out of range in in_ClearContainer");
		return;
	}

	if (containerEmpty[i]==false)
		currentNumberOfContainers--;

	for (j=0;j<kMaxInventorySize;j++)
	{
		container[i][j].itemID		=	0;
		container[i][j].itemCount	=	0;
	}

	containerID[i]																		= 0;

	containerEmpty[i]																	=	true;
	containerDecayTime[i]															=	0;

	if ((containerRow[i]>=0) && (containerRow[i]<=kMapWidth) && (containerCol[i]>=0) && (containerCol[i]<kMapHeight))
		containerMap[containerRow[i]][containerCol[i]]	=	0;

	containerRow[i]																		=	0;

	containerCol[i]																		=	0;

	containerHidden[i]															  =	0;

	containerLock[i]																	=	0;

	containerTrap[i]																	=	0;

	containerMagicLock[i]															=	0;

	containerMagicTrap[i]															=	0;

	containerGM[i]                                    = false;

	strcpy(containerName[i],"");

	strcpy(containerGift[i],"");

	strcpy(containerTrapName[i],"");

	strcpy(containerMagicTrapName[i],"");

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_ClearTracks(int	i) // server
// ----------------------------------------------------------------- //

{
	//int				j;


	if ((i<0) || (i>=kMaxTracks))
	{
		fi_WriteToErrorLog("Out of range in in_ClearTracks");
		return;
	}

	if ((tracksRow[i]>=0) && (tracksRow[i]<=kMapWidth) && (tracksCol[i]>=0) && (tracksCol[i]<kMapHeight))
	{
		tracksMap[tracksRow[i]][tracksCol[i]]		=	0;
		tracksRow[i]								=	0;
		tracksCol[i]								=	0;
		tracksID[i]									= 	0;
		tracksDecayTime[i]							=	0;
		strcpy(tracksNote[i], "");
		strcpy(tracksGift[i],"");
		currentNumberOfTracks--;
	}
}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_ItemDestroyed(int i, int count) // server
// ----------------------------------------------------------------- //

{

	if ((i<0) || (i>=kMaxItems))
	{
		fi_WriteToErrorLog("Out of range in in_ItemDestroyed");
		return;
	}

	if (in_CanGroup(i))
		itemList[i].currentCount=itemList[i].currentCount-count;
	else
		itemList[i].currentCount=itemList[i].currentCount--;

	if (itemList[i].currentCount<0)
		itemList[i].currentCount=0;

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_CheckContainerDecay(void) // server
// ----------------------------------------------------------------- //

{
	int									i;
	int									j;
	unsigned int				currentTime=TickCount();
	int                 r;
	int                 c;
	int                 rare;
	int                 breakout=0;
	int                 numItems=0;
	int                 itemFound=0;

	//char				debugStr[kStringLength];
	//char				debugStr2[kStringLength];

#ifdef _FUNCTIONTRACEDEBUGGING
	fi_WriteToErrorLog("in_CheckContainerDecay Entry");
#endif

	for (i=1;i<kMaxContainers;i++)
		if (containerEmpty[i]==false)
			if ((currentTime>containerDecayTime[i]) && (containerDecayTime[i]!=0) && (containerID[i]!=30))  // permanent chest
			{
				// gs questionable 2/26/03 //  nw_GetMessages();
				if (creatureMap[containerRow[i]][containerCol[i]]==0)
				{
					numItems=0;
					itemFound=0;

					for (j=0;j<kMaxInventorySize;j++)
						if (container[i][j].itemID!=0)
						{
							numItems++;
							itemFound=container[i][j].itemID;

							if (itemList[container[i][j].itemID].desiredCount>0)
								in_ItemDestroyed(container[i][j].itemID, container[i][j].itemCount);
						}

					if (numItems==1)
						if (itemFound==kBrackenSeed)
							if (canWalkOn[map[containerRow[i]][containerCol[i]]])
								cr_SpawnCreature(kReaper, containerRow[i], containerCol[i]);

					in_ClearContainer(i);

					//    qu_SpawnQuest();
				}
				else
					containerDecayTime[i]=currentTime+3600;
			}

	for (i=1; i<kMaxTracks; i++)
		if (currentTime>tracksDecayTime[i] && tracksDecayTime[i]!=0)
			in_ClearTracks(i);

	if (currentTime>treasureSpawnTime)
	{
		fi_SpawnChests();
		treasureSpawnTime=currentTime+(tb_Rnd((3600*30),(3600*180))); //treasureSpawnTime=currentTime+(3600*20);
	}

#ifdef _FUNCTIONTRACEDEBUGGING
	fi_WriteToErrorLog("in_CheckContainerDecay Exit");

#endif
	return;

	breakout=0;

	if (fireDungeonTreasureToSpawn>0)
		if (currentTime>treasureSpawnTime)
		{
			while (fireDungeonTreasureToSpawn>0) // spawn chests in volcano
			{
				// gs questionable 2/26/03 //  nw_GetMessages();
				breakout++;

				if (breakout>1000) break;

				if (tb_Rnd(0, 100)<60) //Volcano first floor
				{
					r=tb_Rnd(224,238);
					c=tb_Rnd(1897,1953);

					if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
					{
						fireDungeonTreasureToSpawn--;
						in_DropGold(55,tb_Rnd(15,60), r, c, 0, false,"");

						if (tb_Rnd(1,5)==1) in_DropItem(0,55,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");

						if (tb_Rnd(1,50)==1) in_DropItem(0,55,kLoafOfBread, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kCrystalRing, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kPlatinumRing, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kGoldRing, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kSilverRing, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kRedDye, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kDiamond, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kRuby, 1, r, c, 0, false, "");

						if (tb_Rnd(1,50)==1) in_DropItem(0,55,kPoisonGland, 1, r, c, 0, false, "");

						if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackOpenPlateHelmet, 127, r, c, 0, false, "");

						if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackBreastPlate, 127, r, c, 0, false, "");

						if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackLegsPlate, 127, r, c, 0, false, "");

						if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackPlateArms, 127, r, c, 0, false, "");

						if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackPlateGauntlets, 127, r, c, 0, false, "");

						if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackSlottedPlateHelmet, 127, r, c, 0, false, "");

						if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackClosedPlateHelmet, 127, r, c, 0, false, "");

						if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackVikingHelmet, 127, r, c, 0, false, "");

						if (tb_Rnd(1,300)==1) in_DropItem(0,55,kDragonTooth, tb_Rnd(1,5), r, c, 0, false, ""); // put in carbon 11/24

						if (tb_Rnd(1,300)==1)
						{
							rare=it_GetRare(0);

							if (rare>0)
								if (in_CanGroup(rare))
									in_DropItem(0,55,rare, 1, r, c, 0, false, "");
								else
									in_DropItem(0,55,rare, 127, r, c, 0, false, "");
						}

						if (tb_Rnd(1,2)==1)
							in_DropItem(0,55,kLock, 1, r, c, tb_Rnd(2,4), false, "");
						else
							in_DropItem(0,55,kLock, 1, r, c, tb_Rnd(3,7), false, "");

						if (tb_Rnd(1,50)==1)
							if (tb_Rnd(1,10)<10)
								in_DropItem(0,55,kTrap, 1, r, c, tb_Rnd(1,30), false, "");
							else
								in_DropItem(0,55,kTrap, 1, r, c, tb_Rnd(1,99), false, "");
					}
				}
				else //Volcano second floor
				{
					r=tb_Rnd(106,109);
					c=tb_Rnd(1924,1957);

					if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
					{
						fireDungeonTreasureToSpawn--;
						in_DropGold(55,tb_Rnd(15,60), r, c, 0, false,"");

						if (tb_Rnd(1,5)==1) in_DropItem(0,55,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");

						if (tb_Rnd(1,50)==1) in_DropItem(0,55,kLoafOfBread, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kCrystalRing, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kPlatinumRing, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kGoldRing, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kSilverRing, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kRedDye, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kDiamond, 1, r, c, 0, false, "");

						if (tb_Rnd(1,100)==1) in_DropItem(0,55,kRuby, 1, r, c, 0, false, "");

						if (tb_Rnd(1,50)==1) in_DropItem(0,55,kPoisonGland, 1, r, c, 0, false, "");

						if (tb_Rnd(1,300)==1) in_DropItem(0,55,kBlackOpenPlateHelmet, 127, r, c, 0, false, "");

						if (tb_Rnd(1,300)==1) in_DropItem(0,55,kBlackBreastPlate, 127, r, c, 0, false, "");

						if (tb_Rnd(1,300)==1) in_DropItem(0,55,kBlackLegsPlate, 127, r, c, 0, false, "");

						if (tb_Rnd(1,300)==1) in_DropItem(0,55,kBlackPlateArms, 127, r, c, 0, false, "");

						if (tb_Rnd(1,300)==1) in_DropItem(0,55,kBlackPlateGauntlets, 127, r, c, 0, false, "");

						if (tb_Rnd(1,300)==1) in_DropItem(0,55,kBlackSlottedPlateHelmet, 127, r, c, 0, false, "");

						if (tb_Rnd(1,300)==1) in_DropItem(0,55,kBlackClosedPlateHelmet, 127, r, c, 0, false, "");

						if (tb_Rnd(1,300)==1) in_DropItem(0,55,kBlackVikingHelmet, 127, r, c, 0, false, "");

						if (tb_Rnd(1,200)==1) in_DropItem(0,55,kDragonTooth, tb_Rnd(1,5), r, c, 0, false, ""); // put in carbon 11/24

						if (tb_Rnd(1,200)==1)
						{
							rare=it_GetRare(0);

							if (rare>0)
								if (in_CanGroup(rare))
									in_DropItem(0,55,rare, 1, r, c, 0, false, "");
								else
									in_DropItem(0,55,rare, 127, r, c, 0, false, "");
						}

						if (tb_Rnd(1,2)==1)
							in_DropItem(0,55,kLock, 1, r, c, tb_Rnd(4,6), false, "");
						else
							in_DropItem(0,55,kLock, 1, r, c, tb_Rnd(5,7), false, "");

						if (tb_Rnd(1,50)==1)
							if (tb_Rnd(1,10)<10)
								in_DropItem(0,55,kTrap, 1, r, c, tb_Rnd(3,30), false, "");
							else
								in_DropItem(0,55,kTrap, 1, r, c, tb_Rnd(5,99), false, "");
					}
				}
			}
		}

	breakout=0;

	if (dungeonTreasureToSpawn>0)
		if (currentTime>treasureSpawnTime)
		{
			while (dungeonTreasureToSpawn>0) // spawn chests in dungeon
			{
				// gs questionable 2/26/03 //  nw_GetMessages();
				breakout++;

				if (breakout>1000) break;

				r=tb_Rnd(42,90);

				c=tb_Rnd(54,227);

				if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
				{
					dungeonTreasureToSpawn--;
					in_DropGold(27,tb_Rnd(5,30), r, c, 0, false,"");

					if (tb_Rnd(1,5)==1) in_DropItem(0,27,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,27,kLoafOfBread, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,27,kCrystalRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,27,kPlatinumRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,27,kGoldRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,27,kSilverRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,27,kRedDye, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,27,kDiamond, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,27,kRuby, 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,27,kPoisonGland, 1, r, c, 0, false, "");

					if (tb_Rnd(1,500)==1) in_DropItem(0,27,kDragonTooth, tb_Rnd(1,5), r, c, 0, false, ""); // put in carbon 11/24

					if (tb_Rnd(1,250)==1)
					{
						rare=it_GetRare(0);

						if (rare>0)
							if (in_CanGroup(rare))
								in_DropItem(0,27,rare, 1, r, c, 0, false, "");
							else
								in_DropItem(0,27,rare, 127, r, c, 0, false, "");
					}

					if (tb_Rnd(1,2)==1)
						in_DropItem(0,27,kLock, 1, r, c, tb_Rnd(1,4), false, "");
					else
						in_DropItem(0,27,kLock, 1, r, c, tb_Rnd(1,8), false, "");

					if (tb_Rnd(1,50)==1)
						if (tb_Rnd(1,10)<10)
							in_DropItem(0,27,kTrap, 1, r, c, tb_Rnd(1,20), false, "");
						else
							in_DropItem(0,27,kTrap, 1, r, c, tb_Rnd(1,99), false, "");
				}
			}
		}

	breakout=0;

	if (sewerTreasureToSpawn>0)
		if (currentTime>treasureSpawnTime)
		{
			while (sewerTreasureToSpawn>0) // spawn chests in sewer
			{
				// gs questionable 2/26/03 //  nw_GetMessages();
				breakout++;

				if (breakout>1000) break;

				r=tb_Rnd(1849,1911);  // udpate

				c=tb_Rnd(1931,1980);  // udpate

				if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
				{
					sewerTreasureToSpawn--;
					fi_WriteToErrorLog("Spawned in sewers");
					in_DropGold(28,tb_Rnd(1,8), r, c, 0, false,"");

					if (tb_Rnd(1,10)==1) in_DropItem(0,28,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,28,kLoafOfBread, 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,28,kLesserHealPotion, 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,28,kManaPotion, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,28,kPoisonGland, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,28,kBlueBoots, 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,28,kLesserDetoxifyPotion, 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,28,kStrengthPotion, 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,28,kNightVisionPotion, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,28,kKatana, 127, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,28,kBroadSword, 127, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,28,kScimitar, 127, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,28,kPurpleDye, 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,28,kBlueDye, 1, r, c, 0, false, "");

					if (tb_Rnd(1,500)==1) in_DropItem(0,28,kDragonTooth, tb_Rnd(1,5), r, c, 0, false, ""); // put in carbon 11/24

					in_DropItem(0,28,kLock, 1, r, c, tb_Rnd(1,2), false, "");

					if (tb_Rnd(1,50)==1)
						in_DropItem(0,28,kTrap, 1, r, c, tb_Rnd(1,5), false, "");
				}
			}
		}

	breakout=0;

	if (lizardTreasureToSpawn>0)
		if (currentTime>treasureSpawnTime)
		{
			while (lizardTreasureToSpawn>0) // spawn chests in strong hold
			{
				// gs questionable 2/26/03 //  nw_GetMessages();
				breakout++;

				if (breakout>1000) break;

				r=tb_Rnd(835,896);

				c=tb_Rnd(208,254);

				if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
				{
					lizardTreasureToSpawn--;
					in_DropGold(29,tb_Rnd(10,45), r, c, 0, false,"");

					if (tb_Rnd(1,5)==1) in_DropItem(0,29,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,29,kLoafOfBread, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,29,kCrystalRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,29,kPlatinumRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,29,kGoldRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,29,kSilverRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,29,kRedDye, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,29,kDiamond, 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1)  in_DropItem(0,29,kPoisonGland, 1, r, c, 0, false, "");

					if (tb_Rnd(1,500)==1)  in_DropItem(0,29,kDragonTooth, 1, r, c, 0, false, "");

					if (tb_Rnd(1,8)==1)   in_DropItem(0,29,kQueensRuby, 1, r, c, 0, false, "");

					if (tb_Rnd(1,750)==1)
					{
						rare=it_GetRare(0);

						if (rare>0)
							in_DropItem(0,29,rare, 1, r, c, 0, false, "");
					}

					if (tb_Rnd(1,2)==1)
						in_DropItem(0,29,kLock, 1, r, c, tb_Rnd(1,2), false, "");
					else
						in_DropItem(0,29,kLock, 1, r, c, tb_Rnd(1,5), false, "");

					if (tb_Rnd(1,25)==1)
						if (tb_Rnd(1,10)<10)
							in_DropItem(0,29,kTrap, 1, r, c, tb_Rnd(1,20), false, "");
						else
							in_DropItem(0,29,kTrap, 1, r, c, tb_Rnd(1,99), false, "");
				}
			}
		}

	breakout=0;

	if (iceTreasureToSpawn>0)
		if (currentTime>treasureSpawnTime)
		{
			while (iceTreasureToSpawn>0) // spawn chests in dungeon
			{
				// gs questionable 2/26/03 //  nw_GetMessages();
				breakout++;

				if (breakout>1000) break;

				r=tb_Rnd(200,329);

				c=tb_Rnd(61,196);

				if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
				{
					iceTreasureToSpawn--;
					in_DropGold(32,tb_Rnd(5,30), r, c, 0, false,"");

					if (tb_Rnd(1,5)==1) in_DropItem(0,32,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,32,kLoafOfBread, tb_Rnd(1,5), r, c, 0, false, ""); // put in carbon 11/24

					if (tb_Rnd(1,100)==1) in_DropItem(0,32,kCrystalRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,32,kPlatinumRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,32,kGoldRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,32,kSilverRing, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,32,kRedDye, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,32,kDiamond, 1, r, c, 0, false, "");

					if (tb_Rnd(1,100)==1) in_DropItem(0,32,kRuby, 1, r, c, 0, false, "");

					if (tb_Rnd(1,50)==1) in_DropItem(0,32,kPoisonGland, tb_Rnd(1,5), r, c, 0, false, ""); // put in carbon 11/24

					if (tb_Rnd(1,500)==1) in_DropItem(0,32,kDragonTooth, tb_Rnd(1,5), r, c, 0, false, ""); // put in carbon 11/24

					if (tb_Rnd(1,100)==1) in_DropItem(0,32,kIceCrystal, 1, r, c, 0, false, "");

					if (tb_Rnd(1,250)==1)
					{
						rare=it_GetRare(0);

						if (rare>0)
							in_DropItem(0,32,rare, 1, r, c, 0, false, "");
					}

					if (tb_Rnd(1,2)==1)
						in_DropItem(0,32,kLock, 1, r, c, tb_Rnd(1,4), false, "");
					else
						in_DropItem(0,32,kLock, 1, r, c, tb_Rnd(1,8), false, "");

					if (tb_Rnd(1,50)==1)
						if (tb_Rnd(1,10)<10)
							in_DropItem(0,32,kTrap, 1, r, c, tb_Rnd(1,20), false, "");
						else
							in_DropItem(0,32,kTrap, 1, r, c, tb_Rnd(1,99), false, "");
				}
			}
		}

	if (currentTime>treasureSpawnTime)
		treasureSpawnTime=currentTime+(3600*5);

}

#endif

// ----------------------------------------------------------------- //
void in_DropFromInventorySlot(int slot,int count) // client
// ----------------------------------------------------------------- //

{
#ifndef _SERVERONLY

	if (slot>=kMaxInventorySize)
		return;

	if (localInventory[slot].itemID==0) // paranoid error checking
		return;

	if (localInventory[slot].itemCount==0) // paranoid error checking
	{
		localInventory[slot].itemID=0;
		return;
	}

	if (in_CanGroup(localInventory[slot].itemID))
	{
		if (count>localInventory[slot].itemCount) //018
			localInventory[slot].itemCount=0;
		else
			localInventory[slot].itemCount=localInventory[slot].itemCount-count;
	}
	else
		localInventory[slot].itemCount	= 0;

	if (localInventory[slot].itemCount==0)
		localInventory[slot].itemID	=	0;

	if (localInventory[slot].itemID==0)
	{
		if (localRightHand==slot)
		{
			localRightHand=0;
			nw_SendArm(kRightHand,0);
		}

		else if (localLeftHand==slot)
		{
			localLeftHand=0;
			nw_SendArm(kLeftHand,0);
		}

		else if (localLegs==slot)
		{
			localLegs=0;
			nw_SendArm(kLegs,0);
		}

		else if (localTorso==slot)
		{
			localTorso=0;
			nw_SendArm(kTorso,0);
		}

		else if (localFeet==slot)
		{
			localFeet=0;
			nw_SendArm(kFeet,0);
		}

		else if (localRightFinger==slot)
		{
			localRightFinger=0;
			nw_SendArm(kRightFinger,0);
		}

		else if (localLeftFinger==slot)
		{
			localLeftFinger=0;
			nw_SendArm(kLeftFinger,0);
		}

		else if (localArms==slot)
		{
			localArms=0;
			nw_SendArm(kArms,0);
		}

		else if (localHead==slot)
		{
			localHead=0;
			nw_SendArm(kHead,0);
		}

		else if (localNeck==slot)
		{
			localNeck=0;
			nw_SendArm(kNeck,0);
		}

		else if (localHands==slot)
		{
			localHands=0;
			nw_SendArm(kHands,0);
		}

		else if (localWaist==slot)
		{
			localWaist=0;
			nw_SendArm(kWaist,0);
		}

	}

	gr_RenderPlayerDisplay(true);

#endif
}

// ----------------------------------------------------------------- //
void in_DamageInventorySlot(int slot, int damage) // client mod 8/9
// ----------------------------------------------------------------- //

{
#ifndef _SERVERONLY
	char								message[kStringLength];//[128];
	char								itemName[kStringLength];//[48];
	TBoolean            groupedItem;


	if (localInventory[slot].itemCount==0)
		return;

	if (in_CanGroup(localInventory[slot].itemID))
	{
		groupedItem=true;
		strcpy(message,"Your ");
		in_GetItemName(localInventory[slot].itemID, itemName, -2);
		strcat(message,itemName);

		if (localInventory[slot].itemID==kShears)
			strcat(message, " have been destroyed.");
		else
			strcat(message," has been destroyed.");

		gr_AddText(message,false,true);

		if (localInventory[slot].itemCount>0) //018
			localInventory[slot].itemCount--;

		if (localInventory[slot].itemCount==0)
			localInventory[slot].itemID			=	0;
	}
	else
	{
		groupedItem=false;
		//if (!((localInventory[slot].itemCount==0) || (localInventory[slot].itemCount==127)))

		if (localInventory[slot].itemCount>127) // mod 8/9
		{
			if (damage>localInventory[slot].itemCount)
				localInventory[slot].itemCount=127;
			else
				localInventory[slot].itemCount=localInventory[slot].itemCount-damage;

			if (localInventory[slot].itemCount<127)
				localInventory[slot].itemCount=127;
		}
		else // mod 8/9
		{
			if (localInventory[slot].itemCount>damage) //018
				localInventory[slot].itemCount=localInventory[slot].itemCount-damage;
			else
				localInventory[slot].itemCount=0;
		}

		if ((localInventory[slot].itemCount==0) || (localInventory[slot].itemCount==127))
		{
			strcpy(message,"Your ");
			in_GetItemName(localInventory[slot].itemID, itemName, -2);
			strcat(message,itemName);
			strcat(message," has been destroyed.");
			gr_AddText(message,false,true);
			localInventory[slot].itemID			=	0;
			localInventory[slot].itemCount	=	0;
		}
	}

	if ((localInventory[slot].itemID==0) || (groupedItem))
	{
		if (localRightHand==slot)
			localRightHand=0;

		else if (localLeftHand==slot)
			localLeftHand=0;

		else if (localLegs==slot)
			localLegs=0;

		else if (localTorso==slot)
			localTorso=0;

		else if (localFeet==slot)
			localFeet=0;

		else if (localRightFinger==slot)
			localRightFinger=0;

		else if (localLeftFinger==slot)
			localLeftFinger=0;

		else if (localArms==slot)
			localArms=0;

		else if (localHead==slot)
			localHead=0;

		else if (localHands==slot)
			localHands=0;

		else if (localNeck==slot)
			localNeck=0;

		else if (localWaist==slot)
			localWaist=0;
	}

	gr_RenderPlayerDisplay(true);

#endif
}

// ----------------------------------------------------------------- //
void in_ConsumeInventorySlot(int slot) // client
// ----------------------------------------------------------------- //

{
	int           i;

	if (slot>kMaxInventorySize) // paranoid error checking
		return;

	if (localInventory[slot].itemID==0)
		return;

	if (nw_CanSend())
	{

		if (itemList[localInventory[slot].itemID].itemType==kBeverageType)
			nw_SendTextToHost("*hiccup*");
			//fx_CreatureServerFX(player[i].creatureIndex, player[i].creatureIndex, kSparkle3, (5*60), 0);
			//fx_CreatureServerFX(i, i, kLightExplosion, (2*60), 0);
	// Dave	
		if (itemList[localInventory[slot].itemID].itemType==kBeverageType2)
			nw_SendTextToHost("*burp*");
		
		if (itemList[localInventory[slot].itemID].itemType==kBeverageType3)
			nw_SendTextToHost("*Ahhhhhhh*");
		
		if (itemList[localInventory[slot].itemID].itemType==kBeverageType4)
			nw_SendTextToHost("Go Buckeyes!");
		
		//Dave add Elixirs
		if (itemList[localInventory[slot].itemID].itemType==kElixirType1)
			nw_SendTextToHost("* <3 <3 <3 *");
		if (itemList[localInventory[slot].itemID].itemType==kElixirType2)
			nw_SendTextToHost("* Uh... Duhhhhh.... *");
		if (itemList[localInventory[slot].itemID].itemType==kElixirType3)
			nw_SendTextToHost("* Zimm Boom Pop *");
		if (itemList[localInventory[slot].itemID].itemType==kElixirType4)
			nw_SendTextToHost("* KaPoooooow *");
		if (itemList[localInventory[slot].itemID].itemType==kElixirType5)
			nw_SendTextToHost("* Tingle *");
	//Dave^
		if ((itemList[localInventory[slot].itemID].itemType==kCookedMeatType) || (itemList[localInventory[slot].itemID].itemType==kCookedVegType))
			nw_SendAction(kEat,kNormalPriority,kSendNow,true);

		if (itemList[localInventory[slot].itemID].itemType==kDyeType)
		{
			gr_AddText("You apply the dye to your hair.",false,true);

			switch (itemList[localInventory[slot].itemID].itemColor)
			{
				case kLightBlue:
					nw_SendAction(kLightBlueHairDye,kNormalPriority,kSendNow,true);
					break;

				case kRed:
					nw_SendAction(kRedHairDye,kNormalPriority,kSendNow,true);
					break;

				case kBlue:
					nw_SendAction(kBlueHairDye,kNormalPriority,kSendNow,true);
					break;

				case kBrown:
					nw_SendAction(kBrownHairDye,kNormalPriority,kSendNow,true);
					break;

				case kBlack:
					nw_SendAction(kBlackHairDye,kNormalPriority,kSendNow,true);
					break;

				case kGreen:
					nw_SendAction(kGreenHairDye,kNormalPriority,kSendNow,true);
					break;

				case kPurple:
					nw_SendAction(kPurpleHairDye,kNormalPriority,kSendNow,true);
					break;

				case kYellow:
					nw_SendAction(kYellowHairDye,kNormalPriority,kSendNow,true);
					break;
			//Dave
				case kGray:
					nw_SendAction(kGrayHairDye,kNormalPriority,kSendNow,true);
					break;
					
				//case kOrange1:
				//	nw_SendAction(kOrangeHairDye,kNormalPriority,kSendNow,true);
				//	break;
			}

		}

		if (itemList[localInventory[slot].itemID].itemType==kPotionType)
		{
			if ((localRightHand!=0) || (localLeftHand!=0))
			{
				gr_AddText("Your hands must be free to use magic.",false,true);
				//gr_RenderInventory(true);
				return;
			}

			gr_AddText("You drink the potion.",false,true);

			if (localInventory[slot].itemID==kLesserHealPotion)
				nw_SendAction(kDrinkLesserHealPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kGreaterHealPotion)
				nw_SendAction(kDrinkGreaterHealPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kFullHealPotion)
				nw_SendAction(kDrinkFullHealPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kManaPotion)
				nw_SendAction(kDrinkManaPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kLesserDetoxifyPotion)
				nw_SendAction(kDrinkLesserDetoxifyPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kGreaterDetoxifyPotion)
				nw_SendAction(kDrinkGreaterDetoxifyPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kFullDetoxifyPotion)
				nw_SendAction(kDrinkFullDetoxifyPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kStrengthPotion)
				nw_SendAction(kDrinkStrengthPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kAgilityPotion)
				nw_SendAction(kDrinkAgilityPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kElevateMindPotion)
				nw_SendAction(kDrinkElevateMindPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kNightVisionPotion)
				nw_SendAction(kDrinkNightVisionPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kInvisibilityPotion)
				nw_SendAction(kDrinkInvisibilityPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kNourishPotion)
				nw_SendAction(kDrinkNourishPotion,kNormalPriority,kSendNow,true);

			if (localInventory[slot].itemID==kOpportunityPotion)
				nw_SendAction(kDrinkOpportunityPotion,kNormalPriority,kSendNow,true);
		}
		

		if (in_CanGroup(localInventory[slot].itemID))
		{
			if (localInventory[slot].itemCount==1)
				localInventory[slot].itemCount	=	0;
			else if (localInventory[slot].itemCount>0)  //018
				localInventory[slot].itemCount--;
		}
		else
			localInventory[slot].itemCount	= 0;

		if (localInventory[slot].itemCount==0)
			localInventory[slot].itemID	=	0;

		localWeight=0;

		for (i=1;i<kMaxInventorySize;i++)
			if (localInventory[i].itemID!=0)
				if (in_CanGroup(localInventory[i].itemID))
					localWeight=localWeight+itemList[localInventory[i].itemID].weight+(itemList[localInventory[i].itemID].weight*localInventory[i].itemCount)/10;
				else
					localWeight=localWeight+itemList[localInventory[i].itemID].weight;

		nw_SendDeleteFromInventorySlot(slot);

		gr_RenderPlayerDisplay(true);
	}

}

#ifdef _SERVER
// ----------------------------------------------------------------- //
void in_MakeContainer(int row, int col, int container, TBoolean gmSpawn) // put in carbon 11/12
// ----------------------------------------------------------------- //

{
	int							inContainer		=	containerMap[row][col];
	int							i;
	//int							j;

	if (inContainer!=0)
	{
		if (!gmSpawn)	//To prevent pets from obliterating anything
			return;
		else
			in_ClearContainer(inContainer);
	}

	if (currentNumberOfContainers>=(kMaxContainers-1)) // no empty containers
		return;

	for (i=currentNumberOfContainers;i<kMaxContainers;i++)
		if (containerEmpty[i])
		{
			in_ClearContainer(i); // poranoid error checking
			inContainer=i;
			containerLock[inContainer]=0;
			containerTrap[inContainer]=0;
			containerMagicLock[inContainer]=0;
			containerMagicTrap[inContainer]=0;
			strcpy(containerName[inContainer],"");
			strcpy(containerGift[inContainer],"");
			break;
		}

	if (inContainer==0)
		for (i=1;i<currentNumberOfContainers;i++)
			if (containerEmpty[i])
			{
				in_ClearContainer(i); // poranoid error checking
				inContainer=i;
				containerLock[inContainer]=0;
				containerTrap[inContainer]=0;
				containerMagicLock[inContainer]=0;
				containerMagicTrap[inContainer]=0;
				strcpy(containerName[inContainer],"");
				strcpy(containerGift[inContainer],"");
				break;
			}

	if (inContainer==0)	// no empty containers
		return;

	containerID[inContainer]  = container;

	containerMap[row][col]		=	inContainer;

	containerDecayTime	[inContainer]	=	TickCount()+(30*3600);

	currentNumberOfContainers++;

	containerEmpty			[inContainer]	=	false;

	containerRow				[inContainer]	= row;

	containerCol				[inContainer]	= col;

	containerGM         [inContainer] = gmSpawn; // carbon 11/20

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
int in_MakeTracks(int row, int col, int tracks)
// ----------------------------------------------------------------- //

{
	int							inTracks		=	tracksMap[row][col];
	int							i;
	//int							j;

	if (containerMap[row][col]!=0) //Tracks override containers
	{
		in_ClearContainer(containerMap[row][col]);
	}

	if (inTracks!=0)
		in_ClearTracks(inTracks);

	if (currentNumberOfTracks>=(kMaxTracks-1))
		return;

	for (i=currentNumberOfTracks;i<kMaxTracks;i++)
		if (tracksID[i]==0)
		{
			//in_ClearTracks(i); // poranoid error checking
			inTracks=i;
			break;
		}

	if (inTracks==0)
		for (i=1;i<currentNumberOfTracks;i++)
			if (tracksID[i]==0)
			{
				//in_ClearTracks(i); // poranoid error checking
				inTracks=i;
				break;
			}

	if (inTracks==0)	// no room for more tracks
	{
		fi_WriteToErrorLog("No more room. Tracks creation aborted");
		return -1;
	}

	tracksID[inTracks]  				= tracks;

	tracksMap[row][col]					= inTracks;
	tracksDecayTime			[inTracks]	= TickCount()+(30*3600);
	tracksRow				[inTracks]	= row;
	tracksCol				[inTracks]	= col;
	currentNumberOfTracks++;
	
	return inTracks;

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
int in_DropProp(int prop, int row, int col)
// ----------------------------------------------------------------- //

{
	int				c;
	int				i;

	if ((row<0) || (row>=kMapWidth) || (col<0) || (col>=kMapHeight))
	{
		fi_WriteToErrorLog("Out of Range - in_DropProp");
		return (0);
	}

	c=containerMap[row][col];

	if (c!=0)
	{
		in_ClearContainer(c);
		containerMap[row][col]=0;
	}

	if (currentNumberOfContainers>=(kMaxContainers-1)) // no empty containers
		return (0);

	for (i=1;i<kMaxContainers;i++)
		if (containerEmpty[i])
		{
			in_ClearContainer(i); // poranoid error checking
			containerID[i]					=	prop;
			containerMap[row][col]	=	i;
			containerDecayTime	[i]	=	TickCount()+(45*3600);
			currentNumberOfContainers++;
			containerEmpty			[i]	=	false;
			containerRow[i]					=	row;
			containerCol[i]					=	col;
			return (0);
		}

	return (0);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
int in_DropItem(int c, int id, int theItem, int count, int row, int col, int decay, TBoolean dead, char name[kStringLength])
// ----------------------------------------------------------------- //

{
	int							inContainer		=	containerMap[row][col];
	int							i;
	int							j;
	int							added					=	0;
	TBoolean				foundGroup		=	false;
	int							itemTypes			=	0;
	int							singleItem		=	0;
	TBoolean        fire          = false;
	TBoolean        permanentChest= false;
	char            debugStr[kStringLength];

	//if ((row<0) || (row>=kMapWidth) || (col<0) || (col>=kMapHeight))			// beta debug
	//	fi_WriteToErrorLog("Out of Range - in_DropItem");

	if ((row<0) || (row>=kMapWidth) || (col<0) || (col>=kMapHeight))
	{
		fi_WriteToErrorLog("Out of Range - in_DropItem");
		return (0);
	}

	if (id==30)
		permanentChest=true;

	if (!dead)
		if ((map[row][col]>=747) && (map[row][col]<=750))	// can't drop in bank
			return (0);

	if (doorway[map[row][col]]) 	// can't drop in doorway
		return (0);

	if (theItem==kRaft)
		if (canSwimIn[map[row][col]])
			return (0);

	if ((theItem<1) || (theItem>kMaxItemsUsed))
	{
		fi_WriteToErrorLog("theItem Out of Range - in_DropItem");
		return (0);
	}

	if (inContainer!=0) // clear gm prop
		if (gmProp[containerID[inContainer]])
		{
			in_ClearContainer(inContainer);
			inContainer=0;
		}

	if (inContainer!=0) // destroyed in fire
		if (containerID[inContainer]==8)
			return (count);

	if (inContainer!=0)
		if (containerID[inContainer]==30)
			permanentChest=true;

	if (inContainer!=0)
		if (containerIsTreasure[containerID[inContainer]])  // update
		{
			if (containerIsCorpse[id])
			{
				if (containerID[inContainer]==55)
					fireDungeonTreasureToSpawn++;

				if (containerID[inContainer]==27)
					dungeonTreasureToSpawn++;

				if (containerID[inContainer]==28)
					sewerTreasureToSpawn++;

				if (containerID[inContainer]==29)
					lizardTreasureToSpawn++;

				if (containerID[inContainer]==32)
					iceTreasureToSpawn++;

				in_ClearContainer(inContainer);

				inContainer=0;
			}
		}

	if (inContainer==0)
	{
		if (currentNumberOfContainers>=(kMaxContainers-1)) // no empty containers
			return (0);

		if ((theItem==kLock) || (theItem==kTrap))
			return (0);

		for (i=currentNumberOfContainers;i<kMaxContainers;i++)
			if (containerEmpty[i])
			{
				in_ClearContainer(i); // poranoid error checking
				inContainer=i;
				containerLock[inContainer]=0;
				containerTrap[inContainer]=0;
				containerMagicLock[inContainer]=0;
				containerMagicTrap[inContainer]=0;

				if (strlen(name)<48)
					strcpy(containerName[inContainer],name);

				if ((theItem==kKindling) && (id==1) && (dead==false) && (canSwimIn[map[row][col]]==false))
					fire=true;

				break;
			}
	}

	if (inContainer==0)
	{
		for (i=1;i<currentNumberOfContainers;i++)
			if (containerEmpty[i])
			{
				in_ClearContainer(i); // poranoid error checking
				inContainer=i;
				containerLock[inContainer]=0;
				containerTrap[inContainer]=0;
				containerMagicLock[inContainer]=0;
				containerMagicTrap[inContainer]=0;

				if (strlen(name)<48)
					strcpy(containerName[inContainer],name);

				if ((theItem==kKindling) && (id==1) && (dead==false) && (canSwimIn[map[row][col]]==false))
					fire=true;

				break;
			}
	}

	if (inContainer==0)	// no empty containers
		return (0);

	if (theItem==kLock)
	{
		if (dead)
			count=1;
		else
		{
			if (containerLock[inContainer]<decay)
				containerLock[inContainer]=decay;

			return (count);
		}
	}

	if (theItem==kTrap)
	{
		if (dead)
			count=1;
		else
		{
			if (containerTrap[inContainer]<decay)
				containerTrap[inContainer]=decay;

			strcpy(containerTrapName[inContainer],"");

			if (c>0)
				if (creature[c].playerIndex!=0)
					if (strlen(player[creature[c].playerIndex].playerNameLC)<15)
						strcpy(containerTrapName[inContainer],player[creature[c].playerIndex].playerNameLC);

			return (count);
		}
	}

	if (!containerIsCorpse[containerID[inContainer]])
		if (containerGM[inContainer]==false)
			containerID[inContainer]=id;

	containerMap				[row][col]		=	inContainer;

	if (in_CanGroup(theItem))
	{
		if (count>kMaxInventoryItemCount)
		{
			fi_WriteToErrorLog("Drop Limit Exceeded");
			tb_IntegerToString(theItem,debugStr);
			fi_WriteToErrorLog(debugStr);
			tb_IntegerToString(count,debugStr);
			fi_WriteToErrorLog(debugStr);
			count=kMaxInventoryItemCount;
		}

		for (j=1;j<kMaxInventorySize;j++)
			if (container[inContainer][j].itemID==theItem)
			{
				foundGroup=true;

				for (i=0;i<count;i++)
					if (container[inContainer][j].itemCount<kMaxInventoryItemCount)
					{
						container[inContainer][j].itemCount++;
						added++;
					}
			}
	}

	if ((added==0) && (foundGroup==false))
		for (j=1;j<kMaxInventorySize;j++)
			if (container[inContainer][j].itemID==0)
			{
				container[inContainer][j].itemID		=	theItem;
				container[inContainer][j].itemCount	=	count;
				added=count;
				break;
			}

	if (added>0)
	{
		if (containerGM[inContainer]==false)
		{
			if (decay==kRandomDecay)
				decay=tb_Rnd(10,30);

			if (decay==0)
				containerDecayTime	[inContainer]	=	0;
			else if ((theItem==kBrackenSeed) && (containerDecayTime[inContainer]==0))
				containerDecayTime	[inContainer]	=	TickCount()+(1*3600);
			else
				containerDecayTime	[inContainer]	=	TickCount()+(decay*3600);
		}

		if (containerEmpty[inContainer])
			currentNumberOfContainers++;

		containerEmpty			[inContainer]	=	false;

		containerRow				[inContainer]	= row;

		containerCol				[inContainer]	= col;
	}

	if ((container[inContainer][0].itemID!=0) || (container[inContainer][0].itemCount!=0))
		itemTypes++;

	for (j=1;j<kMaxInventorySize;j++)
	{
		if ((container[inContainer][j].itemID>kMaxItemsUsed) || (container[inContainer][j].itemCount==0)) // paranoid error checking
		{
			container[inContainer][j].itemID    = 0;
			container[inContainer][j].itemCount = 0;
		}

		if (container[inContainer][j].itemID!=0)
		{
			singleItem=container[inContainer][j].itemID;
			itemTypes++;

			if (itemTypes>=2)
			{
				if ((!((containerIsCorpse[containerID[inContainer]]) || (containerIsTreasure[containerID[inContainer]]))) && (permanentChest==false))
					if (containerGM[inContainer]==false)
						containerID[inContainer]=1;

				break;
			}
		}
	}

	if (((itemTypes==1) && (!containerIsCorpse[containerID[inContainer]])) && (permanentChest==false) && (containerGM[inContainer]==false))
	{
		//Container images
		if (singleItem==kGarlic)								              containerID[inContainer]=6;
		else if (singleItem==kLotusFlower)			              containerID[inContainer]=7;
		else if (singleItem==kMandrakeRoot)			              containerID[inContainer]=9;
		else if (singleItem==kVolcanicAsh)			              containerID[inContainer]=11;
		else if (singleItem==kGinseng)					              containerID[inContainer]=10;
		else if (singleItem==kGuildHallDeed)					      containerID[inContainer]=12;
		else if (singleItem==kHouseDeed)					      containerID[inContainer]=12;
		else if (singleItem==kBrackenSeed)					          containerID[inContainer]=47;
		else if (singleItem==kLockbox)					          containerID[inContainer]=48;
		
		
		else if (singleItem==kRedEgg)					                containerID[inContainer]=62;
		else if (singleItem==kPurpleEgg)					            containerID[inContainer]=63;
		else if (singleItem==kGreenEgg)					              containerID[inContainer]=64;
		else if (singleItem==kBlueEgg)					              containerID[inContainer]=65;
		else if (singleItem==kBlueFlag)					              containerID[inContainer]=67;
		else if (singleItem==kRedFlag)					              containerID[inContainer]=68;
		else if (singleItem==kGreenFlag)					            containerID[inContainer]=69;
		else if (singleItem==kYellowFlag)					            containerID[inContainer]=70;
		else if (singleItem==kNightVisionPotion)					    containerID[inContainer]=72;
		else if (singleItem==kInvisibilityPotion)					    containerID[inContainer]=72;
		else if (singleItem==kOpportunityPotion)					    containerID[inContainer]=72;
		else if (singleItem==kNourishPotion)					    	containerID[inContainer]=72;
		else if (singleItem==kDragonTooth)					            containerID[inContainer]=75;
		else if (singleItem==kSharkFin)						            containerID[inContainer]=75;
		else if (singleItem==kCoal)					           			containerID[inContainer]=76;
		else if (singleItem==kAmber)					            	containerID[inContainer]=77;
		else if (singleItem==kElectricEel)					            containerID[inContainer]=78;
		else if (singleItem==kLog)					            		containerID[inContainer]=79;
		else if (singleItem==kPoisonGland)					            containerID[inContainer]=80;
		else if (singleItem==kEmptyBottle)					            containerID[inContainer]=81;
		else if (singleItem==kLesserHealPotion)					        containerID[inContainer]=83;
		else if (singleItem==kGreaterHealPotion)					    containerID[inContainer]=83;
		else if (singleItem==kFullHealPotion)					        containerID[inContainer]=83;
		else if (singleItem==kWaterBottle)						        containerID[inContainer]=83; //Dave
		else if (singleItem==kElixirTingle)					    		containerID[inContainer]=83;
		else if (singleItem==kManaPotion)					        	containerID[inContainer]=84;
		else if (singleItem==kStrengthPotion)					        containerID[inContainer]=84;
		else if (singleItem==kAgilityPotion)					        containerID[inContainer]=84;
		else if (singleItem==kElevateMindPotion)					    containerID[inContainer]=84;
		else if (singleItem==kElixirZimm)					   			containerID[inContainer]=84;
		else if (singleItem==kLesserDetoxifyPotion)					    containerID[inContainer]=85;
		else if (singleItem==kGreaterDetoxifyPotion)					containerID[inContainer]=85;
		else if (singleItem==kFullDetoxifyPotion)					    containerID[inContainer]=85;
		else if (singleItem==kRedCrystal)					    		containerID[inContainer]=87;
		else if (singleItem==kGreenCrystal)					    		containerID[inContainer]=88;
		else if (singleItem==kOrangeCrystal)					    	containerID[inContainer]=89;
		else if (singleItem==kYellowCrystal)					    	containerID[inContainer]=90;
		else if (singleItem==kBlackCrystal)					    		containerID[inContainer]=91;
		else if (singleItem==kPurpleCrystal)					    	containerID[inContainer]=92;
		else if (singleItem==kBlueCrystal)					    		containerID[inContainer]=93;
		else if (singleItem==kFireCrystal)					    		containerID[inContainer]=94;
		else if (singleItem==kPoison)					    			containerID[inContainer]=95;
		else if (singleItem==kElixirKapow)					 			containerID[inContainer]=95;
		else if (singleItem==kCandy)									containerID[inContainer]=98;
		else if (singleItem==kCurio1)									containerID[inContainer]=99;
		else if (singleItem==kCurio2)									containerID[inContainer]=100;
		else if (singleItem==kCurio3)									containerID[inContainer]=101;
		else if (singleItem==kCurioBig)									containerID[inContainer]=102;
		else if (singleItem==kSpiderEgg)								containerID[inContainer]=102;
		else if (singleItem==kCurioSmall)								containerID[inContainer]=103;
		else if (singleItem==kBody)										containerID[inContainer]=105;
		else if (singleItem==kGoldenBox)								containerID[inContainer]=106;
		else if (singleItem==kPureBlueCrystal)							containerID[inContainer]=107;
		else if (singleItem==kPureRedCrystal)							containerID[inContainer]=108;
		else if (singleItem==kPureYellowCrystal)						containerID[inContainer]=109;
		else if (singleItem==kPureOrangeCrystal)						containerID[inContainer]=110;
		else if (singleItem==kPureGreenCrystal)							containerID[inContainer]=111;
		else if (singleItem==kPurePurpleCrystal)						containerID[inContainer]=112;
		else if (singleItem==kPureFireCrystal)							containerID[inContainer]=113;
		else if (singleItem==kPureIceCrystal)							containerID[inContainer]=114;
		else if (singleItem==kPureBlackCrystal)							containerID[inContainer]=115;
		else if (singleItem==kBrigandHead)								containerID[inContainer]=116;
		else if (singleItem==kNPCPrivateerHead)							containerID[inContainer]=117;
		else if (singleItem==kSorcererHead)								containerID[inContainer]=117;
		else if (singleItem==kDragonEggGreen)							containerID[inContainer]=127;
		else if (singleItem==kDragonEggGold)							containerID[inContainer]=128;
		else if (singleItem==kDragonEggBlack)							containerID[inContainer]=129;
		else if (singleItem==kDragonEggRed)								containerID[inContainer]=130;
		else if (singleItem==kDragonEggBlue)							containerID[inContainer]=131;
		else if (singleItem==kDragonEggPurple)							containerID[inContainer]=132;
		else if (singleItem==kRustyNecklace)							containerID[inContainer]=139;
		else if (singleItem==kNeviaFlower)								containerID[inContainer]=140;
		else if (singleItem==kPureCrystalRing)							containerID[inContainer]=141;
		//Dave
		else if (singleItem==kRedRose)									containerID[inContainer]=143;
		else if (singleItem==kYellowRose)								containerID[inContainer]=144;
		else if (singleItem==kBlueRose)									containerID[inContainer]=145;
		else if (singleItem==kBlackRose)								containerID[inContainer]=146;
		else if (singleItem==kMushroom)									containerID[inContainer]=147;
		else if (singleItem==kRaft)										containerID[inContainer]=148;
		else if (singleItem==kMoldyBook)								containerID[inContainer]=149;
		else if (singleItem==kLogbook)									containerID[inContainer]=149;
		else if (singleItem==kRuinedBook)								containerID[inContainer]=149;
		else if (singleItem==kBookOfTheStars)							containerID[inContainer]=150;
		else if (singleItem==kBookOfBlood)								containerID[inContainer]=150;
		else if (singleItem==kTomeoftheLost)								containerID[inContainer]=150;
		else if (singleItem==kBookofFairyTales)							containerID[inContainer]=150;
		else if (singleItem==kBookOfTheDead)							containerID[inContainer]=151;
		else if (singleItem==kAncientTome)								containerID[inContainer]=152;
		else if (singleItem==kBookOfSky)								containerID[inContainer]=152;
		else if (singleItem==kBookofMoons)								containerID[inContainer]=152;
		else if (singleItem==kRedWine)									containerID[inContainer]=153;
		else if (singleItem==kMaraliWhiskey)							containerID[inContainer]=153;
		else if (singleItem==kRedNatesRotgutRum)						containerID[inContainer]=153;
		else if (singleItem==kElixirHeart)								containerID[inContainer]=153;
		else if (singleItem==kEggNog)									containerID[inContainer]=153;
		else if (singleItem==kBuckeyeBeer)								containerID[inContainer]=154;
		else if (singleItem==kMugOfAle)									containerID[inContainer]=154;
		else if (singleItem==kLerilinCremeale)							containerID[inContainer]=154;
		else if (singleItem==kMead)										containerID[inContainer]=154;
		else if (singleItem==kMirithPaleAle)							containerID[inContainer]=154;
		else if (singleItem==kLobster)									containerID[inContainer]=155;
		else if (singleItem==kSilverLeaf)								containerID[inContainer]=156;
		else if (singleItem==kJinkDoll)									containerID[inContainer]=157;
		else if (singleItem==kVoodooDoll)								containerID[inContainer]=158;
		else if (singleItem==kAndrisChampagne)							containerID[inContainer]=159;
		else if (singleItem==kRedNatesRotgutRum)						containerID[inContainer]=159;
		else if (singleItem==kWhiteWine)								containerID[inContainer]=159;
		else if (singleItem==kFoehanVodka)								containerID[inContainer]=160;
		else if (singleItem==kPortGastMoonshine)						containerID[inContainer]=160;
		else if (singleItem==kHayateSake)								containerID[inContainer]=160;
		else if (singleItem==kBlueMoonGin)								containerID[inContainer]=160;
		else if (singleItem==kWhiteWine)								containerID[inContainer]=160;
		else if (singleItem==kSheepsMilk)								containerID[inContainer]=160;
		else if (singleItem==kElixirDuh)								containerID[inContainer]=160;
		else if (singleItem==kPumpkinPie)								containerID[inContainer]=161;
		else if (singleItem==kPizza)									containerID[inContainer]=161;
		else if (singleItem==kWizardToken)								containerID[inContainer]=164;
		else if (singleItem==kDruidToken)								containerID[inContainer]=165;
		else if (singleItem==kClericToken)								containerID[inContainer]=166;
		else if (singleItem==kFighterToken)								containerID[inContainer]=167;
		else if (singleItem==kRangerToken)								containerID[inContainer]=168;
		else if (singleItem==kRogueToken)								containerID[inContainer]=169;
		else if (singleItem==kTrout)									containerID[inContainer]=170;
		else if (singleItem==kTuna)										containerID[inContainer]=171;
		else if (singleItem==kSeaBass)									containerID[inContainer]=172;
		else if (singleItem==kRingOfStrength)							containerID[inContainer]=173;
		else if (singleItem==kRingOfShadows)							containerID[inContainer]=173;
		else if (singleItem==kManaRing)									containerID[inContainer]=173;
		else if (singleItem==kNeptunesRing)								containerID[inContainer]=173;
		else if (singleItem==kElementalRing)							containerID[inContainer]=174;
		else if (singleItem==kLesserManaRing)							containerID[inContainer]=174;
		else if (singleItem==k15ARRing)									containerID[inContainer]=175;
		else if (singleItem==k10ARRing)									containerID[inContainer]=175;
		else if (singleItem==k5ARRing)									containerID[inContainer]=175;
		else if (singleItem==kRemoveCurseRing)							containerID[inContainer]=176;
		else if (singleItem==kLesserHealRing)							containerID[inContainer]=176;
		else if (singleItem==kGreaterHealRing)							containerID[inContainer]=176;
		else if (singleItem==kLightningBoltRing)						containerID[inContainer]=177;
		else if (singleItem==kRingOfBruteStrength)						containerID[inContainer]=177;
		else if (singleItem==kDiscretionRing)							containerID[inContainer]=177;
		else if (singleItem==kDragonStoneRing)							containerID[inContainer]=178;
		else if (singleItem==kPoisonRing)								containerID[inContainer]=178;
		else if (singleItem==kHadesRing)								containerID[inContainer]=178;
		else if (singleItem==kDruidRing)								containerID[inContainer]=179;
		else if (singleItem==kChaosRing)								containerID[inContainer]=179;
		else if (singleItem==kRingOfLife)								containerID[inContainer]=179;
		else if (singleItem==kRingOfStealth)							containerID[inContainer]=179;
		else if (singleItem==kBananaPie)								containerID[inContainer]=180;
		else if (singleItem==kApplePie)									containerID[inContainer]=181;
		else if (singleItem==kLobsterPie)								containerID[inContainer]=182;
		else if (singleItem==kSkull)									containerID[inContainer]=185;
		else if (singleItem==kEelPie)									containerID[inContainer]=186;
		else if (singleItem==kCrate)									containerID[inContainer]=187;
		else if (singleItem==kIceCrystal)								containerID[inContainer]=190;
		else if (singleItem==kCandle)									containerID[inContainer]=198;
		else if (singleItem==kBirthdayCake)								containerID[inContainer]=199;
		else if (singleItem==kBlackEgg)					                containerID[inContainer]=201;
		else if (singleItem==kCharStatue)					            containerID[inContainer]=204;
		else if (singleItem==kSkeletonKey)					            containerID[inContainer]=205;
		else if (singleItem==kSmallKey)					           		containerID[inContainer]=206;
		else if (singleItem==kRustyKey)					            	containerID[inContainer]=207;
		else if (singleItem==kBronzeKey)					            containerID[inContainer]=207;
		else if (singleItem==kIronKey)					            	containerID[inContainer]=209;
		
		else if (singleItem==kInfusedRed)					    		containerID[inContainer]=211;
		else if (singleItem==kInfusedGreen)					    		containerID[inContainer]=212;
		else if (singleItem==kInfusedOrange)					    	containerID[inContainer]=213;
		else if (singleItem==kInfusedYellow)					    	containerID[inContainer]=214;
		else if (singleItem==kInfusedBlack)					    		containerID[inContainer]=215;
		else if (singleItem==kInfusedPurple)					    	containerID[inContainer]=216;
		else if (singleItem==kInfusedBlue)					    		containerID[inContainer]=217;
		else if (singleItem==kInfusedFire)					    		containerID[inContainer]=218;
		else if (singleItem==kInfusedPBlue)							containerID[inContainer]=219;
		else if (singleItem==kInfusedPRed)							containerID[inContainer]=220;
		else if (singleItem==kInfusedPYellow)						containerID[inContainer]=221;
		else if (singleItem==kInfusedPOrange)						containerID[inContainer]=222;
		else if (singleItem==kInfusedPGreen)							containerID[inContainer]=223;
		else if (singleItem==kInfusedPPurple)						containerID[inContainer]=224;
		//else if (singleItem==kInfusedPFire)							containerID[inContainer]=225;
		//else if (singleItem==kInfusedPIce)							containerID[inContainer]=226;
		else if (singleItem==kInfusedPBlack)							containerID[inContainer]=227;
		else if (singleItem==kPurpleRose)								containerID[inContainer]=230;
		
		
		else if ((singleItem>=4000) && (singleItem<5000))		 		containerID[inContainer]=12;
	}

	if (fire)
	{
		containerID[inContainer]=8;
		containerDecayTime[inContainer]	=	TickCount()+(5400);
	}

	if (permanentChest)
		containerID[inContainer]=30; 

	return (added);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
int in_BankItem(int i, int theItem, int count)
// ----------------------------------------------------------------- //

{
	int							j;
	int							k;
	int							added=0;
	TBoolean				foundGroup=false;


	if ((theItem<1) || (theItem>kMaxItemsUsed)) // paranoid error checking
		return (0);

	if (in_CanGroup(theItem))
	{
		for (j=1;j<kMaxInventorySize;j++)
			if (player[i].bank[j].itemID==theItem)
			{
				foundGroup=true;

				for (k=0;k<count;k++)
					if (player[i].bank[j].itemCount<kMaxInventoryItemCount)
					{
						player[i].bank[j].itemCount++;
						added++;
					}
			}
	}

	if ((added==0) && (foundGroup==false))
		for (j=1;j<kMaxInventorySize;j++)
			if (player[i].bank[j].itemID==0)
			{
				player[i].bank[j].itemID		=	theItem;
				player[i].bank[j].itemCount	=	count;
				added=count;
				break;
			}

	return (added);

}

#endif

// ----------------------------------------------------------------- //
UInt32 in_CanAddGoldtoInventory_c(UInt32 gold)
// ----------------------------------------------------------------- //

{
	//int						gold1;
	//int						gold2;
	UInt32				added=gold;

	if ((gold+in_GetGoldTotal(localInventory[0].itemID, localInventory[0].itemCount))>kMaxGold)
		added=kMaxGold-in_GetGoldTotal(localInventory[0].itemID, localInventory[0].itemCount);

	return (added);

}

// ----------------------------------------------------------------- //
int in_CanAddtoInventory_c(int item, int count) // client
// ----------------------------------------------------------------- //

{
	int						j;

	if ((item==0) || (count==0))
		return (0);
	
	if (itemList[item].itemType==kSpellType || itemList[item].itemType==kSpecialAttackType)
		return (true);

	if (in_CanGroup(item))
		for (j=1;j<kMaxInventorySize;j++)
			if (localInventory[j].itemID==item)
			{
				int numberAdded=count;

				if (localInventory[j].itemCount+numberAdded>kMaxInventoryItemCount)
					numberAdded=kMaxInventoryItemCount-localInventory[j].itemCount;

				return (numberAdded);
			}

	for (j=1;j<kMaxInventorySize;j++)
		if (localInventory[j].itemID==0)
		{
			if (in_CanGroup(item))
				return (count);
			else
				return (1);
		}

	return (0);

}

// ----------------------------------------------------------------- //
void in_ClearLocalInventory(void)
// ----------------------------------------------------------------- //

{
	int					i;

	for (i=0;i<kMaxInventorySize;i++)
	{
		localInventory[i].itemID		=	0;
		localInventory[i].itemCount	=	0;
		inventorySlotArmed[i]				=	false;
	}

}


// ----------------------------------------------------------------- //
void in_ClearPickUpPack(void)
// ----------------------------------------------------------------- //

{
	int					i;

	strcpy(localContainerName,"");

	for (i=0;i<kMaxInventorySize;i++)
	{
		pickUpInventory[i].itemID			=	0;
		pickUpInventory[i].itemCount	=	0;
	}

}

// ----------------------------------------------------------------- //
void in_ClearBank(void)
// ----------------------------------------------------------------- //

{
	int					i;

	for (i=0;i<kMaxInventorySize;i++)
	{
		bankInventory[i].itemID			=	0;
		bankInventory[i].itemCount	=	0;
	}

}


// ----------------------------------------------------------------- //
int in_GetEmptyContainer(void)
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	int             i;

	for (i=1;i<kMaxContainers;i++)
		if (containerEmpty[i])
		{
			in_ClearContainer(i); // poranoid error checking
			containerLock[i]=0;
			containerTrap[i]=0;
			containerMagicLock[i]=0;
			containerMagicTrap[i]=0;
			strcpy(containerName[i],"");
			strcpy(containerGift[i],"");
			strcpy(containerTrapName[i],"");
			strcpy(containerMagicTrapName[i],"");
			return (i);
			break;
		}

	return (-1);

#endif
}

// ----------------------------------------------------------------- //
void in_InitializeContainers(void)
// ----------------------------------------------------------------- //

{
	int								i;
	int								j;
	int								x;
	int								y;
	//int								r; // beta
	//int								c;
	//int								a;
	//int               rare;

	in_ClearLocalInventory();
	in_ClearBank();
	in_ClearPickUpPack();
	strcpy(localContainerName,"");

	for (i=0;i<kMaxInventorySize;i++)
	{
		makeList[i].itemID=0;
		makeList[i].itemCount=0;
	}

#ifdef _SERVER

	for (x=0;x<kMapWidth;x++)
		for (y=0;y<kMapHeight;y++)
			containerMap[x][y]=0;

	for (i=0;i<kMaxContainers;i++)
	{
		for (j=0;j<kMaxInventorySize;j++)
		{
			container[i][j].itemID		=	0;
			container[i][j].itemCount	=	0;
		}

		containerID[i]				= 0;

		containerEmpty[i]			=	true;
		containerDecayTime[i]	=	0;
		containerRow[i]				=	0;
		containerCol[i]				=	0;
		containerLock[i]			=	0;
		containerTrap[i]      = 0;
		containerMagicLock[i]	=	0;
		containerMagicTrap[i]	=	0;
		containerHidden[i]	  =	false;
		strcpy(containerName[i],"");
		strcpy(containerGift[i],"");
		strcpy(containerTrapName[i],"");
		strcpy(containerMagicTrapName[i],"");
	}

	for (i=0;i<kMaxTracks;i++)
	{
		tracksID[i]				= 0;
		tracksDecayTime[i]	=	0;
		tracksRow[i]				=	0;
		tracksCol[i]				=	0;
		strcpy(tracksNote[i],"");
		strcpy(tracksGift[i],"");
	}

	currentNumberOfContainers=1;

	currentNumberOfTracks=1;

	//--------------------------------------------------------------

	//fi_LoadAllContainers(); // mod 8/12

	//--------------------------------------------------------------

	// version 0.1.2 - half all reagent spawns
	ginsengSpawn      = 1000*4;//1000*4
	lotusFlowerSpawn  = 450*5;//450*5
	mandrakeRootSpawn = 1200*3;//1200*3
	garlicSpawn       = 1300*3;//1300*3
	volcanicAshSpawn  = 400*12;//400*12
	mushroomSpawn  = 400*5; //Dave 1400*5
	//eggSpawn = 500*5;
	treasureSpawnTime = 0;


	/*
	  i=0;
	  while (i<4000)
	  	{
	  		x=tb_Rnd(500,3000);
	  		y=tb_Rnd(500,3000);
	  		if ((map[x][y]>=256) && (map[x][y]<=263))
	  	    if (gr_SnowAt(x, y)==false)
					{
						in_DropItem(10,kGinseng, 1, x, y, kRandomDecay, false);
						i++;
					}
	  	}

	  i=0;
	  while (i<2000)
	  	{
	  		x=tb_Rnd(500,3000);
	  		y=tb_Rnd(500,3000);
	  		if ((map[x][y]>=256) && (map[x][y]<=263))
	  	    if (gr_SnowAt(x, y)==false)
					{
						in_DropItem(7,kLotusFlower, 1, x, y, kRandomDecay, false);
						i++;
					}
	  	}

	  i=0;
	  while (i<4000)
	  	{
	  		x=tb_Rnd(500,3000);
	  		y=tb_Rnd(500,3000);
	  		if ((map[x][y]>=256) && (map[x][y]<=263))
	  	    if (gr_SnowAt(x, y)==false)
					{
						in_DropItem(9,kMandrakeRoot, 1, x, y, kRandomDecay, false);
						i++;
					}
	  	}

	  i=0;
	  while (i<2500)
	  	{
	  		x=tb_Rnd(500,3000);
	  		y=tb_Rnd(500,3000);
	  		if ((map[x][y]>=256) && (map[x][y]<=263))
	  	    if (gr_SnowAt(x, y)==false)
					{
						in_DropItem(8,kAmber, 1, x, y, kRandomDecay, false);
						i++;
					}
	  	}

	  i=0;
	  while (i<5000)
	  	{
	  		x=tb_Rnd(500,3000);
	  		y=tb_Rnd(500,3000);
	  		if ((map[x][y]>=256) && (map[x][y]<=263))
	  	    if (gr_SnowAt(x, y)==false)
					{
						in_DropItem(6,kGarlic, 1, x, y, kRandomDecay, false);
						i++;
					}
	  	}

	  i=0;
	  while (i<1000)
	  	{
	  		x=tb_Rnd(500,3000);
	  		y=tb_Rnd(500,3000);
	  		if ((map[x][y]>=256) && (map[x][y]<=263))
	  	    if (gr_SnowAt(x, y)==false)
					{
						in_DropItem(11,kVolcanicAsh, 1, x, y, kRandomDecay, false);
						i++;
					}
	  	}

	  i=0;
	  while (i<2000)
	  	{
	  		x=tb_Rnd(500,3000);
	  		y=tb_Rnd(500,3000);
	  		if ((map[x][y]>=256) && (map[x][y]<=263))
	  	    if (gr_SnowAt(x, y)==false)
					{
						in_DropItem(12,kMoonseed, 1, x, y, kRandomDecay, false);
						i++;
					}
	  	}
	*/

	/*
		i=0;
		while (i<10000)
			{
				x=tb_Rnd(1500,2500);
				y=tb_Rnd(1500,2500);
				if (canWalkOn[map[x][y]])
					{
					//	in_DropItem(1,kFeather, 230, x, y, kThreeMinuteDecay*1000, false,"");
					//	in_DropItem(1,kShaft, 230, x, y, kThreeMinuteDecay*1000, false);

					//	in_DropItem(1,kAgilityPotion, 10, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kElevateMindPotion, 10, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kStrengthPotion, 10, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kCrystalRing, 1, x, y, kThreeMinuteDecay*1000, false,"");
					//	in_DropItem(1,kWoodenShield, 255, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kBlackClosedPlateHelmet, 255, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kLeatherBelt, 1, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kSextant, 1, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kRaft, 1, x, y, kThreeMinuteDecay*1000, false);


				//		in_DropItem(1,kOpenPlateHelmet, 127, x, y, kThreeMinuteDecay*1000, false,"");
				//		in_DropItem(1,kLegsPlate, 255, x, y, kThreeMinuteDecay*1000, false);

				//		in_DropItem(1,kBreastChain, 255, x, y, kThreeMinuteDecay*1000, false,"");
				//		in_DropItem(1,kChainLegs, 255, x, y, kThreeMinuteDecay*1000, false);

				//		in_DropItem(1,kBlackBreastPlate, 255, x, y, kThreeMinuteDecay*1000, false);
				//		in_DropItem(1,kBlackLegsPlate, 255, x, y, kThreeMinuteDecay*1000, false);

				//		in_DropItem(1,kBrownBreastLeather, 255, x, y, kThreeMinuteDecay*1000, false);
				//		in_DropItem(1,kBrownLeatherLegs, 255, x, y, kThreeMinuteDecay*1000, false);

				//		in_DropItem(1,kBlackPlateGauntlets, 255, x, y, kThreeMinuteDecay*1000, false);
				//		in_DropItem(1,kChainGauntlets, 255, x, y, kThreeMinuteDecay*1000, false);
				//		in_DropItem(1,kPlateGauntlets, 255, x, y, kThreeMinuteDecay*1000, false);

				//		in_DropItem(1,kChainArms, 255, x, y, kThreeMinuteDecay*1000, false);
				//		in_DropItem(1,kPlateArms, 255, x, y, kThreeMinuteDecay*1000, false);
				//		in_DropItem(1,kBlackPlateArms, 255, x, y, kThreeMinuteDecay*1000, false);
				//		in_DropItem(1,kBrownLeatherArms, 255, x, y, kThreeMinuteDecay*1000, false);

					//	in_DropItem(1,kBoard, 50, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kFerriteOre, 50, x, y, kThreeMinuteDecay*1000, false);

						in_DropItem(1,4000, 1, x, y, kThreeMinuteDecay*1000, false,"");
						in_DropItem(1,kKatana, 50, x, y, kThreeMinuteDecay*1000, false,"");
						in_DropItem(1,kDragonTooth, 1000, x, y, kThreeMinuteDecay*1000, false,"");
						in_DropItem(1,kEmptyBottle, 100, x, y, kThreeMinuteDecay*1000, false,"");
						in_DropItem(1,kMandrakeRoot, 1000, x, y, kThreeMinuteDecay*1000, false,"");
						in_DropItem(1,kAmber, 10000, x, y, kThreeMinuteDecay*1000, false,"");
						in_DropItem(1,kGarlic, 15000, x, y, kThreeMinuteDecay*1000, false,"");
						in_DropItem(1,kVolcanicAsh, 20000, x, y, kThreeMinuteDecay*1000, false,"");
					  in_DropItem(1,kPoisonGland, 30000, x, y, kThreeMinuteDecay*1000, false,"");
						in_DropItem(1,kCoal, 40000, x, y, kThreeMinuteDecay*1000, false,"");
				  	in_DropItem(1,kElectricEel, 50000, x, y, kThreeMinuteDecay*1000, false,"");
				  //	in_DropItem(1,kLog, 250, x, y, kThreeMinuteDecay*1000, false);

				  //	in_DropItem(1,kWoodworkingTools, 5, x, y, kThreeMinuteDecay*1000, false,"");
				  //	in_DropItem(1,kBoard, 100, x, y, kThreeMinuteDecay*1000, false,"");
				  //	in_DropItem(1,kShears, 15, x, y, kThreeMinuteDecay*1000, false);
				  //	in_DropItem(1,kSextant, 64, x, y, kThreeMinuteDecay*1000, false,"");
				  //	in_DropItem(1,kFerriteOre, 100, x, y, kThreeMinuteDecay*1000, false);
				  //	in_DropItem(1,kKatanaDurability, 127, x, y, kThreeMinuteDecay*1000, false,"");

				  	//in_DropItem(1,kTrap, 100, x, y, kThreeMinuteDecay*1000, false);

				  	//in_DropItem(1,kKatanaSilver, 127, x, y, kThreeMinuteDecay*1000, false);
				  	//in_DropItem(1,kKatanaPoison, 127, x, y, kThreeMinuteDecay*1000, false);

					//	in_DropItem(1,kEmptyBottle, 50, x, y, kThreeMinuteDecay*1000, false);

					//	in_DropItem(1,kRedDye, 50, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kBlueDye, 50, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kBrownDye, 50, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kBlackDye, 50, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kGre}ye, 50, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kPurpleDye, 50, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kYellowDye, 50, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kLeatherHide, 50, x, y, kThreeMinuteDecay*1000, false);

					//	in_DropItem(1,kRollOfCloth, 50, x, y, kThreeMinuteDecay*1000, false);
					//	in_DropItem(1,kLeatherHide, 50, x, y, kThreeMinuteDecay*1000, false);

				//		c=tb_Rnd(1,4);
					//	for (a=0;a<c;a++)
					//		{
					//			do
					//				{ r=tb_Rnd(1,kMaxItems); }
					//			while ((itemList[r].itemType==0) || (itemList[r].itemType==kSpellType));
					//			if (!in_CanGroup(r))
					//				in_DropItem(1,r, 127, x, y, kTenMinuteDecay*1000, false);
					//			else
					//				in_DropItem(1,r, 1, x, y, kThreeMinuteDecay*1000, false);
								in_DropGold(1,10000, x, y, kThreeMinuteDecay*1000, false,"");
								i++;
					//		}
					}
			}
	*/

	/*
	  x=0;
	  while (x<100)  // fire dungeon spawn
	    {
	      r=tb_Rnd(449,576);
	      c=tb_Rnd(70,208);
	      if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
	        {
	          x++;
					  in_DropGold(55,tb_Rnd(15,60), r, c, 0, false,"");

	      		if (tb_Rnd(1,5)==1) in_DropItem(0,55,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,55,kLoafOfBread, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,55,kCrystalRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,55,kPlatinumRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,55,kGoldRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,55,kSilverRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,55,kRedDye, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,55,kDiamond, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,55,kRuby, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,55,kPoisonGland,tb_Rnd(1,5), r, c, 0, false, "");// put in carbon 11/24

					  if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackOpenPlateHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackBreastPlate, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackLegsPlate, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackPlateArms, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackPlateGauntlets, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackSlottedPlateHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackClosedPlateHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,55,kBlackVikingHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,300)==1) in_DropItem(0,55,kDragonTooth, tb_Rnd(1,5), r, c, 0, false, ""); // put in carbon 11/24

					  if (tb_Rnd(1,300)==1)
					    {
	    				  rare=it_GetRare(0);
	    				  if (rare>0)
	    				    if (in_CanGroup(rare))
	    				      in_DropItem(0,55,rare, 1, r, c, 0, false, "");
	    				    else
	    				      in_DropItem(0,55,rare, 127, r, c, 0, false, "");
					    }

					  if (tb_Rnd(1,2)==1)
	    			  in_DropItem(0,55,kLock, 1, r, c, tb_Rnd(2,4), false, "");
	    			else
	    			  in_DropItem(0,55,kLock, 1, r, c, tb_Rnd(3,7), false, "");
	          if (tb_Rnd(1,50)==1)
	  				  if (tb_Rnd(1,10)<10)
	      			  in_DropItem(0,55,kTrap, 1, r, c, tb_Rnd(1,30), false, "");
	      			else
	      			  in_DropItem(0,55,kTrap, 1, r, c, tb_Rnd(1,99), false, "");
	        }
	    }

	  x=0;
	  while (x<50)  // dungeon spawn
	    {
	      r=tb_Rnd(42,90);
	      c=tb_Rnd(54,227);
	      if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
	        {
	          x++;
					  in_DropGold(27,tb_Rnd(5,20), r, c, 0, false,"");

	      		if (tb_Rnd(1,5)==1) in_DropItem(0,27,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,27,kLoafOfBread, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,27,kCrystalRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,27,kPlatinumRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,27,kGoldRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,27,kSilverRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,27,kRedDye, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,27,kDiamond, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,27,kRuby, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,27,kPoisonGland, tb_Rnd(1,5), r, c, 0, false, "");// put in carbon 11/24

					  if (tb_Rnd(1,500)==1) in_DropItem(0,27,kBlackOpenPlateHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,27,kBlackBreastPlate, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,27,kBlackLegsPlate, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,27,kBlackPlateArms, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,27,kBlackPlateGauntlets, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,27,kBlackSlottedPlateHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,27,kBlackClosedPlateHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,27,kBlackVikingHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,27,kDragonTooth, tb_Rnd(1,5), r, c, 0, false, "");// put in carbon 11/24

					  if (tb_Rnd(1,500)==1)
					    {
	    				  rare=it_GetRare(0);
	    				  if (rare>0)
	    				    if (in_CanGroup(rare))
	    				      in_DropItem(0,27,rare, 1, r, c, 0, false, "");
	    				    else
	    				      in_DropItem(0,27,rare, 127, r, c, 0, false, "");
					    }

					  if (tb_Rnd(1,2)==1)
	    			  in_DropItem(0,27,kLock, 1, r, c, tb_Rnd(1,4), false, "");
	    			else
	    			  in_DropItem(0,27,kLock, 1, r, c, tb_Rnd(1,8), false, "");
	          if (tb_Rnd(1,50)==1)
	  				  if (tb_Rnd(1,10)<10)
	      			  in_DropItem(0,27,kTrap, 1, r, c, tb_Rnd(1,20), false, "");
	      			else
	      			  in_DropItem(0,27,kTrap, 1, r, c, tb_Rnd(1,99), false, "");
	        }
	    }

	  x=0;
	  while (x<40)  // sewer spawn
	    {
	      r=tb_Rnd(105,187);
	      c=tb_Rnd(54,128);
	      if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
	        {
	          x++;
					  in_DropGold(28,tb_Rnd(1,8), r, c, 0, false,"");

	      		if (tb_Rnd(1,10)==1) in_DropItem(0,28,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,28,kLoafOfBread, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,28,kLesserHealPotion, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,28,kManaPotion, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,28,kPoisonGland, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,28,kBlueBoots, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,28,kLesserDetoxifyPotion, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,28,kStrengthPotion, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,28,kNightVisionPotion, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,28,kKatana, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,28,kBroadSword, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,28,kScimitar, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,28,kPurpleDye, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,28,kBlueDye, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,28,kDragonTooth, 1, r, c, 0, false, "");

	    			in_DropItem(0,28,kLock, 1, r, c, tb_Rnd(1,2), false, "");
	          if (tb_Rnd(1,50)==1)
	    			  in_DropItem(0,28,kTrap, 1, r, c, tb_Rnd(1,5), false, "");
	        }
	    }

	  x=0;
	  while (x<15)  // lizard strong hold spawn
	    {
	      r=tb_Rnd(1939,1963);
	      c=tb_Rnd(1922,1944);
	      if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
	        {
	          x++;
					  in_DropGold(29,tb_Rnd(10,45), r, c, 0, false,"");

	      		if (tb_Rnd(1,5)==1) in_DropItem(0,29,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,29,kLoafOfBread, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,29,kCrystalRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,29,kPlatinumRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,29,kGoldRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,29,kSilverRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,29,kRedDye, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,29,kDiamond, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1)  in_DropItem(0,29,kPoisonGland, 1, r, c, 0, false, "");
	          if (tb_Rnd(1,8)==1)   in_DropItem(0,29,kQueensRuby, 1, r, c, 0, false, "");
	          if (tb_Rnd(1,500)==1)  in_DropItem(0,29,kDragonTooth, 1, r, c, 0, false, "");

					  if (tb_Rnd(1,750)==1)
					    {
	    				  rare=it_GetRare(0);
	    				  if (rare>0)
	    				    if (in_CanGroup(rare))
	    				      in_DropItem(0,29,rare, 1, r, c, 0, false, "");
	    				    else
	    				      in_DropItem(0,29,rare, 127, r, c, 0, false, "");
					    }

					  if (tb_Rnd(1,2)==1)
	    			  in_DropItem(0,29,kLock, 1, r, c, tb_Rnd(1,3), false, "");
	    			else
	    			  in_DropItem(0,29,kLock, 1, r, c, tb_Rnd(1,5), false, "");
	          if (tb_Rnd(1,25)==1)
	  				  if (tb_Rnd(1,10)<10)
	      			  in_DropItem(0,29,kTrap, 1, r, c, tb_Rnd(1,20), false, "");
	      			else
	      			  in_DropItem(0,29,kTrap, 1, r, c, tb_Rnd(1,99), false, "");
	        }
	    }

	  x=0;
	  while (x<200)  // ice dungeon spawn
	    {
	      r=tb_Rnd(200,329);
	      c=tb_Rnd(61,196);
	      if ((canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (containerMap[r][c]==0)) // update
	        {
	          x++;
					  in_DropGold(32,tb_Rnd(5,20), r, c, 0, false,"");

	      		if (tb_Rnd(1,5)==1) in_DropItem(0,32,qu_GetQuestForSpawn(), 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,32,kLoafOfBread, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,32,kCrystalRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,32,kPlatinumRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,32,kGoldRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,32,kSilverRing, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,32,kRedDye, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,32,kDiamond, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,100)==1) in_DropItem(0,32,kRuby, 1, r, c, 0, false, "");
					  if (tb_Rnd(1,50)==1) in_DropItem(0,32,kPoisonGland, tb_Rnd(1,5), r, c, 0, false, "");// put in carbon 11/24

					  if (tb_Rnd(1,500)==1) in_DropItem(0,32,kBlackOpenPlateHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,32,kBlackBreastPlate, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,32,kBlackLegsPlate, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,32,kBlackPlateArms, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,32,kBlackPlateGauntlets, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,32,kBlackSlottedPlateHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,32,kBlackClosedPlateHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,32,kBlackVikingHelmet, 127, r, c, 0, false, "");
					  if (tb_Rnd(1,500)==1) in_DropItem(0,32,kDragonTooth, tb_Rnd(1,5), r, c, 0, false, "");// put in carbon 11/24

					  if (tb_Rnd(1,100)==1) in_DropItem(0,32,kIceCrystal, 1, r, c, 0, false, "");

					  if (tb_Rnd(1,500)==1)
					    {
	    				  rare=it_GetRare(0);
	    				  if (rare>0)
	    				    if (in_CanGroup(rare))
	    				      in_DropItem(0,32,rare, 1, r, c, 0, false, "");
	    				    else
	    				      in_DropItem(0,32,rare, 127, r, c, 0, false, "");
					    }

					  if (tb_Rnd(1,2)==1)
	    			  in_DropItem(0,32,kLock, 1, r, c, tb_Rnd(3,6), false, "");
	    			else
	    			  in_DropItem(0,32,kLock, 1, r, c, tb_Rnd(6,10), false, "");

	          if (tb_Rnd(1,50)==1)
	  				  if (tb_Rnd(1,10)<10)
	      			  in_DropItem(0,32,kTrap, 1, r, c, tb_Rnd(1,20), false, "");
	      			else
	      			  in_DropItem(0,32,kTrap, 1, r, c, tb_Rnd(1,99), false, "");
	        }
	    }
	*/
	//for (i=0;i<25;i++)
	//  qu_SpawnQuest();

#endif

}

// ----------------------------------------------------------------- //
TBoolean in_CanGroup(int item)
// ----------------------------------------------------------------- //

{

	if (item==0)									return (false); //018

	if (it_IsQuest(item))		  					return (false);

	if (it_IsWeapon(item))							return (false);

	if (it_IsArmor(item, false))					return (false);
	
	//Dave kMountType
	
	if (itemList[item].itemType==kMountType) 		return (false);

	if (itemList[item].itemType==kMagicRobeType) 	return (false);

	if (itemList[item].itemType==kLesserRingType)	return (false);
	/*
	{
		if (item==kRingOfStrength ||
			item==kRingOfBruteStrength ||
			item==kDiscretionRing ||
			item==kRingOfShadows ||
			item==kRingOfStealth ||
			item==kManaRing  ||
			item==kGMHorse  ||
			item==kFastHorse
		)
			return false;//Dave  was true
			  
		else
			return false;
	}
*/
	return (true);
}
