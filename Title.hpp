# pragma once
# include "Common.hpp"
# include "RichButton.hpp"

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init);

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

public:
	RichButton button1{ U"🏆"_emoji };
	RichButton button2{ U"↩️"_emoji };
	RichButton button3{ U"🎮"_emoji };
	RichButton button4{ U"✉️"_emoji };
};
