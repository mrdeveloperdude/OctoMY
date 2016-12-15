#ifndef CLIENTMODEL_HPP
#define CLIENTMODEL_HPP



#include <QAbstractItemModel>
#include <QFileIconProvider>
#include <QIcon>
#include <QVector>
#include <QMap>

class Client;
class ClientDirectory;

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
		}
		Node *parent;
		QVector<Node> *children;
	};

private:

	ClientDirectory *clients;

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
	explicit ClientModel(ClientDirectory *, QObject *parent = nullptr);
	virtual ~ClientModel();

public:

	QModelIndex index(int row, int column, const QModelIndex &parent) const Q_DECL_OVERRIDE;
	QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;

	int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

	bool hasChildren(const QModelIndex &parent) const Q_DECL_OVERRIDE;
	Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

};


#endif // CLIENTMODEL_HPP
