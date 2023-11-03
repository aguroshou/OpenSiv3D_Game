# include "Ranking.hpp"
# include <emscripten.h>

namespace s3d::detail
{
	__attribute__((import_name("siv3dCreateXMLHTTPRequest")))
		extern int32 siv3dCreateXMLHTTPRequest();
	__attribute__((import_name("siv3dSetXMLHTTPRequestCallback")))
		extern void siv3dSetXMLHTTPRequestCallback(int32 id, void(*callback)(int32, void*), void* userData);
	__attribute__((import_name("siv3dSetXMLHTTPRequestErrorCallback")))
		extern void siv3dSetXMLHTTPRequestErrorCallback(int32 id, void(*callback)(int32, void*), void* userData);
	__attribute__((import_name("siv3dSetXMLHTTPRequestRequestHeader")))
		extern void siv3dSetXMLHTTPRequestRequestHeader(int32 id, const char* name, const char* value);
	__attribute__((import_name("siv3dGetXMLHTTPRequestResponseHeaders")))
		extern char* siv3dGetXMLHTTPRequestResponseHeaders(int32 id);
	__attribute__((import_name("siv3dSendXMLHTTPRequest")))
		extern void siv3dSendXMLHTTPRequest(int32 id, const char* data);
	__attribute__((import_name("siv3dOpenXMLHTTPRequest")))
		extern void siv3dOpenXMLHTTPRequest(int32 id, const char* method, const char* url);
}

/// @brief POST メソッドで Web サーバにリクエストを送ります。
/// @param url URL
/// @param headers ヘッダ
/// @param src 送信するデータの先頭ポインタ
/// @param size 送信するデータのサイズ（バイト）
/// @return レスポンスを待つタスク
/// @pre SIV3D_ENGINE(Network)->init() が呼び出されていること。
AsyncTask<HTTPResponse> SimplePostAsync(URLView url, const HashTable<String, String>& headers, const void* src, size_t size)
{
	// ネットワークのエンジンが初期化されていることを事前条件にする
	// SIV3D_ENGINE(Network)->init();

	const auto wgetHandle = detail::siv3dCreateXMLHTTPRequest();

	detail::siv3dOpenXMLHTTPRequest(wgetHandle, "POST", url.toUTF8().data());

	for (auto&& [key, value] : headers)
	{
		detail::siv3dSetXMLHTTPRequestRequestHeader(wgetHandle, key.toUTF8().data(), value.toUTF8().data());
	}

	constexpr auto CallBack = [](int32 requestID, void* userData)
		{
			char* responseHeader = detail::siv3dGetXMLHTTPRequestResponseHeaders(requestID);
			HTTPResponse response{ std::string(responseHeader) };
			::free(responseHeader);

			auto promise = static_cast<std::promise<HTTPResponse>*>(userData);
			promise->set_value(response);
			delete promise;

			EM_ASM("setTimeout(function() { _siv3dMaybeAwake(); }, 0)");
		};

	auto promise = new std::promise<HTTPResponse>;
	AsyncTask<HTTPResponse> task = promise->get_future();
	detail::siv3dSetXMLHTTPRequestCallback(wgetHandle, CallBack, promise);
	detail::siv3dSetXMLHTTPRequestErrorCallback(wgetHandle, CallBack, promise);

	if (src)
	{
		std::string body(static_cast<const char*>(src), size);
		detail::siv3dSendXMLHTTPRequest(wgetHandle, body.data());
	}
	else
	{
		detail::siv3dSendXMLHTTPRequest(wgetHandle, nullptr);
	}

	return task;
}



/// @brief 有効なレコードかどうかをチェックします。
/// @param value レコードが格納された JSON
/// @return 有効なレコードなら true, そうでなければ false
bool IsValidRecord(const JSON& value)
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
bool ReadLeaderboard(const JSON& json, Array<Record>& dst)
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
		leaderboard << std::move(record);
	}

	dst = std::move(leaderboard);
	return true;
}

/// @brief サーバからリーダーボードを取得するタスクを作成します。
/// @param url サーバの URL
/// @param count 取得上限数
/// @return タスク
AsyncHTTPTask CreateGetTask(const URLView url, int32 count = 10)
{
	// GET リクエストの URL を作成する
	const URL requestURL = U"{}?count={}"_fmt(url, count);

	return SimpleHTTP::SaveAsync(requestURL, U"/leaderboard.json");
}

/// @brief サーバにスコアを送信するタスクを作成します。
/// @param url サーバの URL
/// @param userName ユーザー名
/// @param score スコア
/// @param additionalData 追加の情報
/// @return タスク
AsyncTask<HTTPResponse> CreatePostTask(const URLView url, const StringView userName, double score, JSON additionalData = JSON::Invalid())
{
	// POST リクエストの URL を作成する
	URL requestURL = U"{}?username={}&score={}"_fmt(url, PercentEncode(userName), PercentEncode(Format(score)));

	if (additionalData)
	{
		requestURL += (U"&data=" + PercentEncode(additionalData.formatMinimum()));
	}

	return SimplePostAsync(requestURL, {}, nullptr, 0);
}

///// @brief ランダムなスコアを返します。
///// @return ランダムなスコア
//double MakeRandomScore()
//{
//	return (Random(10000) / 100.0);
//}
//
///// @brief ランダムなユーザー名を作成します。
///// @return ランダムなユーザー名
//String MakeRandomUserName()
//{
//	static const Array<String> words1 =
//	{
//		U"Blue", U"Red", U"Green", U"Silver", U"Gold",
//		U"Happy", U"Angry", U"Sad", U"Exciting", U"Scary",
//		U"Big", U"Small", U"Large", U"Tiny", U"Short",
//	};
//
//	static const Array<String> words2 =
//	{
//		U"Lion", U"Dragon", U"Tiger", U"Eagle", U"Shark",
//		U"Pizza", U"Curry", U"Ramen", U"Sushi", U"Salad",
//		U"Cat", U"Dog", U"Mouse", U"Rabbit", U"Fox",
//	};
//
//	return (U"{} {} {:0>4}"_fmt(words1.choice(), words2.choice(), Random(9999)));
//}

