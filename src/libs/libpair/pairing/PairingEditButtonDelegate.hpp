#ifndef PAIRINGEDITBUTTONDELEGATE_HPP
#define PAIRINGEDITBUTTONDELEGATE_HPP

#include <QSizeF>
#include <QPixmap>
#include <QItemDelegate>

class PairingActivity;
class QPainter;



/**
 * @brief The PairingEditButtonDelegate class is a delegate that shows a potential
 * associate in a list. It displays the following main parts;
 * 1. Identicon of the associate
 * 2. Connection status (bars represent connection stability/speed)
 * 3. Trust status (unknown, distrusted or trusted)
 * 4. Edit button (to make changes to trust level and more)
 */


class PairingEditButtonDelegate: public QItemDelegate
{
	Q_OBJECT
private:
	bool mDebug{false};

public:
	explicit PairingEditButtonDelegate(QObject *parent=nullptr);
	virtual ~PairingEditButtonDelegate() override;

public:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
	void startEdit(const QString id);

};


#endif
// PAIRINGEDITBUTTONDELEGATE_HPP
