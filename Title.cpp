# include "Title.hpp"

Title::Title(const InitData& init)
	: IScene{ init }
{
	titleBgmAudio.playOneShot(0.2);
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

	m_pStartRichButton->draw(m_StartRect, font, U"あそぶ");
	m_pRankingRichButton->draw( m_RankingRect, font, U"ランキング");
	//button2.draw(Rect{ 220, 500, 160, 60 }, font, U"もどる");
	//button3.draw(Rect{ 400, 500, 160, 60 }, font, U"あそぶ");
	//button4.draw(Rect{ 580, 500, 160, 60 }, font, U"とうろく");
}
