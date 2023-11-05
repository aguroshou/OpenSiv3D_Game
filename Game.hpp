# pragma once
# include "Common.hpp"

// ゲームシーン
class Game : public App::Scene
{
private:

	enum BodyType {
		DEFAULT,
		PLAYER,
		ENEMY,
		ITEM
	};

public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;

	P2Body* findBodyFromID(P2BodyID id, const Array<P2Body>& bodyList);



private:
	// FIXME; 1つの関数に2つの処理を組み合わせてしまっているので、修正したほうが良いかもしれません…。
	P2Body* findBodyAndSetBodyTypeFromID(P2BodyID id, BodyType& type);

	bool IsCloseToOtherBodies(const Vec2& position, const Array<P2Body>& otherBodies, const float radius);

	const int RETRY_COUNT_MAX = 100;
	Vec2 GetRandomPositionWithSafety(const Array<P2Body>& otherBodies);

private:

	// 2D 物理演算のシミュレーションステップ（秒）
	const double StepSec = (1.0 / 200.0);
	// 2D 物理演算のシミュレーション蓄積時間（秒）
	double accumulatorSec = 0.0;

	// 2D 物理演算のワールド
	P2World world;
	Array<P2Body>  walls;

	int32 playerHitPoint = 3;

	const double playerAnimalRadius = 20;
	//Array<P2Body> playerAnimals;
	Array<P2Body> playerAnimals;

	bool isPlayerAnimalGrab = false;
	int32 grabAnimalIndex = 0;
	bool isGrabbing = false;

	const double itemRadius = 20;
	Array<P2Body> items;

	const double enemyAnimalRadius = 20;
	Array<P2Body> enemyAnimals;

	const double objectSize = 20;
	Array<P2Body> objects;

	// 基本サイズ 50 のフォントを作成
	//const Font font{ 50 };
	const Font font{ 48, U"images/DelaGothicOne-Regular.ttf" };


	// 詳しい仕組みを理解できていませんが、カメラ座標を(640, 360)とすることで、Cursor::PosF()とplayerAnimals[0].getPos()の座標が一致するようです。
	// そのため、カメラの座標・ズームなどを変更してしまうと不具合が発生してしまいます。
	// playerAnimalsのドラッグアンドドロップの判定方法を変更しているため、この実装方法としています。
	Camera2D camera{ Vec2{ 640, 360 }, 1.0, CameraControl::None_ };
	Array<P2Body> bodies;

	// 1秒毎に1点を加算するためのタイマーです。
	Stopwatch oneSecondScoreTimer;
	// ゲーム開始後の累計時間のタイマーです。
	Stopwatch gameTimer;

	Stopwatch spawnPlayerTimer;
	double spawnPlayerIntervalTimeMax = 5;
	double spawnPlayerIntervalAcceleration = 0.05; // 5%ずつスポーン時間が早くなっていきます。

	Stopwatch spawnEnemyTimer;
	double spawnEnemyIntervalTimeMax = 5;
	double spawnEnemyIntervalAcceleration = -0.1; // 10%ずつスポーン時間が遅くなっていきます。

	Stopwatch spawnObjectTimer;
	double spawnObjectIntervalTimeMax = 5;
	double spawnObjectIntervalAcceleration = -1; // 100%ずつスポーン時間が遅くなっていきます。5,10,20,40,80秒後にスポーンします。
	int32 objectMaxNumber = 5;

	//int32 spawnPlayerCount = 0;
	//std::vector<int32> spawnPlayerTimes = { 0,10,20,30,40,50 };
	//int32 spawnPlayerCountInterval = 0;

	//int32 spawnEnemyCount = 0;
	//std::vector<int32> spawnEnemyTimes = { 0,10,20,30,40,50 };

	// レベルは、ステージ上のモグラの数です。
	int32 level = 5;
	// 経験値==レベルとなったときに、レベルが上がります。
	int32 experiencePoint = 0;

	int32 score = 0;

	Vec2 clickStartPosition = Vec2(0, 0);

	const Texture textureMole{ U"images/mole.png" };
	const Texture textureHole{ U"images/hole.png" };
	const Texture textureHammer{ U"images/hammer.png" };
	const Texture textureGame{ U"images/game.png" };
	const Texture textureGameMenu{ U"images/gameMenu.png" };
	const Texture textureRock{ U"images/rock.png" };

	const Audio dragAudio{ U"sounds/drag.wav" };
	const Audio dropWaitAudio{ U"sounds/dropWait.wav" };
	const Audio dropMoveAudio{ U"sounds/dropMove.wav" };
	const Audio enemyHitAudio{ U"sounds/enemyHit.wav" };
	const Audio goalAudio{ U"sounds/goal.wav" };
	const Audio playerHitPoint1Audio{ U"sounds/playerHitPoint1.wav" };
	const Audio gameBgmAudio{ Audio::Stream, U"sounds/gameBgm.wav", Loop::Yes };

};


