#include "treeitem.hpp"
#include "logs.hpp"
// Tracy
#include <tracy/Tracy.hpp>

TreeItem::TreeItem(QVariantList data, TreeItem *parent)
	: m_itemData(std::move(data))
	, m_parentItem(parent) {
}

void TreeItem::appendChild(std::unique_ptr<TreeItem> &&child) {
		ZoneScoped;

		m_childItems.push_back(std::move(child));
}

TreeItem *TreeItem::child(size_t row) const {
		ZoneScoped;

		if (row >= 0 && row < childCount()) {
				return m_childItems.at(row).get();
		}

		logCreate("Row out of range: " + std::to_string(row));

		return nullptr;
}

size_t TreeItem::childCount() const {
		return m_childItems.size();
}

qsizetype TreeItem::columnCount() const {
		return m_itemData.count();
}

QVariant TreeItem::data(qsizetype column) const {
		return m_itemData.value(column);
}

TreeItem *TreeItem::parentItem() const {
		return m_parentItem;
}

void TreeItem::clearChildren() {
		m_childItems.clear();
}

int32_t TreeItem::row() const {
		ZoneScoped;

		if (m_parentItem == nullptr) {
				logCreate("Item is orphaned, has no parent");
				return 0;
		}

		// Szuka wskaźnika do obecnego TreeItem'u
		const auto IT = std::ranges::find_if(std::as_const(m_parentItem->m_childItems), [this](const std::unique_ptr<TreeItem> &treeItem) {
				return treeItem.get() == this;
		});

		// Określa i zwraca położenie tego TreeItemu w wektorze m_childItems
		if (IT != m_parentItem->m_childItems.cend()) {
				return static_cast<int32_t>(std::distance(m_parentItem->m_childItems.cbegin(), IT));
		}

		Q_ASSERT(false);
		return -1;
}