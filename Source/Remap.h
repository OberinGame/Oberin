#ifndef _REMAP_FILE
#define _REMAP_FILE

#define TRect																	Rect
#define TPoint																Point
#define TBoolean															Boolean
#define begin																	{
#define end																		}
#define	tb_SetRect														SetRect
#define	tb_PointInRect												PtInRect
#define tb_GetMousePosition		    						GetMouse
#define tb_InsetRect													InsetRect
#define	TBitmap																int
#define	TWindow																int
#define TRegion																int
#define tb_IntersectRect(r1,r2,r3)  					SectRect(r3,r2,r1)
#define tb_OffsetRect(r, x, y)  							OffsetRect(r, x, y)
#define tb_Rnd(min, max)  										(((abs(Random())%(max-min+1))+min))
#define tb_Round(f)  													((int)(f+0.5))

#define RC(v,r)  															v

#define MIN_UTIL(a, b) ((a) < (b) ? (a) : (b))
#define MAX_UTIL(a, b) ((a) > (b) ? (a) : (b))

//#define tb_SetRect(r, a, b, c, d)	\
//	r.left = a;										\
//	r.right = c;										\
//	r.top = b;											\
//	r.bottom = d

//#define tb_OffsetRect(r, x, y)		\
//	r.left += x;										\
//	r.right += x;									\
//	r.top += y;										\
//	r.bottom += y

#define tb_UnionRect(s, d, u)									\
	*u.top    = MIN_UTIL(*s.top, *d.top);				\
	*u.left   = MIN_UTIL(*s.left, *d.left);			\
	*u.bottom = MAX_UTIL(*s.bottom, *d.bottom);	\
	*u.right  = MAX_UTIL(*s.right, *d.right)

#endif  //  _REMAP_FILE