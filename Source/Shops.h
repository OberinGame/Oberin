#ifndef _SHOP
#define _SHOP

#include "Keys.h"
#include "Remap.h"

//- constants ---------------------------------------------

#define									kMaxShops									100
#define									kMaxStables								100

#define									kSellOnly									1
#define									kBuyOnly									2
#define									kBuyAndSell								3

#define									kBank											9999
/*
#define									kBlacksmithShop						2
#define									kEateryShop								3
#define									kTailorShop								4
#define									kProvisionerShop					5
#define									kTinkerShop								6
#define									kMagicShop								7
#define									kHerbalistShop						8
#define									kWeaponShop								9
#define									kAlchemistShop						10
#define									kBakeryShop								11
#define									kJewelerShop							12
#define									kArcheryShop							13
#define									kArmorShop								14
#define									kPubShop								  15
#define									kCarthProvisionerShop			16
*/
//----------------------------------------------------------

typedef struct commerceType {
		UInt16								itemID;
		UInt16								cost;
		TBoolean							forSale;
		TBoolean							willBuy;
		UInt8									transactions;
		UInt8                 availibility;
	} commerceType;	

//----------------------------------------------------------
extern TRect							shopRect						[kMaxShops];
extern char								traderName					[kMaxShops][48];
extern TRect							stableRect					[kMaxStables];

extern commerceType				shopInventory				[kMaxInventorySize];
extern commerceType				sellList						[kMaxInventorySize];
extern commerceType				shop								[kMaxShops][kMaxInventorySize];
//extern int								shopID1							[kMaxShops];
//extern int								shopID2							[kMaxShops];

//----------------------------------------------------------

void 				sh_ClearShopContents						(void);
void 				sh_InitializeShops							(void);
int					sh_GetShopID										(int i);	// server
int					sh_GetTrader										(int i);	// server
TBoolean 		sh_AnythingInShopList						(void);
TBoolean 		sh_AnythingInSellList						(void);
void 				sh_RotateShopInventory					(void);	// server
void 				sh_ClearSellList								(void);	// client
TBoolean 		sh_InSellList										(int item);	// client
int 				sh_SellPriceOf									(int item);	// client
int 				sh_ServerSellPriceOf						(int i, int item);	// client
void 				sh_AddItemToShop								(int theShop, int itemID, int transactions, int availibility, float times);	// server
TBoolean    sh_StandingInInn                (int i);	// server
TBoolean		sh_StandingNextToTrader					(int i);	// server
void 				sh_AdjustShopPrices							(void);	// server

#endif