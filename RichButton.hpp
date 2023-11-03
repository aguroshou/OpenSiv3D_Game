# pragma once
# include "Common.hpp"

//// ボタンの背景テクスチャを作成する
//Texture CreateButtonTexture()
//{
//	MSRenderTexture renderTexture{ Size{ 160, 60 }, ColorF{ 0.96 } };
//	{
//		const ScopedRenderTarget2D renderTarget{ renderTexture };
//
//		const ColorF PatternColor{ 0.85 };
//
//		for (int32 x = 0; x <= 8; ++x)
//		{
//			RectF{ Arg::center((x * 20), 25), 2 }.rotated(45_deg).draw(PatternColor);
//			RectF{ Arg::center((10 + x * 20), 30), 3 }.rotated(45_deg).draw(PatternColor);
//			RectF{ Arg::center((x * 20), 35), 4 }.rotated(45_deg).draw(PatternColor);
//			RectF{ Arg::center((10 + x * 20), 40), 5 }.rotated(45_deg).draw(PatternColor);
//			RectF{ Arg::center((x * 20), 45), 6 }.rotated(45_deg).draw(PatternColor);
//			RectF{ Arg::center((10 + x * 20), 50), 7 }.rotated(45_deg).draw(PatternColor);
//			RectF{ Arg::center((x * 20), 55), 8 }.rotated(45_deg).draw(PatternColor);
//			RectF{ Arg::center((10 + x * 20), 60), 9 }.rotated(45_deg).draw(PatternColor);
//		}
//	}
//
//	// MSRenderTexture の完成には
//	// 2D 描画命令の発行 (Flush) + MSAA の解決 (Resolve) が必要
//	Graphics2D::Flush();
//	renderTexture.resolve();
//
//	// 完成したテクスチャを返す
//	return renderTexture;
//}

class RichButton
{
public:

	RichButton() = default;

	explicit RichButton(const Emoji& emoji);

	void draw(const Rect& rect, /*const Texture& buttonTexture, */const Font& font, const String& text);

private:

	Texture m_emoji;

	MultiPolygon m_bufferedEmoji;

	Transition m_transition{ 0.0s, 0.8s };

	static constexpr double EmojiScale = 0.25;

	static MultiPolygon CreateEmojiPolygons(const Emoji& emoji);

	static MultiPolygon MakeRoundBuffer(const MultiPolygon& polygons, int32 distance);
};

//void Main()
//{
//	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
//
//	//const Texture buttonTexture = CreateButtonTexture();
//	const Font font{ FontMethod::MSDF, 48, Typeface::Heavy };
//
//	RichButton button1{ U"🏆"_emoji };
//	RichButton button2{ U"↩️"_emoji };
//	RichButton button3{ U"🎮"_emoji };
//	RichButton button4{ U"✉️"_emoji };
//
//	while (System::Update())
//	{
//		button1.draw(Rect{ 40, 500, 160, 60 }, font, U"ランキング");
//		button2.draw(Rect{ 220, 500, 160, 60 }, font, U"もどる");
//		button3.draw(Rect{ 400, 500, 160, 60 }, font, U"あそぶ");
//		button4.draw(Rect{ 580, 500, 160, 60 }, font, U"とうろく");
//		//button1.draw(Rect{ 40, 500, 160, 60 }, buttonTexture, font, U"ランキング");
//		//button2.draw(Rect{ 220, 500, 160, 60 }, buttonTexture, font, U"もどる");
//		//button3.draw(Rect{ 400, 500, 160, 60 }, buttonTexture, font, U"あそぶ");
//		//button4.draw(Rect{ 580, 500, 160, 60 }, buttonTexture, font, U"とうろく");
//	}
//}
