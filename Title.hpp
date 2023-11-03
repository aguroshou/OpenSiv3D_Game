# pragma once
# include "Common.hpp"
# include "RichButton.hpp"

extern RichButton button1;

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init);
	~Title();

	void update() override;

	void draw() const override;

private:

	Rect m_startButton{ Arg::center = Scene::Center(), 300, 60 };
	Transition m_startTransition{ 0.4s, 0.2s };

	Rect m_rankingButton{ Arg::center = Scene::Center().movedBy(0, 100), 300, 60 };
	Transition m_exitTransition{ 0.4s, 0.2s };

	Rect m_exitButton{ Arg::center = Scene::Center().movedBy(0, 200), 300, 60 };
	Transition m_rankingTransition{ 0.4s, 0.2s };

	const Audio titleBgmAudio{ Audio::Stream, U"sounds/titleBgm.wav" };

	const Texture titleTexture{ U"images/title.png" };

	const Font font{ FontMethod::MSDF, 48, Typeface::Heavy };

	Rect m_StartRect{ 220, 415, 160, 60 };
	Rect m_RankingRect{ 220, 520, 160, 60 };

	// FIXME: 実体で実装する方法が分かりませんでした…。ポインタであればなぜかエラーが発生せずに実行できます。あとで勉強しなおす必要があります…。
	RichButton* m_pStartRichButton;
	RichButton* m_pRankingRichButton;

	//RichButton button1{ U"🏆"_emoji };
	//RichButton button2{ U"↩️"_emoji };
	//RichButton button3{ U"🎮"_emoji };
	//RichButton button4{ U"✉️"_emoji };
	//RichButton button1;
	//RichButton button2{ U"↩️"_emoji };
	//RichButton button3{ U"🎮"_emoji };
	//RichButton button4{ U"✉️"_emoji };
};
