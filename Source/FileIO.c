#include "Globals.h"

#include "Toolbox.h"
#include "Common.h"
#include "FileIO.h"
#include "Players.h"
#include "Creatures.h"
#include "Graphics.h"
#include "Items.h"
#include "Inventory.h"
#include "Skills.h"
#include "GameText.h"
#include "Shops.h"
#include "Npc.h"
#include "Resource.h"

//-----------------------------------------------

#ifdef _SERVER
extern creatureType					creature					[kMaxCreatures];
FILE												*conversationStream;
int													currentDay;
FILE												*gmStream;
extern UInt16								map[kMapWidth][kMapHeight];
#endif

#ifndef _SERVER
FILE												*conversationStream;
int													currentDay;
#endif

int                         previousBugReport;
unsigned long				loopTime;
int							traceCharacters=0;
char						textLog[1000];
unsigned long				logTime;

extern latterType							ladderJump				[kMaxLadders];

extern FSSpec				AppFSSpec;
extern short				AppRefNum;

extern TRect								shopRect								[kMaxShops];
extern char									traderName							[kMaxShops][48];

extern UInt16							  containerMap				    [kMapWidth][kMapHeight];
extern inventoryType				container						    [kMaxContainers][kMaxInventorySize];
extern unsigned long				containerDecayTime	    [kMaxContainers];
extern TBoolean						  containerEmpty			    [kMaxContainers];
extern int									containerRow				    [kMaxContainers];
extern int									containerCol				    [kMaxContainers];
extern UInt8								containerID					    [kMaxContainers];
extern UInt8								containerHidden			    [kMaxContainers];
extern UInt8								containerLock				    [kMaxContainers];
extern UInt8								containerTrap				    [kMaxContainers];
extern char								  containerTrapName		    [kMaxContainers][20];
extern UInt8								containerMagicLock	    [kMaxContainers];
extern UInt8								containerMagicTrap	    [kMaxContainers];
extern char								  containerMagicTrapName	[kMaxContainers][20];
extern char								  containerName           [kMaxContainers][48]; //[15];
extern char								  containerGift           [kMaxContainers][48]; //[15];
extern UInt8								inData							    [kDataSize];
extern UInt8								testData							  [kDataSize];
extern char									ban											[1000][18];
extern long                 checkCount;
extern long                 checkCount1;
extern long                 checkCount2;
extern long                 checkCount3;
extern long                 checkCount4;
extern long                 checkCount5;
extern long                 checkCount6;
extern long                 checkCount7;
extern long                 checkCount8;
extern long                 checkCount9;
extern long                 checkCount10;
extern long                 checkCount11;
extern long                 checkCount12;
extern long                 checkCount13;
extern long                 checkCount14;
extern long                 checkCount15;
extern unsigned long        checkGold;
extern unsigned long        checkMaxGold;
extern guildHallType		guildHalls[kMaxGuildHalls];
extern spawnRegionType		spawnRegion[kMaxSpawnRegions];
extern int          		currentNumberOfContainers;
extern int					guildHallsPtr;
extern int					guildMap[kMapWidth][kMapHeight];
extern int					maxSkillLevel[7][kMaxSkills];
extern char					localMacros[kMaxMacros][2][kStringLength];
//Dave
extern 							TBoolean						canSwimIn[kMaxIcons];
extern 							TBoolean						canWalkOn[kMaxIcons];
extern 							TBoolean						canFlyOver[kMaxIcons];
extern 							TBoolean						doorway[kMaxIcons];
extern UInt16			map						[kMapWidth][kMapHeight];
int                 r;
int                 c;
//-----------------------------------------------

#ifndef _SERVERONLY
int										rowOffset;
int 									colOffset;
UInt16 								clientMap[kMapWidth][kMapHeight];
FSSpec								worldFSSpec;
short									worldRefNum;
#endif


// ----------------------------------------------------------------- //
void fi_StartDebugTimer(void) //[128])
// ----------------------------------------------------------------- //

{

	loopTime=TickCount();

}

// ----------------------------------------------------------------- //
void fi_CheckDebugTimer(char location[kStringLength]) //[128])
// ----------------------------------------------------------------- //

{

	if (TickCount()>loopTime+(60*60))
		fi_WriteToErrorLog(location);

}

// ----------------------------------------------------------------- //
void fi_LoadMacros(void)
// ----------------------------------------------------------------- //
{
	FILE				*stream;
	char				filename[kStringLength];
	int					cpt;

	return;
	
	strcpy(filename,":Oberin Cache:Macros");

	for (cpt=0; cpt<kMaxMacros;cpt++)
	{
		strcpy(localMacros[cpt][0],"");
	}

	cpt=0;

	stream = fopen(filename, "r" );

	if (stream != NULL)
	{
		fseek(stream, 0L, SEEK_SET );

		do
		{
			fgets(localMacros[cpt][0], kStringLength, stream);

			localMacros[cpt][0][strlen(localMacros[cpt][0])-1]='\0';

			fgets(localMacros[cpt][1], kStringLength, stream);

			localMacros[cpt][1][strlen(localMacros[cpt][1])-1]='\0';

			if (localMacros[cpt][0][0]!='/')
			{
				fi_WriteToErrorLog("Invalid Macro");
				strcpy(localMacros[cpt][0],"");
				strcpy(localMacros[cpt][1],"");
			}
			else
				cpt++;
		}
		while (!feof(stream) && cpt<kMaxMacros);

		fclose(stream);
	}
}

// ----------------------------------------------------------------- //
void fi_ScanFile(char name[kStringLength]) //[128])
// ----------------------------------------------------------------- //

{
	UInt32          skill[kMaxSkills];
	int             j;
	//int             total;
	//int             max;
	//int             level;
	char            filename[kStringLength];
	FILE					  *stream;
	UInt32					inValue;
	char						inString[kStringLength];
	UInt32					gold1;
	UInt32					gold2;
	TBoolean        inGuild;

	strcpy(filename,":Accounts Check:");
	strcat(filename,name);

	checkGold=0;
	inGuild=true;
	stream = fopen(filename, "r" );

	if (stream != NULL)
	{

		fseek( stream, 0L, SEEK_SET );

		//fscanf( stream, "%s", inString);//password
		fgets(inString,kStringLength,stream);

		//fscanf( stream, "%s", inString);// player name
		fgets(inString,kStringLength,stream);

		//fscanf( stream, "%s", inString);// guild name
		fgets(inString,kStringLength,stream);

///      if (strcmp(inString,"None")!=0)
//        {
		//checkCount++;
//          inGuild=true;
//        }

//      if (strcmp(inString,"MR*Apprentice")==0)
//        {
//          checkCount++;
//          inGuild=true;
//        }

		fscanf( stream, "%i", &inValue);//class
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//sharing
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//demo
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//new player
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//account num
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//row
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//col
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//thief
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//title
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//gender
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//category
		fgets(inString,kStringLength,stream);

		if (inValue!=0)
		{
			fclose( stream );
			return;
		}

		fscanf( stream, "%i", &inValue);//dead

		fgets(inString,kStringLength,stream);

		//fscanf( stream, "%s", inString);//line break
		fgets(inString,kStringLength,stream);

		for (j=0;j<13;j++)
		{
			fscanf( stream, "%i", &inValue);//arm
			fgets(inString,kStringLength,stream);
		}

		fscanf( stream, "%i", &inValue);//hunger

		fgets(inString,kStringLength,stream);

		//fscanf( stream, "%s", inString);//line break
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);//str
		fgets(inString,kStringLength,stream);
		fscanf( stream, "%i", &inValue);//int
		fgets(inString,kStringLength,stream);
		fscanf( stream, "%i", &inValue);//dex
		fgets(inString,kStringLength,stream);
		fscanf( stream, "%i", &inValue);//hp
		fgets(inString,kStringLength,stream);
		fscanf( stream, "%i", &inValue);//mp
		fgets(inString,kStringLength,stream);

		//fscanf( stream, "%s", inString);//line break
		fgets(inString,kStringLength,stream);

		for (j=0;j<kMaxSkills;j++)
		{
			fscanf( stream, "%i", &inValue);//skills
			fgets(inString,kStringLength,stream);
			skill[j]  =	(UInt32)inValue;
		}

		//fscanf( stream, "%s", inString);//line break
		fgets(inString,kStringLength,stream);

		for (j=0;j<kMaxInventorySize;j++)
		{
			fscanf( stream, "%i", &gold1); //inv item
			fgets(inString,kStringLength,stream);
			fscanf( stream, "%i", &gold2); //inv count
			fgets(inString,kStringLength,stream);

			if ((j==0) && (inGuild))
				checkGold=checkGold+in_GetGoldTotal(gold1, gold2);
		}

		//fscanf( stream, "%s", inString);//line break
		fgets(inString,kStringLength,stream);

		for (j=0;j<kMaxInventorySize;j++)
		{
			fscanf( stream, "%i", &gold1); //bank item
			fgets(inString,kStringLength,stream);
			fscanf( stream, "%i", &gold2); //bank count
			fgets(inString,kStringLength,stream);

			if ((j==0) && (inGuild))
				checkGold=checkGold+in_GetGoldTotal(gold1, gold2);
		}

		//fscanf( stream, "%s", inString);//line break
		fgets(inString,kStringLength,stream);

		for (j=0;j<kMaxPets;j++)
		{
			fscanf( stream, "%i", &inValue);
			fgets(inString,kStringLength,stream);
			//fscanf( stream, "%s", inString);
			fgets(inString,kStringLength,stream);
		}

		fclose( stream );

	}

	checkCount++;

	checkMaxGold=checkMaxGold+checkGold;

	if ((checkGold>=0) && (checkGold<5000))
		checkCount1++;

	if ((checkGold>=5000) && (checkGold<10000))
		checkCount2++;

	if ((checkGold>=10000) && (checkGold<15000))
		checkCount3++;

	if ((checkGold>=15000) && (checkGold<20000))
		checkCount4++;

	if ((checkGold>=20000) && (checkGold<25000))
		checkCount5++;

	if ((checkGold>=25000) && (checkGold<35000))
		checkCount6++;

	if ((checkGold>=35000) && (checkGold<50000))
		checkCount7++;

	if ((checkGold>=50000) && (checkGold<75000))
		checkCount8++;

	if ((checkGold>=75000) && (checkGold<100000))
		checkCount9++;

	if ((checkGold>=100000) && (checkGold<150000))
		checkCount10++;

	if ((checkGold>=150000) && (checkGold<200000))
		checkCount11++;

	if ((checkGold>=200000) && (checkGold<300000))
		checkCount12++;

	if ((checkGold>=300000) && (checkGold<500000))
		checkCount13++;

	if ((checkGold>=500000) && (checkGold<1000000))
		checkCount14++;

	if (checkGold>=1000000)
		checkCount15++;

}

/*
// ----------------------------------------------------------------- //
void fi_ScanFile(char name[kStringLength]) //[128])
// ----------------------------------------------------------------- //

{
  UInt32          skill[kMaxSkills];
  int             j;
  int             total;
  int             max;
  int             level;
  char            filename[kStringLength];
	FILE					  *stream;
	UInt32					inValue;
	char						inString[kStringLength];

	strcpy(filename,":Accounts Check:");
	strcat(filename,name);

	stream = fopen(filename, "r" );
	if (stream != NULL)
		{

			fseek( stream, 0L, SEEK_SET );

			fscanf( stream, "%s", inString);

			fscanf( stream, "%s", inString);

			fscanf( stream, "%s", inString);

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%i", &inValue);
			if ((inValue==1) || (inValue==2))
			  {
      fi_WriteToErrorLog(name);
			    fclose( stream );
			    return;
			  }

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%s", inString);

			for (j=0;j<13;j++)
				{
					fscanf( stream, "%i", &inValue);
				}

			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%s", inString);

			fscanf( stream, "%i", &inValue);
			fscanf( stream, "%i", &inValue);
			fscanf( stream, "%i", &inValue);
			fscanf( stream, "%i", &inValue);
			fscanf( stream, "%i", &inValue);

			fscanf( stream, "%s", inString);

			for (j=0;j<kMaxSkills;j++)
				{
					fscanf( stream, "%i", &inValue);
					skill[j]  =	(UInt32)inValue;
				}

			fscanf( stream, "%s", inString);

			for (j=0;j<kMaxInventorySize;j++)
				{
					fscanf( stream, "%i", &inValue);
					if (j>0)
					  {
  					  if (in_CanGroup(inValue))
  					    {
					        fscanf( stream, "%i", &inValue);
					        if (inValue>kMaxInventoryItemCount)
  	                fi_WriteToErrorLog(name);
  					    }
  					  else
  					    {
					        fscanf( stream, "%i", &inValue);
					        if (inValue>255)
  	                fi_WriteToErrorLog(name);
  					    }
					  }
					else
					  fscanf( stream, "%i", &inValue);
				}

			fscanf( stream, "%s", inString);

			for (j=0;j<kMaxInventorySize;j++)
				{
					fscanf( stream, "%i", &inValue);
					if (j>0)
					  {
  					  if (in_CanGroup(inValue))
  					    {
					        fscanf( stream, "%i", &inValue);
					        if (inValue>kMaxInventoryItemCount)
  	                fi_WriteToErrorLog(name);
  					    }
  					  else
  					    {
					        fscanf( stream, "%i", &inValue);
					        if (inValue>255)
  	                fi_WriteToErrorLog(name);
  					    }
					  }
					else
					  fscanf( stream, "%i", &inValue);
				}

			fscanf( stream, "%s", inString);

			for (j=0;j<kMaxPets;j++)
				{
					fscanf( stream, "%i", &inValue);
			    fscanf( stream, "%s", inString);
				}

			fclose( stream );

		}


 	level=sk_GetSkillLevel(skill[30]);
 	if (level>=8)
  	fi_WriteToErrorLog(name);


  max   = 0;
  total = 0;
  level = 0;
  for (j=0;j<kMaxSkills;j++)
  	{
  	  level=sk_GetSkillLevel(skill[j]);
  		total=total+level;
  		if (level>max)
  		  max=level;
  	}

  if ((max>4) && (total<11))
    {
      fi_WriteToErrorLog("Cheater -------------------");
      fi_WriteToErrorLog(name);
    }
  else if ((max>7) && (total<14))
    {
      fi_WriteToErrorLog("Cheater -------------------");
      fi_WriteToErrorLog(name);
    }
  else if (max>7)
    {
      fi_WriteToErrorLog("Maybe -------------------");
      fi_WriteToErrorLog(name);
    }


}
*/

#ifndef _SERVERONLY
// ----------------------------------------------------------------- //
void fi_OpenMapFile(void)
// ----------------------------------------------------------------- //

{
	OSErr					e;

	return; // gs map change

	FSMakeFSSpec(AppFSSpec.vRefNum, AppFSSpec.parID, "\pOberin World", &worldFSSpec);
	e=FSpOpenDF (&worldFSSpec, fsCurPerm, &worldRefNum);

}

// ----------------------------------------------------------------- //
void fi_CloseMapFile(void)
// ----------------------------------------------------------------- //

{
	OSErr					e;

	return; // gs map change

	e	=	FSClose(worldRefNum);

}

// ----------------------------------------------------------------- //
void fi_LoadClientMap(int row, int col)
// ----------------------------------------------------------------- //

{
	//int						i;
	int						x;
	OSErr					e;
	long					s;
	TBoolean			getFromDisk=false;
	int						testRow;
	int						testCol;
	int						prowOffset=rowOffset;
	int						pcolOffset=colOffset;
	TBoolean      posError;
	TBoolean      readError;

	return; // gs map change

	if ((row>=65000) || (col>=65000))
		return;

	testRow=(playerRow-75);

	if (testRow<0)
		testRow=0;

	if (testRow<rowOffset)
		getFromDisk=true;

	testCol=(playerCol-75);

	if (testCol<0)
		testCol=0;

	if (testCol<colOffset)
		getFromDisk=true;

	testRow=(playerRow+75);

	if (testRow>=kMapWidth)
		testRow=kMapWidth-1;

	if (testRow>rowOffset+400)
		getFromDisk=true;

	testCol=(playerCol+75);

	if (testCol>=kMapHeight)
		testCol=kMapHeight-1;

	if (testCol>colOffset+400)
		getFromDisk=true;

	if (getFromDisk)
	{

		if (row<0)
			row=0;

		if (col<0)
			col=0;

		if (row+400>=kMapWidth)
			row=kMapWidth-401;

		if (col+400>=kMapHeight)
			col=kMapHeight-401;

		rowOffset=row;

		colOffset=col;

		if ((rowOffset==prowOffset) && (colOffset=pcolOffset)		)
			return;

#ifdef _DEBUGGING
		gr_AddText("Loading map segment.",true,true);	// beta

#endif

		s=800;

		posError=false;

		readError=false;

		for (x=0;x<400;x=x+1)
		{
			e = SetFPos(worldRefNum, fsFromStart, (((rowOffset+x)*kMapWidth+colOffset)*2));

			if (e!=noErr)
				posError=true;

			e	=	FSRead(worldRefNum, &s, &clientMap[x][0]);

			if (e!=noErr)
				readError=true;
		}

		if (posError)
			fi_WriteToErrorLog("Map file position error");

		if (readError)
			fi_WriteToErrorLog("Map file read error");

	}

}

#endif

// ----------------------------------------------------------------- //
void fi_WriteToBugLog(int i, char errorMessage[kStringLength]) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*bugStream;
	int                   j;
	DateTimeRec						Date;
	unsigned long					Secs;
	char									dateAndTime[kStringLength]; //[128];
	char									temp[kStringLength]; //[50];
	long                  t;


	if (previousBugReport==i)
	{
		bugStream = fopen(":Oberin Logs:Oberin Bug Log", "a" );

		if (bugStream != NULL)
		{
			fprintf( bugStream, "%s", "---------------------------------");
			fprintf( bugStream, "%s", "\n");
			fprintf( bugStream, "%s", errorMessage);
			fprintf( bugStream, "%s", "\n");
			fprintf( bugStream, "%s", "---------------------------------");
			fprintf( bugStream, "%s", "\n");
			fclose( bugStream );
		}

		return;
	}

	previousBugReport=i;

	strcpy(dateAndTime,"Time:   ");

	t = ReadDateTime(&Secs);
	SecondsToDate(Secs, &Date);

	tb_IntegerToString(Date.month,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime,"/");

	tb_IntegerToString(Date.day,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime,"/");

	tb_IntegerToString(Date.year,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime," at ");

	tb_IntegerToString(Date.hour,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime,":");

	tb_IntegerToString(Date.minute,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime,":");

	tb_IntegerToString(Date.second,temp);
	strcat(dateAndTime,temp);

	bugStream = fopen(":Oberin Logs:Oberin Bug Log", "a" );

	if (bugStream != NULL)
	{
		fprintf( bugStream, "%s", "---------------------------------");
		fprintf( bugStream, "%s", "\n");

		fprintf( bugStream, "%s", "- NEW BUG -");
		fprintf( bugStream, "%s", "\n");

		fprintf( bugStream, "%s", "---------------------------------");
		fprintf( bugStream, "%s", "\n");

		fprintf( bugStream, "%s", player[i].userName);
		fprintf( bugStream, "%s", "\n");

		fprintf( bugStream, "%s", player[i].playerName);
		fprintf( bugStream, "%s", "\n");

		fprintf( bugStream, "%s", dateAndTime);
		fprintf( bugStream, "%s", "\n");

		fprintf( bugStream, "%s", "--- location");
		fprintf( bugStream, "%s", "\n");

		fprintf( bugStream, "%i", (int)creature[player[i].creatureIndex].row);
		fprintf( bugStream, "%s", "\n");

		fprintf( bugStream, "%i", (int)creature[player[i].creatureIndex].col);
		fprintf( bugStream, "%s", "\n");

		fprintf( bugStream, "%s", "--- inv");
		fprintf( bugStream, "%s", "\n");

		for (j=0;j<kMaxInventorySize;j++)
		{
			fprintf( bugStream, "%i", (int)creature[player[i].creatureIndex].inventory[j].itemID);
			fprintf( bugStream, "%s", "\n");

			fprintf( bugStream, "%i", (int)creature[player[i].creatureIndex].inventory[j].itemCount);
			fprintf( bugStream, "%s", "\n");
		}

		fprintf( bugStream, "%s", "--- armed");

		fprintf( bugStream, "%s", "\n");

		for (j=0;j<13;j++)
		{
			fprintf( bugStream, "%i", (int)player[i].arm[j]);
			fprintf( bugStream, "%s", "\n");
		}

		fprintf( bugStream, "%s", "---------------------------------");

		fprintf( bugStream, "%s", "\n");
		fprintf( bugStream, "%s", errorMessage);
		fprintf( bugStream, "%s", "\n");
		fprintf( bugStream, "%s", "---------------------------------");
		fprintf( bugStream, "%s", "\n");
		fclose( bugStream );
	}
}

// ----------------------------------------------------------------- //
void fi_WriteToTextLog(char errorMessage[kStringLength]) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*errorStream;
	DateTimeRec								Date;
	unsigned long							Secs;
	char									dateAndTime[kStringLength]; //[128];
	long									t;
	char									temp[kStringLength]; //[50];
	OSErr                 					err;
	long									theDirID;
	TBoolean								isDirectory;
	
	if (TickCount()>logTime+90*60 || strlen(errorMessage)+strlen(textLog)>999) //time to log
	{
		logTime=TickCount();
		
		err=GetDirectoryID(AppFSSpec.vRefNum,AppFSSpec.parID,"\pOberin Logs",&theDirID,&isDirectory);

		if ((err==fnfErr) || (err==dirNFErr))
			DirCreate(AppFSSpec.vRefNum,AppFSSpec.parID,"\pOberin Logs",&theDirID);

		strcpy(dateAndTime,":Oberin Logs:Log ");

		t = ReadDateTime(&Secs);

		SecondsToDate(Secs, &Date);

		tb_IntegerToString(Date.month,temp);

		strcat(dateAndTime,temp);

		strcat(dateAndTime,"/");

		tb_IntegerToString(Date.day,temp);

		strcat(dateAndTime,temp);

		strcat(dateAndTime,"/");

		tb_IntegerToString(Date.year,temp);

		strcat(dateAndTime,temp);

		errorStream = fopen(dateAndTime, "a" );

		if (errorStream != NULL)
		{
			fprintf( errorStream, "%s", textLog);
			//fprintf( errorStream, "%s", "\n");
			if (strcmp(errorMessage, "")!=0)
				fprintf( errorStream, "\n%s\n", errorMessage);
			fclose( errorStream );
		}
		
		strcpy(textLog, "");		
	}
	else //delayed log to prevent log abuse
	{
		strcat(textLog, errorMessage);
		strcat(textLog,"\n");	
	}
}

