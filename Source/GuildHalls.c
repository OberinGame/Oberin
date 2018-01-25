#include "Updater.h"
#include "Globals.h"
#include "Toolbox.h"
#include "Keys.h"
#include "Remap.h"
#include "Players.h"
#include "Creatures.h"
#include "Common.h"
#include "FileIO.h"
#include "GuildHalls.h"

extern	UInt16 map[kMapWidth][kMapHeight];
extern	UInt16 clientMap[kMapWidth][kMapHeight];

extern	TBoolean doorway[kMapWidth][kMapHeight];
extern	TBoolean canWalkOn[kMapWidth][kMapHeight];
extern	TBoolean canSwimIn[kMapWidth][kMapHeight];
extern	TBoolean canFlyOver[kMapWidth][kMapHeight];


#ifdef _SERVER

int					guildHallsPtr;
guildHallType		guildHalls[kMaxGuildHalls];
int					guildMap[kMapWidth][kMapHeight];

#endif


#ifndef _SERVERONLY


UInt8				localHallsIDs[kMaxGuildHalls];

#endif


// ----------------------------------------------------------------- //
void gh_InitGuildHalls()
// ----------------------------------------------------------------- //
{
	int cpt, cpt2;

#ifdef _SERVER

	guildHallsPtr=0;

	for (cpt=0; cpt<kMapWidth; cpt++)
		for (cpt2=0; cpt2<kMapHeight; cpt2++)
			guildMap[cpt][cpt2]=0;

	for (cpt=0; cpt<kMaxGuildHalls; cpt++)
		for (cpt2=0; cpt2<kMaxGuildChests; cpt2++)
			guildHalls[cpt].chests[cpt2][0]=0;

#endif

#ifndef _SERVERONLY

	for (cpt=0; cpt<kMaxGuildHalls; cpt++)
		localHallsIDs[cpt]=0;

#endif
	fi_LoadGuildHalls();

}

#ifdef _SERVER

// ----------------------------------------------------------------- //
TBoolean gh_OkForHall(int i)  //Check the tile for hall building
// ----------------------------------------------------------------- //

{
	if (i>244 && i<267)
		return (true);

	if (i>358 && i<386)
		return (true);

	if (i>395 && i<438)
		return (true);

	if (i>470 && i<502)
		return (true);

	if (i>861 && i<864)
		return (true);

	if (i>908 && i<928)
		return (true);

	if (i>1039 && i<1048)
		return (true);

	if (i==535 || i==621 || i==793 || i==826 || i==843 || i==866 || i==946 || i==957 || i==959 || i==973 || i==986)
		return (true);

	return (false);
}

