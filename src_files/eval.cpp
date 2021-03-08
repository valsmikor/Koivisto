
/****************************************************************************************************
 *                                                                                                  *
 *                                     Koivisto UCI Chess engine                                    *
 *                           by. Kim Kahre, Finn Eggers and Eugenio Bruno                           *
 *                                                                                                  *
 *                 Koivisto is free software: you can redistribute it and/or modify                 *
 *               it under the terms of the GNU General Public License as published by               *
 *                 the Free Software Foundation, either version 3 of the License, or                *
 *                                (at your option) any later version.                               *
 *                    Koivisto is distributed in the hope that it will be useful,                   *
 *                  but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 *                   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
 *                           GNU General Public License for more details.                           *
 *                 You should have received a copy of the GNU General Public License                *
 *                 along with Koivisto.  If not, see <http://www.gnu.org/licenses/>.                *
 *                                                                                                  *
 ****************************************************************************************************/

#include "eval.h"

#include <immintrin.h>
#include <iomanip>


EvalScore SIDE_TO_MOVE                  = M(   10,   15);
EvalScore PAWN_STRUCTURE                = M(   10,    4);
EvalScore PAWN_PASSED                   = M(    1,   47);
EvalScore PAWN_ISOLATED                 = M(   -2,  -15);
EvalScore PAWN_DOUBLED                  = M(   -7,   -5);
EvalScore PAWN_DOUBLED_AND_ISOLATED     = M(   -8,  -28);
EvalScore PAWN_BACKWARD                 = M(  -12,    1);
EvalScore PAWN_OPEN                     = M(  -10,   -6);
EvalScore PAWN_BLOCKED                  = M(   -4,  -15);
EvalScore KNIGHT_OUTPOST                = M(   26,   19);
EvalScore KNIGHT_DISTANCE_ENEMY_KING    = M(   -4,   -1);
EvalScore ROOK_OPEN_FILE                = M(   25,   -2);
EvalScore ROOK_HALF_OPEN_FILE           = M(    1,  -13);
EvalScore ROOK_KING_LINE                = M(    3,    9);
EvalScore BISHOP_DOUBLED                = M(   16,   81);
EvalScore BISHOP_FIANCHETTO             = M(   -4,    3);
EvalScore BISHOP_PIECE_SAME_SQUARE_E    = M(    3,    6);
EvalScore QUEEN_DISTANCE_ENEMY_KING     = M(    3,  -35);
EvalScore KING_CLOSE_OPPONENT           = M(  -29,   21);
EvalScore KING_PAWN_SHIELD              = M(   31,   12);
EvalScore CASTLING_RIGHTS               = M(   22,   -5);
EvalScore MINOR_BEHIND_PAWN             = M(    6,   25);

EvalScore mobilityKnight[9] = {
    M(  -49,    3), M(  -38,   60), M(  -33,   92), M(  -29,  111), M(  -25,  123),
    M(  -20,  134), M(  -14,  136), M(   -4,  129), M(   11,  114), };

EvalScore mobilityBishop[14] = {
    M(  -10,  -35), M(    1,   38), M(    8,   73), M(   12,   95), M(   18,  112),
    M(   21,  127), M(   21,  134), M(   20,  139), M(   22,  143), M(   25,  142),
    M(   31,  137), M(   47,  129), M(   53,  139), M(   75,  110), };

EvalScore mobilityRook[15] = {
    M(  -45,   81), M(  -38,  128), M(  -35,  166), M(  -34,  196), M(  -33,  211),
    M(  -27,  218), M(  -22,  226), M(  -14,  228), M(   -7,  233), M(   -1,  238),
    M(    5,  241), M(   10,  244), M(   19,  241), M(   44,  223), M(  102,  191), };

EvalScore mobilityQueen[28] = {
    M( -175,  179), M( -158,  182), M( -145,  316), M( -141,  400), M( -139,  448),
    M( -139,  479), M( -138,  505), M( -136,  523), M( -135,  537), M( -133,  546),
    M( -131,  555), M( -129,  562), M( -128,  566), M( -128,  574), M( -129,  580),
    M( -131,  584), M( -132,  586), M( -134,  588), M( -134,  589), M( -127,  583),
    M( -121,  576), M( -120,  571), M( -127,  569), M( -113,  560), M( -178,  589),
    M(  -51,  518), M(  -90,  571), M( -143,  602), };