// ----------------------------------------------------------------- //
void fi_WriteToDebuggerLog(char errorMessage[kStringLength]) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*errorStream;


	errorStream = fopen(":Oberin Logs:Oberin Debugger Log", "a" );

	if (errorStream != NULL)
	{
		fprintf( errorStream, "%s", errorMessage);
		fprintf( errorStream, "%s", "\n");
		fclose( errorStream );
	}
}

// ----------------------------------------------------------------- //
void fi_WriteToQuestFile(int number, int gold, int experience, int item, int count, char quest[1024]) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*questStream;
	char									filename[kStringLength];
	char									filenumber[kStringLength];
	//int                   i;

	//for (i=0;i<strlen(quest);i++)
	//	if (quest[i]==' ')
	//		quest[i]='*';

	tb_IntegerToString(number,filenumber);
	strcpy(filename,":Oberin Cache:");
	strcat(filename,filenumber);

	questStream = fopen(filename, "w" );

	if (questStream != NULL)
	{
		fprintf( questStream, "%i", (int)gold);
		fprintf( questStream, "%s", "\n");

		fprintf( questStream, "%i", (int)experience);
		fprintf( questStream, "%s", "\n");

		fprintf( questStream, "%i", (int)item);
		fprintf( questStream, "%s", "\n");

		fprintf( questStream, "%i", (int)count);
		fprintf( questStream, "%s", "\n");

		fprintf( questStream, "%s", quest);
		fprintf( questStream, "%s", "\n");

		fclose( questStream );
	}

	//for (i=0;i<strlen(quest);i++)
	//	if (quest[i]=='*')
	//		quest[i]=' ';

}

// ----------------------------------------------------------------- //
void fi_SaveSignMessage(int row, int col, char signStr[kStringLength])
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	FILE									*signStream;
	char									filename[kStringLength];
	char									filenumber[kStringLength];
	//int                   i;
	char									theSign[kStringLength];

	strcpy(filename,":Oberin Signs:Sign ");
	tb_IntegerToString(row,filenumber);
	strcat(filename,filenumber);
	strcat(filename,"-");
	tb_IntegerToString(col,filenumber);
	strcat(filename,filenumber);

	signStream = fopen(filename, "w" );

	if (signStream != NULL)
	{
		strcpy(theSign,signStr);

		if (strcmp(theSign,"")==0)
		{
			if (map[row][col]==1048)
				strcpy(theSign,"This house does not have an owner.");
		}

		fprintf( signStream, "%s", theSign);

		fprintf( signStream, "%s", "\n");

		fclose( signStream );
	}

#endif
}

// ----------------------------------------------------------------- //
void fi_SaveGuildHall(guildHallType theHall)
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	FILE									*hallStream;
	char									filename[kStringLength];
	char									filenumber[kStringLength];
	int                   					i;
	char									theChar[kStringLength], otherChar[kStringLength];

	strcpy(filename,":Oberin Halls:Hall ");
	tb_IntegerToString(theHall.row,filenumber);
	strcat(filename,filenumber);
	strcat(filename,"-");
	tb_IntegerToString(theHall.col,filenumber);
	strcat(filename,filenumber);

	hallStream = fopen(filename, "w" );

	if (hallStream != NULL)
	{
		tb_NumToString(theHall.saved, theChar);
		fprintf( hallStream, "%s", theChar);
		fprintf( hallStream, "%s", "\n");
		fprintf( hallStream, "%s", theHall.guild);
		fprintf( hallStream, "%s", "\n");
		tb_NumToString(theHall.size, theChar);
		fprintf( hallStream, "%s", theChar);
		fprintf( hallStream, "%s", "\n");

		for (i=0; i<kMaxExtras; i++)
		{
			if (theHall.extras[i].type!=0)
			{
				strcpy(theChar, "#extra(");
				tb_NumToString(theHall.extras[i].type, otherChar);
				strcat(theChar, otherChar);
				strcat(theChar, ",");
				tb_NumToString(theHall.extras[i].row, otherChar);
				strcat(theChar, otherChar);
				strcat(theChar, ",");
				tb_NumToString(theHall.extras[i].col, otherChar);
				strcat(theChar, otherChar);
				strcat(theChar, ")");
				fprintf( hallStream, "%s", theChar);
				fprintf( hallStream, "%s", "\n");
			}
		}

		for (i=0; i<kMaxGuildChests; i++)
		{
			if (theHall.chests[i][0]!=0)
			{
				strcpy(theChar, "#chest(");
				tb_NumToString(theHall.chests[i][1], otherChar);
				strcat(theChar, otherChar);
				strcat(theChar, ",");
				tb_NumToString(theHall.chests[i][2], otherChar);
				strcat(theChar, otherChar);
				strcat(theChar, ")");
				fprintf( hallStream, "%s", theChar);
				fprintf( hallStream, "%s", "\n");
				fi_SaveContainer(theHall.chests[i][1], theHall.chests[i][2]);
			}
		}

		fclose( hallStream );
	}

	fi_SaveAllGuildHalls();

#endif
}

// ----------------------------------------------------------------- //
void fi_SaveAllGuildHalls()
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	FILE									*hallStream;
	char									filename[kStringLength];
	int                   					i;
	char									theChar[kStringLength];

	strcpy(filename,":Oberin Halls:Halls Locations");

	hallStream = fopen(filename, "w" );

	if (hallStream != NULL)
	{
		for (i=1; i<=guildHallsPtr; i++)
		{
			tb_NumToString(guildHalls[i].row, theChar);
			fprintf( hallStream, "%s", theChar);
			fprintf( hallStream, "%s", "-");

			tb_NumToString(guildHalls[i].col, theChar);
			fprintf( hallStream, "%s", theChar);
			fprintf( hallStream, "%s", "\n");
		}

		fclose( hallStream );
	}

#endif
}

// ----------------------------------------------------------------- //
void fi_LoadGuildHalls()
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	FILE									*hallStream;
	char									filename[kStringLength];
	char									inString[kStringLength];
	char									cRow[kStringLength], cCol[kStringLength];
	long									row, col;
	int										j=0, cpt, cpt2;

	strcpy(filename,":Oberin Halls:Halls Locations");

	hallStream = fopen(filename, "r" );

	if (hallStream != NULL)
	{
		fseek(hallStream, 0L, SEEK_SET );

		while (!feof(hallStream))
		{
			strcpy(inString, "");

			fgets(inString, kStringLength, hallStream);

			if (feof(hallStream))
			{
				fclose(hallStream);
				return;
			}

			for (cpt=0; cpt<5; cpt++)
			{
				cRow[cpt]='\0';
				cCol[cpt]='\0';
			}

			cpt2=strcspn(inString, "-");

			for (cpt=0; cpt<cpt2; cpt++)
			{
				cRow[cpt]=inString[cpt];
			}

			j=0;

			for (cpt=cpt2+1; cpt<strlen(inString)-1; cpt++)
			{
				cCol[j++]=inString[cpt];
			}

			tb_StringToNum(cRow, &row);

			tb_StringToNum(cCol, &col);
			fi_LoadGuildHall(row, col);
		}
	}

	fclose(hallStream);

#endif
}

// ----------------------------------------------------------------- //
void fi_LoadGuildHall(int row, int col)
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	FILE									*hallStream;
	char									filename[kStringLength];
	char									fileNumber[kStringLength];
	int                   					oRow, oCol, eRow, eCol, i, j, start, extraPtr=0, chestPtr=0;
	long									size, saved;
	long									extras[kMaxExtras][3], chests[kMaxGuildChests][2];
	char									guild[kStringLength], theChar[kStringLength], otherChar[kStringLength];
	guildHallType							theHall;

	strcpy(filename,":Oberin Halls:Hall ");
	tb_IntegerToString(row, fileNumber);
	strcat(filename, fileNumber);
	strcat(filename, "-");
	tb_IntegerToString(col, fileNumber);
	strcat(filename, fileNumber);

	hallStream = fopen(filename, "r" );

	if (hallStream != NULL)
	{
		fseek(hallStream, 0L, SEEK_SET );
		fgets(theChar, kStringLength, hallStream);
		theChar[strlen(theChar)-1]='\0';
		tb_StringToNum(theChar, &saved);
		fgets(theChar, kStringLength, hallStream);
		theChar[strlen(theChar)-1]='\0';
		strcpy(guild, theChar);
		fgets(theChar, kStringLength, hallStream);
		theChar[strlen(theChar)-1]='\0';
		tb_StringToNum(theChar, &size);

		while (!feof(hallStream))
		{
			strcpy(theChar, "");
			fgets(theChar, kStringLength, hallStream);

			if (feof(hallStream))
				goto endread;

			if (strncmp(theChar, "#extra(", 7)==0)
			{
				j=0;
				start=7;

				for (i=7; i<strlen(theChar); i++)
				{
					if (theChar[i]!=',')
					{
						otherChar[j]=theChar[i];
						otherChar[j+1]='\0';
						j++;

						if (j>=kStringLength)
							break;

						start++;
					}
					else
						break;
				}

				tb_StringToNum(otherChar, &extras[extraPtr][0]);

				j=0;
				start++;

				for (i=start; i<strlen(theChar); i++)
				{
					if (theChar[i]!=',')
					{
						otherChar[j]=theChar[i];
						otherChar[j+1]='\0';
						j++;

						if (j>=kStringLength)
							break;

						start++;
					}
					else
						break;
				}

				tb_StringToNum(otherChar, &extras[extraPtr][1]);

				j=0;
				start++;

				for (i=start; i<strlen(theChar); i++)
				{
					if (theChar[i]!=')')
					{
						otherChar[j]=theChar[i];
						otherChar[j+1]='\0';
						j++;

						if (j>=kStringLength)
							break;

						start++;
					}
					else
						break;
				}

				tb_StringToNum(otherChar, &extras[extraPtr][2]);

				extraPtr++;
			}

			if (strncmp(theChar, "#chest(", 7)==0)
			{
				j=0;
				start=7;

				for (i=7; i<strlen(theChar); i++)
				{
					if (theChar[i]!=',')
					{
						otherChar[j]=theChar[i];
						otherChar[j+1]='\0';
						j++;

						if (j>=kStringLength)
							break;

						start++;
					}
					else
						break;
				}

				tb_StringToNum(otherChar, &chests[chestPtr][0]);

				j=0;
				start++;

				for (i=start; i<strlen(theChar); i++)
				{
					if (theChar[i]!=')')
					{
						otherChar[j]=theChar[i];
						otherChar[j+1]='\0';
						j++;

						if (j>=kStringLength)
							break;

						start++;
					}
					else
						break;
				}

				tb_StringToNum(otherChar, &chests[chestPtr][1]);

				chestPtr++;
			}
		}

endread:

		fclose(hallStream);
		guildHallsPtr++;
		theHall.size=size;
		theHall.row=row;
		theHall.col=col;
		theHall.saved=saved;
		strcpy(theHall.guild, guild);

		for (i=0; i<extraPtr; i++)
		{
			theHall.extras[i].type=extras[i][0];
			theHall.extras[i].row=extras[i][1];
			theHall.extras[i].col=extras[i][2];
		}

		for (i=extraPtr; i<kMaxExtras; i++)
			theHall.extras[i].type=0;

		for (i=0; i<chestPtr; i++)
		{
			fi_LoadContainer(30, chests[i][0], chests[i][1]);
			j=containerMap[chests[i][0]][chests[i][1]];
			theHall.chests[i][0]=j;
			theHall.chests[i][1]=chests[i][0];
			theHall.chests[i][2]=chests[i][1];
		}

		for (i=chestPtr; i<kMaxGuildChests; i++)
			theHall.chests[i][0]=0;

		guildHalls[guildHallsPtr]=theHall;

		switch (size)
		{
			case 1: //Small
				eRow=7;
				eCol=9;
				break;
			case 2: //Medium
				eRow=10;
				eCol=11;
				break;
			case 3: //Large
				eRow=eCol=19;
				break;
			default:
				return;

		}

		for (oRow=row; oRow<row+eRow; oRow++)
			for (oCol=col; oCol<col+eCol; oCol++)
			{
				guildMap[oRow][oCol]=guildHallsPtr;
			}


		gh_BuildGuildHallServer(theHall);

		fi_SaveGuildHall(theHall);
	}

#endif
}

// ----------------------------------------------------------------- //
void fi_LoadSignMessage(int row, int col, char signStr[kStringLength])
// ----------------------------------------------------------------- //

{
	FILE									*signStream;
	char									filename[kStringLength];
	char									filenumber[kStringLength];
	//UInt32								inValue;
	//int                   i;
	char									inStr[kStringLength];

	strcpy(filename,":Oberin Signs:Sign ");
	tb_IntegerToString(row,filenumber);
	strcat(filename,filenumber);
	strcat(filename,"-");
	tb_IntegerToString(col,filenumber);
	strcat(filename,filenumber);

	signStream = fopen(filename, "r" );

	if (signStream != NULL)
	{
		fseek( signStream, 0L, SEEK_SET );
		fgets(inStr,kStringLength,signStream);
		inStr[strlen(inStr)-1]='\0';
		strcpy(signStr,"Sign: ");

		if ((strlen(signStr)+strlen(inStr))>=kStringLength)
			strcpy(signStr,"Error: Sign text too long.");
		else
			strcat(signStr,inStr);

		fclose( signStream );
	}
	else
	{
		strcpy(signStr,"");
	}

}

// ----------------------------------------------------------------- //
void fi_LoadContainer(int type, int row, int col)
// ----------------------------------------------------------------- //

{
	FILE									*conStream;
	char									filename[kStringLength];
	char									filenumber[kStringLength];
	char									inString[kStringLength];
	UInt32								inValue;
	//int                   i;
	int                   j;
	int                   con;



	con=in_GetEmptyContainer();

	if (con<0)
		return;

	in_ClearContainer(con);

	strcpy(filename,":Oberin Containers:Con ");

	tb_IntegerToString(row,filenumber);

	strcat(filename,filenumber);

	strcat(filename,"-");

	tb_IntegerToString(col,filenumber);

	strcat(filename,filenumber);

	conStream = fopen(filename, "r" );

	if (conStream != NULL)
	{
		fseek( conStream, 0L, SEEK_SET );

		for (j=0;j<kMaxInventorySize;j++)
		{
			fscanf( conStream, "%i", &inValue);		container[con][j].itemID			=	(UInt16)inValue;
			fgets(inString,kStringLength,conStream);
			fscanf( conStream, "%i", &inValue);		container[con][j].itemCount		=	(UInt16)inValue;
			fgets(inString,kStringLength,conStream);
		}

		fscanf( conStream, "%i", &inValue);		containerID[con]		              =	(UInt8)inValue;

		fgets(inString,kStringLength,conStream);
		fscanf( conStream, "%i", &inValue);		containerLock[con]		            =	(UInt8)inValue;
		fgets(inString,kStringLength,conStream);
		fscanf( conStream, "%i", &inValue);		containerTrap[con]		            =	(UInt8)inValue;
		fgets(inString,kStringLength,conStream);
		fscanf( conStream, "%i", &inValue);		containerMagicLock[con]		        =	(UInt8)inValue;
		fgets(inString,kStringLength,conStream);
		fscanf( conStream, "%i", &inValue);		containerMagicTrap[con]		        =	(UInt8)inValue;
		fgets(inString,kStringLength,conStream);

		//fscanf( conStream, "%s", inString);
		fgets(inString,kStringLength,conStream);
		inString[strlen(inString)-1]='\0';

		if ((strlen(inString)==0) || (strlen(inString)>=48))
			strcpy(containerGift[con],"");
		else
		{
			strcpy(containerGift[con],inString);
			//for (i=0;i<strlen(containerGift[con]);i++)
			//	if (containerGift[con][i]=='*')
			//		containerGift[con][i]=' ';
		}

		if (strcmp(containerGift[con],"All")==0)
			strcpy(containerGift[con],"");

		fclose( conStream );
	}
	else
	{
		containerID[con]          = type;
	}

	containerHidden[con]      = false;

	containerEmpty[con]       = false;
	containerMap[row][col]    = con;
	containerRow[con]         = row;
	containerCol[con]         = col;
	containerDecayTime[con]   = 0;

	currentNumberOfContainers++;

}

// ----------------------------------------------------------------- //
void fi_LoadAllContainers(void)
// ----------------------------------------------------------------- //

{

	/*OSStatus			status;
	FSSpec				theDir;
	FSRef				containerDir;
	Str255				thePath="\p:";//"\p:Oberin Containers:";
	CatPositionRec		theIterator;
	HFSUniStr255		theFileUnicode;
	CFStringRef			theFileCFS;
	char*				theFile;
	char				cRow[kStringLength], cCol[kStringLength];
	int 				cpt, cpt2;
	long				row, col;

	theIterator.initialize=0;

	FSMakeFSSpec(0,0, thePath, &theDir);
	FSpMakeFSRef (&theDir,&containerDir);



	while ((status=FSIterateForks(&containerDir, &theIterator, &theFileUnicode,NULL, NULL))==noErr)
	{
		theFileCFS = CFStringCreateWithCharacters( kCFAllocatorDefault, theFileUnicode.unicode, theFileUnicode.length );
		theFile= NewPtr(CFStringGetLength(theFileCFS)+1);
		CFStringGetCString(theFileCFS, theFile, CFStringGetLength(theFileCFS)+1, kCFStringEncodingMacRoman);
		
		for (cpt=0; cpt<4; cpt++)
		{
			cRow[cpt]='\0';
			cCol[cpt]='\0';
		}
		
		cpt2=strcspn(theFile, "-");
		for (cpt=0; cpt<cpt2; cpt++)
		{
			cRow[cpt]=theFile[cpt];
		}
		for (cpt=cpt2+1; cpt<strlen(theFile); cpt++)
		{
			cCol[cpt]=theFile[cpt];
		}
		tb_StringToNum(cRow, &row);
		tb_StringToNum(cCol, &col);
		gr_AddText("Loading container", 1, 1);
		fi_LoadContainer(30, row, col);
		CFRelease(theFileCFS);
		DisposePtr(theFile);
		gr_AddText("found", 1, 1);
	}*/

	return;

	/*fi_LoadContainer(30,2294,1919);
	fi_LoadContainer(30,2294,1934);
	fi_LoadContainer(30,2285,1924);
	fi_LoadContainer(30,2285,1923);
	fi_LoadContainer(30,2285,1922);
	fi_LoadContainer(30,2285,1921);
	fi_LoadContainer(30,2285,1920);
	fi_LoadContainer(30,2285,1919);
	fi_LoadContainer(30,2283,1921);
	fi_LoadContainer(30,2283,1922);
	fi_LoadContainer(30,2283,1923);

	// houses------

	fi_LoadContainer(30,2108,1968);
	fi_LoadContainer(30,2099,1969);
	fi_LoadContainer(30,2106,1979);
	fi_LoadContainer(30,2089,1970);
	fi_LoadContainer(30,2092,1978);
	fi_LoadContainer(30,2097,1989);
	fi_LoadContainer(30,2081,1980);
	fi_LoadContainer(30,2084,1988);
	fi_LoadContainer(30,2074,1991);
	fi_LoadContainer(30,2069,1996);
	fi_LoadContainer(30,2061,1994);
	fi_LoadContainer(30,2060,2003);
	fi_LoadContainer(30,2085,2000);
	fi_LoadContainer(30,2084,2000);
	fi_LoadContainer(30,2083,2000);
	fi_LoadContainer(30,2069,2010);
	fi_LoadContainer(30,2068,2010);
	fi_LoadContainer(30,2068,2012);
	fi_LoadContainer(30,2062,2023);
	fi_LoadContainer(30,2061,2021);
	fi_LoadContainer(30,2059,2021);

	fi_LoadContainer(30,2980,2058);
	fi_LoadContainer(30,2987,2054);
	fi_LoadContainer(30,2989,2062);
	fi_LoadContainer(30,2961,2077);
	fi_LoadContainer(30,2972,2078);
	fi_LoadContainer(30,2029,2275);
	fi_LoadContainer(30,2029,2282);
	fi_LoadContainer(30,2014,2275);
	fi_LoadContainer(30,2017,2263);
	fi_LoadContainer(30,2017,2252);
	fi_LoadContainer(30,2636,1525);
	fi_LoadContainer(30,2642,1531);
	fi_LoadContainer(30,2059,1897);
	fi_LoadContainer(30,2068,1895);
	fi_LoadContainer(30,2076,1895);
	fi_LoadContainer(30,2073,1886);
	fi_LoadContainer(30,2086,1899);
	fi_LoadContainer(30,2084,1888);

	fi_LoadContainer(30,810,3274);
	fi_LoadContainer(30,818,3270);
	fi_LoadContainer(30,828,3270);
	fi_LoadContainer(30,838,3268);

	// mince guild
	fi_LoadContainer(30,2027,2295);
	fi_LoadContainer(30,2026,2295);
	fi_LoadContainer(30,2026,2303);
	fi_LoadContainer(30,2024,2303);
	fi_LoadContainer(30,2022,2303);
	fi_LoadContainer(30,2020,2304);
	fi_LoadContainer(30,2020,2303);
	fi_LoadContainer(30,2020,2302);
	fi_LoadContainer(30,2020,2301);
	fi_LoadContainer(30,2022,2301);*/

}

// ----------------------------------------------------------------- //
void fi_SaveAllContainers(void)
// ----------------------------------------------------------------- //

