#ifndef AGENTITEMDELEGATE_HPP
#define AGENTITEMDELEGATE_HPP



#include <QSizeF>
#include <QPixmap>
#include <QItemDelegate>

class PairingActivity;
class QPainter;



/**
 * @brief The AgentItemDelegate class is a delegate that shows an agent in a
 * list. It displays the following main parts;
 * 1. Icon
 * 2. Name
 * 3. Trust
 * 4. Online status
 * 4. Edit button (to make changes to the trust)
 * 5. Control button (to go to the remote control of this agent)
 */


class AgentItemDelegate: public QItemDelegate
{
	Q_OBJECT
private:
	bool mDebug{false};
	
public:
	explicit AgentItemDelegate(QObject *parent=nullptr);
	virtual ~AgentItemDelegate() override;
	
public:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	
signals:
	void startEdit(const QString id);
	void startControl(const QString id);
	
};


#endif // AGENTITEMDELEGATE_HPP
