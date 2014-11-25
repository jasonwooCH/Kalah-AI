#include "Game.h"

Game::Game(const Board& b, Player* south, Player* north)
	:m_Splayer(south), m_Nplayer(north), m_board(b), m_yourTurn(south)
{}

void Game::display() const
{
	// Display the player name and the board

	std::cout << "          " << m_Nplayer->name() << endl;

	std::cout << "     ";
	for (int i = 1; i <= m_board.holes(); i++) {
		std::cout << m_board.beans(NORTH, i) << "   ";
	}
	std::cout << endl;

	std::cout << "  " << m_board.beans(NORTH, 0);

	// good spacing to make it look symmetric
	for (int i = 0; i < m_board.holes(); i++) {
		std::cout << "    ";
	}

	std::cout << m_board.beans(SOUTH, 0) << endl;

	std::cout << "     ";
	for (int i = 1; i <= m_board.holes(); i++) {
		std::cout << m_board.beans(SOUTH, i) << "   ";
	}
	std::cout << endl;

	std::cout << "          " << m_Splayer->name() << endl;
}

void Game::status(bool& over, bool& hasWinner, Side& winner) const
{
	// Game status and checking winner
	if (m_board.beansInPlay(NORTH) != 0 && m_board.beansInPlay(SOUTH) != 0)
		over = false;

	else {
		over = true;

		if (m_board.beans(SOUTH, 0) == m_board.beans(NORTH, 0))
			hasWinner = false;

		else if (m_board.beans(SOUTH, 0) > m_board.beans(NORTH, 0)) {
			hasWinner = true;
			winner = SOUTH;
		}

		else {
			hasWinner = true;
			winner = NORTH;
		}
	}
}

bool Game::move()
{
	bool over, hasWinner; Side winner;
	status(over, hasWinner, winner);

	if (over)
		return false;

	Side endSide, turn; int endHole; Player* notTurn;

	if (m_yourTurn == m_Splayer) {      // indicate whose turn it is
		turn = SOUTH;
		notTurn = m_Nplayer;
	}
	else {
		turn = NORTH;
		notTurn = m_Splayer;
	}

	int turnMove = m_yourTurn->chooseMove(m_board, turn);

	std::cout << m_yourTurn->name() << " chooses hole " << turnMove << endl;

	m_board.sow(turn, turnMove, endSide, endHole);

	// The game rule is that you get another turn when the last bean is dropped in the player's pot
	while (endHole == 0) {
		display();
		std::cout << m_yourTurn->name() << " gets another turn." << endl;
		int turnMove = m_yourTurn->chooseMove(m_board, turn);
		std::cout << m_yourTurn->name() << " chooses hole " << turnMove << endl;
		m_board.sow(turn, turnMove, endSide, endHole);
	}

	// Capture
	// If player lands the last bean in his/her own empty hole, and directly opposite to that hole
	// opposition's hole is not empty, the player in turn captures those beans. 
	if (endSide == turn && m_board.beans(endSide, endHole) == 1 && m_board.beans(opponent(turn), endHole) > 0) {
		m_board.moveToPot(turn, endHole, turn);
		m_board.moveToPot(opponent(turn), endHole, turn);
		display();
	}
	else
		display();

	m_yourTurn = notTurn;   // change turns

	return true;
}

void Game::play()
{
	bool over, hasWinner; Side winner;
	status(over, hasWinner, winner);

	display();

	if (!m_Splayer->isInteractive() && !m_Nplayer->isInteractive()) {
		while (!over) {
			move();
			std::cout << "Press ENTER to continue";   // so that each computer turn, we can follow the moves
			std::cin.ignore(1000, '\n');
			status(over, hasWinner, winner);    // keep checking status every move
		}
	}

	else {
		while (!over) { // play until over
			move();
			status(over, hasWinner, winner);
		}
	}

	// only if game is over, it will get here
	// at most one side will have beans left over. Sweep those into the pot
	if (m_board.beansInPlay(SOUTH) != 0) {
		for (int i = 1; i < m_board.holes() + 1; i++) { // game over, sweep any left overs
			m_board.moveToPot(SOUTH, i, SOUTH);
		}
		std::cout << "Sweeping remaining beans into " << m_Splayer->name() << "'s pot." << endl;
	}
	else if (m_board.beansInPlay(NORTH) != 0) {
		for (int i = 1; i < m_board.holes() + 1; i++) {
			m_board.moveToPot(NORTH, i, NORTH);
		}
		std::cout << "Sweeping remaining beans into " << m_Nplayer->name() << "'s pot." << endl;
	}

	display();  // show the end result

	status(over, hasWinner, winner);  // after sweep, need to check who really won

	if (!hasWinner) {
		std::cout << "The game resulted in a tie." << endl;
		return;
	}

	std::string winnerName;
	if (winner == SOUTH)
		winnerName = m_Splayer->name();
	else
		winnerName = m_Nplayer->name();

	std::cout << "The winner is " << winnerName << "." << endl;
	return;
}

int Game::beans(Side s, int hole) const
//Return the number of beans in the indicated hole or pot of the game's board, or −1 if the hole number is invalid. This function exists so that we and you can more easily test your program.
{
	if (hole < 0 || hole > m_board.holes())
		return -1;

	return m_board.beans(s, hole);
}
