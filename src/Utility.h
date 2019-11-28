#pragma once

#include <QColor>
#include <QPixmap>
#include <QImage>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Heightmap.h"

class Utility
{
public:

	static QPixmap heightmapToQPixmap(const Heightmap& heightmap);

	static Heightmap QPixmapToHeightmap(const QPixmap& pixmap);

	static QString vecToQColorName(glm::vec4 color);

	static QColor vecToQColor(glm::vec4 color);

	static glm::vec4 QColorToVec(const QColor& color);

	static QColor getAverageColor(const QImage& image);
};
