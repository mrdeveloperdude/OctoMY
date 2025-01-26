#ifndef CELLDELEGATE_HPP
#define CELLDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QDoubleValidator>

class CellDelegate : public QStyledItemDelegate {
	Q_OBJECT
public:
	explicit CellDelegate(QObject *parent = nullptr);
	
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	
	bool eventFilter(QObject *object, QEvent *event) override;
};

#endif // CELLDELEGATE_HPP