// ----------------------------------------------------------------- //
void  gh_BuildGuildHallServer(guildHallType theHall)
// ----------------------------------------------------------------- //
{
	int	sRow=theHall.row;
	int	sCol=theHall.col;
	int	cpt, cpt2;

	/*
	south face is 863
	the rest can stay what they are, but add them to north and east also
	*/

	if (!theHall.saved) //No need to build if already on world map
		switch (theHall.size)
		{
			case 1 : //Small guild hall

				//Clearance

				for (cpt=sCol; cpt<sCol+9; cpt++)
				{
					//map[sRow+9][cpt]=256;
					map[sRow-1][cpt]=863;
				}

				/*for (cpt=sRow-1; cpt<=sRow+7; cpt++)
				{
					map[cpt][sCol-1]=256;
					map[cpt][sCol+9]=256;
				}*/

				//External structure

				map[sRow+1][sCol]=map[sRow+3][sCol]=map[sRow+4][sCol]=map[sRow+5][sCol]=map[sRow+1][sCol+8]=map[sRow+5][sCol+8]=597;

				map[sRow+2][sCol]=map[sRow+3][sCol+8]=975;

				map[sRow][sCol]=map[sRow+6][sCol]=map[sRow+6][sCol+8]=map[sRow][sCol+8]=602;

				map[sRow+6][sCol+1]=map[sRow+6][sCol+3]=map[sRow+6][sCol+5]=map[sRow+6][sCol+7]=map[sRow][sCol+7]=map[sRow][sCol+6]=map[sRow][sCol+5]=map[sRow][sCol+2]=map[sRow][sCol+1]=598;

				map[sRow+6][sCol+2]=map[sRow+6][sCol+4]=map[sRow+6][sCol+6]=618;

				map[sRow+4][sCol+8]=668;

				map[sRow+2][sCol+8]=667;

				map[sRow][sCol+4]=633;

				map[sRow][sCol+3]=634;

				//Shadows

				for (cpt=sCol+1; cpt<=sCol+6; cpt++)
					map[sRow+5][cpt]=612;

				map[sRow+5][sCol+7]=615;

				for (cpt=sRow+1; cpt<=sRow+4; cpt++)
					map[cpt][sCol+7]=613;


				//Floor

				for (cpt2=sCol+1; cpt2<=sCol+6; cpt2++)
					for (cpt=sRow+1; cpt<sRow+5; cpt++)
						map[cpt][cpt2]=608;


				break;

			case 2 : //Medium guild hall

				//Clearance

				for (cpt=sCol; cpt<sCol+11; cpt++)
				{
					//map[sRow+9][cpt]=256;
					map[sRow-1][cpt]=863;
				}

				/*for (cpt=sRow-1; cpt<=sRow+10; cpt++)
				{
					map[cpt][sCol-1]=256;
					map[cpt][sCol+9]=256;
				}*/

				//External Structure

				map[sRow][sCol]=map[sRow+9][sCol]=map[sRow+9][sCol+10]=map[sRow][sCol+10]=602;

				map[sRow+1][sCol]=map[sRow+2][sCol]=map[sRow+4][sCol]=map[sRow+5][sCol]=map[sRow+7][sCol]=map[sRow+8][sCol]=map[sRow+8][sCol+10]=map[sRow+1][sCol+10]=597;

				map[sRow+3][sCol]=map[sRow+6][sCol]=map[sRow+6][sCol+10]=map[sRow+3][sCol+10]=975;

				map[sRow+9][sCol+1]=map[sRow+9][sCol+2]=map[sRow+9][sCol+4]=map[sRow+9][sCol+6]=map[sRow+9][sCol+8]=map[sRow+9][sCol+9]=map[sRow][sCol+1]=map[sRow][sCol+2]=map[sRow][sCol+8]=map[sRow][sCol+9]=598;

				map[sRow+9][sCol+3]=map[sRow+9][sCol+7]=618;

				map[sRow+9][sCol+5]=648;

				map[sRow+7][sCol+10]=map[sRow+2][sCol+10]=619;

				map[sRow+5][sCol+10]=668;

				map[sRow+4][sCol+10]=667;

				map[sRow][sCol+3]=map[sRow][sCol+7]=602;

				map[sRow][sCol+4]=map[sRow][sCol+5]=map[sRow][sCol+6]=658;

				//Indoor elements

				map[sRow+8][sCol+2]=655;

				map[sRow+7][sCol+6]=map[sRow+7][sCol+7]=map[sRow+7][sCol+8]=639;

				map[sRow+6][sCol+5]=641;

				map[sRow+5][sCol+6]=map[sRow+5][sCol+7]=map[sRow+5][sCol+8]=640;

				map[sRow+6][sCol+6]=map[sRow+6][sCol+8]=628;

				map[sRow+6][sCol+7]=637;

				//Shadows

				map[sRow+8][sCol+1]=612;

				map[sRow+8][sCol+9]=615;

				for (cpt=sCol+3; cpt<=sCol+8; cpt++)
					map[sRow+8][cpt]=612;

				for (cpt=sRow+1; cpt<=sRow+7; cpt++)
					map[cpt][sCol+9]=613;

				//Floor

				map[sRow+7][sCol+5]=map[sRow+5][sCol+5]=608;

				for (cpt=sRow+5; cpt<=sRow+7; cpt++)
					for (cpt2=sCol+1; cpt2<=sCol+4; cpt2++)
						map[cpt][cpt2]=608;

				for (cpt=sRow+1; cpt<=sRow+4; cpt++)
					for (cpt2=sCol+1; cpt2<=sCol+8; cpt2++)
						map[cpt][cpt2]=608;

				break;

			case 3 : //Large guild hall

				//Clearance

				for (cpt=sCol; cpt<sCol+19; cpt++)
				{
					//map[sRow+9][cpt]=256;
					map[sRow-1][cpt]=863;
				}

				/*for (cpt=sRow-1; cpt<=sRow+19; cpt++)
				{
					map[cpt][sCol-1]=256;
					map[cpt][sCol+9]=256;
				}*/

				//External Structure

				map[sRow][sCol]=map[sRow+11][sCol]=map[sRow+18][sCol]=map[sRow+18][sCol+7]=map[sRow+11][sCol+7]=map[sRow+11][sCol+12]=map[sRow+18][sCol+12]=map[sRow+18][sCol+18]=map[sRow+11][sCol+18]=map[sRow][sCol+18]=map[sRow][sCol+12]=map[sRow][sCol+6]=602;

				map[sRow+3][sCol]=map[sRow+7][sCol]=map[sRow+15][sCol]=map[sRow+15][sCol+18]=map[sRow+7][sCol+18]=map[sRow+3][sCol+18]=975;

				map[sRow+1][sCol]=map[sRow+2][sCol]=map[sRow+14][sCol+12]=597;

				for (cpt=sRow+4; cpt<=sRow+17; cpt++)
				{
					map[cpt][sCol]=597;

					if (cpt==sRow+6 || cpt==sRow+10 || cpt==sRow+14)
						cpt++;
				}

				for (cpt=sRow+12; cpt<=sRow+17; cpt++)
				{
					map[cpt][sCol+7]=597;
					map[cpt][sCol+12]=597;

					if (cpt==sRow+13)
						cpt++;
				}

				for (cpt=sRow+1; cpt<=sRow+17; cpt++)
				{
					map[cpt][sCol+18]=597;

					if (cpt==sRow+2 || cpt==sRow+4 || cpt==sRow+6 || cpt==sRow+8 || cpt==sRow+10 || cpt==sRow+13 || cpt==sRow+14)
						cpt++;
				}

				map[sRow+11][sCol+8]=map[sRow+11][sCol+11]=598;

				for (cpt=sCol+1; cpt<=sCol+17; cpt++)
				{
					map[sRow][cpt]=598;

					if (cpt==sCol+5)
						cpt=sCol+12;
				}

				for (cpt=sCol+1; cpt<=sCol+17; cpt++)
				{
					map[sRow+18][cpt]=598;

					if (cpt==sCol+3 || cpt==sCol+14)
						cpt++;

					if (cpt==sCol+6)
						cpt=sCol+12;
				}

				for (cpt=sCol+8; cpt<=sCol+11; cpt++)
					map[sRow+18][cpt]=558;

				for (cpt=sCol+7; cpt<=sCol+11; cpt++)
					map[sRow][cpt]=658;

				map[sRow+18][sCol+4]=map[sRow+18][sCol+15]=618;

				map[sRow+14][sCol+7]=map[sRow+14][sCol+18]=map[sRow+9][sCol+18]=map[sRow+5][sCol+18]=619;

				map[sRow+11][sCol+9]=634;

				map[sRow+11][sCol+10]=633;

				//Indoor elements

				map[sRow+16][sCol+6]=map[sRow+12][sCol+6]=671;

				map[sRow+15][sCol+6]=map[sRow+13][sCol+6]=670;

				map[sRow+15][sCol+5]=map[sRow+13][sCol+5]=665;

				for (cpt=sCol+7; cpt<=sCol+12; cpt++)
				{
					map[sRow+9][cpt]=639;
					map[sRow+7][cpt]=640;
				}

				map[sRow+8][sCol+5]=641;

				map[sRow+8][sCol+13]=642;

				for (cpt=sCol+6; cpt<=sCol+12; cpt++)
				{
					map[sRow+8][cpt]=628;

					if (cpt==sCol+7 || cpt==sCol+10)
						cpt++;
				}

				map[sRow+8][sCol+8]=map[sRow+8][sCol+11]=637;

				map[sRow+15][sCol+16]=652;
				map[sRow+14][sCol+16]=646;
				map[sRow+17][sCol+14]=map[sRow+17][sCol+16]=655;

				//Shadows

				for (cpt=sCol+1; cpt<=sCol+17; cpt++)
				{
					if (cpt<sCol+7)
						map[sRow+17][cpt]=612;
					else if (cpt>sCol+6 && cpt<sCol+13)
						map[sRow+10][cpt]=612;
					else
					{
						map[sRow+17][cpt]=612;

						if (cpt==sCol+13 || cpt==sCol+15)
							cpt++;
					}
				}

				map[sRow+17][sCol+6]=map[sRow+17][sCol+17]=615;

				map[sRow+14][sCol+6]=map[sRow+11][sCol+6]=613;

				for (cpt=sRow+1; cpt<=sRow+16; cpt++)
				{
					map[cpt][sCol+17]=613;
				}

				//Floor

				map[sRow+11][sCol+5]=map[sRow+16][sCol+5]=map[sRow+14][sCol+5]=map[sRow+12][sCol+5]=map[sRow+7][sCol+5]=map[sRow+7][sCol+6]=map[sRow+7][sCol+13]=map[sRow+16][sCol+16]=608;

				for (cpt=sRow+7; cpt<=sRow+16; cpt++)
					for (cpt2=sCol+1; cpt2<=sCol+4; cpt2++)
						map[cpt][cpt2]=608;

				for (cpt=sRow+9; cpt<=sRow+10; cpt++)
					for (cpt2=sCol+5; cpt2<=sCol+6; cpt2++)
						map[cpt][cpt2]=608;

				for (cpt=sRow+1; cpt<=sRow+6; cpt++)
					for (cpt2=sCol+1; cpt2<=sCol+16; cpt2++)
						map[cpt][cpt2]=608;

				for (cpt=sRow+9; cpt<=sRow+16; cpt++)
					for (cpt2=sCol+13; cpt2<=sCol+15; cpt2++)
						map[cpt][cpt2]=608;

				for (cpt=sRow+7; cpt<=sRow+8; cpt++)
					for (cpt2=sCol+14; cpt2<=sCol+15; cpt2++)
						map[cpt][cpt2]=608;

				for (cpt=sRow+7; cpt<=sRow+13; cpt++)
					map[cpt][sCol+16]=608;


				break;

		}

	for (cpt=0; cpt<kMaxExtras; cpt++)
	{
		if (theHall.extras[cpt].type==0)
			continue;

		map[theHall.extras[cpt].row][theHall.extras[cpt].col]=theHall.extras[cpt].type;

		if (theHall.extras[cpt].type==974)
			ac_AddChessTable(theHall.extras[cpt].row, theHall.extras[cpt].col);
	}
}

