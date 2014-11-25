#ifndef BOARD_DEFINED
#define BOARD_DEFINED

#include <iostream>
#include "Side.h"
using namespace std;

class Board
{
public:
	Board(int nHoles, int nInitialBeansPerHole);
	Board(const Board& copy);
	~Board();
	int holes() const;
	int beans(Side s, int hole) const;
	int beansInPlay(Side s) const;
	int totalBeans() const;
	bool sow(Side s, int hole, Side& endSide, int& endHole);
	bool moveToPot(Side s, int hole, Side potOwner);
	bool setBeans(Side s, int hole, int beans);
private:
	int m_nholes;
	int m_nbeans;
	int* m_north;
	int* m_south;
};

#endif //BOARD_DEFINED
