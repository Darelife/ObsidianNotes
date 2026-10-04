Absolutely. The easiest way to understand a chess engine is to separate it into three big parts:

```text
1. Representation — "What does the board currently look like?"
2. Search         — "What happens if I play this move?"
3. Evaluation     — "How good is this resulting position?"
```

The **eval bar** is basically just the final output of parts 2 and 3.

I'll go through the whole pipeline from the ground up.

---

# 1. Board representation

Before your engine can think about chess, it needs some way to represent:

```text
8 × 8 board
whose turn it is
castling rights
en passant square
halfmove clock
etc.
```

The most obvious representation is an array:

```cpp
int board[8][8];
```

Maybe:

```cpp
EMPTY = 0

WHITE_PAWN   = 1
WHITE_KNIGHT = 2
WHITE_BISHOP = 3
WHITE_ROOK   = 4
WHITE_QUEEN  = 5
WHITE_KING   = 6

BLACK_PAWN   = -1
...
```

Then:

```text
board[0][0] = white rook
board[0][1] = white knight
...
```

This is very intuitive.

But serious engines generally use **bitboards**.

---

# 2. Bitboards

A chess board has exactly 64 squares.

A `uint64_t` also has exactly 64 bits.

So:

```text
bit 0  = a1
bit 1  = b1
bit 2  = c1
...
bit 63 = h8
```

Suppose white pawns are on:

```text
a2 b2 c2 d2 e2 f2 g2 h2
```

You can represent all of them using one 64-bit integer:

```cpp
uint64_t whitePawns;
```

where those eight corresponding bits are `1`.

For example:

```text
00000000
00000000
00000000
00000000
00000000
00000000
11111111
00000000
```

Then you might maintain:

```cpp
uint64_t whitePawns;
uint64_t whiteKnights;
uint64_t whiteBishops;
uint64_t whiteRooks;
uint64_t whiteQueens;
uint64_t whiteKing;

uint64_t blackPawns;
...
```

Why is this useful?

Because CPU bit operations are insanely cheap.

Suppose:

```cpp
uint64_t occupied = whitePieces | blackPieces;
```

That's one CPU operation that tells you every occupied square.

Likewise:

```cpp
uint64_t empty = ~occupied;
```

And:

```cpp
uint64_t attacks = knightAttacks[square];
```

can immediately give every square attacked by a knight.

This is why chess engines love bitboards.

---

# 3. Move generation

Now the engine asks:

> What moves are available from this position?

For example:

```text
e2e4
e2e3
g1f3
b1c3
...
```

This is **move generation**.

There are two concepts:

```text
pseudo-legal moves
legal moves
```

## Pseudo-legal move

A move that follows how the piece moves.

Example:

```text
bishop moves diagonally
rook moves straight
pawn moves forward
```

But it might leave your king in check.

For example:

```text
Black rook
   |
   |
White bishop
   |
White king
```

The bishop can geometrically move away.

But doing so exposes the king to the rook.

So the move is:

```text
pseudo-legal: yes
legal: no
```

A common engine strategy is:

```cpp
moves = generatePseudoLegalMoves();

for (Move m : moves) {
    makeMove(m);

    if (!kingInCheck())
        legal.push_back(m);

    undoMove(m);
}
```

Later you can optimize this.

---

# 4. `makeMove()`

Your engine will constantly simulate moves.

Suppose the current position is:

```text
White to move
```

and you're considering:

```text
e2 → e4
```

You need:

```cpp
makeMove(move);
```

to modify the internal board.

It may need to update:

```text
piece location
side to move
captured piece
castling rights
en passant square
halfmove clock
Zobrist hash
```

For a normal pawn move:

```text
remove pawn from e2
place pawn on e4
sideToMove = BLACK
enPassantSquare = e3
```

For castling:

```text
king e1 → g1
rook h1 → f1
```

For promotion:

```text
pawn e7 → e8
pawn becomes queen
```

There are quite a few edge cases.

---

# 5. `undoMove()`

Search involves doing this:

```text
try e4
    try e5
        try Nf3
            ...
```

Then going back.

So:

```cpp
makeMove(e4);
search();
undoMove(e4);
```

`undoMove()` must restore the exact original state.

A common technique is storing an `UndoInfo`:

```cpp
struct UndoInfo {
    int capturedPiece;
    int oldCastlingRights;
    int oldEnPassantSquare;
    int oldHalfmoveClock;
    uint64_t oldHash;
};
```

Then:

```cpp
UndoInfo info = makeMove(move);

search(...);

undoMove(move, info);
```

Making and undoing moves efficiently is incredibly important because your engine may do this **millions of times per second**.

---

# 6. Perft

Before writing any intelligent chess logic, you need to prove your move generation is correct.

That's what **perft** does.

Perft means roughly:

> Count all legal positions reachable after N moves.

Example:

```cpp
long long perft(Board& b, int depth) {
    if (depth == 0)
        return 1;

    long long nodes = 0;

    for (Move m : generateLegalMoves(b)) {
        b.makeMove(m);
        nodes += perft(b, depth - 1);
        b.undoMove(m);
    }

    return nodes;
}
```

The standard starting position has known values:

```text
depth 1: 20
depth 2: 400
depth 3: 8902
depth 4: 197281
depth 5: 4865609
```

So if your engine gives:

```text
depth 4 = 197277
```

you know your move generation is wrong.

Maybe:

```text
castling bug
en passant bug
check detection bug
promotion bug
pinned piece bug
```

Perft is basically the unit test of chess engines.

Do this before search.

---

# 7. Static evaluation

Suppose the engine reaches a position and wants to know:

> Is White better or Black better?

That's what the **evaluation function** does.

```cpp
int evaluate(Position& p);
```

It might return:

```text
+230
```

meaning White is better.

Or:

```text
-170
```

meaning Black is better.

Usually scores are measured in **centipawns**.

---

# 8. Centipawns

A pawn is roughly assigned:

```text
100 centipawns
```

So:

```text
+100 = White is approximately one pawn ahead
-100 = Black is approximately one pawn ahead
```

Typical material values:

```text
Pawn   = 100
Knight = 320
Bishop = 330
Rook   = 500
Queen  = 900
```

The king doesn't get a normal value because losing the king isn't allowed.

So:

```cpp
int evaluateMaterial() {
    return
        100 * (whitePawns - blackPawns)
      + 320 * (whiteKnights - blackKnights)
      + 330 * (whiteBishops - blackBishops)
      + 500 * (whiteRooks - blackRooks)
      + 900 * (whiteQueens - blackQueens);
}
```

Imagine:

```text
White:
Queen
2 rooks
2 bishops
2 knights
8 pawns

Black:
Queen
2 rooks
2 bishops
2 knights
7 pawns
```

White is one pawn ahead.

Evaluation:

```text
+100
```

---

# 9. Why material alone isn't enough

Consider:

```text
White:
King exposed
queen trapped
pieces undeveloped

Black:
safe king
active pieces
huge attack
```

Material might be exactly equal:

```text
0
```

But the position clearly isn't equal.

So engines add positional factors.

---

# 10. Piece-square tables

A knight is not equally useful on every square.

A knight on:

```text
a1
```

is usually worse than a knight on:

```text
d5
```

So you can assign bonuses.

Example knight table:

```cpp
int knightTable[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    ...
};
```

Then:

```cpp
score += knightTable[knightSquare];
```

You are essentially saying:

```text
knight on d5:
+20

knight on a1:
-50
```

This teaches the engine:

```text
centralize knights
avoid bad edge squares
```

without explicitly writing chess rules.

You can have tables for:

```text
pawn
knight
bishop
rook
queen
king
```

---

# 11. Middlegame vs endgame

Piece values change depending on the phase of the game.

For example:

King:

```text
Middlegame:
being in the center = terrible

Endgame:
being in the center = useful
```

So engines may have:

```cpp
kingMiddleGameTable[64];
kingEndGameTable[64];
```

Then interpolate between them.

Example:

```text
Lots of queens/rooks remaining
→ 90% middlegame evaluation

Very little material
→ 90% endgame evaluation
```

This is called **tapered evaluation**.

---

# 12. Mobility

Mobility asks:

> How many useful moves does this piece have?

A bishop trapped behind pawns:

```text
mobility = 1
```

An active bishop controlling the board:

```text
mobility = 10
```

So:

```cpp
score += bishopMoves * MOBILITY_BONUS;
```

This encourages active pieces.

---