#endif

#ifndef _SERVERONLY

// ----------------------------------------------------------------- //
void  gh_BuildGuildHall(int sRow, int sCol, int size, extraType* extras, TBoolean saved)
// ----------------------------------------------------------------- //
{
	int	cpt, cpt2;

	if (!saved) //No need to build if already on world map
		switch (size)
		{
			case 1 : //Small guild hall

				//Clearance

				for (cpt=sCol; cpt<sCol+9; cpt++)
				{
					//clientMap[sRow+9][cpt]=256;
					clientMap[sRow-1][cpt]=863;
				}

				/*for (cpt=sRow-1; cpt<=sRow+7; cpt++)
				{
					clientMap[cpt][sCol-1]=256;
					clientMap[cpt][sCol+9]=256;
				}*/


				//External structure

				clientMap[sRow+1][sCol]=clientMap[sRow+3][sCol]=clientMap[sRow+4][sCol]=clientMap[sRow+5][sCol]=clientMap[sRow+1][sCol+8]=clientMap[sRow+5][sCol+8]=597;

				clientMap[sRow+2][sCol]=clientMap[sRow+3][sCol+8]=975;

				clientMap[sRow][sCol]=clientMap[sRow+6][sCol]=clientMap[sRow+6][sCol+8]=clientMap[sRow][sCol+8]=602;

				clientMap[sRow+6][sCol+1]=clientMap[sRow+6][sCol+3]=clientMap[sRow+6][sCol+5]=clientMap[sRow+6][sCol+7]=clientMap[sRow][sCol+7]=clientMap[sRow][sCol+6]=clientMap[sRow][sCol+5]=clientMap[sRow][sCol+2]=clientMap[sRow][sCol+1]=598;

				clientMap[sRow+6][sCol+2]=clientMap[sRow+6][sCol+4]=clientMap[sRow+6][sCol+6]=618;

				clientMap[sRow+4][sCol+8]=668;

				clientMap[sRow+2][sCol+8]=667;

				clientMap[sRow][sCol+4]=633;

				clientMap[sRow][sCol+3]=634;

				//Shadows

				for (cpt=sCol+1; cpt<=sCol+6; cpt++)
					clientMap[sRow+5][cpt]=612;

				clientMap[sRow+5][sCol+7]=615;

				for (cpt=sRow+1; cpt<=sRow+4; cpt++)
					clientMap[cpt][sCol+7]=613;

				//Floor

				for (cpt2=sCol+1; cpt2<=sCol+6; cpt2++)
					for (cpt=sRow+1; cpt<sRow+5; cpt++)
						clientMap[cpt][cpt2]=608;


				break;

			case 2 : //Medium guild hall

				//Clearance

				for (cpt=sCol; cpt<sCol+11; cpt++)
				{
					//clientMap[sRow+9][cpt]=256;
					clientMap[sRow-1][cpt]=863;
				}

				/*for (cpt=sRow-1; cpt<=sRow+10; cpt++)
				{
					clientMap[cpt][sCol-1]=256;
					clientMap[cpt][sCol+9]=256;
				}*/

				//External Structure

				clientMap[sRow][sCol]=clientMap[sRow+9][sCol]=clientMap[sRow+9][sCol+10]=clientMap[sRow][sCol+10]=602;

				clientMap[sRow+1][sCol]=clientMap[sRow+2][sCol]=clientMap[sRow+4][sCol]=clientMap[sRow+5][sCol]=clientMap[sRow+7][sCol]=clientMap[sRow+8][sCol]=clientMap[sRow+8][sCol+10]=clientMap[sRow+1][sCol+10]=597;

				clientMap[sRow+3][sCol]=clientMap[sRow+6][sCol]=clientMap[sRow+6][sCol+10]=clientMap[sRow+3][sCol+10]=975;

				clientMap[sRow+9][sCol+1]=clientMap[sRow+9][sCol+2]=clientMap[sRow+9][sCol+4]=clientMap[sRow+9][sCol+6]=clientMap[sRow+9][sCol+8]=clientMap[sRow+9][sCol+9]=clientMap[sRow][sCol+1]=clientMap[sRow][sCol+2]=clientMap[sRow][sCol+8]=clientMap[sRow][sCol+9]=598;

				clientMap[sRow+9][sCol+3]=clientMap[sRow+9][sCol+7]=618;

				clientMap[sRow+9][sCol+5]=648;

				clientMap[sRow+7][sCol+10]=clientMap[sRow+2][sCol+10]=619;

				clientMap[sRow+5][sCol+10]=668;

				clientMap[sRow+4][sCol+10]=667;

				clientMap[sRow][sCol+3]=clientMap[sRow][sCol+7]=602;

				clientMap[sRow][sCol+4]=clientMap[sRow][sCol+5]=clientMap[sRow][sCol+6]=658;

				//Indoor elements

				clientMap[sRow+8][sCol+2]=655;

				clientMap[sRow+7][sCol+6]=clientMap[sRow+7][sCol+7]=clientMap[sRow+7][sCol+8]=639;

				clientMap[sRow+6][sCol+5]=641;

				clientMap[sRow+5][sCol+6]=clientMap[sRow+5][sCol+7]=clientMap[sRow+5][sCol+8]=640;

				clientMap[sRow+6][sCol+6]=clientMap[sRow+6][sCol+8]=628;

				clientMap[sRow+6][sCol+7]=637;

				//Shadows

				clientMap[sRow+8][sCol+1]=612;

				clientMap[sRow+8][sCol+9]=615;

				for (cpt=sCol+3; cpt<=sCol+8; cpt++)
					clientMap[sRow+8][cpt]=612;

				for (cpt=sRow+1; cpt<=sRow+7; cpt++)
					clientMap[cpt][sCol+9]=613;

				//Floor

				clientMap[sRow+7][sCol+5]=clientMap[sRow+5][sCol+5]=608;

				for (cpt=sRow+5; cpt<=sRow+7; cpt++)
					for (cpt2=sCol+1; cpt2<=sCol+4; cpt2++)
						clientMap[cpt][cpt2]=608;

				for (cpt=sRow+1; cpt<=sRow+4; cpt++)
					for (cpt2=sCol+1; cpt2<=sCol+8; cpt2++)
						clientMap[cpt][cpt2]=608;

				break;

			case 3 : //Large guild hall

				//Clearance

				for (cpt=sCol; cpt<sCol+19; cpt++)
				{
					//clientMap[sRow+9][cpt]=256;
					clientMap[sRow-1][cpt]=863;
				}

				/*for (cpt=sRow-1; cpt<=sRow+19; cpt++)
				{
					clientMap[cpt][sCol-1]=256;
					clientMap[cpt][sCol+9]=256;
				}*/

				//External Structure

				clientMap[sRow][sCol]=clientMap[sRow+11][sCol]=clientMap[sRow+18][sCol]=clientMap[sRow+18][sCol+7]=clientMap[sRow+11][sCol+7]=clientMap[sRow+11][sCol+12]=clientMap[sRow+18][sCol+12]=clientMap[sRow+18][sCol+18]=clientMap[sRow+11][sCol+18]=clientMap[sRow][sCol+18]=clientMap[sRow][sCol+12]=clientMap[sRow][sCol+6]=602;

				clientMap[sRow+3][sCol]=clientMap[sRow+7][sCol]=clientMap[sRow+15][sCol]=clientMap[sRow+15][sCol+18]=clientMap[sRow+7][sCol+18]=clientMap[sRow+3][sCol+18]=975;

				clientMap[sRow+1][sCol]=clientMap[sRow+2][sCol]=clientMap[sRow+14][sCol+12]=597;

				for (cpt=sRow+4; cpt<=sRow+17; cpt++)
				{
					clientMap[cpt][sCol]=597;

					if (cpt==sRow+6 || cpt==sRow+10 || cpt==sRow+14)
						cpt++;
				}

				for (cpt=sRow+12; cpt<=sRow+17; cpt++)
				{
					clientMap[cpt][sCol+7]=597;
					clientMap[cpt][sCol+12]=597;

					if (cpt==sRow+13)
						cpt++;
				}

				for (cpt=sRow+1; cpt<=sRow+17; cpt++)
				{
					clientMap[cpt][sCol+18]=597;

					if (cpt==sRow+2 || cpt==sRow+4 || cpt==sRow+6 || cpt==sRow+8 || cpt==sRow+10 || cpt==sRow+13 || cpt==sRow+14)
						cpt++;
				}

				clientMap[sRow+11][sCol+8]=clientMap[sRow+11][sCol+11]=598;

				for (cpt=sCol+1; cpt<=sCol+17; cpt++)
				{
					clientMap[sRow][cpt]=598;

					if (cpt==sCol+5)
						cpt=sCol+12;
				}

				for (cpt=sCol+1; cpt<=sCol+17; cpt++)
				{
					clientMap[sRow+18][cpt]=598;

					if (cpt==sCol+3 || cpt==sCol+14)
						cpt++;

					if (cpt==sCol+6)
						cpt=sCol+12;
				}

				for (cpt=sCol+8; cpt<=sCol+11; cpt++)
					clientMap[sRow+18][cpt]=558;

				for (cpt=sCol+7; cpt<=sCol+11; cpt++)
					clientMap[sRow][cpt]=658;

				clientMap[sRow+18][sCol+4]=clientMap[sRow+18][sCol+15]=618;

				clientMap[sRow+14][sCol+7]=clientMap[sRow+14][sCol+18]=clientMap[sRow+9][sCol+18]=clientMap[sRow+5][sCol+18]=619;

				clientMap[sRow+11][sCol+9]=634;

				clientMap[sRow+11][sCol+10]=633;

				//Indoor elements

				clientMap[sRow+16][sCol+6]=clientMap[sRow+12][sCol+6]=671;

				clientMap[sRow+15][sCol+6]=clientMap[sRow+13][sCol+6]=670;

				clientMap[sRow+15][sCol+5]=clientMap[sRow+13][sCol+5]=665;

				for (cpt=sCol+7; cpt<=sCol+12; cpt++)
				{
					clientMap[sRow+9][cpt]=639;
					clientMap[sRow+7][cpt]=640;
				}

				clientMap[sRow+8][sCol+5]=641;

				clientMap[sRow+8][sCol+13]=642;

				for (cpt=sCol+6; cpt<=sCol+12; cpt++)
				{
					clientMap[sRow+8][cpt]=628;

					if (cpt==sCol+7 || cpt==sCol+10)
						cpt++;
				}

				clientMap[sRow+8][sCol+8]=clientMap[sRow+8][sCol+11]=637;

				clientMap[sRow+15][sCol+16]=652;
				clientMap[sRow+14][sCol+16]=646;
				clientMap[sRow+17][sCol+14]=clientMap[sRow+17][sCol+16]=655;

				//Shadows

				for (cpt=sCol+1; cpt<=sCol+17; cpt++)
				{
					if (cpt<sCol+7)
						clientMap[sRow+17][cpt]=612;
					else if (cpt>sCol+6 && cpt<sCol+13)
						clientMap[sRow+10][cpt]=612;
					else
					{
						clientMap[sRow+17][cpt]=612;

						if (cpt==sCol+13 || cpt==sCol+15)
							cpt++;
					}
				}

				clientMap[sRow+17][sCol+6]=clientMap[sRow+17][sCol+17]=615;

				clientMap[sRow+14][sCol+6]=clientMap[sRow+11][sCol+6]=613;

				for (cpt=sRow+1; cpt<=sRow+16; cpt++)
				{
					clientMap[cpt][sCol+17]=613;
				}

				//Floor

				clientMap[sRow+11][sCol+5]=clientMap[sRow+16][sCol+5]=clientMap[sRow+14][sCol+5]=clientMap[sRow+12][sCol+5]=clientMap[sRow+7][sCol+5]=clientMap[sRow+7][sCol+6]=clientMap[sRow+7][sCol+13]=clientMap[sRow+16][sCol+16]=608;

				for (cpt=sRow+7; cpt<=sRow+16; cpt++)
					for (cpt2=sCol+1; cpt2<=sCol+4; cpt2++)
						clientMap[cpt][cpt2]=608;

				for (cpt=sRow+9; cpt<=sRow+10; cpt++)
					for (cpt2=sCol+5; cpt2<=sCol+6; cpt2++)
						clientMap[cpt][cpt2]=608;

				for (cpt=sRow+1; cpt<=sRow+6; cpt++)
					for (cpt2=sCol+1; cpt2<=sCol+16; cpt2++)
						clientMap[cpt][cpt2]=608;

				for (cpt=sRow+9; cpt<=sRow+16; cpt++)
					for (cpt2=sCol+13; cpt2<=sCol+15; cpt2++)
						clientMap[cpt][cpt2]=608;

				for (cpt=sRow+7; cpt<=sRow+8; cpt++)
					for (cpt2=sCol+14; cpt2<=sCol+15; cpt2++)
						clientMap[cpt][cpt2]=608;

				for (cpt=sRow+7; cpt<=sRow+13; cpt++)
					clientMap[cpt][sCol+16]=608;


				break;

		}

	cpt=0;

	while (extras[cpt].type!=65535)
	{
		clientMap[extras[cpt].row][extras[cpt].col]=extras[cpt].type;
		cpt++;
	}

	gr_RenderMapWindow(true);
}
#endif