{
	int cpt, cpt2;

	for (cpt=0; cpt<kMaxGuildHalls; cpt++)
		for (cpt2=0; cpt2<kMaxGuildChests; cpt2++)
			if (guildHalls[cpt].chests[cpt2][0]!=0)
				fi_SaveContainer(guildHalls[cpt].chests[cpt2][1], guildHalls[cpt].chests[cpt2][2]);

	/*fi_SaveContainer(2294,1919);
	fi_SaveContainer(2294,1934);
	fi_SaveContainer(2285,1924);
	fi_SaveContainer(2285,1923);
	fi_SaveContainer(2285,1922);
	fi_SaveContainer(2285,1921);
	fi_SaveContainer(2285,1920);
	fi_SaveContainer(2285,1919);
	fi_SaveContainer(2283,1921);
	fi_SaveContainer(2283,1922);
	fi_SaveContainer(2283,1923);

	fi_SaveContainer(2108,1968);
	fi_SaveContainer(2099,1969);
	fi_SaveContainer(2106,1979);
	fi_SaveContainer(2089,1970);
	fi_SaveContainer(2092,1978);
	fi_SaveContainer(2097,1989);
	fi_SaveContainer(2081,1980);
	fi_SaveContainer(2084,1988);
	fi_SaveContainer(2074,1991);
	fi_SaveContainer(2069,1996);
	fi_SaveContainer(2061,1994);
	fi_SaveContainer(2060,2003);
	fi_SaveContainer(2085,2000);
	fi_SaveContainer(2084,2000);
	fi_SaveContainer(2083,2000);
	fi_SaveContainer(2069,2010);
	fi_SaveContainer(2068,2010);
	fi_SaveContainer(2068,2012);
	fi_SaveContainer(2062,2023);
	fi_SaveContainer(2061,2021);
	fi_SaveContainer(2059,2021);

	fi_SaveContainer(2980,2058);
	fi_SaveContainer(2987,2054);
	fi_SaveContainer(2989,2062);
	fi_SaveContainer(2961,2077);
	fi_SaveContainer(2972,2078);
	fi_SaveContainer(2029,2275);
	fi_SaveContainer(2029,2282);
	fi_SaveContainer(2014,2275);
	fi_SaveContainer(2017,2263);
	fi_SaveContainer(2017,2252);
	fi_SaveContainer(2636,1525);
	fi_SaveContainer(2642,1531);
	fi_SaveContainer(2059,1897);
	fi_SaveContainer(2068,1895);
	fi_SaveContainer(2076,1895);
	fi_SaveContainer(2073,1886);
	fi_SaveContainer(2086,1899);
	fi_SaveContainer(2084,1888);

	fi_SaveContainer(810,3274);
	fi_SaveContainer(818,3270);
	fi_SaveContainer(828,3270);
	fi_SaveContainer(838,3268);

	// mince guild
	fi_SaveContainer(2027,2295);
	fi_SaveContainer(2026,2295);
	fi_SaveContainer(2026,2303);
	fi_SaveContainer(2024,2303);
	fi_SaveContainer(2022,2303);
	fi_SaveContainer(2020,2304);
	fi_SaveContainer(2020,2303);
	fi_SaveContainer(2020,2302);
	fi_SaveContainer(2020,2301);
	fi_SaveContainer(2022,2301);*/

}

// ----------------------------------------------------------------- //
void fi_SaveContainer(int row, int col)
// ----------------------------------------------------------------- //

{
	FILE									*conStream;
	char									filename[kStringLength];
	char									filenumber[kStringLength];
	int                   j;
	//int                   i;
	int                   con;
	//UInt32								inValue;
	char									owner[kStringLength];

	con=containerMap[row][col];

	if (con==0)
		return;

	strcpy(filename,":Oberin Containers:Con ");

	tb_IntegerToString(row,filenumber);

	strcat(filename,filenumber);

	strcat(filename,"-");

	tb_IntegerToString(col,filenumber);

	strcat(filename,filenumber);

	conStream = fopen(filename, "w" );

	if (conStream != NULL)
	{
		for (j=0;j<kMaxInventorySize;j++)
		{
			fprintf( conStream, "%i", (int)container[con][j].itemID);
			fprintf( conStream, "%s", "\n");

			fprintf( conStream, "%i", (int)container[con][j].itemCount);
			fprintf( conStream, "%s", "\n");
		}

		fprintf( conStream, "%i", (int)containerID[con]);

		fprintf( conStream, "%s", "\n");

		fprintf( conStream, "%i", (int)containerLock[con]);
		fprintf( conStream, "%s", "\n");

		fprintf( conStream, "%i", (int)containerTrap[con]);
		fprintf( conStream, "%s", "\n");

		fprintf( conStream, "%i", (int)containerMagicLock[con]);
		fprintf( conStream, "%s", "\n");

		fprintf( conStream, "%i", (int)containerMagicTrap[con]);
		fprintf( conStream, "%s", "\n");

		strcpy(owner,containerGift[con]);

		if (strcmp(owner,"")==0)
			strcpy(owner,"All");

		//for (i=0;i<strlen(owner);i++)
		//	if (owner[i]==' ')
		//		owner[i]='*';

		fprintf( conStream, "%s", owner);

		fprintf( conStream, "%s", "\n");

		fclose( conStream );
	}

}

// ----------------------------------------------------------------- //
int fi_GetData(char filename[kStringLength], char searchFor[kStringLength], int defaultValue)
// ----------------------------------------------------------------- //

{
#ifndef _SERVER
	return (defaultValue);
#endif

#ifdef _SERVER
	FILE									*dataStream;
	char									inStr[kStringLength];
	char									rinStr[kStringLength];
	char									value[kStringLength];
	long									ivalue;
	int										i;
	int										j;

	fi_StartDebugTimer();
	dataStream = fopen(filename, "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetData");

			if (!fgets(rinStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return (defaultValue);
			}

			j=0;

			strcpy(inStr,"");

			for (i=0;i<strlen(rinStr);i++)
				if (rinStr[i]!=' ')
				{
					inStr[j]=rinStr[i];
					inStr[j+1]='\0';
					j++;
				}

			if (strncmp(searchFor,inStr,strlen(searchFor))==0)
			{
				j=0;

				for (i=strlen(searchFor);i<kStringLength;i++)
				{
					if (inStr[i]==';')
						break;

					value[j]=inStr[i];

					value[j+1]='\0';

					j++;
				}

				tb_StringToNum(value, &ivalue);

				fclose( dataStream );

				// --- begin range checking ---

				if (strcmp(searchFor,"SPEED=")==0)           {  if (ivalue<35) ivalue=35;  if (ivalue>1000) ivalue=1000;  }

				if (strcmp(searchFor,"POISONOUS=")==0)       {  if (ivalue<0) ivalue=0;  if (ivalue>100) ivalue=100;  }

				if (strcmp(searchFor,"CURSE=")==0)       		 {  if (ivalue<0) ivalue=0;  if (ivalue>100) ivalue=100;  }

				if (strcmp(searchFor,"HIT_POINTS=")==0)      {  if (ivalue<1) ivalue=1;  if (ivalue>100000) ivalue=100000;  }

				if (strcmp(searchFor,"STRENGTH=")==0)        {  if (ivalue<1) ivalue=1;  if (ivalue>100000) ivalue=100000;  }

				if (strcmp(searchFor,"INTELLIGENCE=")==0)    {  if (ivalue<1) ivalue=1;  if (ivalue>100000) ivalue=100000;  }

				if (strcmp(searchFor,"DEXTERITY=")==0)       {  if (ivalue<1) ivalue=1;  if (ivalue>100000) ivalue=100000;  }

				if (strcmp(searchFor,"MAGIC_POINTS=")==0)    {  if (ivalue<0) ivalue=0;  if (ivalue>100000) ivalue=100000;  }

				if (strcmp(searchFor,"MIN_GOLD=")==0)        {  if (ivalue<0) ivalue=0;  if (ivalue>100000) ivalue=100000;  }

				if (strcmp(searchFor,"MAX_GOLD=")==0)        {  if (ivalue<0) ivalue=0;  if (ivalue>100000) ivalue=100000;  }

				if (strcmp(searchFor,"MAGIC_RESIST=")==0)    {  if (ivalue<0) ivalue=0;  if (ivalue>100) ivalue=100;  }

				if (strcmp(searchFor,"TAMING_LEVEL=")==0)    {  if (ivalue<1) ivalue=1;  if (ivalue>101) ivalue=101;  }

				if (strcmp(searchFor,"ARMOR_RATING=")==0)    {  if (ivalue<1) ivalue=1;  if (ivalue>100000) ivalue=100000;  }

				if (strcmp(searchFor,"LOCK=")==0)            {  if (ivalue<0) ivalue=0;  if (ivalue>100) ivalue=100;  }

				if (strcmp(searchFor,"MIN_DAMAGE=")==0)      {  if (ivalue<0) ivalue=0;  if (ivalue>100000) ivalue=100000;  }

				if (strcmp(searchFor,"MAX_DAMAGE=")==0)      {  if (ivalue<0) ivalue=0;  if (ivalue>100000) ivalue=100000;  }

				if (strcmp(searchFor,"LEVEL=")==0)			 {	if (ivalue<1) ivalue=1;  if (ivalue>100)  ivalue=100;   }

				// --- end range checking ---

				return ((int)ivalue);
			}
		}

		fclose( dataStream );

		return (defaultValue);
	}

	return (defaultValue);

#endif
}

// ----------------------------------------------------------------- //
void fi_GetSpellList(char filename[kStringLength], int c)
// ----------------------------------------------------------------- //

{
#ifndef _SERVER
	return;
#endif

#ifdef _SERVER
	FILE									*dataStream;
	char									inStr[kStringLength];
	char									rinStr[kStringLength];
	//char									value[kStringLength];
	long									ivalue;
	int										i;
	int										j;
	int										spell=0;

	fi_StartDebugTimer();
	dataStream = fopen(filename, "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetSpellList");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			j=0;

			if (strncmp(inStr,"#spell(", 7)==0 && spell<kMaxCreatureSpells)
			{
				strcpy(rinStr, "");
				
				j=7;
				i=0;

				while (inStr[j]!=',')
				{
					rinStr[i++]=inStr[j++];
					rinStr[i]='\0';
				}
				
				j++;
				
				tb_StringToNum(rinStr, &ivalue);
								
				creatureInfo[c].spellList[spell][0]=ivalue;
				
				strcpy(rinStr, "");

				i=0;

				while (inStr[j]!=',')
				{
					rinStr[i++]=inStr[j++];
					rinStr[i]='\0';
				}
				
				j++;
				
				tb_StringToNum(rinStr, &ivalue);
				creatureInfo[c].spellList[spell][1]=ivalue;
				
				strcpy(rinStr, "");
				
				i=0;

				while (inStr[j]!=')')
				{
					rinStr[i++]=inStr[j++];
					rinStr[i]='\0';
				}
				
				tb_StringToNum(rinStr, &ivalue);
				creatureInfo[c].spellList[spell][2]=ivalue;
				
				spell++;
			}
		}
	}

	fclose (dataStream);

#endif
}

// ----------------------------------------------------------------- //
TBoolean fi_GetBoolean(char filename[kStringLength], char searchFor[kStringLength], TBoolean defaultValue)
// ----------------------------------------------------------------- //

{
	FILE									*dataStream;
	char									inStr[kStringLength];
	char									rinStr[kStringLength];
	char									value[kStringLength];
	int										i;
	int										j;

	fi_StartDebugTimer();
	dataStream = fopen(filename, "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetBoolean");

			if (!fgets(rinStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return (defaultValue);
			}

			j=0;

			strcpy(inStr,"");

			for (i=0;i<strlen(rinStr);i++)
				if (rinStr[i]!=' ')
				{
					inStr[j]=rinStr[i];
					inStr[j+1]='\0';
					j++;
				}

			if (strncmp(searchFor,inStr,strlen(searchFor))==0)
			{
				j=0;

				for (i=strlen(searchFor);i<kStringLength;i++)
				{
					if (inStr[i]==';')
						break;

					value[j]=inStr[i];

					value[j+1]='\0';

					j++;
				}

				fclose( dataStream );

				if (strcmp(value,"TRUE")==0) return (true);

				if (strcmp(value,"FALSE")==0) return (false);

				return (defaultValue);
			}
		}

		fclose( dataStream );

		return (defaultValue);
	}

	return (defaultValue);

}

// ----------------------------------------------------------------- //
void fi_GetCreatureTalkScript(char filename[kStringLength], int theCreature)
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	int										k;
	//char									condition[10][kStringLength];
	int										scriptLine;
	int										start;

	fi_StartDebugTimer();
	dataStream = fopen(filename, "r" );

	if (dataStream != NULL)
	{
		scriptLine=0;

		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetCreatureTalkScript");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#if",inStr,3)==0)
			{
				creatureInfo[theCreature].talkScript[scriptLine].language=kEnglish;

				if (strstr(inStr,"ENGLISH"))
					creatureInfo[theCreature].talkScript[scriptLine].language=kEnglish;

				// --- check states ---
				if (strstr(inStr,"CHASING"))
					creatureInfo[theCreature].talkScript[scriptLine].state=kChasingTalk;

				if (strstr(inStr,"ATTACKING"))
					creatureInfo[theCreature].talkScript[scriptLine].state=kAttackingTalk;
				
				if (strstr(inStr,"MORPHED"))
					creatureInfo[theCreature].talkScript[scriptLine].state=kMorphedTalk;

				if (strstr(inStr,"RETREATING"))
					creatureInfo[theCreature].talkScript[scriptLine].state=kRetreatingTalk;

				if (strstr(inStr,"KILLING"))
					creatureInfo[theCreature].talkScript[scriptLine].state=kKillingTalk;

				if (strstr(inStr,"HIDING"))
					creatureInfo[theCreature].talkScript[scriptLine].state=kHidingTalk;

				if (strstr(inStr,"JUST_HID"))
					creatureInfo[theCreature].talkScript[scriptLine].state=kJustHidTalk;

				// --- check states ---
				if (strstr(inStr,"HUMAN"))
					creatureInfo[theCreature].talkScript[scriptLine].target=kHumanTarget;

				if (strstr(inStr,"PET"))
					creatureInfo[theCreature].talkScript[scriptLine].target=kPetTarget;

				if (strstr(inStr,"CLERIC"))
					creatureInfo[theCreature].talkScript[scriptLine].target=kClericTarget;

				if (strstr(inStr,"DRUID"))
					creatureInfo[theCreature].talkScript[scriptLine].target=kDruidTarget;

				if (strstr(inStr,"FIGHTER"))
					creatureInfo[theCreature].talkScript[scriptLine].target=kFighterTarget;

				if (strstr(inStr,"RANGER"))
					creatureInfo[theCreature].talkScript[scriptLine].target=kRangerTarget;

				if (strstr(inStr,"ROGUE"))
					creatureInfo[theCreature].talkScript[scriptLine].target=kRogueTarget;

				if (strstr(inStr,"WIZARD"))
					creatureInfo[theCreature].talkScript[scriptLine].target=kWizardTarget;

				if (strstr(inStr,"MORPHED"))
					creatureInfo[theCreature].talkScript[scriptLine].target=kMorphedTarget;
				
				
				start=0;

				for (i=0;i<strlen(inStr)-3;i++)
					if ((inStr[i]=='s') && (inStr[i+1]=='a') && (inStr[i+2]=='y') && (inStr[i+3]=='('))
					{
						start=i+4;
						break;
					}

				if ((start>0) && (start<strlen(inStr)))
				{
					k=0;

					for (j=start;j<strlen(inStr);j++)
					{
						if (inStr[j]==')')
							break;

						creatureInfo[theCreature].talkScript[scriptLine].say[k]=inStr[j];

						creatureInfo[theCreature].talkScript[scriptLine].say[k+1]='\0';

						k++;

						if (k>=kStringLength)
							break;
					}

					scriptLine++;
				}
				else
				{
					creatureInfo[theCreature].talkScript[scriptLine].language=0;
					creatureInfo[theCreature].talkScript[scriptLine].state=0;
				}

				if (scriptLine>=kMaxTalkScripts)
				{
					fclose( dataStream );
					return;
				}
			}
		}

		fclose( dataStream );

		return;
	}

	return;

#endif
}

// ----------------------------------------------------------------- //
void fi_GetCreatureSpawns(void)
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	char									numStr4[kStringLength];
	char									numStr5[kStringLength];
	char									numStr6[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	long									num4;
	long									num5;
	long									num6;
	int										theCreature;

	fi_StartDebugTimer();
	dataStream = fopen(":Creature Settings:Spawns", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetCreatureSpawns");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#spawn",inStr,6)==0)
			{
				theCreature=0;

				if (strstr(inStr,"GAPER")) theCreature=kGazer;
				else if (strstr(inStr,"AIR ELEMENTAL")) theCreature=kAirElemental;
				else if (strstr(inStr,"WATER ELEMENTAL")) theCreature=kWaterElemental;
				else if (strstr(inStr,"FIRE ELEMENTAL")) theCreature=kFireElemental;
				else if (strstr(inStr,"POISON ELEMENTAL")) theCreature=kPoisonElemental;
				//Dave
				else if (strstr(inStr,"EARTH ELEMENTAL")) theCreature=kEarthElemental;
				else if (strstr(inStr,"SEA TURTLE")) theCreature=kSeaTurtle;
				else if (strstr(inStr,"SKELETON")) theCreature=kSkeleton;
				else if (strstr(inStr,"SKELETON ARCHER")) theCreature=kSkeletonArcher;
				else if (strstr(inStr,"SLUDGE BEAST")) theCreature=kSlug;
				else if (strstr(inStr,"HELL HOUND")) theCreature=kHellHound;
				else if (strstr(inStr,"GREY WOLF")) theCreature=kGreyWolf;
				else if (strstr(inStr,"BROWN WOLF")) theCreature=kBrownWolf;
				else if (strstr(inStr,"BLACK WOLF")) theCreature=kBlackWolf;
				else if (strstr(inStr,"GREY DOG")) theCreature=kGreyDog;
				else if (strstr(inStr,"BROWN DOG")) theCreature=kBrownDog;
				else if (strstr(inStr,"BLACK DOG")) theCreature=kBlackDog;
				else if (strstr(inStr,"RED DRAGON")) theCreature=kRedDragon;
				else if (strstr(inStr,"BLACK DRAGON")) theCreature=kBlackDragon;
				else if (strstr(inStr,"ICE DRAGON")) theCreature=kIceDragon;
				else if (strstr(inStr,"FOREST DRAGON")) theCreature=kForestDragon;
				else if (strstr(inStr,"BRACKEN")) theCreature=kReaper;
				else if (strstr(inStr,"SHEEP")) theCreature=kSheep;
				else if (strstr(inStr,"GIANT SCORPION")) theCreature=kScorpion;
				else if (strstr(inStr,"HARPIE")) theCreature=kHarpie;
				else if (strstr(inStr,"BONE MAGE")) theCreature=kBoneMage;
				else if (strstr(inStr,"GIANT RAT")) theCreature=kRat;
				else if (strstr(inStr,"LIZARD MAN")) theCreature=kLizardMan;
				else if (strstr(inStr,"GIANT BAT")) theCreature=kBat;
				else if (strstr(inStr,"GIANT SNAKE")) theCreature=kSnake;
				else if (strstr(inStr,"GIANT SPIDER")) theCreature=kSpider;
				else if (strstr(inStr,"FROST SPIDER")) theCreature=kFrostSpider;
				else if (strstr(inStr,"LAVA SPIDER")) theCreature=kLavaSpider;
				else if (strstr(inStr,"SAND SPIDER")) theCreature=kSandSpider;
				else if (strstr(inStr,"GIANT SNAIL")) theCreature=kSnail;
				else if (strstr(inStr,"GIANT ANT")) theCreature=kAnt;
				else if (strstr(inStr,"FOREST TROLL")) theCreature=kForestTroll;
				else if (strstr(inStr,"TROLL")) theCreature=kTroll;
				else if (strstr(inStr,"ELDER CENTAUR")) theCreature=kElderCentaur;
				else if (strstr(inStr,"CENTAUR")) theCreature=kCentaur;
				else if (strstr(inStr,"SEA SERPENT")) theCreature=kSerpent;
				else if (strstr(inStr,"ZOMBIE")) theCreature=kZombie;
				else if (strstr(inStr,"RUST BEAST")) theCreature=kRustBeast;
				else if (strstr(inStr,"POISON BEAST")) theCreature=kPoisonBeast;
				else if (strstr(inStr,"HILL GIANT")) theCreature=kHillGiant;
				else if (strstr(inStr,"FROST GIANT")) theCreature=kFrostGiant;
				else if (strstr(inStr,"GIANT")) theCreature=kGiant;
				else if (strstr(inStr,"ABYSS DRAGON")) theCreature=kPinkDragon;
				else if (strstr(inStr,"QUEEN ANT")) theCreature=kQueenAnt;
				else if (strstr(inStr,"WORKER ANT")) theCreature=kWorkAnt;
				else if (strstr(inStr,"WARRIOR ANT")) theCreature=kWarriorAnt;
				else if (strstr(inStr,"FORAGER ANT")) theCreature=kForagerAnt;
				else if (strstr(inStr,"BLINK HOUND")) theCreature=kBlinkHound;
				else if (strstr(inStr,"DWARF")) theCreature=kDwarf;
				else if (strstr(inStr,"RED DRACO")) theCreature=kRedDraco;
				else if (strstr(inStr,"BLACK DRACO")) theCreature=kBlackDraco;
				else if (strstr(inStr,"FOREST DRACO")) theCreature=kForestDraco;
				else if (strstr(inStr,"ICE DRACO")) theCreature=kIceDraco;
				else if (strstr(inStr,"GOOEY")) theCreature=kCube;
				else if (strstr(inStr,"TUSKER")) theCreature=kTusker;
				else if (strstr(inStr,"ANCIENT BLACK DRAGON")) theCreature=kBlackDragonAncient;
				else if (strstr(inStr,"ANCIENT ICE DRAGON")) theCreature=kIceDragonAncient;
				else if (strstr(inStr,"ANCIENT RED DRAGON")) theCreature=kRedDragonAncient;
				else if (strstr(inStr,"ANCIENT FOREST DRAGON")) theCreature=kForestDragonAncient;
				else if (strstr(inStr,"LICH")) theCreature=kLich;
				else if (strstr(inStr,"LION"))	theCreature=kLion;
				else if (strstr(inStr,"STONE GOLEM")) theCreature=kGolem;
				//Dave
				else if (strstr(inStr,"ICE GOLEM")) theCreature=kIceGolem;
				else if (strstr(inStr,"MAGMA GOLEM")) theCreature=kMagmaGolem;
				else if (strstr(inStr,"SAND GOLEM")) theCreature=kSandGolem;
				else if (strstr(inStr,"PIXIE")) theCreature=kPixie;
				else if (strstr(inStr,"SAND DRAGON")) theCreature=kSandDragon;
				else if (strstr(inStr,"SAND DRACO")) theCreature=kSandDraco;
				else if (strstr(inStr,"PHASE SERPENT")) theCreature=kPhaseSerpent;
				else if (strstr(inStr,"BLACK BEAR")) theCreature=kBlackBear;
				else if (strstr(inStr,"BROWN BEAR")) theCreature=kBrownBear;
				else if (strstr(inStr,"SHARK")) theCreature=kShark;
				else if (strstr(inStr,"LIZARD SHAMAN")) theCreature=kLizardManShaman;
				else if (strstr(inStr,"HELL MAGE")) theCreature=kHellMage;
				else if (strstr(inStr,"WRATH")) theCreature=kWrath;
				//else if (strstr(inStr,"SNAPPING TURTLE")) theCreature=kSnapTurtle;

				if (theCreature==0)
					continue;

				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]==',')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");
				strcpy(numStr3,"");
				strcpy(numStr4,"");
				strcpy(numStr5,"");
				strcpy(numStr6,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr3[j]=inStr[i];

						numStr3[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr4[j]=inStr[i];

						numStr4[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr5[j]=inStr[i];

						numStr5[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr6[j]=inStr[i];

						numStr6[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);
					tb_StringToNum(numStr3, &num3);
					tb_StringToNum(numStr4, &num4);
					tb_StringToNum(numStr5, &num5);
					tb_StringToNum(numStr6, &num6);

					if (num3<num1) continue;

					if (num4<num2) continue;

					if (num5==0) continue;

					if (num1<0) continue;

					if (num1>=kMapWidth) continue;

					if (num2<0) continue;

					if (num2>=kMapWidth) continue;

					if (num3<0) continue;

					if (num3>=kMapWidth) continue;

					if (num4<0) continue;

					if (num4>=kMapWidth) continue;

					for (i=1;i<kMaxSpawnRegions;i++)
						if (spawnRegion[i].creatureID==0)
						{
							spawnRegion[i].creatureID=theCreature;
							tb_SetRect(&spawnRegion[i].theRegion,num1,num2,num3,num4);
							spawnRegion[i].currentPopulation=0;
							spawnRegion[i].maxPopulation=num5;
							spawnRegion[i].range=num6;
							break;
						}

				}

			}
		}

		fclose( dataStream );

		return;
	}

	return;