# 13. Pawn structure

Pawns have huge strategic importance.

You can evaluate things like:

### Doubled pawns

```text
White pawns:
c2
c3
```

Two pawns on the same file.

Often a weakness.

```cpp
score -= doubledPawnPenalty;
```

### Isolated pawn

A pawn with no friendly pawns on adjacent files.

Example:

```text
d4 pawn

no pawn on c-file
no pawn on e-file
```

Potential weakness.

### Passed pawn

A pawn that has no enemy pawn capable of stopping it directly.

Example:

```text
White pawn on e6
no Black pawns on d/e/f files ahead
```

Very valuable.

You might give:

```text
pawn on 2nd rank: +10
pawn on 5th rank: +40
pawn on 7th rank: +150
```

because it's close to promotion.

---

# 14. King safety

An exposed king is dangerous.

You might evaluate:

```text
pawn shield
enemy pieces nearby
open files near king
enemy queen proximity
number of attacked king squares
```

For example:

```text
castled king:

♙ ♙ ♙
  ♔

good
```

versus:

```text
no pawns
open rook file
king exposed

bad
```

You might subtract:

```cpp
score -= kingDanger * penalty;
```

---

# 15. Static evaluation is NOT the eval bar yet

This distinction is extremely important.

Suppose your static evaluator says:

```text
Position = +300
```

Maybe White is up a bishop.

But perhaps Black can immediately play:

```text
Qxd1
```

and win White's queen.

If you only evaluate the current position, you miss that.

So chess engines **search into the future**.

---

# 16. Minimax

Imagine White has three legal moves:

```text
A
B
C
```

After each move, Black responds.

White wants:

```text
maximum evaluation
```

Black wants:

```text
minimum evaluation
```

Hence:

```text
MINIMAX
```

Conceptually:

```text
                 White
             /     |     \
            A      B      C
           / \    / \    / \
        Black   Black   Black
```

Suppose leaf evaluations are:

```text
A:
+5
-2
+3

B:
+1
+2

C:
+4
+7
```

Black chooses the worst outcome for White.

So:

```text
A → min(+5,-2,+3) = -2
B → min(+1,+2)    = +1
C → min(+4,+7)    = +4
```

White chooses:

```text
max(-2,+1,+4) = +4
```

Therefore White chooses C.

---

# 17. Why assume the opponent plays the best move?

Because otherwise the engine would make nonsense plans like:

```text
"If Black blunders their queen, this move is amazing!"
```

You assume:

```text
I play optimally.
Opponent plays optimally.
```

Therefore you're looking for the best move against the strongest possible response.

---

# 18. Negamax

Minimax code often has separate functions:

```cpp
maxPlayer()
minPlayer()
```

But chess is a zero-sum game.

If a position is:

```text
+300 for White
```

then from Black's perspective it's:

```text
-300
```

So:

```text
scoreForMe = -scoreForOpponent
```

This lets us simplify minimax into **negamax**.

Core idea:

```cpp
score = -negamax(child);
```

Example:

```cpp
int negamax(Position& p, int depth) {
    if (depth == 0)
        return evaluate(p);

    int best = -INF;

    for (Move move : legalMoves(p)) {
        makeMove(move);

        int score = -negamax(p, depth - 1);

        undoMove(move);

        best = max(best, score);
    }

    return best;
}
```

One subtle thing:

A typical negamax evaluator returns evaluation **from the perspective of the player whose turn it is**.

Then negation works naturally.

---

# 19. Search depth

Suppose:

```text
depth = 1
```

The engine sees:

```text
my move
```

Depth 2:

```text
my move
opponent move
```

Depth 3:

```text
my move
opponent move
my move
```

And so on.

The problem is chess has roughly:

```text
30–40 legal moves per position
```

If branching factor ≈ 35:

```text
depth 1:       35
depth 2:    1,225
depth 3:   42,875
depth 4: 1,500,625
depth 5: 52,000,000
depth 6: ~1.8 billion
```

You cannot naïvely search deeply.

This is why alpha-beta pruning matters enormously.

---

# 20. Alpha-beta pruning

Suppose White already found a move giving:

```text
+5
```

Now you examine another move.

Black has a response giving White:

```text
+2
```

Do you need to explore Black's other responses?

No.

Because Black will happily choose:

