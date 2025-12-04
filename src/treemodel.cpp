#include "treemodel.hpp"
#include "treeitem.hpp"
// Qt
#include <QStringList>

TreeModel::TreeModel(const std::vector<Track> &tracks, const QVariantList &columnsNames, QObject *parent)
	: QAbstractItemModel(parent)
	, m_rootItem(std::make_unique<TreeItem>(columnsNames)) {

		setupModelData(tracks, m_rootItem.get());
}

TreeModel::~TreeModel() = default;

int32_t TreeModel::columnCount(const QModelIndex &parent) const {
		if (parent.isValid()) {
				return static_cast<TreeItem *>(parent.internalPointer())->columnCount();
		}
		return m_rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int32_t role) const {
		if (!index.isValid() || role != Qt::DisplayRole) {
				return {};
		}

		const TreeItem *item = static_cast<const TreeItem *>(index.internalPointer());
		return item->data(index.column());
}

QVariant TreeModel::dataAtColumn(const QModelIndex &index, int32_t role, int32_t column) {
		if (!index.isValid() || role != Qt::DisplayRole) {
				return {};
		}

		const TreeItem *item = static_cast<const TreeItem *>(index.internalPointer());
		return item->data(column);
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
		if (index.isValid()) {
				return QAbstractItemModel::flags(index);
		}
		return {Qt::NoItemFlags};
}

QVariant TreeModel::headerData(int32_t section, Qt::Orientation orientation, int32_t role) const {
		if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
				return m_rootItem->data(section);
		}
		return {};
}

QModelIndex TreeModel::index(int32_t row, int32_t column, const QModelIndex &parent) const {
		if (!hasIndex(row, column, parent)) {
				return {};
		}

		TreeItem *parentItem{};

		if (parent.isValid()) {
				parentItem = static_cast<TreeItem *>(parent.internalPointer());
		} else {
				parentItem = m_rootItem.get();
		}

		if (TreeItem *childItem = parentItem->child(row)) {
				return createIndex(row, column, childItem);
		}
		return {};
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
		if (!index.isValid()) {
				return {};
		}

		TreeItem *childItem = static_cast<TreeItem *>(index.internalPointer());
		TreeItem *parentItem = childItem->parentItem();

		if (parentItem != m_rootItem.get()) {
				return createIndex(parentItem->row(), 0, parentItem);
		}
		return {};
}

int32_t TreeModel::rowCount(const QModelIndex &parent) const {
		if (parent.column() > 0) {
				return 0;
		}

		const TreeItem *parentItem{};

		if (parent.isValid()) {
				parentItem = static_cast<const TreeItem *>(parent.internalPointer());
		} else {
				parentItem = m_rootItem.get();
		}

		return parentItem->childCount();
}

void TreeModel::setColumnsNames(const QVariantList &columnsNames) {
		m_rootItem = std::make_unique<TreeItem>(columnsNames);
}

void TreeModel::setupModelData(const std::vector<Track> &tracks, TreeItem *parent) {
		struct ParentIndentation {
				TreeItem *parent;
				qsizetype indentation;
		} __attribute__((aligned(16)));

		constexpr qsizetype POSITION_ZERO{0};

		QList<ParentIndentation> state{
			{.parent = parent, .indentation = POSITION_ZERO}
		};

		for (const auto &line : tracks) {
				QVariantList columnData;

				columnData << line.number;
				columnData << line.title;
				columnData << line.album;
				columnData << line.artist;
				columnData << line.duration;
				columnData << line.year;
				columnData << line.bitrate;
				columnData << line.fileSize;
				columnData << line.path;
				columnData << line.coverArtPath;

				if (state.constLast().indentation < POSITION_ZERO) {
						TreeItem *lastParent = state.constLast().parent;
						if (lastParent->childCount() > 0) {
								state.append({.parent = lastParent->child(lastParent->childCount() - 1), .indentation = POSITION_ZERO});
						}
				}

				TreeItem *lastParent = state.constLast().parent;
				lastParent->appendChild(std::make_unique<TreeItem>(columnData, lastParent));
		}
}
