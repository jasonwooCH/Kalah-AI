#include "Board.h"

Board::Board(int nHoles, int nInitialBeansPerHole)
	:m_nholes(nHoles), m_nbeans(nInitialBeansPerHole)
{
	m_north = new int[m_nholes + 1];
	m_south = new int[m_nholes + 1];

	for (int i = 1; i < m_nholes + 1; i++) {
		m_north[i] = m_nbeans;
		m_south[i] = m_nbeans;
	}

	*m_north = *m_south = 0;
}

Board::Board(const Board& copy) // copy constructor
{
	m_nholes = copy.m_nholes;
	m_nbeans = copy.m_nbeans;

	m_north = new int[m_nholes + 1];  // new dynamically allocated arrays
	m_south = new int[m_nholes + 1];

	for (int i = 0; i < m_nholes + 1; i++) {  // copy the values
		m_north[i] = copy.m_north[i];
		m_south[i] = copy.m_south[i];
	}
}

Board::~Board()
{
	delete[] m_north;
	delete[] m_south;
}

int Board::holes() const
//Return the number of holes on a side (not counting the pot).
{
	return m_nholes;
}

int Board::beans(Side s, int hole) const
//Return the number of beans in the indicated hole or pot, or −1 if the hole number is invalid.
{

	if (hole > m_nholes || hole < 0) {
		return -1;
	}

	if (s == SOUTH && hole == 0) {
		return *m_south;
	}

	if (s == SOUTH)
		return m_south[m_nholes + 1 - hole];

	return m_north[hole];
}

int Board::beansInPlay(Side s) const
//Return the total number of beans in all the holes on the indicated side, not counting the beans in the pot.
{
	int count = 0;
	if (s == SOUTH) {
		for (int i = 1; i < m_nholes + 1; i++) {
			count += *(m_south + i);
		}
	}
	else {
		for (int i = 1; i < m_nholes + 1; i++) {
			count += *(m_north + i);
		}
	}

	return count;
}

int Board::totalBeans() const
//Return the total number of beans in the game.
{
	return 2 * m_nholes * m_nbeans;
}

bool Board::sow(Side s, int hole, Side& endSide, int& endHole)
{
	if (hole < 1 || hole > m_nholes || beans(s, hole) == 0)
		return false;

	int sowing = beans(s, hole);
	int* p;
	int* north_end = m_north + m_nholes;    // easy access to locations in array
	int* south_end = m_south + m_nholes;
	int* opposite_end; int* opposite_pot;
	int* my_end; int* my_pot;

	if (s == SOUTH) {
		hole = (m_nholes + 1 - hole);
		p = (m_south + hole);
		*p = 0;
		opposite_end = north_end;   // easy access
		opposite_pot = m_north;
		my_end = south_end;
		my_pot = m_south;
	}

	else {
		p = (m_north + hole);
		*p = 0;
		opposite_end = south_end;
		opposite_pot = m_south;
		my_end = north_end;
		my_pot = m_north;
	}

	endSide = s;
	endHole = hole;

	while (sowing != 0) {

		if (p == my_pot) {  // if it was at pot before, point to the opponent
			p = opposite_end + 1;
			endSide = opponent(s);
			endHole = m_nholes + 1;
		}

		p--; endHole--;

		if (p == opposite_pot) {   // if at opponent pot, don't wanna sow into it
			p = my_end;             // come back to my side
			endSide = s;
			endHole = m_nholes;
		}

		*p += 1;
		sowing--;
	}

	if (endSide == SOUTH && endHole != 0)
		endHole = m_nholes + 1 - endHole;   // south number back to normal

	return true;
}

bool Board::moveToPot(Side s, int hole, Side potOwner)
{
	if (hole < 1 || hole > m_nholes || beans(s, hole) == 0)
		return false;

	int move = beans(s, hole);

	switch (s) {
	case SOUTH:
		*(m_south + m_nholes + 1 - hole) = 0;   // set the location to zero
		break;
	case NORTH:
		*(m_north + hole) = 0;
		break;
	}

	switch (potOwner) {
	case SOUTH:
		*m_south += move;   // add to pot
		break;
	case NORTH:
		*m_north += move;
		break;
	}

	return true;
}

bool Board::setBeans(Side s, int hole, int beans)
{
	if (hole < 0 || hole > m_nholes || beans < 0)
		return false;

	int before = Board::beans(s, hole);

	switch (s) {
	case SOUTH:
		if (hole == 0)
			*m_south = beans;
		else
			*(m_south + m_nholes + 1 - hole) = beans;
		break;
	case NORTH:
		*(m_north + hole) = beans;
		break;
	}

	int difference = beans - before;    // change total bean number
	m_nbeans += difference;

	return true;
}
