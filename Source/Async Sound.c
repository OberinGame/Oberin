#include "Async Sound.h"

#pragma mark Constants

#define kMaximumSoundChannels	8

#pragma mark Globals

extern short			MusicVolume;
extern short			SoundVolume;
extern short			SpeechVolume;

int        gSoundVolume;
//short							MusicVolume;
//short							SoundVolume;
//short							SpeechVolume;

SndListHandle 		theMusicSnd;
SndChannelPtr			gMusicChannel;
SndListHandle 		theSound[kMaximumSoundChannels];
SndChannelPtr			gSoundChannelArray[kMaximumSoundChannels];
Boolean						gSoundChannelPlaying[kMaximumSoundChannels];
short							gSoundPriority[kMaximumSoundChannels];

int							soundList[kMaxSoundFX][kMaxSoundTypes];

pascal void	SoundIsFinishedCallBack(SndChannelPtr theSoundChannel, SndCommand *infoRecord);

//----------------------------------------------------------------------------------
void	StopAsyncLoop(void)
//----------------------------------------------------------------------------------

{
//	OSErr				err = noErr;

//	err=SndDisposeChannel(gMusicChannel, true);
//	gMusicChannel = NULL;

}

//----------------------------------------------------------------------------------
void	SetMusicAmp(void)
//----------------------------------------------------------------------------------

{
//	SndCommand			soundCommand;

// 	soundCommand.cmd = ampCmd;
//	soundCommand.param1 = (UInt16)((MusicVolume-1)*36);
//	soundCommand.param2 = 0;
//	SndDoImmediate(gMusicChannel, &soundCommand);

}

//----------------------------------------------------------------------------------
OSErr	PlayAsyncLoop(short /*id*/)
//----------------------------------------------------------------------------------

{

}

//----------------------------------------------------------------------------------
void	InitializeAsyncSoundPlayer(void)
//----------------------------------------------------------------------------------