#endif
}

// ----------------------------------------------------------------- //
void fi_GetCreatureInventory(char filename[kStringLength], int theCreature)
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	TBoolean							goodData;

	fi_StartDebugTimer();
	dataStream = fopen(filename, "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetCreatureInventory");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#inventory",inStr,10)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");
				strcpy(numStr3,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr3[j]=inStr[i];

						numStr3[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);
					tb_StringToNum(numStr3, &num3);
					goodData=true;

					if ((num1<0) || (num1>=kMaxItemsUsed))
						goodData=false;

					if (num2>num3)
						num2=0;

					if (num3==0)
						goodData=false;

					if (num3>1000)
						num3=1000;

					if (goodData)
						cr_AddToCreatureControlInventory(theCreature, num1, num2, num3);

				}

			}
		}

		fclose( dataStream );

		return;
	}

	return;

#endif
}

// ----------------------------------------------------------------- //
void fi_GetWeaponModifier(char filename[kStringLength], int theCreature)
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	long									num1;
	TBoolean							foundModifier;
	int										weapon;

	fi_StartDebugTimer();
	dataStream = fopen(filename, "r" );

	if (dataStream != NULL)
	{

		for (j=0;j<kMaxWeaponModifiers;j++)
		{
			creatureInfo[theCreature].weaponID[j]								= 0;
			creatureInfo[theCreature].weaponModifier[j]					= 1.0;
		}

		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetWeaponModifier");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			foundModifier=false;

			if (strncmp("#katana",inStr,strlen("#katana"))==0)									{	weapon=kKatana;	foundModifier=true;	}

			if (strncmp("#longsword",inStr,strlen("#longsword"))==0)						{	weapon=kLongSword;	foundModifier=true;	}

			if (strncmp("#flamingsword",inStr,strlen("#flamingsword"))==0)			{	weapon=kFireSword;	foundModifier=true;	}

			if (strncmp("#broadsword",inStr,strlen("#broadsword"))==0)					{	weapon=kBroadSword;	foundModifier=true;	}

			if (strncmp("#scimitar",inStr,strlen("#scimitar"))==0)							{	weapon=kScimitar;	foundModifier=true;	}

			if (strncmp("#holydagger",inStr,strlen("#holydagger"))==0)					{	weapon=kHolyDagger;	foundModifier=true;	}

			if (strncmp("#blooddagger",inStr,strlen("#blooddagger"))==0)				{	weapon=kBloodDagger;	foundModifier=true;	}

			if (strncmp("#knife",inStr,strlen("#knife"))==0)										{	weapon=kKnife;	foundModifier=true;	}

			if (strncmp("#stave",inStr,strlen("#stave"))==0)										{	weapon=kStave;	foundModifier=true;	}

			if (strncmp("#sickle",inStr,strlen("#sickle"))==0)									{	weapon=kSickle;	foundModifier=true;	}

			if (strncmp("#club",inStr,strlen("#club"))==0)											{	weapon=kClub;	foundModifier=true;	}

			if (strncmp("#flail",inStr,strlen("#flail"))==0)										{	weapon=kFlail;	foundModifier=true;	}

			if (strncmp("#shortbow",inStr,strlen("#shortbow"))==0)							{	weapon=kShortBow;	foundModifier=true;	}

			if (strncmp("#longbow",inStr,strlen("#longbow"))==0)								{	weapon=kLongBow;	foundModifier=true;	}

			if (strncmp("#battleaxe",inStr,strlen("#battleaxe"))==0)						{	weapon=kBattleAxe;	foundModifier=true;	}

			if (strncmp("#frostaxe",inStr,strlen("#frostaxe"))==0)							{	weapon=kFrostAxe;	foundModifier=true;	}

			if (strncmp("#handaxe",inStr,strlen("#handaxe"))==0)								{	weapon=kHandAxe;	foundModifier=true;	}

			if (strncmp("#dwarvenhandaxe",inStr,strlen("#dwarvenhandaxe"))==0)	{	weapon=kDwarvenHandAxe;	foundModifier=true;	}

			if (strncmp("#amberaxe",inStr,strlen("#amberaxe"))==0)							{	weapon=kAmberAxe;	foundModifier=true;	}

			if (strncmp("#glaive",inStr,strlen("#glaive"))==0)									{	weapon=kGlaive;	foundModifier=true;	}

			if (strncmp("#neptunestrident",inStr,strlen("#neptunestrident"))==0){	weapon=kNeptunesTrident;	foundModifier=true;	}

			if (strncmp("#spear",inStr,strlen("#spear"))==0)										{	weapon=kSpear;	foundModifier=true;	}

			if (strncmp("#warhammer",inStr,strlen("#warhammer"))==0)						{	weapon=kWarHammer;	foundModifier=true;	}

			if (strncmp("#mace",inStr,strlen("#mace"))==0)											{	weapon=kMace;	foundModifier=true;	}

			if (strncmp("#dagger",inStr,strlen("#dagger"))==0)											{	weapon=kKnife;	foundModifier=true;	}
			
			if (strncmp("#pokingstick", inStr, strlen("#pokingstick"))==0)				{	weapon=kGMStaff;	foundModifier=true;	}
			//Dave			
			if (strncmp("#runesword", inStr, strlen("#runesword"))==0)				{	weapon=kRuneSword;	foundModifier=true;	}
			
			if (strncmp("#bigjackkatana", inStr, strlen("#bigjackkatana"))==0)			{	weapon=kBigJackKatana;	foundModifier=true;	}
			
			if (strncmp("#scepter",inStr,strlen("#scepter"))==0)							{	weapon=kRoyalSceptre;	foundModifier=true;	}

			
			
			if (foundModifier)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					for (j=0;j<kMaxWeaponModifiers;j++)
						if (creatureInfo[theCreature].weaponID[j]==0)
						{
							creatureInfo[theCreature].weaponID[j]								= weapon;
							creatureInfo[theCreature].weaponModifier[j]					= (float)num1/100;
							break;
						}

				}

			}
		}

		fclose( dataStream );

		return;
	}

	return;

#endif
}

// ----------------------------------------------------------------- //
void fi_GetWeaponSettings(void)
// ----------------------------------------------------------------- //

{
#ifdef _SERVER
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	TBoolean							foundModifier;
	int										weapon;

	fi_StartDebugTimer();
	dataStream = fopen("Weapon Settings", "r" );

	if (dataStream != NULL)
	{

		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetWeaponSettings");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			foundModifier=false;

			if (strncmp("#katana",inStr,strlen("#katana"))==0)									{	weapon=kKatana;	foundModifier=true;	}

			if (strncmp("#longsword",inStr,strlen("#longsword"))==0)						{	weapon=kLongSword;	foundModifier=true;	}

			if (strncmp("#flamingsword",inStr,strlen("#flamingsword"))==0)			{	weapon=kFireSword;	foundModifier=true;	}

			if (strncmp("#broadsword",inStr,strlen("#broadsword"))==0)					{	weapon=kBroadSword;	foundModifier=true;	}

			if (strncmp("#scimitar",inStr,strlen("#scimitar"))==0)							{	weapon=kScimitar;	foundModifier=true;	}

			if (strncmp("#holydagger",inStr,strlen("#holydagger"))==0)					{	weapon=kHolyDagger;	foundModifier=true;	}

			if (strncmp("#blooddagger",inStr,strlen("#blooddagger"))==0)				{	weapon=kBloodDagger;	foundModifier=true;	}

			if (strncmp("#knife",inStr,strlen("#knife"))==0)										{	weapon=kKnife;	foundModifier=true;	}

			if (strncmp("#stave",inStr,strlen("#stave"))==0)										{	weapon=kStave;	foundModifier=true;	}

			if (strncmp("#sickle",inStr,strlen("#sickle"))==0)									{	weapon=kSickle;	foundModifier=true;	}

			if (strncmp("#club",inStr,strlen("#club"))==0)											{	weapon=kClub;	foundModifier=true;	}

			if (strncmp("#flail",inStr,strlen("#flail"))==0)										{	weapon=kFlail;	foundModifier=true;	}

			if (strncmp("#shortbow",inStr,strlen("#shortbow"))==0)							{	weapon=kShortBow;	foundModifier=true;	}

			if (strncmp("#longbow",inStr,strlen("#longbow"))==0)								{	weapon=kLongBow;	foundModifier=true;	}

			if (strncmp("#battleaxe",inStr,strlen("#battleaxe"))==0)						{	weapon=kBattleAxe;	foundModifier=true;	}

			if (strncmp("#frostaxe",inStr,strlen("#frostaxe"))==0)							{	weapon=kFrostAxe;	foundModifier=true;	}

			if (strncmp("#handaxe",inStr,strlen("#handaxe"))==0)								{	weapon=kHandAxe;	foundModifier=true;	}

			if (strncmp("#dwarvenhandaxe",inStr,strlen("#dwarvenhandaxe"))==0)	{	weapon=kDwarvenHandAxe;	foundModifier=true;	}

			if (strncmp("#amberaxe",inStr,strlen("#amberaxe"))==0)							{	weapon=kAmberAxe;	foundModifier=true;	}

			if (strncmp("#glaive",inStr,strlen("#glaive"))==0)									{	weapon=kGlaive;	foundModifier=true;	}

			if (strncmp("#neptunestrident",inStr,strlen("#neptunestrident"))==0){	weapon=kNeptunesTrident;	foundModifier=true;	}

			if (strncmp("#spear",inStr,strlen("#spear"))==0)										{	weapon=kSpear;	foundModifier=true;	}

			if (strncmp("#warhammer",inStr,strlen("#warhammer"))==0)						{	weapon=kWarHammer;	foundModifier=true;	}

			if (strncmp("#mace",inStr,strlen("#mace"))==0)											{	weapon=kMace;	foundModifier=true;	}

			if (strncmp("#dagger",inStr,strlen("#dagger"))==0)											{	weapon=kKnife;	foundModifier=true;	}
			
			if (strncmp("#pokingstick", inStr, strlen("#pokingstick"))==0)				{	weapon=kGMStaff;	foundModifier=true;	}
			//Dave			
			if (strncmp("#runesword", inStr, strlen("#runesword"))==0)				{	weapon=kRuneSword;	foundModifier=true;	}
			
			if (strncmp("#bigjackkatana", inStr, strlen("#bigjackkatana"))==0)			{	weapon=kBigJackKatana;	foundModifier=true;	}
			
			if (strncmp("#scepter",inStr,strlen("#scepter"))==0)							{	weapon=kRoyalSceptre;	foundModifier=true;	}

			if (foundModifier)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");
				strcpy(numStr3,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr3[j]=inStr[i];

						numStr3[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);
					tb_StringToNum(numStr3, &num3);

					itemList[weapon].weaponSpeed							= num1;
					itemList[weapon].weaponDamage							= num2;
					itemList[weapon].durability								= num3;

				}

			}
		}

		fclose( dataStream );
	}

#endif

	it_CreateMagicWeapon(kKatanaSpeed,                kKatana, 1, kSpeedAttribute);
	
	it_CreateMagicWeapon(kKatanaPower,                kKatana, 1, kPowerAttribute);
	
	it_CreateMagicWeapon(kKatanaDurability,           kKatana, 1, kDurabilityAttribute);
	
	it_Ingredients(kKatanaPower, kBlacksmithing, 11, 1, kKatana, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kKatanaDurability, kBlacksmithing, 10, 1, kKatana, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kKatanaSpeed, kBlacksmithing, 12, 1, kKatana, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kKatanaSilver,               kKatana, 1, kSilverAttribute);

	it_CreateMagicWeapon(kKatanaPoison,               kKatana, 0, kPoisonAttribute);

	it_Ingredients	(kKatanaPoison, kPoisoning, 5, 1, kKatana, 1, kPoison, 0,0,0,0,0,0,1);

	it_CreateMagicWeapon(kLongSwordSpeed,                kLongSword, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kLongSwordPower,                kLongSword, 1, kPowerAttribute);

	it_CreateMagicWeapon(kLongSwordDurability,           kLongSword, 1, kDurabilityAttribute);

	it_Ingredients(kLongSwordPower, kBlacksmithing, 11, 1, kLongSword, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kLongSwordDurability, kBlacksmithing, 10, 1, kLongSword, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kLongSwordSpeed, kBlacksmithing, 12, 1, kLongSword, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kLongSwordSilver,               kLongSword, 1, kSilverAttribute);

	it_CreateMagicWeapon(kLongSwordPoison,               kLongSword, 0, kPoisonAttribute);

	it_Ingredients	(kLongSwordPoison, kPoisoning, 7, 1, kLongSword, 1, kPoison, 0,0,0,0,0,0,1);

	it_CreateMagicWeapon(kBroadSwordSpeed,                kBroadSword, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kBroadSwordPower,                kBroadSword, 1, kPowerAttribute);

	it_CreateMagicWeapon(kBroadSwordDurability,           kBroadSword, 1, kDurabilityAttribute);

	it_Ingredients(kBroadSwordPower, kBlacksmithing, 11, 1, kBroadSword, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kBroadSwordDurability, kBlacksmithing, 10, 1, kBroadSword, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kBroadSwordSpeed, kBlacksmithing, 12, 1, kBroadSword, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kBroadSwordSilver,               kBroadSword, 1, kSilverAttribute);

	it_CreateMagicWeapon(kBroadSwordPoison,               kBroadSword, 0, kPoisonAttribute);

	it_Ingredients	(kBroadSwordPoison, kPoisoning, 2, 1, kBroadSword, 1, kPoison, 0,0,0,0,0,0,1);

	it_CreateMagicWeapon(kScimitarSpeed,                kScimitar, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kScimitarPower,                kScimitar, 1, kPowerAttribute);

	it_CreateMagicWeapon(kScimitarDurability,           kScimitar, 1, kDurabilityAttribute);

	it_Ingredients(kScimitarPower, kBlacksmithing, 11, 1, kScimitar, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kScimitarDurability, kBlacksmithing, 10, 1, kScimitar, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kScimitarSpeed, kBlacksmithing, 12, 1, kScimitar, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kScimitarSilver,               kScimitar, 1, kSilverAttribute);

	it_CreateMagicWeapon(kScimitarPoison,               kScimitar, 0, kPoisonAttribute);

	it_Ingredients	(kScimitarPoison, kPoisoning, 5, 1, kScimitar, 1, kPoison, 0,0,0,0,0,0,1);

	it_CreateMagicWeapon(kKnifeSpeed,                kKnife, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kKnifePower,                kKnife, 1, kPowerAttribute);

	it_CreateMagicWeapon(kKnifeDurability,           kKnife, 1, kDurabilityAttribute);

	it_Ingredients(kKnifePower, kBlacksmithing, 11, 1, kKnife, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kKnifeDurability, kBlacksmithing, 10, 1, kKnife, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kKnifeSpeed, kBlacksmithing, 12, 1, kKnife, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kKnifeSilver,               kKnife, 1, kSilverAttribute);

	it_CreateMagicWeapon(kKnifePoison,               kKnife, 0, kPoisonAttribute);

	it_Ingredients	(kKnifePoison, kPoisoning, 1, 1, kKnife, 1, kPoison, 0,0,0,0,0,0,1);

	it_CreateMagicWeapon(kStaveSpeed,                kStave, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kStavePower,                kStave, 1, kPowerAttribute);
	
	it_CreateMagicWeapon(kStaveDurability,           kStave, 1, kDurabilityAttribute);
	//Dave 09-05-2011
	it_Ingredients(kStavePower, kWoodworking, 10, 1, kStave, 5, kBoard, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kStaveDurability, kWoodworking, 9, 1, kStave, 5, kBoard, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kStaveSpeed, kWoodworking, 11, 1, kStave, 5, kBoard, 1, kEssenceOfSpeed, 0,0,0,0,1);
	
	it_CreateMagicWeapon(kSickleSpeed,                kSickle, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kSicklePower,                kSickle, 1, kPowerAttribute);

	it_CreateMagicWeapon(kSickleDurability,           kSickle, 1, kDurabilityAttribute);

	it_Ingredients(kSicklePower, kBlacksmithing, 11, 1, kSickle, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kSickleDurability, kBlacksmithing, 10, 1, kSickle, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kSickleSpeed, kBlacksmithing, 12, 1, kSickle, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kSickleSilver,               kSickle, 1, kSilverAttribute);

	it_CreateMagicWeapon(kSicklePoison,               kSickle, 0, kPoisonAttribute);

	it_Ingredients	(kSicklePoison, kPoisoning, 6, 1, kSickle, 1, kPoison, 0,0,0,0,0,0,1);

	it_CreateMagicWeapon(kClubSpeed,                kClub, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kClubPower,                kClub, 1, kPowerAttribute);

	it_CreateMagicWeapon(kClubDurability,           kClub, 1, kDurabilityAttribute);
 	//Dave 09-05-2011
	it_Ingredients(kClubPower, kWoodworking, 10, 1, kClub, 5, kBoard, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kClubDurability, kWoodworking, 9, 1, kClub, 5, kBoard, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kClubSpeed, kWoodworking, 11, 1, kClub, 5, kBoard, 1, kEssenceOfSpeed, 0,0,0,0,1);
	
	it_CreateMagicWeapon(kFlailSpeed,                kFlail, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kFlailPower,                kFlail, 1, kPowerAttribute);

	it_CreateMagicWeapon(kFlailDurability,           kFlail, 1, kDurabilityAttribute);

	it_Ingredients(kFlailPower, kBlacksmithing, 11, 1, kFlail, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kFlailDurability, kBlacksmithing, 10, 1, kFlail, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kFlailSpeed, kBlacksmithing, 12, 1, kFlail, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kShortBowSpeed,                kShortBow, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kShortBowPower,                kShortBow, 1, kPowerAttribute);

	it_CreateMagicWeapon(kShortBowDurability,           kShortBow, 1, kDurabilityAttribute);

	it_Ingredients(kShortBowPower, kWoodworking, 11, 1, kShortBow, 5, kBoard, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kShortBowDurability, kWoodworking, 10, 1, kShortBow, 5, kBoard, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kShortBowSpeed, kWoodworking, 12, 1, kShortBow, 5, kBoard, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kLongBowSpeed,                kLongBow, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kLongBowPower,                kLongBow, 1, kPowerAttribute);

	it_CreateMagicWeapon(kLongBowDurability,           kLongBow, 1, kDurabilityAttribute);

	it_Ingredients(kLongBowPower, kWoodworking, 11, 1, kLongBow, 5, kBoard, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kLongBowDurability, kWoodworking, 10, 1, kLongBow, 5, kBoard, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kLongBowSpeed, kWoodworking, 12, 1, kLongBow, 5, kBoard, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kBattleAxeSpeed,                kBattleAxe, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kBattleAxePower,                kBattleAxe, 1, kPowerAttribute);

	it_CreateMagicWeapon(kBattleAxeDurability,           kBattleAxe, 1, kDurabilityAttribute);

	it_Ingredients(kBattleAxePower, kBlacksmithing, 11, 1, kBattleAxe, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kBattleAxeDurability, kBlacksmithing, 10, 1, kBattleAxe, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kBattleAxeSpeed, kBlacksmithing, 12, 1, kBattleAxe, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);

	
	it_CreateMagicWeapon(kBattleAxeSilver,               kBattleAxe, 1, kSilverAttribute);

	it_CreateMagicWeapon(kBattleAxePoison,               kBattleAxe, 0, kPoisonAttribute);

	it_Ingredients	(kBattleAxePoison, kPoisoning, 8, 1, kBattleAxe, 1, kPoison, 0,0,0,0,0,0,1);

	it_CreateMagicWeapon(kHandAxeSpeed,                kHandAxe, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kHandAxePower,                kHandAxe, 1, kPowerAttribute);

	it_CreateMagicWeapon(kHandAxeDurability,           kHandAxe, 1, kDurabilityAttribute);

	it_CreateMagicWeapon(kHandAxeSilver,               kHandAxe, 1, kSilverAttribute);

	it_CreateMagicWeapon(kHandAxePoison,               kHandAxe, 0, kPoisonAttribute);

	it_Ingredients	(kHandAxePoison, kPoisoning, 1, 1, kHandAxe, 1, kPoison, 0,0,0,0,0,0,1);

	it_CreateMagicWeapon(kGlaiveSpeed,                kGlaive, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kGlaivePower,                kGlaive, 1, kPowerAttribute);

	it_CreateMagicWeapon(kGlaiveDurability,           kGlaive, 1, kDurabilityAttribute);

	it_Ingredients(kGlaivePower, kBlacksmithing, 11, 1, kGlaive, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kGlaiveDurability, kBlacksmithing, 10, 1, kGlaive, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kGlaiveSpeed, kBlacksmithing, 12, 1, kGlaive, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kGlaiveSilver,               kGlaive, 1, kSilverAttribute);

	it_CreateMagicWeapon(kGlaivePoison,               kGlaive, 0, kPoisonAttribute);

	it_Ingredients	(kGlaivePoison, kPoisoning, 8, 1, kGlaive, 1, kPoison, 0,0,0,0,0,0,1);

	it_CreateMagicWeapon(kSpearSpeed,                kSpear, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kSpearPower,                kSpear, 1, kPowerAttribute);

	it_CreateMagicWeapon(kSpearDurability,           kSpear, 1, kDurabilityAttribute);

	it_Ingredients(kSpearPower, kBlacksmithing, 11, 1, kSpear, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kSpearDurability, kBlacksmithing, 10, 1, kSpear, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kSpearSpeed, kBlacksmithing, 12, 1, kSpear, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kSpearSilver,               kSpear, 1, kSilverAttribute);

	it_CreateMagicWeapon(kSpearPoison,               kSpear, 0, kPoisonAttribute);

	it_Ingredients	(kSpearPoison, kPoisoning, 3, 1, kSpear, 1, kPoison, 0,0,0,0,0,0,1);

	it_CreateMagicWeapon(kWarHammerSpeed,                kWarHammer, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kWarHammerPower,                kWarHammer, 1, kPowerAttribute);

	it_CreateMagicWeapon(kWarHammerDurability,           kWarHammer, 1, kDurabilityAttribute);

	it_Ingredients(kWarHammerPower, kBlacksmithing, 11, 1, kWarHammer, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kWarHammerDurability, kBlacksmithing, 10, 1, kWarHammer, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kWarHammerSpeed, kBlacksmithing, 12, 1, kWarHammer, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);


	it_CreateMagicWeapon(kMaceSpeed,                kMace, 1, kSpeedAttribute);

	it_CreateMagicWeapon(kMacePower,                kMace, 1, kPowerAttribute);

	it_CreateMagicWeapon(kMaceDurability,           kMace, 1, kDurabilityAttribute);
	
	it_Ingredients(kMacePower, kBlacksmithing, 11, 1, kMace, 10, kIngot, 1, kEssenceOfPower, 0,0,0,0,1);
	it_Ingredients(kMaceDurability, kBlacksmithing, 10, 1, kMace, 10, kIngot, 1, kEssenceOfDurability, 0,0,0,0,1);
	it_Ingredients(kMaceSpeed, kBlacksmithing, 12, 1, kMace, 10, kIngot, 1, kEssenceOfSpeed, 0,0,0,0,1);
}

