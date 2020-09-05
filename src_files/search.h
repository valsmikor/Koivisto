//
// Created by finne on 5/30/2020.
//

#ifndef KOIVISTO_SEARCH_H
#define KOIVISTO_SEARCH_H

#include <iostream>
#include <cmath>
#include <tgmath.h>
#include <chrono>
#include <ctime>
#include <cassert>
#include <stdint.h>
#include <string>
#include "Bitboard.h"
#include "Board.h"
//#include "Move.h"
#include "TranspositionTable.h"
#include "eval.h"
#include "MoveOrderer.h"
#include "History.h"
#include "TimeManager.h"

extern int _threadCount;

extern int RAZOR_MARGIN;
extern int FUTILITY_MARGIN;
extern int SE_MARGIN_STATIC;
extern int LMR_DIV;

#define MAX_THREADS 128

/**
 * used to store information about a search
 */
struct SearchOverview{
    int nodes;
    Score score;
    int depth;
    int time;
    Move move;
};


void initLmr();
Score getWDL(Board* board);
void search_stop();
void search_setHashSize(int hashSize);
void search_clearHash();
void search_useTB(bool val);
void search_init(int hashSize);     //used to create arrays, movelists etc
void search_cleanUp();              //used to clean up the memory
SearchOverview search_overview();             //used to get information about the latest search
void search_enable_infoStrings();
void search_disable_inforStrings();


Move bestMove(Board *b, Depth maxDepth, TimeManager *timeManager, int threadId = 1);
Score pvSearch(Board *b, Score alpha, Score beta, Depth depth, Depth ply, SearchData *sd, Move skipMove);
Score  qSearch(Board *b, Score alpha, Score beta, Depth ply, SearchData *sd);



#endif //KOIVISTO_SEARCH_H