{
	short			i;
	short			j;
	OSErr			err;
	SndChannelPtr	newChannel;

	for (i = 0; i < kMaximumSoundChannels; i++)
	{
		gSoundChannelPlaying[i] = false;
		gSoundPriority[i] = 0;
		theSound[i]=NULL;
		newChannel = NULL;
		err = SndNewChannel(	&newChannel, 0, 0, NewSndCallBackUPP(SoundIsFinishedCallBack));
		//err = SndNewChannel(	&newChannel, 0, 0, NULL);
		gSoundChannelArray[i] = newChannel;
	}

	for (i = 0; i < kMaxSoundFX; i++)
		for (j = 0; j < kMaxSoundTypes; j++)
			soundList[i][j]=0;

	soundList	[kWolfHowl][0]								=	128;

	soundList	[kWolfHowl][1]								=	133;

	soundList	[kWolfAttack][0]							=	129;

	soundList	[kWolfAttack][1]							=	130;

	soundList	[kWolfAttack][2]							=	131;

	soundList	[kSeagull][0]									=	132;

	soundList	[kDragonGrowl][0]							=	138;

	soundList	[kDragonGrowl][1]							=	139;

	soundList	[kDragonGrowl][2]							=	140;

	soundList	[kDragonAttack][0]						=	145;

	soundList	[kDragonAttack][1]						=	146;

	soundList	[kDragonAttack][2]						=	147;

	soundList	[kSlugAttack][0]							=	148;

	soundList	[kSlugAttack][1]							=	149;

	soundList	[kSlugAttack][2]							=	150;

	soundList	[kSheepSound][0]							=	134;

	soundList	[kSheepSound][1]							=	135;

	soundList	[kSheepAttack][0]							=	134;

	soundList	[kHarpieAttack][0]						=	136;

	soundList	[kHarpieAttack][1]						=	137;

	soundList	[kWoodChop][0]						    =	142;

	soundList	[kWoodChop][1]						    =	143;

	soundList	[kWoodChop][2]						    =	144;

	soundList	[kArrowSnd][0]						    =	166;

	soundList	[kArrowSnd][1]						    =	167;

	soundList	[kArrowSnd][2]						    =	168;

	soundList	[kMetalMetalSnd][0]						=	169;

	soundList	[kMetalMetalSnd][1]						=	170;

	soundList	[kMetalMetalSnd][2]						=	171;

	soundList	[kMetalMetalSnd][3]						=	172;

	soundList	[kMetalNonMetalSnd][0]				=	173;

	soundList	[kMetalNonMetalSnd][1]				=	174;

	soundList	[kMetalNonMetalSnd][2]				=	175;

	soundList	[kMetalNonMetalSnd][3]				=	176;

	soundList	[kMetalMetalCritSnd][0]				=	251;

	soundList	[kMetalMetalCritSnd][1]				=	252;

	soundList	[kMetalMetalCritSnd][2]				=	253;

	soundList	[kMetalMetalCritSnd][3]				=	254;

	soundList	[kMetalNonMetalCritSnd][0]				=	241;

	soundList	[kMetalNonMetalCritSnd][1]				=	242;

	soundList	[kMetalNonMetalCritSnd][2]				=	243;

	soundList	[kMetalNonMetalCritSnd][3]				=	244;

	soundList	[kMissedSnd][0]				=	120;

	soundList	[kMissedSnd][1]				=	121;

	soundList	[kMissedSnd][2]				=	122;

	soundList	[kMissedSnd][3]				=	123;

	soundList	[kCubeAttack][0]				=	125;

	soundList	[kCubeAttack][1]				=	126;

	soundList	[kCubeAttack][2]				=	127;

	soundList	[kBlinkHoundAttack][0]				=	237;

	soundList	[kBlinkHoundAttack][1]				=	238;

	soundList	[kBlinkHoundAttack][2]				=	239;

	soundList	[kNonMetalSnd][0]						  =	177;

	soundList	[kNonMetalSnd][1]						  =	178;

	soundList	[kNonMetalSnd][2]						  =	179;

	soundList	[kNonMetalSnd][3]						  =	180;

	soundList	[kBoltSnd][0]						      =	181;

	soundList	[kExplosionSnd][0]						=	182;

	soundList	[kGaspSnd][0]						      =	183;

	soundList	[kMineSnd][0]						      =	184;

	soundList	[kMineSnd][1]						      =	185;

	soundList	[kMineSnd][2]						      =	186;

	soundList	[kWoodworkingSnd][0]					=	187;

	soundList	[kBlacksmithySnd][0]					=	188;

	soundList	[kBlacksmithySnd][1]					=	189;

	soundList	[kBlacksmithySnd][2]				  =	190;

	soundList	[kRatSqueak][0]							  =	191;

	soundList	[kRatSqueak][1]							  =	192;

	soundList	[kRatSqueak][2]							  =	193;

	soundList	[kRatAttack][0]						    =	194;

	soundList	[kRatAttack][1]						    =	195;

	soundList	[kRatAttack][2]						    =	196;

	soundList	[kLizardSnd][0]						    =	197;

	soundList	[kLizardSnd][1]						    =	198;

	soundList	[kLizardSnd][2]						    =	199;

	soundList	[kSpiderAttackSnd][0]					=	202;

	soundList	[kSpiderAttackSnd][1]					=	203;

	soundList	[kSpiderAttackSnd][2]					=	204;

	soundList	[kSnailAttackSnd][0]					=	205;

	soundList	[kSnailAttackSnd][1]					=	206;

	soundList	[kSnailAttackSnd][2]					=	207;

	soundList	[kTrollSnd][0]					      =	208;

	soundList	[kTrollSnd][1]					      =	209;

	soundList	[kTrollSnd][2]					      =	210;

	soundList	[kGiantSnd][0]					      =	211;

	soundList	[kGiantSnd][1]					      =	212;

	soundList	[kGiantSnd][2]					      =	213;

	soundList	[kZombieSnd][0]					      =	214;

	soundList	[kZombieSnd][1]					      =	215;

	soundList	[kBluntAttackSnd][0]					=	216;

	soundList	[kBluntAttackSnd][1]					=	217;

	soundList	[kBluntAttackSnd][2]					=	218;

	soundList	[kAntAttackSnd][0]						=	219;

	soundList	[kAntAttackSnd][1]						=	220;

	soundList	[kAntAttackSnd][2]						=	221;

	soundList	[kSerpentAttackSnd][0]				=	222;

	soundList	[kSerpentAttackSnd][1]				=	223;

	soundList	[kSerpentAttackSnd][2]				=	224;

	soundList	[kZombieAttackSnd][0]					=	178;

	soundList	[kZombieAttackSnd][1]					=	179;

	soundList	[kZombieAttackSnd][2]					=	180;
	
	soundList	[kThunderSnd][0]					    =	181;

	soundList	[kBatAttackSnd][0]					  =	228;

	soundList	[kBatAttackSnd][1]					  =	229;

	soundList	[kBatAttackSnd][2]					  =	227;

	soundList	[kQueenAntAttackSnd][0]					  =	231;

	soundList	[kQueenAntAttackSnd][1]					  =	232;

	soundList	[kSnakeAttackSnd][0]					  =	234;

	soundList	[kSnakeAttackSnd][1]					  =	235;

	soundList	[kSnakeSnd][0]					    	=	233;

	soundList	[kScreamSnd][0]					    =	226;

	soundList	[kBeepSnd][0]					        =	999;

	soundList	[kFanfareSnd][0]					    =	250;

	soundList	[kDracoAttackSnd][0]					=	302;

	soundList	[kDracoAttackSnd][1]					=	303;

	soundList	[kDracoAttackSnd][2]					=	304;

	soundList	[kDracoGrowlSnd][0]					=	305;

	soundList	[kTuskerAttackSnd][0]					=	306;

	soundList	[kTuskerAttackSnd][1]					=	307;

	soundList	[kTuskerAttackSnd][2]					=	308;

	soundList	[kFemaleGaspSnd][0]						=	309;

	soundList	[kSkillLevelSnd][0]						=	236;

	soundList	[kFireballSnd][0]						=	310;

	soundList	[kNonMetalCritSnd][0]				=	311;

	soundList	[kNonMetalCritSnd][1]				=	312;

	soundList	[kNonMetalCritSnd][2]				=	313;

	soundList	[kNonMetalCritSnd][3]				=	314;
	
	soundList	[kFluteSnd][0]					=	317;
	
	soundList	[kFluteSnd][1]					=	318;
	
	soundList	[kFluteSnd][2]					=	319;
	
	soundList	[kFluteSnd][3]					=	320;
	
	soundList	[kLichLaughSnd][0]						= 321;
	
	soundList	[kLichLaughSnd][1]						= 322;
	
	soundList	[kLichLaughSnd][2]						= 323;
	
	soundList	[kLockPickSnd][0]					= 324;
	
	soundList	[kLevelUpSnd][0]					= 315;
	
	soundList	[kMirithHornSnd][0]					= 325;
	
	soundList	[kLionAttackSnd][0]					= 326;

	soundList	[kLionAttackSnd][1]					= 327;

	soundList	[kLionAttackSnd][2]					= 328;
	
	soundList	[kDragonFlightSnd][0]				= 316;
	
	soundList	[kGolemAttackSnd][0]					= 329;

	soundList	[kGolemAttackSnd][1]					= 330;

	soundList	[kGolemAttackSnd][2]					= 331;
	
	soundList	[kDiceRollSnd][0]					= 332;
	
	soundList	[kPixieAttackSnd][0]					= 333;
	
	soundList	[kPixieAttackSnd][1]					= 334;
	
	soundList	[kPixieAttackSnd][2]					= 335;
	
	soundList	[kWaterDripSnd][0]						= 336;
	
	soundList	[kWaterDripSnd][1]						= 337;
	
	soundList	[kWaterDripSnd][2]						= 338;
	
	soundList	[kBearAttackSnd][0]						= 339;
	
	soundList	[kBearAttackSnd][1]						= 340;
	
	soundList	[kBearAttackSnd][2]						= 341;
	
	soundList	[kArrowCritSnd][0]						= 342;
	
	soundList	[kArrowCritSnd][1]						= 343;
	
	soundList	[kArrowCritSnd][2]						= 344;
	
	soundList	[kSharkAttackSnd][0]						= 345;
	
	soundList	[kSharkAttackSnd][1]						= 346;
	
	soundList	[kSharkAttackSnd][2]						= 347;
	
	soundList	[kSharkSnd][0]							= 348;
}