// ----------------------------------------------------------------- //
void fi_GetBankSettings(void)
// ----------------------------------------------------------------- //

{
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	char									numStr4[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	long									num4;
	int										currentBank;

	fi_StartDebugTimer();

	for (i=0;i>kMaxBanks;i++)
		tb_SetRect(&bankRect[i],0,0,0,0);

	currentBank=0;

	dataStream = fopen("Server Settings", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetBankSettings");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#bank",inStr,5)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");
				strcpy(numStr3,"");
				strcpy(numStr4,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr3[j]=inStr[i];

						numStr3[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr4[j]=inStr[i];

						numStr4[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);
					tb_StringToNum(numStr3, &num3);
					tb_StringToNum(numStr4, &num4);

					if (currentBank<kMaxBanks)
					{
						tb_SetRect(&bankRect[currentBank],num1,num2,num3,num4);
						currentBank++;
					}

				}

			}
		}

		fclose( dataStream );

		return;
	}

	return;

}

// ----------------------------------------------------------------- //
void fi_GetTempleSettings(void)
// ----------------------------------------------------------------- //

{
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	char									numStr4[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	long									num4;
	int										currentTemple;

	for (i=0;i>kMaxTemples;i++)
		tb_SetRect(&templeRect[i],0,0,0,0);

	currentTemple=0;

	fi_StartDebugTimer();

	dataStream = fopen("Server Settings", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetTempleSettings");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#temple",inStr,7)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");
				strcpy(numStr3,"");
				strcpy(numStr4,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr3[j]=inStr[i];

						numStr3[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr4[j]=inStr[i];

						numStr4[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);
					tb_StringToNum(numStr3, &num3);
					tb_StringToNum(numStr4, &num4);

					if (currentTemple<kMaxTemples)
					{
						tb_SetRect(&templeRect[currentTemple],num1,num2,num3,num4);
						currentTemple++;
					}

				}

			}
		}

		fclose( dataStream );

		return;
	}

	return;

}

// ----------------------------------------------------------------- //
void fi_GetPortalSettings(void)
// ----------------------------------------------------------------- //

{
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	char									numStr4[kStringLength];
	char									numStr5[kStringLength];
	char									numStr6[kStringLength];
	char									numStr7[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	long									num4;
	long									num5;
	long									num6;
	long									num7;
	int										currentPortal;

	for (i=0;i>kMaxPortals;i++)
	{
		tb_SetRect(&portalRect[i],0,0,0,0);
		portalItem[i]=0;
		portalRow[i]=0;
		portalCol[i]=0;
	}

	currentPortal=0;

	fi_StartDebugTimer();
	dataStream = fopen("Server Settings", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetPortalSettings");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#portal",inStr,7)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");
				strcpy(numStr3,"");
				strcpy(numStr4,"");
				strcpy(numStr5,"");
				strcpy(numStr6,"");
				strcpy(numStr7,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr3[j]=inStr[i];

						numStr3[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr4[j]=inStr[i];

						numStr4[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr5[j]=inStr[i];

						numStr5[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr6[j]=inStr[i];

						numStr6[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr7[j]=inStr[i];

						numStr7[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);
					tb_StringToNum(numStr3, &num3);
					tb_StringToNum(numStr4, &num4);
					tb_StringToNum(numStr5, &num5);
					tb_StringToNum(numStr6, &num6);
					tb_StringToNum(numStr7, &num7);

					if (currentPortal<kMaxPortals)
					{
						tb_SetRect(&portalRect[currentPortal],num1,num2,num3,num4);
						portalItem[currentPortal]=num5;
						portalRow[currentPortal]=num6;
						portalCol[currentPortal]=num7;
						currentPortal++;
					}

				}

			}
		}

		fclose( dataStream );

		return;
	}

	return;

}

// ----------------------------------------------------------------- //
void fi_GetStableSettings(void)
// ----------------------------------------------------------------- //

{
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	char									numStr4[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	long									num4;
	int										currentStable;

	for (i=0;i<kMaxStables;i++)
		tb_SetRect(&stableRect[i],0,0,0,0);

	currentStable=0;

	fi_StartDebugTimer();

	dataStream = fopen("Server Settings", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetStableSettings");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#stable",inStr,7)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");
				strcpy(numStr3,"");
				strcpy(numStr4,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr3[j]=inStr[i];

						numStr3[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr4[j]=inStr[i];

						numStr4[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);
					tb_StringToNum(numStr3, &num3);
					tb_StringToNum(numStr4, &num4);

					if (currentStable<kMaxStables)
					{
						tb_SetRect(&stableRect[currentStable],num1,num2,num3,num4);
						currentStable++;
					}

				}

			}
		}

		fclose( dataStream );

		return;
	}

	return;

}

// ----------------------------------------------------------------- //
void fi_GetLadderSettings(void)
// ----------------------------------------------------------------- //

{
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	char									numStr4[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	long									num4;
	int										currentLadder;

	for (i=0;i<kMaxLadders;i++)
	{
		ladderJump[i].sRow=0;
		ladderJump[i].sCol=0;
		ladderJump[i].dRow=0;
		ladderJump[i].dCol=0;
	}

	currentLadder=0;

	fi_StartDebugTimer();
	dataStream = fopen("Server Settings", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetLadderSettings");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#jump",inStr,5)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");
				strcpy(numStr3,"");
				strcpy(numStr4,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr3[j]=inStr[i];

						numStr3[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr4[j]=inStr[i];

						numStr4[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);
					tb_StringToNum(numStr3, &num3);
					tb_StringToNum(numStr4, &num4);

					if (num1<0) continue;

					if (num1>=kMapWidth) continue;

					if (num2<0) continue;

					if (num2>=kMapWidth) continue;

					if (num3<0) continue;

					if (num3>=kMapWidth) continue;

					if (num4<0) continue;

					if (num4>=kMapWidth) continue;

					if (currentLadder<kMaxLadders)
					{
						ladderJump[currentLadder].sRow=num1;
						ladderJump[currentLadder].sCol=num2;
						ladderJump[currentLadder].dRow=num3;
						ladderJump[currentLadder].dCol=num4;
						currentLadder++;
					}

				}

			}
		}

		fclose( dataStream );

		return;
	}

	return;

}

// ----------------------------------------------------------------- //
void fi_GetItemSettings(void)
// ----------------------------------------------------------------- //

{
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	long									num1;
	long									num2;

	fi_StartDebugTimer();
	dataStream = fopen("Item Settings", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetItemSettings");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#item",inStr,5)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);

					if (num1<0) continue;

					if (num1>=kMaxItems) continue;

					if (num2<1) continue;

					if (num2>60000) continue;

					itemList[num1].price=num2;

				}

			}
		}

		fclose( dataStream );

		return;
	}

	return;

}

// ----------------------------------------------------------------- //
void fi_GetGMSettings(void)
// ----------------------------------------------------------------- //

{
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	long									num1;
	long									num2;
	//long									num3;
	//long									num4;
	int										currentJump;
	char									command[kStringLength];

	for (i=0;i<kMaxGMJumps;i++)
	{
		strcpy(gmJump[i].command,"");
		gmJump[i].row=0;
		gmJump[i].col=0;
	}

	currentJump=0;

	fi_StartDebugTimer();
	dataStream = fopen("Server Settings", "r" );

	if (dataStream != NULL)
	{
		fi_CheckDebugTimer("fi_GetGMSettings");

		while (!feof(dataStream))
		{
			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#gmjump",inStr,7)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='"')
					{
						start=i+1;
						break;
					}

				strcpy(command,"");

				j=0;

				for (i=start;i<strlen(inStr);i++)
				{
					if (inStr[i]=='"')
						break;

					if (j<47)
					{
						command[j]=inStr[i];
						command[j+1]='\0';
						j++;
					}
				}

				start=i;

				for (i=start;i<strlen(inStr);i++)
					if (inStr[i]==',')
					{
						start=i+1;
						break;
					}

				if (start>=strlen(inStr))
					continue;

				strcpy(numStr1,"");

				strcpy(numStr2,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);

					if (num1<0) continue;

					if (num1>=kMapWidth) continue;

					if (num2<0) continue;

					if (num2>=kMapWidth) continue;

					if (currentJump<kMaxGMJumps)
					{
						if (strlen(command)>kGMJumpStringLength)
							command[kGMJumpStringLength-1]='\0';

						strcpy(gmJump[currentJump].command,command);

						gmJump[currentJump].row=num1;

						gmJump[currentJump].col=num2;

						currentJump++;
					}

				}

			}
		}

		fclose( dataStream );

		return;
	}

	return;

}

// ----------------------------------------------------------------- //
void fi_SendLoginMessage(int i)
// ----------------------------------------------------------------- //

{
	FILE									*dataStream;
	char									inStr[kStringLength];


	dataStream = fopen("Login Message", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strlen(inStr)>0)
			{
				if (inStr[strlen(inStr)-1]=='\n')
					inStr[strlen(inStr)-1]='\0';

				nw_SendSystemTextToClient(i, inStr);
			}
		}

		fclose( dataStream );

		return;
	}

	return;

}


// ----------------------------------------------------------------- //
void fi_GetShopSettings(void)
// ----------------------------------------------------------------- //

{
	FILE									*dataStream;
	char									inStr[kStringLength];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	char									numStr4[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	long									num4;
	int										currentShop;
	int										currentSlot;
	TBoolean							loadingShop;
	char									name[48];
	TBoolean							traderShop;
	float									fl;

	for (j=0;j<kMaxShops;j++)
		for (i=0;i<kMaxInventorySize;i++)
		{
			shop[j][i].itemID					=	0;
			shop[j][i].cost						=	0;
			shop[j][i].forSale				=	false;
			shop[j][i].willBuy				=	false;
			shop[j][i].transactions		=	0;
			shop[j][i].availibility   = 0;
		}

	for (i=0;i<kMaxShops;i++)
	{
		tb_SetRect(&shopRect[i],0,0,0,0);
		strcpy(traderName[i],"");
	}

	currentShop=1;

	fi_StartDebugTimer();
	dataStream = fopen("Server Settings", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetShopSettings");

			if (!fgets(inStr,kStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#shop",inStr,5)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");
				strcpy(numStr3,"");
				strcpy(numStr4,"");
				num1=0;
				num2=0;
				num3=0;
				num4=0;

				traderShop=false;

				for (i=0;i<strlen(inStr);i++)
				{
					if (inStr[i]==';')
						break;

					if (inStr[i]=='"')
					{
						traderShop=true;
						start=i+1;
						break;
					}
				}

				if (traderShop)
				{
					strcpy(name,"");
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]=='"')
							break;

						if (j<47)
						{
							name[j]=inStr[i];
							name[j+1]='\0';
							j++;
						}
					}
				}
				else
				{
					if ((start>0) && (start<strlen(inStr)))
					{
						j=0;

						for (i=start;i<strlen(inStr);i++)
						{
							if (inStr[i]==',')
								break;

							numStr1[j]=inStr[i];

							numStr1[j+1]='\0';

							j++;

							start++;
						}

						start++;

						j=0;

						for (i=start;i<strlen(inStr);i++)
						{
							if (inStr[i]==',')
								break;

							numStr2[j]=inStr[i];

							numStr2[j+1]='\0';

							j++;

							start++;
						}

						start++;

						j=0;

						for (i=start;i<strlen(inStr);i++)
						{
							if (inStr[i]==',')
								break;

							numStr3[j]=inStr[i];

							numStr3[j+1]='\0';

							j++;

							start++;
						}

						start++;

						j=0;

						for (i=start;i<strlen(inStr);i++)
						{
							if (inStr[i]==')')
								break;

							numStr4[j]=inStr[i];

							numStr4[j+1]='\0';

							j++;

							start++;
						}

						tb_StringToNum(numStr1, &num1);

						tb_StringToNum(numStr2, &num2);
						tb_StringToNum(numStr3, &num3);
						tb_StringToNum(numStr4, &num4);
					}
				}


				if (currentShop<kMaxShops)
				{
					if (!traderShop)
						tb_SetRect(&shopRect[currentShop],num1,num2,num3,num4);
					else
					{
						strcpy(traderName[currentShop],name);
						tb_SetRect(&shopRect[currentShop],1,1,1,1);
					}

					currentSlot=0;

					loadingShop=true;

					while (loadingShop)
					{
						if (!fgets(inStr,kStringLength,dataStream))
						{
							fclose( dataStream );
							return;
						}

						if (strncmp("#additem",inStr,8)==0)
						{

							start=0;

							for (i=0;i<strlen(inStr);i++)
								if (inStr[i]=='(')
								{
									start=i+1;
									break;
								}

							strcpy(numStr1,"");

							strcpy(numStr2,"");
							strcpy(numStr3,"");

							if ((start>0) && (start<strlen(inStr)))
							{
								j=0;

								for (i=start;i<strlen(inStr);i++)
								{
									if (inStr[i]==',')
										break;

									numStr1[j]=inStr[i];

									numStr1[j+1]='\0';

									j++;

									start++;
								}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
								{
									if (inStr[i]==',')
										break;

									numStr2[j]=inStr[i];

									numStr2[j+1]='\0';

									j++;

									start++;
								}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
								{
									if (inStr[i]==',')
										break;

									numStr3[j]=inStr[i];

									numStr3[j+1]='\0';

									j++;

									start++;
								}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
								{
									if (inStr[i]==')')
										break;

									numStr4[j]=inStr[i];

									numStr4[j+1]='\0';

									j++;

									start++;
								}

								tb_StringToNum(numStr1, &num1);

								//tb_StringToNum(numStr2, &num2);
								tb_StringToNum(numStr3, &num3);
								tb_StringToFloat(numStr4, &fl);

								if (currentSlot<kMaxInventorySize)
								{
									if (strcmp(numStr2,"SELL")==0)
										sh_AddItemToShop	(currentShop,				num1,				kSellOnly,     num3, fl);

									if (strcmp(numStr2,"BUY")==0)
										sh_AddItemToShop	(currentShop,				num1,				kBuyOnly,      num3, fl);

									if (strcmp(numStr2,"BUYANDSELL")==0)
										sh_AddItemToShop	(currentShop,				num1,				kBuyAndSell,   num3, fl);

									currentSlot++;
								}

								//currentSlot++;

								//if (currentSlot>=kMaxInventorySize)
								//currentSlot=kMaxInventorySize-1;
								//goto NEXT_SHOP;

							}

							//if ((strncmp("#}shop",inStr,8)==0) || (currentSlot>=kMaxInventorySize))
							//	goto NEXT_SHOP;
						}

						if (strncmp("#endshop",inStr,8)==0)
							loadingShop=false;

					}

					//NEXT_SHOP:
					//	currentShop++;
					currentShop++;
				}


				//}
				//NEXT_SHOP:
				//currentShop++;

			}



		}

		fclose( dataStream );

		return;
	}

	return;

}

// ----------------------------------------------------------------- //
void fi_GetNPCSettings(void)
// ----------------------------------------------------------------- //

