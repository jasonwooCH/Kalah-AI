#include "Board.h"
#include "Player.h"
#include "Game.h"
#include "Side.h"

int main()
{
	HumanPlayer hp("Marge");
	SmartPlayer bp("Homer");
	Board b(5, 2);
	Game g(b, &hp, &bp);
	g.play();
}

