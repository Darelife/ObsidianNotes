1. Points of each piece
2. Use bitmask in `uint64_t` to store everything, for quick queries
3. Try every possible move, and in that add a check, to verify legality
4. Make a `makeMove()` function, as well as an `undoMove()` function
5. Perft: DFS on the legal moves
6. Static Evaluation: Get a value from the engine, in terms of `Centipawns`.
7. CentiPawns: A pawn is assigned 100 centipawns.
   Pawn   = 100
   Knight = 320
   Bishop = 330
   Rook   = 500
   Queen  = 900
   +100 = White is approximately one pawn ahead
   -100 = Black is approximately one pawn ahead
   till now, we're just checking the material counts.
8. Piece square tables
```cpp
   int knightTable[64] = {
	    -50,-40,-30,-30,-30,-30,-40,-50,
	    -40,-20,  0,  0,  0,  0,-20,-40,
	    -30,  0, 10, 15, 15, 10,  0,-30,
	    -30,  5, 15, 20, 20, 15,  5,-30,
	    ...
	};	
```
9. Middle game vs End game
   In the middle game, for the king, being in the center isn't the best, but in the end game, the center could be useful. So, we could have 2 different tables, kingMiddleGameTable[64]; kingEndGameTable[64];. 
10. Mobility: How many useful moves does this piece have? Eg: bishop stranded behind is bad...we could have a bonus score attached to mobility for the pieces.
11. Pawn structure: Having 2 pawns in the same file isn't great, so we could have a penalty for it.
12. Passed pawn: A pawn that has no enemy pawn capable of stopping it directly. For this case, we could have (pawn on the 2nd rank = 10 points, 5th rank = 40 points, and on the 7th rank = 150 points)
13. King Safety: Having a king exposed is bad. So, evaluate the current position around the king (how exposed is the king?), and based on it, give a score.
14. Minimax: