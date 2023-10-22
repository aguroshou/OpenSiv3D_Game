# include "Game.hpp"

Game::Game(const InitData& init)
	: IScene{ init }
{
	//// 横 (Scene::Width() / blockSize.x) 個、縦 5 個のブロックを配列に追加する
	//for (auto p : step(Size{ (Scene::Width() / BrickSize.x), 5 }))
	//{
	//	m_bricks << Rect{ (p.x * BrickSize.x), (60 + p.y * BrickSize.y), BrickSize };
	//}


	// 追加

	// 背景色を設定する
	Scene::SetBackground(ColorF{ 0.4, 0.7, 1.0 });

	world.setGravity(0.0);

	// 上下左右の壁
	walls << world.createRect(P2Static, Vec2{ 640, -20 }, SizeF{ 1480, 200 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });
	walls << world.createRect(P2Static, Vec2{ -100, 360 }, SizeF{ 200, 920 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });
	walls << world.createRect(P2Static, Vec2{ 640, 820 }, SizeF{ 1480, 200 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });
	walls << world.createRect(P2Static, Vec2{ 1380, 360 }, SizeF{ 200, 920 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });

	for (int32 index = 0; index < 5; ++index)
	{
		playerAnimals << world.createCircle(P2Dynamic, Vec2{ 100 + index * 50, 100 }, playerAnimalRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
		playerAnimalIDs.emplace(playerAnimals[index].id());
	}

	for (int32 index = 0; index < 5; ++index)
	{
		enemyAnimals << world.createCircle(P2Dynamic, Vec2{ 300 + index * 50, 500 }, enemyAnimalRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
		enemyAnimalIDs.emplace(enemyAnimals[index].id());
	}

	timer.start();





}

void Game::update()
{
	//// ボールを移動
	//m_ball.moveBy(m_ballVelocity * Scene::DeltaTime());

	//// ブロックを順にチェック
	//for (auto it = m_bricks.begin(); it != m_bricks.end(); ++it)
	//{
	//	// ブロックとボールが交差していたら
	//	if (it->intersects(m_ball))
	//	{
	//		// ボールの向きを反転する
	//		(it->bottom().intersects(m_ball) || it->top().intersects(m_ball)
	//			? m_ballVelocity.y : m_ballVelocity.x) *= -1;

	//		// ブロックを配列から削除（イテレータが無効になるので注意）
	//		m_bricks.erase(it);

	//		AudioAsset(U"Brick").playOneShot(0.5);

	//		// スコアを加算
	//		++m_score;

	//		// これ以上チェックしない
	//		break;
	//	}
	//}

	//// 天井にぶつかったらはね返る
	//if (m_ball.y < 0 && m_ballVelocity.y < 0)
	//{
	//	m_ballVelocity.y *= -1;
	//}

	//// 左右の壁にぶつかったらはね返る
	//if ((m_ball.x < 0 && m_ballVelocity.x < 0)
	//	|| (Scene::Width() < m_ball.x && 0 < m_ballVelocity.x))
	//{
	//	m_ballVelocity.x *= -1;
	//}

	//// パドルにあたったらはね返る
	//if (const Rect paddle = getPaddle();
	//	(0 < m_ballVelocity.y) && paddle.intersects(m_ball))
	//{
	//	// パドルの中心からの距離に応じてはね返る方向を変える
	//	m_ballVelocity = Vec2{ (m_ball.x - paddle.center().x) * 10, -m_ballVelocity.y }.setLength(Speed);
	//}

	//// 画面外に出るか、ブロックが無くなったら
	//if ((Scene::Height() < m_ball.y) || m_bricks.isEmpty())
	//{
	//	// ランキング画面へ
	//	changeScene(State::Ranking);

	//	getData().lastGameScore = m_score;
	//}


	// 追加


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


}

void Game::draw() const
{
	//Scene::SetBackground(ColorF{ 0.2 });

	//// すべてのブロックを描画する
	//for (const auto& brick : m_bricks)
	//{
	//	brick.stretched(-1).draw(HSV{ brick.y - 40 });
	//}

	//// ボールを描く
	//m_ball.draw();

	//// パドルを描く
	//getPaddle().draw();

	//FontAsset(U"GameScore")(m_score).draw(10, 10);


	// 追加

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

Rect Game::getPaddle() const
{
	return{ Arg::center(Cursor::Pos().x, 500), 60, 10 };
}