```text
+2
```

instead of allowing White to get +5.

Therefore this entire branch cannot beat White's existing +5 move.

You stop searching it.

That is **pruning**.

---

# 21. Alpha and beta

You maintain two bounds:

```text
alpha = best score current player is guaranteed
beta  = upper bound opponent will allow
```

In negamax:

```cpp
int negamax(Board& board, int depth, int alpha, int beta) {
    if (depth == 0)
        return evaluate(board);

    for (Move move : moves) {
        makeMove(move);

        int score = -negamax(
            board,
            depth - 1,
            -beta,
            -alpha
        );

        undoMove(move);

        if (score >= beta)
            return beta;

        alpha = max(alpha, score);
    }

    return alpha;
}
```

The magical part is:

```cpp
-alpha
-beta
```

because perspective flips every turn.

---

# 22. Alpha-beta does NOT change the answer

This is important.

Alpha-beta and normal minimax produce the same move.

Alpha-beta simply avoids investigating branches that mathematically cannot matter.

So:

```text
minimax:
10 million positions

alpha-beta:
maybe 500,000 positions
```

Same result.

Much faster.

---

# 23. Move ordering

Alpha-beta becomes dramatically stronger if you search promising moves first.

Suppose the best move is searched first.

You establish a strong alpha quickly.

Then lots of later branches can be pruned.

So engines order moves.

Typical priority:

```text
1. Previous best move
2. Winning captures
3. Promotions
4. Killer moves
5. History heuristic
6. Quiet moves
7. Bad captures
```

This seems like a performance optimization, but it indirectly makes your engine much stronger because:

```text
better ordering
→ more pruning
→ deeper search in same time
→ better moves
```

---

# 24. MVV-LVA

A simple capture ordering method is:

```text
Most Valuable Victim
Least Valuable Attacker
```

Example:

```text
pawn captures queen
```

very promising.

```text
queen captures pawn
```

potentially dangerous.

So captures can be ranked by something like:

```cpp
score = victimValue * 10 - attackerValue;
```

---

# 25. Iterative deepening

Suppose your engine has:

```text
2 seconds
```

You don't just directly run:

```text
search depth 12
```

because you might run out of time halfway and have no answer.

Instead:

```text
depth 1 → complete
depth 2 → complete
depth 3 → complete
depth 4 → complete
...
```

Until time expires.

This is **iterative deepening**.

Pseudo-code:

```cpp
for (int depth = 1; ; depth++) {
    if (timeUp())
        break;

    bestMove = search(depth);
}
```

If time expires during depth 9:

```text
use result from completed depth 8
```

---

# 26. Why iterative deepening can actually make things faster

At first this looks wasteful.

You've already searched depths:

```text
1
2
3
4
```

Why search them again?

Because the shallower search tells you which moves are promising.

Suppose depth 6 discovered:

```text
Nf3 looks strongest
```

At depth 7, search:

```text
Nf3 first
```

Then alpha-beta pruning becomes far more efficient.

So iterative deepening often helps overall speed.

---

# 27. Horizon effect

Now there's an important flaw with fixed depth.

Imagine your search ends here:

```text
White queen
Black rook attacking queen
```

Maybe evaluation says:

```text
White +8
```

because White has a queen.

But on the very next move, outside the search horizon:

```text
Black captures the queen
```

The engine didn't see it.

This is the **horizon effect**.

The evaluation cutoff happened in the middle of tactical action.

---

# 28. Quiescence search

To solve this, when normal search reaches:

```cpp
depth == 0
```

you don't immediately call static evaluation.

Instead, continue searching tactical moves such as:

```text
captures
promotions
sometimes checks
```

until the position becomes "quiet."

Hence:

```text
quiescence search
```

Pseudo-code:

```cpp
int quiescence(Position& p, int alpha, int beta) {
    int standPat = evaluate(p);

    if (standPat >= beta)
        return beta;

    alpha = max(alpha, standPat);

    for (Move capture : generateCaptures(p)) {
        makeMove(capture);

        int score = -quiescence(p, -beta, -alpha);

        undoMove(capture);

        if (score >= beta)
            return beta;

        alpha = max(alpha, score);
    }

    return alpha;
}
```

So instead of:

```text
search stops
queen hanging
evaluate +900
```

you see:

