#include "FX.h"
#include "Globals.h"
#include "Network.h"
#include "Creatures.h"
#include "Skills.h"
#include "FileIO.h"
#include "Items.h"

////////////////////////////////////////////////////////////////////
/*
The special effects contained in this file are effects that need
to maintain their state information between frames.  These are
different from the simpler effects contained in our toolbox. The states
of the effects in our toolbox are initialized each time the effect
function is called.  For example, the random intensity of the light
source effect.  By contrast, the effects in this file get intialized
once and then their state changes from frame to frame until the effect
dies out or it is disposed of.

The effects in this file use standar toolbox calls including the new
special effects toolbox functions.

Note: The effects in this file do not allocate dynamic memory. The
"create" functions simply initialize the effect variables and tag the
effect as active.  The "dispose" functions simply tag the effect as
in active.
*/
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////
TFXSparklesData 				fxSparkle						[_MAXEFFECTS];
TFXLightFlash   				fxFlash							[_MAXEFFECTS];
TFXParticleExplosion 		fxParticleExplosion	[_MAXEFFECTS];
TFXParticleHose					fxParticleHose			[_MAXEFFECTS];
TFXFire									fxFire							[_MAXEFFECTS];
int								fxFrames[kMaxFX];

#ifdef _SERVER
UInt8									fxMap								[kMapWidth][kMapHeight];
UInt8									serverFX						[kMaxFX];
unsigned long							serverFXKillTime		[kMaxFX];
UInt16									serverFXRow					[kMaxFX];
int										serverFXMisc				[kMaxFX];
int										serverFXMisc2				[kMaxFX];
UInt16									serverFXCol					[kMaxFX];
UInt8									dayNight;

extern	unsigned long			dayBegun;
#endif


#ifdef _SERVER
// ----------------------------------------------------------------- //
TBoolean fx_MovingFX(int fx)
// ----------------------------------------------------------------- //
{
	if (fx>100 && fx<112)	return true; //fireball
	if (fx>112 && fx<124)	return true; //MM
	
	return false;
}

// ----------------------------------------------------------------- //
void fx_ServiceFX(void) // server
// ----------------------------------------------------------------- //

