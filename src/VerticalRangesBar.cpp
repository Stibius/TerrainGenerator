
#include "VerticalRangesBar.h"

#include <QPainter>

#include <limits>
#include <utility> 

VerticalRangesBar::VerticalRangesBar(QWidget* parent)
	: QWidget(parent)
{
	m_sections.push_back(BarSection(0, this->height(), m_defaultSectionColor));
	m_selectedIndex = 0;
	m_oldSize = this->size();

	emit selectedSectionChanged(m_selectedIndex);
}

void VerticalRangesBar::setLength(int length)
{
	setMinimumHeight(length);
	setMaximumHeight(length);
}

bool VerticalRangesBar::addSection(int index, const QColor& color)
{
	if (index < 0 || index >= m_sections.count())
	{
		return false;
	}

	if (isSectionDivisible(index))
	{
		BarSection& section = m_sections[index];

		BarSection newSection = BarSection(section.m_start, section.m_height / 2, color);
		section.m_start = newSection.m_start + newSection.m_height;
		section.m_height -= newSection.m_height;
		m_sections.insert(&section, newSection);

		update();

		emit selectedSectionChanged(m_selectedIndex);

		return true;
	}

	return false;
}

void VerticalRangesBar::removeSection(int index)
{
	if (index < 0 || index >= m_sections.count())
	{
		return;
	}

	const BarSection& current = m_sections[index];

	if (index < m_sections.count() - 1)
	{
		BarSection& next = m_sections[index + 1];

		next.m_height = next.m_start + next.m_height - current.m_start;
		next.m_start = current.m_start;
	}
	else if (index > 0)
	{
		BarSection& prev = m_sections[index - 1];

		prev.m_height = current.m_start + current.m_height - prev.m_start;
	}
	else
	{
		return;
	}

	m_sections.remove(index);

	if (m_selectedIndex > index || m_selectedIndex == m_sections.count())
	{
		m_selectedIndex--;
	}

	update();

	if (m_selectedIndex == index)
	{
		emit selectedSectionChanged(m_selectedIndex);
	}
}

void VerticalRangesBar::moveSectionUp(int index)
{
	if (index >= 0 && index < m_sections.size() - 1)
	{
		BarSection& upper = m_sections[index + 1];
		BarSection& lower = m_sections[index];

		upper.m_start = lower.m_start;
		lower.m_start += upper.m_height;
		
		std::swap(upper, lower);

		if (m_selectedIndex == index)
		{
			m_selectedIndex = index + 1;
		}
		else if (m_selectedIndex == index + 1)
		{
			m_selectedIndex = index;
		}

		update();
	}
}

void VerticalRangesBar::moveSectionDown(int index)
{
	if (index > 0 && index < m_sections.size())
	{
		BarSection& upper = m_sections[index];
		BarSection& lower = m_sections[index - 1];

		upper.m_start = lower.m_start;
		lower.m_start += upper.m_height;

		std::swap(upper, lower);

		if (m_selectedIndex == index)
		{
			m_selectedIndex = index - 1;
		}
		else if (m_selectedIndex == index - 1)
		{
			m_selectedIndex = index;
		}

		update();
	}
}

void VerticalRangesBar::selectSection(int index)
{
	if (index != m_selectedIndex && index >= 0 && index < m_sections.count())
	{
		m_selectedIndex = index;

		update();

		emit selectedSectionChanged(m_selectedIndex);
	}
}

void VerticalRangesBar::setSectionColor(int index, const QColor& color)
{
	if (index >= 0 && index < m_sections.count())
	{
		m_sections[index].m_color = color;

		update();
	}
}

void VerticalRangesBar::setDividerColor(const QColor& color)
{
	m_dividerColor = color;

	update();
}

void VerticalRangesBar::setBorderColor(const QColor& color)
{
	m_borderColor = color;

	update();
}

void VerticalRangesBar::setSelectionColor(const QColor& color)
{
	m_selectionColor = color;

	update();
}

int VerticalRangesBar::getSelectedSection() const
{
	return m_selectedIndex;
}

float VerticalRangesBar::getSectionLowerBound(int index) const
{
	if (index < 0 || index >= m_sections.count())
	{
		return -1.0f;
	}

	return static_cast<float>(m_sections[index].m_start) / static_cast<float>(this->height() - 1);
}

float VerticalRangesBar::getSectionUpperBound(int index) const
{
	if (index < 0 || index >= m_sections.count())
	{
		return -1.0f;
	}

	return static_cast<float>(m_sections[index].m_start + m_sections[index].m_height) / static_cast<float>(this->height());
}

bool VerticalRangesBar::isSectionDivisible(int index) const
{
	if (index < 0 || index >= m_sections.count())
	{
		return false;
	}

	if (m_sections[index].m_height >= 2 * MIN_SECTION_HEIGHT)
	{
		return true;
	}
	else
	{
		return false;
	}
}

const QColor& VerticalRangesBar::getSectionColor(int index) const
{
	if (index >= 0 && index < m_sections.count())
	{
		return m_sections[index].m_color;
	}
	else
	{
		return QColor(0, 0, 0);
	}
}

int VerticalRangesBar::getSectionCount() const
{
	return m_sections.count();
}

const QColor& VerticalRangesBar::getDividerColor() const
{
	return m_dividerColor;
}

const QColor& VerticalRangesBar::getBorderColor() const
{
	return m_borderColor;
}

const QColor& VerticalRangesBar::getSelectionColor() const
{
	return m_selectionColor;
}