```text
queen captured
actual position evaluated
```

Huge improvement.

---

# 29. Transposition

Chess positions can be reached in different orders.

Example:

```text
Nf3 Nf6 g3 g6
```

and:

```text
g3 g6 Nf3 Nf6
```

can reach the same position.

These different move sequences are called **transpositions**.

Without caching:

```text
search position A
later reach A again
search everything again
```

Wasteful.

---

# 30. Transposition table

You store previous search results:

```cpp
unordered_map<Hash, Entry> table;
```

Entry might contain:

```cpp
struct TTEntry {
    uint64_t key;
    int depth;
    int score;
    Move bestMove;
};
```

Then:

```cpp
if (table contains currentPosition &&
    storedDepth >= requestedDepth) {

    return storedScore;
}
```

In practice you won't use an ordinary `unordered_map` for a serious engine; you'd use a fixed-size hash table for performance.

---

# 31. Zobrist hashing

How do we quickly generate a hash for a chess position?

You could hash every square from scratch every move.

But that's expensive.

Instead engines commonly use **Zobrist hashing**.

Generate random 64-bit numbers:

```text
random[piece][square]
```

For example:

```text
white pawn on e4 → random[WP][e4]
black queen on d8 → random[BQ][d8]
```

Position hash:

```text
hash =
random[WP][e4]
XOR random[BQ][d8]
XOR ...
```

Why XOR?

Because you can update incrementally.

Suppose pawn moves:

```text
e2 → e4
```

Old hash:

```cpp
hash ^= zobrist[WHITE_PAWN][e2];
hash ^= zobrist[WHITE_PAWN][e4];
```

The first XOR removes it.

The second adds the new location.

Also hash:

```text
side to move
castling rights
en passant
```

Now your position can be identified by a single:

```cpp
uint64_t
```

Extremely fast.

---

# 32. Mate scoring

Static evaluation values might look like:

```text
+500
+130
-760
```

But checkmate must be bigger than any normal position.

So:

```cpp
constexpr int MATE_SCORE = 30000;
```

If the current player is checkmated:

```cpp
return -MATE_SCORE + ply;
```

Why add `ply`?

Consider:

```text
mate in 2
mate in 5
```

Both are wins.

But you want the quicker mate.

Example:

```text
mate in 2 → 29996
mate in 5 → 29990
```

so the engine prefers mate in 2.

Similarly when losing:

```text
delay mate as long as possible
```

---

# 33. Ply vs move

Chess terminology:

```text
1 move = White + Black
1 ply  = one player's move
```

So:

```text
White e4 = ply 1
Black e5 = ply 2
White Nf3 = ply 3
```

Search depth is usually expressed in plies.

So:

```text
depth 10
```

roughly means:

```text
5 full moves
```

though extensions and quiescence make this less exact.

---

# 34. Principal variation

Suppose your engine says:

```text
+1.42
```

It should also have a line explaining why:

```text
1. Nf3 Nf6
2. g3 g6
3. Bg2 Bg7
4. O-O O-O
```

The engine's currently believed best line is the:

```text
Principal Variation
```

or:

```text
PV
```

Chess GUIs often show:

```text
Depth 18
+0.72
Nf3 Nf6 g3 g6 Bg2 ...
```

That line comes from the search tree.

---

# 35. The eval bar

Now finally, the bar.

Suppose search returns:

```text
+200 centipawns
```

Usually UI displays:

```text
+2.00
```

But that does NOT necessarily mean:

```text
White is literally two pawns ahead.
```

It means roughly:

```text
engine considers White's advantage comparable to about two pawns
```

It could be:

```text
material
king safety
space
initiative
piece activity
passed pawn
```

combined.

---

# 36. Why the bar shouldn't be linear

Suppose:

```text
0.0
+1
+2
+5
+10
```

If you map directly:

```text
50%
55%
60%
75%
100%
```

that's not very meaningful.

The practical difference between:

```text
+0 and +1
```

matters a lot.

But:

```text
+8 vs +12
```

both basically mean White is overwhelmingly winning.

So use a curve that saturates.

One simple function:

```cpp
double whiteProbability(int cp) {
    return 1.0 / (1.0 + exp(-cp / 300.0));
}
```

Example shape:

