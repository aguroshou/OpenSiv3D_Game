# pragma once
# include "Common.hpp"
# include "RichButton.hpp"

// ボタンの背景テクスチャを作成する
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

RichButton::RichButton(const Emoji& emoji)
	: m_emoji{ emoji }
	, m_bufferedEmoji{ MakeRoundBuffer(CreateEmojiPolygons(emoji), 4).scaled(EmojiScale) }
{
}


void RichButton::draw(const Rect& rect,/* const Texture& buttonTexture, */const Font& font, const String& text)
{
	//const ColorF PrimaryColor{ 0.3, 0.5, 1.0 };
	const ColorF PrimaryColor{ 0.3, 0.25, 0.2 };
	const ColorF TextColor{ 0.8, 0.8, 0.8 };
	const Color TextMouseOverColor{ 241, 130, 0 };

	const RoundRect roundRect{ rect, 10 };

	m_isMouseOver = roundRect.mouseOver();

	m_transition.update((not roundRect.intersects(Cursor::PreviousPos())) && m_isMouseOver);



	{
		double angle = Math::Sin(m_transition.value() * 8_pi) * 6_deg * m_transition.value();
		const Vec2 emojiCenter = rect.getRelativePoint(0.05, 0.5);

		{
			const Transformer2D transformer{ Mat3x2::Rotate(angle, emojiCenter) };
			m_bufferedEmoji.draw(emojiCenter, m_isMouseOver ? PrimaryColor : ColorF{ 0.3, 0.25, 0.2 });
			m_emoji.scaled(EmojiScale).rotated(angle).drawAt(emojiCenter);
		}
	}

	if (m_isMouseOver)
	{
		Cursor::RequestStyle(CursorStyle::Hand);

		font(text).drawAt(TextStyle::Outline(0.2, 0.2, PrimaryColor), 26, rect.getRelativePoint(0.6, 0.5), TextMouseOverColor);

		//roundRect(buttonTexture).draw(MouseL.pressed() ? ColorF{ 0.95 } : ColorF{ 1.05 }).drawFrame(0, 2, PrimaryColor);
	}
	else
	{
		font(text).drawAt(TextStyle::Outline(0.2, 0.2, PrimaryColor), 26, rect.getRelativePoint(0.6, 0.5), PrimaryColor);

		//roundRect(buttonTexture).draw().drawFrame(2);
	}

}

MultiPolygon RichButton::CreateEmojiPolygons(const Emoji& emoji)
{
	return Image{ emoji }.alphaToPolygonsCentered(160, AllowHoles::No);
}

MultiPolygon RichButton::MakeRoundBuffer(const MultiPolygon& polygons, int32 distance)
{
	MultiPolygon result;

	for (const auto& polygon : polygons)
	{
		result = Geometry2D::Or(result, polygon.calculateRoundBuffer(distance));
	}

	return result;
}
