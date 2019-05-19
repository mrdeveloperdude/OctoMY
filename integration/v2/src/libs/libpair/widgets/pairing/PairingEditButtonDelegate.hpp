#ifndef PAIRINGEDITBUTTONDELEGATE_HPP
#define PAIRINGEDITBUTTONDELEGATE_HPP

#include <QSizeF>
#include <QPixmap>
#include <QItemDelegate>

class PairingWizard;
class QPainter;

class PairingEditButtonDelegate: public QItemDelegate
{
	Q_OBJECT

public:
	explicit PairingEditButtonDelegate(QObject *parent=nullptr);
	virtual ~PairingEditButtonDelegate() Q_DECL_OVERRIDE;

public:
	static QSizeF calcSize(QSizeF ds, const qint32 w, const qint32 h, qreal const z=0.0);

public:
	QPixmap svgToPixmap(QString fn, const  qint32 w, const qint32 h, const qreal zoom=0.0) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) Q_DECL_OVERRIDE;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

signals:
	void startEdit(int row);

};


#endif
// PAIRINGEDITBUTTONDELEGATE_HPP