```text
cp       White bar

-1000      ~3%
-500      ~16%
-200      ~34%
0          50%
+200       ~66%
+500       ~84%
+1000      ~97%
```

Exact values depend on your constant.

This is called a **logistic/sigmoid function**.

---

# 37. Important caveat: eval isn't exactly win probability

A value like:

```text
+2.0
```

doesn't inherently mean:

```text
75% chance White wins
```

Centipawn evaluation and win probability are different concepts.

If you want a real win-probability bar, you need to calibrate:

```text
engine evaluation
→ actual historical outcomes
```

For example:

```text
At +200 cp in positions of this type,
White wins X%,
draws Y%,
loses Z%.
```

Modern engines have more sophisticated mappings.

For your own engine, though, a sigmoid is perfectly reasonable for visual presentation.

---

# 38. Mate display

If the engine detects:

```text
mate in 3
```

don't show:

```text
+300.00
```

Show:

```text
M3
```

or:

```text
#3
```

For example:

```cpp
if (score >= MATE_THRESHOLD) {
    displayMate(...);
} else {
    display(score / 100.0);
}
```

The bar can become:

```text
100% White
```

when mate is forced.

---

# 39. Search nodes

When engines show:

```text
Nodes: 1,830,229
```

that means they evaluated approximately that many nodes in the search tree.

A node roughly corresponds to:

```text
one chess position encountered
```

You might track:

```cpp
uint64_t nodes = 0;

int negamax(...) {
    nodes++;
    ...
}
```

---

# 40. Nodes per second

You can then compute:

```text
NPS = nodes / time
```

Example:

```text
2,000,000 nodes
0.5 seconds

NPS = 4,000,000
```

This is useful for benchmarking optimization.

If you switch from a 2D board to optimized bitboards and get:

```text
before: 1M NPS
after: 8M NPS
```

that's a huge win.

---

# 41. Checkmate and stalemate

When generating moves:

```cpp
moves = legalMoves();

if (moves.empty()) {
```

there are two possibilities.

If king is in check:

```text
checkmate
```

Return:

```cpp
-MATE_SCORE + ply
```

If king is not in check:

```text
stalemate
```

Return:

```cpp
0
```

because it's a draw.

---

# 42. Draw detection

Eventually you'll need:

```text
threefold repetition
50-move rule
insufficient material
stalemate
```

For repetition, Zobrist hashes are extremely useful.

Maintain:

```cpp
vector<uint64_t> positionHistory;
```

and check whether the current position occurred before.

---

# 43. Killer move heuristic

This is an optimization for move ordering.

Suppose at some depth a quiet move:

```text
Nd5
```

causes a beta cutoff.

That means it was surprisingly strong.

There's a decent chance that same move might be strong in sibling positions.

So store:

```cpp
killerMoves[depth][0]
killerMoves[depth][1]
```

Then search those moves early next time.

---

# 44. History heuristic

Another move ordering technique.

Maintain something like:

```cpp
history[from][to];
```

Whenever a quiet move causes a cutoff:

```cpp
history[from][to] += depth * depth;
```

Over time:

```text
moves that historically cause cutoffs
```

get searched earlier.

This is basically the engine learning:

> This quiet move tends to be useful in search.

Not machine learning; just statistics accumulated during the current search.

---

# 45. Null move pruning

Much later, you'll encounter this.

Idea:

> What if I voluntarily give the opponent an extra turn?

Normally that would be terrible.

So if your position is still so strong that even after skipping your turn you're above beta:

```text
this position is clearly strong enough
```

and you can prune.

Roughly:

```cpp
makeNullMove();

score = -search(depth - R);

undoNullMove();

if (score >= beta)
    return beta;
```

Very powerful.

But there are issues with:

```text
zugzwang
```

especially in endgames.

So don't implement it early.

---

# 46. Late Move Reductions

Another advanced optimization.

Suppose you have 35 moves.

You already searched:

```text
top 10 promising moves
```

and none worked.

Move #32 is some random quiet rook move.

Instead of searching it at full depth:

```text
depth 10
```

initially search:

```text
depth 7
```

If it unexpectedly looks good, then re-search fully.

This is **Late Move Reduction**, or LMR.

Modern engines depend heavily on techniques like this.

---

# 47. Search extensions

Sometimes a position is tactically important enough to search deeper.

