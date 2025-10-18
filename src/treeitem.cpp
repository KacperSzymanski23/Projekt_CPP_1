#include "treeitem.hpp"

TreeItem::TreeItem(QVariantList data, TreeItem *parent)
	: m_itemData(std::move(data))
	, m_parentItem(parent) {
}

void TreeItem::appendChild(std::unique_ptr<TreeItem> &&child) {
		m_childItems.push_back(std::move(child));
}

TreeItem *TreeItem::child(int32_t row) {
		return row >= 0 && row < childCount() ? m_childItems.at(row).get() : nullptr;
}

int32_t TreeItem::childCount() const {
		return static_cast<int32_t>(m_childItems.size());
}

int32_t TreeItem::columnCount() const {
		return static_cast<int32_t>(m_itemData.count());
}

QVariant TreeItem::data(int32_t column) const {
		return m_itemData.value(column);
}

TreeItem *TreeItem::parentItem() {
		return m_parentItem;
}

int32_t TreeItem::row() const {
		if (m_parentItem == nullptr) {
				return 0;
		}
		const auto IT = std::find_if(
			m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(),
			[this](const std::unique_ptr<TreeItem> &treeItem) { return treeItem.get() == this; }
		);

		if (IT != m_parentItem->m_childItems.cend()) {
				return std::distance(m_parentItem->m_childItems.cbegin(), IT);
		}
		Q_ASSERT(false);
		return -1;
}