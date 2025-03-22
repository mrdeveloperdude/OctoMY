#ifndef FLOWLAYOUT_HPP
#define FLOWLAYOUT_HPP

#include <QLayout>
#include <QRect>
#include <QStyle>
#include <QList>

class FlowLayout : public QLayout {
private:
	QList<QLayoutItem *> itemList;
	int m_hSpace;
	int m_vSpace;

public:
	FlowLayout(QWidget *parent=nullptr, int margin = -1, int hSpacing = -1, int vSpacing = -1);
	~FlowLayout();
	
	void addItem(QLayoutItem *item) override;
	int horizontalSpacing() const;
	int verticalSpacing() const;
	int count() const override;
	QLayoutItem *itemAt(int index) const override;
	QLayoutItem *takeAt(int index) override;
	Qt::Orientations expandingDirections() const override;
	bool hasHeightForWidth() const override;
	int heightForWidth(int) const override;
	void setGeometry(const QRect &rect) override;
	QSize sizeHint() const override;
	QSize minimumSize() const override;
	
protected:
	int doLayout(const QRect &rect, bool testOnly) const;
	
private:
	int smartSpacing(QStyle::PixelMetric pm) const;
	

};

#endif // FLOWLAYOUT_HPP
