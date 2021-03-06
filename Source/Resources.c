#include "Toolbox.h"
#include "Globals.h"
#include "Common.h"
#include "Players.h"
#include "Network.h"
#include "FileIO.h"
#include "Graphics.h"
#include "Inventory.h"
#include "Creatures.h"
#include "Resources.h"

//-----------------------------------------------

UInt8								resources[kMapWidth][kMapHeight];
int									resourceXPtr;
int									resourceYPtr;
unsigned long				resourceNextRefresh;
//-----------------------------------------------

// ----------------------------------------------------------------- //
void rs_InitResources(void)
// ----------------------------------------------------------------- //

{
	int							x;
	int							y;

	for (x=0;x<kMapWidth;x++)
		for (y=0;y<kMapHeight;y++)
			resources[x][y]=0;

	resourceXPtr=0;

	resourceYPtr=0;

	resourceNextRefresh=TickCount()+1;

}

// ----------------------------------------------------------------- //
void rs_RefreshResources(void)
// ----------------------------------------------------------------- //

{
	int								i;

	if (TickCount()<resourceNextRefresh)
		return;

	for (i=0;i<2400;i++)
		begin

		if ((map[resourceXPtr][resourceYPtr]>=359) && (map[resourceXPtr][resourceYPtr]<=368))	// -- replenish trees --
			begin
			if (resources[resourceXPtr][resourceYPtr]>5)
				resources[resourceXPtr][resourceYPtr]=resources[resourceXPtr][resourceYPtr]-5;
			else
				resources[resourceXPtr][resourceYPtr]=0;

	end

	//---------------------------------
	resourceXPtr++;

	if (resourceXPtr>=kMapWidth)
		begin
		resourceXPtr=0;

	resourceYPtr++;

	if (resourceYPtr>=kMapHeight)
		resourceYPtr=0;

	end
	//---------------------------------

	end

	resourceNextRefresh=TickCount()+1;

}

