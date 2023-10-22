# include "Ranking.hpp"

Ranking::Ranking(const InitData& init)
	: IScene{ init }
{
	auto& data = getData();

	if (data.lastGameScore)
	{
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
}

void Ranking::update()
{
	if (MouseL.down())
	{
		// タイトルシーンへ
		changeScene(State::Title);
	}
}

void Ranking::draw() const
{
	Scene::SetBackground(ColorF{ 0.4, 0.6, 0.9 });

	FontAsset(U"Ranking")(U"RANKING").drawAt(400, 60);

	const auto& data = getData();

	// ランキングを表示
	for (auto i : step(RankingCount))
	{
		const RectF rect{ 100, 120 + i * 80, 600, 80 };

		rect.draw(ColorF{ 1.0, 1.0 - i * 0.2 });

		FontAsset(U"Ranking")(data.highScores[i]).drawAt(rect.center(), ColorF{ 0.25 });

		// ランクインしていたら
		if (i == m_rank)
		{
			rect.stretched(Periodic::Triangle0_1(0.5s) * 10).drawFrame(10, ColorF{ 0.8, 0.6, 0.4 });
		}
	}
}
