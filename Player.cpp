#include "Player.h"

//========================================================================
// Timer t;                 // create a timer
// t.start();               // start the timer
// double d = t.elapsed();  // milliseconds since timer was last started
//========================================================================

#if __cplusplus == 201103L  // C++11

#include <chrono>

class Timer
{
public:
	Timer()
	{
		start();
	}
	void start()
	{
		m_time = std::chrono::high_resolution_clock::now();
	}
	double elapsed() const
	{
		std::chrono::duration<double, std::milli> diff =
			std::chrono::high_resolution_clock::now() - m_time;
		return diff.count();
	}
private:
	std::chrono::high_resolution_clock::time_point m_time;
};

#elif defined(_MSC_VER)  // not C++11, but Windows

#include <windows.h>

class Timer
{
public:
	Timer()
	{
		QueryPerformanceFrequency(&ticksPerSecond);
		start();
	}
	void start()
	{
		QueryPerformanceCounter(&m_time);
	}
	double elapsed() const
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000.0 * (now.QuadPart - m_time.QuadPart)) / ticksPerSecond.QuadPart;
	}
private:
	LARGE_INTEGER m_time;
	LARGE_INTEGER ticksPerSecond;
};

#else // not C++11 or Windows, so C++98

#include <ctime>

class Timer
{
public:
	Timer()
	{
		start();
	}
	void start()
	{
		m_time = std::clock();
	}
	double elapsed() const
	{
		return (1000.0 * (std::clock() - m_time)) / CLOCKS_PER_SEC;
	}
private:
	std::clock_t m_time;
};

#endif
///////////////////////////////
void bestMove(Side s, GameTree* test, int& bestHole, int& value, Timer& timeOut);
///////////////////////////////


Player::Player(std::string name)
	:m_name(name)
{}

std::string Player::name() const { return m_name; }

bool Player::isInteractive() const { return false; }

HumanPlayer::HumanPlayer(std::string name)
	:Player(name)
{ }

BadPlayer::BadPlayer(std::string name)
	: Player(name)
{ }

SmartPlayer::SmartPlayer(std::string name)
	: Player(name)
{
	m_root = new GameTree;
}

int BadPlayer::chooseMove(const Board& b, Side s) const
{
	// The bad A.I. will always choose the first hole with beans
	// regardless of the condition of the game
	if (b.beansInPlay(s) == 0)
		return -1;

	for (int i = 1; i < b.holes() + 1; i++) {
		if (b.beans(s, i) != 0) {
			return i;
		}
	}

	return -1;
}

int HumanPlayer::chooseMove(const Board &b, Side s) const
{
	// Asks for human input for moves
	// If error, repeatedly alerts and asks again
	if (b.beansInPlay(s) == 0)
		return -1;

	int input;

	std::cout << "Select a hole, " << name() << ": ";
	std::cin >> input;

	while (input < 1 || input > b.holes()) {
		std::cout << "The hole number must be from 1 to " << b.holes() << "." << endl;
		std::cout << "Select a hole, " << name() << ": ";
		std::cin >> input;
	}

	while (b.beans(s, input) == 0) {
		std::cout << "There are no beans in that hole." << endl;
		std::cout << "Select a hole, " << name() << ": ";
		std::cin >> input;
	}

	return input;
}

int SmartPlayer::chooseMove(const Board &b, Side s) const
{
	if (b.beansInPlay(s) == 0)
		return -1;

	// make a copy of the current board in play
	m_root->m_copy = new Board(b);

	int bH, val;
	switch (s) {
	case SOUTH:
		val = -1000;    // so that when no good moves are available, they will still choose -999
		break;          // worst case scenario, just in case
	case NORTH:
		val = 1000;
		break;
	}

	Timer timeOut;
	timeOut.start();

	bestMove(s, m_root, bH, val, timeOut);

	return bH;
}

void bestMove(Side s, GameTree* test, int& bestHole, int& value, Timer& timeOut)
{
	Board* current = test->m_copy;  // The board we are currently evaluating
	Side opp = opponent(s);
	int holes = current->holes();

	if (current->beansInPlay(s) == 0 || current->beansInPlay(opp) == 0) {
		
		for (int i = 1; i <= holes; i++) {
			current->moveToPot(s, i, s);
			current->moveToPot(opp, i, opp);    //if game is over, clean up whatever might be left
		}

		if (current->beans(s, 0) > current->beans(opp, 0))  //when game is won
			switch (s) {
			case SOUTH:
				value = 999;    // big for SOUTH, small for NORTH
				break;
			case NORTH:
				value = -999;
				break;
		}
		else if (current->beans(s, 0) < current->beans(opp, 0)) // if game is lost
			switch (s) {
			case SOUTH:
				value = -999;   // small for SOUTH, big for NORTH
				break;
			case NORTH:
				value = 999;
		}
		else
			value = 0;  // tied

		bestHole = -1;
		return;
	}

	if (timeOut.elapsed() > 4900) { // within 4.9 seconds
		bestHole = -1;
		value = (current->beans(SOUTH, 0) - current->beans(NORTH, 0));
		return;
	}

	for (int i = 1; i <= holes; i++) {

		int num;
		if (s == SOUTH)
			num = holes + 1 - i;    // numbering system for south holes
		else
			num = i;

		if (current->beans(s, num) > 0) {   // enter if not an empty hole

			GameTree* branch = new GameTree;

			Board* branch_board = branch->m_copy = new Board(*current); // copy of the board on the branch

			Side endSide; int endHole;

			int bh2, val2;

			branch_board->sow(s, num, endSide, endHole);    //sow the branch copy

			if (endHole == 0)
			{
				bestMove(s, branch, bh2, val2, timeOut);
			}

			else if (endSide == s && endHole != 0 && branch_board->beans(s, endHole) == 1
				&& branch_board->beans(opp, endHole) > 0)
			{
				branch_board->moveToPot(s, endHole, s); // capture
				branch_board->moveToPot(opp, endHole, s);
			}

			bestMove(opp, branch, bh2, val2, timeOut);  // check the lower levels

			if ((val2 > value && s == SOUTH)
				|| (val2 < value && s == NORTH)) { // bigger number for south, lower for north

				value = val2;

				bestHole = num;
			}

			delete branch_board; delete branch; // delete the dynamically allocated copies
		}
	}

	return;
}
