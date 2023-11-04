# pragma once
# include "Common.hpp"
# include "RichButton.hpp"
# include "Url.hpp"

// ランキングシーン
class Ranking : public App::Scene
{
private:
	/// @brief レコード
	struct Record
	{
		/// @brief ユーザー名
		String userName;

		/// @brief スコア
		int32 score;

		/// @brief 追加の情報（このサンプルでは特に使いません）
		JSON data;
	};

public:

	bool IsValidRecord(const JSON& value);
	bool ReadLeaderboard(const JSON& json, Array<Record>& dst);
	SimpleTable ToTable(const Array<Record>& leaderboard);
	AsyncHTTPTask CreateGetTask(const URLView url, int32 count = 10);
	AsyncHTTPTask CreatePostTask(const URLView url, const StringView userName, double score, JSON additionalData = JSON::Invalid());

	Ranking(const InitData& init);
	~Ranking();

	void update() override;

	void draw() const override;

private:
	static constexpr int32 RankingCount = 5;

	int32 m_rank = -1;

	Rect m_SubscribeRect{ 220, 415, 160, 60 };
	Rect m_BackRect{ 220, 520, 160, 60 };

	const Font font{ FontMethod::MSDF, 48, Typeface::Heavy };

	const Texture rankingTexture{ U"images/ranking.png" };

	RichButton* m_pSubscribeRichButton;
	RichButton* m_pBackRichButton;


#ifdef URL_HEADER
	// 念のためにURLは公開リポジトリへアップロードしないようにしています。
	const std::string url{ SIV3D_OBFUSCATE(SPREAD_SHEET_URL) };
#else
	// FIXME: 下記リポジトリに掲載されている、スプレッドシートのURLを掲載していますが、別の方が実装する際には修正する必要があります。
	//https://github.com/Siv3D/Siv3D-Samples/tree/main/Samples/Leaderboard
	// Google Apps Script の URL（サンプル用の URL. 定期的に記録がクリアされます）
	// 実行ファイルに URL が直接埋め込まれるのを防ぐため、SIV3D_OBFUSCATE() で URL を難読化
	const std::string url{ SIV3D_OBFUSCATE("https://script.google.com/macros/s/AKfycby-oJycoQPkdumtPjNYoXej1hRB-BjZaa9ZgnfHHesdMkqbY6TeW3h9fctIxXeIPM2A/exec") };
#endif

	//const std::string url{ SIV3D_OBFUSCATE("https://script.google.com/macros/s/AKfycby-oJycoQPkdumtPjNYoXej1hRB-BjZaa9ZgnfHHesdMkqbY6TeW3h9fctIxXeIPM2A/exec") };
	const URL LeaderboardURL = Unicode::Widen(url);

	// リーダーボードを表示するテーブル
	SimpleTable table;

	// リーダーボードを取得するタスク
	Optional<AsyncHTTPTask> leaderboardGetTask;/* = CreateGetTask(LeaderboardURL);*/

	// スコアを送信するタスク
	Optional<AsyncHTTPTask> scorePostTask;

	// 自身のユーザ名
	String userName;

	// 自身のスコア
	double score = 0;

	// 最後にリーダーボードを取得した時刻
	DateTime lastUpdateTime{ 2023, 1, 1 };

	// スコアを送信したか
	bool isScorePosted = false;

	Array<Record> leaderboard;

	TextEditState userNameTextBox;

};