Ranking::Ranking(const InitData& init)
	: IScene{ init }
{
	auto& data = getData();

	if (data.lastGameScore)
	{
		//const int32 lastScore = *data.lastGameScore;

		// ランキングを再構成
		//data.highScores << lastScore;
		//data.highScores.rsort();
		//data.highScores.resize(RankingCount);

		//// ランクインしていたら m_rank に順位をセット
		//for (int32 i = 0; i < RankingCount; ++i)
		//{
		//	if (data.highScores[i] == lastScore)
		//	{
		//		m_rank = i;
		//		break;
		//	}
		//}

		score = *data.lastGameScore;

		data.lastGameScore.reset();
	}
	else
	{
		score = 0;
	}

	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// リーダーボードを取得するタスク
	leaderboardGetTask = CreateGetTask(LeaderboardURL);

	userNameTextBox.text = (U"");
	//userName = (U"");
	//userName = MakeRandomUserName();

	//score = MakeRandomScore();

}

void Ranking::update()
{
	// 通信が完了しているか
	const bool isReady = (not leaderboardGetTask) && (not scorePostTask);

	// 自身のユーザー名を更新する
	//if (SimpleGUI::Button(U"\U000F0004 {}"_fmt(userName), Vec2{ 40, 40 }, 330))
	//{
	//	userName = MakeRandomUserName();
	//	isScorePosted = false;
	//}

	font(U"↓15文字以内で名前を入力してください。").draw(20, Vec2{ 40, 10 }, ColorF{ 0.11 });

	if (isScorePosted)
	{
		SimpleGUI::TextBox(userNameTextBox, Vec2{ 40, 40 }, 330, 15, false);
	}
	else
	{
		SimpleGUI::TextBox(userNameTextBox, Vec2{ 40, 40 }, 330, 15, true);
	}


	// 自身のスコアを更新する
	//if (SimpleGUI::Button(U"SCORE:{}"_fmt(score), Vec2{ 384, 40 }, 160, false))
	//{
	//	//score = MakeRandomScore();
	//	//isScorePosted = false;
	//}
	font(U"SCORE:{}"_fmt(score)).draw(20, Vec2{ 384, 40 }, ColorF{ 0.11 });

	// 現在のスコアを送信する
	if (SimpleGUI::Button(U"ランキング登録", { 560, 40 }, 160, (isReady && (not isScorePosted) && (userNameTextBox.text != (U"")) && score != 0)))
	{
		//scorePostTask = CreatePostTask(LeaderboardURL, userName, score);
		scorePostTask = CreatePostTask(LeaderboardURL, userNameTextBox.text, score);
	}

	if (SimpleGUI::Button(U"タイトルへ戻る", { 860, 40 }, 160, true))
	{
		// タイトルシーンへ
		changeScene(State::Title);
	}

	// リーダーボードを更新する
	//if (SimpleGUI::Button(U"ランキング更新", { 560, 100 }, 160, isReady))
	//{
	//	leaderboardGetTask = CreateGetTask(LeaderboardURL);
	//}

	// リーダーボードの更新時刻を表示する
	font(U"Last updated:\n{}"_fmt(lastUpdateTime)).draw(12, 560, 140, ColorF{ 0.25 });

	// スコア送信処理が完了したら
	if (scorePostTask && scorePostTask->isReady())
	{
		if (const auto response = scorePostTask->get();
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
			if (ReadLeaderboard(JSON::Load(U"/leaderboard.json"), leaderboard))
			{
				// 最後にリーダーボードを取得した時刻を更新する
				lastUpdateTime = DateTime::Now();
			}
			else
			{
				Print << U"Failed to read the leaderboard.";
			}
		}

		leaderboardGetTask.reset();
	}

	// リーダーボードを描画する
	if (leaderboard)
	{
		for (size_t i = 0; i < leaderboard.size(); ++i)
		{
			const auto& record = leaderboard[i];

			font(U"rank: {}, name: {}, score: {}"_fmt((i + 1), record.userName, record.score)).draw(20, Vec2{ 40, (100 + i * 30) }, ColorF{ 0.11 });
		}
	}
	else
	{
		// リーダーボードが空の場合は、ロード中であることを示すアニメーションを描画する
		Circle{ 292, 260, 80 }.drawArc((Scene::Time() * 90_deg), 300_deg, 10, 0);
	}
}

void Ranking::draw() const
{
	//Scene::SetBackground(ColorF{ 0.4, 0.6, 0.9 });

	//FontAsset(U"Ranking")(U"RANKING").drawAt(400, 60);

	//const auto& data = getData();

	//// ランキングを表示
	//for (auto i : step(RankingCount))
	//{
	//	const RectF rect{ 100, 120 + i * 80, 600, 80 };

	//	rect.draw(ColorF{ 1.0, 1.0 - i * 0.2 });

	//	FontAsset(U"Ranking")(data.highScores[i]).drawAt(rect.center(), ColorF{ 0.25 });

	//	// ランクインしていたら
	//	if (i == m_rank)
	//	{
	//		rect.stretched(Periodic::Triangle0_1(0.5s) * 10).drawFrame(10, ColorF{ 0.8, 0.6, 0.4 });
	//	}
	//}



}
