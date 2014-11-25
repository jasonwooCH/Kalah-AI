#ifndef SIDE_DEFINED
#define SIDE_DEFINED

//#include <vector>

class Board;
class SmartPlayer;

using namespace std;

class Game;

enum Side { NORTH, SOUTH };

inline
Side opponent(Side s)
{
	return Side(1 - s);
}

struct GameTree // for SmartPlayer move;
{
	Board* m_copy;
};

#endif // SIDE_DEFINED