Example:

```text
king in check
```

Instead of:

```text
depth 0 → evaluate
```

you might extend:

```text
depth + 1
```

so you don't stop in a bizarre tactical position.

Historically there are:

```text
check extensions
recapture extensions
passed pawn extensions
```

Modern engine extension logic can get complicated.

---

# 48. NNUE

This is much later.

Traditional evaluation says:

```cpp
score =
    material
  + pieceSquare
  + kingSafety
  + pawnStructure
  + mobility
  + ...
```

A modern approach is:

```text
chess position
        ↓
small neural network
        ↓
evaluation
```

One famous system is:

```text
NNUE
```

Originally:

```text
Efficiently Updatable Neural Network
```

The clever part is that moving one chess piece changes only a small amount of neural-network input, so you don't recompute the entire network.

Stockfish uses NNUE-style evaluation.

You absolutely should **not start with NNUE**.

A handcrafted evaluator teaches you much more about how chess engines work.

---

# 49. Search and evaluation work together

This is one of the most important ideas.

Imagine engine A:

```text
Amazing evaluation
searches depth 4
```

Engine B:

```text
simple evaluation
searches depth 10
```

Engine B may be significantly stronger because tactics dominate chess.

For example:

```text
Engine A:
"This knight is beautifully centralized! +0.7"

Engine B:
"Cool, but it gets checkmated in 5."
```

Search often beats subtle evaluation.

That's why engine design spends enormous effort on:

```text
alpha-beta
move ordering
pruning
hashing
efficient move generation
```

---

# 50. A clean first engine

For your first version, I would intentionally make it stupid.

### Version 1

Board:

```cpp
Piece board[64];
```

Evaluation:

```cpp
material only
```

Search:

```cpp
negamax
```

Depth:

```text
4–5
```

No GUI.

Just:

```text
Input FEN
Output best move + eval
```

Example:

```text
position:
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

bestmove e2e4
eval +0.15
nodes 183029
```

---

# 51. Then build it up

I'd use roughly this progression:

```text
Stage 1
Board representation

Stage 2
Pseudo-legal move generation

Stage 3
Check detection

Stage 4
Legal move generation

Stage 5
makeMove / undoMove

Stage 6
Perft
```

At this point you have a correct chess rules engine.

Then:

```text
Stage 7
Material evaluation

Stage 8
Negamax

Stage 9
Alpha-beta

Stage 10
Iterative deepening

Stage 11
Quiescence
```

Now you have a real chess engine.

Then:

```text
Stage 12
Piece-square tables

Stage 13
Zobrist hashing

Stage 14
Transposition table

Stage 15
Move ordering

Stage 16
Killer/history heuristic

Stage 17
Better pawn/king evaluation
```

Then optimization:

```text
Stage 18
Bitboards

Stage 19
Faster attack generation

Stage 20
Null move pruning

Stage 21
LMR

Stage 22
Better time management
```

And only way later:

```text
NNUE
```

---

# 52. The full mental model

When your GUI asks:

> What should the eval bar display?

Your engine does something conceptually like:

```cpp
SearchResult search(Position position) {

    Move bestMove;
    int bestScore;

    for (int depth = 1; !timeUp(); depth++) {

        auto result =
            alphaBeta(position, depth);

        bestMove = result.move;
        bestScore = result.score;
    }

    return {
        bestMove,
        bestScore
    };
}
```

Suppose:

```text
bestScore = 173
```

Then:

```text
engine evaluation = +1.73
```

Your UI converts:

```text
+173 centipawns
```

to something like:

```text
70% white bar
30% black bar
```

Visually:

```text
┌──────┐
│██████│ Black
│██████│
│██████│
│──────│
│      │
│      │ White
│      │
└──────┘
```

or inverted depending on your orientation.

And when the player makes another move:

```text
position changes
→ search starts again
→ new best score
→ bar animates to new value
```

That's basically how the system fits together.

The most fun part is that once you've implemented **legal move generation + perft + alpha-beta**, you suddenly have something that genuinely plays chess. Everything after that is repeatedly asking: **"How can I search one ply deeper without doing exponentially more work?"** That's where chess engines become a really interesting algorithms/systems project.

---

https://chatgpt.com/share/6aab1459-f2cc-83ee-85c5-6aa305182bea

