# include <Siv3D.hpp>

void Main()
{
	// ウィンドウを 1280x720 にリサイズする
	Window::Resize(1280, 720);

	// 背景色を設定する
	Scene::SetBackground(ColorF{ 0.4, 0.7, 1.0 });

	// 2D 物理演算のシミュレーションステップ（秒）
	constexpr double StepSec = (1.0 / 200.0);

	// 2D 物理演算のシミュレーション蓄積時間（秒）
	double accumulatorSec = 0.0;

	// 2D 物理演算のワールド
	P2World world;

	world.setGravity(0.0);

	// 上下左右の壁
	Array<P2Body>  walls;
	walls << world.createRect(P2Static, Vec2{ 640, -20 }, SizeF{ 1480, 200 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });
	walls << world.createRect(P2Static, Vec2{ -100, 360 }, SizeF{ 200, 920 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });
	walls << world.createRect(P2Static, Vec2{ 640, 820 }, SizeF{ 1480, 200 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });
	walls << world.createRect(P2Static, Vec2{ 1380, 360 }, SizeF{ 200, 920 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });

	constexpr double playerAnimalRadius = 20;

	Array<P2Body> playerAnimals;
	HashSet<P2BodyID> playerAnimalIDs;
	for (int32 index = 0; index < 5; ++index)
	{
		playerAnimals << world.createCircle(P2Dynamic, Vec2{ 100 + index * 50, 100 }, playerAnimalRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
		playerAnimalIDs.emplace(playerAnimals[index].id());
	}
	bool isPlayerAnimalGrab = false;

	int32 grabAnimalIndex = 0;
	bool isGrabbing = false;

	constexpr double enemyAnimalRadius = 20;
	Array<P2Body> enemyAnimals;
	HashSet<P2BodyID> enemyAnimalIDs;
	for (int32 index = 0; index < 5; ++index)
	{
		enemyAnimals << world.createCircle(P2Dynamic, Vec2{ 300 + index * 50, 500 }, enemyAnimalRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
		enemyAnimalIDs.emplace(enemyAnimals[index].id());
	}


	// 基本サイズ 50 のフォントを作成
	const Font font{ 50 };

	// 詳しい仕組みを理解できていませんが、カメラ座標を(640, 360)とすることで、Cursor::PosF()とplayerAnimals[0].getPos()の座標が一致するようです。
	// そのため、カメラの座標・ズームなどを変更してしまうと不具合が発生してしまいます。
	Camera2D camera{ Vec2{ 640, 360 }, 1.0, CameraControl::None_ };
	Array<P2Body> bodies;

	// タイマーを初期化
	Stopwatch timer;
	timer.start();
	int32 score = 0;

	while (System::Update())
	{
		if (MouseL.down())
		{
			double minAnimalToCursorDistance = 100000;
			for (int index = 0; index < playerAnimals.size(); index++)
			{
				double animalToCursorDistance = playerAnimals[index].getPos().distanceFrom(Cursor::PosF());
				if (animalToCursorDistance < 100 && animalToCursorDistance < minAnimalToCursorDistance)
				{
					minAnimalToCursorDistance = animalToCursorDistance;
					grabAnimalIndex = index;

					isGrabbing = true;
				}
			}
		}

		if (MouseL.up() && isGrabbing)
		{
			isGrabbing = false;
			Vec2 moveVector = Cursor::PosF() - playerAnimals[grabAnimalIndex].getPos();
			playerAnimals[grabAnimalIndex].setVelocity(moveVector.normalized() * 50);
		}

		// プレイヤーの動物をドラッグアンドドロップできるときに、マウスカーソルを手のアイコンにする
		if (!isGrabbing)
		{
			for (int index = 0; index < playerAnimals.size(); index++)
			{
				double animalToCursorDistance = playerAnimals[index].getPos().distanceFrom(Cursor::PosF());
				if (animalToCursorDistance < 100)
				{
					Cursor::RequestStyle(CursorStyle::Hand);
				}
			}
		}

		//for (const auto& playerAnimal : playerAnimals)
		//{
		//	for (const auto& enemyAnimal : enemyAnimals)
		//	{
		//		playerAnimal.
		//	}
		//}
		// ボールと接触しているボディの ID を格納
		for (auto&& [pair, collision] : world.getCollisions())
		{
			if (playerAnimalIDs.contains(pair.a) && enemyAnimalIDs.contains(pair.b))
			{
				Print << pair.a << U" vs " << pair.b;
			}
			else if (playerAnimalIDs.contains(pair.b) && enemyAnimalIDs.contains(pair.a))
			{
				Print << pair.a << U" vs " << pair.b;
			}

			//if (pair.a == ballID)
			//{
			//	collidedIDs.emplace(pair.b);
			//}
			//else if (pair.b == ballID)
			//{
			//	collidedIDs.emplace(pair.a);
			//}
		}

		if (timer.sF() >= 1.0)
		{
			score++;
			timer.restart();
		}

		////////////////////////////////
		//
		//	状態更新
		//
		////////////////////////////////

		for (accumulatorSec += Scene::DeltaTime(); StepSec <= accumulatorSec; accumulatorSec -= StepSec)
		{
			// 2D 物理演算のワールドを更新する
			world.update(StepSec);
		}

		// 2D カメラを更新する
		camera.update();

		////////////////////////////////
		//
		//	描画
		//
		////////////////////////////////

		for (const auto& playerAnimal : playerAnimals)
		{
			playerAnimal.draw(ColorF{ 1, 1, 1 })
				.drawFrame(2); // 輪郭
		}
		for (const auto& enemyAnimal : enemyAnimals)
		{
			enemyAnimal.draw(ColorF{ 0, 0, 0 })
				.drawFrame(2); // 輪郭
		}

		for (const auto& wall : walls)
		{
			wall.draw(ColorF{ 0.0, 0.0, 0.0 });
		}

		// 左上位置 (20, 20) からテキストを描く
		font(U"LIFE:{}/SCORE:{}"_fmt(0, score)).draw(50, 5, ColorF{ 1.0, 0.5, 0.0 });
	}
}
