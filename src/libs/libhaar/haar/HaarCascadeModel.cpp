#include "HaarCascadeModel.hpp"

#include "CameraProcessor.hpp"
#include "HaarRendering.hpp"

#include <QPixmap>

HaarCascadeModel::HaarCascadeModel(QObject *parent) : QAbstractListModel(parent) {}

int HaarCascadeModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return m_cascades.size();
}
QVariant HaarCascadeModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid() || index.row() >= m_cascades.size())
		return QVariant();
	
	const auto &cascade = m_cascades[index.row()];
	
	switch (role) {
	case Qt::DisplayRole:
	case NameRole:
		return cascade->name; // Return the cascade's name
	case Qt::DecorationRole:
	case ImageRole:
		return QPixmap::fromImage(cascade->preview); // Return the cascade's preview image
	case FilenameRole:
		return cascade->filename; // Return the cascade's filename
	default:
		return QVariant();
	}
}

void HaarCascadeModel::addCascade(QSharedPointer<HaarCascade> cascade) {
	if (!cascade)
		return;
	
	// Generate preview for the cascade
	cascade->preview = QImage(CameraProcessor::PREVIEW_SIZE, CameraProcessor::PREVIEW_SIZE, QImage::Format_ARGB32);
	previewHaarCascade(cascade->preview, *cascade);
	
	beginInsertRows(QModelIndex(), m_cascades.size(), m_cascades.size());
	m_cascades.append(cascade);
	endInsertRows();
}


QSharedPointer<HaarCascade> HaarCascadeModel::cascadeAt(int index) const {
	if (index >= 0 && index < m_cascades.size()) {
		return m_cascades.at(index);
	}
	return nullptr;
}