//----------------------------------------------------------------------------------
OSErr	PlayAsyncSound(int id)
//----------------------------------------------------------------------------------

{

	short							i;
	short							channelToUse = -1;
	OSErr							err = noErr;
	short							priority=0;
	//SndCommand				sCommand;
	int               volume;

#ifdef _SERVERONLY
	return;
#endif;

	if (gSoundVolume==0)
		return;

	for (i = 0; i < kMaximumSoundChannels; i++)
		if (gSoundChannelPlaying[i] == false)
			channelToUse = i;

	if (channelToUse != -1)
	{
		volume=gSoundVolume*36;

		if (theSound[channelToUse]!=NULL)
			HUnlock((Handle)theSound[channelToUse]);

		theSound[channelToUse] = (SndListHandle)GetResource('snd ', id);

		if (theSound[channelToUse]!=NULL)
		{
			SndChannelPtr		newChannel = NULL;
			HLock((Handle)theSound[channelToUse]);
			newChannel = gSoundChannelArray[channelToUse];

			if (err == noErr)
			{
				err = SndPlay(newChannel, theSound[channelToUse], true);

				if (err == noErr)
				{
					SndCommand		soundCommand;

					soundCommand.cmd = volumeCmd;
					soundCommand.param2 = (volume*65536)+volume;
					SndDoImmediate(newChannel, &soundCommand);

					gSoundChannelPlaying[channelToUse] = true;
					gSoundPriority[channelToUse] = priority;
					soundCommand.cmd = callBackCmd;
					soundCommand.param2 = (long)&gSoundChannelPlaying[channelToUse];
					err = SndDoCommand(newChannel, &soundCommand, false);
				}
			}
		}
	}

	return err;

}

//----------------------------------------------------------------------------------
pascal void	SoundIsFinishedCallBack(SndChannelPtr /*theSoundChannel*/, SndCommand *infoRecord)
//----------------------------------------------------------------------------------

{
	Boolean		*sndIsPlaying;

	sndIsPlaying = (Boolean *)infoRecord->param2;
	*sndIsPlaying = false;

}

//----------------------------------------------------------------------------------
short	HowManySoundsArePlaying(void)
//----------------------------------------------------------------------------------

{
	short	numSoundsPlaying = 0;
	short	i;

	for (i = 0; i < kMaximumSoundChannels; i++)
	{
		if (gSoundChannelPlaying[i] == true)
			numSoundsPlaying += 1;
	}

	return numSoundsPlaying;
}

//----------------------------------------------------------------------------------
void	CleanUpSounds(Boolean stopImmediately)
//----------------------------------------------------------------------------------

{
	short	i;

	for (i = 0; i < kMaximumSoundChannels; i++)
	{
		if (gSoundChannelArray[i] != NULL)
		{
			SndDisposeChannel(gSoundChannelArray[i], stopImmediately);
			gSoundChannelArray[i] = NULL;
		}
	}

}


