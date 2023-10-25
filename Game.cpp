﻿# include "Game.hpp"



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
		enemyAnimals << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(enemyAnimals), enemyAnimalRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
		enemyAnimalIDs.push_back(enemyAnimals[index].id());
	}

	for (int32 index = 0; index < 5; ++index)
	{
		//Vec2 randomPosition = RandomVec2(1280, 720);
		//while (IsCloseToOtherBodies(randomPosition, enemyAnimals, 100)) {
		//	randomPosition = RandomVec2(1280, 720);
		//}
		playerAnimals << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(enemyAnimals), playerAnimalRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
		playerAnimalIDs.push_back(playerAnimals[index].id());
	}

	for (int32 index = 0; index < 5; ++index)
	{
		//Vec2 randomPosition = RandomVec2(1280, 720);
		//while (IsCloseToOtherBodies(randomPosition, playerAnimals, 100)) {
		//	randomPosition = RandomVec2(1280, 720);
		//}

		items << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(playerAnimals), itemRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
		itemIDs.push_back(items[index].id());
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
			playerAnimals << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(enemyAnimals), playerAnimalRadius,
			P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });

			items << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(playerAnimals), itemRadius,
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
			enemyAnimals << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(playerAnimals), enemyAnimalRadius,
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
	for (int32 enemyIndex = 0; enemyIndex < enemyAnimals.size(); enemyIndex++)
	{
		int32 minDistancePlayerIndex = 0;
		double minEnemyToPlayerDistance = 100000;
		for (int32 playerIndex = 0; playerIndex < playerAnimals.size(); playerIndex++)
		{
			double enemyToPlayerDistance = enemyAnimals[enemyIndex].getPos().distanceFrom(playerAnimals[playerIndex].getPos());
			if (enemyToPlayerDistance < minEnemyToPlayerDistance)
			{
				minEnemyToPlayerDistance = enemyToPlayerDistance;
				minDistancePlayerIndex = playerIndex;
			}
		}
		const Vec2 direction = playerAnimals[minDistancePlayerIndex].getPos() - enemyAnimals[enemyIndex].getPos();		
		const Vec2 speed = 10 * direction.normalized();
		enemyAnimals[enemyIndex].setVelocity(speed);
	}

	// 1体のプレイヤーに対して、最低1体は敵を近づけるようにします。
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
		P2Body* pA;
		BodyType aType = BodyType::DEFAULT;
		pA = findBodyAndSetBodyTypeFromID(pair.a, aType);

		P2Body* pB;
		BodyType bType = BodyType::DEFAULT;
		pB = findBodyAndSetBodyTypeFromID(pair.b, bType);

		if ((aType == BodyType::PLAYER && bType == BodyType::ENEMY) ||
			(aType == BodyType::ENEMY && bType == BodyType::PLAYER))
		{
			// ランキング画面へ
			changeScene(State::Ranking);
			getData().lastGameScore = score;
		}
		else if ((aType == BodyType::PLAYER && bType == BodyType::ITEM) ||
			(aType == BodyType::ITEM && bType == BodyType::PLAYER))
		{
			if (aType == BodyType::ITEM)
			{
				P2Body* pTmpBody = pA;
				pA = pB;
				pB = pTmpBody;
			}
			//FIXME: P2Bodyの削除は、release関数を呼ぶだけで良いのか分かっていません。
			pA->release();
			pB->release();
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

P2Body* Game::findBodyFromID(P2BodyID id, const Array<P2Body>& bodyList)
{
	for (Array<P2Body>::const_iterator it = bodyList.begin(); it < bodyList.end(); it++)
	{
		if (it->id() == id)
		{
			return const_cast<P2Body*>(&(*it));
		}
	}
	return NULL;
}

P2Body* Game::findBodyAndSetBodyTypeFromID(P2BodyID id, BodyType& type)
{
	P2Body* pA = findBodyFromID(id, playerAnimals);
	if (pA != NULL)
	{
		type = BodyType::PLAYER;
		return pA;
	}

	pA = findBodyFromID(id, enemyAnimals);
	if (pA != NULL)
	{
		type = BodyType::ENEMY;
		return pA;
	}

	pA = findBodyFromID(id, items);
	if (pA != NULL)
	{
		type = BodyType::ITEM;
		return pA;
	}

	type = BodyType::DEFAULT;
	return NULL;
}

bool Game::IsCloseToOtherBodies(const Vec2& position, const Array<P2Body>& otherBodies, const float radius) {
	for (const P2Body otherBody : otherBodies) {
		if (otherBody.getPos().distanceFrom(position) < radius) {
			return true; // 他の座標と重なる
		}
	}
	return false; // 他の座標と重ならない
}

Vec2 Game::GetRandomPositionWithSafety(const Array<P2Body>& otherBodies) {
	Vec2 randomPosition = Vec2(0, 0);
	bool isSafe = false;
	double radius = 300;
	int32 retryCount = 0;
	while (!isSafe) {
		isSafe = true;
		randomPosition = RandomVec2(1280, 720);
		for (const P2Body otherBody : otherBodies) {
			if (otherBody.getPos().distanceFrom(randomPosition) < radius) {
				isSafe = false; // 他の座標と重なる
				break;
			}
		}
		retryCount++;
		if (retryCount >= RETRY_COUNT_MAX)
		{
			break;
		}
	}
	return randomPosition;
}