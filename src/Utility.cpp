#include "Utility.h"

QPixmap Utility::heightmapToQPixmap(const Heightmap& heightmap)
{
	if (heightmap.isEmpty())
	{
		return QPixmap(0, 0);
	}

	QImage image(heightmap.getWidth(), heightmap.getHeight(), QImage::Format_ARGB32);

	Heightmap convertedHeightmap = heightmap;
	convertedHeightmap.normalize();

	for (int row = 0; row < heightmap.getHeight(); ++row)
	{
		for (int col = 0; col < heightmap.getWidth(); ++col)
		{
			int value = static_cast<int>(convertedHeightmap.at(row, col) * 255);
			image.setPixelColor(col, row, QColor(value, value, value, 255));
		}
	}

	return QPixmap::fromImage(image);
}

Heightmap Utility::QPixmapToHeightmap(const QPixmap& pixmap)
{
	Heightmap result;

	if (pixmap.isNull())
	{
		return result;
	}

	QImage image = pixmap.toImage();

	result.setSize(pixmap.width(), pixmap.height());

	for (int row = 0; row < result.getHeight(); ++row)
	{
		for (int col = 0; col < result.getWidth(); ++col)
		{
			result.at(row, col) = image.pixelColor(col, row).redF();
		}
	}

	return result;
}

QString Utility::vecToQColorName(glm::vec4 color)
{
	return QColor(color.r * 255, color.g * 255, color.b * 255).name();
}

QColor Utility::vecToQColor(glm::vec4 color)
{
	return QColor(color.r * 255, color.g * 255, color.b * 255);
}

glm::vec4 Utility::QColorToVec(const QColor& color)
{
	return glm::vec4(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

QColor Utility::getAverageColor(const QImage& image)
{
	int red = 0;
	int green = 0;
	int blue = 0;

	for (int y = 0; y < image.height(); ++y)
	{
		for (int x = 0; x < image.width(); ++x)
		{
			QColor color = image.pixel(x, y);
			red += color.red();
			green += color.green();
			blue += color.blue();
		}
	}

	return QColor(
		red / (image.width() * image.height()), 
		green / (image.width() * image.height()), 
		blue / (image.width() * image.height()));
}
