# include "Title.hpp"

//button1 = RichButton{ U"🏆"_emoji };

Title::Title(const InitData& init)
	: IScene{ init }
	//, button1{ U"🏆"_emoji }
{
	titleBgmAudio.playOneShot(0.2);
	//RichButton button1( U"🏆"_emoji );
	m_pStartRichButton = new RichButton(U"🎮"_emoji);
	m_pRankingRichButton = new RichButton(U"🏆"_emoji);
}

Title::~Title()
{
	delete m_pStartRichButton;
	delete m_pRankingRichButton;
}

void Title::update()
{
	m_startTransition.update(m_startButton.mouseOver());
	m_rankingTransition.update(m_rankingButton.mouseOver());
	m_exitTransition.update(m_exitButton.mouseOver());

	//if (m_startButton.mouseOver() || m_rankingButton.mouseOver() || m_exitButton.mouseOver())
	//{
	//	Cursor::RequestStyle(CursorStyle::Hand);
	//}

	//if (m_startButton.leftClicked())
	//{
	//	// ゲームシーンへ
	//	changeScene(State::Game);
	//}
	//else if (m_rankingButton.leftClicked())
	//{
	//	// ランキングシーンへ
	//	changeScene(State::Ranking);
	//}
	//else if (m_exitButton.leftClicked())
	//{
	//	// 終了
	//	System::Exit();
	//}

	if (m_StartRect.leftClicked())
	{
		// ゲームシーンへ
		changeScene(State::Game);
	}
	else if (m_RankingRect.leftClicked())
	{
		// ランキングシーンへ
		changeScene(State::Ranking);
	}
}

void Title::draw() const
{
	Scene::SetBackground(ColorF{ 0, 0, 0 });

	titleTexture.resized(1280).draw(0, 0);

	FontAsset(U"Menu")(U"モグラ逃し")
		.drawAt(TextStyle::OutlineShadow(0.2, ColorF{ 0.2, 0.6, 0.2 }, Vec2{ 3, 3 }, ColorF{ 0.0, 0.5 }), 100, Vec2{ 400, 100 });

	//m_startButton.draw(ColorF{ 1.0, m_startTransition.value() }).drawFrame(2);
	//m_rankingButton.draw(ColorF{ 1.0, m_rankingTransition.value() }).drawFrame(2);
	//m_exitButton.draw(ColorF{ 1.0, m_exitTransition.value() }).drawFrame(2);

	//FontAsset(U"Menu")(U"PLAY").drawAt(m_startButton.center(), ColorF{ 0.25 });
	//FontAsset(U"Menu")(U"RANKING").drawAt(m_rankingButton.center(), ColorF{ 0.25 });
	//FontAsset(U"Menu")(U"EXIT").drawAt(m_exitButton.center(), ColorF{ 0.25 });


	m_pStartRichButton->draw(m_StartRect, font, U"あそぶ");
	//pStartButton->draw(Rect{ 220, 415, 160, 60 }, font, U"あそぶ");
	m_pRankingRichButton->draw( m_RankingRect, font, U"ランキング");
	//button2.draw(Rect{ 220, 500, 160, 60 }, font, U"もどる");
	//button3.draw(Rect{ 400, 500, 160, 60 }, font, U"あそぶ");
	//button4.draw(Rect{ 580, 500, 160, 60 }, font, U"とうろく");
}
