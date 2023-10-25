# include "Game.hpp"



Game::Game(const InitData& init)
	: IScene{ init }
{
	//// 横 (Scene::Width() / blockSize.x) 個、縦 5 個のブロックを配列に追加する
	//for (auto p : step(Size{ (Scene::Width() / BrickSize.x), 5 }))
	//{
	//	m_bricks << Rect{ (p.x * BrickSize.x), (60 + p.y * BrickSize.y), BrickSize };
	//}


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
		playerAnimalIDs.push_back(playerAnimals[index].id());
	}

	for (int32 index = 0; index < 5; ++index)
	{
		items << world.createCircle(P2Dynamic, Vec2{ 500 + index * 50, 600 }, itemRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
		itemIDs.push_back(items[index].id());
	}	

	for (int32 index = 0; index < 5; ++index)
	{
		enemyAnimals << world.createCircle(P2Dynamic, Vec2{ 300 + index * 50, 500 }, enemyAnimalRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
		enemyAnimalIDs.push_back(enemyAnimals[index].id());
	}

	oneSecondScoreTimer.start();
	gameTimer.start();

}

void Game::update()
{
	for (int32 spawnPlayerTime : spawnPlayerTimes)
	{
		if (spawnPlayerTime <= gameTimer.s())
		{
			spawnPlayerTimes.erase(std::remove(spawnPlayerTimes.begin(), spawnPlayerTimes.end(), spawnPlayerTime), spawnPlayerTimes.end());
			playerAnimals << world.createCircle(P2Dynamic, Vec2{ 100, 100 }, playerAnimalRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
			// FIXME: ↓あとで修正する必要があります…。
			playerAnimalIDs.push_back(playerAnimals[playerAnimals.size() - 1].id());
		}
	}

	for (int32 spawnEnemyTime : spawnEnemyTimes)
	{
		if (spawnEnemyTime <= gameTimer.s())
		{
			spawnEnemyTimes.erase(std::remove(spawnEnemyTimes.begin(), spawnEnemyTimes.end(), spawnEnemyTime), spawnEnemyTimes.end());
			enemyAnimals << world.createCircle(P2Dynamic, Vec2{ 300, 500 }, enemyAnimalRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
			// FIXME: ↓あとで修正する必要があります…。
			enemyAnimalIDs.push_back(enemyAnimals[enemyAnimals.size() - 1].id());
		}
	}


	if (MouseL.down())
	{
		double minAnimalToCursorDistance = 100000;
		for (int32 index = 0; index < playerAnimals.size(); index++)
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
		for (const auto& playerAnimal : playerAnimals)
		{
			double animalToCursorDistance = playerAnimal.getPos().distanceFrom(Cursor::PosF());
			if (animalToCursorDistance < 100)
			{
				Cursor::RequestStyle(CursorStyle::Hand);
			}
		}
	}

	// 敵の動物から一番近いプレイヤーの動物の方向へ近づけます。
	//for (int32 enemyIndex = 0; enemyIndex < enemyAnimals.size(); enemyIndex++)
	//{
	//	int32 minDistancePlayerIndex = 0;
	//	double minEnemyToPlayerDistance = 100000;
	//	for (int32 playerIndex = 0; playerIndex < playerAnimals.size(); playerIndex++)
	//	{
	//		double enemyToPlayerDistance = enemyAnimals[enemyIndex].getPos().distanceFrom(playerAnimals[playerIndex].getPos());
	//		if (enemyToPlayerDistance < minEnemyToPlayerDistance)
	//		{
	//			minEnemyToPlayerDistance = enemyToPlayerDistance;
	//			minDistancePlayerIndex = playerIndex;
	//		}
	//	}
	//	const Vec2 direction = playerAnimals[minDistancePlayerIndex].getPos() - enemyAnimals[enemyIndex].getPos();		
	//	const Vec2 speed = 10 * direction.normalized();
	//	enemyAnimals[enemyIndex].setVelocity(speed);
	//}

	// プレイヤーから一番近い敵を近づけます。追う・追われる対象が1対1の関係となります。
	std::vector<bool> isTargetDecideds(enemyAnimals.size(), false);
	for (int32 playerIndex = 0; playerIndex < playerAnimals.size(); playerIndex++)
	{
		int32 minDistanceEnemyIndex = 0;
		double minEnemyToPlayerDistance = 100000;
		for (int32 enemyIndex = 0; enemyIndex < enemyAnimals.size(); enemyIndex++)
		{
			if (isTargetDecideds[enemyIndex])
			{
				continue;
			}
			double enemyToPlayerDistance = enemyAnimals[enemyIndex].getPos().distanceFrom(playerAnimals[playerIndex].getPos());
			if (enemyToPlayerDistance < minEnemyToPlayerDistance)
			{
				minEnemyToPlayerDistance = enemyToPlayerDistance;
				minDistanceEnemyIndex = enemyIndex;
				//Print << minDistanceEnemyIndex;
			}
		}
		isTargetDecideds[minDistanceEnemyIndex] = true;

		const Vec2 direction = playerAnimals[playerIndex].getPos() - enemyAnimals[minDistanceEnemyIndex].getPos();
		const Vec2 speed = 10 * direction.normalized();
		enemyAnimals[minDistanceEnemyIndex].setVelocity(speed);
	}

	// ボールと接触しているボディの ID を格納
	for (auto&& [pair, collision] : world.getCollisions())
	{
		// FIXME: ほぼ同じ2つの処理を関数にまとめる
		if (playerAnimalIDs.contains(pair.a) && enemyAnimalIDs.contains(pair.b))
		{
			//Print << pair.a << U" vs " << pair.b;
			// ランキング画面へ
			changeScene(State::Ranking);
			getData().lastGameScore = score;
		}
		else if (playerAnimalIDs.contains(pair.b) && enemyAnimalIDs.contains(pair.a))
		{
			//Print << pair.a << U" vs " << pair.b;
			// ランキング画面へ
			changeScene(State::Ranking);
			getData().lastGameScore = score;
		}
		else if (enemyAnimalIDs.contains(pair.a) && enemyAnimalIDs.contains(pair.b))
		{
			//Print << pair.a << U" vs " << pair.b;
			// FIXME: プログラムの整理
			//int32 aIndex = 0, bIndex = 0;
			//for (int32 i = 0; i < enemyAnimalIDs.size(); i++)
			//{
			//	if (enemyAnimalIDs[i] == pair.a)
			//	{
			//		aIndex = i;
			//	}
			//	if (enemyAnimalIDs[i] == pair.b)
			//	{
			//		bIndex = i;
			//	}
			//}
		}
		if ((playerAnimalIDs.contains(pair.a) && itemIDs.contains(pair.b)) ||
			(playerAnimalIDs.contains(pair.b) && itemIDs.contains(pair.a)))
		{
			P2BodyID playerID = pair.a;
			P2BodyID itemID = pair.b;
			if (playerAnimalIDs.contains(pair.b))
			{
				P2BodyID playerID = pair.b;
				P2BodyID itemID = pair.a;
			}

			int32 playerIndex = 0, itemIndex = 0;
			for (int32 i = 0; i < playerAnimalIDs.size(); i++)
			{
				if (playerAnimalIDs[i] == playerID)
				{
					playerIndex = i;
				}
			}
			playerAnimals.remove_at(playerIndex);
			playerAnimalIDs.remove_at(playerIndex);
			Print << playerIndex;

			for (int32 i = 0; i < itemIDs.size(); i++)
			{
				if (itemIDs[i] == itemID)
				{
					itemIndex = i;
				}
			}
			items.remove_at(itemIndex);
			itemIDs.remove_at(itemIndex);

			Print << itemIndex;

			score += 100;
		}
	}

	if (oneSecondScoreTimer.sF() >= 1.0)
	{
		score++;
		oneSecondScoreTimer.restart();
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
	for (const auto& item : items)
	{
		item.draw(ColorF{ 0, 1, 0 })
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

	font(U"SCORE:{}"_fmt(score)).draw(50, 5, ColorF{ 1.0, 0.5, 0.0 });
}

P2Body* Game::findPlayerFromID(P2BodyID id)
{
	for (Array<P2Body>::iterator playerAnimalIt = playerAnimals.begin(); playerAnimalIt < playerAnimals.end(); playerAnimalIt++)
	{
		if (playerAnimalIt->id() == id)
		{
			//  &(*playerAnimalIt); の意味をChatGPTへ質問したときのメモです。
			//	& (*playerAnimalIt)は、C++においてポインタの取得と逆参照を組み合わせて元のオブジェクトを取得する操作です。
			//	* playerAnimalIt - playerAnimalItが指すイテレータの位置にあるオブジェクトを逆参照して取得します。この操作により、playerAnimalItが指すP2Bodyオブジェクトにアクセスできます。
			//	& (*playerAnimalIt) - 1で取得したオブジェクトへのポインタを取得します。つまり、元のオブジェクトへのポインタが得られます。
			//	この操作は、イテレータからオブジェクトへのポインタを取得するために必要です。なぜなら、playerAnimalItはイテレータであり、元のオブジェクト自体ではないからです。そのため、元のオブジェクトにアクセスするには、イテレータを逆参照してオブジェクトを取得し、それに対してポインタを取得する必要があります。
			return &(*playerAnimalIt);
		}
	}
	return &(*playerAnimals.end());
}

P2Body* Game::findEnemyFromID(P2BodyID id)
{
	for (Array<P2Body>::iterator enemyAnimalIt = enemyAnimals.begin(); enemyAnimalIt < enemyAnimals.end(); enemyAnimalIt++)
	{
		if (enemyAnimalIt->id() == id)
		{
			return &(*enemyAnimalIt);
		}
	}
	return &(*enemyAnimals.end());
}

P2Body* Game::findItemFromID(P2BodyID id)
{
	for (Array<P2Body>::iterator itemIt = items.begin(); itemIt < items.end(); itemIt++)
	{
		if (itemIt->id() == id)
		{
			return &(*itemIt);
		}
	}
	return &(*items.end());
}
