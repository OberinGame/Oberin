/*#include "Toolbox.h"
#include "Items.h"
#include "Inventory.h"
#include "Creatures.h"
#include "Players.h"
#include "Graphics.h"
#include "Resource.h"
#include "Skills.h"
#include "FileIO.h"
#include "Graphics.h"
#include "Players.h"
#include "Items.h"
*/
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

itemType						itemList						[kMaxItems];
UInt16						  rareItem						[kMaxRareItems];
char 								makeSuccessMessage	[kStringLength];//[128];
char 								makeFailedMessage		[kStringLength];//[128];

int                 rareItemPtr;

extern playerLocalSkillsType				localSkills				[kMaxSkills];
extern int													targetMode;
extern UInt8												localRightHand;
extern UInt8												localLeftHand;
extern UInt8												localLegs;
extern UInt8												localTorso;
extern UInt8												localFeet;
extern UInt8												localArms;
extern UInt8												localRightFinger;
extern UInt8												localLeftFinger;
extern UInt8												localHead;
extern UInt8												localNeck;
extern UInt8												localHands;
extern UInt8												localWaist;
extern UInt8												localMount;
extern UInt8				localClass;

extern UInt8				currentCycle;
extern int					localSpat;

extern int          		maxSkillLevel[7][kMaxSkills];
extern TBoolean						questChar[kMaxScreenElements];
extern TBoolean						fullMoon;
//Dave
extern creatureType									creature					[kMaxCreatures];

#ifdef _SERVER
// ----------------------------------------------------------------- //
void it_SuccessfulMake(int i) // server
// ----------------------------------------------------------------- //

{
	if (player[i].gatherType!=0)
		nw_SendDisplayMessage(i,kSuccessfulMake);

	player[i].gatherType= 0;

	player[i].gathering	=	0;

	player[i].gatherRow	=	0;

	player[i].gatherCol	=	0;

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void it_FailedMake(int i) // server
// ----------------------------------------------------------------- //

{

	in_DamageToolInHand(i);

	if (player[i].gatherType!=0)
		nw_SendDisplayMessage(i,kFailedToMake);

	player[i].gatherType= 0;

	player[i].gathering	=	0;

	player[i].gatherRow	=	0;

	player[i].gatherCol	=	0;

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void it_MakeItem(int i, int item)
// ----------------------------------------------------------------- //

{
	int							j;
	int							k;
	int							makeCount;
	TBoolean				success=false;
	int							canMake=0;
	TBoolean				failed;
	int							count;
	int							p;
	int             condition=-1;

	//if (item>=kMaxItems)			// beta debug
	//	fi_WriteToErrorLog("Out of Range - it_MakeItem");

	if ((itemList[item].makeSkill==kAlchemyDruid) || (itemList[item].makeSkill==kAlchemyCleric) || (itemList[item].makeSkill==kPoisoning))
	{
		if (itemList[item].makeMana>creature[player[i].creatureIndex].magicPoints)
		{
			nw_SendDisplayMessage(i,kNoManaForPotion);
			return;
		}
	}
	
	if (itemList[item].makeSkill==kEnchanting)
	{
		if (itemList[item].makeMana>creature[player[i].creatureIndex].magicPoints)
		{
			nw_SendDisplayMessage(i,kNoManaForEnchant);
			return;
		}
	}

	//--------------------------------------------------------

	count=1;

	if ((item==kArrow) || (item==kBolt))
		count=20;

	for (p=0;p<count;p++)
	{

		failed=false;

		for (j=0;j<5;j++)
			if (itemList[item].ingredientCount[j]>0)
			{
				if (in_QuantityOf_s(player[i].creatureIndex,itemList[item].ingredientID[j])<itemList[item].ingredientCount[j])	// no ingredients
				{
					failed=true;
					break;
				}
			}

		if (failed)
		{
			if (canMake==0)
			{
				it_FailedMake(i);
				return;
			}

			break;
		}

		canMake++;

		for (j=0;j<5;j++)
			if (itemList[item].ingredientCount[j]>0)
				for (k=1;k<kMaxInventorySize;k++)
					if (creature[player[i].creatureIndex].inventory[k].itemID==itemList[item].ingredientID[j])
					{
						if (itemList[item].makeSkill==kPoisoning && !in_CanGroup(creature[player[i].creatureIndex].inventory[k].itemID))
							condition=creature[player[i].creatureIndex].inventory[k].itemCount;

						in_RemoveFromInventorySlot(player[i].creatureIndex,k,itemList[item].ingredientCount[j]);

						break;
					}
					
		

	}

	if (itemList[item].makeSkill==kBlacksmithing)							success=sk_CraftingSkill(kBlacksmithing, 						i, item, true, false, true);

	if (itemList[item].makeSkill==kWoodworking)								success=sk_CraftingSkill(kWoodworking, 							i, item, true, false, true);

	if (itemList[item].makeSkill==kAlchemyCleric)							success=sk_CraftingSkill(kAlchemyCleric, 						i, item, false, true, true);

	if (itemList[item].makeSkill==kAlchemyDruid)							success=sk_CraftingSkill(kAlchemyDruid, 						i, item, false, true, true);

	if (itemList[item].makeSkill==kEnchanting)								success=sk_CraftingSkill(kEnchanting,							i, item, false, true, true);
	
	if (itemList[item].makeSkill==kPoisoning)							success=sk_CraftingSkill(kPoisoning, 						i, item, false, true, true);

	if (itemList[item].makeSkill==kCooking)										success=sk_CraftingSkill(kCooking, 									i, item, false, true, false);

	if (itemList[item].makeSkill==kTailoringLeatherworking)		success=sk_CraftingSkill(kTailoringLeatherworking, 	i, item, false, true, true);

	if (itemList[item].makeSkill==kTinkering)									success=sk_CraftingSkill(kTinkering, 								i, item, false, true, true);

	if (success)
	{
		if ((itemList[item].makeSkill==kAlchemyDruid) || (itemList[item].makeSkill==kAlchemyCleric) || (itemList[item].makeSkill==kPoisoning) || (itemList[item].makeSkill==kEnchanting))
		{
			if (itemList[item].makeMana>creature[player[i].creatureIndex].magicPoints)
				creature[player[i].creatureIndex].magicPoints=0;
			else
				creature[player[i].creatureIndex].magicPoints=creature[player[i].creatureIndex].magicPoints-itemList[item].makeMana;

			player[i].sendStatsRefresh=true;
		}

		if (in_CanGroup(item))
			makeCount=itemList[item].makeCount;
		else
		{
			if (condition<0)
			{
				if (sk_GetSkillLevel(player[i].skill[itemList[item].makeSkill].level)>=(itemList[item].makeSkillLevel*2))  //Dave removed Enchanting from non GM list, original line below
				//if (itemList[item].makeSkill!=kEnchanting && sk_GetSkillLevel(player[i].skill[itemList[item].makeSkill].level)>=(itemList[item].makeSkillLevel*2))
					//if (player[i].skill[itemList[item].makeSkill].level==100)
					makeCount=255;	// GM
				else
					makeCount=127;
			}
			else
				makeCount=condition;
		}

		if (in_CanGroup(item))  //018
			makeCount=makeCount*canMake;
		
		if (in_AddtoInventory(player[i].creatureIndex, item, makeCount)==0)
		{
			nw_SendDisplayMessage(i,kYourPackFull);
			it_FailedMake(i);
		}
		else
			it_SuccessfulMake(i);
	}
	else
		it_FailedMake(i);

	nw_SendInventoryRefresh(i);

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
UInt8 it_GetMountSpeed(int i) // server
// ----------------------------------------------------------------- //

{
	if (i==kSickHorse)			return(3);
	if (i==kHorse)				return(5);
	if (i==kFastHorse)			return(8);
	if (i==kVeryFastHorse)		return(11);
	if (i==kMagicMount1)		return(13);
	if (i==kMagicMount1)		return(13);
	if (i==kMagicMount1)		return(13);
	if (i==kMagicMount1)		return(13);
	if (i==kGMHorse)			return(15);
	
	return(0);
}

#endif


// ----------------------------------------------------------------- //
void it_MagicLoreSuccess(int item)	// client
// ----------------------------------------------------------------- //

{
/*
	if (item==kKatanaDurability)
		gr_AddText("The item is a magical katana of durability.",false,true);
	else if (item==kKatanaPower)
		gr_AddText("The item is a magical katana of power.",false,true);
	else if (item==kKatanaSpeed)
		gr_AddText("The item is a magical katana of speed.",false,true);

	else if (item==kLongSwordDurability)
		gr_AddText("The item is a magical long sword of durability.",false,true);
	else if (item==kLongSwordPower)
		gr_AddText("The item is a magical long sword of power.",false,true);
	else if (item==kLongSwordSpeed)
		gr_AddText("The item is a magical long sword of speed.",false,true);

	else if (item==kBroadSwordDurability)
		gr_AddText("The item is a magical broad sword of durability.",false,true);
	else if (item==kBroadSwordPower)
		gr_AddText("The item is a magical broad sword of power.",false,true);
	else if (item==kBroadSwordSpeed)
		gr_AddText("The item is a magical broad sword of speed.",false,true);

	else if (item==kScimitarDurability)
		gr_AddText("The item is a magical scimitar of durability.",false,true);
	else if (item==kScimitarPower)
		gr_AddText("The item is a magical scimitar of power.",false,true);
	else if (item==kScimitarSpeed)
		gr_AddText("The item is a magical scimitar of speed.",false,true);

	else if (item==kKnifeDurability)
		gr_AddText("The item is a magical dagger of durability.",false,true);
	else if (item==kKnifePower)
		gr_AddText("The item is a magical dagger of power.",false,true);
	else if (item==kKnifeSpeed)
		gr_AddText("The item is a magical dagger of speed.",false,true);

	else if (item==kStaveDurability)
		gr_AddText("The item is a magical stave of durability.",false,true);
	else if (item==kStavePower)
		gr_AddText("The item is a magical stave of power.",false,true);
	else if (item==kStaveSpeed)
		gr_AddText("The item is a magical stave of speed.",false,true);

	else if (item==kSickleDurability)
		gr_AddText("The item is a magical sickle of durability.",false,true);
	else if (item==kSicklePower)
		gr_AddText("The item is a magical sickle of power.",false,true);
	else if (item==kSickleSpeed)
		gr_AddText("The item is a magical sickle of speed.",false,true);

	else if (item==kClubDurability)
		gr_AddText("The item is a magical club of durability.",false,true);
	else if (item==kClubPower)
		gr_AddText("The item is a magical club of power.",false,true);
	else if (item==kClubSpeed)
		gr_AddText("The item is a magical club of speed.",false,true);

	else if (item==kFlailDurability)
		gr_AddText("The item is a magical flail of durability.",false,true);
	else if (item==kFlailPower)
		gr_AddText("The item is a magical flail of power.",false,true);
	else if (item==kFlailSpeed)
		gr_AddText("The item is a magical flail of speed.",false,true);

	else if (item==kShortBowDurability)
		gr_AddText("The item is a magical short bow of durability.",false,true);
	else if (item==kShortBowPower)
		gr_AddText("The item is a magical short bow of power.",false,true);
	else if (item==kShortBowSpeed)
		gr_AddText("The item is a magical short bow of speed.",false,true);

	else if (item==kBattleAxeDurability)
		gr_AddText("The item is a magical battle axe of durability.",false,true);
	else if (item==kBattleAxePower)
		gr_AddText("The item is a magical battle axe of power.",false,true);
	else if (item==kBattleAxeSpeed)
		gr_AddText("The item is a magical battle axe of speed.",false,true);

	else if (item==kHandAxeDurability)
		gr_AddText("The item is a magical hand axe of durability.",false,true);
	else if (item==kHandAxePower)
		gr_AddText("The item is a magical hand axe of power.",false,true);
	else if (item==kHandAxeSpeed)
		gr_AddText("The item is a magical hand axe of speed.",false,true);

	else if (item==kGlaiveDurability)
		gr_AddText("The item is a magical glaive of durability.",false,true);
	else if (item==kGlaivePower)
		gr_AddText("The item is a magical glaive of power.",false,true);
	else if (item==kGlaiveSpeed)
		gr_AddText("The item is a magical glaive of speed.",false,true);

	else if (item==kSpearDurability)
		gr_AddText("The item is a magical spear of durability.",false,true);
	else if (item==kSpearPower)
		gr_AddText("The item is a magical spear of power.",false,true);
	else if (item==kSpearSpeed)
		gr_AddText("The item is a magical spear of speed.",false,true);

	else if (item==kWarHammerDurability)
		gr_AddText("The item is a magical war hammer of durability.",false,true);
	else if (item==kWarHammerPower)
		gr_AddText("The item is a magical war hammer of power.",false,true);
	else if (item==kWarHammerSpeed)
		gr_AddText("The item is a magical war hammer of speed.",false,true);

	else if (item==kMaceDurability)
		gr_AddText("The item is a magical mace of durability.",false,true);
	else if (item==kMacePower)
		gr_AddText("The item is a magical mace of power.",false,true);
	else if (item==kMaceSpeed)
		gr_AddText("The item is a magical mace of speed.",false,true);

	else if (item==kLongBowDurability)
		gr_AddText("The item is a magical long bow of durability.",false,true);
	else if (item==kLongBowPower)
		gr_AddText("The item is a magical long bow of power.",false,true);
	else if (item==kLongBowSpeed)
		gr_AddText("The item is a magical long bow of speed.",false,true);

	else
		gr_AddText("You are unable to determine anything useful.",false,true);

*/
}

// ----------------------------------------------------------------- //
TBoolean in_ItemIsUsable(int item)	// client
// ----------------------------------------------------------------- //

{

	if ((itemList[item].itemType==kCookedMeatType) || (itemList[item].itemType==kCookedVegType))
		return (true);

	if (itemList[item].itemType==kBeverageType)  // *hiccup*
		return (true);
	//Dave
	if (itemList[item].itemType==kBeverageType2)  //  *burp*
		return (true);
	
	if (itemList[item].itemType==kBeverageType3)  // *Ahhhhhhhh*
		return (true);
	
	if (itemList[item].itemType==kBeverageType4)  // Go Buckeyes!
		return (true);
	//Dave 11-25-2012 elixers
	if (itemList[item].itemType==kElixirType1)  // <3
		return (true);
	if (itemList[item].itemType==kElixirType2)  // Duhhhh
		return (true);
	if (itemList[item].itemType==kElixirType3)  // zimm boom pop
		return (true);
	if (itemList[item].itemType==kElixirType4)  // kapow
		return (true);
	if (itemList[item].itemType==kElixirType5)  // tingle
		return (true);
	
	if (itemList[item].itemType==kRewardType)
		return(true);
	//Dave^
	if (itemList[item].itemType==kPotionType)
		return (true);

	if (itemList[item].itemType==kOrbType)
		return (true);

	if (itemList[item].itemType==kNavigationToolType)
		return (true);

	if (itemList[item].itemType==kQuestType)
		return (true);

	if (itemList[item].useLocation!=kCantArm)
		return (true);

	if (itemList[item].itemType==kDyeType)
		return(true);

	return (false);

}

// ----------------------------------------------------------------- //
void it_TryToRepairItem(int slot)	// client
// ----------------------------------------------------------------- //

{
	TBoolean          canUseSkill;

	if ((slot<1) || (slot>=kMaxInventorySize))
		return;

	if (!((it_IsArmor(localInventory[slot].itemID, false)) || (it_IsWeapon(localInventory[slot].itemID))))
	{
		gr_AddText("You cannot repair that item.",false,true);
		return;
	}
	
	if (itemList[localInventory[slot].itemID].makeSkill==kTailoringLeatherworking)
	{
		gr_AddText("You cannot repair that item.",false,true);
		return;
	}
	
	if (itemList[localInventory[slot].itemID].magical)
	{
		gr_AddText("You cannot repair magical items.", false, true);
		return;
	}

	if ((localInventory[slot].itemCount==127) || (localInventory[slot].itemCount==255)) // doesn't need repair
	{
		gr_AddText("That item doesn't need to be repaired.",false,true);
		return;
	}

	if (localInventory[slot].itemCount>127) // repairing GM item
	{
		if ((itemList[localInventory[slot].itemID].makeSkillLevel*2)>localSkills[itemList[localInventory[slot].itemID].makeSkill].level)	// not enough skill
		{
			gr_AddText("You lack the skill required to repair that item.",false,true);
			return;
		}
	}
	else
	{
		if (itemList[localInventory[slot].itemID].makeSkillLevel>localSkills[itemList[localInventory[slot].itemID].makeSkill].level)	// not enough skill
		{
			gr_AddText("You lack the skill required to repair that item.",false,true);
			return;
		}
	}

	if (itemList[localInventory[slot].itemID].makeSkill==kWoodworking)
	{
		if (!in_ItemTypeEquiped(kWoodToolType))
		{
			if (in_PlayerHasItemType(kWoodToolType)>0)
			{
				gr_AddText("You remove your woodcrafting tools from your bag.",false,true);
				in_UseItem(in_PlayerHasItemType(kWoodToolType));
			}
			else
			{
				gr_AddText("You must have woodcrafting tools.",false,true);
				return;
			}
		}

		nw_SendRepair(slot);
	}

	if (itemList[localInventory[slot].itemID].makeSkill==kBlacksmithing)
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

		canUseSkill=false;


		if (clientMap[playerRow+13-1][playerCol+13-1]==738) 			canUseSkill=true;
		else if (clientMap[playerRow+13-1][playerCol+13]==738) 	  canUseSkill=true;
		else if (clientMap[playerRow+13-1][playerCol+13+1]==738)  canUseSkill=true;
		else if (clientMap[playerRow+13][playerCol+13-1]==738) 	  canUseSkill=true;
		else if (clientMap[playerRow+13][playerCol+13+1]==738) 	  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13-1]==738)  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13]==738) 	  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13+1]==738)  canUseSkill=true;

		if (clientMap[playerRow+13-1][playerCol+13-1]==644) 			canUseSkill=true;
		else if (clientMap[playerRow+13-1][playerCol+13]==644) 	  canUseSkill=true;
		else if (clientMap[playerRow+13-1][playerCol+13+1]==644)  canUseSkill=true;
		else if (clientMap[playerRow+13][playerCol+13-1]==644) 	  canUseSkill=true;
		else if (clientMap[playerRow+13][playerCol+13+1]==644) 	  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13-1]==644)  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13]==644) 	  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13+1]==644)  canUseSkill=true;


		if (clientMap[playerRow+13-1][playerCol+13-1]==645) 			canUseSkill=true;
		else if (clientMap[playerRow+13-1][playerCol+13]==645) 	  canUseSkill=true;
		else if (clientMap[playerRow+13-1][playerCol+13+1]==645)  canUseSkill=true;
		else if (clientMap[playerRow+13][playerCol+13-1]==645) 	  canUseSkill=true;
		else if (clientMap[playerRow+13][playerCol+13+1]==645) 	  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13-1]==645)  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13]==645) 	  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13+1]==645)  canUseSkill=true;

		if (clientMap[playerRow+13-1][playerCol+13-1]==1180) 			canUseSkill=true;
		else if (clientMap[playerRow+13-1][playerCol+13]==1180) 	  canUseSkill=true;
		else if (clientMap[playerRow+13-1][playerCol+13+1]==1180)  canUseSkill=true;
		else if (clientMap[playerRow+13][playerCol+13-1]==1180) 	  canUseSkill=true;
		else if (clientMap[playerRow+13][playerCol+13+1]==1180) 	  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13-1]==1180)  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13]==1180) 	  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13+1]==1180)  canUseSkill=true;

		if (clientMap[playerRow+13-1][playerCol+13-1]==1181) 			canUseSkill=true;
		else if (clientMap[playerRow+13-1][playerCol+13]==1181) 	  canUseSkill=true;
		else if (clientMap[playerRow+13-1][playerCol+13+1]==1181)  canUseSkill=true;
		else if (clientMap[playerRow+13][playerCol+13-1]==1181) 	  canUseSkill=true;
		else if (clientMap[playerRow+13][playerCol+13+1]==1181) 	  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13-1]==1181)  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13]==1181) 	  canUseSkill=true;
		else if (clientMap[playerRow+13+1][playerCol+13+1]==1181)  canUseSkill=true;

		if (canUseSkill)
		{
			nw_SendRepair(slot);
		}
		else
			gr_AddText("You must be facing a forge to use it.",false,true);
	}

}

// ----------------------------------------------------------------- //
void it_TryToMakeItem(int item)	// client
// ----------------------------------------------------------------- //

