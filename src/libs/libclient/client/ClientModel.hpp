#ifndef CLIENTMODEL_HPP
#define CLIENTMODEL_HPP

#include <QAbstractItemModel>
#include <QFileIconProvider>
#include <QIcon>
#include <QVector>
#include <QMap>

class CommsSession;
class CommsSessionDirectory;

/*!
 * \brief The ClientModel class is a QAbstractItemModel implementation for displaying clients in the UI
 */
class ClientModel : public QAbstractItemModel
{
	Q_OBJECT

private:
	struct Node {
		Node(Node *parent = 0) : parent(parent), children(0) {}
		~Node()
		{
			delete children;
			children=nullptr;
		}
		Node *parent;
		QVector<Node> *children;
	};

private:
	CommsSessionDirectory *clients;

	QIcon services;
	QVector<Node> *tree;
	QFileIconProvider iconProvider;


private:
	Node *node(int row, Node *parent) const;
	Node *parent(Node *child) const;
	int row(Node *node) const;
	int rc() const;
	int cc() const;

public:
	explicit ClientModel(CommsSessionDirectory *, QObject *parent = nullptr);
	virtual ~ClientModel();

public:
	QModelIndex index(int row, int column, const QModelIndex &parent) const override;
	QModelIndex parent(const QModelIndex &child) const override;

	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;

	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	bool hasChildren(const QModelIndex &parent) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

};


#endif
// CLIENTMODEL_HPP