{
	int										cStringLength=256;
	FILE									*dataStream;
	char									inStr[256];
	//char									rinStr[kStringLength];
	//char									value[kStringLength];
	int										i;
	int										j;
	int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	char									numStr4[kStringLength];
	char									numStr5[kStringLength];
	char									numStr6[kStringLength];
	char									numStr7[kStringLength];
	char									numStr8[kStringLength];
	char									numStr9[kStringLength];
	char									numStr10[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	long									num4;
	long									num5;
	long									num6;
	long									num7;
	long									num8;
	long									num9;
	long									num10;
	//int										currentShop;
	//int										currentSlot;
	//TBoolean							loadingShop;
	char									name[kStringLength];
	TBoolean							loadingNPC;
	int										currentNPC=1;
	char									question[kStringLength];
	char									answer[kStringLength];
	char									response[kStringLength];

	//char									debug[255];

	fi_StartDebugTimer();
	dataStream = fopen("NPC Settings", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			fi_CheckDebugTimer("fi_GetNPCSettings");

			if (!fgets(inStr,cStringLength,dataStream))
			{
				fclose( dataStream );
				np_CheckNPCs();
				return;
			}

			if (strncmp("#npc",inStr,4)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='"')
					{
						start=i+1;
						break;
					}

				j=0;

				for (i=start;i<strlen(inStr);i++)
					if (inStr[i]!='"')
					{
						name[j]=inStr[i];
						name[j+1]='\0';
						j++;

						if (j>=kStringLength)
							break;

						start++;
					}
					else
						break;

				name[19]='\0';

				for (i=start;i<strlen(inStr);i++)
					if (inStr[i]!=',')
						start++;
					else
						break;

				start++;

				strcpy(numStr1,"");

				strcpy(numStr2,"");

				strcpy(numStr3,"");

				strcpy(numStr6,"");

				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr3[j]=inStr[i];

						numStr3[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr6[j]=inStr[i];

						numStr6[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr7[j]=inStr[i];

						numStr7[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr8[j]=inStr[i];

						numStr8[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr9[j]=inStr[i];

						numStr9[j+1]='\0';

						j++;

						start++;
					}
					
					start++;
					j=0;
					
					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr10[j]=inStr[i];

						numStr10[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);

					tb_StringToNum(numStr2, &num2);
					tb_StringToNum(numStr3, &num3);
					tb_StringToNum(numStr6, &num6);
					tb_StringToNum(numStr7, &num7);
					tb_StringToNum(numStr8, &num8);
					tb_StringToNum(numStr9, &num9);
					tb_StringToNum(numStr10, &num10);

					//if (num10!=1)
					//{
					/*	strcpy(debug, name);
					strcat(debug, "\t\t:\t\t");
					strcat(debug, numStr1);
					strcat(debug, "\t\t;\t\t");
					strcat(debug, numStr2);
					strcat(debug, "\t\t;\t\t");
					strcat(debug, numStr3);
					strcat(debug, "\t\t;\t\t");
					strcat(debug, numStr6);
					strcat(debug, "\t\t;\t\t");
					strcat(debug, numStr7);
					strcat(debug, "\t\t;\t\t");
					strcat(debug, numStr8);
					strcat(debug, "\t\t;\t\t");
					strcat(debug, numStr9);
					strcat(debug, "\t\t;\t\t");
					strcat(debug, numStr10);

					fi_WriteToErrorLog(debug);*/
					//}

					if (num3>200) num3=200;

					if ((num1<0) || (num1>=kMapWidth) || (num2<0) || (num2>=kMapHeight))
					{
						num1=964;
						num2=654;
						num3=4;
					}

					if (num6==kNPCGuard)
						np_SpawnNPC(currentNPC, num1, num2, num3, kBreastPlate, kLegsPlate, num6, num7, num8, num9, num10, false);
					else if (num6==kNPCBrigand)
						np_SpawnNPC(currentNPC, num1, num2, num3, kBreastChain, kBrownLeatherLegs, num6, num7, num8, num9, num10, false);
					else if (num6==kNPCMercenary)
						np_SpawnNPC(currentNPC, num1, num2, num3, kBreastChain, kChainLegs, num6, num7, num8, num9, num10, false);
					else if (num6==kNPCDwarf1)
						np_SpawnNPC(currentNPC, num1, num2, num3, kBlackBreastPlate, kBlackLegsPlate, num6, num7, num8, num9, num10, false);
					//Dave
					else if (num6==kNPCPrivateer)
						np_SpawnNPC(currentNPC, num1, num2, num3, 0, kBrownLeatherLegs, num6, num7, num8, num9, num10, false);
					else
					{

						if (tb_Rnd(1,7)==1)
							np_SpawnNPC(currentNPC, num1, num2, num3, tb_Rnd(kGrayRobe,kPurpleRobe), 0, kNPCRegular, num7, num8, num9, num10, false);
						else
							np_SpawnNPC(currentNPC, num1, num2, num3, tb_Rnd(kGrayShirt,kPurpleShirt), tb_Rnd(kGrayPants,kPurplePants), kNPCRegular, num7, num8, num9, num10, false);
					}
					
					if (strlen(name)>=kNPCNameLength)
						name[kNPCNameLength-1]='\0';

					strcpy(npc[currentNPC].name,name);
					
					npc[currentNPC].spawned=false;

					loadingNPC=true;

					while (loadingNPC)
					{
						if (!fgets(inStr,cStringLength,dataStream))
						{
							fclose( dataStream );
							np_CheckNPCs();
							return;
						}

						if (strncmp("#endnpc",inStr,7)==0)
							loadingNPC=false;

						//-----------------------------------------------------------------------------//
						if (strncmp("#if",inStr,3)==0)
						{
							strcpy(question,"");
							strcpy(answer,"");
							start=0;

							for (i=0;i<strlen(inStr);i++)
								if (inStr[i]=='"')
								{
									start=i+1;
									break;
								}

							if (start>0)
							{
								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]=='"')
										break;
									else
									{
										if (j>=kNPCQuestionLength)
											break;

										question[j]=inStr[i];

										question[j+1]='\0';

										j++;
									}
							}

							start=0;

							for (i=0;i<strlen(inStr)-3;i++)
								if ((inStr[i]=='s') && (inStr[i+1]=='a') && (inStr[i+2]=='y') && (inStr[i+3]=='('))
								{
									start=i+4;
									break;
								}

							if ((start>0) && (start<strlen(inStr)))
							{
								k=0;

								for (j=start;j<strlen(inStr);j++)
								{
									if (inStr[j]==')')
										break;

									answer[k]=inStr[j];

									answer[k+1]='\0';

									k++;

									if (k>=kStringLength)
										break;
								}
							}

							np_AddQuestion(currentNPC,question,answer);
						}

						//-----------------------------------------------------------------------------//
						if (strncmp("#trade",inStr,6)==0)
						{
							start=0;

							for (i=0;i<strlen(inStr);i++)
								if (inStr[i]=='(')
								{
									start=i+1;
									break;
								}

							strcpy(numStr1,"");

							strcpy(numStr2,"");
							strcpy(numStr3,"");
							strcpy(numStr4,"");

							if ((start>0) && (start<strlen(inStr)))
							{
								j=0;

								for (i=start;i<strlen(inStr);i++)
								{
									if (inStr[i]==',')
										break;

									numStr1[j]=inStr[i];

									numStr1[j+1]='\0';

									j++;

									start++;
								}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
								{
									if (inStr[i]==',')
										break;

									numStr2[j]=inStr[i];

									numStr2[j+1]='\0';

									j++;

									start++;
								}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
								{
									if (inStr[i]==',')
										break;

									numStr3[j]=inStr[i];

									numStr3[j+1]='\0';

									j++;

									start++;
								}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
								{
									if (inStr[i]==',')
										break;

									numStr4[j]=inStr[i];

									numStr4[j+1]='\0';

									j++;

									start++;
								}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
								{
									if (inStr[i]==',')
										break;

									numStr5[j]=inStr[i];

									numStr5[j+1]='\0';

									j++;

									start++;
								}

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]=='"')
									{
										start=i+1;
										break;
									}

								j=0;

								strcpy(response,"");

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]!='"')
									{
										response[j]=inStr[i];
										response[j+1]='\0';
										j++;

										if (j>=(kStringLength-1))
											break;
									}
									else
										break;

								tb_StringToNum(numStr1, &num1);

								tb_StringToNum(numStr2, &num2);

								tb_StringToNum(numStr3, &num3);

								tb_StringToNum(numStr4, &num4);

								tb_StringToNum(numStr5, &num5);

								np_AddTrade(currentNPC,num1,num2,num3,num4,num5,response);
							}
						}

						//-----------------------------------------------------------------------------//
					}

					if (currentNPC<(kMaxNPCs-1))
						currentNPC++;

				}
			}


		}
	}

	fclose( dataStream );
	np_CheckNPCs();

}

// ----------------------------------------------------------------- //
void fi_SpawnChests(void)
// ----------------------------------------------------------------- //

{
	int										cStringLength=256;
	FILE									*dataStream;
	char									inStr[256];
	int										i;
	int										j;
	//int										k;
	int										start;
	char									numStr1[kStringLength];
	char									numStr2[kStringLength];
	char									numStr3[kStringLength];
	char									numStr4[kStringLength];
	char									numStr5[kStringLength];
	char									numStr6[kStringLength];
	long									num1;
	long									num2;
	long									num3;
	long									num4;
	long									num5;
	long									num6;
	TBoolean								loadingChest;
	TBoolean								okToSpawn;

	//fi_StartDebugTimer();
	dataStream = fopen("Chests Settings", "r" );

	if (dataStream != NULL)
	{
		while (!feof(dataStream))
		{
			//fi_CheckDebugTimer("fi_SpawnChests");
			if (!fgets(inStr,cStringLength,dataStream))
			{
				fclose( dataStream );
				return;
			}

			if (strncmp("#chest",inStr,6)==0)
			{
				start=0;

				for (i=0;i<strlen(inStr);i++)
					if (inStr[i]=='(')
					{
						start=i+1;
						break;
					}

				strcpy(numStr1,"");

				strcpy(numStr2,"");
				strcpy(numStr3,"");
				strcpy(numStr4,"");
					
				if ((start>0) && (start<strlen(inStr)))
				{
					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr1[j]=inStr[i];

						numStr1[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr2[j]=inStr[i];

						numStr2[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==',')
							break;

						numStr3[j]=inStr[i];

						numStr3[j+1]='\0';

						j++;

						start++;
					}

					start++;

					j=0;

					for (i=start;i<strlen(inStr);i++)
					{
						if (inStr[i]==')')
							break;

						numStr4[j]=inStr[i];

						numStr4[j+1]='\0';

						j++;

						start++;
					}

					tb_StringToNum(numStr1, &num1);
					tb_StringToNum(numStr2, &num2);
					tb_StringToNum(numStr3, &num3);
					tb_StringToNum(numStr4, &num4);
					
					//Dave randomize chest spawn location.....
					
						
						//num1=num1+(tb_Rnd(0,1));
						//num2=num2+(tb_Rnd(0,1)-tb_Rnd(0,1));
						
					if ((num1<0) || (num1>=kMapWidth))
						{
						num1=964;
						}
					if ((num2<0) || (num2>=kMapHeight))	
						{
						num2=654;
						}
					
					//if ((kWater==false) && (canWalkOn[map[r][c]]) && (doorway[map[r][c]]==false) && (canFlyOver[map[r][c]]==false) && (containerMap[r][c]==0))
						okToSpawn=true;
					
					if (containerMap[num1][num2]!=0)
						okToSpawn=false;
					
					if (okToSpawn)
								
						
					in_DropGold(28, tb_Rnd(num3, num4), num1, num2, 0, false, "");

					loadingChest=true;

					while (loadingChest)
					{
						if (!fgets(inStr,cStringLength,dataStream))
						{
							fclose( dataStream );
							return;
						}
							
						if (strncmp("#endchest",inStr,9)==0)
							loadingChest=false;

						//-----------------------------------------------------------------------------//
						if (strncmp("#lock",inStr,5)==0)
						{
							strcpy(numStr3,"");
							strcpy(numStr4,"");
							strcpy(numStr5,"");
							start=0;

							for (i=0;i<strlen(inStr);i++)
								if (inStr[i]=='(')
								{
									start=i+1;
									break;
								}

							if (start>0)
							{
								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]==',')
										break;
									else
									{
										numStr3[j]=inStr[i];
										numStr3[j+1]='\0';
										j++;
										start++;
									}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]==',')
										break;
									else
									{
										numStr4[j]=inStr[i];
										numStr4[j+1]='\0';
										j++;
										start++;
									}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]==')')
										break;
									else
									{
										numStr5[j]=inStr[i];
										numStr5[j+1]='\0';
										j++;
										start++;
									}
							}

							if (okToSpawn)
							{
								tb_StringToNum(numStr3, &num3);
								tb_StringToNum(numStr4, &num4);
								tb_StringToNum(numStr5, &num5);

								if (tb_Rnd(1,100)<num5)
									in_DropItem(0, 28, kLock, 1, num1, num2, tb_Rnd(num3, num4), false, "");
							}
						}

						//-----------------------------------------------------------------------------//
						if (strncmp("#trap",inStr,5)==0)
						{
							strcpy(numStr3,"");
							strcpy(numStr4,"");
							strcpy(numStr5,"");
							start=0;

							for (i=0;i<strlen(inStr);i++)
								if (inStr[i]=='(')
								{
									start=i+1;
									break;
								}

							if (start>0)
							{
								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]==',')
										break;
									else
									{
										numStr3[j]=inStr[i];
										numStr3[j+1]='\0';
										j++;
										start++;
									}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]==',')
										break;
									else
									{
										numStr4[j]=inStr[i];
										numStr4[j+1]='\0';
										j++;
										start++;
									}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]==')')
										break;
									else
									{
										numStr5[j]=inStr[i];
										numStr5[j+1]='\0';
										j++;
										start++;
									}
							}

							if (okToSpawn)
							{
								tb_StringToNum(numStr3, &num3);
								tb_StringToNum(numStr4, &num4);
								tb_StringToNum(numStr5, &num5);

								if (tb_Rnd(1,100)<num5)
									in_DropItem(0, 28, kTrap, 1, num1, num2, tb_Rnd(num3, num4), false, "");
							}
						}

						//-----------------------------------------------------------------------------//
						if (strncmp("#item",inStr,5)==0)
						{
							strcpy(numStr3,"");
							strcpy(numStr4,"");
							strcpy(numStr5,"");
							strcpy(numStr6,"");
							start=0;

							for (i=0;i<strlen(inStr);i++)
								if (inStr[i]=='(')
								{
									start=i+1;
									break;
								}

							if (start>0)
							{
								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]==',')
										break;
									else
									{
										numStr3[j]=inStr[i];
										numStr3[j+1]='\0';
										j++;
										start++;
									}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]==',')
										break;
									else
									{
										numStr4[j]=inStr[i];
										numStr4[j+1]='\0';
										j++;
										start++;
									}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]==',')
										break;
									else
									{
										numStr5[j]=inStr[i];
										numStr5[j+1]='\0';
										j++;
										start++;
									}

								start++;

								j=0;

								for (i=start;i<strlen(inStr);i++)
									if (inStr[i]==')')
										break;
									else
									{
										numStr6[j]=inStr[i];
										numStr6[j+1]='\0';
										j++;
										start++;
									}
							}

							if (okToSpawn)
							{
								tb_StringToNum(numStr3, &num3);
								tb_StringToNum(numStr4, &num4);
								tb_StringToNum(numStr5, &num5);
								tb_StringToNum(numStr6, &num6);

								if (tb_Rnd(1,100)<num6)
								{
									if (in_CanGroup(num3))
										in_DropItem(0, 28, num3, tb_Rnd(num4,num5), num1, num2, 0, false, "");
									else
									{
										num5=tb_Rnd(num4, num5);

										for (num4=0; num4<num5; num4++)
											in_DropItem(0, 28, num3, 127, num1, num2, 0, false, "");
									}
								}

							}
						}

						//-----------------------------------------------------------------------------//
					}


				}
			}


		}
	}

	fclose( dataStream );

}

// ----------------------------------------------------------------- //
TBoolean fi_ReadQuestFile(int number, int *gold, int *experience, int *item, int *count, char quest[1024]) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*questStream;
	char									filename[kStringLength];
	char									filenumber[kStringLength];
	char									inString[kStringLength];
	UInt32								inValue;
	//int                   i;

	tb_IntegerToString(number,filenumber);
	strcpy(filename,":Oberin Cache:");
	strcat(filename,filenumber);

	questStream = fopen(filename, "r" );

	if (questStream != NULL)
	{

		fseek( questStream, 0L, SEEK_SET );

		fscanf( questStream, "%i", &inValue);
		fgets(inString,kStringLength,questStream);
		*gold	=	(int)inValue;

		fscanf( questStream, "%i", &inValue);
		fgets(inString,kStringLength,questStream);
		*experience	=	(int)inValue;

		fscanf( questStream, "%i", &inValue);
		fgets(inString,kStringLength,questStream);
		*item	=	(int)inValue;

		fscanf( questStream, "%i", &inValue);
		fgets(inString,kStringLength,questStream);
		*count	=	(int)inValue;

		fscanf( questStream, "%s", quest);

		fclose( questStream );

		//for (i=0;i<strlen(quest);i++)
		//	if (quest[i]=='*')
		//		quest[i]=' ';

		return (true);

	}

	return (false);

}

// ----------------------------------------------------------------- //
void fi_LoadBanFile(void) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*macStream;
	int                   j;

	for (j=0;j<1000;j++)
		strcpy(ban[j],"");

	j=0;

	macStream = fopen("ban", "r" );

	if (macStream != NULL)
	{
		fseek( macStream, 0L, SEEK_SET );

		while (true)
		{
			//fscanf( macStream, "%s", ban[j]);
			fgets(ban[j],kStringLength,macStream);
			ban[j][strlen(ban[j])-1]='\0';
			j++;

			if (feof(macStream))
				break;
		}
	}

	fclose( macStream );

}

// ----------------------------------------------------------------- //
TBoolean fi_IsCharacterCreatorUp(void)
// ----------------------------------------------------------------- //

{
	FILE									*stream;

	stream = fopen("CC Status", "r" );

	if (stream != NULL)
	{
		fclose( stream );
		return (true);
	}

	fclose( stream );

	return (false);

}

// ----------------------------------------------------------------- //
int fi_TurnCharacterCreatorOn(void)
// ----------------------------------------------------------------- //

{
	FILE									*stream;

	stream = fopen("CC Status", "a" );

	if (stream != NULL)
		fprintf( stream, "%s", "On");
	else
		return (1);

	fclose( stream );

	return (0);

}

// ----------------------------------------------------------------- //
int fi_TurnCharacterCreatorOff(void)
// ----------------------------------------------------------------- //

{

	return (remove("CC Status"));

}

// ----------------------------------------------------------------- //
void fi_AddToBanFile(int i, int c) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*macStream;
	//int                   j;

	if (creature[c].playerIndex==0)
		return;

	//if (player[i].category!=1)
	//if (pl_GMLevel(i)<kGMLevel) // put in carbon 11/27
	if (pl_AuthorizedTo(player[i].creatureIndex,kCanBan)==false) // put in carbon 11/27
		return;

	if (strcmp(player[creature[c].playerIndex].macID,"")==0)
	{
		nw_SendTextToClient(i, "No MAC", 0);
		return;
	}

	macStream = fopen("ban", "a" );

	if (macStream != NULL)
	{
		fprintf( macStream, "%s", player[creature[c].playerIndex].macID);
		fprintf( macStream, "%s", "\n");
		fclose( macStream );
	}

	fi_LoadBanFile();

}

// ----------------------------------------------------------------- //
void fi_GetAKA(int i, int c) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*macStream;
	char                  filename[kStringLength];
	int                   j;
	char                  theName[kStringLength];
	char                  macFile[kStringLength];
	char				*myIP;//[64];
	int						k;

	//if (player[i].god==false)
	//  return;

	//if (player[i].category!=1)
	//if (pl_GMLevel(i)<kGMLevel) // put in carbon 11/27

	if (pl_AuthorizedTo(player[i].creatureIndex,kCanAKA)==false)
		return;

	if (creature[c].playerIndex==0)
		return;

	strcpy(macFile,player[creature[c].playerIndex].macID);

	if (strcmp(macFile,"")==0)
	{
		nw_SendTextToClient(i, "No MAC", 0);
		return;
	}
	
	if (strcmp(macFile,"52 54 00 12 34 56")==0)
	{
		if (NSpPlayer_GetIPAddress(gNetGame,player[creature[c].playerIndex].id,&myIP)==kNMNoError)
			{
				strcpy(filename,":AKA IP:");
				for(k=0; k<strlen(myIP); k++)
				{
					if(myIP[k]==':')
					{
						myIP[k]='\0';	
					}
						
				}
				strcat(filename,myIP);
				NSpPlayer_FreeAddress( gNetGame, (void **)&myIP );
				}
	}
	else
	{
		strcpy(filename, ":AKA Mac:");

		strcat(filename, macFile);
	}
	macStream = fopen(filename, "r" );

	if (macStream != NULL)
	{
		fseek( macStream, 0L, SEEK_SET );

		while (true)
		{
			fscanf( macStream, "%s", theName);

			if (feof(macStream))
				break;

			for (j=0;j<strlen(theName);j++)
				if (theName[j]=='*')
					theName[j]=' ';

			nw_SendTextToClient(i, theName, 0);
		}
	}

	fclose( macStream );

}

// ----------------------------------------------------------------- //
void fi_WriteToMacFile(char macMessage[kStringLength], int c) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*macStream;
	char                  filename[kStringLength];
	char                  check[kStringLength];
	int                   j;
	char                  theName[kStringLength];
	char				*myIP;//[64];
	int						k;

	strcpy(theName,player[c].playerName);

	for (j=0;j<strlen(theName);j++)
		if (theName[j]==' ')
			theName[j]='*';
	if (strcmp(macMessage,"52 54 00 12 34 56")==0)
	{
		if (NSpPlayer_GetIPAddress(gNetGame,player[c].id,&myIP)==kNMNoError)
			{
				strcpy(filename,":AKA IP:");
				for(k=0; k<strlen(myIP); k++)
				{
					if(myIP[k]==':')
					{
						myIP[k]='\0';	
					}
						
				}
				strcat(filename,myIP);
				NSpPlayer_FreeAddress( gNetGame, (void **)&myIP );
				}
	}
	else
	{

	strcpy(filename, ":AKA Mac:");

	strcat(filename, macMessage);
	}

	macStream = fopen(filename, "r" );

	if (macStream != NULL)
	{
		fseek( macStream, 0L, SEEK_SET );

		while (true)
		{
			fscanf( macStream, "%s", check);

			if (feof(macStream))
				break;

			if (strcmp(check,theName)==0)
			{
				fclose( macStream );
				return;
			}

		}

		fclose( macStream );
	}

	//--------------------------------------------------

	macStream = fopen(filename, "a" );

	if (macStream != NULL)
	{
		fprintf( macStream, "%s", theName);
		fprintf( macStream, "%s", "\n");
		fclose( macStream );
	}

}

// ----------------------------------------------------------------- //
void fi_WriteNumberToErrorLog(int number) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*errorStream;
	int                   e;
	char                  errorMessage[kStringLength];


	tb_NumToString(number, errorMessage);

	errorStream = fopen(":Oberin Logs:Oberin Error Log", "a" );

	if (errorStream != NULL)
	{
		e=fprintf( errorStream, "%s", errorMessage);
		e=fprintf( errorStream, "%s", "\n");
		e=fclose( errorStream );
	}


}

// ----------------------------------------------------------------- //
void fi_WriteToTraceLog(char errorMessage[kStringLength]) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*errorStream;
	int                   e;

	return;

	errorStream = fopen(":Oberin Logs:Oberin Trace Log", "a" );

	if (errorStream != NULL)
	{
		e=fprintf( errorStream, "%s", errorMessage);
		traceCharacters++;

		if (traceCharacters>=80)
		{
			e=fprintf( errorStream, "%s", "\n");
			traceCharacters=0;
		}

		e=fclose( errorStream );
	}

}

// ----------------------------------------------------------------- //
void fi_WriteToErrorLog(char errorMessage[kStringLength]) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*errorStream;
	int                   e;

	errorStream = fopen(":Oberin Logs:Oberin Error Log", "a" );

	if (errorStream != NULL)
	{
		e=fprintf( errorStream, "%s", errorMessage);
		e=fprintf( errorStream, "%s", "\n");
		e=fclose( errorStream );
	}

}

// ----------------------------------------------------------------- //
void fi_WriteToCSLog(char csMessage[kStringLength]) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*csStream;
	int                   e;

	csStream = fopen(":Oberin Logs:Oberin CS Log", "a" );

	if (csStream != NULL)
	{
		e=fprintf( csStream, "%s", csMessage);
		e=fclose( csStream );
	}

}

// ----------------------------------------------------------------- //
void fi_DumpTestData(int length) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*errorStream;
	int                   i;

	errorStream = fopen("Server Flash Bug", "a" );

	if (errorStream != NULL)
	{

		fprintf( errorStream, "%s", "---------------------------------");
		fprintf( errorStream, "%s", "\n");

		for (i=0;i<=length;i++)
		{
			fprintf( errorStream, "%i", testData[i]);
			fprintf( errorStream, "%s", "\n");
		}

		fclose( errorStream );
	}
}

