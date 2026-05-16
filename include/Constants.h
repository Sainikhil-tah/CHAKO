// to make the header include only once
// including this will stop redefination of things in the header
#pragma once 
const int BOARD_SIZE=8;
const int WHITE=0;
const int BLACK=1;
const char empty='.';
const int DEFAULT_DEPTH=4;

//we use capital letters for white 
//we use small letters for black 
//dont get confused as we are using 0 for white and capital letters for white 
//i used this because ASCII(A)<ASCII(a)

// white piece defination 
const char W_KING='K',W_QUEEN='Q',W_ROOK='R',W_BISHOP='B',W_KNIGHT='N',W_PAWN='P';

// black piece defination
const char B_KING='k',B_QUEEN='q',B_ROOK='r',B_BISHOP='b',B_KNIGHT='n',B_PAWN='p';

// castling rights 
// castling possible for white on WQ white castle on queen side WK white castle on king side , similarly for black
// we use castling rights with 1,2,4,8 because they are single bith numbers 
// 1->0001 2->0010 4->0100 8->1000
// we use this because it will be simple to remove rights using &~
// initally we will have future chance of castling for every side as no king and respective rooks are moved 
// when king on white moved we de 15&~(1|2) 15&~(3) ->1100 only black have future castling rights 
// when rook on row 7 col 0  is moved i.e white root we romve 15&~(1) as now we cant castle that side 
// This Bitwise representation make this very easy to keep track of future castling rights 

// en-passent is -1 as it is not possible at the very start so keep -1 not a valid column 
// when ever it is possible change it to the valid col number
const int CASTLE_WK=1,CASTLE_WQ=2,CASTLE_BK=4,CASTLE_BQ=8;
const int NO_EP=-1;

// material value for evaluation (copied from WEB for maximum accuraccy)
// Evaluation scores
const int INF_SCORE = 100000;
const int CHECKMATE_SCORE = 9000;
const int STALEMATE_SCORE = 0;

// Material values (for evaluation)
const int VAL_PAWN = 100;
const int VAL_KNIGHT = 320;
const int VAL_BISHOP = 330;
const int VAL_ROOK = 500;
const int VAL_QUEEN = 900;
const int VAL_KING = 20000;