EvalScore hangingEval[5] = {
    M(   -3,   -0), M(   -4,   -0), M(   -6,   -6), M(   -6,   -6), M(   -5,   -9), };

EvalScore pinnedEval[15] = {
    M(    3,   -9), M(   -4,    4), M(   -3,   34), M(  -21,  -64), M(  -27,  -14),
    M(  -15,   30), M(   -1,  -10), M(  -28,  -17), M(  -11,   17), M(  -53,  -36),
    M(    2,  -12), M(   -3,   18), M(   -6,  -87), M(  -39,  -63), M(   -9,   21), };

EvalScore passer_rank_n[16] = {
    M(    0,    0), M(   -7,  -29), M(  -13,  -25), M(   -8,    4),
    M(   22,   31), M(   43,   92), M(   25,   55), M(    0,    0),
    M(    0,    0), M(    1,    2), M(  -30,  -61), M(  -28,  -14),
    M(  -15,  -11), M(   34,   17), M(  100, -114), M(    0,    0), };

EvalScore bishop_pawn_same_color_table_o[9] = {
    M(  -32,   45), M(  -39,   47), M(  -39,   32),
    M(  -43,   22), M(  -47,   12), M(  -52,   -1),
    M(  -56,  -20), M(  -57,  -37), M(  -68,  -87), };

EvalScore bishop_pawn_same_color_table_e[9] = {
    M(  -20,   43), M(  -37,   51), M(  -43,   40),
    M(  -49,   31), M(  -54,   19), M(  -58,    2),
    M(  -61,  -20), M(  -59,  -37), M(  -67,  -54), };

EvalScore* evfeatures[] {
    &SIDE_TO_MOVE,
    &PAWN_STRUCTURE,
    &PAWN_PASSED,
    &PAWN_ISOLATED,
    &PAWN_DOUBLED,
    &PAWN_DOUBLED_AND_ISOLATED,
    &PAWN_BACKWARD,
    &PAWN_OPEN,
    &PAWN_BLOCKED,
    
    &KNIGHT_OUTPOST,
    &KNIGHT_DISTANCE_ENEMY_KING,
    
    &ROOK_OPEN_FILE,
    &ROOK_HALF_OPEN_FILE,
    &ROOK_KING_LINE,
    
    &BISHOP_DOUBLED,
    &BISHOP_FIANCHETTO,
    &BISHOP_PIECE_SAME_SQUARE_E,
    
    &QUEEN_DISTANCE_ENEMY_KING,
    
    &KING_CLOSE_OPPONENT,
    &KING_PAWN_SHIELD,
    
    &CASTLING_RIGHTS,

    &MINOR_BEHIND_PAWN,
    
};

int kingDangerFactors[KING_DANGER_FACTORS_COUNT]{
    1139, 680, 2121, 2819, 2166, 2988, 734,
};
int kingSafetyAttackWeights[N_PIECE]{
    0, 2, 4, 4, 1, -3,
};


int mobEntryCount[6] {0, 9, 14, 15, 28, 0};

float* phaseValues = new float[6] {
    0, 1, 1, 2, 4, 0,
};


EvalScore* mobilities[6] {nullptr, mobilityKnight, mobilityBishop, mobilityRook, mobilityQueen, nullptr};

bool hasMatingMaterial(Board* b, bool side) {
    if ((b->getPieces()[QUEEN + side * 6] | b->getPieces()[ROOK + side * 6] | b->getPieces()[PAWN + side * 6])
        || (bitCount(b->getPieces()[BISHOP + side * 6] | b->getPieces()[KNIGHT + side * 6]) > 1
            && b->getPieces()[BISHOP + side * 6]))
        return true;
    return false;
}

void addToKingSafety(U64 attacks, U64 kingZone, int& pieceCount, int& valueOfAttacks, int factor) {
    if (attacks & kingZone) {
        pieceCount++;
        valueOfAttacks += factor * bitCount(attacks & kingZone);
    }
}

/**
 * checks if the given square is an outpost given the color and a bitboard of the opponent pawns
 */