// ----------------------------------------------------------------- //
void fi_DumpInData(void) //[128])
// ----------------------------------------------------------------- //

{
	FILE									*errorStream;
	int                   i;

	return;

	errorStream = fopen(":Oberin Logs:Send this to Stefan@oberin.com", "a" );

	if (errorStream != NULL)
	{

		fprintf( errorStream, "%s", "---------------------------------");
		fprintf( errorStream, "%s", "\n");

		for (i=0;i<kDataSize;i++)
		{
			fprintf( errorStream, "%i", inData[i]);
			fprintf( errorStream, "%s", "\n");
		}

		fclose( errorStream );
	}
}


#ifdef _SERVER
// ----------------------------------------------------------------- //
void fi_OpenGMLog(void)
// ----------------------------------------------------------------- //

{
	char									filename[kStringLength]; //[50];
	//int										j;
	//long									t;
	//DateTimeRec						Date;
	//unsigned long					Secs;
	//char									temp[kStringLength]; //[50];


//	strcpy(filename,"GM Log");

//	gmStream = fopen(filename, "a" );

//	if (gmStream != NULL)
//		fseek( gmStream, 0L, SEEK_SET );
//
}

// ----------------------------------------------------------------- //
void fi_WriteToGMLog(int i, char playerText[kStringLength])
// ----------------------------------------------------------------- //

{
	FILE									*stream;
	char									filename[kStringLength]; //[50];
	//int										j;
	long									t;
	DateTimeRec						Date;
	unsigned long					Secs;
	char									dateAndTime[kStringLength]; //[128];
	char									temp[kStringLength]; //[50];
	char									*myIP;
	
	strcpy(filename,":GM Logs:");

	strcat(filename,player[i].playerName);
	strcat(filename," - GMLog");
	stream = fopen(filename, "a" );
	if (stream != NULL)
	{

		fseek( stream, 0L, SEEK_SET );
		fprintf( stream, "%s", playerText);
		fprintf( stream, "%s", "\n");
		fclose( stream );

	}
	else
		fclose( stream );

}

// ----------------------------------------------------------------- //
void fi_CloseGMLog(void)
// ----------------------------------------------------------------- //

{

	if (gmStream != NULL)
		fclose( gmStream );

}

// ----------------------------------------------------------------- //
void fi_OpenConversationLog(void)
// ----------------------------------------------------------------- //

{
	char									filename[kStringLength]; //[50];
	//int										j;
	long									t;
	DateTimeRec						Date;
	unsigned long					Secs;
	char									temp[kStringLength]; //[50];


	previousBugReport=-1;

	strcpy(filename,":Conversation Logs:");
	strcat(filename,"Transcript from ");

	t = ReadDateTime(&Secs);
	SecondsToDate(Secs, &Date);

	tb_IntegerToString(Date.month,temp);
	strcat(filename,temp);

	strcat(filename,"/");

	tb_IntegerToString(Date.day,temp);
	strcat(filename,temp);
	currentDay=Date.day;

	strcat(filename,"/");

	tb_IntegerToString(Date.year,temp);
	strcat(filename,temp);

	conversationStream = fopen(filename, "a" );

	if (conversationStream != NULL)
		fseek( conversationStream, 0L, SEEK_SET );

}

// ----------------------------------------------------------------- //
void fi_WriteToConversationLog(int	i, char	playerText[kStringLength]) //[kCommandStringMaxLength])
// ----------------------------------------------------------------- //

{
	//int										j;
	long									t;
	DateTimeRec						Date;
	unsigned long					Secs;
	//char									temp[kStringLength]; //[50];


	t = ReadDateTime(&Secs);
	SecondsToDate(Secs, &Date);

	if (Date.day!=currentDay)
	{
		fi_CloseConversationLog();
		fi_OpenConversationLog();
	}

	if (conversationStream != NULL)
	{

		fprintf( conversationStream, "%s", "(");
		fprintf( conversationStream, "%s", player[i].userName);
		fprintf( conversationStream, "%s", "/");
		fprintf( conversationStream, "%s", player[i].playerName);
		fprintf( conversationStream, "%s", "):  ");
		fprintf( conversationStream, "%s", playerText);
		fprintf( conversationStream, "%s", "\n");

	}

}

// ----------------------------------------------------------------- //
void fi_CloseConversationLog(void)
// ----------------------------------------------------------------- //

{

	if (conversationStream != NULL)
		fclose( conversationStream );

}

// ----------------------------------------------------------------- //
void fi_SavePlayerData(int i)
// ----------------------------------------------------------------- //

{
	FILE									*stream;
	char									filename[kStringLength]; //[50];
	int										j;
	int                   thief;

	strcpy(filename,":Accounts:");
	strcat(filename,player[i].userName);

	stream = fopen(filename, "r" );  //Used to prevent a player from deleting his char (via the CC)

	if (stream==NULL)				 //But still having it online and thus saved again.
		return;

	fclose(stream);

	//remove(filename);

	stream = fopen(filename, "w" );

	if (stream != NULL)
	{
		fseek( stream, 0L, SEEK_SET );
		// Character sheet info
		fprintf( stream, "%i", kServerVersion);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%s", player[i].password);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%s", player[i].playerName);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%s", player[i].playerNameTemp);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%s", player[i].guildName);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%s", player[i].titleCustom);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].characterClass);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].sharing);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].demo);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].newPlayer);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].accountNumber);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].row);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].col);
		fprintf( stream, "%s", "\n");

		if (player[i].thiefUntil>0)
		{
			if (player[i].thiefUntil<TickCount())
				thief=0;
			else
				thief=((player[i].thiefUntil-TickCount())/3600);
		}
		else
			thief=0;

		fprintf( stream, "%i", (int)thief);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].title);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].gender);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].category);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].dead);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].curse);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].mute);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].poison);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].hairColor);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)player[i].hairStyle);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%s", "------Begin-Armed-----");

		fprintf( stream, "%s", "\n");

		for (j=0;j<13;j++)
		{
			fprintf( stream, "%i", (int)player[i].arm[j]);
			fprintf( stream, "%s", "\n");
		}

		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].hunger);

		fprintf( stream, "%s", "\n");

		fprintf( stream, "%s", "------Begin-Stats-----");
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].strength);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].intelligence);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].dexterity);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].hitPoints);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].magicPoints);
		fprintf( stream, "%s", "\n");
		
		fprintf( stream, "%i", (int)creature[player[i].creatureIndex].stamina);
		fprintf( stream, "%s", "\n");

		fprintf( stream, "%s", "------Begin-Skills-----");
		fprintf( stream, "%s", "\n");

		for (j=0;j<kMaxSkills;j++)
		{
			fprintf( stream, "%i", (int)player[i].skill[j].level);
			fprintf( stream, "%s", "\n");
		}

		fprintf( stream, "%s", "-------Begin-Inventory------");

		fprintf( stream, "%s", "\n");

		for (j=0;j<kMaxInventorySize;j++)
		{
			fprintf( stream, "%i", (int)creature[player[i].creatureIndex].inventory[j].itemID);
			fprintf( stream, "%s", "\n");

			fprintf( stream, "%i", (int)creature[player[i].creatureIndex].inventory[j].itemCount);
			fprintf( stream, "%s", "\n");
		}

		fprintf( stream, "%s", "-------Begin-Bank------");

		fprintf( stream, "%s", "\n");

		for (j=0;j<kMaxInventorySize;j++)
		{
			fprintf( stream, "%i", (int)player[i].bank[j].itemID);
			fprintf( stream, "%s", "\n");

			fprintf( stream, "%i", (int)player[i].bank[j].itemCount);
			fprintf( stream, "%s", "\n");
		}

		fprintf( stream, "%s", "-------Begin-Pets------");

		fprintf( stream, "%s", "\n");

		for (j=0;j<kMaxPets;j++)
		{
			fprintf( stream, "%i", (int)player[i].petType[j]);
			fprintf( stream, "%s", "\n");

			fprintf( stream, "%s", player[i].petName[j]);
			fprintf( stream, "%s", "\n");

			fprintf( stream, "%i", (int)player[i].petExperience[j]);
			fprintf( stream, "%s", "\n");
			
			fprintf( stream, "%i", (int)player[i].petHealth[j]);
			fprintf( stream, "%s", "\n");

		}

		fclose( stream );

	}
	else
		fclose( stream );

}

// ----------------------------------------------------------------- //
void fi_UpdatePlayerLog(int i, int logStatus)
// ----------------------------------------------------------------- //

{
	FILE									*stream;
	char									filename[kStringLength]; //[50];
	//int										j;
	long									t;
	DateTimeRec						Date;
	unsigned long					Secs;
	char									dateAndTime[kStringLength]; //[128];
	char									temp[kStringLength]; //[50];
	char									*myIP;


	if (logStatus==0)
		strcpy(dateAndTime,"Log On:   ");
	else
		strcpy(dateAndTime,"Log Off:  ");

	t = ReadDateTime(&Secs);

	SecondsToDate(Secs, &Date);

	tb_IntegerToString(Date.month,temp);

	strcat(dateAndTime,temp);

	strcat(dateAndTime,"/");

	tb_IntegerToString(Date.day,temp);

	strcat(dateAndTime,temp);

	strcat(dateAndTime,"/");

	tb_IntegerToString(Date.year,temp);

	strcat(dateAndTime,temp);

	strcat(dateAndTime," at ");

	tb_IntegerToString(Date.hour,temp);

	if (Date.hour<10)
		strcat(dateAndTime,"0");

	strcat(dateAndTime,temp);

	strcat(dateAndTime,":");

	tb_IntegerToString(Date.minute,temp);

	if (Date.minute<10)
		strcat(dateAndTime,"0");

	strcat(dateAndTime,temp);

	strcat(dateAndTime,":");

	tb_IntegerToString(Date.second,temp);

	if (Date.second<10)
		strcat(dateAndTime,"0");

	strcat(dateAndTime,temp);

	strcat(dateAndTime," - ");

	if (NSpPlayer_GetIPAddress(gNetGame,player[i].id,&myIP)==kNMNoError)
	{
		strcat(dateAndTime,myIP);
		NSpPlayer_FreeAddress( gNetGame, (void **)&myIP );
	}

	strcpy(filename,":Activity Logs:");

	strcat(filename,player[i].userName);
	strcat(filename," Activity Log");

	//remove(filename);

	stream = fopen(filename, "a" );

	if (stream != NULL)
	{

		fseek( stream, 0L, SEEK_SET );

		fprintf( stream, "%s", dateAndTime);
		fprintf( stream, "%s", "\n");

		fclose( stream );

	}
	else
		fclose( stream );

}

// ----------------------------------------------------------------- //
void fi_UpdateActivityLog(int i)
// ----------------------------------------------------------------- //

{
	FILE									*stream;
	//char									filename[kStringLength];
	//int										j;
	long									t;
	DateTimeRec						Date;
	unsigned long					Secs;
	char									dateAndTime[kStringLength];
	char									temp[kStringLength];
	char									*myIP;


	strcpy(dateAndTime,"Log On:   ");

	t = ReadDateTime(&Secs);
	SecondsToDate(Secs, &Date);

	tb_IntegerToString(Date.month,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime,"/");

	tb_IntegerToString(Date.day,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime,"/");

	tb_IntegerToString(Date.year,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime," at ");

	tb_IntegerToString(Date.hour,temp);

	if (Date.hour<10)
		strcat(dateAndTime,"0");

	strcat(dateAndTime,temp);

	strcat(dateAndTime,":");

	tb_IntegerToString(Date.minute,temp);

	if (Date.minute<10)
		strcat(dateAndTime,"0");

	strcat(dateAndTime,temp);

	strcat(dateAndTime,":");

	tb_IntegerToString(Date.second,temp);

	if (Date.second<10)
		strcat(dateAndTime,"0");

	strcat(dateAndTime,temp);

	strcat(dateAndTime," - ");

	strcat(dateAndTime,player[i].userName);

	strcat(dateAndTime," - ");

	if (NSpPlayer_GetIPAddress(gNetGame,player[i].id,&myIP)==kNMNoError)
	{
		strcat(dateAndTime,myIP);
		NSpPlayer_FreeAddress( gNetGame, (void **)&myIP );
	}

	//remove(filename);

	stream = fopen("Connection Log", "a" );

	if (stream != NULL)
	{

		fseek( stream, 0L, SEEK_SET );

		fprintf( stream, "%s", dateAndTime);
		fprintf( stream, "%s", "\n");

		fclose( stream );

	}
	else
		fclose( stream );

}

// ----------------------------------------------------------------- //
void fi_UpdateIPLog(NSpPlayerID playerID)
// ----------------------------------------------------------------- //

{
	FILE									*stream;
	//char									filename[kStringLength];
	//int										j;
	long									t;
	DateTimeRec						Date;
	unsigned long					Secs;
	char									dateAndTime[kStringLength];
	char									temp[kStringLength];
	char									*myIP;


	strcpy(dateAndTime,"Log On:   ");

	t = ReadDateTime(&Secs);
	SecondsToDate(Secs, &Date);

	tb_IntegerToString(Date.month,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime,"/");

	tb_IntegerToString(Date.day,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime,"/");

	tb_IntegerToString(Date.year,temp);
	strcat(dateAndTime,temp);

	strcat(dateAndTime," at ");

	tb_IntegerToString(Date.hour,temp);

	if (Date.hour<10)
		strcat(dateAndTime,"0");

	strcat(dateAndTime,temp);

	strcat(dateAndTime,":");

	tb_IntegerToString(Date.minute,temp);

	if (Date.minute<10)
		strcat(dateAndTime,"0");

	strcat(dateAndTime,temp);

	strcat(dateAndTime,":");

	tb_IntegerToString(Date.second,temp);

	if (Date.second<10)
		strcat(dateAndTime,"0");

	strcat(dateAndTime,temp);

	strcat(dateAndTime," - ");

	if (NSpPlayer_GetIPAddress(gNetGame,playerID,&myIP)==kNMNoError)
	{
		strcat(dateAndTime,myIP);
		NSpPlayer_FreeAddress( gNetGame, (void **)&myIP );
	}

	//remove(filename);

	stream = fopen("IP Log", "a" );

	if (stream != NULL)
	{

		fseek( stream, 0L, SEEK_SET );

		fprintf( stream, "%s", dateAndTime);
		fprintf( stream, "%s", "\n");

		fclose( stream );

	}
	else
		fclose( stream );

}

// ----------------------------------------------------------------- //
void fi_Logout(int i)
// ----------------------------------------------------------------- //

{

	//#ifdef _SERVERONLY
	//return;
	//#endif
	
	pl_StablePets(i);

	fi_SavePlayerData(i);

	fi_UpdatePlayerLog(i, 1);

	numberOfPlayersOnline--;
	numberOfLogoffs++;

}

// ----------------------------------------------------------------- //
TBoolean fi_Logon(char userName[kStringLength], char password[kStringLength], playerType *player, creatureType *creature, TBoolean passwordRequired)
//TBoolean fi_Logon(char userName[15], char password[15], playerType *player, creatureType *creature, TBoolean passwordRequired)
// ----------------------------------------------------------------- //

