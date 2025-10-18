#ifndef TREEITEM_HPP
#define TREEITEM_HPP

// Qt
#include <QList>
#include <QVariant>

class TreeItem {
	  public:
		explicit TreeItem(QVariantList data, TreeItem *parentItem = nullptr);

		void appendChild(std::unique_ptr<TreeItem> &&child);

		TreeItem *child(int32_t row);
		int32_t childCount() const;
		int32_t columnCount() const;
		QVariant data(int32_t column) const;
		int32_t row() const;
		TreeItem *parentItem();

	  private:
		std::vector<std::unique_ptr<TreeItem>> m_childItems;

		QVariantList m_itemData;
		TreeItem *m_parentItem;
};

#endif // TREEITEM_HPP