bool isOutpost(Square s, Color c, U64 opponentPawns, U64 pawnCover) {
    U64 sq = ONE << s;

    if (c == WHITE) {
        if (((whitePassedPawnMask[s] & ~FILES[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover)) {
            return true;
        }
    } else {
        if (((blackPassedPawnMask[s] & ~FILES[fileIndex(s)]) & opponentPawns) == 0 && (sq & pawnCover)) {
            return true;
        }
    }
    return false;
}

bb::Score Evaluator::evaluateTempo(Board* b){
    phase = (24.0f + phaseValues[5] - phaseValues[0] * bitCount(b->getPieces()[WHITE_PAWN] | b->getPieces()[BLACK_PAWN])
        - phaseValues[1] * bitCount(b->getPieces()[WHITE_KNIGHT] | b->getPieces()[BLACK_KNIGHT])
        - phaseValues[2] * bitCount(b->getPieces()[WHITE_BISHOP] | b->getPieces()[BLACK_BISHOP])
        - phaseValues[3] * bitCount(b->getPieces()[WHITE_ROOK] | b->getPieces()[BLACK_ROOK])
        - phaseValues[4] * bitCount(b->getPieces()[WHITE_QUEEN] | b->getPieces()[BLACK_QUEEN]))
        / 24.0f;

    if (phase > 1)
        phase = 1;
    if (phase < 0)
        phase = 0;

   return MgScore(SIDE_TO_MOVE) * (1 - phase) + EgScore(SIDE_TO_MOVE) * (phase);
}

EvalScore Evaluator::computeHangingPieces(Board* b) {
    U64 WnotAttacked = ~b->getAttackedSquares(WHITE);
    U64 BnotAttacked = ~b->getAttackedSquares(BLACK);

    EvalScore res = M(0, 0);

    for (int i = PAWN; i <= QUEEN; i++) {
        res += hangingEval[i]
               * (+bitCount(b->getPieces(WHITE, i) & WnotAttacked) - bitCount(b->getPieces(BLACK, i) & BnotAttacked));
    }
    return res;
}

EvalScore Evaluator::computePinnedPieces(Board* b, Color color) {
    
    
    EvalScore result = 0;
    
    Color us = color;
    Color them = 1 - color;
    
    // figure out where the opponent has pieces
    U64 opponentOcc = b->getTeamOccupied()[them];
    U64      ourOcc = b->getTeamOccupied()[us];
    
    // get the pieces which can pin our king
    U64 bishops = b->getPieces(them, BISHOP);
    U64   rooks = b->getPieces(them, ROOK);
    U64  queens = b->getPieces(them, QUEEN);
    
    // get the king positions
    Square kingSq = bitscanForward(b->getPieces(us, KING));
    
    // get the potential pinners for rook/bishop attacks
    U64 rookAttacks   = lookUpRookAttack  (kingSq, opponentOcc) & (rooks   | queens);
    U64 bishopAttacks = lookUpBishopAttack(kingSq, opponentOcc) & (bishops | queens);
    
    // get all pinners (either rook or bishop attackers)
    U64 potentialPinners = (rookAttacks | bishopAttacks);
    
    while(potentialPinners){

        Square pinnerSquare = bitscanForward(potentialPinners);

        // get all the squares in between the king and the potential pinner
        U64 inBetween = inBetweenSquares[kingSq][pinnerSquare];

        // if there is exactly one of our pieces in the way, consider it pinned. Otherwise, continue
        U64 potentialPinned = ourOcc & inBetween;
        if (potentialPinned == 0 || lsbIsolation(potentialPinned) != potentialPinned){
            potentialPinners = lsbReset(potentialPinners);
            continue;
        }

        // extract the pinner pieces and the piece that pins
        Piece pinnedPiece = b->getPiece(bitscanForward(potentialPinned));
        Piece pinnerPiece = b->getPiece(pinnerSquare) - BISHOP;

        // normalise the values (black pieces will be made to white pieces)
        if(us == WHITE){
            pinnerPiece -= 6;
        }else{
            pinnedPiece -= 6;
        }

        // add to the result indexing using pinnedPiece for which there are 5 different pieces and the pinner
        result += pinnedEval[pinnedPiece * 3 + pinnerPiece];
        
        // reset the lsb
        potentialPinners = lsbReset(potentialPinners);
    }
    
    return result;
}

template<Color us>
EvalScore Evaluator::kingSafety(Board* b){
    EvalScore result;
    
    constexpr Color them = 1 - us;
    
    U64 themAttacks =
        attacks[them][PAWN] |
        attacks[them][KNIGHT] |
        attacks[them][BISHOP] |
        attacks[them][ROOK] |
        attacks[them][QUEEN] |
        attacks[them][KING];
    
    U64 defendedByPtr =
            attacks[us][PAWN] |
            attacks[us][KNIGHT] |
            attacks[us][BISHOP] |
            attacks[us][ROOK];
    
    U64 occ = *b->getOccupied();
    
    U64 weakSquares = themAttacks &~defendedByPtr;
    
    U64 knightCheckSquares = KNIGHT_ATTACKS[kingSquare[us]] & ~attacks[us][PAWN];
    U64 bishopCheckSquares = lookUpBishopAttack(kingSquare[us], occ) & ~attacks[us][PAWN];
    U64 rookCheckSquares   = lookUpRookAttack  (kingSquare[us], occ) & ~attacks[us][PAWN];
    U64 queenCheckSquares  = bishopCheckSquares | rookCheckSquares;
    
//    std::cout << *b << std::endl;
//    std::cout << bitCount(attacks[them][ROOK] & rookCheckSquares) << std::endl;
    
    int danger =
         + kingDangerFactors[ATTACK_COUNT]      * pieceAttackScore[them]
         + kingDangerFactors[QUEEN_EXISTENCE]   * (b->getPieces(them, QUEEN) != 0)
         + kingDangerFactors[WEAK_KING_RING]    * bitCount(kingSafetySquares[us] & weakSquares)
         + kingDangerFactors[SAFE_QUEEN_CHECK]  * bitCount(attacks[them][QUEEN ] & queenCheckSquares)
         + kingDangerFactors[SAFE_ROOK_CHECK ]  * bitCount(attacks[them][ROOK  ] & rookCheckSquares)
         + kingDangerFactors[SAFE_BISHOP_CHECK] * bitCount(attacks[them][BISHOP] & bishopCheckSquares)
         + kingDangerFactors[SAFE_KNIGHT_CHECK] * bitCount(attacks[them][KNIGHT] & knightCheckSquares);
   
    result = M(danger / 256, 0);
    
    return -result;
}


/**
 * evaluates the board.
 * @param b
 * @return
 */
bb::Score Evaluator::evaluate(Board* b) {

    Score res = 0;

    U64 whiteTeam = b->getTeamOccupied()[WHITE];
    U64 blackTeam = b->getTeamOccupied()[BLACK];
    U64 occupied  = *b->getOccupied();



    Square square;
    U64    attacks;
    U64    k;
    
    
    kingSquare[WHITE] = bitscanForward(b->getPieces()[WHITE_KING]);
    kingSquare[BLACK] = bitscanForward(b->getPieces()[BLACK_KING]);
    pieceAttackScore[WHITE] = 0;
    pieceAttackScore[BLACK] = 0;
    Evaluator::attacks[WHITE][PAWN] = 0;
    Evaluator::attacks[WHITE][KNIGHT] = 0;
    Evaluator::attacks[WHITE][BISHOP] = 0;
    Evaluator::attacks[WHITE][ROOK] = 0;
    Evaluator::attacks[WHITE][QUEEN] = 0;
    Evaluator::attacks[WHITE][KING] = 0;
    Evaluator::attacks[BLACK][PAWN] = 0;
    Evaluator::attacks[BLACK][KNIGHT] = 0;
    Evaluator::attacks[BLACK][BISHOP] = 0;
    Evaluator::attacks[BLACK][ROOK] = 0;
    Evaluator::attacks[BLACK][QUEEN] = 0;
    Evaluator::attacks[BLACK][KING] = 0;
    
    phase = (24.0f + phaseValues[5] - phaseValues[0] * bitCount(b->getPieces()[WHITE_PAWN] | b->getPieces()[BLACK_PAWN])
             - phaseValues[1] * bitCount(b->getPieces()[WHITE_KNIGHT] | b->getPieces()[BLACK_KNIGHT])
             - phaseValues[2] * bitCount(b->getPieces()[WHITE_BISHOP] | b->getPieces()[BLACK_BISHOP])
             - phaseValues[3] * bitCount(b->getPieces()[WHITE_ROOK] | b->getPieces()[BLACK_ROOK])
             - phaseValues[4] * bitCount(b->getPieces()[WHITE_QUEEN] | b->getPieces()[BLACK_QUEEN]))
            / 24.0f;

    if (phase > 1)
        phase = 1;
    if (phase < 0)
        phase = 0;

    kingSafetySquares[WHITE] = KING_ATTACKS[kingSquare[WHITE]];
    kingSafetySquares[BLACK] = KING_ATTACKS[kingSquare[BLACK]];
    if(fileIndex(kingSquare[WHITE]) == 0) kingSafetySquares[WHITE] |= shiftEast(kingSafetySquares[WHITE]);
    if(fileIndex(kingSquare[WHITE]) == 7) kingSafetySquares[WHITE] |= shiftWest(kingSafetySquares[WHITE]);
    if(fileIndex(kingSquare[BLACK]) == 0) kingSafetySquares[BLACK] |= shiftEast(kingSafetySquares[BLACK]);
    if(fileIndex(kingSquare[BLACK]) == 7) kingSafetySquares[BLACK] |= shiftWest(kingSafetySquares[BLACK]);
    
    /**********************************************************************************
     *                                  P A W N S                                     *
     **********************************************************************************/

    U64 whitePawns = b->getPieces()[WHITE_PAWN];
    U64 blackPawns = b->getPieces()[BLACK_PAWN];

    // all passed pawns for white/black
    U64 whitePassers = wPassedPawns(whitePawns, blackPawns);
    U64 blackPassers = bPassedPawns(blackPawns, whitePawns);

    // doubled pawns without the pawn least developed
    U64 whiteDoubledWithoutFirst = wFrontSpans(whitePawns) & whitePawns;
    U64 blackDoubledWithoutFirst = bFrontSpans(blackPawns) & blackPawns;

    // all doubled pawns
    U64 whiteDoubledPawns = whiteDoubledWithoutFirst | (wRearSpans(whiteDoubledWithoutFirst) & whitePawns);
    U64 blackDoubledPawns = blackDoubledWithoutFirst | (bRearSpans(blackDoubledWithoutFirst) & blackPawns);

    // all isolated pawns
    U64 whiteIsolatedPawns = whitePawns & ~(fillFile(shiftWest(whitePawns) | shiftEast(whitePawns)));
    U64 blackIsolatedPawns = blackPawns & ~(fillFile(shiftWest(blackPawns) | shiftEast(blackPawns)));

    U64 whiteBlockedPawns = shiftNorth(whitePawns) & (whiteTeam | blackTeam);
    U64 blackBlockedPawns = shiftSouth(blackPawns) & (whiteTeam | blackTeam);

    U64 openFilesWhite = ~fillFile(whitePawns);
    U64 openFilesBlack = ~fillFile(blackPawns);
    U64 openFiles      = openFilesBlack & openFilesWhite;

    k = whitePawns;

    EvalScore evalScore    = M(0, 0);
    EvalScore featureScore = M(0, 0);
    EvalScore mobScore     = M(0, 0);
    EvalScore materialScore= M(0, 0);

    while (k) {
        square = bitscanForward(k);
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][WHITE_PAWN][square];
     
        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_PAWN];
    while (k) {
        square = bitscanForward(k);
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][BLACK_PAWN][square];
        k = lsbReset(k);
    }

    k = whitePassers;
    while (k) {
        square = bitscanForward(k);
        featureScore += passer_rank_n[getBit(whiteBlockedPawns, square) * 8 + rankIndex(square)];
        k = lsbReset(k);
    }
    k = blackPassers;
    while (k) {
        square = bitscanForward(k);
        featureScore -= passer_rank_n[getBit(blackBlockedPawns, square) * 8 + 7 - rankIndex(square)];
        k = lsbReset(k);
    }

    U64 whitePawnEastCover = shiftNorthEast(whitePawns) & whitePawns;
    U64 whitePawnWestCover = shiftNorthWest(whitePawns) & whitePawns;
    U64 blackPawnEastCover = shiftSouthEast(blackPawns) & blackPawns;
    U64 blackPawnWestCover = shiftSouthWest(blackPawns) & blackPawns;

    U64 whitePawnCover = shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns);
    U64 blackPawnCover = shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns);

    U64 mobilitySquaresWhite = ~whiteTeam & ~(blackPawnCover);
    U64 mobilitySquaresBlack = ~blackTeam & ~(whitePawnCover);

    // clang-format off
    featureScore += PAWN_DOUBLED_AND_ISOLATED * (
            + bitCount(whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & blackDoubledPawns));
    featureScore += PAWN_DOUBLED * (
            + bitCount(~whiteIsolatedPawns & whiteDoubledPawns)
            - bitCount(~blackIsolatedPawns & blackDoubledPawns));
    featureScore += PAWN_ISOLATED * (
            + bitCount(whiteIsolatedPawns & ~whiteDoubledPawns)
            - bitCount(blackIsolatedPawns & ~blackDoubledPawns));
    featureScore += PAWN_PASSED * (
            + bitCount(whitePassers)
            - bitCount(blackPassers));
    featureScore += PAWN_STRUCTURE * (
            + bitCount(whitePawnEastCover)
            + bitCount(whitePawnWestCover)
            - bitCount(blackPawnEastCover)
            - bitCount(blackPawnWestCover));
    featureScore += PAWN_OPEN * (
            + bitCount(whitePawns & ~fillSouth(blackPawns))
            - bitCount(blackPawns & ~fillNorth(whitePawns)));
    featureScore += PAWN_BACKWARD * (
            + bitCount(fillSouth(~wAttackFrontSpans(whitePawns) & blackPawnCover) & whitePawns)
            - bitCount(fillNorth(~bAttackFrontSpans(blackPawns) & whitePawnCover) & blackPawns));
    featureScore += PAWN_BLOCKED * (
            + bitCount(whiteBlockedPawns)
            - bitCount(blackBlockedPawns));
    featureScore += MINOR_BEHIND_PAWN * (
            + bitCount(shiftNorth(b->getPieces()[WHITE_KNIGHT]|b->getPieces()[WHITE_BISHOP])&(b->getPieces()[WHITE_PAWN]|b->getPieces()[BLACK_PAWN]))
            - bitCount(shiftSouth(b->getPieces()[BLACK_KNIGHT]|b->getPieces()[BLACK_BISHOP])&(b->getPieces()[WHITE_PAWN]|b->getPieces()[BLACK_PAWN])));
    
    
    
    Evaluator::attacks[WHITE][PAWN] = whitePawnCover;
    Evaluator::attacks[BLACK][PAWN] = blackPawnCover;

    /**********************************************************************************
     *                                  K N I G H T S                                 *
     **********************************************************************************/

    k = b->getPieces()[WHITE_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
        Evaluator::attacks[WHITE][KNIGHT] |= attacks;
    
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][WHITE_KNIGHT][square];
        
        
        mobScore += mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresWhite)];

        featureScore += KNIGHT_OUTPOST * isOutpost(square, WHITE, blackPawns, whitePawnCover);
        featureScore += KNIGHT_DISTANCE_ENEMY_KING * manhattanDistance(square, kingSquare[BLACK]);
        
        pieceAttackScore[WHITE] += bitCount(kingSafetySquares[BLACK] & attacks) * kingSafetyAttackWeights[KNIGHT];

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_KNIGHT];
    while (k) {
        square  = bitscanForward(k);
        attacks = KNIGHT_ATTACKS[square];
        Evaluator::attacks[BLACK][KNIGHT] |= attacks;
    
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][BLACK_KNIGHT][square];
        
        mobScore -= mobilityKnight[bitCount(KNIGHT_ATTACKS[square] & mobilitySquaresBlack)];

        featureScore -= KNIGHT_OUTPOST * isOutpost(square, BLACK, whitePawns, blackPawnCover);
        featureScore -= KNIGHT_DISTANCE_ENEMY_KING * manhattanDistance(square, kingSquare[WHITE]);

        pieceAttackScore[BLACK] += bitCount(kingSafetySquares[WHITE] & attacks) * kingSafetyAttackWeights[KNIGHT];

        k = lsbReset(k);
    }
  
    /**********************************************************************************
     *                                  B I S H O P S                                 *
     **********************************************************************************/
    
    
    
    k = b->getPieces()[WHITE_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpBishopAttack(square, occupied & ~b->getPieces()[WHITE_QUEEN]);
        Evaluator::attacks[WHITE][BISHOP] |= attacks;
        
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][WHITE_BISHOP][square];
        
        
        mobScore += mobilityBishop[bitCount(attacks & mobilitySquaresWhite)];

        featureScore += bishop_pawn_same_color_table_e[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore += bishop_pawn_same_color_table_o[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore += BISHOP_PIECE_SAME_SQUARE_E 
                        * bitCount(blackTeam & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
        featureScore += BISHOP_FIANCHETTO
                        * (square == G2 && whitePawns & ONE << F2 && whitePawns & ONE << H2
                           && whitePawns & (ONE << G3 | ONE << G4));
        featureScore += BISHOP_FIANCHETTO
                        * (square == B2 && whitePawns & ONE << A2 && whitePawns & ONE << C2
                           && whitePawns & (ONE << B3 | ONE << B4));
    
        pieceAttackScore[WHITE] += bitCount(kingSafetySquares[BLACK] & attacks) * kingSafetyAttackWeights[BISHOP];

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_BISHOP];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpBishopAttack(square, occupied & ~b->getPieces()[BLACK_QUEEN]);
        Evaluator::attacks[BLACK][BISHOP] |= attacks;
    
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][BLACK_BISHOP][square];
    
        mobScore -= mobilityBishop[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= bishop_pawn_same_color_table_e[bitCount(whitePawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore -= bishop_pawn_same_color_table_o[bitCount(blackPawns & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES))];
        featureScore -= BISHOP_PIECE_SAME_SQUARE_E 
                        * bitCount(whiteTeam & (((ONE << square) & WHITE_SQUARES) ? WHITE_SQUARES : BLACK_SQUARES));
        featureScore -= BISHOP_FIANCHETTO
                        * (square == G7 && blackPawns & ONE << F7 && blackPawns & ONE << H7
                           && blackPawns & (ONE << G6 | ONE << G5));
        featureScore -= BISHOP_FIANCHETTO
                        * (square == B2 && blackPawns & ONE << A7 && blackPawns & ONE << C7
                           && blackPawns & (ONE << B6 | ONE << B5));

        pieceAttackScore[BLACK] += bitCount(kingSafetySquares[WHITE] & attacks) * kingSafetyAttackWeights[BISHOP];

        k = lsbReset(k);
    }
    // clang-format off
    featureScore += BISHOP_DOUBLED * (
            + (bitCount(b->getPieces()[WHITE_BISHOP]) == 2)
            - (bitCount(b->getPieces()[BLACK_BISHOP]) == 2));
    // clang-format on
    
 
    
    /**********************************************************************************
     *                                  R O O K S                                     *
     **********************************************************************************/

    k = b->getPieces()[WHITE_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied & ~b->getPieces()[WHITE_ROOK] & ~b->getPieces()[WHITE_QUEEN]);
        Evaluator::attacks[WHITE][ROOK] |= attacks;
        
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][WHITE_ROOK][square];
        mobScore += mobilityRook[bitCount(attacks & mobilitySquaresWhite)];

        pieceAttackScore[WHITE] += bitCount(kingSafetySquares[BLACK] & attacks) * kingSafetyAttackWeights[ROOK];

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_ROOK];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack(square, occupied & ~b->getPieces()[BLACK_ROOK] & ~b->getPieces()[BLACK_QUEEN]);
        Evaluator::attacks[BLACK][ROOK] |= attacks;
    
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][BLACK_ROOK][square];
        mobScore -= mobilityRook[bitCount(attacks & mobilitySquaresBlack)];

        pieceAttackScore[BLACK] += bitCount(kingSafetySquares[WHITE] & attacks) * kingSafetyAttackWeights[ROOK];

        k = lsbReset(k);
    }

    // clang-format off
    featureScore += ROOK_KING_LINE * (
            + bitCount(lookUpRookAttack(kingSquare[BLACK], occupied) & b->getPieces(WHITE, ROOK))
            - bitCount(lookUpRookAttack(kingSquare[WHITE], occupied) & b->getPieces(BLACK, ROOK)));
    featureScore += ROOK_OPEN_FILE * (
            + bitCount(openFiles & b->getPieces(WHITE, ROOK))
            - bitCount(openFiles & b->getPieces(BLACK, ROOK)));
    featureScore += ROOK_HALF_OPEN_FILE * (
            + bitCount(openFilesBlack & ~openFiles & b->getPieces(WHITE, ROOK))
            - bitCount(openFilesWhite & ~openFiles & b->getPieces(BLACK, ROOK)));
    // clang-format on

    
    /**********************************************************************************
     *                                  Q U E E N S                                   *
     **********************************************************************************/

    k = b->getPieces()[WHITE_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack  (square,    occupied & ~b->getPieces()[WHITE_ROOK])
                | lookUpBishopAttack(square,    occupied & ~b->getPieces()[WHITE_BISHOP]);
        Evaluator::attacks[WHITE][QUEEN] |= attacks;
        
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][WHITE_QUEEN][square];
        mobScore += mobilityQueen[bitCount(attacks & mobilitySquaresWhite)];
        featureScore += QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, kingSquare[BLACK]);
    
        pieceAttackScore[WHITE] += bitCount(kingSafetySquares[BLACK] & attacks) * kingSafetyAttackWeights[QUEEN];

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_QUEEN];
    while (k) {
        square  = bitscanForward(k);
        attacks = lookUpRookAttack  (square,    occupied & ~b->getPieces()[BLACK_ROOK])
                | lookUpBishopAttack(square,    occupied & ~b->getPieces()[BLACK_BISHOP]);
        Evaluator::attacks[BLACK][QUEEN] |= attacks;
    
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][BLACK_QUEEN][square];
        mobScore -= mobilityQueen[bitCount(attacks & mobilitySquaresBlack)];
        featureScore -= QUEEN_DISTANCE_ENEMY_KING * manhattanDistance(square, kingSquare[WHITE]);

        pieceAttackScore[BLACK] += bitCount(kingSafetySquares[WHITE] & attacks) * kingSafetyAttackWeights[QUEEN];

        k = lsbReset(k);
    }
    
    /**********************************************************************************
     *                                  K I N G S                                     *
     **********************************************************************************/
    k = b->getPieces()[WHITE_KING];

    while (k) {
        square = bitscanForward(k);
        Evaluator::attacks[WHITE][KING] = KING_ATTACKS[square];
        pieceAttackScore[WHITE] += bitCount(kingSafetySquares[BLACK] & KING_ATTACKS[square]) * kingSafetyAttackWeights[KING];
    
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][WHITE_KING][square];

        featureScore += KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & whitePawns);
        featureScore += KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & blackTeam);

        k = lsbReset(k);
    }

    k = b->getPieces()[BLACK_KING];
    while (k) {
        square = bitscanForward(k);
        Evaluator::attacks[BLACK][KING] = KING_ATTACKS[square];
        pieceAttackScore[BLACK] += bitCount(kingSafetySquares[WHITE] & KING_ATTACKS[square]) * kingSafetyAttackWeights[KING];
    
        materialScore += piece_kk_square_tables[kingSquare[WHITE]][kingSquare[BLACK]][BLACK_KING][square];

        featureScore -= KING_PAWN_SHIELD * bitCount(KING_ATTACKS[square] & blackPawns);
        featureScore -= KING_CLOSE_OPPONENT * bitCount(KING_ATTACKS[square] & whiteTeam);

        k = lsbReset(k);
    }
    
    
    EvalScore hangingEvalScore = computeHangingPieces(b);
    EvalScore pinnedEvalScore  = computePinnedPieces(b, WHITE) - computePinnedPieces(b, BLACK);
    
    // clang-format off
    featureScore += CASTLING_RIGHTS*(
            + b->getCastlingChance(STATUS_INDEX_WHITE_QUEENSIDE_CASTLING)
            + b->getCastlingChance(STATUS_INDEX_WHITE_KINGSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_QUEENSIDE_CASTLING)
            - b->getCastlingChance(STATUS_INDEX_BLACK_KINGSIDE_CASTLING));
    // clang-format on
    featureScore += SIDE_TO_MOVE * (b->getActivePlayer() == WHITE ? 1 : -1);

    EvalScore totalScore = evalScore + pinnedEvalScore + hangingEvalScore + featureScore + mobScore + materialScore;
    
    totalScore += kingSafety<WHITE>(b) - kingSafety<BLACK>(b);
    
    res += (int) ((float) MgScore(totalScore) * (1 - phase));
    res += (int) ((float) EgScore(totalScore) * (phase));

    if (!hasMatingMaterial(b, res > 0 ? WHITE : BLACK))
        res = res / 10;
    return res;
}

void printEvaluation(Board* board) {

    using namespace std;

    Evaluator ev {};
    Score     score = ev.evaluate(board);
    float     phase = ev.getPhase();

    std::cout <<
        setw(15) << right << "evaluation: " << left << setw(8) << score <<
        setw(15) << right << "phase: "      << left << setprecision(3) << setw(8) << phase << std::endl;
}

float Evaluator::getPhase() { return phase; }