{
	FILE									*stream;
	char									filename[kStringLength]; //[50];
	char									fpassword[kStringLength]; //[15];
	int										j, cpt;
	UInt32									inValue;
	char									inString[kStringLength];
	char									itName[kStringLength];
	char                  					debugStr[kStringLength];
	int                   					location;
	UInt32									oldMace, oldPole, oldDaggers, oldSwords;
	long									version;
	int										readUntil;

	//-- clear creature record --------------------------
	creature->id																=	0;
	creature->row																=	0;
	creature->col																=	0;
	creature->nextMoveTime											=	0;
	creature->dieTime     											=	0;
	creature->moveDirection											= 0;
	creature->hiding														= false;
	creature->alignment													= kGood;
	creature->strength													= 0;
	creature->intelligence											= 0;
	creature->dexterity													= 0;
	creature->strengthOffset										= 0;
	creature->intelligenceOffset								= 0;
	creature->regeneratingTime		=0;
	creature->regeneratingPower		=0;
	creature->dexterityOffset										= 0;
	creature->strengthTime											= 0;
	creature->intelligenceTime									= 0;
	creature->dexterityTime											= 0;
	creature->hitPoints													= 0;
	creature->magicPoints												= 0;
	creature->armorRating												= 0;
	creature->stamina														= 0;
	creature->specialAttackType											= -1;
	creature->specialAttackArg1											= -1;
	creature->specialAttackArg2											= -1;
	creature->playerIndex												= 0;
	creature->npcIndex													= 0;
	creature->facing														= 0;
	creature->strikePose												= 0;
	creature->nextStrikeTime										= 0;
	creature->attackTarget											= 0;
	creature->nextAttackTime										= 0;
	creature->missedLastHit											= true;
	creature->activeFX													= false;
	creature->finalStrikeSound                  = 0;
	creature->soundFX														= 0;
	creature->soundFXTime												= 0;
	creature->soundID												    = 0;
	creature->trackRow													= 0;
	creature->trackCol													= 0;
	creature->tracking													= false;
	creature->master														= 0;
	creature->command														= 0;
	creature->poison														= 0;
	creature->nextStatTime											= 0;
	creature->paralyzed													= 0;
	creature->reflectDamage											= 0;
	creature->disorientation										= 0;
	creature->lifespan										      = 0;
	creature->magicReflection										= false;
	creature->beingAttacked										  = 0;
	creature->hunger										        = 0;
	creature->range										          = 0;
	creature->startRow										      = 0;
	creature->startCol										      = 0;
	creature->respawn										        = false;
	creature->respawnTime										    = 0;
	creature->taunted												= 0;
	creature->level										          = 1;
	creature->cornered										      = false;
	creature->wounded										        = false;
	creature->home										          = 0;
	creature->skipSpell										      = 0;
	creature->nextTrackRow                      = 0;
	creature->nextTrackCol                      = 0;
	creature->possibleTarget                    = 0;
	creature->mute								=false;
	creature->petExp							=0;
	creature->noDrop							=false;
	creature->godMode							=false;
	creature->noDeath							=false;
	creature->autoLock							=false;
	creature->opportunityTime					=0;
	creature->damageReduc						=1.0f;

	for (j=0;j<kMaxCreatureEffects;j++)
	{
		creature->fx[j]													=	0;
		creature->fxTarget[j]										=	0;
		creature->fxTargetRow[j]								=	0;
		creature->fxTargetCol[j]								=	0;
	}

	for (j=0;j<kMaxInventorySize;j++)
	{
		creature->inventory[j].itemID						=	0;
		creature->inventory[j].itemCount				=	0;
	}

	creature->pathPtr=0;

	for (j=0;j<kMaxPath;j++)
	{
		creature->pathRow[j]										=	0;
		creature->pathRow[j]										=	0;
	}

	//----------------------------------------------------

	//-- clear player record -----------------------------
	player->active															=	false;

	player->online															= false;

	player->skipNextRefresh											=	false;

	player->sendStatsRefresh										=	false;

	player->sendPetsRefresh										  =	false;

	player->creatureIndex												=	0;

	player->id																	= 0;

	player->lastRefresh													= 0;

	player->thiefUntil													= 0;

	player->lastTalkRefresh											= 0;

	player->playerRefreshTime										= 0;

	player->previousStenchMessage								= 0;

	player->gathering														= 0;

	player->gatherType													= 0;

	player->gatherRow														= 0;

	player->gatherCol														= 0;

	player->strengthCounter											= 0;

	player->intelligenceCounter									= 0;

	player->dexterityCounter										= 0;

	player->meditating													= false;

	player->healing															= false;

	player->previousMessageID										=	9999999;

	//strcpy(player->talk, "");
	strcpy(player->playerName, "");

	strcpy(player->playerNameTemp, "None");

	strcpy(player->guildName, "None");

	strcpy(player->titleCustom, "None");

	player->accountNumber												= 0;

	//player->talkTime														=	0;
	//player->talkID														  =	0;
	player->category														=	0;

	player->god														      =	false;

	player->revealHidden												=	0;

	player->title																=	0;

	player->gender															=	0;

	player->router															=	true;

	player->totalRefresh                        = 0;

	player->stealthCool														=	0;

	player->stealth															=	false;

	player->meet																=	0;

	player->turnUntil														=	0;

	player->characterClass										  =	0;

	player->sharing										          = false;

	player->demo										            = true;

	player->helper										          = false;

	player->noGain										          = false;

	player->slow										            = false;

	player->morph										            = 0;

	player->newPlayer										        = true;

	player->paralyzeField										    = 0;

	player->poisonField										      = 0;
	
	player->creatureField										      = 0;
	
	player->fieldFrom										      = 0;

	player->speed												= 0;

	player->previousRow										      = 0;

	player->previousCol										      = 0;

	player->magicAttacker										    = 0;

	player->resistanceGained										= 0;

	player->normalAttacker										  = 0;

	player->fightingSkillGained								  = 0;

	player->lastSkillGain                       = 0;

	player->hairColor							= 0;

	player->hairStyle							= 0;

	player->stealthMoves						= 0;

	player->lastBackstabTarget					= 0;

	player->possibleBackstab					= false;

	player->noTrack								= false;
	
	player->npcDressup							= 0;
	
	player->speed								= 0;
	
	player->morphTime							= 0;

	for (j=0;j<kHeardSize;j++)
		player->heard[j]=0;

	for (j=0;j<kMaxPlayers;j++)
		player->sentName[j]=false;

	for (j=0;j<16;j++)
	{
		strcpy(player->friendName[j],"");
		player->friendID[j]=0;
	}

	for (j=0;j<kMaxMacroTrace;j++)
	{
		player->anatomyTarget[j]              = 0;
		player->tamingTarget[j]               = 0;
		player->detectingHiddenTarget[j]      = 0;
		player->thieveryTarget[j]             = 0;
	}

	player->lastHideRow                         = 0;

	player->lastHideCol                         = 0;

	player->lastTrackingRow                   = 0;
	player->lastTrackingCol                   = 0;

	for (j=0;j<kMaxPets;j++)
	{
		player->pet[j]													=	0;
		player->petInStable[j]									=	false;
		player->petType[j]											=	0;
		player->petExperience[j]								=	0;
		player->petHealth[j]								=	0;
		strcpy(player->petName[j],"");
		player->petLoyalty[j]                   = 0;
	}

	for (j=0;j<13;j++)
		player->arm[j]														=	0;

	for (j=0;j<kMaxSkills;j++)
	{
		player->skill[j].level									=	0;
		//player->skill[j].direction							=	0;
		//player->skill[j].counter								=	0;
	}

	for (j=0;j<kMaxInventorySize;j++)
	{
		player->bank[j].itemID									=	0;
		player->bank[j].itemCount								=	0;
	}

	for (j=0;j<kMaxTrail;j++)
	{
		player->rowTrail[j]											=	0;
		player->colTrail[j]											=	0;
	}

	//----------------------------------------------------

	strcpy(filename,":Accounts:");

	strcat(filename,userName);

	stream = fopen(filename, "r" );

	if (stream != NULL)
	{
		fseek( stream, 0L, SEEK_SET );

		fgets(fpassword,kStringLength,stream);

		if (strlen(fpassword)>3)
			version=0;
		else
		{
			fpassword[strlen(fpassword)-1]='\0';
			tb_StringToNum(fpassword, &version);
		}
		
		if (version<26)	//char too old
			return false;

		//fscanf( stream, "%s", fpassword);
		if (version!=0)
			fgets(fpassword,kStringLength,stream);

		fpassword[strlen(fpassword)-1]='\0';

		if ((strcmp(password,fpassword)!=0) && (passwordRequired))
		{
			fclose( stream );
			return (false);
		}

		//fscanf( stream, "%s", inString);
		fgets(inString,kStringLength,stream);

		inString[strlen(inString)-1]='\0';

		if (strlen(inString)>14)
		{
			fi_WriteToErrorLog("--------------------------");
			fi_WriteToErrorLog(filename);
			fi_WriteToErrorLog("Name too long");
			fi_WriteToErrorLog("--------------------------");
		}

		for (j=0;j<15;j++)
			player->playerName[j]=inString[j];

		player->playerName[14]='\0';

		if (version>20)
		{
			fgets(inString,kStringLength,stream);
			inString[strlen(inString)-1]='\0';

			if (strlen(inString)>14)
			{
				fi_WriteToErrorLog("--------------------------");
				fi_WriteToErrorLog(filename);
				fi_WriteToErrorLog("Temp Name too long");
				fi_WriteToErrorLog("--------------------------");
			}

			for (j=0;j<15;j++)
				player->playerNameTemp[j]=inString[j];

			player->playerNameTemp[14]='\0';
		}

		//fscanf( stream, "%s", inString);
		fgets(inString,kStringLength,stream);

		inString[strlen(inString)-1]='\0';

		if (strlen(inString)>20)
		{
			fi_WriteToErrorLog("--------------------------");
			fi_WriteToErrorLog(filename);
			fi_WriteToErrorLog("Guild name too long");
			fi_WriteToErrorLog("--------------------------");
		}

		for (j=0;j<20;j++)
			player->guildName[j]=inString[j];

		player->guildName[20]='\0';

		if (version>20)
		{
			fgets(inString,kStringLength,stream);
			inString[strlen(inString)-1]='\0';

			if (strlen(inString)>20)
			{
				fi_WriteToErrorLog("--------------------------");
				fi_WriteToErrorLog(filename);
				fi_WriteToErrorLog("Custom title too long");
				fi_WriteToErrorLog("--------------------------");
			}

			for (j=0;j<21;j++)
				player->titleCustom[j]=inString[j];

			player->titleCustom[20]='\0';
		}

		fscanf( stream, "%i", &inValue);		player->characterClass				=	(UInt8)inValue;

		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);		player->sharing				        =	(TBoolean)inValue;
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);		player->demo				          =	(TBoolean)inValue;
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);		player->newPlayer				      =	(TBoolean)inValue;
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);		player->accountNumber					=	(int)inValue;
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);		creature->row									=	(UInt16)inValue;
		fgets(inString,kStringLength,stream);

		if ((creature->row<0) || (creature->row>1999))
		{
			fclose( stream );
			fi_WriteToErrorLog("--------------------------");
			fi_WriteToErrorLog(filename);
			fi_WriteToErrorLog("Corrupt row location");
			fi_WriteToErrorLog("--------------------------");
			return (false);
		}

		fscanf( stream, "%i", &inValue);		creature->col									=	(UInt16)inValue;

		fgets(inString,kStringLength,stream);

		if ((creature->col<0) || (creature->col>1999))
		{
			fclose( stream );
			fi_WriteToErrorLog("--------------------------");
			fi_WriteToErrorLog(filename);
			fi_WriteToErrorLog("Corrupt col location");
			fi_WriteToErrorLog("--------------------------");
			return (false);
		}

		fscanf( stream, "%i", &inValue);		player->thiefUntil						=	TickCount()+(inValue*3600);

		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);		player->title									=	(int)inValue;
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);		player->gender								=	(UInt8)inValue;
		fgets(inString,kStringLength,stream);

		if (player->gender>1)
		{
			fclose( stream );
			fi_WriteToErrorLog("--------------------------");
			fi_WriteToErrorLog(filename);
			fi_WriteToErrorLog("Corrupt gender");
			fi_WriteToErrorLog("--------------------------");
			return (false);
		}

		fscanf( stream, "%i", &inValue);		player->category							=	(UInt8)inValue;

		fgets(inString,kStringLength,stream);

		if (player->category==4 || player->category==5)  //Qchars log in with noTrack
			player->noTrack=true;
		
		if (player->category!=0 && player->category!=8) //auto stealth on
			player->stealth=true;

		fscanf( stream, "%i", &inValue);		player->dead									=	(UInt8)inValue;

		fgets(inString,kStringLength,stream);

		if (player->dead>1)
		{
			fclose( stream );
			fi_WriteToErrorLog("--------------------------");
			fi_WriteToErrorLog(filename);
			fi_WriteToErrorLog("Corrupt dead field");
			fi_WriteToErrorLog("--------------------------");
			return (false);
		}

		fscanf( stream, "%i", &inValue);		player->curse									=	(UInt16)inValue;

		fgets(inString,kStringLength,stream);

		//fscanf( stream, "%s", inString);

		fgets(inString,kStringLength,stream);

		creature->mute						=(TBoolean)atoi(inString);

		fgets(inString,kStringLength,stream);


		creature->poison			=(UInt8)atoi(inString);

		fgets(inString, kStringLength, stream);

		player->hairColor=(UInt8)atoi(inString);

		if (player->hairColor==99) //Random hair coloring
		{
			j=tb_Rnd(1,3);

			if (j==1)
				player->hairColor=kBlack;
			else if (j==2)
				player->hairColor=kBrown;
			else if (j==3)
				player->hairColor=kYellow;
		}

		fgets(inString, kStringLength, stream);

		player->hairStyle=(UInt8)atoi(inString);

		fgets(inString, kStringLength, stream);

		for (j=0;j<13;j++)
		{
			fscanf( stream, "%i", &inValue);		player->arm[j]						=	(UInt8)inValue;
			fgets(inString,kStringLength,stream);
		}

		fscanf( stream, "%i", &inValue);		creature->hunger							=	(UInt16)inValue;

		fgets(inString,kStringLength,stream);

		//fscanf( stream, "%s", inString);
		fgets(inString,kStringLength,stream);

		fscanf( stream, "%i", &inValue);		creature->strength						=	(UInt8)inValue;
		fgets(inString,kStringLength,stream);
		fscanf( stream, "%i", &inValue);		creature->intelligence				=	(UInt8)inValue;
		fgets(inString,kStringLength,stream);
		fscanf( stream, "%i", &inValue);		creature->dexterity						=	(UInt8)inValue;
		fgets(inString,kStringLength,stream);
		fscanf( stream, "%i", &inValue);		creature->hitPoints						=	(float)inValue;
		fgets(inString,kStringLength,stream);
		fscanf( stream, "%i", &inValue);		creature->magicPoints					=	(UInt8)inValue;
		fgets(inString,kStringLength,stream);
		
		if (version>28)
		{
			fscanf( stream, "%i", &inValue);		creature->stamina					=	(UInt8)inValue;
			fgets(inString,kStringLength,stream);
		}
		else
			creature->stamina	= creature->dexterity;

		//fscanf( stream, "%s", inString);
		fgets(inString,kStringLength,stream);
		
		if (version<29)
			readUntil=32;
		else
			readUntil=kMaxSkills;


		for (j=0;j<readUntil;j++)
		{
			fscanf( stream, "%i", &inValue);

			if (version==28)
			{
				if (j<6)
					player->skill[j].level = (UInt32)inValue;

				if (j==6)
					oldDaggers=(UInt32)inValue;
				
				if (j==7)
					player->skill[j-1].level = (UInt32)inValue;

				if (j>7 && j<13)
					player->skill[j].level = (UInt32)inValue;

				if (j==13)
					oldMace=(UInt32)inValue;

				if (j>13 && j<18)
					player->skill[j-1].level = (UInt32)inValue;
				
				if (j>=18 && j<21)
				{
					player->skill[j].level = (UInt32)inValue;
					
					if (j==20) //Poisoning
						if (player->characterClass==6) //mage
							player->skill[kEnchanting].level= (UInt32)inValue;
				}

				if (j==21)
					oldPole=(UInt32)inValue;

				if (j>21 && j<24)
					player->skill[j-1].level = (UInt32)inValue;

				if (j==24)
					player->skill[j+2].level = (UInt32)inValue;

				if (j==25)
					oldSwords=(UInt32)inValue;

				if (j>25)
					player->skill[j+1].level = (UInt32)inValue;
			}
			else
				player->skill[j].level = (UInt32)inValue;

			fgets(inString,kStringLength,stream);
		}

		if (player->category==0 || player->category==8)
			for (j=0;j<kMaxSkills;j++)
			{
				if (player->skill[j].level>0 && sk_GetSkillLevel(player->skill[j].level)>maxSkillLevel[player->characterClass][j])
				{
					player->skill[j].level=100;

					for (cpt=2; cpt<maxSkillLevel[player->characterClass][j]; cpt++)
						player->skill[j].level*=2;

					player->skill[j].level--;
				}
			}

		if (version<29)
		{
			player->skill[kSpecialFighter].level=0;
			player->skill[kSpecialRanger].level=0;
			player->skill[kSpecialRogue].level=0;

			if ((player->category!=0 && player->category!=8)) //GM/Qchar
			{
				player->skill[kMelee].level=oldSwords;
				player->skill[kSpecialFighter].level=oldSwords;
				player->skill[kSpecialRanger].level=oldSwords;
				player->skill[kSpecialRogue].level=oldSwords;
				player->skill[kEnchanting].level=player->skill[kPoisoning].level;
			}
			else
			{
				if (player->characterClass==3)
				{
					//Fighter
					player->skill[kMelee].level=oldSwords+oldMace+oldPole/3;
					player->skill[kSpecialFighter].level=player->skill[kMelee].level;
				}

				if (player->characterClass==4)
				{
					//Special Ranger
					player->skill[kMelee].level=oldSwords;
					player->skill[kSpecialRanger].level=player->skill[kMelee].level;
				}

				if (player->characterClass==5)
				{
					//Special Rogue
					player->skill[kMelee].level=oldDaggers;
					player->skill[kSpecialRogue].level=player->skill[kPoisoning].level;
				}
			}
		}
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//Dave 
//fscanf( stream, "%s", inString);
		fgets(inString,kStringLength,stream);

		for (j=0;j<kMaxInventorySize;j++)
		{
			fscanf( stream, "%i", &inValue);		creature->inventory[j].itemID				=	(UInt16)inValue;
			fgets(inString,kStringLength,stream);
			fscanf( stream, "%i", &inValue);		creature->inventory[j].itemCount		=	(UInt16)inValue;;
			fgets(inString,kStringLength,stream);

			if (j>0)
				if (creature->inventory[j].itemID==0)
					creature->inventory[j].itemCount=0;
				
			if (j>0 && player->category==0 && it_IsSpecialItem(creature->inventory[j].itemID, creature->inventory[j].itemCount))
			{
				in_GetItemName(creature->inventory[j].itemID, itName, creature->inventory[j].itemCount);
				sprintf(debugStr, "Item Warning: %s has %d %s in their inventory.", userName, creature->inventory[j].itemCount, itName);
				fi_WriteToErrorLog(debugStr);
			}
		}

		//fscanf( stream, "%s", inString);
		fgets(inString,kStringLength,stream);

		for (j=0;j<kMaxInventorySize;j++)
		{
			fscanf( stream, "%i", &inValue);		player->bank[j].itemID				=	(UInt16)inValue;
			fgets(inString,kStringLength,stream);
			fscanf( stream, "%i", &inValue);		player->bank[j].itemCount			=	(UInt16)inValue;
			fgets(inString,kStringLength,stream);

			if (j>0)
				if (player->bank[j].itemID==0)
					player->bank[j].itemCount=0;
			
			if (j>0 && player->category==0 && it_IsSpecialItem(player->bank[j].itemID, player->bank[j].itemCount))
			{
				in_GetItemName(player->bank[j].itemID, itName, player->bank[j].itemCount);
				sprintf(debugStr, "Item Warning: %s has %d %s in their bank.", userName, player->bank[j].itemCount, itName);
				//fi_WriteToErrorLog(debugStr);
			}
		}

//Dave^
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
		//fscanf( stream, "%s", inString);
		fgets(inString,kStringLength,stream);

		for (j=0;j<kMaxPets;j++)
		{
			fscanf( stream, "%i", &inValue);		player->petType[j]						=	(UInt16)inValue;
			fgets(inString,kStringLength,stream);

			if (player->petType[j]!=0)
				player->petInStable[j]=true;

			fgets(inString,kStringLength,stream);

			inString[strlen(inString)-1]='\0';

			if (strlen(inString)<15)
				strcpy(player->petName[j],inString);
			else
				strcpy(player->petName[j],"None");

			if (player->petType[j]==0)
				strcpy(player->petName[j],"None");

			if (version>20)
			{
				fscanf( stream, "%i", &inValue);		player->petExperience[j]						=	(UInt32)inValue;
				fgets(inString,kStringLength,stream);

				if (player->petType[j]==0)
					player->petExperience[j]=0;
				
				if (version>29)
				{
					fscanf( stream, "%i", &inValue);		player->petHealth[j]						=	(UInt32)inValue;
					fgets(inString,kStringLength,stream);
				}
				else
				{
					player->petHealth[j]=5000; //max out
				}
			}
			else
			{
				if (player->petInStable[j])
				{
					if (creatureInfo[player->petType[j]].creatureLevel>1)
					{
						player->petExperience[j]=100;

						for (cpt=2; cpt<creatureInfo[player->petType[j]].creatureLevel; cpt++)
							player->petExperience[j]*=2;

						player->petExperience[j]+=tb_Rnd(10,20);
					}
				}
			}
		}

		fclose( stream );


		//------- Check data integrity -------

		for (j=1;j<13;j++)
			if (player->arm[j]>0)
			{
				location=j;

				if (location==kLeftFinger) location=kRightFinger;

				if (itemList[creature->inventory[player->arm[j]].itemID].useLocation!=location)	// check for armable in given location
				{
					fi_WriteToErrorLog("Bad Arm on Load");
					player->arm[j]=0;
				}
			}

		for (j=1;j<kMaxInventorySize;j++)
		{
			if (in_CanGroup(creature->inventory[j].itemID))
			{
				if (creature->inventory[j].itemCount>kMaxInventoryItemCount)
				{
					fi_WriteToErrorLog("--Inventory Error Max Count Error--");
					fi_WriteToErrorLog(player->playerName);
					tb_IntegerToString(creature->inventory[j].itemID,debugStr);
					fi_WriteToErrorLog(debugStr);
					tb_IntegerToString(creature->inventory[j].itemCount,debugStr);
					fi_WriteToErrorLog(debugStr);
					creature->inventory[j].itemCount=kMaxInventoryItemCount;
				}
			}
			else
			{
				if (creature->inventory[j].itemCount>255)
				{
					fi_WriteToErrorLog("--Inventory Error Max Condition Error--");
					fi_WriteToErrorLog(player->playerName);
					tb_IntegerToString(creature->inventory[j].itemID,debugStr);
					fi_WriteToErrorLog(debugStr);
					tb_IntegerToString(creature->inventory[j].itemCount,debugStr);
					fi_WriteToErrorLog(debugStr);
					creature->inventory[j].itemCount=255;
				}
			}
		}

		for (j=1;j<kMaxInventorySize;j++)
		{
			if (in_CanGroup(player->bank[j].itemID))
			{
				if (player->bank[j].itemCount>kMaxInventoryItemCount)
				{
					fi_WriteToErrorLog("--Bank Error Max Count Error--");
					fi_WriteToErrorLog(player->playerName);
					tb_IntegerToString(player->bank[j].itemID,debugStr);
					fi_WriteToErrorLog(debugStr);
					tb_IntegerToString(player->bank[j].itemCount,debugStr);
					fi_WriteToErrorLog(debugStr);
					player->bank[j].itemCount=kMaxInventoryItemCount;
				}
			}
			else
			{
				if (player->bank[j].itemCount>255)
				{
					fi_WriteToErrorLog("--Bank Error Max Condition Error--");
					fi_WriteToErrorLog(player->playerName);
					tb_IntegerToString(player->bank[j].itemID,debugStr);
					fi_WriteToErrorLog(debugStr);
					tb_IntegerToString(player->bank[j].itemCount,debugStr);
					fi_WriteToErrorLog(debugStr);
					player->bank[j].itemCount=255;
				}
			}
		}

		//------------------------------------

		return (true);

	}
	else
	{
		fclose( stream );
		return (false);
	}

}

// ----------------------------------------------------------------- //
void fi_UpdateCCData(int i, char oldName[kStringLength], char newName[kStringLength])
// ----------------------------------------------------------------- //
{
	FILE									*stream;
	char									fileName[kStringLength];
	char									inString[kStringLength];
	char									nameTest[kStringLength];
	char									reWrite[255][kStringLength];
	int 									cpt, cpt2, cpt3, cpt4;


	strcpy(fileName, ":CC Logs:");
	strcat(fileName, player[i].macID);
	strcat(fileName, " ");

	stream = fopen(fileName, "r+");

	if (stream != NULL)
	{
		fseek( stream, 0L, SEEK_SET );
		cpt3=0;

		while (!feof(stream))
		{
			strcpy(reWrite[cpt3], "");
			strcpy(inString, "");
			strcpy(nameTest, "");

			fgets(inString,kStringLength,stream);

			if (strcmp(inString, "")==0)
				break;

			cpt=0;

			while (inString[cpt]!='*' && inString[cpt]!='\n' && inString[cpt]!='\r')
			{
				reWrite[cpt3][cpt]=inString[cpt];
				reWrite[cpt3][cpt+1]='\0';
				cpt++;
			}

			reWrite[cpt3][cpt]='*';
			reWrite[cpt3][cpt+1]=' ';
			reWrite[cpt3][cpt+2]='\0';
			cpt+=2;
			cpt2=0;
			cpt4=cpt;

			while (inString[cpt]!='*' && inString[cpt]!='\n' && inString[cpt]!='\r')
			{
				nameTest[cpt2++]=inString[cpt];
				nameTest[cpt2]='\0';
				cpt++;
			}

			cpt--;

			nameTest[cpt2-1]='\0';

			if (strcmp(nameTest, oldName)==0)
				strcpy(nameTest, newName);

			strcat(reWrite[cpt3], nameTest);

			cpt4+=strlen(nameTest);

			while (inString[cpt]!='\n' && inString[cpt]!='\r')
			{
				reWrite[cpt3][cpt4++]=inString[cpt];
				reWrite[cpt3][cpt4]='\0';
				cpt++;
			}

			cpt3++;

			if (cpt3==255)
				break;
		}

		fseek( stream, 0L, SEEK_SET );

		for (cpt=0; cpt<cpt3; cpt++)
		{
			fprintf(stream, "%s", reWrite[cpt]);
			fprintf(stream, "%s", "\n");
		}
	}

	fclose(stream);
}


// ----------------------------------------------------------------- //
void fi_ScanForRares(void)
// ----------------------------------------------------------------- //

{
	FILE									*stream;
	char									filename[kStringLength]; //[50];
	creatureType 	        theCreature;
	playerType		        thePlayer;
	int                   j;

	return;

	stream = fopen(":Accounts:accounts", "r" );

	if (stream != NULL)
	{
		fseek( stream, 0L, SEEK_SET );

		while (true)
		{
			//fscanf( stream, "%s", filename);
			fgets(filename,kStringLength,stream);

			if (feof(stream))
			{
				fclose( stream );
				return;
			}

			if (fi_Logon(filename, "password", &thePlayer, &theCreature, false))
			{

				for (j=1;j<kMaxInventorySize;j++)
					if ((theCreature.inventory[j].itemID>0) && (theCreature.inventory[j].itemID<=kMaxItemsUsed))
						if (itemList[theCreature.inventory[j].itemID].desiredCount>0)
						{
							if (in_CanGroup(theCreature.inventory[j].itemID))
								itemList[theCreature.inventory[j].itemID].currentCount=itemList[theCreature.inventory[j].itemID].currentCount+theCreature.inventory[j].itemCount;
							else
								itemList[theCreature.inventory[j].itemID].currentCount++;
						}

				for (j=1;j<kMaxInventorySize;j++)
					if ((thePlayer.bank[j].itemID>0) && (thePlayer.bank[j].itemID<=kMaxItemsUsed))
						if (itemList[thePlayer.bank[j].itemID].desiredCount>0)
						{
							if (in_CanGroup(thePlayer.bank[j].itemID))
								itemList[thePlayer.bank[j].itemID].currentCount=itemList[thePlayer.bank[j].itemID].currentCount+thePlayer.bank[j].itemCount;
							else
								itemList[thePlayer.bank[j].itemID].currentCount++;
						}

			}
		}
	}

}

#endif