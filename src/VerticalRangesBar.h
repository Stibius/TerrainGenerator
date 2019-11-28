#pragma once

#include <QColor>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QVector>
#include <QWidget>

class VerticalRangesBar : public QWidget
{
	Q_OBJECT

public:

	explicit VerticalRangesBar(QWidget* parent = nullptr);

	VerticalRangesBar(const VerticalRangesBar& other) = default;

	VerticalRangesBar(VerticalRangesBar&& other) = default;

	VerticalRangesBar& operator=(const VerticalRangesBar& other) = default;

	VerticalRangesBar& operator=(VerticalRangesBar&& other) = default;

	~VerticalRangesBar() override = default;

	void setLength(int length);

	bool addSection(int index, const QColor& color = QColor(255, 255, 255));

	void removeSection(int index);

	void moveSectionUp(int index);

	void moveSectionDown(int index);

	void selectSection(int index);

	void setSectionColor(int index, const QColor& color);

	void setDividerColor(const QColor& color);

	void setBorderColor(const QColor& color);

	void setSelectionColor(const QColor& color);

	int getSelectedSection() const;

	float getSectionLowerBound(int index) const;

	float getSectionUpperBound(int index) const;

	bool isSectionDivisible(int index) const;

	const QColor& getSectionColor(int index) const;

	int getSectionCount() const;

	const QColor& getDividerColor() const;

	const QColor& getBorderColor() const;

	const QColor& getSelectionColor() const;

signals:

	void sectionHeightsChanged();

	void selectedSectionChanged(int index);

protected:

	void resizeEvent(QResizeEvent* event) override;

	void paintEvent(QPaintEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;

	void mouseReleaseEvent(QMouseEvent* event) override;

	void mouseMoveEvent(QMouseEvent* event) override;

private:

	struct BarSection
	{
		int m_start;
		int m_height;
		QColor m_color;

		BarSection();

		BarSection(int start, int height, const QColor& color);
	};

	int getDivider(int y) const;

	int getSection(int y);

	QVector<BarSection> m_sections;
	int m_selectedIndex = -1;

	QColor m_dividerColor = QColor(0, 0, 0);
	QColor m_defaultSectionColor = QColor(255, 255, 255);
	QColor m_borderColor = QColor(0, 0, 0);
	QColor m_selectionColor = QColor(0, 0, 0);

	static constexpr int DIVIDER_HEIGHT = 2;
	static constexpr int MIN_SECTION_HEIGHT = 5;
	static constexpr int SELECTION_BORDER_SIZE = 2;
	static constexpr int BORDER_SIZE = 1;

	int m_draggedDivider = -1;
	int m_lastDragPos;

	QSize m_oldSize;
};
