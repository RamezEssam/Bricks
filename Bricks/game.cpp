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
		return this->gameState;
	}

	void Game::SetGameState(GameState s) {
		this->gameState = s;
	}

	// ---------------------------   CanonBall  --------------------------------------------------------
	CanonBall::CanonBall(Vector2 pos, Vector2 dir, float r, Color c, int lag) {
		this->pos = pos;
		this->dir = dir;
		this->radius = r;
		this->velocity = 0.0f;
		this->color = c;
		this->lag = lag;
		this->state = BallState::Resting;
		this->prev_state = BallState::Resting;
	}

	void CanonBall::Move(float v, Vector2 dir) {
		
		this->pos.x += dir.x * v;
		this->pos.y += dir.y * v;
		this->dir = dir;
		this->velocity = v;
	}

	Vector2 CanonBall::GetPos() const {
		return this->pos;
	}

	void CanonBall::SetPos(Vector2 pos) {
		this->pos = pos;
	}

	Vector2 CanonBall::GetDir() const {
		return this->dir;
	}

	void CanonBall::SetDir(Vector2 dir) {
		this->dir = dir;
	}

	float CanonBall::GetVelocity() const {
		return this->velocity;
	}

	void CanonBall::SetVelocity(float v) {
		this->velocity = v;
	}

	float CanonBall::GetRadius() const {
		return this->radius;
	}

	void CanonBall::SetRadius(float r) {
		this->radius = r;
	}

	Color CanonBall::GetColor() const {
		return this->color;
	}

	void CanonBall::SetColor(Color c) {
		this->color = c;
	}

	float CanonBall::GetLag() const {
		return this->lag;
	}

	void CanonBall::SetLag(float lag) {
		if (this->lag >= 0) {
			this->lag= lag;
		}
	}

	void CanonBall::SetState(Bricks::BallState s) {
		this->state = s;
	}

	BallState CanonBall::GetState() const {
		return this->state;
	}

	void CanonBall::SetPrevState(Bricks::BallState s) {
		this->prev_state = s;
	}

	BallState CanonBall::GetPrevState() const {
		return this->prev_state;
	}


	// ---------------------------   Brick  --------------------------------------------------------
	Brick::Brick() {
		this->health = 0;
		this->length = 0;
		this->color = {};
		this->pos = {};
		this->state = BrickState::Dead;
	}
	Brick::Brick(int length, int health, Color color, Vector2 pos) {
		this->health = health;
		this->length = length;
		this->color = color;
		this->pos = pos;
		this->state = BrickState::Alive;
	}

	int Brick::GetLength() const {
		return this->length;
	}

	int Brick::GetHealth() const {
		return this->health;
	}

	Color Brick::GetColor() const {
		return this->color;
	}

	Vector2 Brick::GetPos() const {
		return this->pos;
	}

	void Brick::SetLength(int l) {
		this->length = l;
	}

	void Brick::SetHealth(int h) {
		this->health = h;
	}

	void Brick::SetColor(Color c) {
		this->color = c;
	}

	void Brick::SetState(BrickState s) {
		this->state = s;
	}

	BrickState Brick::GetState() const {
		return this->state;
	}
}