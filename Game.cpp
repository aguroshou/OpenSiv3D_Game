﻿# include "Game.hpp"

Game::Game(const InitData& init)
	: IScene{ init }
{
	// 背景色を設定する
	Scene::SetBackground(ColorF{ 0.4, 0.7, 1.0 });

	world.setGravity(0.0);

	// 上下左右の壁
	walls << world.createRect(P2Static, Vec2{ 640, -20 }, SizeF{ 1480, 200 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });
	walls << world.createRect(P2Static, Vec2{ -100, 360 }, SizeF{ 200, 920 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });
	walls << world.createRect(P2Static, Vec2{ 640, 820 }, SizeF{ 1480, 200 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });
	walls << world.createRect(P2Static, Vec2{ 1380, 360 }, SizeF{ 200, 920 }, P2Material{ .density = 20.0, .restitution = 0.0, .friction = 0.0 });

	//for (int32 index = 0; index < 5; ++index)
	//{
	//	enemyAnimals << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(enemyAnimals), enemyAnimalRadius,
	//		P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
	//}

	//for (int32 index = 0; index < 5; ++index)
	//{
	//	playerAnimals << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(enemyAnimals), playerAnimalRadius,
	//		P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
	//}

	//for (int32 index = 0; index < 5; ++index)
	//{
	//	items << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(playerAnimals), itemRadius,
	//		P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
	//}

	oneSecondScoreTimer.start();
	gameTimer.start();
	spawnPlayerTimer.start();
	spawnEnemyTimer.start();
	spawnObjectTimer.start();

	gameBgmAudio.playOneShot(0.1);

}

