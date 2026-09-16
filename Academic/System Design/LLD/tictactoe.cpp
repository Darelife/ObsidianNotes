/*
 Board : int[n][n] = 0 -> empty, 1 -> player1, 2 -> player2
 Winner "First", "Second", "Draw", "Pending"

  Board
  +Board()
  +reset()
  +getBoard()
  +getWinner()
  +getCurrentPlayer()
  +makeMove(Move m) : Move = {int player, int row, int col}

  User
  User id
  Stats (ELO Rating, Win/Loss/Draw, Best Opponent, Best Opponent Defeated)

  Game
  -gameId
  -user1
  -user2
  -ListOfMoves
  +initializeGame()
  +undo()
 */

#include <bits/stdc++.h>
using namespace std;

struct Move
{
  int player;
  int row;
  int col;
};

struct User
{
  int userId;
  int eloRating;
  int wins;
  int losses;
  int draws;
  int bestOpponentId;
  int bestOpponentDefeatedId;

  int getId() const;
};

int User::getId() const { return userId; }

class Board
{
  int n;
  int **board;
  Move *moves;
  int moveCount;

  Board(int size) : n(size)
  {
    board = new int *[n];
    for (int i = 0; i < n; i++)
    {
      board[i] = new int[n];
      for (int j = 0; j < n; j++)
      {
        board[i][j] = 0;
      }
    }
    moveCount = 0;
    moves = new Move[n * n];
  }

  void reset()
  {
    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
      {
        board[i][j] = 0;
      }
    }
    moveCount = 0;
    delete[] moves;
    moves = new Move[n * n];
  }

  int **getBoard() const
  {
    return board;
  }

  void printBoard() const
  {
    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
      {
        cout << board[i][j] << " ";
      }
      cout << endl;
    }
  }

  void getWinner(Move lastMove) const
  {
    // in O(n) time, check if the last move made the player win, if not, check if the number of moves made is n*n, if yes, return "Draw", else return "Pending"
    int player = lastMove.player;
    int row = lastMove.row;
    int col = lastMove.col;

    for (int i = 0; i < n; i++)
    {
      if (board[row][i] != player)
      {
        break;
      }
      if (i == n - 1)
      {
        cout << "Winner: Player " << player << endl;
        return;
      }
    }

    for (int i = 0; i < n; i++)
    {
      if (board[i][col] != player)
      {
        break;
      }
      if (i == n - 1)
      {
        cout << "Winner: Player " << player << endl;
        return;
      }
    }

    if (row == col)
    {
      for (int i = 0; i < n; i++)
      {
        if (board[i][i] != player)
        {
          break;
        }
        if (i == n - 1)
        {
          cout << "Winner: Player " << player << endl;
          return;
        }
      }
    }

    if (row + col == n - 1)
    {
      for (int i = 0; i < n; i++)
      {
        if (board[i][n - 1 - i] != player)
        {
          break;
        }
        if (i == n - 1)
        {
          cout << "Winner: Player " << player << endl;
          return;
        }
      }
    }

    if (moveCount == n * n)
    {
      cout << "Draw" << endl;
    }
    else
    {
      cout << "Pending" << endl;
    }
  }

  ~Board()
  {
    for (int i = 0; i < n; i++)
    {
      delete[] board[i];
    }
    delete[] board;
  }
};