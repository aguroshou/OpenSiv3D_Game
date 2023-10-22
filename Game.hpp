# pragma once
# include "Common.hpp"

// ゲームシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;

private:

	// ブロックのサイズ
	static constexpr Size BrickSize{ 40, 20 };

	// ボールの速さ
	static constexpr double Speed = 480.0;

	// ボールの速度
	Vec2 m_ballVelocity{ 0, -Speed };

	// ボール
	Circle m_ball{ 400, 400, 8 };

	// ブロックの配列
	Array<Rect> m_bricks;

	// 現在のゲームのスコア
	int32 m_score = 0;

	Rect getPaddle() const;


	// 追加

	// 2D 物理演算のシミュレーションステップ（秒）
	const double StepSec = (1.0 / 200.0);
	// 2D 物理演算のシミュレーション蓄積時間（秒）
	double accumulatorSec = 0.0;

	// 2D 物理演算のワールド
	P2World world;
	Array<P2Body>  walls;
	const double playerAnimalRadius = 20;

	Array<P2Body> playerAnimals;
	HashSet<P2BodyID> playerAnimalIDs;


	bool isPlayerAnimalGrab = false;

	int32 grabAnimalIndex = 0;
	bool isGrabbing = false;

	const double enemyAnimalRadius = 20;
	Array<P2Body> enemyAnimals;
	HashSet<P2BodyID> enemyAnimalIDs;

	// 基本サイズ 50 のフォントを作成
	const Font font{ 50 };

	// 詳しい仕組みを理解できていませんが、カメラ座標を(640, 360)とすることで、Cursor::PosF()とplayerAnimals[0].getPos()の座標が一致するようです。
	// そのため、カメラの座標・ズームなどを変更してしまうと不具合が発生してしまいます。
	Camera2D camera{ Vec2{ 640, 360 }, 1.0, CameraControl::None_ };
	Array<P2Body> bodies;

	// タイマーを初期化
	Stopwatch timer;

	int32 score = 0;










};


