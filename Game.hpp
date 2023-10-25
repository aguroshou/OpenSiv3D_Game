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

	//↓おそらく不要です。
	P2Body* findPlayerFromID(P2BodyID id);
	P2Body* findEnemyFromID(P2BodyID id);
	P2Body* findItemFromID(P2BodyID id);

	P2Body* findBodyFromID(P2BodyID id, const Array<P2Body>& bodyList);

	// FIXME; 1つの関数に2つの処理を組み合わせてしまっているので、修正したほうが良いかもしれません…。
	P2Body* findBodyAndSetBodyTypeFromID(P2BodyID id, BodyType& type);

private:

	// 2D 物理演算のシミュレーションステップ（秒）
	const double StepSec = (1.0 / 200.0);
	// 2D 物理演算のシミュレーション蓄積時間（秒）
	double accumulatorSec = 0.0;

	// 2D 物理演算のワールド
	P2World world;
	Array<P2Body>  walls;

	const double playerAnimalRadius = 20;
	Array<P2Body> playerAnimals;
	Array<P2BodyID> playerAnimalIDs;

	bool isPlayerAnimalGrab = false;
	int32 grabAnimalIndex = 0;
	bool isGrabbing = false;

	const double itemRadius = 20;
	Array<P2Body> items;
	Array<P2BodyID> itemIDs;

	const double enemyAnimalRadius = 20;
	Array<P2Body> enemyAnimals;
	Array<P2BodyID> enemyAnimalIDs;

	// 基本サイズ 50 のフォントを作成
	const Font font{ 50 };

	// 詳しい仕組みを理解できていませんが、カメラ座標を(640, 360)とすることで、Cursor::PosF()とplayerAnimals[0].getPos()の座標が一致するようです。
	// そのため、カメラの座標・ズームなどを変更してしまうと不具合が発生してしまいます。
	// playerAnimalsのドラッグアンドドロップの判定方法を変更しているため、この実装方法としています。
	Camera2D camera{ Vec2{ 640, 360 }, 1.0, CameraControl::None_ };
	Array<P2Body> bodies;

	// 1秒毎に1点を加算するためのタイマーです。
	Stopwatch oneSecondScoreTimer;
	// ゲーム開始後の累計時間のタイマーです。
	Stopwatch gameTimer;

	//int32 spawnPlayerCount = 0;
	std::vector<int32> spawnPlayerTimes = { 0,10,20,30,40,50 };
	//int32 spawnPlayerCountInterval = 0;

	//int32 spawnEnemyCount = 0;
	std::vector<int32> spawnEnemyTimes = { 0,10,20,30,40,50 };


	int32 score = 0;










};


