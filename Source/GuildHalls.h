#ifndef _GUILDHALLS
#define _GUILDHALLS

#include "Keys.h"
#include "Remap.h"
#include "Inventory.h"
#include "Players.h"
#include "Creatures.h"

//-----------------------------------------------------------------

#define							kMaxGuildHalls											150
#define							kMaxExtras												9
#define							kMaxGuildChests											100

//-----------------------------------------------------------------

typedef struct extraType {
		UInt16			type;
		UInt16			row;
		UInt16			col;
	} extraType;
	
typedef struct guildHallType {
		UInt8			size;
		TBoolean		saved; //On the map file or not
		UInt16			row;
		UInt16  		col;
		char			guild[kStringLength];
		extraType		extras[kMaxExtras];
		int				chests[kMaxGuildChests][3];
	} guildHallType;
	
	
	
//-----------------------------------------------------------------

void					gh_InitGuildHalls(); //server
TBoolean				gh_OkForHall(int); //server
void					gh_BuildGuildHallServer(guildHallType); //server
void					gh_BuildGuildHall(int, int, int, extraType*, TBoolean);		

#endif