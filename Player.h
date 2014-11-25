#ifndef PLAYER_DEFINED
#define PLAYER_DEFINED

#include <iostream>
#include <string>
#include <vector>
#include "Side.h"
#include "Board.h"

class Player
{
public:
	Player(std::string name);
	std::string name() const;
	virtual bool isInteractive() const;
	virtual int chooseMove(const Board& b, Side s) const = 0;
	virtual ~Player() {};
private:
	std::string m_name;
};

class HumanPlayer : public Player
{
public:
	HumanPlayer(std::string name);
	virtual bool isInteractive() const { return true; }
	virtual int chooseMove(const Board& b, Side s) const;
	virtual ~HumanPlayer() {};
};

class BadPlayer : public Player
{
public:
	BadPlayer(std::string name);
	virtual int chooseMove(const Board& b, Side s) const;
	virtual ~BadPlayer() {};
};

class SmartPlayer : public Player
{
public:
	SmartPlayer(std::string name);
	virtual int chooseMove(const Board& b, Side s) const;
	virtual ~SmartPlayer() { delete m_root; }
private:
	GameTree* m_root;
};

#endif // PLAYER_DEFINED