{
	int								i;
	unsigned long			currentTime=TickCount();

#ifdef _FUNCTIONTRACEDEBUGGING
	fi_WriteToErrorLog("fx_ServiceFX Entry");
#endif

	for (i=1;i<kMaxFX;i++)
	{   
		/*if (currentTime>serverFXKillTime[i])
		  if (!fx_MovingFX(serverFX[i]))
			if (serverFXRow[i]>=0)
				if (serverFXRow[i]<kMapWidth)
					if (serverFXCol[i]>=0)
						if (serverFXCol[i]<kMapHeight)
						{
							fxMap[serverFXRow[i]][serverFXCol[i]]=0;
							serverFXRow[i]				=0;
							serverFXCol[i]				=0;
						}*/
						
						
		if ((serverFX[i]!=0) && (serverFXKillTime[i]!=0))
			if (currentTime>serverFXKillTime[i])
			{
				if (serverFX[i]==kFireballX)
				{
					if (serverFXMisc[i]!=0) //Fireball ended route
					{
						fx_CreatureServerFX(serverFXMisc2[i], serverFXMisc2[i], kFireballX2, 4*60, 0);
						sk_CalculateDamage(kFireballSpell, serverFXMisc[i], serverFXMisc2[i]);
					}

					if (serverFXRow[i]>=0)
						if (serverFXRow[i]<kMapWidth)
							if (serverFXCol[i]>=0)
								if (serverFXCol[i]<kMapHeight)
									fxMap[serverFXRow[i]][serverFXCol[i]]=0;

					serverFX[i]					=0;

					serverFXKillTime[i]			=0;

					serverFXRow[i]				=0;

					serverFXCol[i]				=0;

					serverFXMisc[i]				=0;

					serverFXMisc2[i]			=0;
				}
				else if (serverFX[i]>kFireballX && serverFX[i]<kFireballX2)
				{
					serverFX[i]--;
					serverFXKillTime[i]=currentTime+15;
				}
				else if (serverFX[i]==kMMX)
				{
					if (serverFXMisc[i]!=0) //Magic Missile ended route
					{
						fx_CreatureServerFX(serverFXMisc2[i], serverFXMisc2[i], kMMX2, 1*60, 0);
						sk_CalculateDamage(kMagicMissileSpell, serverFXMisc[i], serverFXMisc2[i]);
					}

					if (serverFXRow[i]>=0)
						if (serverFXRow[i]<kMapWidth)
							if (serverFXCol[i]>=0)
								if (serverFXCol[i]<kMapHeight)
									fxMap[serverFXRow[i]][serverFXCol[i]]=0;

					serverFX[i]					=0;

					serverFXKillTime[i]			=0;

					serverFXRow[i]				=0;

					serverFXCol[i]				=0;

					serverFXMisc[i]				=0;

					serverFXMisc2[i]			=0;
				}
				else if (serverFX[i]>kMMX && serverFX[i]<kMMX2)
				{
					serverFX[i]--;
					serverFXKillTime[i]=currentTime+5;
				}
				else
				{
					if (serverFXRow[i]>=0)
						if (serverFXRow[i]<kMapWidth)
							if (serverFXCol[i]>=0)
								if (serverFXCol[i]<kMapHeight)
									fxMap[serverFXRow[i]][serverFXCol[i]]=0;

					serverFX[i]					=0;

					serverFXKillTime[i]			=0;

					serverFXRow[i]				=0;

					serverFXCol[i]				=0;

					serverFXMisc[i]				=0;

					serverFXMisc2[i]			=0;
				}
			}
	}

#ifdef _FUNCTIONTRACEDEBUGGING
	fi_WriteToErrorLog("fx_ServiceFX Exit");

#endif

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void fx_CreatureServerFX(int cc, int ct, int type, int life, int misc) // server
// ----------------------------------------------------------------- //

{
	int						j;
	int						fxIndex;

	if (creature[cc].playerIndex!=0)
		if (player[creature[cc].playerIndex].stealth)
			if (type!=kLightExplosion)
				return;

	if (creature[ct].playerIndex!=0)
		if (player[creature[ct].playerIndex].stealth)
			if (type!=kLightExplosion)
				return;

	for (j=0;j<kMaxCreatureEffects;j++)
		if (creature[cc].fx[j]==0)
		{
			fxIndex=fx_AddServerFX(type,life,0,0,misc,0);

			if (fxIndex>0)
			{
				creature[cc].fx[j]			=	fxIndex;
				creature[cc].fxTarget[j]	=	ct;
				creature[cc].fxKillTime[j]	=	serverFXKillTime[fxIndex];
				creature[cc].fxTargetRow[j]	=	creature[ct].row;
				creature[cc].fxTargetCol[j]	=	creature[ct].col;
				creature[cc].activeFX		=	true;
			}

			break;
		}

}

// ----------------------------------------------------------------- //
int fx_AddServerFX(int type, int life, int row, int col, int misc, int misc2) // server
// ----------------------------------------------------------------- //

{
	int							i;
	unsigned long				currentTime=TickCount();
	
	if ((row<0) || (row>=kMapWidth) || (col<0) || (col>=kMapHeight))			// beta debug
		fi_WriteToErrorLog("Out of Range - fx_AddServerFX");

	if (!((row==0) && (col==0)))
		if (fxMap[row][col]!=type && fxMap[row][col]!=0)
			return (0);

	for (i=1;i<kMaxFX;i++)
		if (serverFX[i]==0)
		{
			serverFX[i]						= type;

			if (life==0)
				serverFXKillTime[i]	= 0;
			else
				serverFXKillTime[i]	= currentTime+life;

			serverFXRow[i]				= row;

			serverFXCol[i]				= col;

			serverFXMisc[i]				= misc;

			serverFXMisc2[i]			= misc2;
			
			if (row!=0 && col!=0)	fxMap[row][col]=i;
						
			return (i);
		}

	return (0);

}

#endif

////////////////////////////////////////////////////////////////////
// Call this once to initialize the global variables
void	fx_Initialize(void)
{
	int				i;
	int				x;
	int				y;

	for (i=0; i<_MAXEFFECTS; i++)
	{
		fxSparkle[i].Active = false;
		fxFlash[i].Active = false;
		fxParticleExplosion[i].Active = false;
		fxParticleHose[i].Active = false;
		fxFire[i].Active = false;
	}

	for (i=0; i<kMaxFX; i++)
		fxFrames[i]=0;

	fxFrames[kFireblastX]=9;

	fxFrames[kMMX]=12;

	fxFrames[kMMX2]=12;

	fxFrames[kFireblast3X]=12;

	fxFrames[kFireblast4X]=12;

	fxFrames[kFireblast5X]=12;

	fxFrames[kFireballX]=20;

	fxFrames[kFireballX2]=20;

	fxFrames[kMassHealX]=14;

	fxFrames[kDomeX]=21;

	fxFrames[kDome2X]=21;

	fxFrames[kExplosionX]=19;

	fxFrames[kBuffStrX]=30;

	fxFrames[kBuffAgiX]=30;

	fxFrames[kBuffMindX]=30;

	fxFrames[kBuffMRX]=30;

	fxFrames[kBuffRDX]=30;
	
	fxFrames[kParaFX]=30;

	fxFrames[kResX]=23;
		
	fxFrames[kCreatureSummonX]=7;
	
	fxFrames[kFBFieldX]=6;
	
	fxFrames[kSpatSwordX]=7;
	
	fxFrames[kBrackenRootX]=1;

#ifdef _SERVER

	for (x=0;x<kMapWidth;x++)
		for (y=0;y<kMapHeight;y++)
			fxMap[x][y]=0;

	for (i=0;i<kMaxFX;i++)
	{
		serverFX[i]=0;
		serverFXKillTime[i]=0;
		serverFXRow[i]=0;
		serverFXCol[i]=0;
		serverFXMisc[i]=0;
		serverFXMisc2[i]=0;
	}

#endif


}

////////////////////////////////////////////////////////////////////
/*	This function creates (initializes) a sparkle effect.

	x,y:	center of sparkle effect in screen coordinates
	r:		radius of effect (no sparkles will be created outside this radius)
	color:	Use one of the defines in FX.h to specify the main color, i.e., red, green or blue
	num:	the number of sparkles in the effect (100 or less)
	size:	the radius of each individual sparkle
	motion:	use one of the defines in FX.h as follows:
				kfxStill:	the sparkles don't move, they'll just appear and then go away
				kfxJitter:	the sparkles will flutter about randomly (kind of looks like flies)
				kfxRise:	the sparkles will slowly rise up (like bubbles or fumes)
				kfxFall:	the sparkles will slowly fall (like dust settling)

  This function will return the index to the new sparkle effect.
*/

int	fx_CreateSparkles(int x, int y, int r, int color, int num, int size, int motion)
{
	int	i;
	int	index = -1;

	for (i=0; i<_MAXEFFECTS; i++)
	{
		if (!fxSparkle[i].Active)
		{
			index = i;
			break;
		}
	}

	if (index > -1)
	{
		fxSparkle[index].Active = true;
		fxSparkle[index].x = x;
		fxSparkle[index].y = y;
		fxSparkle[index].r = r;
		fxSparkle[index].color = color;
		fxSparkle[index].num = num;
		fxSparkle[index].size = size;
		fxSparkle[index].start = true;
		fxSparkle[index].motion = motion;
	}

	return index;
}


////////////////////////////////////////////////////////////////////
// Mark the given sparkle effect as inactive
void fx_DisposeSparkles(int index)
{
	if ((index >= 0)  && (index < _MAXEFFECTS))
		fxSparkle[index].Active = false;
}

////////////////////////////////////////////////////////////////////
// Draw the sparkle effect to the given bitmap.
// Each time this function is called the sparkle effect's state is updated.
// The effect dies out in after 24 calls at which point the function returns
// FALSE.
TBoolean	fx_DrawSparkles(TBitmap bitmap, int index, int nx, int ny)
{
	int			i;
	TBoolean	done = true;
	int			x, y;
	float		r, a;
	int			m;
	TBoolean	retval = true;
	int			dx, dy;


	if (fxSparkle[index].start)
	{
		for (i=0; i<fxSparkle[index].num; i++)
		{
			r = (float) tb_Rnd(0,fxSparkle[index].r);
			a = (float) tb_Rnd(0, 360);
			x = tb_Round(r * cos(a*3.14/180));
			y = tb_Round(r * sin(a*3.14/180));

			fxSparkle[index].xi[i] = fxSparkle[index].x+x;
			fxSparkle[index].yi[i] = fxSparkle[index].y+y;
			fxSparkle[index].c[i] = tb_Rnd(4, 14);
			fxSparkle[index].d[i] = tb_Rnd(0, 8);
			fxSparkle[index].start = false;
		}
	}

	dx=nx-fxSparkle[index].x;

	dy=ny-fxSparkle[index].y;

	fxSparkle[index].x=nx;
	fxSparkle[index].y=ny;

	for (i=0; i<fxSparkle[index].num; i++)
	{
		fxSparkle[index].xi[i]+=dx;
		fxSparkle[index].yi[i]+=dy;
	}

	for (i=0; i<fxSparkle[index].num; i++)
	{
		if (fxSparkle[index].d[i]>0)
			fxSparkle[index].d[i]--;
		else if (fxSparkle[index].c[i]>0)
		{
			done = false;
			fxSparkle[index].c[i]--;

			if (fxSparkle[index].motion == kfxJitter)
			{
				m = tb_Rnd(0,1);

				if (m==1)
					fxSparkle[index].dx = 1;
				else
					fxSparkle[index].dx = -1;

				m = tb_Rnd(0,1);

				if (m==1)
					fxSparkle[index].dy = 1;
				else
					fxSparkle[index].dy = -1;

				fxSparkle[index].xi[i]+=fxSparkle[index].dx;

				fxSparkle[index].yi[i]+=fxSparkle[index].dy;
			}
			else if (fxSparkle[index].motion == kfxRise)
			{
				fxSparkle[index].yi[i]--;
			}
			else if (fxSparkle[index].motion == kfxFall)
			{
				fxSparkle[index].yi[i]++;
			}

			tb_RenderColoredLightSource(	bitmap,

										 fxSparkle[index].xi[i],
										 fxSparkle[index].yi[i],
										 fxSparkle[index].size,
										 fxSparkle[index].color, 100, gameTime);

		}
		else if (fxSparkle[index].Active)
		{
			r = (float) tb_Rnd(0,fxSparkle[index].r);
			a = (float) tb_Rnd(0, 360);
			x = tb_Round(r * cos(a*3.14/180));
			y = tb_Round(r * sin(a*3.14/180));

			fxSparkle[index].xi[i] = fxSparkle[index].x+x;
			fxSparkle[index].yi[i] = fxSparkle[index].y+y;
			fxSparkle[index].c[i] = tb_Rnd(4, 14);
			fxSparkle[index].d[i] = tb_Rnd(0, 8);
		}
	}

	if (done && fxSparkle[index].Active) fxSparkle[index].start = true;

	if (done && !fxSparkle[index].Active) retval = false;

	return retval;
}

////////////////////////////////////////////////////////////////////
/*	This function creates a new flash effect, which is a quickly expanding
    light source.

	x,y:	center of the effect in screen coordinates
	w,h:	width and height of the effect. For elliptical effects
			w and h should be different, whereas for circular effects
			h is ingnored and w is used as the RADIUS.
	color:	use one of the defines in FX.h to specify a color
	type:	use one of the defines in FX.h to specify the type of this effect:

				kfxLightSource:			expanding circular light source
				kfxLightRing:			expanding circular ring of light
				kfxLightEllipse:		expanding elliptical light source
				kfxLightEllipseRing:	expanding elliptical ring of light

	Returns an index to the newly initialized flash effect.
*/
int		fx_CreateFlash(int x, int y, int w, int h, int color, int type)
{
	int	i;
	int	index = -1;

	for (i=0; i<_MAXEFFECTS; i++)
	{
		if (!fxFlash[i].Active)
		{
			index = i;
			break;
		}
	}

	if (index > -1)
	{
		fxFlash[index].Active = true;
		fxFlash[index].x = x;
		fxFlash[index].y = y;
		fxFlash[index].radius = w;
		fxFlash[index].w = w;
		fxFlash[index].h = h;
		fxFlash[index].color = color;
		fxFlash[index].start = true;
		fxFlash[index].type = type;
	}

	return index;
}

////////////////////////////////////////////////////////////////////
// Tag the given flash effect as inactive
void	fx_DisposeFlash(int index)
{
	if ((index >= 0)  && (index < _MAXEFFECTS))
		fxFlash[index].Active = false;
}

////////////////////////////////////////////////////////////////////
// Draw the flash effect to the given bitmap. This function returns
// FALSE whent the flash effect is complete, which occurs after 4 steps
// (animation frames)
TBoolean	fx_DrawFlash(TBitmap bitmap, int index, int nx, int ny)
{
	TBoolean	retval = true;


	if (!fxFlash[index].Active) return false;

	fxFlash[index].x=nx;

	fxFlash[index].y=ny;

	if (fxFlash[index].start)
	{
		fxFlash[index].wsize = 0;
		fxFlash[index].hsize = 0;
		fxFlash[index].dw = fxFlash[index].w/4;
		fxFlash[index].dh = fxFlash[index].h/4;
		fxFlash[index].start = false;
		fxFlash[index].step = 0;
		fxFlash[index].level = 150;
	}

	fxFlash[index].step++;

	if (fxFlash[index].step>2)
		fxFlash[index].level-=25;

	if (fxFlash[index].level <=10)
		fxFlash[index].level = 10;

	fxFlash[index].wsize += fxFlash[index].dw;

	fxFlash[index].hsize += fxFlash[index].dh;

	if (fxFlash[index].step>4)
	{
		if (fxFlash[index].Active)
		{
			fxFlash[index].start = true;
			retval = true;
		}
		else
			retval = false;
	}


	if (fxFlash[index].type == kfxLightSource)
		tb_RenderColoredLightSource(	bitmap,
									 fxFlash[index].x,
									 fxFlash[index].y,
									 fxFlash[index].wsize,
									 fxFlash[index].color,
									 fxFlash[index].level, gameTime);

	if (fxFlash[index].type == kfxLightRing)
		tb_RenderLightRing(	bitmap,
							fxFlash[index].x,
							fxFlash[index].y,
							fxFlash[index].wsize,
							15*fxFlash[index].step,
							fxFlash[index].color,
							fxFlash[index].level, gameTime);

	if (fxFlash[index].type == kfxLightEllipse)
		tb_RenderLightEllipse(	bitmap,
							   fxFlash[index].x,
							   fxFlash[index].y,
							   fxFlash[index].wsize,
							   fxFlash[index].hsize,
							   fxFlash[index].color,
							   fxFlash[index].level, gameTime);

	if (fxFlash[index].type == kfxLightEllipseRing)
		tb_RenderLightRingEllipse(	bitmap,
								   fxFlash[index].x,
								   fxFlash[index].y,
								   fxFlash[index].wsize,
								   fxFlash[index].hsize,
								   15*fxFlash[index].step,
								   fxFlash[index].color,
								   fxFlash[index].level, gameTime);


	return retval;

}


// ----------------------------------------------------------------- //
void	fx_DrawFireball(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	tb_RenderBitmapFrame(bitmap, kfxFireballID, x, y, tot, fr, 62965, 55255, 13107, 100);
	//tb_RenderBitmapFrame(bitmap, kfxFireballID, x, y, tot, fr, 57054, 11565, 4626, 100);
}


// ----------------------------------------------------------------- //
void	fx_MagicMissile(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	int r,g,b;
	int fra;

	//char debug[255];

	if (fr==0)
	{
		fra=tb_Rnd(0,10);
	}
	else
		fra=fr;

	if (fra==1 || fra==7)
	{
		r=57825;
		g=23644;
		b=21588;
	}
	else if (fra==3 || fra==9)
	{
		r=27242;
		g=29298;
		b=58339;
	}
	else if (fra==5)
	{
		r=35466;
		g=57825;
		b=27756;
	}
	else
	{
		r=65535;
		g=65535;
		b=65535;
	}

	tb_RenderBitmapFrame(bitmap, kfxMMID, x, y, tot, fr, r, g, b, 100);
}


// ----------------------------------------------------------------- //
void	fx_MassHeal(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	tb_RenderBitmapFrame(bitmap, kfxMassHealID, x, y-30, tot, fr, 64764, 48573, 65021, 100);
}

// ----------------------------------------------------------------- //
void	fx_Dome(int bitmap, int type, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	int r= 60909;
	int g=12593;
	int b=45232;

	if (type==kDomeX)
	{
		r=34952;
		g=65535;
		b=63736;
	}

	tb_RenderBitmapFrame(bitmap, kfxDomeID, x, y, tot, fr, r, b, g, 100);
}


// ----------------------------------------------------------------- //
void	fx_Explosion(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	tb_RenderBitmapFrame(bitmap, kfxExplosionID, x, y, tot, fr, 62965, 55255, 13107, 100);
}

//Dave
// ----------------------------------------------------------------- //
void	fx_Explosion2(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	tb_RenderBitmapFrame(bitmap, kfxExplosionID, x, y, tot, fr, 62965, 55255, 13107, 100);
}


// ----------------------------------------------------------------- //
void	fx_Buff(int bitmap, int buff, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	int	r,g,b;

	switch (buff)
	{
		case kBuffStrX:
			r=57311;
			g=17733;
			b=16962;
			break;

		case kBuffAgiX:
			r=52171;
			g=41120;
			b=64764;
			break;

		case kBuffMindX:
			r=33924;
			g=42405;
			b=12593;
			break;

		case kBuffMRX:
			r=65021;
			g=39835;
			b=8738;
			break;

		case kBuffRDX:
			r=58853;
			g=64764;
			b=37522;
			break;
	}

	tb_RenderBitmapFrame(bitmap, kfxBuffID, x, y, tot, fr, r, g, b, 100);
}

// ----------------------------------------------------------------- //
void	fx_CreatureSummon(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //
{
	tb_RenderBitmapFrame(bitmap, kfxSummonID, x, y, tot, fr, 28270, 65535, 31868, 100);	
}

// ----------------------------------------------------------------- //
void	fx_SpatAxe(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //
{
	tb_RenderBitmapFrame(bitmap, kfxSpatSwordID, x-5, y-25, tot, fr, 50372, 51914, 65278, 100);	
}

// ----------------------------------------------------------------- //
void	fx_SpatSword(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //
{
	tb_RenderBitmapFrame(bitmap, kfxSpatSwordID, x, y-30, tot, fr, 50372, 51914, 65278, 100);	
}

// ----------------------------------------------------------------- //
void	fx_FBField(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //
{
	tb_RenderBitmapFrame(bitmap, kfxFBFieldID, x+3, y+15, tot, fr, 65535, 60138, 60909, 100);	
}

// ----------------------------------------------------------------- //
void	fx_BrackenRoot(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //
{
	tb_RenderBitmapFrame(bitmap, kfxBrackRoot, x, y+20, tot, fr, 32000, 32000, 32000, 100);	
}

// ----------------------------------------------------------------- //
void	fx_Res(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	tb_RenderBitmapFrame(bitmap, kfxResID, x, y, tot, fr, 65535, 64764, 44718, 70);
}


// ----------------------------------------------------------------- //
void	fx_Fireblast(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	tb_RenderBitmapFrame(bitmap, kfxFireblastID, x, y, tot, fr, 62965, 55255, 13107, 100);
	//tb_RenderBitmapFrame(bitmap, kfxFireblastID, x, y, tot, fr, 57054, 11565, 4626, 100);
}

// ----------------------------------------------------------------- //
void	fx_Fireblast2(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	tb_RenderBitmapFrame(bitmap, kfxFireblast2ID, x, y, tot, fr, 62965, 55255, 13107, 100);
	//tb_RenderBitmapFrame(bitmap, kfxFireblast2ID, x, y, tot, fr, 57054, 11565, 4626, 100);
}

// ----------------------------------------------------------------- //
void	fx_Fireblast3(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	tb_RenderBitmapFrame(bitmap, kfxFireblast3ID, x, y, tot, fr, 0, 0, 0, 100);
	//tb_RenderBitmapFrame(bitmap, kfxFireblast3ID, x, y, tot, fr, 55255, 55255, 55255, 100);
}

// ----------------------------------------------------------------- //
void	fx_Fireblast4(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	tb_RenderBitmapFrame(bitmap, kfxFireblast4ID, x, y, tot, fr, 62965, 55255, 13107, 100);
	//tb_RenderBitmapFrame(bitmap, kfxFireblast4ID, x, y, tot, fr, 57054, 11565, 4626, 100);
}

// ----------------------------------------------------------------- //
void	fx_Fireblast5(int bitmap, int tot, int fr, int x, int y)
// ----------------------------------------------------------------- //

{
	tb_RenderBitmapFrame(bitmap, kfxFireblast5ID, x, y, tot, fr, 62965, 55255, 13107, 100);
	//tb_RenderBitmapFrame(bitmap, kfxFireblast5ID, x, y, tot, fr, 57054, 11565, 4626, 100);
}


/////////////////////////////////////////////////////////////////////
/*
	This function creates a lens flare effect. Note the bitmap resources
	that must be used with this effect are indicated by the defines in FX.h
	as follows:

		#define	kfxStarID	5000
		#define	kfxFlare0ID	4000
		#define	kfxFlare1ID	4001
		#define	kfxFlare2ID	4002
		#define	kfxFlare3ID	4003
		#define	kfxFlare4ID	4004

	The number of frames in the STAR is specified in FX.h as:

		#define	kfxStarFrames	4

	x,y:	this is the center of the flickering star part of the lens flare
	angle:	this is the angle of the effect (0 to the right, 90 up, 180 to the left, 270 down,...)
	length:	this is the length of the effect which determines the spacing between the
			star part and the flare parts
	r,g,b:	the red, green, and blue color components of the effect
	alpha:	the level of transparency of the effect, the lower the number the more transparent

	There are no other functions reguired to use the lens flare, simply call this function each
	time you want one drawn to the given bitmap.
*/
void	fx_LensFlare(TBitmap bitmap, int x, int y, float angle, int length, unsigned int r, unsigned int g, unsigned int b, int alpha)
{
	float	c,s;
	int	x1,y1;
	int	dl;
	float	f = (float)alpha/100.0f;

	c = cos(angle*tbPI/180.0f);
	s = sin(angle*tbPI/180.0f);
	dl = tb_Round(length/6.0f);

	tb_RenderBitmap(bitmap, kfxStarID, x, y, kfxStarFrames, r, g, b, tb_Round(100*f));


	x1 = x+(c * (-1*dl));
	y1 = y+(-s * (-1*dl));
	tb_RenderBitmap(bitmap, kfxFlare1ID, x1, y1, 1, r, g, b, tb_Round(100*f));

	tb_RenderBitmap(bitmap, kfxFlare3ID, x, y, 1, r, g, b, tb_Round(70*f));

	x1 = x+(c * (1*dl));
	y1 = y+(-s * (1*dl));
	tb_RenderBitmap(bitmap, kfxFlare2ID, x1, y1, 1, r, g, b, tb_Round(50*f));

	x1 = x+(c * (1.5*dl));
	y1 = y+(-s * (1.5*dl));
	tb_RenderBitmap(bitmap, kfxFlare4ID, x1, y1, 1, r, g, b, tb_Round(20*f));

	x1 = x+(c * (2.0*dl));
	y1 = y+(-s * (2.0*dl));
	tb_RenderBitmap(bitmap, kfxFlare1ID, x1, y1, 1, r, g, b, tb_Round(80*f));

	x1 = x+(c * (2.75*dl));
	y1 = y+(-s * (2.75*dl));
	tb_RenderBitmap(bitmap, kfxFlare2ID, x1, y1, 1, r, g, b, tb_Round(35*f));

	x1 = x+(c * (3.5*dl));
	y1 = y+(-s * (3.5*dl));
	tb_RenderBitmap(bitmap, kfxFlare1ID, x1, y1, 1, r, g, b, tb_Round(65*f));
}

///////////////////////////////////////////////////////////////////
/*  This is generic function to update the state of a given particle.
	There's no need to call this function directly, it's used by the
	particle explosion and hose functions.

	p:		pointer to a particle structure
	dtime:	time increment in milliseconds to advance the state of the particle

	If the total elapsed time for this particle has exceeded the particle's
	set life, then this function returns FALSE indicating that the particle
	should expire.
*/
TBoolean	fx_UpdateParticleState(TFXParticle* p, int dtime)
{
	TBoolean retval;
	float	t;

	p->time+=dtime;
	t = (float)p->time/1000.0f;
	p->x = p->vi * cos(p->angle*tbPI/180.0f) * t;
	p->y = p->vi * sin(p->angle*tbPI/180.0f) * t + (p->gravity*t*t)/2.0f;

	if (p->time >= p->life)
		retval = false;
	else
		retval = true;

	return retval;
}


/////////////////////////////////////////////////////////////////////
/*	This function creates a new particle explosion effect.

	x,y:	starting point of the effect
	force:	a measure of how fast the particles will be sent flying (it's actually the initial
			velocity of the particles)
	life:	life of the particles in milliseconds; particles will fade and die out as they approach
			their specified life
	gravity:	the acceleration due to gravity which controls the rate at which particles will fall
				as they fly
	angle:	initial trajectory angle of the particles, specify 999 to create a particle explosion
			that emits particles in all directions; otherwise, 0 right, 90 up, 180 left, etc...

	clr:	use one of the defines in FX.h to indicate a color; kfxRandom randomly selects one of
			kfxRed, kfxBlue or kfxGreen.

	Returns an index to the new effect.
*/
int		fx_CreateParticleExplosion(int x, int y, int force, int life, float gravity, float angle, int clr)
{
	int	i;
	int	index = -1;
	int	m;

	for (i=0; i<_MAXEFFECTS; i++)
	{
		if (!fxParticleExplosion[i].Active)
		{
			index = i;
			break;
		}
	}

	if (index > -1)
	{
		fxParticleExplosion[index].Active = true;
		fxParticleExplosion[index].x = x;
		fxParticleExplosion[index].y = y;
		fxParticleExplosion[index].force = force;

		for (i=0; i<_MAXPARTICLES; i++)
		{
			fxParticleExplosion[index].p[i].x = 0;
			fxParticleExplosion[index].p[i].y = 0;
			fxParticleExplosion[index].p[i].vi = tb_Rnd(force/2, force);

			if (angle < 999)
			{
				if (tb_Rnd(0,1) == 0)
					m = -1;
				else
					m = 1;

				fxParticleExplosion[index].p[i].angle = -angle + m * tb_Rnd(0,10);
			}
			else
				fxParticleExplosion[index].p[i].angle = tb_Rnd(0,360);

			fxParticleExplosion[index].p[i].life = life;

			fxParticleExplosion[index].p[i].r = tb_Rnd(40000, 65535);

			fxParticleExplosion[index].p[i].g = tb_Rnd(30000, 45535);

			fxParticleExplosion[index].p[i].b = tb_Rnd(30000, 45535);

			fxParticleExplosion[index].p[i].time = 0;

			fxParticleExplosion[index].p[i].Active = true;

			fxParticleExplosion[index].p[i].gravity = gravity;

			fxParticleExplosion[index].p[i].clr = clr;

		}
	}

	return index;
}

///////////////////////////////////////////////////////////////////
// Deactivates the given effect
void	fx_DisposeParticleExplosion(int index)
{
	if ((index >= 0)  && (index < _MAXEFFECTS))
		fxParticleExplosion[index].Active = false;
}

///////////////////////////////////////////////////////////////////
// Draws the particle system and updates the state of each particle.
// Returns false when all of the particles have died out.
TBoolean	fx_DrawParticleExplosion(TBitmap bitmap, int index)
{
	int	i;
	TBoolean	finished = true;
	int			r;
	int clr;

	if (fxParticleExplosion[index].Active)
		for (i=0; i<_MAXPARTICLES; i++)
		{
			if (fxParticleExplosion[index].p[i].Active)
			{
				finished = false;
				r = tb_Round(4 *
							 (float)(fxParticleExplosion[index].p[i].life-fxParticleExplosion[index].p[i].time)/
							 (fxParticleExplosion[index].p[i].life));

				if (r>0)
				{
					if (fxParticleExplosion[index].p[i].clr == kfxRandom)
						clr = tb_Rnd(kfxRed,kfxBlue);
					else
						clr = fxParticleExplosion[index].p[i].clr;

					tb_RenderColoredLightSource(	bitmap,
												 fxParticleExplosion[index].x+tb_Round(fxParticleExplosion[index].p[i].x),
												 fxParticleExplosion[index].y+tb_Round(fxParticleExplosion[index].p[i].y),
												 r,
												 clr,
												 20, gameTime);
				}

				tb_SetPixelRGB(	bitmap,

								fxParticleExplosion[index].x+tb_Round(fxParticleExplosion[index].p[i].x),
								fxParticleExplosion[index].y+tb_Round(fxParticleExplosion[index].p[i].y),
								fxParticleExplosion[index].p[i].r,
								fxParticleExplosion[index].p[i].g,
								fxParticleExplosion[index].p[i].b);

				fxParticleExplosion[index].p[i].Active = fx_UpdateParticleState(&(fxParticleExplosion[index].p[i]), 100);
			}
		}

	if (finished)
		fxParticleExplosion[index].Active = false;

	return !finished;
}


/////////////////////////////////////////////////////////////////////
/*	This function creates a new particle hose effect.

	x,y:	starting point of the effect
	force:	a measure of how fast the particles will be sent flying (it's actually the initial
			velocity of the particles)
	life:	life of the particles in milliseconds; particles will fade and die out as they approach
			their specified life
	gravity:	the acceleration due to gravity which controls the rate at which particles will fall
				as they fly
	angle:	initial trajectory angle of the particles, 0 right, 90 up, 180 left, etc...
	da:		deviation from initial angle; use a small number > zero to give a little randomness to the trajectory

	clr:	use one of the defines in FX.h to indicate a color; kfxRandom randomly selects one of
			kfxRed, kfxBlue or kfxGreen.

	Returns an index to the new effect.

	NOTE: A particle hose is unlike an explosion in that the hose will continue to spray particles.
	As older particles die, new ones are automatically created.
*/
int		fx_CreateParticleHose(int x, int y, int force, int life, float gravity, float angle, int da, int clr)
{
	int	i;
	int	index = -1;
	//int	m;

	for (i=0; i<_MAXEFFECTS; i++)
	{
		if (!fxParticleHose[i].Active)
		{
			index = i;
			break;
		}
	}

	if (index > -1)
	{
		fxParticleHose[index].Active = true;
		fxParticleHose[index].x = x;
		fxParticleHose[index].y = y;
		fxParticleHose[index].force = force;
		fxParticleHose[index].angle = angle;
		fxParticleHose[index].life = life;
		fxParticleHose[index].gravity = gravity;
		fxParticleHose[index].da = da;
		fxParticleHose[index].clr = clr;
	}

	return index;
}

////////////////////////////////////////////////////////////////////
// Kill the hose
void	fx_DisposeParticleHose(int index)
{
	if ((index >= 0)  && (index < _MAXEFFECTS))
		fxParticleHose[index].Active = false;
}

////////////////////////////////////////////////////////////////////
// Draw the effect and update the state of the particles.
// Even though this function has a return value, it will always return
// TRUE since the effect will continue to spray particles until the
// effect is disposed of.
TBoolean	fx_DrawParticleHose(TBitmap bitmap, int index, int nx, int ny)
{
	int	i;
	TBoolean	finished = true;
	int			r;
	int	m;
	int clr;


	if (fxParticleHose[index].Active)
	{
		// activate a new particle
		m=-1;

		for (i=0; i<_MAXPARTICLES; i++)
		{
			if (!fxParticleHose[index].p[i].Active)
			{
				m = i;
				break;
			}
		}

		if (m!=-1)
		{
			i = m;
			fxParticleHose[index].p[i].x = 0;
			fxParticleHose[index].p[i].y = 0;
			fxParticleHose[index].p[i].vi = tb_Rnd(fxParticleHose[index].force/2, fxParticleHose[index].force);

			if (tb_Rnd(0,1) == 0)
				m = -1;
			else
				m = 1;

			fxParticleHose[index].p[i].angle = -fxParticleHose[index].angle + m * tb_Rnd(0,fxParticleHose[index].da);

			fxParticleHose[index].p[i].life = fxParticleHose[index].life;

			fxParticleHose[index].p[i].r = tb_Rnd(40000, 65535);

			fxParticleHose[index].p[i].g = tb_Rnd(30000, 45535);

			fxParticleHose[index].p[i].b = tb_Rnd(30000, 45535);

			fxParticleHose[index].p[i].time = 0;

			fxParticleHose[index].p[i].Active = true;

			fxParticleHose[index].p[i].gravity = fxParticleHose[index].gravity;

			fxParticleHose[index].p[i].clr = fxParticleHose[index].clr;
		}

		fxParticleHose[index].x=nx;

		fxParticleHose[index].y=ny;

		for (i=0; i<_MAXPARTICLES; i++)
		{
			if (fxParticleHose[index].p[i].Active)
			{
				finished = false;
				r = tb_Round(4 *
							 (float)(fxParticleHose[index].p[i].life-fxParticleHose[index].p[i].time)/
							 (fxParticleHose[index].p[i].life));

				if (r>0)
				{
					if (fxParticleHose[index].p[i].clr == kfxRandom)
						clr = tb_Rnd(kfxRed,kfxBlue);
					else
						clr = fxParticleHose[index].p[i].clr;

					tb_RenderColoredLightSource(	bitmap,
												 fxParticleHose[index].x+tb_Round(fxParticleHose[index].p[i].x),
												 fxParticleHose[index].y+tb_Round(fxParticleHose[index].p[i].y),
												 r,
												 clr,
												 20, gameTime);
				}

				tb_SetPixelRGB(	bitmap,

								fxParticleHose[index].x+tb_Round(fxParticleHose[index].p[i].x),
								fxParticleHose[index].y+tb_Round(fxParticleHose[index].p[i].y),
								fxParticleHose[index].p[i].r,
								fxParticleHose[index].p[i].g,
								fxParticleHose[index].p[i].b);

				fxParticleHose[index].p[i].Active = fx_UpdateParticleState(&(fxParticleHose[index].p[i]), 100);
			}
		}
	}

	if (finished)
		fxParticleHose[index].Active = false;

	return !finished;

}

/////////////////////////////////////////////////////////////////////
/*	This function creates a new fire effect.

	r:		the rectangle in screen coordinates that contains the effect
	shape:	specifies the shape of the fire effect, i.e., whether it is a cone of fire
			or a wall of fire.

			Use kfxFireCone or kfxFireWall

	clr:	specifies the color scheme; use:

			kfxWhiteFire
			kfxRedFire			or		kfxRed
			kfxGreenFire		or		kfxGreen
			kfxBlueFire			or		kfxBlue

	blend:	specifies how the fire particles are blended together to create different fire "textures"

			Use:

			kfxBlendNone	-->	kind of pixelated
			kfxBlendAll		--> almost cloudy
			kfxBlendVert	-->	best for flames
			kfxBlendHorz	-->	good for gas or steam
			kfxBlendDiagL	-->	good for rolling gas or steam (or falling water)
			kfxBlendDiagR	-->	good for rolling gas or steam (or falling water)

	invert:	specifies whether the effect is inverted, e.g., when creating a water fall effect

	bottomShape:	specifies the shape of the bottom portion of the fire

			Use:

			kfxFlatBottom		-->	flat horizontal bottom
			kfxRoundBottom		--> rounded bottom (works best when fire rectangle is square, or slightly taller than wide)
			kfxCornerBottom		--> makes a pointy bottom with two sides at 45 degrees (works best when fire rectangle is square)
			kfxSlantLBottom		--> fire slants down and to the left (works best when fire rectangle is twice as tall as it is wide)
			kfxSlantRBottom		-->	fire slants down and to the right (works best when fire rectangle is twice as tall as it is wide)

	Returns an index to the new effect.

	NOTE: This effect dynamically allocates memory, so its dispose function must be called to clean it up.
*/
int		fx_CreateFire(TRect r, int shape, int clr, int blend, TBoolean invert, int bottomShape, TBoolean prime)
{

	int	index = -1;
	int i;
	pTFXFireParticle p;
	int	x,y;

	for (i=0; i<_MAXEFFECTS; i++)
	{
		if (!fxFire[i].Active)
		{
			index = i;
			break;
		}
	}

	if (index > -1)
	{
		fxFire[index].bottomShape = bottomShape;
		fxFire[index].blend = blend;
		fxFire[index].invert = invert;
		fxFire[index].clr = clr;
		fxFire[index].shape = shape;
		fxFire[index].Active = true;
		fxFire[index].w = r.right - r.left;
		fxFire[index].h = r.bottom - r.top;
		fxFire[index].p = (pTFXFireParticle) malloc(fxFire[index].w*fxFire[index].h*sizeof(TFXFireParticle));
		tb_SetRect(&fxFire[index].r, r.left, r.top, r.right, r.bottom);

		if (prime)
		{
			for (x=0; x<(fxFire[index].w-1); x++)
				for (y=0; y<(fxFire[index].h-1); y++)
				{
					p = &(fxFire[index].p[y * fxFire[index].w + x]);
					p->life = 0;
					p->age = 0;
					p->ri = 0;
					p->gi = 0;
					p->bi = 0;
					p->rn = 0;
					p->gn = 0;
					p->bn = 0;
					p->x = x;
					p->y = 0;
				}

			fx_PrimeFlames(index);
		}
		else
		{
			for (x=0; x<(fxFire[index].w-1); x++)
				for (y=0; y<(fxFire[index].h-1); y++)
				{
					p = &(fxFire[index].p[y * fxFire[index].w + x]);
					p->life = 0;
					p->age = 0;
					p->ri = 0;
					p->gi = 0;
					p->bi = 0;
					p->rn = 0;
					p->gn = 0;
					p->bn = 0;
					p->x = x;
					p->y = 0;
				}
		}

	}

	//fx_GrowFire(index, (r.bottom-r.top));
	//fx_PrimeFlames(index);

	return index;
}

////////////////////////////////////////////////////////////////////
// Kill the fire effect and clean up the dynamically allocated memory
void	fx_DisposeFire(int index)
{
	if ((index >= 0)  && (index < _MAXEFFECTS))
	{
		fxFire[index].Active = false;

		if (fxFire[index].p!=NULL)  //fs
			free(fxFire[index].p);

		//tb_DisposeBitmap(fxFire[index].buf);
	}
}

/////////////////////////////////////////////////////////////////////
// Draw the fire effect
void	fx_DrawFire(TBitmap bitmap, int index, int nx, int ny)
{
	int					w,h;
	int					x,y;
	pTFXFireParticle 		p0, p1, p2;
	unsigned int 			ra, ga, ba;
	unsigned int 			rs, gs, bs;
	int					d;
	int					low, high;
	int					pth;
	int					ptv;
	TRect				sr;
	int					i;
	int 					bot;
	int 					ymin;
	int					x1;


	if ((index<0) || (index>=_MAXEFFECTS))			// beta debug
		fi_WriteToErrorLog("Out of Range - fx_DrawFire");

	tb_GetBitmapRect(bitmap, &sr);

	bot = fxFire[index].bottomShape;

	w = fxFire[index].w;

	h = fxFire[index].h;

	tb_SetRect(&fxFire[index].r, nx, ny, nx+w, ny+h);

	// copy row n to row n+1 from h down to 0
	// and adjust colors representing cooling effect
	for (x=0; x<w-1; x+=2)
	{
		// going once
		if (bot == kfxFlatBottom)
			ymin = 0;
		else if (bot == kfxCornerBottom)
		{
			if (x<(w/2))
			{
				ymin = tb_Round(((float)h/2.0f) - (x));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
			else
			{
				ymin = tb_Round(((float)h/2.0f) - (w-x));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
		}
		else if (bot == kfxRoundBottom)
		{
			if (x<(w/2))
			{
				ymin = tb_Round( (h/2) - sqrt( (w/2)*(w/2) - ((w/2)-x)*((w/2)-x) ) );
				//ymin = tb_Round( ((h/2)*(h/2)) - ( (w/2)*(w/2) - ((w/2)-x)*((w/2)-x) ) );

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
			else
			{
				ymin = tb_Round( (h/2) -  sqrt( (w/2)*(w/2) - (x-(w/2))*(x-(w/2)) ) );
				//ymin = tb_Round( ((h/2)*(h/2)) -  ( (w/2)*(w/2) - (x-(w/2))*(x-(w/2)) ) );

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
		}
		else if (bot == kfxSlantRBottom)
		{
			ymin = tb_Round(((float)h/2.0f) - (x));

			if (ymin<1) ymin = 1;

			if (ymin>(h/2)) ymin = (h/2);
		}
		else if (bot == kfxSlantLBottom)
		{
			ymin = tb_Round(((float)h/2.0f) - (w-x));

			if (ymin<1) ymin = 1;

			if (ymin>(h/2)) ymin = (h/2);
		}

		for (y=h-1; y>ymin; y--)
		{
			p0 = &(fxFire[index].p[(y-1) * fxFire[index].w + x]);
			p1 = &(fxFire[index].p[(y) * fxFire[index].w + x]);

			memcpy(p1, p0, sizeof(TFXFireParticle));

			p1->age++;

			if ( (p1->life > 0) &&
					(p1->life - p1->age) >= 0)
			{

				if (fxFire[index].clr == kfxRedFire)
				{
					// mostly red
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/3.0f);
				}
				else if (fxFire[index].clr == kfxGreenFire)
				{
					// mostly green
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
				}
				else if (fxFire[index].clr == kfxBlueFire)
				{
					// mostly blue
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
				}
				else if (fxFire[index].clr == kfxWhiteFire)
				{
					// kind of misty
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
				}

				if (p1->rn < 5000) p1->rn = 0;

				if (p1->gn < 5000) p1->gn = 0;

				if (p1->bn < 5000) p1->bn = 0;

				if (p1->rn > 65535) p1->rn = 65535;

				if (p1->gn > 65535) p1->gn = 65535;

				if (p1->bn > 65535) p1->bn = 65535;


			}
			else
			{
				p1->rn = 0;
				p1->gn = 0;
				p1->bn = 0;
			}
		}

		// get new row 0 values
		if (fxFire[index].shape == kfxFireCone)
		{
			if (x<w/2)
			{
				low = tb_Round((h/2.0f) * ((x*x)/(w*w/4.0f)));
				high = tb_Round((h-1) * ((x*x)/(w*w/4.0f)));
			}

			if (x>=w/2)
			{
				low = tb_Round((h/2.0f) * ((w-x)*(w-x)/(w*w/4.0f)));
				high = tb_Round((h-1) * ((w-x)*(w-x)/(w*w/4.0f)));
			}
		}

		if (fxFire[index].shape == kfxFireWall)
		{
			if (x<tb_Round((float)w/8.0f))
			{
				low = tb_Round((h/2.0f) * ((x*x)/(w*w/64.0f)));
				high = tb_Round((h-1) * ((x*x)/(w*w/64.0f)));
			}
			else
				if (x>(w-tb_Round((float)w/8.0f)))
				{
					low = tb_Round((h/2.0f) * ((w-x)*(w-x)/(w*w/64.0f)));
					high = tb_Round((h-1) * ((w-x)*(w-x)/(w*w/64.0f)));
				}
				else
				{
					low = tb_Round((h/2.0f));
					high = tb_Round((h-1));
				}
		}

		if (bot == kfxSlantRBottom)
		{
			low=tb_Round(2*low/3);
			high=tb_Round(2*high/3);
		}
		else if (bot == kfxSlantLBottom)
		{
			low=tb_Round(2*low/3);
			high=tb_Round(2*high/3);
		}

		p0 = &(fxFire[index].p[ymin * w + x]);

		p0->life = tb_Rnd(low, high);
		p0->age = 0;

		p0->ri = tb_Round(65535 * (float)tb_Rnd(90, 100)/100.0f);
		p0->gi = tb_Round(65535 * (float)tb_Rnd(80, 100)/100.0f);
		p0->bi = tb_Round(65535 * (float)tb_Rnd(50, 100)/100.0f);

		p0->rn = 0;
		p0->gn = 0;
		p0->bn = 0;
		p0->x = x;
		p0->y = ymin;




		// going twice
		x1=x+1;

		if (bot == kfxFlatBottom)
			ymin = 0;
		else if (bot == kfxCornerBottom)
		{
			if (x1<(w/2))
			{
				ymin = tb_Round(((float)h/2.0f) - (x1));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
			else
			{
				ymin = tb_Round(((float)h/2.0f) - (w-x1));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
		}
		else if (bot == kfxRoundBottom)
		{
			if (x1<(w/2))
			{
				ymin = tb_Round( (h/2) - sqrt( (w/2)*(w/2) - ((w/2)-x1)*((w/2)-x1) ) );

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
			else
			{
				ymin = tb_Round( (h/2) -  sqrt( (w/2)*(w/2) - (x1-(w/2))*(x1-(w/2)) ) );

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
		}
		else if (bot == kfxSlantRBottom)
		{
			ymin = tb_Round(((float)h/2.0f) - (x1));

			if (ymin<1) ymin = 1;

			if (ymin>(h/2)) ymin = (h/2);
		}
		else if (bot == kfxSlantLBottom)
		{
			ymin = tb_Round(((float)h/2.0f) - (w-x1));

			if (ymin<1) ymin = 1;

			if (ymin>(h/2)) ymin = (h/2);
		}

		for (y=h-1; y>ymin; y--)
		{
			p0 = &(fxFire[index].p[(y-1) * fxFire[index].w + x1]);
			p1 = &(fxFire[index].p[(y) * fxFire[index].w + x1]);

			memcpy(p1, p0, sizeof(TFXFireParticle));

			p1->age++;

			if ( (p1->life > 0) &&
					(p1->life - p1->age) >= 0)
			{

				if (fxFire[index].clr == kfxRedFire)
				{
					// mostly red
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/3.0f);
				}
				else if (fxFire[index].clr == kfxGreenFire)
				{
					// mostly green
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
				}
				else if (fxFire[index].clr == kfxBlueFire)
				{
					// mostly blue
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
				}
				else if (fxFire[index].clr == kfxWhiteFire)
				{
					// kind of misty
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
				}

				if (p1->rn < 5000) p1->rn = 0;

				if (p1->gn < 5000) p1->gn = 0;

				if (p1->bn < 5000) p1->bn = 0;

				if (p1->rn > 65535) p1->rn = 65535;

				if (p1->gn > 65535) p1->gn = 65535;

				if (p1->bn > 65535) p1->bn = 65535;


			}
			else
			{
				p1->rn = 0;
				p1->gn = 0;
				p1->bn = 0;
			}
		}

		// get new row 0 values
		if (fxFire[index].shape == kfxFireCone)
		{
			if (x1<w/2)
			{
				low = tb_Round((h/2.0f) * ((x1*x1)/(w*w/4.0f)));
				high = tb_Round((h-1) * ((x1*x1)/(w*w/4.0f)));
			}

			if (x1>=w/2)
			{
				low = tb_Round((h/2.0f) * ((w-x1)*(w-x1)/(w*w/4.0f)));
				high = tb_Round((h-1) * ((w-x1)*(w-x1)/(w*w/4.0f)));
			}
		}

		if (fxFire[index].shape == kfxFireWall)
		{
			if (x1<tb_Round((float)w/8.0f))
			{
				low = tb_Round((h/2.0f) * ((x1*x1)/(w*w/64.0f)));
				high = tb_Round((h-1) * ((x1*x1)/(w*w/64.0f)));
			}
			else
				if (x1>(w-tb_Round((float)w/8.0f)))
				{
					low = tb_Round((h/2.0f) * ((w-x1)*(w-x1)/(w*w/64.0f)));
					high = tb_Round((h-1) * ((w-x1)*(w-x1)/(w*w/64.0f)));
				}
				else
				{
					low = tb_Round((h/2.0f));
					high = tb_Round((h-1));
				}
		}

		if (bot == kfxSlantRBottom)
		{
			low=tb_Round(2*low/3);
			high=tb_Round(2*high/3);
		}
		else if (bot == kfxSlantLBottom)
		{
			low=tb_Round(2*low/3);
			high=tb_Round(2*high/3);
		}

		p0 = &(fxFire[index].p[ymin * w + x1]);

		p0->life = tb_Rnd(low, high);
		p0->age = 0;

		p0->ri = tb_Round(65535 * (float)tb_Rnd(90, 100)/100.0f);
		p0->gi = tb_Round(65535 * (float)tb_Rnd(80, 100)/100.0f);
		p0->bi = tb_Round(65535 * (float)tb_Rnd(50, 100)/100.0f);

		p0->rn = 0;
		p0->gn = 0;
		p0->bn = 0;
		p0->x = x1;
		p0->y = ymin;










	}
/*
	
		// generate some random puffs
		y = 0;
		for(i=0; i<20; i++)
		{
			x = tb_Rnd(4, w-5);

			if(bot == kfxFlatBottom)
				ymin = 0;
			else if(bot == kfxCornerBottom)
			{
				if(x<(w/2))
				{
					ymin = tb_Round(((float)h/2.0f) - (x));
					if(ymin<1) ymin = 1;
					if(ymin>(h/2)) ymin = (h/2);
				} else
				{
					ymin = tb_Round(((float)h/2.0f) - (w-x));
					if(ymin<1) ymin = 1;
					if(ymin>(h/2)) ymin = (h/2);
				}
			} else if(bot == kfxRoundBottom)
			{
				if(x<(w/2))
				{
					ymin = tb_Round( (h/2) - sqrt( (w/2)*(w/2) - ((w/2)-x)*((w/2)-x) ) );
					if(ymin<1) ymin = 1;
					if(ymin>(h/2)) ymin = (h/2);
				} else
				{
					ymin = tb_Round( (h/2) -  sqrt( (w/2)*(w/2) - (x-(w/2))*(x-(w/2)) ) );
					if(ymin<1) ymin = 1;
					if(ymin>(h/2)) ymin = (h/2);
				}
			} else if(bot == kfxSlantRBottom)
			{
				ymin = tb_Round(((float)h/2.0f) - (x));
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			} else if(bot == kfxSlantLBottom)
			{
				ymin = tb_Round(((float)h/2.0f) - (w-x));
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			}

			y = tb_Rnd(ymin+5, tb_Round(0.8f*h));

			p0 = &(fxFire[index].p[y * w + x]);
			p0->ri = tb_Round((float)p0->ri*2.0f);
			p0->gi = tb_Round((float)p0->gi*2.0f);
			p0->bi = tb_Round((float)p0->bi*2.0f);
			if(p0->ri > 65535) p0->ri = 65535;
			if(p0->gi > 65535) p0->gi = 65535;
			if(p0->bi > 65535) p0->bi = 65535;

			p2 = &(fxFire[index].p[(y-1) * w + x]);
			memcpy(p2, p0, sizeof(TFXFireParticle));
			p2->y = (y-1);

			p2 = &(fxFire[index].p[(y+1) * w + x]);
			memcpy(p2, p0, sizeof(TFXFireParticle));
			p2->y = (y+1);

			p2 = &(fxFire[index].p[(y) * w + (x+1)]);
			memcpy(p2, p0, sizeof(TFXFireParticle));
			p2->x = (x+1);

			p2 = &(fxFire[index].p[(y) * w + (x-1)]);
			memcpy(p2, p0, sizeof(TFXFireParticle));
			p2->x = (x-1);
		}*/
		

	// copy effect to the bitmap
	for (y=0; y<h; y++)
	{
		for (x=0; x<(w-1); x++)
		{
			p0 = &(fxFire[index].p[y * w + x]);

			if ((p0->age < p0->life) && (p0->rn > 0 || p0->gn > 0 || p0->bn > 0))
			{
				d = 0;
				ra = 0;
				ga = 0;
				ba = 0;
				// get average
				ra += p0->rn;
				ga += p0->gn;
				ba += p0->bn;
				d++;

				if ((fxFire[index].blend == kfxBlendVert)	||
						(fxFire[index].blend == kfxBlendAll) )
				{
					if ( (y-1) > 0 )
					{
						if (p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y-1) * w + x]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}

					if ( (y+1) < h )
					{
						if (p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y+1) * w + x]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}


					if ( (y-2) > 0 )
					{
						if (p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y-2) * w + x]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}

					if ( (y+2) < h )
					{
						if (p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y+2) * w + x]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}
				}

				if ((fxFire[index].blend == kfxBlendHorz)	|| (fxFire[index].blend == kfxBlendAll))
				{
					if ( (x+1) < w )
					{
						if (p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[y * w + (x+1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}

					if ( (x-1) > 0 )
					{
						if (p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[y * w + (x-1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}
				}

				if ((fxFire[index].blend == kfxBlendDiagR)	|| (fxFire[index].blend == kfxBlendAll))
				{
					if ( ((y-1) > 0) && ((x-1) > 0))
					{
						if (p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y-1) * w + (x-1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}

					if ( ((y+1) < h) && ((x+1) < w))
					{
						if (p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y+1) * w + (x+1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}
				}

				if ((fxFire[index].blend == kfxBlendDiagL)	||
						(fxFire[index].blend == kfxBlendAll) )
				{

					if ( ((y-1) > 0) && ((x+1) < w))
					{
						if (p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y-1) * w + (x+1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}

					if ( ((y+1) < h) && ((x-1) > 0))
					{
						if (p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y+1) * w + (x-1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}
				}

				ra = ra/d;

				ga = ga/d;
				ba = ba/d;

				if ((ra > 5000 ||ga > 5000 || ba > 5000))
				{
					pth = fxFire[index].r.left + x;

					if (fxFire[index].invert)
						ptv = fxFire[index].r.top + y;
					else
						ptv = fxFire[index].r.top + (h-y);

					if (pth<=sr.left) 		continue;

					if (pth>=sr.right) 		continue;

					if (ptv<=sr.top) 		continue;

					if (ptv>=sr.bottom) 	continue;

					tb_GetPixelRGB(bitmap, pth, ptv, &rs, &gs, &bs);

					rs+=ra;

					gs+=ga;

					bs+=ba;

					rs=tb_Round(rs* (0.6f+(float)(ra+ga+ba)/(196605.0f)));

					gs=tb_Round(gs* (0.6f+(float)(ra+ga+ba)/(196605.0f)));

					bs=tb_Round(bs* (0.6f+(float)(ra+ga+ba)/(196605.0f)));

					if (rs>65535) rs = 65535;

					if (gs>65535) gs = 65535;

					if (bs>65535) bs = 65535;

					tb_SetPixelRGB(	bitmap, pth, ptv, rs, gs, bs);

				}

			}
		}
	}

}


/*
void	fx_DrawFire(TBitmap bitmap, int index, int nx, int ny)
{
	int									w,h;
	int									x,y;
	pTFXFireParticle 		p0, p1, p2;
	unsigned int 				ra, ga, ba;
	unsigned int 				rs, gs, bs;
	int									d;
	int									low, high;
	int									pth;
	int									ptv;
	TRect								sr;
	int									i,j;
	int 								bot;
	int 								ymin;


	tb_GetBitmapRect(bitmap, &sr);

	bot = fxFire[index].bottomShape;

	w = fxFire[index].w;
	h = fxFire[index].h;
	tb_SetRect(&fxFire[index].r, nx, ny, nx+w, ny+h);

	// copy row n to row n+1 from h down to 0
	// and adjust colors representing cooling effect
	for(x=0; x<w; x++)
	{
		if(bot == kfxFlatBottom)
			ymin = 0;
		else if(bot == kfxCornerBottom)
		{
			if(x<(w/2))
			{
				ymin = tb_Round(((float)h/2.0f) - (x));
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			} else
			{
				ymin = tb_Round(((float)h/2.0f) - (w-x));
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			}
		} else if(bot == kfxRoundBottom)
		{
			if(x<(w/2))
			{
				ymin = tb_Round( (h/2) - sqrt( (w/2)*(w/2) - ((w/2)-x)*((w/2)-x) ) );
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			} else
			{
				ymin = tb_Round( (h/2) -  sqrt( (w/2)*(w/2) - (x-(w/2))*(x-(w/2)) ) );
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			}
		} else if(bot == kfxSlantRBottom)
		{
			ymin = tb_Round(((float)h/2.0f) - (x));
			if(ymin<1) ymin = 1;
			if(ymin>(h/2)) ymin = (h/2);
		} else if(bot == kfxSlantLBottom)
		{
			ymin = tb_Round(((float)h/2.0f) - (w-x));
			if(ymin<1) ymin = 1;
			if(ymin>(h/2)) ymin = (h/2);
		}

		for(y=h-1; y>ymin; y--)
		{
			p0 = &(fxFire[index].p[(y-1) * fxFire[index].w + x]);
			p1 = &(fxFire[index].p[(y) * fxFire[index].w + x]);

			memcpy(p1, p0, sizeof(TFXFireParticle));

			p1->age++;

			if( (p1->life > 0) &&
				(p1->life - p1->age) >= 0)
			{

				if(fxFire[index].clr == kfxRedFire)
				{
					// mostly red
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/3.0f);
				} else if(fxFire[index].clr == kfxGreenFire)
				{
					// mostly green
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
				} else if(fxFire[index].clr == kfxBlueFire)
				{
					// mostly blue
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
				} else if(fxFire[index].clr == kfxWhiteFire)
				{
					// kind of misty
					p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
					p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
					p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
				}

				if(p1->rn < 5000) p1->rn = 0;
				if(p1->gn < 5000) p1->gn = 0;
				if(p1->bn < 5000) p1->bn = 0;

				if(p1->rn > 65535) p1->rn = 65535;
				if(p1->gn > 65535) p1->gn = 65535;
				if(p1->bn > 65535) p1->bn = 65535;


			} else
			{
				p1->rn = 0;
				p1->gn = 0;
				p1->bn = 0;
			}
		}
	}

	// get new row 0 through 3 values
	y = 0;
	for(x=0; x<w; x++)
	{

		if(bot == kfxFlatBottom)
			ymin = 0;
		else if(bot == kfxCornerBottom)
		{
			if(x<(w/2))
			{
				ymin = tb_Round(((float)h/2.0f) - (x));
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			} else
			{
				ymin = tb_Round(((float)h/2.0f) - (w-x));
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			}
		} else if(bot == kfxRoundBottom)
		{
			if(x<(w/2))
			{
				ymin = tb_Round( (h/2) - sqrt( (w/2)*(w/2) - ((w/2)-x)*((w/2)-x) ) );
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			} else
			{
				ymin = tb_Round( (h/2) -  sqrt( (w/2)*(w/2) - (x-(w/2))*(x-(w/2)) ) );
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			}
		} else if(bot == kfxSlantRBottom)
		{
			ymin = tb_Round(((float)h/2.0f) - (x));
			if(ymin<1) ymin = 1;
			if(ymin>(h/2)) ymin = (h/2);
		} else if(bot == kfxSlantLBottom)
		{
			ymin = tb_Round(((float)h/2.0f) - (w-x));
			if(ymin<1) ymin = 1;
			if(ymin>(h/2)) ymin = (h/2);
		}

		if(fxFire[index].shape == kfxFireCone)
		{
			if(x<w/2)
			{
				low = tb_Round((h/2.0f) * ((x*x)/(w*w/4.0f)));
				high = tb_Round((h-1) * ((x*x)/(w*w/4.0f)));
			}
			if(x>=w/2)
			{
				low = tb_Round((h/2.0f) * ((w-x)*(w-x)/(w*w/4.0f)));
				high = tb_Round((h-1) * ((w-x)*(w-x)/(w*w/4.0f)));
			}
		}

		if(fxFire[index].shape == kfxFireWall)
		{
			if(x<tb_Round((float)w/8.0f))
			{
				low = tb_Round((h/2.0f) * ((x*x)/(w*w/64.0f)));
				high = tb_Round((h-1) * ((x*x)/(w*w/64.0f)));
			} else
			if(x>(w-tb_Round((float)w/8.0f)))
			{
				low = tb_Round((h/2.0f) * ((w-x)*(w-x)/(w*w/64.0f)));
				high = tb_Round((h-1) * ((w-x)*(w-x)/(w*w/64.0f)));
			} else
			{
				low = tb_Round((h/2.0f));
				high = tb_Round((h-1));
			}
		}

		if(bot == kfxSlantRBottom)
		{
			low=tb_Round(2*low/3);
			high=tb_Round(2*high/3);
		} else if(bot == kfxSlantLBottom)
		{
			low=tb_Round(2*low/3);
			high=tb_Round(2*high/3);
		}

		p0 = &(fxFire[index].p[ymin * w + x]);
		p0->life = tb_Rnd(low, high);
		p0->age = 0;

		p0->ri = tb_Round(65535 * (float)tb_Rnd(90, 100)/100.0f);
		p0->gi = tb_Round(65535 * (float)tb_Rnd(80, 100)/100.0f);
		p0->bi = tb_Round(65535 * (float)tb_Rnd(50, 100)/100.0f);

		p0->rn = 0;
		p0->gn = 0;
		p0->bn = 0;
		p0->x = x;
		p0->y = ymin;
	}

	// generate some random puffs
	y = 0;
	for(i=0; i<20; i++)
	{
		x = tb_Rnd(4, w-5);

		if(bot == kfxFlatBottom)
			ymin = 0;
		else if(bot == kfxCornerBottom)
		{
			if(x<(w/2))
			{
				ymin = tb_Round(((float)h/2.0f) - (x));
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			} else
			{
				ymin = tb_Round(((float)h/2.0f) - (w-x));
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			}
		} else if(bot == kfxRoundBottom)
		{
			if(x<(w/2))
			{
				ymin = tb_Round( (h/2) - sqrt( (w/2)*(w/2) - ((w/2)-x)*((w/2)-x) ) );
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			} else
			{
				ymin = tb_Round( (h/2) -  sqrt( (w/2)*(w/2) - (x-(w/2))*(x-(w/2)) ) );
				if(ymin<1) ymin = 1;
				if(ymin>(h/2)) ymin = (h/2);
			}
		} else if(bot == kfxSlantRBottom)
		{
			ymin = tb_Round(((float)h/2.0f) - (x));
			if(ymin<1) ymin = 1;
			if(ymin>(h/2)) ymin = (h/2);
		} else if(bot == kfxSlantLBottom)
		{
			ymin = tb_Round(((float)h/2.0f) - (w-x));
			if(ymin<1) ymin = 1;
			if(ymin>(h/2)) ymin = (h/2);
		}

		y = tb_Rnd(ymin+5, tb_Round(0.8f*h));

		p0 = &(fxFire[index].p[y * w + x]);
		p0->ri = tb_Round((float)p0->ri*2.0f);
		p0->gi = tb_Round((float)p0->gi*2.0f);
		p0->bi = tb_Round((float)p0->bi*2.0f);
		if(p0->ri > 65535) p0->ri = 65535;
		if(p0->gi > 65535) p0->gi = 65535;
		if(p0->bi > 65535) p0->bi = 65535;

		p2 = &(fxFire[index].p[(y-1) * w + x]);
		memcpy(p2, p0, sizeof(TFXFireParticle));
		p2->y = (y-1);

		p2 = &(fxFire[index].p[(y+1) * w + x]);
		memcpy(p2, p0, sizeof(TFXFireParticle));
		p2->y = (y+1);

		p2 = &(fxFire[index].p[(y) * w + (x+1)]);
		memcpy(p2, p0, sizeof(TFXFireParticle));
		p2->x = (x+1);

		p2 = &(fxFire[index].p[(y) * w + (x-1)]);
		memcpy(p2, p0, sizeof(TFXFireParticle));
		p2->x = (x-1);
	}

	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			p0 = &(fxFire[index].p[y * w + x]);

			if ((p0->age < p0->life) && (p0->rn > 0 || p0->gn > 0 || p0->bn > 0))
			{
				d = 0;
				ra = 0;
				ga = 0;
				ba = 0;
				// get average
				ra += p0->rn;
				ga += p0->gn;
				ba += p0->bn;
				d++;

				if ((fxFire[index].blend == kfxBlendVert)	|| (fxFire[index].blend == kfxBlendAll))
				{
					if( (y-1) > 0 )
					{
						if(p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y-1) * w + x]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}

					if( (y+1) < h )
					{
						if(p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y+1) * w + x]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}


					if( (y-2) > 0 )
					{
						if(p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y-2) * w + x]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}

					if( (y+2) < h )
					{
						if(p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y+2) * w + x]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}
				}

				if ((fxFire[index].blend == kfxBlendHorz)	|| (fxFire[index].blend == kfxBlendAll))
				{
					if( (x+1) < w )
					{
						if(p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[y * w + (x+1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}

					if( (x-1) > 0 )
					{
						if(p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[y * w + (x-1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}
				}

				if ((fxFire[index].blend == kfxBlendDiagR)	|| (fxFire[index].blend == kfxBlendAll))
				{
					if( ((y-1) > 0) && ((x-1) > 0))
					{
						if(p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y-1) * w + (x-1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}

					if( ((y+1) < h) && ((x+1) < w))
					{
						if(p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y+1) * w + (x+1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}
				}

				if((fxFire[index].blend == kfxBlendDiagL)	||
				   (fxFire[index].blend == kfxBlendAll) )
				{

					if( ((y-1) > 0) && ((x+1) < w))
					{
						if(p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y-1) * w + (x+1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}

					if( ((y+1) < h) && ((x-1) > 0))
					{
						if(p0->age < p0->life)
						{
							p0 = &(fxFire[index].p[(y+1) * w + (x-1)]);
							ra += p0->rn;
							ga += p0->gn;
							ba += p0->bn;
							d++;
						}
					}
				}

				ra = ra/d;
				ga = ga/d;
				ba = ba/d;

				if ((ra > 5000 ||ga > 5000 || ba > 5000))
				{
					pth = fxFire[index].r.left + x;
					if(fxFire[index].invert)
						ptv = fxFire[index].r.top + y;
					else
						ptv = fxFire[index].r.top + (h-y);

					if (pth<=sr.left) 	continue;
					if (pth>=sr.right) 	continue;
					if (ptv<=sr.top) 		continue;
					if (ptv>=sr.bottom) continue;

					tb_GetPixelRGB(bitmap, pth, ptv, &rs, &gs, &bs);
					rs+=ra;
					gs+=ga;
					bs+=ba;
					rs=tb_Round(rs* (0.6f+(float)(ra+ga+ba)/(196605.0f)));
					gs=tb_Round(gs* (0.6f+(float)(ra+ga+ba)/(196605.0f)));
					bs=tb_Round(bs* (0.6f+(float)(ra+ga+ba)/(196605.0f)));

					if(rs>65535) rs = 65535;
					if(gs>65535) gs = 65535;
					if(bs>65535) bs = 65535;
					tb_SetPixelRGB(	bitmap, pth, ptv, rs, gs, bs);

				}

			}
		}
	}

}
*/

/////////////////////////////////////////////////////////////////////
// Grow the fire effect
void	fx_GrowFire(int index, int height)
{
	int									w,h;
	int									x,y;
	pTFXFireParticle 		p0, p1;//, p2;
	//unsigned int 				ra, ga, ba;
	//unsigned int 				rs, gs, bs;
	//int									d;
	int									low, high;
	//int									i,j,k;
	int									k;
	int 								bot;
	int 								ymin;
	int									grow=(height*0.40);

	if ((index<0) || (index>=_MAXEFFECTS))			// beta debug
		fi_WriteToErrorLog("Out of Range - fx_GrowFire");

	bot = fxFire[index].bottomShape;

	w = fxFire[index].w;

	h = fxFire[index].h;

	for (k=0;k<grow;k=k+1)
	{

		// copy row n to row n+1 from h down to 0
		// and adjust colors representing cooling effect
		for (x=0; x<w; x=x+1)
		{
			if (bot == kfxFlatBottom)
				ymin = 0;
			else if (bot == kfxCornerBottom)
			{
				if (x<(w/2))
				{
					ymin = tb_Round(((float)h/2.0f) - (x));

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
				else
				{
					ymin = tb_Round(((float)h/2.0f) - (w-x));

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
			}
			else if (bot == kfxRoundBottom)
			{
				if (x<(w/2))
				{
					ymin = tb_Round( (h/2) - sqrt( (w/2)*(w/2) - ((w/2)-x)*((w/2)-x) ) );

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
				else
				{
					ymin = tb_Round( (h/2) -  sqrt( (w/2)*(w/2) - (x-(w/2))*(x-(w/2)) ) );

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
			}
			else if (bot == kfxSlantRBottom)
			{
				ymin = tb_Round(((float)h/2.0f) - (x));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
			else if (bot == kfxSlantLBottom)
			{
				ymin = tb_Round(((float)h/2.0f) - (w-x));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}

			//for(y=h-1; y>ymin; y=y-1)
			for (y=k; y>ymin; y=y-1)
			{
				p0 = &(fxFire[index].p[(y-1) * fxFire[index].w + x]);
				p1 = &(fxFire[index].p[(y) * fxFire[index].w + x]);

				memcpy(p1, p0, sizeof(TFXFireParticle));

				p1->age++;

				if ( (p1->life > 0) &&
						(p1->life - p1->age) >= 0)
				{

					if (fxFire[index].clr == kfxRedFire)
					{
						// mostly red
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/3.0f);
					}
					else if (fxFire[index].clr == kfxGreenFire)
					{
						// mostly green
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
					}
					else if (fxFire[index].clr == kfxBlueFire)
					{
						// mostly blue
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
					}
					else if (fxFire[index].clr == kfxWhiteFire)
					{
						// kind of misty
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
					}

					if (p1->rn < 5000) p1->rn = 0;

					if (p1->gn < 5000) p1->gn = 0;

					if (p1->bn < 5000) p1->bn = 0;

					if (p1->rn > 65535) p1->rn = 65535;

					if (p1->gn > 65535) p1->gn = 65535;

					if (p1->bn > 65535) p1->bn = 65535;


				}
				else
				{
					p1->rn = 0;
					p1->gn = 0;
					p1->bn = 0;
				}
			}
		}

		// get new row 0 through 3 values
		y = 0;

		for (x=0; x<w; x=x+1)
		{

			if (bot == kfxFlatBottom)
				ymin = 0;
			else if (bot == kfxCornerBottom)
			{
				if (x<(w/2))
				{
					ymin = tb_Round(((float)h/2.0f) - (x));

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
				else
				{
					ymin = tb_Round(((float)h/2.0f) - (w-x));

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
			}
			else if (bot == kfxRoundBottom)
			{
				if (x<(w/2))
				{
					ymin = tb_Round( (h/2) - sqrt( (w/2)*(w/2) - ((w/2)-x)*((w/2)-x) ) );

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
				else
				{
					ymin = tb_Round( (h/2) -  sqrt( (w/2)*(w/2) - (x-(w/2))*(x-(w/2)) ) );

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
			}
			else if (bot == kfxSlantRBottom)
			{
				ymin = tb_Round(((float)h/2.0f) - (x));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
			else if (bot == kfxSlantLBottom)
			{
				ymin = tb_Round(((float)h/2.0f) - (w-x));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}

			if (fxFire[index].shape == kfxFireCone)
			{
				if (x<w/2)
				{
					low = tb_Round((h/2.0f) * ((x*x)/(w*w/4.0f)));
					high = tb_Round((h-1) * ((x*x)/(w*w/4.0f)));
				}

				if (x>=w/2)
				{
					low = tb_Round((h/2.0f) * ((w-x)*(w-x)/(w*w/4.0f)));
					high = tb_Round((h-1) * ((w-x)*(w-x)/(w*w/4.0f)));
				}
			}

			if (fxFire[index].shape == kfxFireWall)
			{
				if (x<tb_Round((float)w/8.0f))
				{
					low = tb_Round((h/2.0f) * ((x*x)/(w*w/64.0f)));
					high = tb_Round((h-1) * ((x*x)/(w*w/64.0f)));
				}
				else
					if (x>(w-tb_Round((float)w/8.0f)))
					{
						low = tb_Round((h/2.0f) * ((w-x)*(w-x)/(w*w/64.0f)));
						high = tb_Round((h-1) * ((w-x)*(w-x)/(w*w/64.0f)));
					}
					else
					{
						low = tb_Round((h/2.0f));
						high = tb_Round((h-1));
					}
			}

			if (bot == kfxSlantRBottom)
			{
				low=tb_Round(2*low/3);
				high=tb_Round(2*high/3);
			}
			else if (bot == kfxSlantLBottom)
			{
				low=tb_Round(2*low/3);
				high=tb_Round(2*high/3);
			}

			p0 = &(fxFire[index].p[ymin * w + x]);

			p0->life = tb_Rnd(low, high);
			p0->age = 0;

			p0->ri = tb_Round(65535 * (float)tb_Rnd(90, 100)/100.0f);
			p0->gi = tb_Round(65535 * (float)tb_Rnd(80, 100)/100.0f);
			p0->bi = tb_Round(65535 * (float)tb_Rnd(50, 100)/100.0f);

			p0->rn = 0;
			p0->gn = 0;
			p0->bn = 0;
			p0->x = x;
			p0->y = ymin;
		}
	}
}

void	fx_PrimeFlames(int index)
{
	int									w,h;
	int									x,y;
	pTFXFireParticle 		p0, p1;//, p2;
	//unsigned int 				ra, ga, ba;
	//unsigned int 				rs, gs, bs;
	//int									d;
	int									low, high;
	int									i;//,j;
	int 								bot;
	int 								ymin;
	int									x1;
	int									grow;

	if ((index<0) || (index>=_MAXEFFECTS))			// beta debug
		fi_WriteToErrorLog("Out of Range - fx_PrimeFlames");

	bot = fxFire[index].bottomShape;

	w = fxFire[index].w;

	h = fxFire[index].h;

	grow=(h*0.40);

	for (i=0; i<grow; i++)
	{

		// copy row n to row n+1 from h down to 0
		// and adjust colors representing cooling effect
		for (x=0; x<w-1; x+=2)
		{
			// going once
			if (bot == kfxFlatBottom)
				ymin = 0;
			else if (bot == kfxCornerBottom)
			{
				if (x<(w/2))
				{
					ymin = tb_Round(((float)h/2.0f) - (x));

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
				else
				{
					ymin = tb_Round(((float)h/2.0f) - (w-x));

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
			}
			else if (bot == kfxRoundBottom)
			{
				if (x<(w/2))
				{
					ymin = tb_Round( (h/2) - sqrt( (w/2)*(w/2) - ((w/2)-x)*((w/2)-x) ) );

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
				else
				{
					ymin = tb_Round( (h/2) -  sqrt( (w/2)*(w/2) - (x-(w/2))*(x-(w/2)) ) );

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
			}
			else if (bot == kfxSlantRBottom)
			{
				ymin = tb_Round(((float)h/2.0f) - (x));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
			else if (bot == kfxSlantLBottom)
			{
				ymin = tb_Round(((float)h/2.0f) - (w-x));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}

			//for(y=h-1; y>ymin; y--)
			for (y=i; y>ymin; y=y-1)
			{
				p0 = &(fxFire[index].p[(y-1) * fxFire[index].w + x]);
				p1 = &(fxFire[index].p[(y) * fxFire[index].w + x]);

				memcpy(p1, p0, sizeof(TFXFireParticle));

				p1->age++;

				if ( (p1->life > 0) &&
						(p1->life - p1->age) >= 0)
				{

					if (fxFire[index].clr == kfxRedFire)
					{
						// mostly red
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/3.0f);
					}
					else if (fxFire[index].clr == kfxGreenFire)
					{
						// mostly green
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
					}
					else if (fxFire[index].clr == kfxBlueFire)
					{
						// mostly blue
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
					}
					else if (fxFire[index].clr == kfxWhiteFire)
					{
						// kind of misty
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
					}

					if (p1->rn < 5000) p1->rn = 0;

					if (p1->gn < 5000) p1->gn = 0;

					if (p1->bn < 5000) p1->bn = 0;

					if (p1->rn > 65535) p1->rn = 65535;

					if (p1->gn > 65535) p1->gn = 65535;

					if (p1->bn > 65535) p1->bn = 65535;


				}
				else
				{
					p1->rn = 0;
					p1->gn = 0;
					p1->bn = 0;
				}
			}

			// get new row 0 values
			if (fxFire[index].shape == kfxFireCone)
			{
				if (x<w/2)
				{
					low = tb_Round((h/2.0f) * ((x*x)/(w*w/4.0f)));
					high = tb_Round((h-1) * ((x*x)/(w*w/4.0f)));
				}

				if (x>=w/2)
				{
					low = tb_Round((h/2.0f) * ((w-x)*(w-x)/(w*w/4.0f)));
					high = tb_Round((h-1) * ((w-x)*(w-x)/(w*w/4.0f)));
				}
			}

			if (fxFire[index].shape == kfxFireWall)
			{
				if (x<tb_Round((float)w/8.0f))
				{
					low = tb_Round((h/2.0f) * ((x*x)/(w*w/64.0f)));
					high = tb_Round((h-1) * ((x*x)/(w*w/64.0f)));
				}
				else
					if (x>(w-tb_Round((float)w/8.0f)))
					{
						low = tb_Round((h/2.0f) * ((w-x)*(w-x)/(w*w/64.0f)));
						high = tb_Round((h-1) * ((w-x)*(w-x)/(w*w/64.0f)));
					}
					else
					{
						low = tb_Round((h/2.0f));
						high = tb_Round((h-1));
					}
			}

			if (bot == kfxSlantRBottom)
			{
				low=tb_Round(2*low/3);
				high=tb_Round(2*high/3);
			}
			else if (bot == kfxSlantLBottom)
			{
				low=tb_Round(2*low/3);
				high=tb_Round(2*high/3);
			}

			p0 = &(fxFire[index].p[ymin * w + x]);

			p0->life = tb_Rnd(low, high);
			p0->age = 0;

			p0->ri = tb_Round(65535 * (float)tb_Rnd(90, 100)/100.0f);
			p0->gi = tb_Round(65535 * (float)tb_Rnd(80, 100)/100.0f);
			p0->bi = tb_Round(65535 * (float)tb_Rnd(50, 100)/100.0f);

			p0->rn = 0;
			p0->gn = 0;
			p0->bn = 0;
			p0->x = x;
			p0->y = ymin;




			// going twice
			x1=x+1;

			if (bot == kfxFlatBottom)
				ymin = 0;
			else if (bot == kfxCornerBottom)
			{
				if (x1<(w/2))
				{
					ymin = tb_Round(((float)h/2.0f) - (x1));

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
				else
				{
					ymin = tb_Round(((float)h/2.0f) - (w-x1));

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
			}
			else if (bot == kfxRoundBottom)
			{
				if (x1<(w/2))
				{
					ymin = tb_Round( (h/2) - sqrt( (w/2)*(w/2) - ((w/2)-x1)*((w/2)-x1) ) );

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
				else
				{
					ymin = tb_Round( (h/2) -  sqrt( (w/2)*(w/2) - (x1-(w/2))*(x1-(w/2)) ) );

					if (ymin<1) ymin = 1;

					if (ymin>(h/2)) ymin = (h/2);
				}
			}
			else if (bot == kfxSlantRBottom)
			{
				ymin = tb_Round(((float)h/2.0f) - (x1));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}
			else if (bot == kfxSlantLBottom)
			{
				ymin = tb_Round(((float)h/2.0f) - (w-x1));

				if (ymin<1) ymin = 1;

				if (ymin>(h/2)) ymin = (h/2);
			}

			for (y=h-1; y>ymin; y--)
			{
				p0 = &(fxFire[index].p[(y-1) * fxFire[index].w + x1]);
				p1 = &(fxFire[index].p[(y) * fxFire[index].w + x1]);

				memcpy(p1, p0, sizeof(TFXFireParticle));

				p1->age++;

				if ( (p1->life > 0) &&
						(p1->life - p1->age) >= 0)
				{

					if (fxFire[index].clr == kfxRedFire)
					{
						// mostly red
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/3.0f);
					}
					else if (fxFire[index].clr == kfxGreenFire)
					{
						// mostly green
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
					}
					else if (fxFire[index].clr == kfxBlueFire)
					{
						// mostly blue
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/1.9f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/1.5f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))*1.5f);
					}
					else if (fxFire[index].clr == kfxWhiteFire)
					{
						// kind of misty
						p1->rn = tb_Round(p1->ri * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
						p1->gn = tb_Round(p1->gi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
						p1->bn = tb_Round(p1->bi * (float)((p1->life - p1->age)/(float)(p1->life))/2.0f);
					}

					if (p1->rn < 5000) p1->rn = 0;

					if (p1->gn < 5000) p1->gn = 0;

					if (p1->bn < 5000) p1->bn = 0;

					if (p1->rn > 65535) p1->rn = 65535;

					if (p1->gn > 65535) p1->gn = 65535;

					if (p1->bn > 65535) p1->bn = 65535;


				}
				else
				{
					p1->rn = 0;
					p1->gn = 0;
					p1->bn = 0;
				}
			}

			// get new row 0 values
			if (fxFire[index].shape == kfxFireCone)
			{
				if (x1<w/2)
				{
					low = tb_Round((h/2.0f) * ((x1*x1)/(w*w/4.0f)));
					high = tb_Round((h-1) * ((x1*x1)/(w*w/4.0f)));
				}

				if (x1>=w/2)
				{
					low = tb_Round((h/2.0f) * ((w-x1)*(w-x1)/(w*w/4.0f)));
					high = tb_Round((h-1) * ((w-x1)*(w-x1)/(w*w/4.0f)));
				}
			}

			if (fxFire[index].shape == kfxFireWall)
			{
				if (x1<tb_Round((float)w/8.0f))
				{
					low = tb_Round((h/2.0f) * ((x1*x1)/(w*w/64.0f)));
					high = tb_Round((h-1) * ((x1*x1)/(w*w/64.0f)));
				}
				else
					if (x1>(w-tb_Round((float)w/8.0f)))
					{
						low = tb_Round((h/2.0f) * ((w-x1)*(w-x1)/(w*w/64.0f)));
						high = tb_Round((h-1) * ((w-x1)*(w-x1)/(w*w/64.0f)));
					}
					else
					{
						low = tb_Round((h/2.0f));
						high = tb_Round((h-1));
					}
			}

			if (bot == kfxSlantRBottom)
			{
				low=tb_Round(2*low/3);
				high=tb_Round(2*high/3);
			}
			else if (bot == kfxSlantLBottom)
			{
				low=tb_Round(2*low/3);
				high=tb_Round(2*high/3);
			}

			p0 = &(fxFire[index].p[ymin * w + x1]);

			p0->life = tb_Rnd(low, high);
			p0->age = 0;

			p0->ri = tb_Round(65535 * (float)tb_Rnd(90, 100)/100.0f);
			p0->gi = tb_Round(65535 * (float)tb_Rnd(80, 100)/100.0f);
			p0->bi = tb_Round(65535 * (float)tb_Rnd(50, 100)/100.0f);

			p0->rn = 0;
			p0->gn = 0;
			p0->bn = 0;
			p0->x = x1;
			p0->y = ymin;


		}



	}

}
