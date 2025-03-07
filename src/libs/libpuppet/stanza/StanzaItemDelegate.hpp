#ifndef STANZAITEMDELEGATE_HPP
#define STANZAITEMDELEGATE_HPP

#include <QSizeF>
#include <QPixmap>
#include <QItemDelegate>

class PairingMenuActivity;
class QPainter;



/**
 * @brief The StanzaItemDelegate class is a delegate that shows a stanza in a
 * list. It displays the following main parts;
 * 1. Icon
 * 2. Title
 * 3. Description
 * 4. Edit button (to make changes to the stanza)
 */


class StanzaItemDelegate: public QItemDelegate
{
	Q_OBJECT
private:
	bool mDebug{false};

public:
	explicit StanzaItemDelegate(QObject *parent=nullptr);
	virtual ~StanzaItemDelegate() override;

public:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
	void startEdit(const QString id);

};


#endif
// STANZAITEMDELEGATE_HPP