void Game::update()
{

	// 配列による時間指定の方法は保留とします。
	//for (int32 spawnPlayerTime : spawnPlayerTimes)
	//{
	//	if (spawnPlayerTime <= gameTimer.s())
	//	{
	//		spawnPlayerTimes.erase(std::remove(spawnPlayerTimes.begin(), spawnPlayerTimes.end(), spawnPlayerTime), spawnPlayerTimes.end());
	//		playerAnimals << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(enemyAnimals), playerAnimalRadius,
	//		P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });

	//		items << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(playerAnimals), itemRadius,
	//		P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });

	//		// FIXME: ↓あとで修正する必要があります…。
	//		playerAnimalIDs.push_back(playerAnimals[playerAnimals.size() - 1].id());
	//	}
	//}

	//for (int32 spawnEnemyTime : spawnEnemyTimes)
	//{
	//	if (spawnEnemyTime <= gameTimer.s())
	//	{
	//		spawnEnemyTimes.erase(std::remove(spawnEnemyTimes.begin(), spawnEnemyTimes.end(), spawnEnemyTime), spawnEnemyTimes.end());
	//		enemyAnimals << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(playerAnimals), enemyAnimalRadius,
	//		P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
	//		// FIXME: ↓あとで修正する必要があります…。
	//		enemyAnimalIDs.push_back(enemyAnimals[enemyAnimals.size() - 1].id());
	//	}
	//}


	// 時間ごとにモグラを出現させるプログラムは、保留とします。
	//if (spawnPlayerIntervalTimeMax <= spawnPlayerTimer.sF())
	if (playerAnimals.size() < level)
	{
		//spawnPlayerIntervalTimeMax *= (1 - spawnPlayerIntervalAcceleration);
		//spawnPlayerTimer.restart();
		playerAnimals << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(enemyAnimals), playerAnimalRadius,
		P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });

		items << world.createCircle(P2Static, GetRandomPositionWithSafety(playerAnimals), itemRadius,
		P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
	}

	if (spawnEnemyIntervalTimeMax <= spawnEnemyTimer.sF())
	{
		spawnEnemyIntervalTimeMax *= (1 - spawnEnemyIntervalAcceleration);
		spawnEnemyTimer.restart();

		enemyAnimals << world.createCircle(P2Dynamic, GetRandomPositionWithSafety(playerAnimals), enemyAnimalRadius,
		P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
	}

	if (spawnObjectIntervalTimeMax <= spawnObjectTimer.sF())
	{
		spawnObjectIntervalTimeMax *= (1 - spawnObjectIntervalAcceleration);
		spawnObjectTimer.restart();

		objects << world.createRect(P2Static, Vec2(Random(13) * 80 + 80 + 40, Random(5) * 80 + 40 + 80 + 80), Vec2(80,80),
		P2Material{ .density = 40.0, .restitution = 0.0, .friction = 0.0 });
	}

	if (MouseL.down())
	{
		double minAnimalToCursorDistance = 100000;
		for (int32 index = 0; index < playerAnimals.size(); index++)
		{
			double animalToCursorDistance = playerAnimals[index].getPos().distanceFrom(Cursor::PosF());
			if (animalToCursorDistance < 70 && animalToCursorDistance < minAnimalToCursorDistance)
			{
				minAnimalToCursorDistance = animalToCursorDistance;
				grabAnimalIndex = index;

				isGrabbing = true;
				clickStartPosition = Cursor::PosF();
				dragAudio.playOneShot(0.1);
			}
		}
	}

	if (MouseL.up() && isGrabbing)
	{
		isGrabbing = false;
		Vec2 moveVector = Cursor::PosF() - clickStartPosition;
		if (moveVector.length() >= 10)
		{
			moveVector.normalize();
			dropMoveAudio.playOneShot(0.2);
		}
		else
		{
			moveVector.set(0, 0);
			dropWaitAudio.playOneShot(1.0);
		}
		playerAnimals[grabAnimalIndex].setVelocity(moveVector * 50);
	}

	// プレイヤーの動物をドラッグアンドドロップできるときに、マウスカーソルを手のアイコンにする
	if (!isGrabbing)
	{
		for (const auto& playerAnimal : playerAnimals)
		{
			double animalToCursorDistance = playerAnimal.getPos().distanceFrom(Cursor::PosF());
			if (animalToCursorDistance < 70)
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
	if (enemyAnimals.size() >= 1)
	{
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
			enemyHitAudio.playOneShot(0.1);
			playerHitPoint--;
			if (playerHitPoint == 0)
			{
				// ランキング画面へ
				changeScene(State::Ranking);
				getData().lastGameScore = score;
			}
			else
			{
				if (playerHitPoint == 1)
				{
					playerHitPoint1Audio.playOneShot(0.2);
				}

				// FIXME: ITEMのときと同じ処理なので、関数にまとめたほうが良さそうです。
				if (aType == BodyType::ENEMY)
				{
					P2Body* pTmpBody = pA;
					pA = pB;
					pB = pTmpBody;
				}

				pA->release();
				pB->release();

				// FIXME: IndexとIteratorの両方を必要としているため、冗長なプログラムとなってしまっています。
				// Array<P2Body>のIndex←→Iteratorの値を取得する関数は無いようなので、作成する必要があるかもしれません。
				int playerAnimalIndex = 0;
				for (auto playerAnimalIt = playerAnimals.begin(); playerAnimalIt != playerAnimals.end();)
				{
					if (const_cast<P2Body*>(&(*playerAnimalIt)) == const_cast<P2Body*>(&(*pA)))
					{
						// ドラッグ中のプレイヤーが削除される場合は、ドラッグを中止します。
						//if (const_cast<P2Body*>(&(*playerAnimalIt)) == const_cast<P2Body*>(&playerAnimals[grabAnimalIndex]))
						if (playerAnimalIndex == grabAnimalIndex)
						{
							isGrabbing = false;
							grabAnimalIndex = 0;
						}
						// ドラッグ中のプレイヤーの番号よりも、削除されるプレイヤーの番号が小さい場合は、配列の番号の変更(-1)に対応します。
						else if (playerAnimalIndex < grabAnimalIndex)
						{
							grabAnimalIndex--;
						}

						// 現在のイテレータが指す要素を削除し、イテレータを進める
						playerAnimals.erase(playerAnimalIt);
						pA = NULL;
						break;
					}
					else
					{
						++playerAnimalIt;
					}
					playerAnimalIndex++;
				}

				for (auto enemyIt = enemyAnimals.begin(); enemyIt != enemyAnimals.end();)
				{
					if (const_cast<P2Body*>(&(*enemyIt)) == const_cast<P2Body*>(&(*pB)))
					{
						// 現在のイテレータが指す要素を削除し、イテレータを進める
						enemyAnimals.erase(enemyIt);
						pB = NULL;
						break;
					}
					else
					{
						++enemyIt;
					}
				}
			}

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

			pA->release();
			pB->release();

			// FIXME: IndexとIteratorの両方を必要としているため、冗長なプログラムとなってしまっています。
			// Array<P2Body>のIndex←→Iteratorの値を取得する関数は無いようなので、作成する必要があるかもしれません。
			int playerAnimalIndex = 0;
			for (auto playerAnimalIt = playerAnimals.begin(); playerAnimalIt != playerAnimals.end();)
			{
				if (const_cast<P2Body*>(&(*playerAnimalIt)) == const_cast<P2Body*>(&(*pA)))
				{
					// ドラッグ中のプレイヤーが削除される場合は、ドラッグを中止します。
					//if (const_cast<P2Body*>(&(*playerAnimalIt)) == const_cast<P2Body*>(&playerAnimals[grabAnimalIndex]))
					if (playerAnimalIndex == grabAnimalIndex)
					{
						isGrabbing = false;
						grabAnimalIndex = 0;
					}
					// ドラッグ中のプレイヤーの番号よりも、削除されるプレイヤーの番号が小さい場合は、配列の番号の変更(-1)に対応します。
					else if (playerAnimalIndex < grabAnimalIndex)
					{
						grabAnimalIndex--;
					}

					// 現在のイテレータが指す要素を削除し、イテレータを進める
					playerAnimals.erase(playerAnimalIt);
					pA = NULL;
					break;
				}
				else
				{
					++playerAnimalIt;
				}
				playerAnimalIndex++;
			}

			for (auto itemIt = items.begin(); itemIt != items.end();)
			{
				if (const_cast<P2Body*>(&(*itemIt)) == const_cast<P2Body*>(&(*pB)))
				{
					// 現在のイテレータが指す要素を削除し、イテレータを進める
					items.erase(itemIt);
					pB = NULL;
					break;
				}
				else
				{
					++itemIt;
				}
			}
			goalAudio.playOneShot(0.5);
			score += 100;

			experiencePoint++;
			if (experiencePoint >= level)
			{
				level++;
				experiencePoint = 0;
			}

		}
	}

	if (oneSecondScoreTimer.sF() >= 1.0)
	{
		score++;
		oneSecondScoreTimer.restart();
	}

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
	textureGame.resized(1280).draw(0, 0);
	textureGameMenu.resized(2300).draw(0, -13);

	for (const auto& playerAnimal : playerAnimals)
	{
		playerAnimal.draw(Color{ 165, 105, 30 })
			.drawFrame(1, Color{ 0, 0, 0 });

		double rotation = Math::Atan2(playerAnimal.getVelocity().x, -playerAnimal.getVelocity().y);
		if (rotation < 0)
		{
			rotation += 360_deg;
		}
		if (playerAnimal.getVelocity() == Vec2(0, 0))
		{
			rotation = 0;
		}

		textureMole.scaled(0.7)
			.rotatedAt(textureMole.size() * 0.7 / 2, rotation)
			.draw(playerAnimal.getPos() - textureMole.size() * 0.7 / 2);
	}
	for (const auto& item : items)
	{
		item.draw(Color{ 144, 190, 14 })
			.drawFrame(1, Color{ 0, 0, 0 });
		textureHole.scaled(0.15).draw(item.getPos() - textureHole.size() * 0.15 / 2);
	}
	for (const auto& enemyAnimal : enemyAnimals)
	{
		double rotation = Math::Atan2(enemyAnimal.getVelocity().x, -enemyAnimal.getVelocity().y);
		if (rotation < 0)
		{
			rotation += 360_deg;
		}
		if (enemyAnimal.getVelocity() == Vec2(0, 0))
		{
			rotation = 0;
		}

		enemyAnimal.draw(Color{ 230, 230, 230 })
			.drawFrame(1, Color{ 0, 0, 0 });
		textureHammer.scaled(0.09)
			.rotatedAt(textureHammer.size() * 0.09 / 2, rotation)
			.draw(enemyAnimal.getPos() - textureHammer.size() * 0.09 / 2);
	}

	for (const auto& object : objects)
	{
		object.draw(Color{ 144, 190, 14 })
			.drawFrame(1, Color{ 0, 0, 0 });
		textureRock.scaled(0.105).draw(object.getPos() - textureRock.size() * 0.105 / 2);
	}

	//for (const auto& wall : walls)
	//{
	//	wall.draw(ColorF{ 0.0, 0.0, 0.0 });
	//}

	if (isGrabbing)
	{
		Vec2 moveVector = Cursor::PosF() - clickStartPosition;
		if (moveVector.length() >= 10)
		{
			moveVector.normalize();
			Line{ playerAnimals[grabAnimalIndex].getPos() + moveVector * 20, (playerAnimals[grabAnimalIndex].getPos() + moveVector * 70) }
				.stretched(-10)
				.drawArrow(10, { 20, 20 }, ColorF{ 1.0, 0.0, 0.0, 0.8 });
		}
	}

	font(U"もぐら：{}"_fmt(playerHitPoint)).draw(50, 5, ColorF{ 0.3, 0.25, 0.2 });
	font(U"レベル：{}"_fmt(level)).draw(450, 5, ColorF{ 0.3, 0.25, 0.2 });
	font(U"スコア：{}"_fmt(score)).draw(850, 5, ColorF{ 0.3, 0.25, 0.2 });
	//double tmpPrecisionValue = Precision(spawnPlayerIntervalTimeMax - spawnPlayerTimer.sF(), 1);
	double tmpPrecisionValue = std::round((spawnPlayerIntervalTimeMax - spawnPlayerTimer.sF()) * 10) / 10.0; // 小数点以下1桁までに制限
	//font(U"PLAYER:{}"_fmt(tmpPrecisionValue)).draw(500, 5, ColorF{ 1.0, 0.5, 0.0 });
	tmpPrecisionValue = std::round((spawnEnemyIntervalTimeMax - spawnEnemyTimer.sF()) * 10) / 10.0; // 小数点以下1桁までに制限

	//font(U"ENEMY:{}"_fmt(tmpPrecisionValue)).draw(1000, 5, ColorF{ 1.0, 0.5, 0.0 });


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
	double radius = 150;
	int32 retryCount = 0;
	while (!isSafe) {
		isSafe = true;
		randomPosition = RandomVec2(1240, 600) + Vec2(20, 100);
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
