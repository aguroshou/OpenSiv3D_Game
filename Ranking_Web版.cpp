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

Ranking::Ranking(const InitData& init)
	: IScene{ init }
{
	auto& data = getData();

	if (data.lastGameScore)
	{
		score = *data.lastGameScore;

		data.lastGameScore.reset();
	}
	else
	{
		score = 0;
	}

	Scene::SetBackground(ColorF{ 0 });

	// リーダーボードを取得するタスク
	leaderboardGetTask = CreateGetTask(LeaderboardURL);

	userNameTextBox.text = (U"");

	m_pSubscribeRichButton = new RichButton(U"✉️"_emoji);
	m_pBackRichButton = new RichButton(U"↩️"_emoji);

	userNameTextBox.text = U"ななし";

	rankingAudio.playOneShot(0.5);
	clickAudio.setVolume(0.3);
}

Ranking::~Ranking()
{
	delete m_pSubscribeRichButton;
	delete m_pBackRichButton;
}

void Ranking::update()
{
	// 通信が完了しているか
	const bool isReady = (not leaderboardGetTask) && (not scorePostTask);

	// 現在のスコアを送信する
	if (m_SubscribeRect.leftClicked() && not isScorePosted)
	{
		clickAudio.play();
		scorePostTask = CreatePostTask(LeaderboardURL, userNameTextBox.text, score);
	}

	if (m_BackRect.leftClicked())
	{
		changeScene(State::Title);
	}

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

	rankingTexture.resized(1280).draw(0, 0);

	SimpleGUI::TextBox(userNameTextBox, Vec2{ 220, 297 }, 220, 10, !isScorePosted);
}

void Ranking::draw() const
{
	m_pSubscribeRichButton->draw(m_SubscribeRect, font, U"とうろく");
	m_pBackRichButton->draw(m_BackRect, font, U"もどる");

	font(U"なまえ").draw(20, Vec2{ 150, 300 }, ColorF{ 0.11 });
	font(U"スコア <{}>"_fmt(score)).draw(20, Vec2{ 150, 250 }, ColorF{ 0.11 });

	// リーダーボードを描画する
	if (leaderboard)
	{
		int32 rankingSize = std::min(static_cast<int32>(leaderboard.size()), 10);
		for (size_t i = 0; i < rankingSize; ++i)
		{
			const auto& record = leaderboard[i];

			font(U"{}"_fmt(record.userName)).draw(20, Vec2{ 820, (185 + i * 50) }, ColorF{ 0.11 });
			font(U"<{}>"_fmt(record.score)).draw(20, Vec2{ 1050, (185 + i * 50) }, ColorF{ 0.11 });
		}
	}
}
