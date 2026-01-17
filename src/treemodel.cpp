#include "treemodel.hpp"
#include "treeitem.hpp"
// Qt
#include <QStringList>
// Tracy
#include <tracy/Tracy.hpp>

TreeModel::TreeModel(const QVariantList &columnsNames, QObject *parent)
	: QAbstractItemModel(parent)
	, p_rootItem(std::make_unique<TreeItem>(columnsNames)) {
}

TreeModel::~TreeModel() = default;

int32_t TreeModel::columnCount(const QModelIndex &parent) const {
		ZoneScoped;

		if (parent.isValid()) {
				return static_cast<TreeItem *>(parent.internalPointer())->columnCount();
		}
		return p_rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int32_t role) const {
		ZoneScoped;

		if (!index.isValid() || role != Qt::DisplayRole) {
				return {};
		}

		const TreeItem *item = static_cast<const TreeItem *>(index.internalPointer());
		return item->data(index.column());
}

QVariant TreeModel::data(const QModelIndex &index, int32_t role, int32_t column) {
		ZoneScoped;

		if (!index.isValid() || role != Qt::DisplayRole) {
				return {};
		}

		const TreeItem *item = static_cast<const TreeItem *>(index.internalPointer());
		return item->data(column);
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
		ZoneScoped;

		if (index.isValid()) {
				return QAbstractItemModel::flags(index);
		}
		return {Qt::NoItemFlags};
}

QVariant TreeModel::headerData(int32_t section, Qt::Orientation orientation, int32_t role) const {
		ZoneScoped;

		if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
				return p_rootItem->data(section);
		}
		return {};
}

QModelIndex TreeModel::index(int32_t row, int32_t column, const QModelIndex &parent) const {
		ZoneScoped;

		if (!hasIndex(row, column, parent)) {
				return {};
		}

		TreeItem *parentItem{};

		if (parent.isValid()) {
				parentItem = static_cast<TreeItem *>(parent.internalPointer());
		} else {
				parentItem = p_rootItem.get();
		}

		if (TreeItem *childItem = parentItem->child(row)) {
				return createIndex(row, column, childItem);
		}
		return {};
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
		ZoneScoped;

		if (!index.isValid()) {
				return {};
		}

		TreeItem *childItem = static_cast<TreeItem *>(index.internalPointer());
		TreeItem *parentItem = childItem->parentItem();

		if (parentItem != p_rootItem.get()) {
				return createIndex(parentItem->row(), 0, parentItem);
		}
		return {};
}

int32_t TreeModel::rowCount(const QModelIndex &parent) const {
		ZoneScoped;

		if (parent.column() > 0) {
				return 0;
		}

		const TreeItem *parentItem{};

		if (parent.isValid()) {
				parentItem = static_cast<const TreeItem *>(parent.internalPointer());
		} else {
				parentItem = p_rootItem.get();
		}

		return static_cast<int32_t>(parentItem->childCount());
}

void TreeModel::setColumnsNames(const QVariantList &columnsNames) {
		beginResetModel();
		p_rootItem = std::make_unique<TreeItem>(columnsNames);
		endResetModel();
}

void TreeModel::initModel() {
		beginResetModel();

		if (p_rootItem != nullptr) {
				p_rootItem->clearChildren();
		}

		setupModelData(p_rootItem.get());
		endResetModel();
}
