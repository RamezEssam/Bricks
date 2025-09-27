#include "game.h"

namespace Bricks {
	// ---------------------------   Game  --------------------------------------------------------
	Game::Game() {
		this->gameState = GameState{ true , 0, 0, 0};
	}
	void Game::SetBallsState(bool ballsReturned) {
		this->gameState.ballsReturned = ballsReturned;
	}

	GameState Game::GetGameState() const {
		return Game::gameState;
	}

	void Game::SetGameState(GameState s) {
		this->gameState = s;
	}

	// ---------------------------   CanonBall  --------------------------------------------------------
	CanonBall::CanonBall(Vector2 pos, Vector2 dir, float r, Color c, int lag) {
		CanonBall::pos = pos;
		CanonBall::dir = dir;
		CanonBall::radius = r;
		CanonBall::velocity = 0.0f;
		CanonBall::color = c;
		CanonBall::lag = lag;
		CanonBall::state = BallState::Resting;
		CanonBall::prev_state = BallState::Resting;
	}

	void CanonBall::Move(float v, Vector2 dir) {
		
		CanonBall::pos.x += dir.x * v;
		CanonBall::pos.y += dir.y * v;
		CanonBall::dir = dir;
		CanonBall::velocity = v;
	}

	Vector2 CanonBall::GetPos() const {
		return CanonBall::pos;
	}

	void CanonBall::SetPos(Vector2 pos) {
		CanonBall::pos = pos;
	}

	Vector2 CanonBall::GetDir() const {
		return CanonBall::dir;
	}

	void CanonBall::SetDir(Vector2 dir) {
		CanonBall::dir = dir;
	}

	float CanonBall::GetVelocity() const {
		return CanonBall::velocity;
	}

	void CanonBall::SetVelocity(float v) {
		CanonBall::velocity = v;
	}

	float CanonBall::GetRadius() const {
		return CanonBall::radius;
	}

	void CanonBall::SetRadius(float r) {
		CanonBall::radius = r;
	}

	Color CanonBall::GetColor() const {
		return CanonBall::color;
	}

	void CanonBall::SetColor(Color c) {
		CanonBall::color = c;
	}

	float CanonBall::GetLag() const {
		return CanonBall::lag;
	}

	void CanonBall::SetLag(float lag) {
		if (CanonBall::lag >= 0) {
			CanonBall::lag = lag;
		}
	}

	void CanonBall::SetState(Bricks::BallState s) {
		CanonBall::state = s;
	}

	BallState CanonBall::GetState() const {
		return CanonBall::state;
	}

	void CanonBall::SetPrevState(Bricks::BallState s) {
		CanonBall::prev_state = s;
	}

	BallState CanonBall::GetPrevState() const {
		return CanonBall::prev_state;
	}


	// ---------------------------   Brick  --------------------------------------------------------
	Brick::Brick(int length, int health, Color color, Vector2 pos) {
		Brick::health = health;
		Brick::length = length;
		Brick::color = color;
		Brick::pos = pos;
		Brick::state = BrickState::Alive;
	}

	int Brick::GetLength() const {
		return Brick::length;
	}

	int Brick::GetHealth() const {
		return Brick::health;
	}

	Color Brick::GetColor() const {
		return Brick::color;
	}

	Vector2 Brick::GetPos() const {
		return Brick::pos;
	}

	void Brick::SetLength(int l) {
		Brick::length = l;
	}

	void Brick::SetHealth(int h) {
		Brick::health = h;
	}

	void Brick::SetColor(Color c) {
		Brick::color = c;
	}

	void Brick::SetState(BrickState s) {
		Brick::state = s;
	}

	BrickState Brick::GetState() const {
		return Brick::state;
	}
}