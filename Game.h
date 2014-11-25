#ifndef GAME_INCLUDED
#define GAME_INCLUDED

#include "Board.h"
#include "Side.h"
#include "Player.h"

class Game
{
public:
	Game(const Board& b, Player* south, Player* north);
	void display() const;
	void status(bool& over, bool& hasWinner, Side& winner) const;
	bool move();
	void play();
	int beans(Side s, int hole) const;
private:
	Player* m_Splayer;
	Player* m_Nplayer;
	Board m_board;
	Player* m_yourTurn;
};


#endif // GAME_INCLUDED