{
	char 					itemName[kStringLength];//[48];
	char 					message[kStringLength];//[128];
	int						i;
	TBoolean			wearingArmor      = false;
	TBoolean			wearingMetalArmor = false;
	int						canGroup;

	//---------------------------------------------------------

	if (itemList[item].makeSkill==kTailoringLeatherworking)
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

	//---------------------------------------------------------
	if (itemList[item].makeSkill==kCooking)
	{
		if (!sk_StandingNearFire())
			gr_AddText("You must be facing a fire to cook.",false,true);

		if (!in_ItemTypeEquiped(kEmptyHands))
		{
			gr_AddText("You free your hands.",false,true);

			if (localRightHand!=0)
				in_UnEquipItem(localRightHand);

			if (localLeftHand!=0)
				in_UnEquipItem(localLeftHand);
		}
	}

	//---------------------------------------------------------
	if ((itemList[item].makeSkill==kEnchanting) || (itemList[item].makeSkill==kAlchemyDruid) || (itemList[item].makeSkill==kAlchemyCleric) || (itemList[item].makeSkill==kPoisoning))
	{
		if (!in_ItemTypeEquiped(kEmptyHands))
		{
			gr_AddText("You free your hands.",false,true);

			if (localRightHand!=0)
				in_UnEquipItem(localRightHand);

			if (localLeftHand!=0)
				in_UnEquipItem(localLeftHand);
		}
	}

	//---------------------------------------------------------
	if (itemList[item].makeSkill==kWoodworking)
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
	}

	//---------------------------------------------------------
	if (itemList[item].makeSkill==kTinkering)
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
	}

	//---------------------------------------------------------
	if (itemList[item].makeSkill==kBlacksmithing)
	{
		if (!sk_StandingNearForge())
			gr_AddText("You must be facing a forge.",false,true);

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
	}
	//---------------------------------------------------------
	//Special rogue, no armor
	//Dave 03-19-2012
			if (itemList[item].makeSkill==kSpecialRogue)
				if (wearingMetalArmor)
				{
					gr_AddText("Your armor is interfering with your special abilities.",false,true);
					return;
				}
	//---------------------------------------------------------			
				
	if (itemList[item].itemType==kSpecialAttackType)
	{
		if (!pl_CheckSpecialAttack(item))
			return;
		
		gr_RenderMakeListWindow(currentSkillDisplay,currentCategory);
		nw_SendSpecialAttack(localSpat);
		
		return;
	}

	if (in_CanGroup(item))
		canGroup=1;
	else
		canGroup=127;

	if ((itemList[item].makeSkill==kMageryWizard) || (itemList[item].makeSkill==kMageryCleric) || (itemList[item].makeSkill==kMageryDruid))
	{

		//------------------------------------------------------------------
	  	if (!(localRightHand!=0 && localInventory[localRightHand].itemID==kGMStaff || localInventory[localRightHand].itemID==kRoyalSceptre))
			if ((localRightHand!=0) || (localLeftHand!=0))
			{
				if ((localRightFinger==0 || itemList[localInventory[localRightFinger].itemID].spell!=item) && (localLeftFinger==0 || itemList[localInventory[localLeftFinger].itemID].spell!=item)) //Can use Rings and hold weapons
				{
					gr_AddText("Your hands must be free to use magic.",false,true);
					return;
				}
			}

		//------------------------------------------------------------------

		if (localRightHand!=0)
			if (it_IsArmor(localInventory[localRightHand].itemID, true))
				wearingArmor=true;

		if (localLeftHand!=0)
			if (it_IsArmor(localInventory[localLeftHand].itemID, true))
				wearingArmor=true;

		if (localLegs!=0)
			if (it_IsArmor(localInventory[localLegs].itemID, true))
				wearingArmor=true;

		if (localTorso!=0)
			if (it_IsArmor(localInventory[localTorso].itemID, true))
				wearingArmor=true;

		if (localFeet!=0)
			if (it_IsArmor(localInventory[localFeet].itemID, true))
				wearingArmor=true;

		if (localArms!=0)
			if (it_IsArmor(localInventory[localArms].itemID, true))
				wearingArmor=true;

		if (localRightFinger!=0)
			if (it_IsArmor(localInventory[localRightFinger].itemID, true))
				wearingArmor=true;

		if (localLeftFinger!=0)
			if (it_IsArmor(localInventory[localLeftFinger].itemID, true))
				wearingArmor=true;

		if (localHead!=0)
			if (it_IsArmor(localInventory[localHead].itemID, true))
				wearingArmor=true;

		if (localNeck!=0)
			if (it_IsArmor(localInventory[localNeck].itemID, true))
				wearingArmor=true;

		if (localHands!=0)
			if (it_IsArmor(localInventory[localHands].itemID, true))
				wearingArmor=true;

		if (localWaist!=0)
			if (it_IsArmor(localInventory[localWaist].itemID, true))
				wearingArmor=true;

		if (localRightHand!=0)
			if (it_IsMetalArmor(localInventory[localRightHand].itemID))
				wearingMetalArmor=true;

		if (localLeftHand!=0)
			if (it_IsMetalArmor(localInventory[localLeftHand].itemID))
				wearingMetalArmor=true;

		if (localLegs!=0)
			if (it_IsMetalArmor(localInventory[localLegs].itemID))
				wearingMetalArmor=true;

		if (localTorso!=0)
			if (it_IsMetalArmor(localInventory[localTorso].itemID))
				wearingMetalArmor=true;

		if (localFeet!=0)
			if (it_IsMetalArmor(localInventory[localFeet].itemID))
				wearingMetalArmor=true;

		if (localArms!=0)
			if (it_IsMetalArmor(localInventory[localArms].itemID))
				wearingMetalArmor=true;

		if (localRightFinger!=0)
			if (it_IsMetalArmor(localInventory[localRightFinger].itemID))
				wearingMetalArmor=true;

		if (localLeftFinger!=0)
			if (it_IsMetalArmor(localInventory[localLeftFinger].itemID))
				wearingMetalArmor=true;

		if (localHead!=0)
			if (it_IsMetalArmor(localInventory[localHead].itemID))
				wearingMetalArmor=true;

		if (localNeck!=0)
			if (it_IsMetalArmor(localInventory[localNeck].itemID))
				wearingMetalArmor=true;

		if (localHands!=0)
			if (it_IsMetalArmor(localInventory[localHands].itemID))
				wearingMetalArmor=true;

		if (localWaist!=0)
			if (it_IsMetalArmor(localInventory[localWaist].itemID))
				wearingMetalArmor=true;

		if ((localRightFinger!=0 && itemList[localInventory[localRightFinger].itemID].spell==item) || (localLeftFinger!=0 && itemList[localInventory[localLeftFinger].itemID].spell==item)) //Can use Rings and wear armor
		{
			wearingArmor=false;
			wearingMetalArmor=false;
		}

		if (localClass<=6) //GMs can cast spell in armor...
		{
			if (itemList[item].makeSkill==kMageryWizard)
				if (wearingArmor)
				{
					gr_AddText("Your armor is interfering with the magic energy.",false,true);
					return;
				}

			if ((itemList[item].makeSkill==kMageryCleric) || (itemList[item].makeSkill==kMageryDruid))
				if (wearingMetalArmor)
				{
					gr_AddText("Your armor is interfering with the magic energy.",false,true);
					return;
				}
		
		
		
		
		}


		//------------------------------------------------------------------

		if (item==kTurnUndeadSpell)
		{
			nw_SendAction(kCastTurnUndeadSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kSummonCreatureSpell)
		{
			nw_SendAction(kCastSummonCreatureSpell,kNormalPriority,kSendNow,true);
			return;
		}
		//Dave 02-06-2011
		if (item==kGreaterSummonCreatureSpell)
		{
			nw_SendAction(kCastGreaterSummonCreatureSpell,kNormalPriority,kSendNow,true);
			return;
		}
		if (item==kProvocationSpell)
		{
			nw_SendAction(kCastProvocationSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kCalmSpell)
		{
			nw_SendAction(kCastCalmSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kRevealSpell)
		{
			nw_SendAction(kCastRevealSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kParalyzeFieldSpell)
		{
			nw_SendAction(kCastParalyzeFieldSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kPoisonFieldSpell)
		{
			nw_SendAction(kCastPoisonFieldSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kMassHealSpell)
		{
			nw_SendAction(kCastMassHealSpell,kNormalPriority,kSendNow,true);
			return;
		}
		
		if (item==kShapeshiftLesserSpell)
		{
			nw_SendAction(kCastShapeshiftLesserSpell,kNormalPriority,kSendNow,true);
			return;
		}
		
		if (item==kShapeshiftSpell)
		{
			nw_SendAction(kCastShapeshiftSpell,kNormalPriority,kSendNow,true);
			return;
		}
		
		if (item==kShapeshiftGreaterSpell)
		{
			nw_SendAction(kCastShapeshiftGreaterSpell,kNormalPriority,kSendNow,true);
			return;
		}
		if (item==kShapeshiftAllSpell)
		{
			nw_SendAction(kCastShapeshiftAllSpell,kNormalPriority,kSendNow,true);
			return;
		}
		
		if (item==kShapeshiftDragonSpell)
		{
			nw_SendAction(kCastShapeshiftDragonSpell,kNormalPriority,kSendNow,true);
			return;
		}
		/**/

		if (item==kDisorientFieldSpell)
		{
			nw_SendAction(kCastDisorientFieldSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kCreateFoodSpell)
		{
			nw_SendAction(kCastCreateFoodSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kTurnUndeadSpell)
		{
			nw_SendAction(kCastTurnUndeadSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kSummonPetSpell)
		{
			nw_SendAction(kCastSummonPetSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kHealPetSpell)
		{
			in_ClearPickUpPack();
			nw_SendAction(kCastHealPetSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kMagicTrapSpell)
		{
			in_ClearPickUpPack();
			nw_SendAction(kCastMagicTrapSpell,kNormalPriority,kSendNow,true);
			return;
		}

		if (item==kMagicDisarmSpell)
		{
			in_ClearPickUpPack();
			nw_SendAction(kCastMagicDisarmSpell,kNormalPriority,kSendNow,true);
			return;
		}
		//Dave
		if (item==kChaosFieldSpell)
		{
			in_ClearPickUpPack();
			nw_SendAction(kCastChaosFieldSpell,kNormalPriority,kSendNow,true);
			return;
		}
		
		gr_AddText("Choose a target...",false,false);

		if (item==kResurrectSpell)
			targetMode=kResurrectTarget;

		if (item==kLightningBoltSpell)
			targetMode=kLightningBoltTarget;

		if (item==kLesserHealSpell)
			targetMode=kLesserHealTarget;

		if (item==kGreaterHealSpell)
			targetMode=kGreaterHealTarget;

		if (item==kRemoveCurseSpell)
			targetMode=kRemoveCurseTarget;

		if (item==kFullHealSpell)
			targetMode=kFullHealTarget;

		if (item==kFireFieldSpell)
			targetMode=kFireFieldTarget;

		if (item==kTeleportSpell)
			targetMode=kTeleportTarget;

		if (item==kPoisonSpell)
			targetMode=kPoisonTarget;

		if (item==kLesserDetoxifySpell)
			targetMode=kLesserDetoxifyTarget;

		if (item==kGreaterDetoxifySpell)
			targetMode=kGreaterDetoxifyTarget;

		if (item==kFullDetoxifySpell)
			targetMode=kFullDetoxifyTarget;

		if (item==kExplosionSpell)
			targetMode=kExplosionTarget;

		if (item==kInvisibilitySpell)
			targetMode=kInvisibilityTarget;

		if (item==kOpportunitySpell)
			targetMode=kOpportunityTarget;

		if (item==kNightVisionSpell)
			targetMode=kNightVisionTarget;

		if (item==kParalyzeSpell)
			targetMode=kParalyzeTarget;

		if (item==kDrainManaSpell)
			targetMode=kDrainManaTarget;

		if (item==kReflectiveArmorSpell)
			targetMode=kReflectiveArmorTarget;

		if (item==kDisorientationSpell)
			targetMode=kDisorientationTarget;

		if (item==kMagicReflectionSpell)
			targetMode=kMagicReflectionTarget;

		if (item==kClumsySpell)
			targetMode=kClumsySpellTarget;

		if (item==kAgilitySpell)
			targetMode=kAgilitySpellTarget;

		if (item==kWeakenSpell)
			targetMode=kWeakenSpellTarget;

		if (item==kStrengthSpell)
			targetMode=kStrengthSpellTarget;

		if (item==kSimpleMindSpell)
			targetMode=kSimpleMindSpellTarget;

		if (item==kElevateMindSpell)
			targetMode=kElevateMindTarget;

		if (item==kFireballSpell)
			targetMode=kFireballTarget;

		if (item==kFireblastSpell)
			targetMode=kFireblastTarget;

		if (item==kMagicMissileSpell)
			targetMode=kMagicMissileTarget;

		if (item==kFireblast3Spell)
			targetMode=kFireblast3Target;

		if (item==kFireblast4Spell)
			targetMode=kFireblast4Target;

		if (item==kFireblast5Spell)
			targetMode=kFireblast5Target;

		return;

	}

	if (item==kArrow)
	{
		gr_AddText("You begin to craft the arrows...",false,true);
		strcpy(makeSuccessMessage,"You successfully craft the arrows.");
		strcpy(makeFailedMessage,"You fail to craft the arrows.");
	}
	else 	if (item==kBolt)
	{
		gr_AddText("You begin to craft the bolts...",false,true);
		strcpy(makeSuccessMessage,"You successfully craft the bolts.");
		strcpy(makeFailedMessage,"You fail to craft the bolts.");
	}
	else 	if (item==kShaft)
	{
		gr_AddText("You begin to carve the shafts...",false,true);
		strcpy(makeSuccessMessage,"You successfully carve the shafts.");
		strcpy(makeFailedMessage,"You fail to carve the shafts.");
	}
	else if ((itemList[item].makeSkill==kAlchemyCleric) || (itemList[item].makeSkill==kAlchemyDruid) || (itemList[item].makeSkill==kPoisoning))
	{
		if (canGroup>=127 || item==kHandAxePoison) //hotfix poison hand axe
		{
			strcpy(message,"You begin to apply the poison to the blade...");
			gr_AddText(message,false,true);
			strcpy(makeSuccessMessage,"You successfully apply the poison to the blade.");
			strcpy(makeFailedMessage,"You fail to apply the poison to the blade.");
		}
		else
		{
			in_GetItemName(item, itemName,canGroup);

			for (i=0;i<strlen(itemName);i++)
				if (((int)itemName[i]>=65) && ((int)itemName[i]<=90)) // make item name lower case
					itemName[i]=(char)((int)itemName[i]+32);

			strcpy(message,"You begin to mix the ");

			strcat(message,itemName);

			strcat(message,"...");

			gr_AddText(message,false,true);

			strcpy(makeSuccessMessage,"You successfully mix the ");

			strcat(makeSuccessMessage,itemName);

			strcat(makeSuccessMessage,".");

			strcpy(makeFailedMessage,"You fail to mix the ");

			strcat(makeFailedMessage,itemName);

			strcat(makeFailedMessage,".");
		}
	}
	else if (itemList[item].makeSkill==kEnchanting)
	{
		if (itemList[item].itemType==kInfusedCrystalType)
		{
			gr_AddText("You begin to infuse the crystal...",false,true);
			strcpy(makeSuccessMessage,"You successfully infuse the crystal.");
			strcpy(makeFailedMessage,"You fail to infuse the crystal.");
		}
		else if (itemList[item].itemType==kEtheriteType)
		{
			gr_AddText("You begin to extract Etherite...",false,true);
			strcpy(makeSuccessMessage,"You successfully extract Etherite.");
			strcpy(makeFailedMessage,"You fail to extract anything.");
		}
		else
		{
			gr_AddText("You begin to make the item...",false,true);
			strcpy(makeSuccessMessage,"You successfully craft the item.");
			strcpy(makeFailedMessage,"You fail to craft the item.");
		}
	}

	//else if (itemList[item].makeSkill==kBowcraftFletching)
	//	{
	//		in_GetItemName(item, itemName,canGroup);
	//		for (i=0;i<strlen(itemName);i++)
	//			if (((int)itemName[i]>=65) && ((int)itemName[i]<=90)) // make item name lower case
	//				itemName[i]=(char)((int)itemName[i]+32);
	//		strcpy(message,"You { to carve the ");
	//		strcat(message,itemName);
	//		strcat(message,"...");
	//		gr_AddText(message,false,true);
	//		strcpy(makeSuccessMessage,"You successfully carve a ");
	//		strcat(makeSuccessMessage,itemName);
	//		strcat(makeSuccessMessage,".");
	//		strcpy(makeFailedMessage,"You fail to carve a ");
	//		strcat(makeFailedMessage,itemName);
	//		strcat(makeFailedMessage,".");
	//	}
	else if (itemList[item].makeSkill==kCooking)
	{
		strcpy(message,"You begin to cook the food...");
		gr_AddText(message,false,true);
		strcpy(makeSuccessMessage,"You successfully cook the food.");
		strcpy(makeFailedMessage,"You fail to cook the food.");
	}
	else if ((it_IsWeapon(item) || (it_IsArmor(item, false))))
	{
		in_GetItemName(item, itemName,canGroup);

		for (i=0;i<strlen(itemName);i++)
			if (((int)itemName[i]>=65) && ((int)itemName[i]<=90)) // make item name lower case
				itemName[i]=(char)((int)itemName[i]+32);

		strcpy(message,"You begin to craft a ");

		strcat(message,itemName);

		strcat(message,"...");

		gr_AddText(message,false,true);

		strcpy(makeSuccessMessage,"You successfully craft a ");

		strcat(makeSuccessMessage,itemName);

		strcat(makeSuccessMessage,".");

		strcpy(makeFailedMessage,"You fail to craft a ");

		strcat(makeFailedMessage,itemName);

		strcat(makeFailedMessage,".");
	}
	else
	{
		gr_AddText("You begin to make the item...",false,true);
		strcpy(makeSuccessMessage,"You successfully craft the item.");
		strcpy(makeFailedMessage,"You fail to craft the item.");
	}

	nw_SendMake(item);

}

// ----------------------------------------------------------------- //
void it_CreateItem(int item, int use, int type, int pictID, int weight, int price, int color, int param1, int param2, int parent) // server
// ----------------------------------------------------------------- //

{

	if (item>=kMaxItems)			// beta debug
		fi_WriteToErrorLog("Out of Range - it_CreateItem");

	if (parent<0)
		parent=item;

	if (use==kBothHands)
	{
		use=kRightHand;
		itemList[item].twoHandItem					= true;
	}
	else
		itemList[item].twoHandItem						= false;

	itemList[item].useLocation							= (UInt8)		use;

	itemList[item].itemType									= (UInt8)		type;

	itemList[item].pictID										= (UInt16)	pictID;

	itemList[item].weight										= (UInt8)		weight;

	itemList[item].price										= (UInt16)	price;

	itemList[item].itemColor								= (UInt8)		color;

	if ((it_IsArmor(item, false)) || (it_IsClothing(item)))
	{
		itemList[item].armorRating					= (UInt8)		param1;
		itemList[item].dexPenalty					  = (UInt8)		param2;
	}

	itemList[item].weaponRange							= (UInt8)		1;

	itemList[item].weaponSpeed							= (UInt16)	300;
	itemList[item].weaponDamage							= (UInt8)		3;
	itemList[item].useSkill									= (UInt8)		0;
	itemList[item].durability								= (UInt8)		20;
	itemList[item].parent								    = parent;

}

// ----------------------------------------------------------------- //
TBoolean it_IsWeapon(int id)
// ----------------------------------------------------------------- //

{

	if (itemList[id].itemType==kMiscWeaponType) 	return (true);

	if (itemList[id].itemType==kAxeWeaponType) 		return (true);

	if (itemList[id].itemType==kArrowWeaponType) 	return (true);

	if (itemList[id].itemType==kBoltWeaponType) 	return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean it_IsQuest(int id)
// ----------------------------------------------------------------- //

{

	if (itemList[id].itemType==kQuestType) 	return (true);

	return (false);

}

// ----------------------------------------------------------------- //
void it_CalculateArmorRating(int i)
// ----------------------------------------------------------------- //

{
	int							previousRating	=	creature[player[i].creatureIndex].armorRating;
	int							newRating				=	0;
	float						tempRating;
	int							j;
	int							armorCondition;

	for (j=1;j<13;j++)
		if (player[i].arm[j]!=0)
		{
			tempRating		=	itemList[creature[player[i].creatureIndex].inventory[player[i].arm[j]].itemID].armorRating;

			if (!in_CanGroup(creature[player[i].creatureIndex].inventory[player[i].arm[j]].itemID) && itemList[creature[player[i].creatureIndex].inventory[player[i].arm[j]].itemID].itemType != kArmorRingType) //Armor Rings always give full AR bonus
			{
				armorCondition	=	creature[player[i].creatureIndex].inventory[player[i].arm[j]].itemCount;

				if (armorCondition>127)	// GM
					armorCondition=armorCondition-128;

				tempRating			=	tempRating*((float)armorCondition/127);
			}

			newRating			=	newRating+tempRating;
		}

	if (newRating!=previousRating)
	{
		creature[player[i].creatureIndex].armorRating=newRating;
		player[i].sendStatsRefresh=true;
	}
	
	if (player[creature[i].playerIndex].morph!=0) //morphed druids   && pl_AuthorizedTo(c1, kRegularPlayer) || pl_AuthorizedTo(c1, kIsQuestScoundrel)
			{
				newRating	= newRating+tb_Rnd(creatureInfo[player[creature[i].playerIndex].morph].armorRating,creatureInfo[player[creature[i].playerIndex].morph].armorRating); //use creature AR
			}

}

// ----------------------------------------------------------------- //
TBoolean it_IsClothing(int id)
// ----------------------------------------------------------------- //

{

	if (itemList[id].itemType==kClothingType)      return (true);

	if (itemList[id].itemType==kRobeType) 		     return (true);

	if (itemList[id].itemType==kMagicRobeType) 		 return (true);

	if (itemList[id].itemType==kGMRobeType) 		   return (true);

	if (itemList[id].itemType==kRoyalRobeType) 		 return (true);

	if (itemList[id].itemType==kShirtType) 		     return (true);

	if (itemList[id].itemType==kPantsType) 		     return (true);

	if (itemList[id].itemType==kWaistType) 		     return (true);

	if (itemList[id].itemType==kBootsType) 		     return (true);

	if (itemList[id].itemType==kHatType) 		       return (true);

	if (itemList[id].itemType==kWizardHatType) 		 return (true);

	if (itemList[id].itemType==kJesterHatType) 		 return (true);

	if (itemList[id].itemType==kCrownType) 		    return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean it_IsArmor(int id, TBoolean magic)
// ----------------------------------------------------------------- //

{

	if (itemList[id].itemType==kArmorType) 						return (true);

	if (itemList[id].itemType==kLeatherArmorType) 		return (true);

	if (magic) return (false); //To make it so RoPs and Armor Rings won't interfere with spell casting

	if (id==kGrayRobeProtection) 											return (true);

	if (id==kLightBlueRobeProtection) 								return (true);

	if (id==kRedRobeProtection) 											return (true);

	if (id==kBlueRobeProtection) 											return (true);

	if (id==kBrownRobeProtection) 										return (true);

	if (id==kBlackRobeProtection) 										return (true);

	if (id==kGreenRobeProtection) 										return (true);

	if (id==kPurpleRobeProtection) 										return (true);

	if (id==kYellowRobeProtection) 										return (true);

	if (itemList[id].itemType==kArmorRingType)							return (true);

	return (false);

}

// ----------------------------------------------------------------- //
TBoolean it_IsMetalArmor(int id)
// ----------------------------------------------------------------- //

{

	if (itemList[id].itemType==kArmorType) 						return (true);

	return (false);

}

// ----------------------------------------------------------------- //
int it_GetRare(int level) // server
// ----------------------------------------------------------------- //

{

	int         try, nbtry;

	int         rare;

	if (tb_Rnd(0,5000)==0)				return (kRaft);

	//if (tb_Rnd(0,50)==0)
	//return (qu_GetQuestForSpawn());
	
	//Easter eggs Dave 04-04-2012
	/*
	if (tb_Rnd(0,10)==0)
		return (kRedEgg);
	if (tb_Rnd(0,10)==0)
		return (kGreenEgg);
	if (tb_Rnd(0,10)==0)
		return (kPurpleEgg);
	if (tb_Rnd(0,10)==0)
		return (kBlueEgg);
	*/
	if (tb_Rnd(0,50)==0)
		return (kGreenCrystal);

	if (tb_Rnd(0,50)==0)
		return (kRedCrystal);

	if (tb_Rnd(0,50)==0)
		return (kBlueCrystal);

	if (tb_Rnd(0,50)==0)
		return (kPurpleCrystal);

	if (tb_Rnd(0,50)==0)
		return (kOrangeCrystal);

	if (tb_Rnd(0,50)==0)
		return (kYellowCrystal);

	if (tb_Rnd(0,250)==0)
		return (kFireCrystal);

	if (tb_Rnd(0,60)==0)
		return (kBlackCrystal);

	if (tb_Rnd(0,3000)==0)  //Dave change 12-02-2010 was tb_Rnd(0,1000)==0 for all but red, purple(4000) and blue
		return (kGrayRobeProtection);

	if (tb_Rnd(0,3000)==0)
		return (kLightBlueRobeProtection);

	if (tb_Rnd(0,4000)==0)
		return (kRedRobeProtection);

	if (tb_Rnd(0,4000)==0)
		return (kBlueRobeProtection);

	if (tb_Rnd(0,3000)==0)
		return (kBrownRobeProtection);

	if (tb_Rnd(0,3000)==0)
		return (kBlackRobeProtection);

	if (tb_Rnd(0,3000)==0)
		return (kGreenRobeProtection);

	if (tb_Rnd(0,8000)==0)
		return (kPurpleRobeProtection);

	if (tb_Rnd(0,4000)==0)
		return (kYellowRobeProtection);

	if (tb_Rnd(0,4000)==0)
		return (kGrayRobeResistance);

	if (tb_Rnd(0,4000)==0)
		return (kLightBlueRobeResistance);

	if (tb_Rnd(0,6000)==0)
		return (kRedRobeResistance);

	if (tb_Rnd(0,6000)==0)
		return (kBlueRobeResistance);

	if (tb_Rnd(0,3000)==0)
		return (kBrownRobeResistance);

	if (tb_Rnd(0,4000)==0)
		return (KBlackRobeResistance);

	if (tb_Rnd(0,3000)==0)
		return (kGreenRobeResistance);

	if (tb_Rnd(0,8000)==0)
		return (kPurpleRobeResistance);

	if (tb_Rnd(0,3000)==0)
		return (kYellowRobeResistance);
	
	if (tb_Rnd(0,1500)==0)
		return (kRustyNecklace);
	
	if (tb_Rnd(0,3000)==0)
		return (kNeviaFlower);
	
	if (tb_Rnd(0,2000)==0)
		return (kMerefolkHair);
	
	if (tb_Rnd(0,3000)==0)
		return (kMagicStone);
	
	if (tb_Rnd(0,3000)==0)
		return (kAncientRune);
	
	if (tb_Rnd(0,3000)==0)
		return (kSmallMoonFragment);
	
	if (tb_Rnd(0,3000)==0)
		return (kPureCrystalRing);

	if (tb_Rnd(0,1000)==0)
		return (kDwarvenHandAxe);

	if (tb_Rnd(0,1000)==0)
		return (kAmberAxe);

	if (tb_Rnd(0,1000)==0)
		return (kCoalAxe);

	if (tb_Rnd(0,1000)==0)
		return (kLobsterPole);

	if (tb_Rnd(0,1000)==0)
		return (kEelPole);

	if (tb_Rnd(0,1000)==0)
		return (kFireSword);

	if (tb_Rnd(0,1000)==0)
		return (kFrostAxe);
	//Dave
	
	if (tb_Rnd(0,1000)==0)
		return (kAncientTome);

	if (tb_Rnd(0,500)==0)	return (kRedRose);//0,500

	if (tb_Rnd(0,500)==0)	return (kYellowRose);//0,500

	if (tb_Rnd(0,500)==0)	return (kBlackRose);//0,500
	//Dave
	if (tb_Rnd(0,500)==0)	return (kBlueRose);//0,500
	
	if (tb_Rnd(0,500)==0)	return (kPurpleRose);//0,500

	nbtry=10;

	if (currentCycle==kLuckyMoon && fullMoon)
		nbtry+=5;

	for (try=0;try<nbtry;try++)
	{
		rare=rareItem[tb_Rnd(0,rareItemPtr-1)];

		if (itemList[rare].desiredCount>=level)
			if (itemList[rare].currentCount<itemList[rare].desiredCount)
			{
				itemList[rare].currentCount++;
				return (rare);
			}
	}

	return (-1);

}

// ----------------------------------------------------------------- //
void it_CreateWeapon(int item, int use, int type, int pictID, int weight, int price, int range, int speed, int damage, int durability, int skill, long count, int parent,TBoolean metal) // server
// ----------------------------------------------------------------- //

{
	char*	theString=(char*)malloc(3*sizeof(char));

	if (item>=kMaxItems)			// beta debug
		fi_WriteToErrorLog("Out of Range - it_CreateWeapon");

	if (parent<0)
		parent=item;

	if (use==kBothHands)
	{
		use=kRightHand;
		itemList[item].twoHandItem					= true;
	}
	else
		itemList[item].twoHandItem						= false;

	itemList[item].useLocation							= (UInt8)		use;

	itemList[item].itemType									= (UInt8)		type;

	itemList[item].pictID										= (UInt16)	pictID;

	itemList[item].weight										= (UInt8)		weight;

	itemList[item].price										= (UInt16)	price;

	itemList[item].itemColor								= (UInt8)		kNoColor;

	itemList[item].weaponRange							= (UInt8)		range;

	itemList[item].weaponSpeed							= (UInt16)	speed;

	itemList[item].weaponDamage							= (UInt8)		damage;

	itemList[item].useSkill									= (UInt8)		skill;

	itemList[item].durability								= (UInt8)		durability;

	itemList[item].desiredCount							= count;

	itemList[item].parent							      = parent;

	itemList[item].metal							      = metal;

	if (count>0)
	{
		rareItem[rareItemPtr]=item;
		rareItemPtr++;
	}

}

// ----------------------------------------------------------------- //
void it_CreateMagicWeapon(int item, int parent, long count, int magic) // server
// ----------------------------------------------------------------- //

{
	float         speed;
	float         damage;
	float         durability;
	int           use;

	speed       = itemList[parent].weaponSpeed;
	damage      = itemList[parent].weaponDamage;
	durability  = itemList[parent].durability;

	if (magic==kSpeedAttribute)
	{
		speed=speed*0.66l;
		itemList[item].magical  = true;
	}

	if (magic==kPowerAttribute)
	{
		damage=damage*2;
		itemList[item].magical  = true;
	}

	if (magic==kDurabilityAttribute)
	{
		durability=durability*3;
		itemList[item].magical  = true;
	}

	if (magic==kPoisonAttribute)
	{
		itemList[item].poison	  = true;
		durability=durability*0.5;
	}

	if (magic==kSilverAttribute)
		itemList[item].silver	      = true;

	if (itemList[parent].twoHandItem)
		use=kBothHands;
	else
		use=kRightHand;

	it_CreateWeapon(item, use, itemList[parent].itemType, itemList[parent].pictID, itemList[parent].weight, (itemList[parent].price*2), itemList[parent].weaponRange, tb_Round(speed), tb_Round(damage), tb_Round(durability), itemList[parent].useSkill, count, parent, itemList[parent].metal);

}

// ----------------------------------------------------------------- //
TBoolean it_CanMake(int item, int skill, int skillLevel)
// ----------------------------------------------------------------- //

{
	//int							i;

	if (itemList[item].makeSkill==255)	// can't make
		return (false);

	if (itemList[item].makeSkill!=skill)	// not right skill
		return (false);

	if (itemList[item].makeSkillLevel>skillLevel)	// not enough skill
		return (false);

	if (localClass<=6 && !questChar[90])
		if (maxSkillLevel[localClass][skill]==0)
			if ((localRightFinger==0 || itemList[localInventory[localRightFinger].itemID].spell!=item) && (localLeftFinger==0 || itemList[localInventory[localLeftFinger].itemID].spell!=item))
				return (false);

//	for (i=0;i<5;i++)
//		if (itemList[item].ingredientCount[i]>0)
//			if (in_QuantityOf_c(itemList[item].ingredientID[i])<itemList[item].ingredientCount[i])	// no ingredients
//				return (false);

	return (true);

}

// ----------------------------------------------------------------- //
TBoolean it_HaveIngredients(int item)
// ----------------------------------------------------------------- //

{
	int							i;

	for (i=0;i<5;i++)
		if (itemList[item].ingredientCount[i]>0)
			if (in_QuantityOf_c(itemList[item].ingredientID[i])<itemList[item].ingredientCount[i])	// no ingredients
				return (false);
				
			
	return (true);

}

// ----------------------------------------------------------------- //
void it_Ingredients(int item, int skill, int skillLevel, int c1, int i1, int c2, int i2, int c3, int i3, int c4, int i4, int c5, int i5, int count)
// ----------------------------------------------------------------- //

{

	if (item>=kMaxItems)			// beta debug
		fi_WriteToErrorLog("Out of Range - it_Ingredients");

	itemList[item].makeSkill							= (UInt8)skill;

	itemList[item].makeSkillLevel					= (UInt8)skillLevel;

	itemList[item].makeCount							= (UInt8)count;

	itemList[item].ingredientCount[0]			= (UInt8)c1;

	itemList[item].ingredientID[0]				= (UInt16)i1;

	itemList[item].ingredientCount[1]			= (UInt8)c2;

	itemList[item].ingredientID[1]				= (UInt16)i2;

	itemList[item].ingredientCount[2]			= (UInt8)c3;

	itemList[item].ingredientID[2]				= (UInt16)i3;

	itemList[item].ingredientCount[3]			= (UInt8)c4;

	itemList[item].ingredientID[3]				= (UInt16)i4;

	itemList[item].ingredientCount[4]			= (UInt8)c5;

	itemList[item].ingredientID[4]				= (UInt16)i5;

}

// ----------------------------------------------------------------- //
void it_InitializeItems(void)
// ----------------------------------------------------------------- //

{
	int														i;
	int														j;
	extern int				mana;//Dave
	
	strcpy(makeSuccessMessage,"");
	strcpy(makeFailedMessage,"");

	for (i=0;i<kMaxItems;i++)
	{
		itemList[i].useLocation									= 0;
		itemList[i].twoHandItem									= false;
		itemList[i].itemType										= 0;
		itemList[i].pictID											= 1000;
		itemList[i].weight											= 0;
		itemList[i].itemColor										= 0;
		itemList[i].price												= 0;
		itemList[i].armorRating									= 0;
		itemList[i].dexPenalty									= 0;
		itemList[i].weaponRange									= 0;
		itemList[i].weaponSpeed									= 0;
		itemList[i].weaponDamage								= 0;
		itemList[i].durability									= 0;
		itemList[i].useSkill										= 0;
		itemList[i].blessed										  = false;
		itemList[i].silver										  = false;
		itemList[i].parent										  = 0;
		itemList[i].magical										  = false;
		itemList[i].poison										  = false;
		itemList[i].metal										    = false;

		itemList[i].makeSkill										= 255;
		itemList[i].makeSkillLevel							= 0;
		itemList[i].makeCount										= 0;
		itemList[i].makeMana							      = 0;

		itemList[i].currentCount								= 0;
		itemList[i].desiredCount								= 0;
		itemList[i].spell										= 0;

		for (j=0;j<5;j++)
		{
			itemList[i].ingredientCount[j]			= 0;
			itemList[i].ingredientID[j]					= 0;
		}
	}

	for (i=0;i<kMaxRareItems;i++)
		rareItem[i]=0;

	rareItemPtr=0;

	//-------------------------------------------------------------------

	for (i=4000;i<5000;i++)
		itemList[i].itemType										= kQuestType;

	//-------------------------------------------------------------------
	
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
	
	it_CreateWeapon	(kKatana, 							kRightHand, kMiscWeaponType, 1001, 4, 250,	1,130, 10, 6, kSwordsmanship, 0, -1,true);

	it_Ingredients	(kKatana, kBlacksmithing, 5, 2, kIngot, 1, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kLongSword, 					kRightHand, kMiscWeaponType, 1027, 6, 400,	1,250, 9, 6, kSwordsmanship, 0, -1,true);

	it_Ingredients	(kLongSword, kBlacksmithing, 7, 4, kIngot, 1, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kFireSword, 					kRightHand, kMiscWeaponType, 1110, 6, 1000,	1,250, 12, 9, kSwordsmanship, 0, -1,true);

	it_CreateWeapon	(kBroadSword, 					kRightHand, kMiscWeaponType, 1028, 6, 150,	1,250, 8, 6, kSwordsmanship, 0, -1,true);

	it_Ingredients	(kBroadSword, kBlacksmithing, 2, 2, kIngot, 1, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kScimitar, 						kRightHand, kMiscWeaponType, 1029, 5, 250,	1,250, 8, 6, kSwordsmanship, 0, -1,true);

	it_Ingredients	(kScimitar, kBlacksmithing, 5, 3, kIngot, 1, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kHolyDagger, 							kRightHand, kMiscWeaponType, 1030, 1, 30,	1,250, 3, 9, kDaggers, 0, -1,true);

	it_CreateWeapon	(kBloodDagger, 							kRightHand, kMiscWeaponType, 1030, 1, 30,	1,250, 3, 9, kDaggers, 0, -1,true);

	it_CreateWeapon	(kKnife, 							kRightHand, kMiscWeaponType, 1030, 1, 30,	1,250, 3, 9, kDaggers, 0, -1,true);

	it_Ingredients	(kKnife, kBlacksmithing, 1, 1, kIngot, 1, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kStave, 							kBothHands, kMiscWeaponType, 1032, 4, 100, 	1,250, 5, 9, kPoleFighting, 0, -1,false);

	it_Ingredients	(kStave, kWoodworking, 4, 2, kBoard, 0, 0, 0,0,0,0,0,0,1);
	
	it_CreateWeapon	(kGMStaff, 							kRightHand, kMiscWeaponType, 1032, 4, 100, 	8,250, 5, 9, kPoleFighting, 0, -1,false);

	it_CreateWeapon	(kSickle, 							kBothHands, kAxeWeaponType, 1033, 6, 300, 1,280, 6, 10, kPoleFighting, 0, -1,true);

	it_Ingredients	(kSickle, kBlacksmithing, 6, 2, kIngot, 2, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kClub, 								kRightHand, kMiscWeaponType, 1034, 6, 75, 	1,250, 5, 7, kMaceFighting, 0, -1,false);

	it_Ingredients	(kClub, kWoodworking, 3, 2, kBoard, 0, 0, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kFlail, 							kRightHand, kMiscWeaponType, 1035, 5, 200, 	1,250, 5, 7, kMaceFighting, 0, -1,true);

	it_Ingredients	(kFlail, kBlacksmithing, 4, 2, kIngot, 1, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kBattleAxe, 					kRightHand, kAxeWeaponType, 1037, 10, 475, 1,290, 9, 15, kMaceFighting, 0, -1,true);

	it_Ingredients	(kBattleAxe, kBlacksmithing, 8, 3, kIngot, 1, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kFrostAxe, 					kRightHand, kAxeWeaponType, 1111, 10, 1000, 1,290, 13, 20, kMaceFighting, 0, -1,true);

	it_CreateWeapon	(kWarHammer, 					kRightHand, kMiscWeaponType, 1041, 7, 200, 1,250, 7, 10, kMaceFighting, 0, -1,true);

	it_Ingredients	(kWarHammer, kBlacksmithing, 5, 1, kIngot, 1, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kMace, 								kRightHand, kMiscWeaponType, 1042, 5, 200, 	1,250, 7, 10, kMaceFighting, 0, -1,true);

	it_Ingredients	(kMace, kBlacksmithing, 4, 1, kIngot, 1, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kShortBow, 								kBothHands, kArrowWeaponType, 1107, 4, 200, 6,360, 5, 12, kArcheryFighting, 0, -1,false);

	it_Ingredients	(kShortBow, kWoodworking, 6, 2, kBoard, 0, 0, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kLongBow, 								kBothHands, kArrowWeaponType, 1036, 8, 350, 7,420, 6, 12, kArcheryFighting, 0, -1,false);

	it_Ingredients	(kLongBow, kWoodworking, 7, 4, kBoard, 0, 0, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kHandAxe, 						kRightHand, kLumberjackToolType, 1038, 6, 20, 	1,250, 6, 15, kMaceFighting, 0, -1,true);

	it_CreateWeapon	(kDwarvenHandAxe, 		kRightHand, kLumberjackToolType, 1038, 6, 20, 	1,250, 6, 15, kMaceFighting, 0, -1,true);

	it_CreateWeapon	(kAmberAxe, 		      kRightHand, kLumberjackToolType, 1038, 6, 20, 	1,250, 6, 15, kMaceFighting, 0, -1,true);

	it_Ingredients	(kHandAxe, kBlacksmithing, 1, 2, kIngot, 1, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kGlaive, 							kBothHands, kAxeWeaponType, 1039, 7, 550, 1,300,11, 10, kPoleFighting, 0, -1,true);

	it_Ingredients	(kGlaive, kBlacksmithing, 8, 2, kIngot, 2, kBoard, 0,0,0,0,0,0,1);

	it_CreateWeapon	(kNeptunesTrident, 					kBothHands, kAxeWeaponType, 1112, 7, 550,	1,300, 11, 10, kPoleFighting, 0, -1,true);

	it_CreateWeapon	(kSpear, 							kBothHands, kMiscWeaponType, 1040, 5, 150, 	1,280, 5, 10, kPoleFighting, 0, -1,true);

	it_Ingredients	(kSpear, kBlacksmithing, 3, 1, kIngot, 2, kBoard, 0,0,0,0,0,0,1);
	//Dave
	it_CreateWeapon	(kBigJackKatana, 					kRightHand, kMiscWeaponType, 1001, 4, 2500,	1,150, 8, 3, kSwordsmanship, 0, -1,true);
	
	it_CreateWeapon	(kRuneSword, 					kRightHand, kMiscWeaponType, 1001, 4, 2500,	1,150, 8, 3, kSwordsmanship, 0, -1,true);
	
	it_CreateWeapon	(kRoyalSceptre, 				kRightHand, kMiscWeaponType, 1042, 5, 200, 	1,250, 7, 10, kMaceFighting, 0, -1,true);
	//itemList[kRoyalScepter].spell		  	=kLightningBoltSpell;
	fi_GetWeaponSettings();

	//-------------------------------------------------------------------

	it_CreateItem		(kChainMailHelmet, 				kHead, kArmorType, 1088, 7, 150, kGray,2,0,-1);  // done

	it_Ingredients	(kChainMailHelmet, kBlacksmithing, 3, 4, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kChainMailHelmet].metal  = true;

	it_CreateItem		(kBreastChain, 				kTorso, kArmorType, 1045, 20, 500, kGray,19,0,-1);  // done kTorso, kArmorType, 1045, 20, 500, kGray,12,0,-1

	it_Ingredients	(kBreastChain, kBlacksmithing, 6, 9, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kBreastChain].metal  = true;

	it_CreateItem		(kChainLegs, 				kLegs, kArmorType, 1095, 20, 275, kGray,10,0,-1);  // done kLegs, kArmorType, 1095, 20, 275, kGray,8,0,-1

	it_Ingredients	(kChainLegs, kBlacksmithing, 4, 6, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kChainLegs].metal  = true;

	it_CreateItem		(kChainArms, 				kArms, kArmorType, 1096, 15, 200, kGray,8,0,-1);  // done kArms, kArmorType, 1096, 15, 200, kGray,8,0,-1

	it_Ingredients	(kChainArms, kBlacksmithing, 4, 6, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kChainArms].metal  = true;

	it_CreateItem		(kChainGauntlets, 				kHands, kArmorType, 1097, 4, 150, kGray,3,0,-1);  // done kHands, kArmorType, 1097, 4, 150, kGray,3,0,-1

	it_Ingredients	(kChainGauntlets, kBlacksmithing, 5, 3, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kChainGauntlets].metal  = true;

	// --------

	it_CreateItem		(kOpenPlateHelmet, 				kHead, kArmorType, 1084, 10, 200, kGray,3,0,-1);  // done

	it_Ingredients	(kOpenPlateHelmet, kBlacksmithing, 5, 6, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kOpenPlateHelmet].metal  = true;

	it_CreateItem		(kBreastPlate, 				kTorso, kArmorType, 1043, 30, 1000, kGray,20,0,-1);  // done kTorso, kArmorType, 1043, 30, 1000, kGray,20,0,-1

	it_Ingredients	(kBreastPlate, kBlacksmithing, 8, 12, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kBreastPlate].metal  = true;

	it_CreateItem		(kLegsPlate, 					kLegs, 	kArmorType, 1089, 30, 600, kGray,15,0,-1);  // done kLegs, 	kArmorType, 1089, 30, 600, kGray,15,0,-1

	it_Ingredients	(kLegsPlate, kBlacksmithing, 7, 8, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kLegsPlate].metal  = true;

	it_CreateItem		(kPlateArms, 				kArms, kArmorType, 1104, 22, 400, kGray,15,0,-1);  // done

	it_Ingredients	(kPlateArms, kBlacksmithing, 7, 8, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kPlateArms].metal  = true;

	it_CreateItem		(kPlateGauntlets, 		kHands, kArmorType, 1082, 6, 200, kGray,5,0,-1);  // done

	it_Ingredients	(kPlateGauntlets, kBlacksmithing, 7, 4, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kPlateGauntlets].metal  = true;
	
	it_CreateItem		(kSlottedPlateHelmet, 				kHead, kArmorType, 1085, 14, 275, kGray,5,0,-1);  // done

	itemList[kSlottedPlateHelmet].metal  = true;

	it_Ingredients	(kSlottedPlateHelmet, kBlacksmithing, 5, 8, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kSlottedPlateHelmet].metal  = true;

	it_CreateItem		(kClosedPlateHelmet, 				  kHead, kArmorType, 1086, 20, 300, kGray,7,0,-1);  // done

	it_Ingredients	(kClosedPlateHelmet, kBlacksmithing, 6, 10, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kClosedPlateHelmet].metal  = true;

	it_CreateItem		(kVikingHelmet, 				      kHead, kArmorType, 1087, 15, 250, kGray,3,0,-1);  // done

	it_Ingredients	(kVikingHelmet, kBlacksmithing, 5, 7, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kVikingHelmet].metal  = true;


	// --

	it_CreateItem		(kBlackOpenPlateHelmet,   kHead,  kArmorType, 1098, 12, 400, kGray,3,0,-1);  // done

	it_Ingredients	(kBlackOpenPlateHelmet, kBlacksmithing, 9, 12, kBlackIngot, 0,0,0,0,0,0,0,0,1);
	
	itemList[kBlackOpenPlateHelmet].metal  = true;

	it_CreateItem		(kBlackBreastPlate, 		  kTorso, kArmorType, 1057, 35, 1500, kBlack,20,0,-1);  // done  kTorso, kArmorType, 1057, 35, 1500, kBlack,20,0,-1
	
	it_Ingredients	(kBlackBreastPlate, kBlacksmithing, 13, 20, kBlackIngot, 0,0,0,0,0,0,0,0,1);
	
	itemList[kBlackBreastPlate].metal  = true;

	it_CreateItem		(kBlackLegsPlate, 			  kLegs, 	kArmorType, 1090, 33, 1000, kBlack,15,0,-1);  // done  kLegs, 	kArmorType, 1090, 33, 1000, kBlack,15,0,-1
	
	it_Ingredients	(kBlackLegsPlate, kBlacksmithing, 12, 18, kBlackIngot, 0,0,0,0,0,0,0,0,1);
	
	itemList[kBlackLegsPlate].metal  = true;

	it_CreateItem		(kBlackPlateArms, 	      kArms,  kArmorType, 1099, 28, 600, kGray,15,0,-1);

	it_Ingredients	(kBlackPlateArms, kBlacksmithing, 11, 16, kBlackIngot, 0,0,0,0,0,0,0,0,1);
	
	itemList[kBlackPlateArms].metal  = true;

	it_CreateItem		(kBlackPlateGauntlets, 		kHands, kArmorType, 1100, 10, 400, kGray,5,0,-1);  // done

	it_Ingredients	(kBlackPlateGauntlets, kBlacksmithing, 9, 9, kBlackIngot, 0,0,0,0,0,0,0,0,1);
	
	itemList[kBlackPlateGauntlets].metal  = true;

	it_CreateItem		(kBlackSlottedPlateHelmet, 		kHead, kArmorType, 1101, 14, 120, kGray,5,0,-1);  // done
	
	it_Ingredients	(kBlackSlottedPlateHelmet, kBlacksmithing, 9, 12, kBlackIngot, 0,0,0,0,0,0,0,0,1);

	itemList[kBlackSlottedPlateHelmet].metal  = true;
	
	it_CreateItem		(kBlackClosedPlateHelmet, 		kHead, kArmorType, 1102, 20, 150, kGray,7,0,-1);  // done

	it_Ingredients	(kBlackClosedPlateHelmet, kBlacksmithing, 10, 12, kBlackIngot, 0,0,0,0,0,0,0,0,1);

	itemList[kBlackClosedPlateHelmet].metal  = true;
	
	it_CreateItem		(kBlackVikingHelmet, 				  kHead, kArmorType, 1103, 15, 95, kGray,3,0,-1);  // done
	
	it_Ingredients	(kBlackVikingHelmet, kBlacksmithing, 11, 13, kBlackIngot, 0,0,0,0,0,0,0,0,1);

	itemList[kBlackVikingHelmet].metal  = true;
	
	it_CreateItem		(kBlackMetalShield, 				kLeftHand, 	kArmorType, 1154, 20, 250, kBlack,8,0,-1);  // done
	
	it_Ingredients	(kBlackMetalShield, kBlacksmithing, 13, 20, kBlackIngot, 0,0,0,0,0,0,0,0,1);

	itemList[kBlackMetalShield].metal  = true;
	

	// --------

	it_CreateItem		(kBurningPlateHelmet,   kHead,  kArmorType, 1155, 10, 90, kRed,3,0,-1);  // done
	
	//it_Ingredients	(kBurningPlateHelmet, kBlacksmithing, 14, 25, kDragonScale, 1, kInfusedRed, 5, kIngot,0,0,0,0,1);
	
	itemList[kBurningPlateHelmet].metal  = true;

	it_CreateItem		(kBurningPlateBreast, 		  kTorso, kArmorType, 1159, 30, 750, kRed,20,0,-1);  // done
		
	//it_Ingredients	(kBurningPlateBreast, kBlacksmithing, 15, 50, kDragonScale, 1, kInfusedRed, 12, kIngot,0,0,0,0,1);

	itemList[kBlackBreastPlate].metal  = true;

	it_CreateItem		(kBurningPlateLegs, 			  kLegs, 	kArmorType, 1158, 30, 525, kRed,15,0,-1);  // done
	
	//it_Ingredients	(kBurningPlateLegs, kBlacksmithing, 16, 50, kDragonScale, 1, kInfusedRed, 12, kIngot,0,0,0,0,1);
	
	itemList[kBurningPlateLegs].metal  = true;

	it_CreateItem		(kBurningPlateArms, 	      kArms,  kArmorType, 1156, 22, 210, kRed,16,0,-1);
	
	//it_Ingredients	(kBurningPlateArms, kBlacksmithing, 16, 35, kDragonScale, 1, kInfusedRed, 9, kIngot,0,0,0,0,1);
	
	itemList[kBurningPlateArms].metal  = true;

	it_CreateItem		(kBurningPlateGauntlets, 		kHands, kArmorType, 1157, 6, 40, kRed,5,0,-1);  // done
	
	//it_Ingredients	(kBurningPlateGauntlets, kBlacksmithing, 16, 20, kDragonScale, 1, kInfusedRed, 5, kIngot,0,0,0,0,1);
	
	itemList[kBurningPlateGauntlets].metal  = true;
	
	it_CreateItem		(kBurningMetalShield, 				kLeftHand, 	kArmorType, 1153, 20, 250, kRed,8,0,-1);  // done
	
	//it_Ingredients	(kBurningMetalShield, kBlacksmithing, 17, 30, kDragonScale, 1, kInfusedRed, 9, kIngot,0,0,0,0,1);

	itemList[kMedalShield].metal  = true;

	// --------
	
	//Dave
	
	it_CreateItem		(kBluePlateHelmet,   kHead,  kArmorType, 1098, 10, 90, kLightBlue,3,0,-1);  // done

	itemList[kBluePlateHelmet].metal  = true;

	it_CreateItem		(kBluePlateBreast, 		  kTorso, kArmorType, 1057, 30, 750, kLightBlue,30,0,-1);  // done

	itemList[kBlackBreastPlate].metal  = true;

	it_CreateItem		(kBluePlateLegs, 			  kLegs, 	kArmorType, 1090, 30, 525, kLightBlue,15,0,-1);  // done

	itemList[kBluePlateLegs].metal  = true;

	it_CreateItem		(kBluePlateArms, 	      kArms,  kArmorType, 1099, 22, 210, kLightBlue,15,0,-1);

	itemList[kBluePlateArms].metal  = true;

	it_CreateItem		(kBluePlateGauntlets, 		kHands, kArmorType, 1100, 6, 40, kLightBlue,5,0,-1);  // done

	itemList[kBluePlateGauntlets].metal  = true;

	//Dave^

	
	// --------

	it_CreateItem		(kMedalShield, 				kLeftHand, 	kArmorType, 1091, 20, 250, kGray,8,0,-1);  // done

	it_Ingredients	(kMedalShield, kBlacksmithing, 5, 5, kIngot, 0, 0, 0,0,0,0,0,0,1);

	itemList[kMedalShield].metal  = true;


	it_CreateItem		(kWoodenShield, 				kLeftHand, 	kArmorType, 1092, 10, 100, kGray,4,0,-1);  // done

	it_Ingredients	(kWoodenShield, kWoodworking, 5, 5, kBoard, 1, kIngot, 1, kPackOfNails,0,0,0,0,1);


	it_CreateItem		(kLeatherBelt, 		kWaist, kWaistType, 1083, 2, 20, kBrown,1,0,-1);  // done

	it_Ingredients	(kLeatherBelt, kTailoringLeatherworking, 1, 1, kLeatherHide, 0, 0, 0,0,0,0,0,0,1);

	//it_CreateItem		(kLightBlueBreastLeather, 		kTorso, kLeatherBreastType, 1058, 15, 175, kLightBlue,8);
	//it_Ingredients	(kLightBlueBreastLeather, kTailoringLeatherworking, 0, 2, kLeatherHide, 1, kLightBlueDye, 0,0,0,0,0,0,1);

	//it_CreateItem		(kRedBreastLeather, 					kTorso, kLeatherBreastType, 1059, 15, 175, kRed,8);
	//it_Ingredients	(kRedBreastLeather, kTailoringLeatherworking, 0, 2, kLeatherHide, 1, kRedDye, 0,0,0,0,0,0,1);

	//it_CreateItem		(kBlueBreastLeather, 				kTorso, kLeatherBreastType, 1060, 15, 175, kBlue,8);
	//it_Ingredients	(kBlueBreastLeather, kTailoringLeatherworking, 0, 2, kLeatherHide, 1, kBlueDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kBrownBreastLeather, 				kTorso, kLeatherArmorType, 1044, 9, 375, kBlack,16,0,-1);	  // done kTorso, kLeatherArmorType, 1044, 9, 375, kBrown,8,0,-1

	it_Ingredients	(kBrownBreastLeather, kTailoringLeatherworking, 7, 3, kLeatherHide, 0, 0, 0,0,0,0,0,0,1);

	it_CreateItem		(kBrownLeatherLegs, 				kLegs, kLeatherArmorType, 1105, 11, 250, kBlack,13,0,-1);	  // done kLegs, kLeatherArmorType, 1105, 9, 250, kBrown,6,0,-1

	it_Ingredients	(kBrownLeatherLegs, kTailoringLeatherworking, 6, 3, kLeatherHide, 0, 0, 0,0,0,0,0,0,1);

	it_CreateItem		(kBrownLeatherArms, 				kArms, kLeatherArmorType, 1058, 8, 175, kBlack,6,0,-1);  // done kArms, kLeatherArmorType, 1058, 8, 175, kGray,6,0,-1

	it_Ingredients	(kBrownLeatherArms, kTailoringLeatherworking, 5, 3, kLeatherHide, 0, 0, 0,0,0,0,0,0,1);
	
	//(int item, int use, int type, int pictID, int weight, int price, int color, int param1, int param2, int parent)     info bar

	//it_CreateItem		(kBlackBreastLeather, 				kTorso, kLeatherArmorType, 1044, 9, 375, kBlack,15,0,-1);
	//it_Ingredients	(kBlackBreastLeather, kTailoringLeatherworking, 0, 2, kLeatherHide, 1, kBlackDye, 0,0,0,0,0,0,1);

	//it_CreateItem		(kGreenBreastLeather, 				kTorso, kLeatherArmorType, 1044, 15, 175, kGreen,15,0,-1);
	//it_Ingredients	(kGreenBreastLeather, kTailoringLeatherworking, 0, 2, kLeatherHide, 1, kGrayDye, 0,0,0,0,0,0,1);

	//it_CreateItem		(kPurpleBreastLeather, 			kTorso, kLeatherArmorType, 1044, 15, 175, kPurple,15,0,-1);
	//it_Ingredients	(kPurpleBreastLeather, kTailoringLeatherworking, 0, 2, kLeatherHide, 1, kPurpleDye, 0,0,0,0,0,0,1);

	//it_CreateItem		(kYellowBreastLeather, 			kTorso, kLeatherArmorType, 1044, 15, 175, kYellow,15,0,-1);
	//it_Ingredients	(kYellowBreastLeather, kTailoringLeatherworking, 0, 2, kLeatherHide, 1, kYellowDye, 0,0,0,0,0,0,1);

	//-------------------------------------------------------------------

	it_CreateItem(kBrackenSeed, 				kCantArm, kMiscType, 1000, 1, 3, kNoColor,0,0,-1);

	it_CreateItem(kGinseng, 						kCantArm, kReagentType, 1000, 1, 3, kNoColor,0,0,-1);

	it_CreateItem(kLotusFlower, 				kCantArm, kReagentType, 1000, 1, 3, kNoColor,0,0,-1);

	it_CreateItem(kMandrakeRoot, 				kCantArm, kReagentType, 1000, 1, 2, kNoColor,0,0,-1);

	it_CreateItem(kAmber, 							kCantArm, kReagentType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem(kGarlic, 							kCantArm, kReagentType, 1000, 1, 2, kNoColor,0,0,-1);

	it_CreateItem(kVolcanicAsh, 				kCantArm, kReagentType, 1000, 1, 10, kNoColor,0,0,-1);

	it_CreateItem(kPoisonGland, 				kCantArm, kReagentType, 1000, 1, 8, kNoColor,0,0,-1);

	it_CreateItem(kDragonTooth, 				kCantArm, kReagentType, 1000, 1, 20, kNoColor,0,0,-1);

	it_CreateItem(kCoal, 						kCantArm, kReagentType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem(kSkull, 						  kCantArm, kMiscType, 1000, 1, 1, kNoColor,0,0,-1);

	it_CreateItem(kDragonScale, 				kCantArm, kMiscType, 1000, 1, 1, kNoColor,0,0,-1);

	it_CreateItem(kGaperStalk, 				kCantArm, kMiscType, 1000, 1, 1, kNoColor,0,0,-1);

	it_CreateItem(kSackOfSpice, 				kCantArm, kMiscType, 1000, 20, 75, kNoColor,0,0,-1);

	//-------------------------------------------------------------------

	it_CreateItem(kAncientTome, 				kCantArm, kMiscType, 1000, 1, 1, kNoColor,0,0,-1);

	it_CreateItem(kIceCrystal, 				  kCantArm, kCrystalType, 1000, 1, 1, kNoColor,0,0,-1);

	it_CreateItem(kRatTail, 				    kCantArm, kMiscType, 1000, 1, 1, kNoColor,0,0,-1);

	it_CreateItem(kOrbOfSeeing, 				kCantArm, kOrbType, 1000, 1, 25, kNoColor,0,0,-1);
	
	it_Ingredients(kOrbOfSeeing, 	kEnchanting, 3, 1, kGaperEye, 1, kElementalDust, 1, kDiamondDust, 30, kEtherite, 0,0,1);
	itemList[kOrbOfSeeing].makeMana = 15;

	it_CreateItem(kRedRose, 				    kCantArm, kMiscType, 1000, 1, 3, kNoColor,0,0,-1);

	it_CreateItem(kYellowRose, 				  kCantArm, kMiscType, 1000, 1, 3, kNoColor,0,0,-1);

	it_CreateItem(kBlackRose, 				  kCantArm, kMiscType, 1000, 1, 3, kNoColor,0,0,-1);
	//Dave
	it_CreateItem(kBlueRose, 				  kCantArm, kMiscType, 1000, 1, 3, kNoColor,0,0,-1);
	
	it_CreateItem(kPurpleRose, 				  kCantArm, kMiscType, 1000, 1, 3, kNoColor,0,0,-1);

	it_CreateItem(kSpiderFang, 		      kCantArm, kMiscType, 1000, 1, 8, kNoColor,0,0,-1);

	it_CreateItem(kScorpionStinger, 		kCantArm, kMiscType, 1000, 1, 8, kNoColor,0,0,-1);

	it_CreateItem(kCrystalRing, 				kRightFinger, kMiscType, 1078, 1, 120, kNoColor,0,0,-1);

	it_CreateItem(kPlatinumRing, 				kRightFinger, kMiscType, 1079, 1, 110, kNoColor,0,0,-1);

	it_CreateItem(kGoldRing, 						kRightFinger, kMiscType, 1080, 1, 100, kNoColor,0,0,-1);

	it_CreateItem(kSilverRing, 					kRightFinger, kMiscType, 1081, 1, 75, kNoColor,0,0,-1);

	itemList[kSilverRing].silver	      = true;

	it_CreateItem(kDragonStoneRing, 		kRightFinger, kMiscType, 1113, 1, 250, kNoColor,0,0,-1);

	it_CreateItem(kElementalRing, 			kRightFinger, kMiscType, 1114, 1, 250, kNoColor,0,0,-1);

	it_CreateItem(kPoisonRing, 				  kRightFinger, kMiscType, 1115, 1, 250, kNoColor,0,0,-1);

	it_CreateItem(kNeptunesRing, 				kRightFinger, kMiscType, 1116, 1, 250, kNoColor,0,0,-1);

	it_CreateItem(kHadesRing, 				  kRightFinger, kMiscType, 1117, 1, 250, kNoColor,0,0,-1);

	it_CreateItem(kManaRing, 				    kRightFinger, kMiscType, 1118, 1, 250, kNoColor,0,0,-1);

	it_CreateItem(kDruidRing, 				  kRightFinger, kMiscType, 1119, 1, 250, kNoColor,0,0,-1);

	it_CreateItem(kChaosRing, 				  kRightFinger, kMiscType, 1120, 1, 250, kNoColor,0,0,-1);
	
	it_CreateItem(kRingOfInsight, 				  kRightFinger, kMiscType, 1114, 1, 250, kNoColor,0,0,-1);

	it_CreateItem(kRingOfLife, 				kRightFinger, kLesserRingType, 1121, 1, 250, kNoColor,0,0,-1);
	//Dave
	itemList[kRingOfLife].spell		  	  =kResurrectSpell;
	//itemList[kRingOfLife].itemCount		=20;

	it_CreateItem(kLesserHealRing, 				  kRightFinger, kLesserRingType, 1136, 1, 250, kNoColor,0,0,-1);

	itemList[kLesserHealRing].spell		  	  =kLesserHealSpell;

	it_CreateItem(kGreaterHealRing, 				  kRightFinger, kLesserRingType, 1137, 1, 250, kNoColor,0,0,-1);

	itemList[kGreaterHealRing].spell		  	  =kGreaterHealSpell;

	it_CreateItem(kLightningBoltRing, 				  kRightFinger, kLesserRingType, 1138, 1, 250, kNoColor,0,0,-1);
	it_Ingredients(kLightningBoltRing,	kEnchanting, 11, 10, kElementalDust, 1, kBoneMageStaff, 1, kBrackenRoots, 1, kPureCrystalRing, 110, kEtherite, 1);
	itemList[kLightningBoltRing].makeMana		=33;
	itemList[kLightningBoltRing].spell		  	=kLightningBoltSpell;

	it_CreateItem(kRemoveCurseRing, 				  kRightFinger, kLesserRingType, 1239, 1, 250, kNoColor,0,0,-1);

	itemList[kRemoveCurseRing].spell		  	  =kRemoveCurseSpell;

	it_CreateItem(kCurio1, 				  kCantArm, kCurioThirdType, 1000, 50, 0, kNoColor,0,0,-1);

	it_CreateItem(kCurio2, 				  kCantArm, kCurioThirdType, 1000, 50, 0, kNoColor,0,0,-1);

	it_CreateItem(kCurio3, 				    kCantArm, kCurioThirdType, 1000, 50, 0, kNoColor,0,0,-1);

	it_CreateItem(kCurioBig, 				  kCantArm, kCurioType, 1000, 150, 0, kNoColor,0,0,-1);

	it_CreateItem(kCurioSmall, 				  kCantArm, kCurioFragmentType, 1000, 10, 0, kNoColor,0,0,-1);

	it_CreateItem(k5ARRing, 				  kRightFinger, kArmorRingType, 1133, 1, 250, kNoColor,4,0,-1);//Dave 07-22-2010 changed rings to 3, 5, 10 ::: 03-20-2012 changed to 4, 7, 12

	it_CreateItem(k10ARRing, 				  kRightFinger, kArmorRingType, 1134, 1, 250, kNoColor,7,0,-1);

	it_CreateItem(k15ARRing, 				  kRightFinger, kArmorRingType, 1135, 1, 250, kNoColor,10,0,-1);

	it_CreateItem(kSilverNecklace, 			  kNeck, kMiscType, 1094, 1, 125, kNoColor,0,0,-1);

	itemList[kSilverNecklace].silver	  = true;

	//-------------------------------------------------------------------

	//it_CreateItem		(kMiningShovel, 		kRightHand, kMiningToolType, 1047, 4, 18, kNoColor,1,0,-1);
	//it_Ingredients	(kMiningShovel,     kBlacksmithing, 0, 2, kIngot, 1, kBoard, 1,kPackOfNails,0,0,0,0,1);

	it_CreateItem		(kCarvingKnife, 		kRightHand, kCarvingToolType, 1059, 4, 20, kNoColor,1,0,-1);

	it_Ingredients	(kCarvingKnife,     kBlacksmithing, 1, 1, kIngot, 1, kBoard, 1,kPackOfNails,0,0,0,0,1);

	it_CreateItem		(kBlacksmithHammer, 		kRightHand, kBlacksmithToolType, 1093, 4, 20, kNoColor,1,0,-1);

	it_Ingredients	(kBlacksmithHammer, kBlacksmithing, 1, 2, kIngot, 1, kBoard, 1,kPackOfNails,0,0,0,0,1);

	it_CreateItem		(kCentaurMiningAxe, 	kRightHand, kMiningToolType, 1046, 4, 20, kNoColor,1,0,-1);

	it_CreateItem		(kCoalAxe, 					  kRightHand, kMiningToolType, 1046, 4, 20, kNoColor,1,0,-1);

	it_CreateItem		(kMiningAxe, 					kRightHand, kMiningToolType, 1046, 4, 20, kNoColor,1,0,-1);

	it_Ingredients	(kMiningAxe, kBlacksmithing, 1, 2, kIngot, 1, kBoard, 1,kPackOfNails,0,0,0,0,1);

	it_CreateItem		(kWoodworkingTools, 		kRightHand, kWoodToolType, 1060, 5, 20, kNoColor,0,0,-1);

	it_Ingredients	(kWoodworkingTools, kTinkering, 1, 1, kIngot, 1, kBoard, 1,kPackOfNails,0,0,0,0,1);

	it_CreateItem		(kTinkeringTools, 			kRightHand, kTinkerToolType, 1061, 4, 20, kNoColor,0,0,-1);

	it_Ingredients	(kTinkeringTools, kTinkering, 2, 1, kIngot, 1, kBoard, 1,kPackOfNails,0,0,0,0,1);

	it_CreateItem		(kShears, 			kRightHand, kSewingToolType, 1108, 2, 20, kNoColor,0,0,-1);

	it_Ingredients	(kShears, kTinkering, 3, 1, kIngot, 0, 0, 0,0,0,0,0,0,1);

	it_CreateItem		(kSextant, 			kCantArm, kNavigationToolType, 1106, 2, 20, kNoColor,0,0,-1);

	it_Ingredients	(kSextant, kTinkering, 3, 1, kIngot, 1, kPackOfNails, 0,0,0,0,0,0,1);

	it_CreateItem		(kLock, 			kCantArm, kMiscType, 1000, 2, 20, kNoColor,0,0,-1);

	it_Ingredients	(kLock, kTinkering, 99, 1, kIngot, 1, kPackOfNails, 0,0,0,0,0,0,1);

	it_CreateItem		(kTrap, 			kCantArm, kMiscType, 1000, 2, 20, kNoColor,0,0,-1);

	it_Ingredients	(kTrap, kTinkering, 5, 1, kIngot, 1, kPackOfNails, 2,kVolcanicAsh,0,0,0,0,1);

	//-------------------------------------------------------------------

	it_CreateItem		(kPackOfNails, 				kCantArm, kMiscType, 1000, 2, 10, kNoColor,0,0,-1);

	it_Ingredients	(kPackOfNails, kTinkering, 4, 1, kIngot, 0, 0, 0,0,0,0,0,0,1);

	//-------------------------------------------------------------------

	it_CreateItem		(kEelPole, 				    kRightHand, kFishingToolType, 1002, 3, 20, kNoColor,0,0,-1);

	it_CreateItem		(kLobsterPole, 				kRightHand, kFishingToolType, 1002, 3, 20, kNoColor,0,0,-1);

	it_CreateItem		(kFishingPole, 				kRightHand, kFishingToolType, 1002, 3, 20, kNoColor,0,0,-1);

	it_Ingredients	(kFishingPole, kWoodworking, 2, 1, kIngot, 2, kBoard, 1,kPackOfNails,0,0,0,0,1);

	//-------------------------------------------------------------------

	//it_CreateItem(kWalkingCane, 				kRightHand, kMiscType, 1031, 2, 30, kNoColor,0);
	it_CreateItem   (kLog, 								kCantArm, kMiscType, 1000, 10, 6, kNoColor,0,0,-1);

	it_CreateItem		(kBoard, 							kCantArm, kMiscType, 1000, 1, 3, kNoColor,0,0,-1);

	it_Ingredients	(kBoard, kWoodworking, 1, 1, kLog, 0, 0, 0,0,0,0,0,0,3);

	it_CreateItem   (kKindling, 								kCantArm, kMiscType, 1000, 5, 4, kNoColor,0,0,-1);

	it_Ingredients	(kKindling, kWoodworking, 1, 1, kLog, 0, 0, 0,0,0,0,0,0,1);

	it_CreateItem   (kFerriteOre, 					kCantArm, kMiscType, 1000, 10, 2, kNoColor,0,0,-1);
	//Dave
	it_CreateItem   (kBlackIngot, 					kCantArm, kMiscType, 1000, 1, 10, kNoColor,0,0,-1);
	
	it_Ingredients	(kBlackIngot, kBlacksmithing, 1, 1, kBlackFerrite, 0, 0, 0,0,0,0,0,0,3);
	
	it_CreateItem   (kBlackFerrite, 					kCantArm, kMiscType, 1000, 10, 4, kNoColor,0,0,-1);
	
	it_CreateItem		(kIngot, 							kCantArm, kMiscType, 1000, 1, 3, kNoColor,0,0,-1);

	it_Ingredients	(kIngot, kBlacksmithing, 1, 1, kFerriteOre, 0, 0, 0,0,0,0,0,0,3);

	it_CreateItem(kEmptyBottle, 				kCantArm, kMiscType, 1000, 1, 2, kNoColor,0,0,-1);

	it_CreateItem(kRollOfCloth, 				kCantArm, kWaistType, 1000, 10, 25, kNoColor,0,0,-1);
	it_Ingredients	(kRollOfCloth, kTailoringLeatherworking, 1, 3, kBundleOfWool, 0, 0, 0,0,0,0,0,0,1);
	
	it_CreateItem		(kLeatherHide, 			kCantArm, kWaistType, 1000, 10, 12, kNoColor,0,0,-1);
	it_Ingredients	(kLeatherHide, kTailoringLeatherworking, 5, 1, kBearSkin, 0, 0, 0,0,0,0,0,0,2);
	
	it_CreateItem		(kFurSkin, 					kCantArm, kMiscType, 1000, 5, 8, kNoColor,0,0,-1);

	it_CreateItem		(kRaft, 					kCantArm, kMiscType, 1000, 100, 500, kNoColor,0,0,-1);

	it_Ingredients	(kRaft, kWoodworking, 8, 8, kLog, 3, kPackOfNails, 0,0,0,0,0,0,1);

	itemList[kRaft].blessed	= true;

	it_CreateItem		(kBandage, 					kCantArm, kMiscTailorType, 1000, 1, 6, kNoColor,0,0,-1);

	it_Ingredients	(kBandage, kTailoringLeatherworking, 1, 1, kRollOfCloth, 0, 0, 0,0,0,0,0,0,10);

	//it_CreateItem(kBlankScroll, 				kCantArm, kMiscType, 1000, 1, 4, kNoColor,0,0);
	it_CreateItem(kDiamond, 						kCantArm, kMiscType, 1000, 1, 400, kNoColor,0,0,-1);

	it_CreateItem(kRuby, 								kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kQueensRuby, 					kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kGoldMirror, 					    kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kBlueTicket, 					    kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kBookOfTheDead, 					kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kMagicRune, 					    kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kSilverDragonClaw, 				kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kRubyHeart, 					    kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kBookOfTheStars, 					kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kKingsSeal, 					    kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kGoldenBox, 					    kCantArm, kMiscType, 1000, 100, 75, kNoColor,0,0,-1);

	it_CreateItem(kBody, 					    kCantArm, kMiscType, 1000, 200, 75, kNoColor,0,0,-1);

	it_CreateItem(kCrystalFlask, 					  kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kSilverLeaf, 					    kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kTomeOfTheDamned, 				kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kTomeOfLife, 					    kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kGoldenFleece, 					  kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kGuildHallDeed, 			 		kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kHouseDeed, 			        kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kCrystalSkull, 					  kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kCrystalSkullPure, 					  kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);
	
	it_CreateItem(kKingsPortrait, 					kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kPersianRug, 					    kCantArm, kMiscType, 1000, 1, 75, kNoColor,0,0,-1);

	it_CreateItem(kGreenCrystal, 						kCantArm, kCrystalType, 1000, 1, 40, kNoColor,0,0,-1);

	it_CreateItem(kRedCrystal, 							kCantArm, kCrystalType, 1000, 1, 40, kNoColor,0,0,-1);

	it_CreateItem(kBlueCrystal, 						kCantArm, kCrystalType, 1000, 1, 40, kNoColor,0,0,-1);

	it_CreateItem(kPurpleCrystal, 						kCantArm, kCrystalType, 1000, 1, 40, kNoColor,0,0,-1);

	it_CreateItem(kOrangeCrystal, 						kCantArm, kCrystalType, 1000, 1, 40, kNoColor,0,0,-1);

	it_CreateItem(kYellowCrystal, 						kCantArm, kCrystalType, 1000, 1, 40, kNoColor,0,0,-1);

	it_CreateItem(kFireCrystal, 						kCantArm, kCrystalType, 1000, 1, 40, kNoColor,0,0,-1);

	it_CreateItem(kBlackCrystal, 						kCantArm, kCrystalType, 1000, 1, 40, kNoColor,0,0,-1);

	it_CreateItem(kPureGreenCrystal, 						kCantArm, kCrystalType, 1000, 10, 40, kNoColor,0,0,-1);

	it_CreateItem(kPureRedCrystal, 							kCantArm, kCrystalType, 1000, 10, 40, kNoColor,0,0,-1);

	it_CreateItem(kPureBlueCrystal, 						kCantArm, kCrystalType, 1000, 10, 40, kNoColor,0,0,-1);

	it_CreateItem(kPurePurpleCrystal, 						kCantArm, kCrystalType, 1000, 10, 40, kNoColor,0,0,-1);

	it_CreateItem(kPureOrangeCrystal, 						kCantArm, kCrystalType, 1000, 10, 40, kNoColor,0,0,-1);

	it_CreateItem(kPureYellowCrystal, 						kCantArm, kCrystalType, 1000, 10, 40, kNoColor,0,0,-1);

	it_CreateItem(kPureFireCrystal, 						kCantArm, kCrystalType, 1000, 10, 40, kNoColor,0,0,-1);

	it_CreateItem(kPureBlackCrystal, 						kCantArm, kCrystalType, 1000, 10, 40, kNoColor,0,0,-1);

	it_CreateItem(kPureIceCrystal, 						kCantArm, kCrystalType, 1000, 10, 40, kNoColor,0,0,-1);

	it_CreateItem(kBrigandHead, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kSorcererHead, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kPirateHead, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem		(kShaft, 							kCantArm, kMiscType, 1000, 1, 1, kNoColor,0,0,-1);

	it_Ingredients	(kShaft, kWoodworking, 5, 1, kBoard, 0, 0, 0,0,0,0,0,0,20);

	it_CreateItem(kFeather, 						kCantArm, kMiscType, 1000, 1, 1, kNoColor,0,0,-1);

	it_CreateItem		(kArrow, 							kCantArm, kMiscType, 1000, 1, 4, kNoColor,0,0,-1);

	it_Ingredients	(kArrow, kWoodworking, 5, 1, kShaft, 1, kFeather, 0,0,0,0,0,0,1);

	it_CreateItem		(kBolt, 								kCantArm, kMiscType, 1000, 1, 5, kNoColor,0,0,-1);

	//it_Ingredients	(kBolt, kWoodworking, 1, 1, kShaft, 1, kFeather, 0,0,0,0,0,0,1);


	//New quest items 04/24/2007

	it_CreateItem(kSmallBag, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kLockbox, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kPapers, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kMagicStone,					  kCantArm, kMiscType, 1000, 2, 50, kNoColor,0,0,-1);

	it_CreateItem(kSmallKey, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kMessage, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kCrate, 					  kCantArm, kMiscType, 1000, 350, 50, kNoColor,0,0,-1);

	it_CreateItem(kVialOfBlood, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kDocuments, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kLetter, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kRustyKey, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kIronKey, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kBronzeKey, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kGoldenBolt, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kSilverBolt, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kDragonEggGreen, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kDragonEggGold, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kDragonEggBlack, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kDragonEggRed, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kDragonEggBlue, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kDragonEggPurple, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kSnakeSkin, 					  kCantArm, kMiscType, 1000, 8, 50, kNoColor,0,0,-1);

	it_CreateItem(kSunstoneRing, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kFlaskOfPerfume, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kLittleStatue, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kLockOfHair, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kJar, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kParchment, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kBoneKnife, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kPearlNecklace, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kStatueOfElara, 					  kCantArm, kMiscType, 1000, 25, 50, kNoColor,0,0,-1);

	it_CreateItem(kMysticSigil, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kArcaneGrimoire, 					  kCantArm, kMiscType, 1000, 2, 50, kNoColor,0,0,-1);

	it_CreateItem(kSilverChalice, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kInfernalTome, 					  kCantArm, kMiscType, 1000, 2, 50, kNoColor,0,0,-1);

	it_CreateItem(kEmeraldPrism, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kSapphirePrism, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kAmethystPrism, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kRubyPrism, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kTopazPrism, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kDiamondPrism, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kGoldCandelabrum, 					  kCantArm, kMiscType, 1000, 2, 50, kNoColor,0,0,-1);

	it_CreateItem(kBookofMoons, 					  kCantArm, kMiscType, 1000, 2, 50, kNoColor,0,0,-1);

	it_CreateItem(kBlinkHoundFur, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kZionidicCrystal, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kVermaStone, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kSpiralCharm, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kGoldThread, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kVialOfCentaurBlood, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	//it_CreateItem(kRoyalSceptre, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kMirithToken, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kMaraliToken, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kAndrisToken, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kMayorSeal,					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kSenateSeal, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kCommanderSeal, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kStoreToken, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kIvoryTusk, 					  kCantArm, kMiscType, 1000, 5, 50, kNoColor,0,0,-1);

	it_CreateItem(kShevPortrait, 					  kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);

	it_CreateItem(kAncientRune,						kCantArm,	kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kNeviaFlower,						kCantArm,	kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSmallMoonFragment,				kCantArm,	kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kMerefolkHair,					kCantArm,	kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kRustyNecklace,					kCantArm,	kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kPureCrystalRing,					kCantArm,	kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);

	it_CreateItem(kBloodAmulet, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBloodStainedMoonstone, 			kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBloodTome, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCrystalNecklace, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kMoonstoneRing, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kJournal, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kTornNote, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCrystalChalice, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBloodyJournal, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBloodyNote, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSkeletonKey, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kNorthStone, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSouthStone, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kEastStone, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kWestStone, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);

	it_CreateItem(kThread, 							kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBead, 							kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBrokenNecklace, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kRuinedBook, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBloodStainedCloth, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kMuddyBoots, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kOrnateStaff, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kMoldyBook, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kLogbook, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kRegister, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kShreddedTome, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kTornPage, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kGlowingBottle, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCrystalDragonStatue, 			kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSilverFigurine, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kStarDiamond, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kShadowedOrb, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSilverTippedQuill, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCrystalGoddessCharm, 			kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kJeweledHairPin, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kMercenaryToken, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCrystalToken, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	//Dave
	it_CreateItem(kDecorativeVase, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kWinningTicket, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBettingSlip, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kLeaderPin, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kOldSword, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCopperMug, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCopperLantern, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kGoldenScale, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kWoodenTrinket, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kPetrifiedGaperEye, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSilkPouch, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kOrnateWalkingStick, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kGoddessCharm, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCrystalSpiderWeb, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kWoodenFlute, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kLute, 							kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSilverFlute, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kGoldLadle, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCharStatue, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kDarkRuby, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kMortarandPestle, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBookofFairyTales, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kJeweledDagger, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kPipe, 							kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kWizardToken, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kDruidToken, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kClericToken, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kFighterToken, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kRangerToken, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kRogueToken, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kJinkDoll, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kVoodooDoll, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kNPCPrivateerHead, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kTuna, 							kCantArm, kMiscType, 1000, 1, 12, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSeaBass, 						kCantArm, kMiscType, 1000, 1, 12, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCopperSpoon, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSilverButton, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kTinPlate, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCrystalBrooch, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kTornCloth, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBookOfBlood, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kRustySextant, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kAmuletOfStars, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kAmuletOfSky, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kAmuletOfTheMoon, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCeruleanDiamond, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kJeweledRing, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBrokenRing, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kDriftWoodFigurine, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSilverShell, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kTinySeaShell, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kReed, 							kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kKelp, 							kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kCharmOfTheSea, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kOrnateShellHorn, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBraidedKelpCharm, 				kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kBookOfSky, 						kCantArm, kMiscType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kGiantHeart, 						kCantArm, kRawMeatType, 1000, 1, 50, kNoColor, 0, 0, -1);
	
	it_CreateItem(kSpiderLeg, 					 	kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kHarpieBeak, 					 	kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kBatWing, 					 	kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kBoneMageStaff, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kBrackenRoots, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kGaperEye, 					 	kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kCubeGoo, 					 	kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kSnailSlime, 					 	kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kZombieBrain, 					kCantArm, kMiscType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kWizardReward, 					kCantArm, kPotionType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kClericReward, 					kCantArm, kPotionType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kDruidReward, 					kCantArm, kPotionType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kFighterReward, 					kCantArm, kPotionType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kRangerReward, 					kCantArm, kPotionType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kRogueReward, 					kCantArm, kPotionType, 1000, 1, 50, kNoColor,0,0,-1);
	
	it_CreateItem(kPixieDust,						kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kCopperCoin,						kCantArm,	kMiscType, 1000, 1,	1,	kNoColor,0,0,-1);
	
	it_CreateItem(kKingFigurine,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kQueenFigurine,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kBishopFigurine,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kKnightFigurine,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kRookFigurine,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kPawnFigurine,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kSilverCoin,						kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kRedToken,						kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kBlackToken,						kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kSilverToken,						kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kGrainOfRedSand,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kCandle,							kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kSpiderEgg,						kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kBearSkin,						kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kSharkFin,						kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kGoldMedal,						kNeck,	kMiscType, 1240, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kSilverMedal,						kNeck,	kMiscType, 1241, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kBronzeMedal,						kNeck,	kMiscType, 1242, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kNeedle,							kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kSLToken,							kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kJadeNecklace,					kNeck,	kMiscType, 1152, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kJadePin,							kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kJadeStatue,						kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kJadeBox,							kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kIridescentToken,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kBrigandInsignia,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kBlueMetal,						kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kPortraitLady,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kPortraitTirana,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
	
	it_CreateItem(kTomeoftheLost,					kCantArm,	kMiscType, 1000, 1,	40,	kNoColor,0,0,-1);
		
	
	// Mounts 12/29
	
	it_CreateItem(kSickHorse,		kMount,	kMountType,	1000,	1,	50,	kNoColor,	0,0,-1);
	it_CreateItem(kHorse,			kMount,	kMountType,	1000,	1,	50,	kNoColor,	0,0,-1);
	it_CreateItem(kFastHorse,		kMount,	kMountType,	1000,	1,	50,	kNoColor,	0,0,-1);
	it_CreateItem(kVeryFastHorse,	kMount,	kMountType,	1000,	1,	50,	kNoColor,	0,0,-1);
	it_CreateItem(kMagicMount1,		kMount,	kMountType,	1000,	1,	50,	kNoColor,	0,0,-1);
	it_CreateItem(kMagicMount2,		kMount,	kMountType,	1000,	1,	50,	kNoColor,	0,0,-1);
	it_CreateItem(kMagicMount3,		kMount,	kMountType,	1000,	1,	50,	kNoColor,	0,0,-1);
	it_CreateItem(kMagicMount4,		kMount,	kMountType,	1000,	1,	50,	kNoColor,	0,0,-1);
	it_CreateItem(kGMHorse,			kMount,	kMountType,	1000,	1,	50,	kNoColor,	0,0,-1);
	//-------------------------------------------------------------------

	it_CreateItem		(kLesserHealPotion, 					kCantArm, kPotionType, 1000, 2, 30, kNoColor,0,0,-1); //done

	it_Ingredients	(kLesserHealPotion, kAlchemyCleric, 1, 1, kEmptyBottle, 1, kGinseng, 0,0,0,0,0,0,1);

	itemList[kLesserHealPotion].makeMana	= 3;

	it_CreateItem		(kGreaterHealPotion, 					kCantArm, kPotionType, 1000, 2, 60, kNoColor,0,0,-1);

	it_Ingredients	(kGreaterHealPotion, kAlchemyCleric, 4, 1, kEmptyBottle, 1, kGinseng, 1,kLotusFlower,0,0,0,0,1); //done

	itemList[kGreaterHealPotion].makeMana	= 12;

	it_CreateItem		(kFullHealPotion, 					kCantArm, kPotionType, 1000, 2, 120, kNoColor,0,0,-1);

	it_Ingredients	(kFullHealPotion, kAlchemyCleric, 7, 1, kEmptyBottle, 1, kGinseng, 1,kLotusFlower,1,kDragonTooth,0,0,1); //done

	itemList[kFullHealPotion].makeMana	= 21;

	it_CreateItem		(kManaPotion, 					kCantArm, kPotionType, 1000, 2, 60, kNoColor,0,0,-1);

	it_Ingredients	(kManaPotion, kAlchemyCleric, 5, 1, kEmptyBottle, 1, kGinseng, 1,kMandrakeRoot,1,kCoal,0,0,1);// //done

	itemList[kManaPotion].makeMana	= 9;

	it_CreateItem		(kLesserDetoxifyPotion, 					kCantArm, kPotionType, 1000, 2, 30, kNoColor,0,0,-1);

	it_Ingredients	(kLesserDetoxifyPotion, kAlchemyCleric, 1, 1, kEmptyBottle, 1, kMandrakeRoot, 0,0,0,0,0,0,1); //done

	itemList[kLesserDetoxifyPotion].makeMana	= 3;

	it_CreateItem		(kGreaterDetoxifyPotion, 					kCantArm, kPotionType, 1000, 2, 30, kNoColor,0,0,-1);

	it_Ingredients	(kGreaterDetoxifyPotion, kAlchemyCleric, 4, 1, kEmptyBottle, 1, kLotusFlower, 1,kMandrakeRoot,0,0,0,0,1); //done

	itemList[kGreaterDetoxifyPotion].makeMana	= 18;

	it_CreateItem		(kFullDetoxifyPotion, 					kCantArm, kPotionType, 1000, 2, 30, kNoColor,0,0,-1);

	it_Ingredients	(kFullDetoxifyPotion, kAlchemyCleric, 6, 1, kEmptyBottle, 1, kLotusFlower, 1,kMandrakeRoot,1,kDragonTooth,0,0,1); //done

	itemList[kFullDetoxifyPotion].makeMana	= 18;

	it_CreateItem		(kStrengthPotion, 					kCantArm, kPotionType, 1000, 2, 60, kNoColor,0,0,-1);

	it_Ingredients	(kStrengthPotion, kAlchemyCleric, 3, 1, kEmptyBottle, 1, kGinseng, 1,kCoal,1,kGarlic,0,0,1); //done

	itemList[kStrengthPotion].makeMana	= 9;

	it_CreateItem		(kAgilityPotion, 					kCantArm, kPotionType, 1000, 2, 60, kNoColor,0,0,-1);

	it_Ingredients	(kAgilityPotion, kAlchemyCleric, 4, 1, kEmptyBottle, 1, kGinseng, 1,kMandrakeRoot,1,kCoal,0,0,1); //done

	itemList[kAgilityPotion].makeMana	= 12;

	it_CreateItem		(kElevateMindPotion, 					kCantArm, kPotionType, 1000, 2, 60, kNoColor,0,0,-1);

	it_Ingredients	(kElevateMindPotion, kAlchemyCleric, 2, 1, kEmptyBottle, 1, kGinseng, 1,kLotusFlower,1,kCoal,0,0,1); //done

	itemList[kElevateMindPotion].makeMana	= 6;




	it_CreateItem		(kNightVisionPotion, 					kCantArm, kPotionType, 1000, 2, 40, kNoColor,0,0,-1);

	it_Ingredients	(kNightVisionPotion, kAlchemyDruid, 2, 1, kEmptyBottle, 1, kLotusFlower, 1,kMandrakeRoot,1,kAmber,0,0,1); //done

	itemList[kNightVisionPotion].makeMana	= 8;//Dave 6

	it_CreateItem		(kInvisibilityPotion, 					kCantArm, kPotionType, 1000, 2, 40, kNoColor,0,0,-1);

	it_Ingredients	(kInvisibilityPotion, kAlchemyDruid, 3, 1, kEmptyBottle, 1, kGinseng, 1,kCoal,1,kGarlic,0,0,1); //done

	itemList[kInvisibilityPotion].makeMana	= 12;//Dave 9

	it_CreateItem		(kNourishPotion, 					kCantArm, kPotionType, 1000, 2, 40, kNoColor,0,0,-1);

	it_Ingredients	(kNourishPotion, kAlchemyDruid, 1, 1, kEmptyBottle, 1, kCoal, 1,kLotusFlower,1,kGarlic,0,0,1); //done

	itemList[kNourishPotion].makeMana	= 5;//Dave 3

	it_CreateItem		(kOpportunityPotion, 					kCantArm, kPotionType, 1000, 2, 40, kNoColor,0,0,-1);

	it_Ingredients	(kOpportunityPotion, kAlchemyDruid, 5, 1, kEmptyBottle, 1, kCoal, 1,kAmber,1,kElectricEel,0,0,1); //done

	itemList[kOpportunityPotion].makeMana	= 15;//Dave 15




	it_CreateItem		(kPoison, 					kCantArm, kMiscType, 1000, 2, 35, kNoColor,0,0,-1);

	it_Ingredients	(kPoison, kPoisoning, 1, 1, kEmptyBottle, 1, kPoisonGland, 1,kCoal,1,kGarlic,0,0,1); //done


	//------------------ Enchanting -------------------------------------------------
	
	//Two kinds of Etherite...
	
	it_CreateItem	(kEtheriteWood,	kCantArm,	kEtheriteType, 1000, 1, 4, kNoColor,0,0,-1);
	it_Ingredients	(kEtheriteWood, kEnchanting, 1, 1,kLog,0,0,0,0,0,0,0,0,1);
	itemList[kEtheriteWood].makeMana=3;
	
	it_CreateItem	(kEtheriteFerrite,	kCantArm,	kEtheriteType, 1000, 1, 4, kNoColor,0,0,-1);
	it_Ingredients	(kEtheriteFerrite, kEnchanting, 1, 1,kFerriteOre,0,0,0,0,0,0,0,0,1);
	itemList[kEtheriteFerrite].makeMana=3;
	//Dave black ferrite etherite
	it_CreateItem	(kBlackEtherite,	kCantArm,	kEtheriteType, 1000, 1, 4, kNoColor,0,0,-1);
	it_Ingredients	(kBlackEtherite, kEnchanting, 1, 1,kBlackFerrite,0,0,0,0,0,0,0,0,1);
	itemList[kBlackEtherite].makeMana=3;
	
	
	
	//Fused into one
	
	it_CreateItem	(kEtherite, kCantArm, kEtheriteType, 1000, 1, 40, kNoColor, 0, 0,-1);
	
	//Crystals...
	
	it_CreateItem	(kInfusedGreen,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedGreen, kEnchanting, 1, 1,kGreenCrystal,10, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedGreen].makeMana=3;
	
	it_CreateItem	(kInfusedRed,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedRed, kEnchanting, 2, 1,kRedCrystal,20, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedRed].makeMana=6;
	
	it_CreateItem	(kInfusedBlue,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedBlue, kEnchanting, 2, 1,kBlueCrystal,20, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedBlue].makeMana=6;
	
	it_CreateItem	(kInfusedYellow,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedYellow, kEnchanting, 3, 1,kYellowCrystal,30, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedYellow].makeMana=9;
	
	it_CreateItem	(kInfusedOrange,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedOrange, kEnchanting, 3, 1,kOrangeCrystal,30, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedOrange].makeMana=9;
	
	it_CreateItem	(kInfusedPurple,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedPurple, kEnchanting, 4, 1,kPurpleCrystal,40, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedPurple].makeMana=12;
	
	it_CreateItem	(kInfusedBlack,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedBlack, kEnchanting, 5, 1,kBlackCrystal,50, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedBlack].makeMana=15;
	
	//Pures...
	
	it_CreateItem	(kInfusedPGreen,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedPGreen, kEnchanting, 6, 1,kPureGreenCrystal,60, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedPGreen].makeMana=18;
	
	it_CreateItem	(kInfusedPRed,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedPRed, kEnchanting, 7, 1,kPureRedCrystal,70, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedPRed].makeMana=21;
	
	it_CreateItem	(kInfusedPBlue,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedPBlue, kEnchanting, 7, 1,kPureBlueCrystal,70, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedPBlue].makeMana=21;
	
	it_CreateItem	(kInfusedPYellow,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedPYellow, kEnchanting, 8, 1,kPureYellowCrystal,80, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedPYellow].makeMana=24;
	
	it_CreateItem	(kInfusedPOrange,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedPOrange, kEnchanting, 8, 1,kPureOrangeCrystal,80, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedPOrange].makeMana=24;
	
	it_CreateItem	(kInfusedPPurple,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedPPurple, kEnchanting, 9, 1,kPurePurpleCrystal,90, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedPPurple].makeMana=27;
	
	it_CreateItem	(kInfusedPBlack,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedPBlack, kEnchanting, 10, 1,kPureBlackCrystal,100, kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedPBlack].makeMana=30;
	
	//Other infusables
	
	it_CreateItem	(kInfusedFire,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedFire, kEnchanting, 10, 1,kFireCrystal,100,kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedFire].makeMana=30;
	
	it_CreateItem	(kInfusedIce,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedIce, kEnchanting, 10, 1,kIceCrystal,100,kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedIce].makeMana=30;
	
	it_CreateItem	(kInfusedCS,	kCantArm,	kInfusedCrystalType, 1000, 1, 40, kNoColor,0,0,-1);
	it_Ingredients	(kInfusedCS, kEnchanting, 10, 1,kCrystalSkull,100,kEtherite,0,0,0,0,0,0,1);
	itemList[kInfusedCS].makeMana=30;
	
	//misc
	
	it_CreateItem		(kEnchantedCloth,	kCantArm,	kMiscType,	1000,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kEnchantedCloth,	kEnchanting, 1, 1,kRollOfCloth, 2, kElementalDust, 10, kEtherite, 0, 0, 0,0, 1);
	itemList[kEnchantedCloth].makeMana	= 3;
	
	it_CreateItem		(kDiamondDust,	kCantArm,	kMiscType,	1000,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kDiamondDust, kEnchanting, 2, 1, kDiamond, 1, kHarpieBeak, 20, kEtherite, 0, 0, 0, 0, 1);
	itemList[kDiamondDust].makeMana	= 6;
	
	it_CreateItem		(kDiscretionRing,	kRightFinger,	kLesserRingType,	1239,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kDiscretionRing,	kEnchanting, 4, 1, kBlinkHoundFur, 1, kGoldRing, 40, kEtherite, 0, 0, 0, 0, 1);
	itemList[kDiscretionRing].makeMana	= 12;
	
	it_CreateItem		(kCharmOfUndeath,	kNeck,	kCharmType,	1147,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kCharmOfUndeath,	kEnchanting, 5, 1, kZombieBrain, 1, kInfusedBlack, 1, kRustyNecklace, 1, kBoneMageStaff, 50, kEtherite, 1);
	itemList[kCharmOfUndeath].makeMana	= 15;
	
	it_CreateItem		(kVialOfAcid,	kCantArm,	kMiscType,	1000,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kVialOfAcid,	kEnchanting, 4, 1, kAcidicGland, 1, kCubeGoo, 1, kElementalDust, 1, kSnailSlime,40, kEtherite, 2);
	itemList[kVialOfAcid].makeMana	= 12;
	
	it_CreateItem		(kRingOfStrength,	kRightFinger,	kLesserRingType,	1142,	1,	40,	kNoColor,0,0,-1);  //Dave 1142,	1,	40,	kNoColor,0,0,-1
	it_Ingredients		(kRingOfStrength,	kEnchanting, 5, 1, kGiantHeart, 1, kPlatinumRing, 1, kInfusedOrange, 50, kEtherite,0, 0, 1);
	itemList[kRingOfStrength].makeMana	= 15;
	
	it_CreateItem		(kRingOfStealth,	kRightFinger,	kLesserRingType,	1141,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kRingOfStealth,	kEnchanting, 6, 1, kInfusedBlue, 1, kBlinkHoundFur, 1, kPlatinumRing, 60, kEtherite,0, 0, 1);
	itemList[kRingOfStealth].makeMana	= 18;
	
	it_CreateItem		(kCharmOfSwiftness,	kNeck,	kCharmType,	1149,	1,	40,	kNoColor,0,0,-1); //Dave 1149,	1,	40,	kNoColor,0,0,-1
	it_Ingredients		(kCharmOfSwiftness, kEnchanting, 6, 1, kInfusedGreen, 1, kInfusedYellow, 4, kSpiderLeg, 5, kElementalDust, 60, kEtherite,1);
	itemList[kCharmOfSwiftness].makeMana	= 18;
	
	it_CreateItem		(kCharmOfLoyalty,	kNeck,	kCharmType,	1150,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kCharmOfLoyalty,	kEnchanting, 6, 1, kInfusedRed, 1, kNeviaFlower, 1, kGaperEye, 1, kTuskerTail, 60, kEtherite,1);
	itemList[kCharmOfLoyalty].makeMana	= 18;
	
	it_CreateItem		(kCharmOfPurity,	kNeck,	kCharmType,	1151,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kCharmOfPurity,	kEnchanting, 7, 1, kZombieBrain, 1, kInfusedBlack, 1, kRustyNecklace, 5, kElementalDust, 70, kEtherite,1);
	itemList[kCharmOfPurity].makeMana	= 21;
	
	it_CreateItem		(kRingOfBruteStrength,	kRightFinger,	kLesserRingType,	1139,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kRingOfBruteStrength,	kEnchanting, 8, 1, kInfusedPOrange, 1, kGiantHeart, 1, kPureCrystalRing, 1, kTuskerTail, 80, kEtherite,1);
	itemList[kRingOfBruteStrength].makeMana	= 24;
	
	it_CreateItem		(kLesserManaRing,	kRightFinger,	kLesserRingType,	1140,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kLesserManaRing,	kEnchanting, 9, 1, kInfusedFire, 1, kInfusedIce, 1, kPureCrystalRing, 90, kEtherite, 0,0,1);
	itemList[kLesserManaRing].makeMana	= 27;
	
	it_CreateItem		(kRingOfShadows,	kRightFinger,	kLesserRingType,	1146,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kRingOfShadows,	kEnchanting, 9, 1, kInfusedPYellow, 1, kSmallMoonFragment, 1, kBlinkHoundFur, 90, kEtherite,0,0,1);
	itemList[kRingOfShadows].makeMana	= 27;
	
	it_CreateItem		(kEssenceOfPower,	kCantArm,	kMiscType,	1000,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kEssenceOfPower,	kEnchanting, 11, 1, kInfusedCS, 1, kAncientRune, 110, kEtherite,0,0,0,0,1);
	itemList[kEssenceOfPower].makeMana	= 33;
	
	it_CreateItem		(kEssenceOfDurability,	kCantArm,	kMiscType,	1000,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kEssenceOfDurability, 	kEnchanting, 10, 1, kInfusedFire, 1, kMagicStone, 100, kEtherite,0,0,0,0,1);
	itemList[kEssenceOfDurability].makeMana	= 30;
	
	it_CreateItem		(kEssenceOfSpeed,	kCantArm,	kMiscType,	1000,	1,	40,	kNoColor,0,0,-1);
	it_Ingredients		(kEssenceOfSpeed,	kEnchanting,	12,	1,	kInfusedIce, 1, kSmallMoonFragment, 120, kEtherite,0, 0, 0, 0, 1);
	itemList[kEssenceOfSpeed].makeMana	= 36;
	
	//-------------------------------------------------------------------

	it_CreateItem		(kLightningBoltSpell, kCantArm, kSpellType, 1000, 0, 30, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kLightningBoltSpell, kMageryWizard, 5, 1, kElectricEel, 1, kVolcanicAsh, 0,0,0,0,0,0,1); // done

	it_CreateItem		(kFireFieldSpell, 		kCantArm, kSpellType, 1000, 0, 30, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kFireFieldSpell, 		kMageryWizard, 5, 1, kLotusFlower, 1, kVolcanicAsh, 1,kCoal,0,0,0,0,1); // done

	it_CreateItem		(kLesserHealSpell, 					kCantArm, kSpellType, 1000, 0, 6, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kLesserHealSpell, 					kMageryCleric, 1, 1, kGinseng, 0, 0, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kGreaterHealSpell, 					kCantArm, kSpellType, 1000, 0, 24, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kGreaterHealSpell, 					kMageryCleric, 4, 1, kGinseng, 1, kLotusFlower, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kRemoveCurseSpell, 					kCantArm, kSpellType, 1000, 0, 24, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kRemoveCurseSpell, 					kMageryCleric, 5, 1, kGinseng, 1, kLotusFlower, 1, kAmber,0,0,0,0,1);// done

	it_CreateItem		(kResurrectSpell, 					kCantArm, kSpellType, 1000, 0, 48, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kResurrectSpell, 					kMageryCleric, 8, 1, kGinseng, 1, kMandrakeRoot, 1, kDragonTooth,0,0,0,0,1);// done

	it_CreateItem		(kFullHealSpell, 					kCantArm, kSpellType, 1000, 0, 42, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kFullHealSpell, 					kMageryCleric, 7, 1, kGinseng, 1, kLotusFlower, 1, kDragonTooth,0,0,0,0,1);// done

	it_CreateItem		(kReflectiveArmorSpell, 					kCantArm, kSpellType, 1000, 0, 30, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kReflectiveArmorSpell, 					kMageryCleric, 5, 1, kLotusFlower, 1, kCoal, 1, kElectricEel,0,0,0,0,1);// done

	it_CreateItem		(kClumsySpell, 					kCantArm, kSpellType, 1000, 0, 12, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kClumsySpell, 					kMageryWizard, 2, 1, kLotusFlower, 1, kPoisonGland, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kCreateFoodSpell, 					kCantArm, kSpellType, 1000, 0, 24, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kCreateFoodSpell, 					kMageryDruid, 4, 1, kGinseng, 1, kAmber, 1, kGarlic,0,0,0,0,1);// done

	it_CreateItem		(kTurnUndeadSpell, 					kCantArm, kSpellType, 1000, 0, 42, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kTurnUndeadSpell, 					kMageryCleric, 7, 1, kLotusFlower, 1, kCoal, 1, kGarlic,0,0,0,0,1);// done

	it_CreateItem		(kSimpleMindSpell, 					kCantArm, kSpellType, 1000, 0, 6, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kSimpleMindSpell, 					kMageryWizard, 1, 1, kLotusFlower, 0, 0, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kNightVisionSpell, 					kCantArm, kSpellType, 1000, 0, 12, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kNightVisionSpell, 					kMageryDruid, 2, 1, kAmber, 1, kVolcanicAsh, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kWeakenSpell, 					kCantArm, kSpellType, 1000, 0, 12, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kWeakenSpell, 					kMageryWizard, 2, 1, kLotusFlower, 1, kElectricEel, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kAgilitySpell, 					kCantArm, kSpellType, 1000, 0, 24, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kAgilitySpell, 					kMageryCleric, 4, 1, kGinseng, 1, kMandrakeRoot, 1, kCoal,0,0,0,0,1);// done

	it_CreateItem		(kElevateMindSpell, 					kCantArm, kSpellType, 1000, 0, 12, kNoColor,0,0,-1);  //----------------------------

	it_Ingredients	(kElevateMindSpell, 					kMageryCleric, 2, 1, kGinseng, 1, kLotusFlower, 1, kCoal,0,0,0,0,1);// done

	it_CreateItem	(kFireballSpell,							kCantArm, kSpellType, 1000, 0, 54, kNoColor, 0, 0, -1);

	it_Ingredients	(kFireballSpell,						kMageryWizard, 9, 1, kVolcanicAsh,1,kCoal,1,kDragonTooth,0,0,0,0,1);

	it_CreateItem	(kMagicMissileSpell,							kCantArm, kSpellType, 1000, 0, 6, kNoColor, 0, 0, -1);

	it_Ingredients	(kMagicMissileSpell,						kMageryWizard, 1, 1, kCoal,0,0,0,0,0,0,0,0,1);

	//it_CreateItem	(kFireblast3Spell,							kCantArm, kSpellType, 1000, 0, 6, kNoColor, 0, 0, -1);
	//it_Ingredients	(kFireblast3Spell,						kMageryWizard, 20, 1, kVolcanicAsh,0,0,0,0,0,0,0,0,1);

	it_CreateItem	(kFireblast4Spell,							kCantArm, kSpellType, 1000, 0, 6, kNoColor, 0, 0, -1);
	it_Ingredients	(kFireblast4Spell,						kMageryWizard, 20, 1, kVolcanicAsh, 1, kDragonTooth,0,0,0,0,0,0,1);

	//it_CreateItem	(kFireblast5Spell,							kCantArm, kSpellType, 1000, 0, 6, kNoColor, 0, 0, -1);
	//it_Ingredients	(kFireblast5Spell,						kMageryWizard, 20, 1, kVolcanicAsh,0,0,0,0,0,0,0,0,1);

	it_CreateItem		(kLesserDetoxifySpell, 					kCantArm, kSpellType, 1000, 0, 6, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kLesserDetoxifySpell, 					kMageryCleric, 1, 1, kMandrakeRoot, 0, 0, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kGreaterDetoxifySpell, 					kCantArm, kSpellType, 1000, 0, 24, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kGreaterDetoxifySpell, 					kMageryCleric, 4, 1, kLotusFlower, 1, kMandrakeRoot, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kFullDetoxifySpell, 					kCantArm, kSpellType, 1000, 0, 36, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kFullDetoxifySpell, 					kMageryCleric, 6, 1, kLotusFlower, 1, kMandrakeRoot, 1, kDragonTooth,0,0,0,0,1);// done

	it_CreateItem		(kMagicTrapSpell, 					kCantArm, kSpellType, 1000, 0, 18, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kMagicTrapSpell, 					kMageryWizard, 3, 1, kCoal, 1, kAmber, 1, kVolcanicAsh,0,0,0,0,1);  // done

	it_CreateItem		(kMagicDisarmSpell, 					kCantArm, kSpellType, 1000, 0, 18, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kMagicDisarmSpell, 					kMageryWizard, 3, 1, kMandrakeRoot, 1, kLotusFlower, 1, kAmber,0,0,0,0,1);

	it_CreateItem		(kStrengthSpell, 					kCantArm, kSpellType, 1000, 0, 18, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kStrengthSpell, 					kMageryCleric, 3, 1, kGinseng, 1, kCoal, 1, kGarlic,0,0,0,0,1); // done

	it_CreateItem		(kSummonPetSpell, 					kCantArm, kSpellType, 1000, 0, 30, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kSummonPetSpell, 					kMageryDruid, 5, 1, kMandrakeRoot, 1, kAmber, 1, kCoal,0,0,0,0,1);  // done

	it_CreateItem		(kHealPetSpell, 					kCantArm, kSpellType, 1000, 0, 22, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kHealPetSpell, 					kMageryDruid, 1, 1, kGinseng, 1, kAmber, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kPoisonSpell, 					kCantArm, kSpellType, 1000, 0, 18, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kPoisonSpell, 					kMageryWizard, 2, 1, kCoal, 1, kPoisonGland, 0, 0,0,0,0,0,1); // done

	it_CreateItem		(kTeleportSpell, 					kCantArm, kSpellType, 1000, 0, 36, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kTeleportSpell, 					kMageryDruid, 6, 1, kLotusFlower, 1, kVolcanicAsh, 1, kAmber,0,0,0,0,1);  // done

	it_CreateItem		(kDrainManaSpell, 					kCantArm, kSpellType, 1000, 0, 30, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kDrainManaSpell, 					kMageryWizard, 5, 1, kLotusFlower, 1, kPoisonGland, 0, 0,0,0,0,0,1);  // done

	it_CreateItem		(kMagicReflectionSpell, 					kCantArm, kSpellType, 1000, 0, 30, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kMagicReflectionSpell, 					kMageryCleric, 5, 1, kLotusFlower, 1, kAmber, 1, kCoal,0,0,0,0,1);  // done

	it_CreateItem		(kDisorientationSpell, 					kCantArm, kSpellType, 1000, 0, 36, kNoColor,0,0,-1); //----------------------------

	it_Ingredients	(kDisorientationSpell, 					kMageryWizard, 6, 1, kGarlic, 1, kPoisonGland, 1, kElectricEel,0,0,0,0,1);  // done

	it_CreateItem		(kParalyzeSpell, 					kCantArm, kSpellType, 1000, 0, 24, kNoColor,0,0,-1); //------------------------- ---

	it_Ingredients	(kParalyzeSpell, 					kMageryWizard, 4, 1, kMandrakeRoot, 1, kGarlic, 1, kElectricEel,0,0,0,0,1); // done

	it_CreateItem		(kSummonCreatureSpell, 					kCantArm, kSpellType, 1000, 0, 42, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kSummonCreatureSpell, 					kMageryDruid, 7, 1, kLotusFlower, 1, kMandrakeRoot, 1, kAmber,0,0,0,0,1); // done

	//Dave 02-06-2011
	it_CreateItem		(kGreaterSummonCreatureSpell, 					kCantArm, kSpellType, 1000, 0, 52, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kGreaterSummonCreatureSpell, 					kMageryDruid, 10, 1, kLotusFlower, 1, kMandrakeRoot, 1, kAmber, 1, kGinseng,0,0,1); // done

	
	it_CreateItem		(kExplosionSpell, 					kCantArm, kSpellType, 1000, 0, 24, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kExplosionSpell, 					kMageryWizard, 4, 1, kVolcanicAsh, 1, kCoal, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kInvisibilitySpell, 					kCantArm, kSpellType, 1000, 0, 18, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kInvisibilitySpell, 					kMageryDruid, 3, 1, kLotusFlower, 1, kAmber, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kOpportunitySpell, 					kCantArm, kSpellType, 1000, 0, 60, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kOpportunitySpell, 					kMageryDruid, 10, 1, kAmber, 1, kCoal, 1, kElectricEel, 1, kDragonTooth,0,0,1);// done

	it_CreateItem		(kRevealSpell, 					kCantArm, kSpellType, 1000, 0, 6, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kRevealSpell, 					kMageryDruid, 1, 1, kAmber, 0, 0, 0, 0,0,0,0,0,1);// done

	it_CreateItem		(kPoisonFieldSpell, 					kCantArm, kSpellType, 1000, 0, 48, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kPoisonFieldSpell, 					kMageryWizard, 8, 1, kCoal, 1, kPoisonGland, 1, kDragonTooth,0,0,0,0,1);// done

	it_CreateItem		(kMassHealSpell, 					kCantArm, kSpellType, 1000, 0, 60, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kMassHealSpell, 					kMageryCleric, 10, 3, kGinseng, 1, kDragonTooth, 0,0,0,0,0,0,1);// done

	it_CreateItem		(kDisorientFieldSpell, 					kCantArm, kSpellType, 1000, 0, 60, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kDisorientFieldSpell, 					kMageryWizard, 10, 1, kGarlic, 1, kPoisonGland, 1, kDragonTooth,0,0,0,0,1);// done

	it_CreateItem		(kParalyzeFieldSpell, 					kCantArm, kSpellType, 1000, 0, 42, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kParalyzeFieldSpell, 					kMageryWizard, 7, 1, kGarlic, 1, kDragonTooth, 1, kElectricEel,0,0,0,0,1);// done

	it_CreateItem		(kProvocationSpell, 					kCantArm, kSpellType, 1000, 0, 48, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kProvocationSpell, 					kMageryDruid, 8, 1, kGarlic, 1, kDragonTooth, 1, kAmber,0,0,0,0,1);// done

	it_CreateItem		(kCalmSpell, 					kCantArm, kSpellType, 1000, 0, 24, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kCalmSpell, 					kMageryDruid, 4, 1, kLotusFlower, 1, kCoal, 1, kAmber,0,0,0,0,1);// done
	//Shapeshift spells
	it_CreateItem		(kShapeshiftLesserSpell, 					kCantArm, kSpellType, 1000, 0, 61, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kShapeshiftLesserSpell, 					kMageryDruid, 5, 1, kGinseng, 1, kGarlic, 1 , kMandrakeRoot, 1, kAmber,0,0,1);// done lvl 3
	
	it_CreateItem		(kShapeshiftSpell, 					kCantArm, kSpellType, 1000, 0, 76, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kShapeshiftSpell, 					kMageryDruid, 8, 1, kGinseng, 1, kGarlic, 1 , kDragonTooth, 1, kAmber,0,0,1);// done lvl 3

	it_CreateItem		(kShapeshiftGreaterSpell, 					kCantArm, kSpellType, 1000, 0, 92, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kShapeshiftGreaterSpell, 					kMageryDruid, 11, 1, kGinseng, 1, kDragonTooth, 1 , kMandrakeRoot, 1, kAmber,0,0,1);// done lvl 3

	it_CreateItem		(kShapeshiftAllSpell, 					kCantArm, kSpellType, 1000, 0, 5, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kShapeshiftAllSpell, 					kMageryDruid, 20, 1, kGinseng, 1, kGarlic, 0,0,0,0,0,0,1);// done lvl 7

	it_CreateItem		(kShapeshiftDragonSpell, 					kCantArm, kSpellType, 1000, 0, 60, kNoColor,0,0,-1);//----------------------------

	it_Ingredients	(kShapeshiftDragonSpell, 					kMageryDruid, 20, 1, kGinseng, 1, kDragonTooth, 0,0,0,0,0,0,1);// done lvl 12
/**/
//	it_CreateItem		(kManaStealSpell, 					kCantArm, kSpellType, 1000, 0, 1, kNoColor,0,0);
//	it_Ingredients	(kManaStealSpell, 					kMageryWizard, 0, 1, kGinseng, 1, kGarlic, 1, kLotusFlower,0,0,0,0,1);

	//Dave
	
	
	it_CreateItem		(kChaosFieldSpell, 					kCantArm, kSpellType, 1000, 0, 4, kNoColor,0,0,-1);//----------------------------

	//it_Ingredients	(kChaosFieldSpell, 					kMageryWizard, 20, 1, kLotusFlower, 0,0,0,0,0,0,0,0,1);// done


	//-------------------------------------------------------------------
	
	//rogue
	it_CreateItem	(kAttackInstantStrike, kCantArm, kSpecialAttackType, 1000, 0, 15, kNoColor,0,0,-1);
	it_Ingredients	(kAttackInstantStrike, kSpecialRogue, 1,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackSting, kCantArm, kSpecialAttackType, 1000, 0, 20, kNoColor,0,0,-1);
	it_Ingredients	(kAttackSting, kSpecialRogue, 3,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackVial, kCantArm, kSpecialAttackType, 1000, 0, 35, kNoColor,0,0,-1);
	it_Ingredients	(kAttackVial, kSpecialRogue, 5,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackBackstab, kCantArm, kSpecialAttackType, 1000, 0, 40, kNoColor,0,0,-1);
	it_Ingredients	(kAttackBackstab, kSpecialRogue, 6,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackDecimate, kCantArm, kSpecialAttackType, 1000, 0, 55, kNoColor,0,0,-1);
	it_Ingredients	(kAttackDecimate, kSpecialRogue, 9,0,0,0,0,0,0,0,0,0,0,1);
	
	//ranger
	
	it_CreateItem	(kAttackDoubleShot, kCantArm, kSpecialAttackType, 1000, 0, 10, kNoColor,0,0,-1);
	it_Ingredients	(kAttackDoubleShot, kSpecialRanger, 1,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackGrapplingShot, kCantArm, kSpecialAttackType, 1000, 0, 25, kNoColor,0,0,-1);
	it_Ingredients	(kAttackGrapplingShot, kSpecialRanger, 7,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackEscapeMove, kCantArm, kSpecialAttackType, 1000, 0, 35, kNoColor,0,0,-1);
	it_Ingredients	(kAttackEscapeMove, kSpecialRanger, 4,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackAimedShot, kCantArm, kSpecialAttackType, 1000, 0, 20, kNoColor,0,0,-1);
	it_Ingredients	(kAttackAimedShot, kSpecialRanger, 5,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackRangerSwiftness, kCantArm, kSpecialAttackType, 1000, 0, 15, kNoColor,0,0,-1);
	it_Ingredients	(kAttackRangerSwiftness, kSpecialRanger, 2,0,0,0,0,0,0,0,0,0,0,1);
	
	//fighter
	
	it_CreateItem	(kAttackBoomingBlow, kCantArm, kSpecialAttackType, 1000, 0, 50, kNoColor,0,0,-1);
	it_Ingredients	(kAttackBoomingBlow, kSpecialFighter, 7,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackDoubleStrike, kCantArm, kSpecialAttackType, 1000, 0, 20, kNoColor,0,0,-1);
	it_Ingredients	(kAttackDoubleStrike, kSpecialFighter, 2,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackQuickDraw, kCantArm, kSpecialAttackType, 1000, 0, 20, kNoColor,0,0,-1);
	it_Ingredients	(kAttackQuickDraw, kSpecialFighter, 3,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackLesserDefense, kCantArm, kSpecialAttackType, 1000, 0, 30, kNoColor,0,0,-1);
	it_Ingredients	(kAttackLesserDefense, kSpecialFighter, 4,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackGreaterDefense, kCantArm, kSpecialAttackType, 1000, 0, 30, kNoColor,0,0,-1);
	it_Ingredients	(kAttackGreaterDefense, kSpecialFighter, 8,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackRage, kCantArm, kSpecialAttackType, 1000, 0, 60, kNoColor,0,0,-1);
	it_Ingredients	(kAttackRage, kSpecialFighter, 6,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackKnockback, kCantArm, kSpecialAttackType, 1000, 0, 25, kNoColor,0,0,-1);
	it_Ingredients	(kAttackKnockback, kSpecialFighter, 5,0,0,0,0,0,0,0,0,0,0,1);
	
	it_CreateItem	(kAttackTaunt, kCantArm, kSpecialAttackType, 1000, 0, 10, kNoColor,0,0,-1);
	it_Ingredients	(kAttackTaunt, kSpecialFighter, 1,0,0,0,0,0,0,0,0,0,0,1);

	//-------------------------------------------------------------------

	it_CreateItem   (kRoastedGaperStalk, 			kCantArm, kCookedMeatType, 1000, 1, 6, kNoColor,0,0,-1);

	it_Ingredients	(kRoastedGaperStalk, 			kCooking, 3, 1, kGaperStalk, 0, 0, 0, 0,0,0,0,0,1); //done

	it_CreateItem   (kCandy, 			      kCantArm, kCookedVegType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem   (kRedEgg, 			      kCantArm, kCookedVegType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem   (kPurpleEgg, 			    kCantArm, kCookedVegType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem   (kGreenEgg, 			    kCantArm, kCookedVegType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem   (kBlueEgg, 			      kCantArm, kCookedVegType, 1000, 1, 4, kNoColor,0,0,-1);
	//Dave add black egg 11-28-2010
	it_CreateItem   (kBlackEgg, 			   kCantArm, kCookedVegType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem   (kBlueFlag, 			    kCantArm, kMiscType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem   (kRedFlag, 			      kCantArm, kMiscType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem   (kGreenFlag, 			    kCantArm, kMiscType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem   (kYellowFlag, 			  kCantArm, kMiscType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem   (kLoafOfBread, 			kCantArm, kCookedVegType, 1000, 1, 4, kNoColor,0,0,-1);

	it_Ingredients	(kLoafOfBread, 			kCooking, 1, 1, kBagOfFlour, 0, 0, 0, 0,0,0,0,0,1); //done

	it_CreateItem   (kApple, 				    kCantArm, kCookedVegType, 1000, 1, 2, kNoColor,0,0,-1);

	it_CreateItem   (kBanana, 				  kCantArm, kCookedVegType, 1000, 1, 2, kNoColor,0,0,-1);

	it_CreateItem   (kBagOfFlour, 			kCantArm, kMiscType, 1000, 1, 2, kNoColor,0,0,-1);

	it_CreateItem		(kCookedSteak, 			kCantArm, kCookedMeatType, 1000, 2, 5, kNoColor,0,0,-1);
	
	it_Ingredients	(kCookedSteak, 			kCooking, 2, 1, kRawSteak, 0, 0, 0, 0,0,0,0,0,1); //done

	it_CreateItem		(kFishSteak, 				kCantArm, kCookedMeatType, 1000, 1, 2, kNoColor,0,0,-1);

	it_Ingredients	(kFishSteak, 				kCooking, 1, 1, kTrout, 0, 0, 0, 0,0,0,0,0,3); //done

	it_CreateItem		(kDriedTrout, 			kCantArm, kCookedMeatType, 1000, 3, 5, kNoColor,0,0,-1);

	it_Ingredients	(kDriedTrout, 			kCooking, 3, 1, kTrout, 0, 0, 0, 0,0,0,0,0,1); //done

	it_CreateItem		(kEelPie, 					kCantArm, kCookedMeatType, 1000, 2, 5, kNoColor,0,0,-1);

	it_Ingredients	(kEelPie, 					kCooking, 6, 2, kElectricEel, 1, kBagOfFlour, 0, 0,0,0,0,0,1); //done

	it_CreateItem		(kLobsterPie, 			kCantArm, kCookedMeatType, 1000, 2, 15, kNoColor,0,0,-1);

	it_Ingredients	(kLobsterPie, 			kCooking, 7, 1, kLobster, 1, kBagOfFlour, 0, 0,0,0,0,0,1); //done

	it_CreateItem		(kBoiledLobster, 		kCantArm, kCookedMeatType, 1000, 2, 15, kNoColor,0,0,-1);

	it_CreateItem		(kApplePie, 				kCantArm, kCookedVegType, 1000, 2, 5, kNoColor,0,0,-1);

	it_Ingredients	(kApplePie, 				kCooking, 4, 5, kApple, 1, kBagOfFlour, 0, 0,0,0,0,0,1); //done

	it_CreateItem		(kBananaPie, 				kCantArm, kCookedVegType, 1000, 2, 5, kNoColor,0,0,-1);

	it_Ingredients	(kBananaPie, 				kCooking, 5, 5, kBanana, 1, kBagOfFlour, 0, 0,0,0,0,0,1); //done
	//Dave
	it_CreateItem		(kGarlicBread, 				kCantArm, kCookedVegType, 1000, 1, 2, kNoColor,0,0,-1);

	it_Ingredients	(kGarlicBread, 				kCooking, 2, 4, kGarlic, 2, kBagOfFlour, 0, 0,0,0,0,0,1); //done
	
	it_CreateItem		(kSteakFlambe, 				kCantArm, kCookedMeatType, 1000, 2, 10, kNoColor,0,0,-1);

	it_Ingredients	(kSteakFlambe, 				kCooking, 8, 1, kRawSteak, 2, kMaraliWhiskey, 1, kMushroom,0,0,0,0,1); //done
	
	it_CreateItem		(kSushi, 				kCantArm, kCookedMeatType, 1000, 1, 10, kNoColor,0,0,-1);

	it_Ingredients	(kSushi, 				kCooking, 9, 2, kLotusFlower, 2, kElectricEel, 2, kGinseng, 1, kTuna,0,0,1); //done
	
	it_CreateItem		(kRedOmelette, 				kCantArm, kCookedMeatType, 1000, 1, 20, kNoColor,0,0,-1);

	it_Ingredients	(kRedOmelette, 				kCooking, 5, 2, kRedEgg, 2, kSheepsMilk, 1, kMushroom,0,0,0,0,1); //done

	it_CreateItem		(kPurpleOmelette, 				kCantArm, kCookedMeatType, 1000, 1, 50, kNoColor,0,0,-1);

	it_Ingredients	(kPurpleOmelette, 				kCooking, 7, 2, kPurpleEgg, 2, kSheepsMilk, 1, kMushroom,0,0,0,0,1); //done
	
	it_CreateItem		(kRumCake, 				kCantArm, kCookedVegType, 1000, 3, 100, kNoColor,0,0,-1);

	it_Ingredients	(kRumCake, 				kCooking, 10, 5, kRedEgg, 2, kSheepsMilk, 1, kRedNatesRotgutRum, 1, kBagOfFlour,0,0,1); //done
	
	it_CreateItem	(kMushroom, 				kCantArm, kBagOfFlour, 1000, 1, 2, kNoColor,0,0,-1);

	it_CreateItem		(kCandiedApple, 				kCantArm, kCookedVegType, 1000, 1, 10, kNoColor,0,0,-1);

	it_Ingredients	(kCandiedApple, 				kCooking, 1, 1, kCandy, 1, kApple, 0,0,0,0,0,0,1); //done
	
	it_CreateItem		(kPizza, 				kCantArm, kCookedVegType, 1000, 1, 38, kNoColor,0,0,-1);

	it_Ingredients	(kPizza, 				kCooking, 11, 3, kMushroom, 1, kBagOfFlour, 1, kGarlic, 1, kSheepsMilk,0,0,1); //done
	
	it_CreateItem		(kPumpkinPie, 				kCantArm, kCookedVegType, 1000, 1, 10, kNoColor,0,0,-1);

	//it_Ingredients	(kPumpkinPie, 				kCooking, 13, 3, kMushroom, 1, kBagOfFlour, 1, kGarlic, 1, kSheepsMilk,0,0,1); //done
	
	it_CreateItem		(kZombieBrainStew, 				kCantArm, kCookedMeatType, 1000, 1, 30, kNoColor,0,0,-1);

	it_Ingredients	(kZombieBrainStew, 				kCooking, 12, 1, kZombieBrain, 1, kWaterBottle, 2, kGarlic, 1, kMushroom,0,0,1); //done
	
	it_CreateItem		(kMandrakeCookie, 				kCantArm, kCookedVegType, 1000, 1, 2, kNoColor,0,0,-1);

	it_Ingredients	(kMandrakeCookie, 				kCooking, 6, 6, kMandrakeRoot, 1, kBagOfFlour, 1, kSheepsMilk, 0,0,0,0,12); //done
	
	it_CreateItem		(kSakeSeaBass, 				kCantArm, kCookedVegType, 1000, 1, 85, kNoColor,0,0,-1);

	it_Ingredients	(kSakeSeaBass, 				kCooking, 13, 1, kHayateSake, 1, kSeaBass, 1, kGarlic, 1, kMushroom,0,0,1); //done
	
	it_CreateItem		(kStuffedGiantHeart, 				kCantArm, kCookedMeatType, 1000, 1, 60, kNoColor,0,0,-1);

	it_Ingredients	(kStuffedGiantHeart, 				kCooking, 9, 1, kGiantHeart, 2, kGaperStalk, 1, kGarlic, 2, kMushroom,0,0,1); //done
														
	it_CreateItem		(kEggNog, 				kCantArm, kCookedVegType, 1000, 1, 60, kNoColor,0,0,-1);

	it_Ingredients	(kEggNog, 				kCooking, 4,  1, kRedNatesRotgutRum, 1, kSheepsMilk, 2, kRedEgg, 0,0,0,0,1); //done
										
	it_CreateItem		(kAndrisBananaCake, 				kCantArm, kCookedMeatType, 1000, 1, 60, kNoColor,0,0,-1);

	it_Ingredients	(kAndrisBananaCake, 				kCooking, 8, 1, kAndrisChampagne, 1, kSheepsMilk, 1, kRedEgg, 1,kBagOfFlour, 2, kBanana,1); //done
	
	it_CreateItem		(kSharkFinSoup, 				kCantArm, kCookedMeatType, 1000, 1, 60, kNoColor,0,0,-1);

	it_Ingredients	(kSharkFinSoup, 				kCooking, 6, 1, kSharkFin, 1, kSheepsMilk, 1, kGarlic, 1,kMushroom, 0, 0,1); //done
	
	it_CreateItem		(kTrollHouseCookie, 				kCantArm, kCookedMeatType, 1000, 1, 60, kNoColor,0,0,-1);

	it_Ingredients	(kTrollHouseCookie, 				kCooking, 11, 1, kBagOfFlour, 1, kSheepsMilk, 1, kGohoranChocolate, 1,kRedEgg, 0, 0,13); //done
	
	/*
	
	it_CreateItem		(kLesserHealPotion, 					kCantArm, kPotionType, 1000, 2, 30, kNoColor,0,0,-1); //done

	it_Ingredients	(kLesserHealPotion, kAlchemyCleric, 1, 1, kEmptyBottle, 1, kGinseng, 0,0,0,0,0,0,1);*/
	
	
	it_CreateItem(kGingerBread, 				kCantArm, kCookedVegType, 1000, 2, 6, kNoColor,0,0,-1);

	it_CreateItem(kOatMealCake, 				kCantArm, kCookedVegType, 1000, 2, 5, kNoColor,0,0,-1);

	it_CreateItem(kLobsterSoup, 				kCantArm, kCookedMeatType, 1000, 2, 12, kNoColor,0,0,-1);
	//Dave
	it_Ingredients	(kLobsterSoup, 				kCooking, 12, 1, kMushroom, 1, kSheepsMilk, 1, kGarlic, 2, kLobster, 0,0,1); //done
	
	it_CreateItem(kMugOfAle, 				    kCantArm, kBeverageType2, 1000, 1, 2, kNoColor,0,0,-1);
	
	it_CreateItem(kWaterBottle, 				kCantArm, kBeverageType3, 1000, 1, 1, kNoColor,0,0,-1);
	
	it_CreateItem(kRedWine, 				 	kCantArm, kBeverageType, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem(kWhiteWine, 				 	kCantArm, kBeverageType, 1000, 1, 4, kNoColor,0,0,-1);
	
	it_CreateItem(kMirithPaleAle, 				kCantArm, kBeverageType2, 1000, 1, 3, kNoColor,0,0,-1);
	
	it_CreateItem(kMaraliWhiskey, 				kCantArm, kBeverageType, 1000, 1, 8, kNoColor,0,0,-1);
	
	it_CreateItem(kAndrisChampagne, 			kCantArm, kBeverageType, 1000, 1, 15, kNoColor,0,0,-1);
	
	it_CreateItem(kBuckeyeBeer, 				kCantArm, kBeverageType4, 1000, 1, 1000, kNoColor,0,0,-1);
	
	it_CreateItem(kLerilinCremeale, 			kCantArm, kBeverageType2, 1000, 1, 4, kNoColor,0,0,-1);

	it_CreateItem(kRedNatesRotgutRum, 			kCantArm, kBeverageType, 1000, 1, 6, kNoColor,0,0,-1);
	
	it_CreateItem(kMead, 						kCantArm, kBeverageType2, 1000, 1, 4, kNoColor,0,0,-1);
	
	it_CreateItem(kBlueMoonGin, 				kCantArm, kBeverageType, 1000, 1, 7, kNoColor,0,0,-1);
	
	it_CreateItem(kHayateSake, 					kCantArm, kBeverageType, 1000, 1, 6, kNoColor,0,0,-1);
	
	it_CreateItem(kSheepsMilk, 				 	kCantArm, kBeverageType3, 1000, 1, 2, kNoColor,0,0,-1);
	
	it_CreateItem(kPortGastMoonshine, 			kCantArm, kBeverageType, 1000, 1, 7, kNoColor,0,0,-1);
	
	it_CreateItem(kFoehanVodka, 				kCantArm, kBeverageType, 1000, 1, 7, kNoColor,0,0,-1);
	
	it_CreateItem		(kEggNog, 				kCantArm, kBeverageType3, 1000, 1, 60, kNoColor,0,0,-1);

	it_Ingredients	(kEggNog, 				kCooking, 4, 1, kRedNatesRotgutRum, 1, kSheepsMilk, 2, kRedEgg, 0,0,0,0,1); //done
	//Dave add 11-25-2012 elixers
	it_CreateItem		(kElixirHeart, 				kCantArm, kElixirType1, 1000, 1, 60, kNoColor,0,0,-1);
	it_CreateItem		(kElixirDuh, 				kCantArm, kElixirType2, 1000, 1, 60, kNoColor,0,0,-1);
	it_CreateItem		(kElixirZimm, 				kCantArm, kElixirType3, 1000, 1, 60, kNoColor,0,0,-1);
	it_CreateItem		(kElixirKapow, 				kCantArm, kElixirType4, 1000, 1, 60, kNoColor,0,0,-1);
	it_CreateItem		(kElixirTingle, 			kCantArm, kElixirType5, 1000, 1, 60, kNoColor,0,0,-1);
	
	
	
	it_CreateItem		(kMorgansGingerbread, 				kCantArm, kCookedVegType, 1000, 1, 160, kNoColor,0,0,-1);
	
	it_CreateItem		(kBirthdayCake, 				kCantArm, kCookedVegType, 1000, 1, 60, kNoColor,0,0,-1);

	it_Ingredients	(kBirthdayCake, 				kCooking, 5, 1, kRedEgg, 1, kSheepsMilk, 1, kBagOfFlour, 3, kCandle,0,0,1); //done
	
	it_CreateItem		(kGohoranChocolate, 				kCantArm, kCookedVegType, 1000, 1, 20, kNoColor,0,0,-1);
	//-------------------------------------------------------------------

	it_CreateItem(kTrout, 							kCantArm, kRawMeatType, 1000, 3, 3, kNoColor,0,0,-1);

	it_CreateItem(kLobster, 						kCantArm, kRawMeatType, 1000, 3, 15, kNoColor,0,0,-1);

	it_CreateItem(kElectricEel, 				kCantArm, kRawMeatType, 1000, 1, 8, kNoColor,0,0,-1);

	it_CreateItem(kRawSteak, 						kCantArm, kRawMeatType, 1000, 2, 2, kNoColor,0,0,-1);

	it_CreateItem(kTurtleShell, 				kCantArm, kMiscType, 		1000, 10, 20, kNoColor,0,0,-1);

	it_CreateItem(kBundleOfWool, 				kCantArm, kMiscType, 		1000, 5, 20, kNoColor,0,0,-1);

	//-------------------------------------------------------------------

	it_CreateItem(kLightBlueDye, 				kCantArm, kDyeType, 1000, 1, 1, kLightBlue,0,0,-1);

	it_CreateItem(kRedDye, 							kCantArm, kDyeType, 1000, 1, 2, kRed,0,0,-1);

	it_CreateItem(kBlueDye, 						kCantArm, kDyeType, 1000, 1, 3, kBlue,0,0,-1);

	it_CreateItem(kBrownDye, 						kCantArm, kDyeType, 1000, 1, 1, kBrown,0,0,-1);

	it_CreateItem(kBlackDye, 						kCantArm, kDyeType, 1000, 1, 8, kBlack,0,0,-1);

	it_CreateItem(kGreenDye, 						kCantArm, kDyeType, 1000, 1, 3, kGreen,0,0,-1);

	it_CreateItem(kPurpleDye, 					kCantArm, kDyeType, 1000, 1, 2, kPurple,0,0,-1);

	it_CreateItem(kYellowDye, 					kCantArm, kDyeType, 1000, 1, 2, kYellow,0,0,-1);
	//Dave
	it_CreateItem(kGrayDye, 					kCantArm, kDyeType, 1000, 1, 2, kGray,0,0,-1);
	
	it_CreateItem(kOrangeDye, 					kCantArm, kDyeType, 1000, 1, 2, kOrange,0,0,-1);

	//-------------------------------------------------------------------

	it_CreateItem		(kGrayWizardHat, 					kHead, kWizardHatType, 1062, 3, 50, kGray,1,0,-1);
	it_Ingredients		(kGrayWizardHat, kTailoringLeatherworking, 3, 2, kEnchantedCloth, 0,0,0,0,0,0,0,0,1);

	it_CreateItem		(kLightBlueWizardHat, 		kHead, kWizardHatType, 1069, 3, 50, kLightBlue,1,0,-1);
	it_Ingredients		(kLightBlueWizardHat, kTailoringLeatherworking, 3, 2, kEnchantedCloth, 1,kLightBlueDye,0,0,0,0,0,0,1);

	it_CreateItem		(kRedWizardHat, 					kHead, kWizardHatType, 1070, 3, 50, kRed,1,0,-1);
	it_Ingredients		(kRedWizardHat, kTailoringLeatherworking, 3, 2, kEnchantedCloth, 1,kRedDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kBlueWizardHat, 					kHead, kWizardHatType, 1063, 3, 50, kBlue,1,0,-1);
	it_Ingredients		(kBlueWizardHat, kTailoringLeatherworking, 3, 2, kEnchantedCloth, 1,kBlueDye,0,0,0,0,0,0,1);

	it_CreateItem		(kBrownWizardHat, 				kHead, kWizardHatType, 1064, 3, 50, kBrown,1,0,-1);
	it_Ingredients		(kBrownWizardHat, kTailoringLeatherworking, 3, 2, kEnchantedCloth, 1,kBrownDye,0,0,0,0,0,0,1);

	it_CreateItem		(kBlackWizardHat, 				kHead, kWizardHatType, 1071, 3, 50, kBlack,1,0,-1);
	it_Ingredients		(kBlackWizardHat, kTailoringLeatherworking, 3, 2, kEnchantedCloth, 1,kBlackDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kGreenWizardHat, 				kHead, kWizardHatType, 1068, 3, 50, kGreen,1,0,-1);
	it_Ingredients		(kGreenWizardHat, kTailoringLeatherworking, 3, 2, kEnchantedCloth, 1,kGreenDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kPurpleWizardHat, 				kHead, kWizardHatType, 1067, 3, 50, kPurple,1,0,-1);
	it_Ingredients		(kPurpleWizardHat, kTailoringLeatherworking, 3, 2, kEnchantedCloth, 1,kPurpleDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kYellowWizardHat, 				kHead, kWizardHatType, 1072, 3, 50, kYellow,1,0,-1);
	it_Ingredients		(kYellowWizardHat, kTailoringLeatherworking, 3, 2, kEnchantedCloth, 1,kYellowDye,0,0,0,0,0,0,1);

	it_CreateItem		(kGrayJesterHat, 					kHead, kJesterHatType, 1123, 3, 50, kGray,1,0,-1);
	it_Ingredients		(kGrayJesterHat, kTailoringLeatherworking, 6, 1, kRollOfCloth, 0,0,0,0,0,0,0,0,1);
	
	it_CreateItem		(kLightBlueJesterHat, 		kHead, kJesterHatType, 1124, 3, 50, kLightBlue,1,0,-1);
	it_Ingredients		(kLightBlueJesterHat, kTailoringLeatherworking, 6, 1, kRollOfCloth, 1, kLightBlueDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kRedJesterHat, 					kHead, kJesterHatType, 1125, 3, 50, kRed,1,0,-1);
	it_Ingredients		(kRedJesterHat, kTailoringLeatherworking, 6, 1, kRollOfCloth, 1, kRedDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kBlueJesterHat, 					kHead, kJesterHatType, 1126, 3, 50, kBlue,1,0,-1);
	it_Ingredients		(kBlueJesterHat, kTailoringLeatherworking, 6, 1, kRollOfCloth, 1, kBlueDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kBrownJesterHat, 				kHead, kJesterHatType, 1127, 3, 50, kBrown,1,0,-1);
	it_Ingredients		(kBrownJesterHat, kTailoringLeatherworking, 6, 1, kRollOfCloth, 1, kBrownDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kBlackJesterHat, 				kHead, kJesterHatType, 1128, 3, 50, kBlack,1,0,-1);
	it_Ingredients		(kBlackJesterHat, kTailoringLeatherworking, 6, 1, kRollOfCloth, 1, kBlackDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kGreenJesterHat, 				kHead, kJesterHatType, 1129, 3, 50, kGreen,1,0,-1);
	it_Ingredients		(kGreenJesterHat, kTailoringLeatherworking, 6, 1, kRollOfCloth, 1, kGreenDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kPurpleJesterHat, 				kHead, kJesterHatType, 1130, 3, 50, kPurple,1,0,-1);
	it_Ingredients		(kPurpleJesterHat, kTailoringLeatherworking, 6, 1, kRollOfCloth, 1, kPurpleDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kYellowJesterHat, 				kHead, kJesterHatType, 1131, 3, 50, kYellow,1,0,-1);
	it_Ingredients		(kYellowJesterHat, kTailoringLeatherworking, 6, 1, kRollOfCloth, 1, kYellowDye,0,0,0,0,0,0,1);
	
	it_CreateItem		(kGoldCrown, 				      kHead, kCrownType, 1132, 3, 50, kNoColor,1,0,-1);
	//Dave 02-24-2012
	it_CreateItem		(kFirstCrown, 				      kHead, kCrownType, 1132, 3, 50, kGreen,1,0,-1);
	it_CreateItem		(kSecondCrown, 				      kHead, kCrownType, 1132, 3, 50, kBlack,1,0,-1);
	it_CreateItem		(kThirdCrown, 				      kHead, kCrownType, 1132, 3, 50, kRed,1,0,-1);

	//-------------------------------------------------------------------

	it_CreateItem		(kLightBlueBoots, 			kFeet, kBootsType, 1049, 4, 30, kLightBlue,1,0,-1);

	it_Ingredients	(kLightBlueBoots, kTailoringLeatherworking, 4, 1, kLeatherHide, 1, kLightBlueDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kRedBoots, 						kFeet, kBootsType, 1050, 4, 30, kRed,1,0,-1);

	it_Ingredients	(kRedBoots, kTailoringLeatherworking, 4, 1, kLeatherHide, 1, kRedDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kBlueBoots, 					kFeet, kBootsType, 1051, 4, 30, kBlue,1,0,-1);

	it_Ingredients	(kBlueBoots, kTailoringLeatherworking, 4, 1, kLeatherHide, 1, kBlueDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kBrownBoots, 					kFeet, kBootsType, 1052, 4, 30, kBrown,1,0,-1);

	it_Ingredients	(kBrownBoots, kTailoringLeatherworking, 4, 1, kLeatherHide, 0, 0, 0,0,0,0,0,0,1);

	it_CreateItem		(kBlackBoots, 					kFeet, kBootsType, 1053, 4, 30, kBlack,1,0,-1);

	it_Ingredients	(kBlackBoots, kTailoringLeatherworking, 4, 1, kLeatherHide, 1, kBlackDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kGreenBoots, 					kFeet, kBootsType, 1054, 4, 30, kGreen,1,0,-1);

	it_Ingredients	(kGreenBoots, kTailoringLeatherworking, 4, 1, kLeatherHide, 1, kGreenDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kPurpleBoots, 				kFeet, kBootsType, 1055, 4, 30, kPurple,1,0,-1);

	it_Ingredients	(kPurpleBoots, kTailoringLeatherworking, 4, 1, kLeatherHide, 1, kPurpleDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kYellowBoots, 				kFeet, kBootsType, 1056, 4, 30, kYellow,1,0,-1);

	it_Ingredients	(kYellowBoots, kTailoringLeatherworking, 4, 1, kLeatherHide, 1, kYellowDye, 0,0,0,0,0,0,1);
	//Dave
	it_CreateItem		(kGrayBoots, 				kFeet, kBootsType, 1160, 4, 30, kNoColor,1,0,-1);


	it_Ingredients	(kGrayBoots, kTailoringLeatherworking, 4, 1, kLeatherHide, 1, kGrayDye, 0,0,0,0,0,0,1);

	//-------------------------------------------------------------------

	it_CreateItem		(kGrayShirt, 					kTorso, kShirtType, 1003, 1, 10, kGray,1,0,-1);

	it_Ingredients	(kGrayShirt, kTailoringLeatherworking, 1, 1, kRollOfCloth, 0, 0, 0,0,0,0,0,0,1);

	it_CreateItem		(kLightBlueShirt, 			kTorso, kShirtType, 1008, 1, 10, kLightBlue,1,0,-1);

	it_Ingredients	(kLightBlueShirt, kTailoringLeatherworking, 1, 1, kRollOfCloth, 1, kLightBlueDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kRedShirt, 						kTorso, kShirtType, 1009, 1, 10, kRed,1,0,-1);

	it_Ingredients	(kRedShirt, kTailoringLeatherworking, 1, 1, kRollOfCloth, 1, kRedDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kBlueShirt, 					kTorso, kShirtType, 1004, 1, 10, kBlue,1,0,-1);

	it_Ingredients	(kBlueShirt, kTailoringLeatherworking, 1, 1, kRollOfCloth, 1, kBlueDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kBrownShirt, 					kTorso, kShirtType, 1005, 1, 10, kBrown,1,0,-1);

	it_Ingredients	(kBrownShirt, kTailoringLeatherworking, 1, 1, kRollOfCloth, 1, kBrownDye, 0,0,0,0,0,0,1);

	it_CreateItem		(KBlackShirt, 					kTorso, kShirtType, 1010, 1, 10, kBlack,1,0,-1);

	it_Ingredients	(KBlackShirt, kTailoringLeatherworking, 1, 1, kRollOfCloth, 1, kBlackDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kGreenShirt, 					kTorso, kShirtType, 1007, 1, 10, kGreen,1,0,-1);

	it_Ingredients	(kGreenShirt, kTailoringLeatherworking, 1, 1, kRollOfCloth, 1, kGreenDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kPurpleShirt, 				kTorso, kShirtType, 1006, 1, 10, kPurple,1,0,-1);

	it_Ingredients	(kPurpleShirt, kTailoringLeatherworking, 1, 1, kRollOfCloth, 1, kPurpleDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kYellowShirt, 				kTorso, kShirtType, 1066, 1, 10, kYellow,1,0,-1);

	it_Ingredients	(kYellowShirt, kTailoringLeatherworking, 1, 1, kRollOfCloth, 1, kYellowDye, 0,0,0,0,0,0,1);

	//-------------------------------------------------------------------

	it_CreateItem		(kGrayPants, 					kLegs, kPantsType, 1011, 1, 14, kGray,1,0,-1);

	it_Ingredients	(kGrayPants, kTailoringLeatherworking, 2, 1, kRollOfCloth, 0, 0, 0,0,0,0,0,0,1);

	it_CreateItem		(kLightBluePants, 			kLegs, kPantsType, 1016, 1, 14, kLightBlue,1,0,-1);

	it_Ingredients	(kLightBluePants, kTailoringLeatherworking, 2, 1, kRollOfCloth, 1, kLightBlueDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kRedPants, 						kLegs, kPantsType, 1017, 1, 14, kRed,1,0,-1);

	it_Ingredients	(kRedPants, kTailoringLeatherworking, 2, 1, kRollOfCloth, 1, kRedDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kBluePants, 					kLegs, kPantsType, 1012, 1, 14, kBlue,1,0,-1);

	it_Ingredients	(kBluePants, kTailoringLeatherworking, 2, 1, kRollOfCloth, 1, kBlueDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kBrownPants, 					kLegs, kPantsType, 1013, 1, 14, kBrown,1,0,-1);

	it_Ingredients	(kBrownPants, kTailoringLeatherworking, 2, 1, kRollOfCloth, 1, kBrownDye, 0,0,0,0,0,0,1);

	it_CreateItem		(KBlackPants, 					kLegs, kPantsType, 1018, 1, 14, kBlack,1,0,-1);

	it_Ingredients	(KBlackPants, kTailoringLeatherworking, 2, 1, kRollOfCloth, 1, kBlackDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kGreenPants, 					kLegs, kPantsType, 1015, 1, 14, kGreen,1,0,-1);

	it_Ingredients	(kGreenPants, kTailoringLeatherworking, 2, 1, kRollOfCloth, 1, kGreenDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kPurplePants, 				kLegs, kPantsType, 1014, 1, 14, kPurple,1,0,-1);

	it_Ingredients	(kPurplePants, kTailoringLeatherworking, 2, 1, kRollOfCloth, 1, kPurpleDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kYellowPants, 				kLegs, kPantsType, 1065, 1, 14, kYellow,1,0,-1);

	it_Ingredients	(kYellowPants, kTailoringLeatherworking, 2, 1, kRollOfCloth, 1, kYellowDye, 0,0,0,0,0,0,1);

	//------------------------------------------------

	it_CreateItem		(kGMRobe, 						kTorso, kGMRobeType, 1109, 2, 30, kNoColor,100,0,-1);

	it_CreateItem		(kRoyalRobe, 				  kTorso, kRoyalRobeType, 1122, 2, 30, kNoColor,50,0,-1);

	it_CreateItem		(kGrayRobeProtection, 						kTorso, kMagicRobeType, 1019, 2, 30, kGray,30,0,-1);  // RoP's 30 AR 08-05-2010
	it_Ingredients		(kGrayRobeProtection,	kTailoringLeatherworking, 12, 1, kMerefolkHair, 3, kEnchantedCloth,0,0,0,0,0,0,1);

	it_CreateItem		(kLightBlueRobeProtection, 				kTorso, kMagicRobeType, 1024, 2, 30, kLightBlue,30,0,-1);
	it_Ingredients		(kLightBlueRobeProtection,	kTailoringLeatherworking, 12, 1, kMerefolkHair, 3, kEnchantedCloth, 1, kLightBlueDye,0,0,0,0,1);
	
	it_CreateItem		(kRedRobeProtection, 							kTorso, kMagicRobeType, 1025, 2, 30, kRed,30,0,-1);
	it_Ingredients		(kRedRobeProtection,	kTailoringLeatherworking, 12, 1, kMerefolkHair, 3, kEnchantedCloth, 1, kRedDye,0,0,0,0,1);
	
	it_CreateItem		(kBlueRobeProtection, 						kTorso, kMagicRobeType, 1020, 2, 30, kBlue,30,0,-1);
	it_Ingredients		(kBlueRobeProtection,	kTailoringLeatherworking, 12, 1, kMerefolkHair, 3, kEnchantedCloth, 1, kBlueDye,0,0,0,0,1);
	
	it_CreateItem		(kBrownRobeProtection, 						kTorso, kMagicRobeType, 1021, 2, 30, kBrown,30,0,-1);
	it_Ingredients		(kBrownRobeProtection,	kTailoringLeatherworking, 12, 1, kMerefolkHair, 3, kEnchantedCloth, 1, kBrownDye,0,0,0,0,1);
	
	it_CreateItem		(kBlackRobeProtection, 						kTorso, kMagicRobeType, 1026, 2, 30, kBlack,30,0,-1);
	it_Ingredients		(kBlackRobeProtection,	kTailoringLeatherworking, 12, 1, kMerefolkHair, 3, kEnchantedCloth, 1, kBlackDye,0,0,0,0,1);
	
	it_CreateItem		(kGreenRobeProtection, 						kTorso, kMagicRobeType, 1023, 2, 30, kGreen,30,0,-1);
	it_Ingredients		(kGreenRobeProtection,	kTailoringLeatherworking, 12, 1, kMerefolkHair, 3, kEnchantedCloth, 1, kGreenDye,0,0,0,0,1);
	
	it_CreateItem		(kPurpleRobeProtection, 					kTorso, kMagicRobeType, 1022, 2, 30, kPurple,30,0,-1);
	it_Ingredients		(kPurpleRobeProtection,	kTailoringLeatherworking, 12, 1, kMerefolkHair, 3, kEnchantedCloth, 1, kPurpleDye,0,0,0,0,1);
	
	it_CreateItem		(kYellowRobeProtection, 					kTorso, kMagicRobeType, 1077, 2, 30, kYellow,30,0,-1);
	it_Ingredients		(kYellowRobeProtection,	kTailoringLeatherworking, 12, 1, kMerefolkHair, 3, kEnchantedCloth, 1, kYellowDye,0,0,0,0,1);
	
	it_CreateItem		(kGrayRobeResistance, 						kTorso, kMagicRobeType, 1019, 2, 30, kGray,1,0,-1);
	it_Ingredients		(kGrayRobeResistance,	kTailoringLeatherworking, 11, 1, kNeviaFlower, 3, kEnchantedCloth, 0,0,0,0,0,0,1);
	
	it_CreateItem		(kLightBlueRobeResistance, 				kTorso, kMagicRobeType, 1024, 2, 30, kLightBlue,1,0,-1);
	it_Ingredients		(kLightBlueRobeResistance,	kTailoringLeatherworking, 11, 1, kNeviaFlower, 3, kEnchantedCloth, 1, kLightBlueDye,0,0,0,0,1);
	
	it_CreateItem		(kRedRobeResistance, 							kTorso, kMagicRobeType, 1025, 2, 30, kRed,1,0,-1);
	it_Ingredients		(kRedRobeResistance,	kTailoringLeatherworking, 11, 1, kNeviaFlower, 3, kEnchantedCloth, 1, kRedDye,0,0,0,0,1);
	
	it_CreateItem		(kBlueRobeResistance, 						kTorso, kMagicRobeType, 1020, 2, 30, kBlue,1,0,-1);
	it_Ingredients		(kBlueRobeResistance,	kTailoringLeatherworking, 11, 1, kNeviaFlower, 3, kEnchantedCloth, 1, kBlueDye,0,0,0,0,1);
	
	it_CreateItem		(kBrownRobeResistance, 						kTorso, kMagicRobeType, 1021, 2, 30, kBrown,1,0,-1);
	it_Ingredients		(kBrownRobeResistance,	kTailoringLeatherworking, 11, 1, kNeviaFlower, 3, kEnchantedCloth, 1, kBrownDye,0,0,0,0,1);
	
	it_CreateItem		(KBlackRobeResistance, 						kTorso, kMagicRobeType, 1026, 2, 30, kBlack,1,0,-1);
	it_Ingredients		(KBlackRobeResistance,	kTailoringLeatherworking, 11, 1, kNeviaFlower, 3, kEnchantedCloth, 1, kBlackDye,0,0,0,0,1);
	
	it_CreateItem		(kGreenRobeResistance, 						kTorso, kMagicRobeType, 1023, 2, 30, kGreen,1,0,-1);
	it_Ingredients		(kGreenRobeResistance,	kTailoringLeatherworking, 11, 1, kNeviaFlower, 3, kEnchantedCloth, 1, kGreenDye,0,0,0,0,1);
	
	it_CreateItem		(kPurpleRobeResistance, 					kTorso, kMagicRobeType, 1022, 2, 30, kPurple,1,0,-1);
	it_Ingredients		(kPurpleRobeResistance,	kTailoringLeatherworking, 11, 1, kNeviaFlower, 3, kEnchantedCloth, 1, kPurpleDye,0,0,0,0,1);
	
	it_CreateItem		(kYellowRobeResistance, 					kTorso, kMagicRobeType, 1077, 2, 30, kYellow,1,0,-1);
	it_Ingredients		(kYellowRobeResistance,	kTailoringLeatherworking, 11, 1, kNeviaFlower, 3, kEnchantedCloth, 1, kYellowDye,0,0,0,0,1);
	
	it_CreateItem		(kGrayRobe, 						kTorso, kRobeType, 1019, 2, 30, kGray,2,0,-1);

	it_Ingredients	(kGrayRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 0, 0, 0,0,0,0,0,0,1);

	it_CreateItem		(kLightBlueRobe, 			kTorso, kRobeType, 1024, 2, 30, kLightBlue,2,0,-1);

	it_Ingredients	(kLightBlueRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 1, kLightBlueDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kRedRobe, 						kTorso, kRobeType, 1025, 2, 30, kRed,2,0,-1);  //Dave test

	it_Ingredients	(kRedRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 1, kRedDye, 0,0,0,0,0,0,1);
	
	//Dave
	//it_CreateItem		(kPinkRobe, 						kTorso, kRobeType, 1025, 2, 30, kPinkDragonColor,2,0,-1);

	//it_Ingredients	(kPinkRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 1, kOrangeCrystal, 0,0,0,0,0,0,1);

	it_CreateItem		(kBlueRobe, 						kTorso, kRobeType, 1020, 2, 30, kBlue,2,0,-1);

	it_Ingredients	(kBlueRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 1, kBlueDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kBrownRobe, 					kTorso, kRobeType, 1021, 2, 30, kBrown,2,0,-1);

	it_Ingredients	(kBrownRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 1, kBrownDye, 0,0,0,0,0,0,1);

	it_CreateItem		(KBlackRobe, 					kTorso, kRobeType, 1026, 2, 30, kBlack,2,0,-1);

	it_Ingredients	(KBlackRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 1, kBlackDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kGreenRobe, 					kTorso, kRobeType, 1023, 2, 30, kGreen,2,0,-1);

	it_Ingredients	(kGreenRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 1, kGreenDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kPurpleRobe, 					kTorso, kRobeType, 1022, 2, 30, kPurple,2,0,-1);

	it_Ingredients	(kPurpleRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 1, kPurpleDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kYellowRobe, 					kTorso, kRobeType, 1077, 2, 30, kYellow,2,0,-1);

	it_Ingredients	(kYellowRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 1, kYellowDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kOrangeRobe, 					kTorso, kRobeType, 1077, 2, 30, kOrange,2,0,-1);

	it_Ingredients	(kOrangeRobe, kTailoringLeatherworking, 3, 2, kRollOfCloth, 1, kOrangeDye, 0,0,0,0,0,0,1);

	it_CreateItem		(kWhiteRobe, 					kTorso, kWhiteRobeType, 1026, 2, 30, kGray,2,0,-1);
	//--------------------------------------------------------------

	fi_GetItemSettings();

#ifdef _SERVER
	
	fi_ScanForRares();

#endif

}

#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean it_IsSpecialItem(int item, int count) //server
// ----------------------------------------------------------------- //

{
	//Checks if a player has an item they shouldn't have, or too much of a rare item
	if (item==kSilverLeaf)										return true;
	if (item>=kBlueFlag && item<=kGreenFlag) 					return true;
	if (item>=kDragonStoneRing && item<=kRingOfLife)			return true;
	if (item>=kCurio1 && item<=kBody)							return true;
	if (item>=kBlueTicket && item<=kSkeletonKey)				return true;
	if (item>=kHorse && item<=kGMHorse)							return true;
	//if (item>=kThread && item<4000)								return true;  //Dave
	
	if (count>=10)
	{
		if (item==kSilverLeaf)
		//if (item>=kCharmOfLoyalty && item<=kEssenceOfSpeed)		return true;
		//if (item>=kAncientRune && item<=kPureCrystalRing)		return true;
		if (item==kInfusedCS)									return true;		
	}
	
	return false;
}

#endif