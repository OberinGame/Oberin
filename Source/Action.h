#ifndef _ACTION
#define _ACTION

#include "Keys.h"
#include "Remap.h"


#define		kMaxChessBoards											35

#define		kBlackKing													1
#define		kBlackQueen													2
#define		kBlackRook													3
#define		kBlackBishop												4
#define		kBlackKnight												5
#define		kBlackPawn													6
#define		kWhiteKing													7
#define		kWhiteQueen													8
#define		kWhiteRook													9
#define		kWhiteBishop												10
#define		kWhiteKnight												11
#define		kWhitePawn													12


TBoolean  ac_AnythingInActionDisplay          (void);
void 			ac_InitializeActions								(void);	// server
void 			ac_GetActionType										(int i, int *action, int *index);	// server
void      ac_ChessMove                        (int i, int moveFrom, int moveTo);	// server
void 			ac_AddChessTable										(int r, int c);	// server


#endif