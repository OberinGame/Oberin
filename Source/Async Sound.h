#pragma once

#ifndef _ASYNC_SOUND_
#define _ASYNC_SOUND_

#include <Sound.h>
#include <Resources.h>


#define kMaxSoundFX						100
#define kMaxSoundTypes					4

#define kWolfHowl						1
#define kWolfAttack						2
#define kSeagull						3
#define kDragonGrowl					4
#define kDragonAttack					5
#define kSlugAttack						6
#define kSheepSound						7
#define kSheepAttack					8
#define kHarpieAttack					9
#define kWoodChop					    10
#define kArrowSnd					    11
#define kMetalMetalSnd					12
#define kMetalNonMetalSnd			    13
#define kNonMetalSnd					14
#define kBoltSnd					    15
#define kExplosionSnd					16
#define kGaspSnd					    17
#define kMineSnd					    18
#define kWoodworkingSnd					19
#define kBlacksmithySnd					20
#define kRatSqueak					    21
#define kRatAttack					    22
#define kLizardSnd					    23
#define kSpiderAttackSnd				24
#define kSnailAttackSnd				    25
#define kTrollSnd				        26
#define kGiantSnd				        27
#define kZombieSnd				        28
#define kBluntAttackSnd				    29
#define kAntAttackSnd				    30
#define kSerpentAttackSnd				31
#define kZombieAttackSnd				32
#define kBatAttackSnd				    33
#define kBeepSnd				        34
#define kThunderSnd				        35
#define kFanfareSnd				        36
#define	kScreamSnd						37
#define kSnakeSnd						38
#define kQueenAntAttackSnd				39
#define kSnakeAttackSnd					40
#define	kMissedSnd						41
#define kMetalMetalCritSnd				42
#define kMetalNonMetalCritSnd			43
#define kCubeAttack						44
#define kBlinkHoundAttack				45
#define kDracoAttackSnd					46
#define kDracoGrowlSnd					47
#define kTuskerAttackSnd				48
#define	kFemaleGaspSnd					49
#define	kSkillLevelSnd					50
#define kFireballSnd					51
#define	kNonMetalCritSnd				52
#define	kLichLaughSnd					53
#define kLockPickSnd					54
#define kFluteSnd						55
#define	kMirithHornSnd					56
#define	kLionAttackSnd					57
#define kLevelUpSnd						58
#define kDragonFlightSnd				59
#define	kGolemAttackSnd					60
#define	kDiceRollSnd					61
#define	kPixieAttackSnd					62
#define	kWaterDripSnd					63
#define	kBearAttackSnd					64
#define	kArrowCritSnd					65
#define	kSharkAttackSnd					66
#define	kSharkSnd						67

extern int							soundList[kMaxSoundFX][kMaxSoundTypes];

void	SetMusicAmp(void);
void	StopAsyncLoop(void);
OSErr	PlayAsyncLoop(short id);
void	InitializeAsyncSoundPlayer(void);
OSErr	PlayAsyncSound(int id);
short	HowManySoundsArePlaying(void);
void	CleanUpSounds(Boolean stopImmediately);
short	HowManySoundsArePlaying(void);

#endif

