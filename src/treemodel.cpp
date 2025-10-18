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

		const auto *item = static_cast<const TreeItem *>(index.internalPointer());
		return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
		return index.isValid() ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}

QVariant TreeModel::headerData(int32_t section, Qt::Orientation orientation, int32_t role) const {
		return orientation == Qt::Horizontal && role == Qt::DisplayRole ? m_rootItem->data(section) : QVariant{};
}

QModelIndex TreeModel::index(int32_t row, int32_t column, const QModelIndex &parent) const {
		if (!hasIndex(row, column, parent)) {
				return {};
		}

		TreeItem *parentItem = parent.isValid() ? static_cast<TreeItem *>(parent.internalPointer()) : m_rootItem.get();

		if (auto *childItem = parentItem->child(row)) {
				return createIndex(row, column, childItem);
		}
		return {};
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
		if (!index.isValid()) {
				return {};
		}

		auto *childItem = static_cast<TreeItem *>(index.internalPointer());
		TreeItem *parentItem = childItem->parentItem();

		return parentItem != m_rootItem.get() ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}

int32_t TreeModel::rowCount(const QModelIndex &parent) const {
		if (parent.column() > 0) {
				return 0;
		}

		const TreeItem *parentItem = parent.isValid() ? static_cast<const TreeItem *>(parent.internalPointer()) : m_rootItem.get();

		return parentItem->childCount();
}

void TreeModel::setColumnsNames(const QVariantList &columnsNames) {
		m_rootItem = {std::make_unique<TreeItem>(columnsNames)};
}

void TreeModel::setupModelData(const std::vector<Track> &tracks, TreeItem *parent) {
		struct ParentIndentation {
				TreeItem *parent;
				qsizetype indentation;
		} __attribute__((aligned(16)));

		QList<ParentIndentation> state{
			{.parent = parent, .indentation = 0}
		};

		for (const auto &line : tracks) {
				qsizetype position = 0;

				QVariantList columnData;

				columnData << line.number;
				columnData << line.title;
				columnData << line.album;
				columnData << line.artist;
				columnData << line.duration;
				columnData << line.year;
				columnData << line.bitrate;
				columnData << line.fileSize;
				columnData << line.cover;
				columnData << line.path;

				if (position > state.constLast().indentation) {
						auto *lastParent = state.constLast().parent;
						if (lastParent->childCount() > 0) {
								state.append({lastParent->child(lastParent->childCount() - 1), position});
						}
				} else {
						while (position < state.constLast().indentation && !state.isEmpty()) {
								state.removeLast();
						}
				}

				auto *lastParent = state.constLast().parent;
				lastParent->appendChild(std::make_unique<TreeItem>(columnData, lastParent));
		}
}
