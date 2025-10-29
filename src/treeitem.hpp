#ifndef TREEITEM_HPP
#define TREEITEM_HPP

// Qt
#include <QVariant>

class TreeItem {
	  public:
		explicit TreeItem(QVariantList data, TreeItem *parentItem = nullptr);

		void appendChild(std::unique_ptr<TreeItem> &&child);

		[[nodiscard]] TreeItem *child(int32_t row) const;
		[[nodiscard]] int32_t childCount() const;
		[[nodiscard]] int32_t columnCount() const;
		[[nodiscard]] QVariant data(int32_t column) const;
		[[nodiscard]] int32_t row() const;
		[[nodiscard]] TreeItem *parentItem() const;

	  private:
		std::vector<std::unique_ptr<TreeItem>> m_childItems;

		QVariantList m_itemData;
		TreeItem *m_parentItem;
};

#endif // TREEITEM_HPP
