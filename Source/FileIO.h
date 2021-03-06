#ifndef _FILEIO
#define _FILEIO

#include "Keys.h"
#include "Remap.h"
#include "Players.h"
#include "Creatures.h"
#include "Common.h"

extern int									rowOffset;
extern int 									colOffset;
extern UInt16 							clientMap[kMapWidth][kMapHeight];

void 			fi_GetGMSettings				(void);
void 			fi_SavePlayerData				(int i);
TBoolean		fi_Logon						(char userName[kStringLength], char password[kStringLength], playerType *player, creatureType *creature, TBoolean passwordRequired);
void 			fi_Logout						(int i);
void 			fi_UpdatePlayerLog						(int i, int logStatus);
void 			fi_OpenConversationLog				(void);
void 			fi_WriteToConversationLog			(int	i, char	playerText[kStringLength]);
void 			fi_CloseConversationLog				(void);
void      fi_WriteToErrorLog            (char errorMessage[kStringLength]);
void      fi_WriteNumberToErrorLog      (int number); //[128])
void      fi_ScanForRares               (void);
void      fi_WriteToBugLog              (int i, char errorMessage[kStringLength]);
void      fi_WriteToTextLog             (char errorMessage[kStringLength]);
void      fi_WriteToDebuggerLog         (char errorMessage[kStringLength]); //[128])
void      fi_WriteToQuestFile           (int number, int gold, int experience, int item, int count, char quest[1024]); //[128])
TBoolean  fi_ReadQuestFile              (int number, int *gold, int *experience, int *item, int *count, char quest[1024]); //[128])
void      fi_LoadContainer              (int type, int row, int col);
void      fi_SaveContainer              (int row, int col);
void      fi_DumpInData                 (void); //[128])
void      fi_OpenGMLog                  (void);
void      fi_WriteToGMLog               (int i, char	playerText[kStringLength]);
void      fi_WriteToCSLog               (char	csMessage[kStringLength]); //Creature spawning
void      fi_CloseGMLog                 (void);
void      fi_WriteToMacFile             (char macMessage[kStringLength], int c); //[128])
void      fi_GetAKA                     (int i, int c); //[128])
void 			fi_AddToBanFile								(int i, int c); //[128])
void 			fi_LoadBanFile								(void); //[128])
void      fi_DumpTestData               (int length); //[128])
TBoolean  fi_IsCharacterCreatorUp       (void);
int       fi_TurnCharacterCreatorOn     (void);
int       fi_TurnCharacterCreatorOff    (void);

void 			fi_OpenMapFile								(void);
void 			fi_LoadClientMap							(int row, int col);
void 			fi_CloseMapFile								(void);
void      fi_ScanFile                   (char name[kStringLength]); //[128])
void      fi_SaveAllContainers          (void);
void      fi_LoadAllContainers          (void);
void      fi_SaveSignMessage            (int row, int col, char signStr[kStringLength]);
void      fi_LoadSignMessage            (int row, int col, char signStr[kStringLength]);
int 			fi_GetData										(char filename[kStringLength], char searchFor[kStringLength], int defaultValue);
TBoolean  fi_GetBoolean									(char filename[kStringLength], char searchFor[kStringLength], TBoolean defaultValue);
void 			fi_GetCreatureTalkScript			(char filename[kStringLength], int theCreature);
void 			fi_GetCreatureInventory				(char filename[kStringLength], int theCreature);
void 			fi_GetCreatureSpawns					(void);
void 			fi_GetBankSettings						(void);
void 			fi_GetTempleSettings					(void);
void 			fi_GetShopSettings						(void);
void 			fi_GetStableSettings					(void);
void 			fi_GetNPCSettings							(void);
void 			fi_GetLadderSettings					(void);
void 			fi_UpdateActivityLog					(int i);
void 			fi_UpdateIPLog								(NSpPlayerID playerID);
void 			fi_GetPortalSettings					(void);
void 			fi_GetWeaponModifier					(char filename[kStringLength], int theCreature);
void 			fi_GetWeaponSettings					(void);
void 			fi_StartDebugTimer						(void); //[128])
void 			fi_CheckDebugTimer						(char location[kStringLength]); //[128])
void 			fi_GetItemSettings						(void);
void 			fi_GetItemSettings						(void);
void 			fi_SendLoginMessage						(int i);
void 			fi_WriteToTraceLog						(char errorMessage[kStringLength]); //[128])
void 			fi_SpawnChests							(void);
void			fi_SaveGuildHall						(guildHallType);
void			fi_LoadGuildHall						(int row, int col);
void			fi_LoadGuildHalls						(void);
void			fi_SaveAllGuildHalls					(void);
void			fi_LoadMacros							(void);
void 			fi_GetSpellList							(char filename[kStringLength], int c);
void			fi_UpdateCCData							(int j, char oldName[kStringLength], char newname[kStringLength]);
#endif