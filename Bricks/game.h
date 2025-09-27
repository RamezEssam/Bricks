#pragma once
#include "raylib.h"


namespace Bricks {
	enum class BallState {
		Resting,
		Launched,
		Flight,
		Return
	};

	enum class BrickState {
		Alive,
		Dead
	};

	
	struct GameState {
		bool ballsReturned;
		int numBallsReturned;
		int bricksALive;
		int currentLevel;
	};

	class CanonBall {
	private:
		Vector2 pos;
		Vector2 dir;
		float velocity;
		float radius;
		Color color;
		float lag;
		BallState state;
		BallState prev_state;
	public:
		CanonBall(Vector2 pos, Vector2 dir, float r, Color c, int lag);

		void Move(float v, Vector2 dir);

		Vector2 GetPos() const;

		void SetPos(Vector2 pos);

		Vector2 GetDir() const;

		void SetDir(Vector2 dir);

		float GetVelocity() const;

		void SetVelocity(float v);

		float GetRadius() const;

		void SetRadius(float r);

		Color GetColor() const;

		void SetColor(Color c);

		float GetLag() const;

		void SetLag(float lag);

		void SetState(BallState s);

		BallState GetState() const;

		void SetPrevState(BallState s);

		BallState GetPrevState() const;



	};

	class Brick {
	private:
		int length;
		int health;
		Color color;
		Vector2 pos;
		BrickState state;
	public:
		Brick(int length, int health, Color color, Vector2 pos);

		int GetLength() const;

		int GetHealth() const;

		Color GetColor() const;

		Vector2 GetPos() const;

		void SetLength(int l);

		void SetHealth(int h);

		void SetColor(Color c);

		void SetState(BrickState s);

		BrickState GetState() const;
	};


	class Game {
	private:
		GameState gameState;
	public:
		Game();
		void SetBallsState(bool ballsReturned);
		GameState GetGameState() const;
		void SetGameState(GameState s);
	};
}
