#include "Keys.h"
#include "Remap.h"
#include "Globals.h"
#include "Creatures.h"
#include "Players.h"
#include "Action.h"

UInt8				        chessBoard[kMaxChessBoards][96];
int                 chessBoardRow1[kMaxChessBoards];
int                 chessBoardCol1[kMaxChessBoards];
int                 chessBoardRow2[kMaxChessBoards];
int                 chessBoardCol2[kMaxChessBoards];

extern int					actionValue;
extern UInt8				localChess[96];

#ifdef _SERVER
extern UInt16									creatureMap				[kMapWidth][kMapHeight];
extern UInt16									map[kMapWidth][kMapHeight];
#endif

// ----------------------------------------------------------------- //
TBoolean ac_AnythingInActionDisplay(void)
// ----------------------------------------------------------------- //

{
	int         i;

	if (actionValue==929)	// chess
	{
		for (i=0;i<96;i++)
			if (localChess[i]!=0)
				return (true);
	}

	return (false);

}

#ifdef _SERVER
// ----------------------------------------------------------------- //
void ac_AddChessTable(int r, int c)	// server
// ----------------------------------------------------------------- //

{
	int						i;

	for (i=0;i<kMaxChessBoards;i++)
		if (chessBoardRow1[i]==0)
		{
			chessBoardRow1[i]=r-1;
			chessBoardCol1[i]=c;
			chessBoardRow2[i]=r+1;
			chessBoardCol2[i]=c;
			return;
		}

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void ac_InitializeActions(void)	// server
// ----------------------------------------------------------------- //

{
	int						i;
	int						j;
	int						r;
	int						c;

	for (i=0;i<kMaxChessBoards;i++)
	{
		chessBoardRow1[i]=0;
		chessBoardCol1[i]=0;
		chessBoardRow2[i]=0;
		chessBoardCol2[i]=0;

		for (j=0;j<96;j++)
			chessBoard[i][j]=0;
	}

	for (r=0;r<kMapWidth;r++)
		for (c=0;c<kMapHeight;c++)
			if ((map[r][c]==928) || (map[r][c]==938) || (map[r][c]==974))
				ac_AddChessTable(r,c);

	/*
	  // chess board 1
	  chessBoardRow1[0]=2188;
	  chessBoardCol1[0]=1803;
	  chessBoardRow2[0]=2190;
	  chessBoardCol2[0]=1803;

	  // chess board 2
	  chessBoardRow1[1]=2186;
	  chessBoardCol1[1]=1803;
	  chessBoardRow2[1]=2184;
	  chessBoardCol2[1]=1803;

	  // chess board 3
	  chessBoardRow1[2]=2203;
	  chessBoardCol1[2]=1929;
	  chessBoardRow2[2]=2203;
	  chessBoardCol2[2]=1931;

	  // chess board 4  ... mariranger
	  chessBoardRow1[3]=2285;
	  chessBoardCol1[3]=1939;
	  chessBoardRow2[3]=2283;
	  chessBoardCol2[3]=1939;
	*/
	for (i=0;i<kMaxChessBoards;i++)
	{
		chessBoard[i][0]	=	kBlackRook;
		chessBoard[i][1]	=	kBlackKnight;
		chessBoard[i][2]	=	kBlackBishop;
		chessBoard[i][3]	=	kBlackKing;
		chessBoard[i][4]	=	kBlackQueen;
		chessBoard[i][5]	=	kBlackBishop;
		chessBoard[i][6]	=	kBlackKnight;
		chessBoard[i][7]	=	kBlackRook;

		chessBoard[i][8]	=	kBlackPawn;
		chessBoard[i][9]	=	kBlackPawn;
		chessBoard[i][10]	=	kBlackPawn;
		chessBoard[i][11]	=	kBlackPawn;
		chessBoard[i][12]	=	kBlackPawn;
		chessBoard[i][13]	=	kBlackPawn;
		chessBoard[i][14]	=	kBlackPawn;
		chessBoard[i][15]	=	kBlackPawn;

		chessBoard[i][48]	=	kWhitePawn;
		chessBoard[i][49]	=	kWhitePawn;
		chessBoard[i][50]	=	kWhitePawn;
		chessBoard[i][51]	=	kWhitePawn;
		chessBoard[i][52]	=	kWhitePawn;
		chessBoard[i][53]	=	kWhitePawn;
		chessBoard[i][54]	=	kWhitePawn;
		chessBoard[i][55]	=	kWhitePawn;

		chessBoard[i][56]	=	kWhiteRook;
		chessBoard[i][57]	=	kWhiteKnight;
		chessBoard[i][58]	=	kWhiteBishop;
		chessBoard[i][59]	=	kWhiteKing;
		chessBoard[i][60]	=	kWhiteQueen;
		chessBoard[i][61]	=	kWhiteBishop;
		chessBoard[i][62]	=	kWhiteKnight;
		chessBoard[i][63]	=	kWhiteRook;
	}

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void ac_ChessMove(int i, int moveFrom, int moveTo)	// server
// ----------------------------------------------------------------- //

{
	int         index=-1;
	int         j;
	int         c1;
	int         c2;
	int         thePiece;

	if ((moveFrom<0) || (moveFrom>=96))
		return;

	if ((moveTo<0) || (moveTo>=96))
		return;

	// -- find the chess board
	for (j=0;j<kMaxChessBoards;j++)
	{
		if ((creature[player[i].creatureIndex].row==chessBoardRow1[j]) && (creature[player[i].creatureIndex].col==chessBoardCol1[j]))
		{
			index=j;
			break;
		}
		else if ((creature[player[i].creatureIndex].row==chessBoardRow2[j]) && (creature[player[i].creatureIndex].col==chessBoardCol2[j]))
		{
			index=j;
			break;
		}
	}

	// -- make the move
	if (index>=0)
	{
		thePiece=chessBoard[index][moveFrom];
		chessBoard[index][moveFrom]=0;

		if (chessBoard[index][moveTo]!=0)
		{
			for (j=64;j<96;j++)
				if (chessBoard[index][j]==0)
				{
					chessBoard[index][j]=chessBoard[index][moveTo];
					break;
				}
		}

		chessBoard[index][moveTo]=thePiece;
	}

	// -- Send update to players
	if (index>=0)
	{
		c1=creatureMap[chessBoardRow1[index]][chessBoardCol1[index]];

		if (c1!=0)
			if (creature[c1].playerIndex!=0)
				nw_SendActionUpdate(creature[c1].playerIndex);

		c2=creatureMap[chessBoardRow2[index]][chessBoardCol2[index]];

		if (c2!=0)
			if (creature[c2].playerIndex!=0)
				nw_SendActionUpdate(creature[c2].playerIndex);
	}

}

#endif

#ifdef _SERVER
// ----------------------------------------------------------------- //
void ac_GetActionType(int i, int *action, int *index)	// server
// ----------------------------------------------------------------- //

{
	int           j;


	// -- find the chess board

	for (j=0;j<kMaxChessBoards;j++)
	{
		if ((creature[player[i].creatureIndex].row==chessBoardRow1[j]) && (creature[player[i].creatureIndex].col==chessBoardCol1[j]))
		{
			*action=929;
			*index=j;
			return;
		}

		if ((creature[player[i].creatureIndex].row==chessBoardRow2[j]) && (creature[player[i].creatureIndex].col==chessBoardCol2[j]))
		{
			*action=929;
			*index=j;
			return;
		}
	}

	*action=0;

	*index=0;

}

#endif