void VerticalRangesBar::resizeEvent(QResizeEvent* event)
{
	if (m_sections.count() == 1)
	{
		m_sections[0].m_start = 0;
		m_sections[0].m_height = this->height();
	}
	else
	{
		float scale = static_cast<float>(this->height()) / static_cast<float>(m_oldSize.height());

		int start = 0;
		for (BarSection& section : m_sections)
		{
			int newLength;
			if (&section == &m_sections.last())
			{
				newLength = this->height() - start;
			}
			else
			{
				newLength = round(section.m_height * scale);

			}
			section.m_start = start;
			section.m_height = newLength;
			start += section.m_height;
		}
	}

	m_oldSize = this->size();

	QWidget::resizeEvent(event);
}

void VerticalRangesBar::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	for (int i = 0; i < m_sections.count(); ++i)
	{
		const BarSection& section = m_sections[i];
		painter.setBrush(section.m_color);
		painter.setPen(Qt::NoPen);
		painter.drawRect(
			0,
			this->height() - section.m_start - section.m_height,
			this->width(),
			section.m_height);
	}

	for (int i = 0; i < m_sections.count(); ++i)
	{
		const BarSection& section = m_sections[i];
		if (i < m_sections.count() - 1)
		{
			painter.setBrush(m_dividerColor);
			painter.setPen(Qt::NoPen);

			painter.drawRect(
				BORDER_SIZE,
				this->height() - section.m_start - section.m_height - DIVIDER_HEIGHT / 2,
				this->width() - 2 * BORDER_SIZE,
				DIVIDER_HEIGHT
			);
		}
	}

	painter.setBrush(Qt::NoBrush);
	painter.setPen(QPen(m_borderColor, BORDER_SIZE, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
	painter.drawRect(
		BORDER_SIZE / 2,
		BORDER_SIZE / 2,
		this->width() - BORDER_SIZE,
		this->height() - BORDER_SIZE);

	if (m_selectedIndex != -1)
	{
		const BarSection& section = m_sections[m_selectedIndex];
		painter.setBrush(Qt::NoBrush);
		painter.setPen(QPen(m_selectionColor, SELECTION_BORDER_SIZE, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
		
		painter.drawRect(
			SELECTION_BORDER_SIZE / 2,
			this->height() - section.m_start - section.m_height + SELECTION_BORDER_SIZE / 2,
			this->width() - SELECTION_BORDER_SIZE,
			section.m_height - SELECTION_BORDER_SIZE);
		
	}
}

void VerticalRangesBar::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		int y = event->y();

		m_draggedDivider = getDivider(y);

		if (m_draggedDivider != -1)
		{
			m_lastDragPos = y;
		}
		else
		{
			int sectionIndex = getSection(y);

			if (sectionIndex != -1 && sectionIndex != m_selectedIndex)
			{
				m_selectedIndex = sectionIndex;

				update();

				emit selectedSectionChanged(m_selectedIndex);
			}
		}
	}
}

void VerticalRangesBar::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_draggedDivider = -1;
	}
}

void VerticalRangesBar::mouseMoveEvent(QMouseEvent* event)
{
	int y = event->y();

	if (m_draggedDivider != -1)
	{
		BarSection& section1 = m_sections[m_draggedDivider];

		section1.m_height += m_lastDragPos - y;
		if (section1.m_height < MIN_SECTION_HEIGHT)
		{
			section1.m_height = MIN_SECTION_HEIGHT;
		}

		if (m_draggedDivider < m_sections.count() - 1)
		{
			BarSection& section2 = m_sections[m_draggedDivider + 1];

			if (section1.m_start + section1.m_height > section2.m_start + section2.m_height - MIN_SECTION_HEIGHT)
			{
				section1.m_height = section2.m_start + section2.m_height - MIN_SECTION_HEIGHT - section1.m_start;
			}

			section2.m_start = section1.m_start + section1.m_height;
			if (m_draggedDivider < m_sections.count() - 2)
			{
				const BarSection& section3 = m_sections[m_draggedDivider + 2];
				section2.m_height = section3.m_start - section2.m_start;
			}
			else
			{
				section2.m_height = this->height() - section2.m_start;
			}
		}
		else
		{
			if (section1.m_start + section1.m_height - 1 >= this->height() - MIN_SECTION_HEIGHT)
			{
				section1.m_height = this->height() - MIN_SECTION_HEIGHT - section1.m_start;
			}
		}
		
		m_lastDragPos = y;

		update();

		emit sectionHeightsChanged();
	}
	else
	{
		if (getDivider(y) != -1)
		{
			this->setCursor(Qt::SplitVCursor);

		}
		else
		{
			this->setCursor(Qt::ArrowCursor);
		}
	}
}

int VerticalRangesBar::getDivider(int y) const
{
	int yy = this->height() - 1 - y;

	for (int i = 0; i < m_sections.count() - 1; ++i)
	{
		int sectionEnd = m_sections[i].m_start + m_sections[i].m_height - 1;

		if (yy > sectionEnd - DIVIDER_HEIGHT / 2 && yy <= sectionEnd + DIVIDER_HEIGHT / 2)
		{
			return i;
		}
	}

	return -1;
}

int VerticalRangesBar::getSection(int y)
{
	int yy = this->height() - 1 - y;

	for (int i = 0; i < m_sections.count(); ++i)
	{
		if (yy >= m_sections[i].m_start && yy < m_sections[i].m_start + m_sections[i].m_height)
		{
			return i;
		}
	}

	return -1;
}

VerticalRangesBar::BarSection::BarSection()
	: m_start(0)
	, m_height(0)
	, m_color(QColor())
{
}

VerticalRangesBar::BarSection::BarSection(int start, int height, const QColor& color)
	: m_start(start)
	, m_height(height)
	, m_color(color)
{

}
