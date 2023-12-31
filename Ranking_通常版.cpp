﻿# include "Ranking.hpp"

/// @brief 有効なレコードかどうかをチェックします。
/// @param value レコードが格納された JSON
/// @return 有効なレコードなら true, そうでなければ false
bool Ranking::IsValidRecord(const JSON& value)
{
	return (value.isObject()
		&& value.hasElement(U"username")
		&& value.hasElement(U"score")
		&& value[U"username"].isString()
		&& value[U"score"].isNumber());
}

/// @brief JSON データをリーダーボードとして読み込みます。
/// @param json JSON データ
/// @param dst 更新するリーダーボード
/// @remark 読み込みに失敗した場合、dst は更新されません。
/// @return 読み込みに成功したら true, 失敗したら false
bool Ranking::ReadLeaderboard(const JSON& json, Array<Record>& dst)
{
	if (not json.isArray())
	{
		return false;
	}

	Array<Record> leaderboard;

	for (auto&& [key, value] : json)
	{
		if (not IsValidRecord(value))
		{
			continue;
		}

		Record record;
		record.userName = value[U"username"].get<String>();
		record.score = value[U"score"].get<double>();

		if (value.contains(U"data"))
		{
			record.data = value[U"data"];
		}

		leaderboard << std::move(record);
	}

	dst = std::move(leaderboard);
	return true;
}

/// @brief サーバからリーダーボードを取得するタスクを作成します。
/// @param url サーバの URL
/// @param count 取得上限数
/// @return タスク
AsyncHTTPTask Ranking::CreateGetTask(const URLView url, int32 count)
{
	// GET リクエストの URL を作成する
	const URL requestURL = U"{}?count={}"_fmt(url, count);

	return SimpleHTTP::GetAsync(requestURL, {});
}

/// @brief サーバにスコアを送信するタスクを作成します。
/// @param url サーバの URL
/// @param userName ユーザー名
/// @param score スコア
/// @param additionalData 追加の情報
/// @return タスク
AsyncHTTPTask Ranking::CreatePostTask(const URLView url, const StringView userName, double score, JSON additionalData)
{
	// POST リクエストの URL を作成する
	URL requestURL = U"{}?username={}&score={}"_fmt(url, PercentEncode(userName), PercentEncode(Format(score)));

	if (additionalData)
	{
		requestURL += (U"&data=" + PercentEncode(additionalData.formatMinimum()));
	}

	const HashTable<String, String> headers =
	{
		{ U"Content-Type", U"application/x-www-form-urlencoded; charset=UTF-8" }
	};

	return SimpleHTTP::PostAsync(requestURL, headers, nullptr, 0);
}

Ranking::Ranking(const InitData& init)
	: IScene{ init }
{
	auto& data = getData();

	if (data.lastGameScore)
	{
		score = *data.lastGameScore;

		const int32 lastScore = *data.lastGameScore;


		// ランキングを再構成
		data.highScores << lastScore;
		data.highScores.rsort();
		data.highScores.resize(RankingCount);

		// ランクインしていたら m_rank に順位をセット
		for (int32 i = 0; i < RankingCount; ++i)
		{
			if (data.highScores[i] == lastScore)
			{
				m_rank = i;
				break;
			}
		}

		data.lastGameScore.reset();
	}

	m_pSubscribeRichButton = new RichButton(U"✉️"_emoji);
	m_pBackRichButton = new RichButton(U"↩️"_emoji);

	// リーダーボードを取得するタスク
	leaderboardGetTask = CreateGetTask(LeaderboardURL);

	userNameTextBox.text = U"ななし";

	rankingAudio.playOneShot(0.3);
}

Ranking::~Ranking()
{
	delete m_pSubscribeRichButton;
	delete m_pBackRichButton;
}

void Ranking::update()
{
	rankingTexture.resized(1280).draw(0, 0);

	font(U"なまえ").draw(20, Vec2{ 150, 300 }, ColorF{ 0.11 });
	font(U"スコア <{}>"_fmt(score)).draw(20, Vec2{ 150, 250 }, ColorF{ 0.11 });
	
	SimpleGUI::TextBox(userNameTextBox, Vec2{ 220, 297 }, 220, 10, !isScorePosted);

	// 通信が完了しているか
	const bool isReady = (not leaderboardGetTask) && (not scorePostTask);

	if (m_SubscribeRect.leftClicked() && isReady && (not isScorePosted))
	{
		clickAudio.playOneShot(0.3);
		scorePostTask = CreatePostTask(LeaderboardURL, userNameTextBox.text, score);
	}
	else if (m_BackRect.leftClicked())
	{
		clickAudio.playOneShot(0.3);
		// ランキングシーンへ
		changeScene(State::Title);
	}

	// スコア送信処理が完了したら
	if (scorePostTask && scorePostTask->isReady())
	{
		if (const auto response = scorePostTask->getResponse();
			response.isOK())
		{
			// スコアを送信済みにし、再送信できないようにする
			isScorePosted = true;

			// リーダーボードを更新する
			leaderboardGetTask = CreateGetTask(LeaderboardURL);
		}
		else
		{
			Print << U"Failed to submit the score.";
		}

		scorePostTask.reset();
	}

	// リーダーボード取得処理が完了したら
	if (leaderboardGetTask && leaderboardGetTask->isReady())
	{
		if (const auto response = leaderboardGetTask->getResponse();
			response.isOK())
		{
			ReadLeaderboard(leaderboardGetTask->getAsJSON(), leaderboard);
		}
		leaderboardGetTask.reset();
	}

	int32 rankingSize = std::min(static_cast<int32>(leaderboard.size()), 10);
	for (size_t i = 0; i < rankingSize; ++i)
	{
		const auto& record = leaderboard[i];

		font(U"{}"_fmt(record.userName)).draw(20, Vec2{ 820, (185 + i * 50) }, ColorF{ 0.11 });
		font(U"<{}>"_fmt(record.score)).draw(20, Vec2{ 1050, (185 + i * 50) }, ColorF{ 0.11 });
	}
}

void Ranking::draw() const
{
	Scene::SetBackground(ColorF{ 0, 0, 0 });

	m_pSubscribeRichButton->draw(m_SubscribeRect, font, U"とうろく");
	m_pBackRichButton->draw(m_BackRect, font, U"もどる");
}

