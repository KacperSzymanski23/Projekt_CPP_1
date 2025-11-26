#ifndef TREEITEM_HPP
#define TREEITEM_HPP

// Qt
#include <QVariant>

class TreeItem {
	  public:
		explicit TreeItem(QVariantList data, TreeItem *parentItem = nullptr);

		void appendChild(std::unique_ptr<TreeItem> &&child); // Dodaje na koniec wektora m_childItems kolejny TreeItem

		[[nodiscard]] TreeItem *child(int32_t row) const; // Zwraca wskaźnik do "dzieci" TreeItem w
		[[nodiscard]] int32_t childCount() const; // Zwaraca liczbę "dzieci" tego TreeItem
		[[nodiscard]] int32_t columnCount() const; // Zwraca liczbę kolumn
		[[nodiscard]] QVariant data(int32_t column) const; // Zwraca daną w podanej w argumęcie kolumnę
		[[nodiscard]] int32_t row() const; // Zwraca pozycję elementu w strukturze drzewa TreeItem'ów względem jego rodzica
		[[nodiscard]] TreeItem *parentItem() const; // Zwraca "rodzica" tego TreeItem'u

	  private:
		std::vector<std::unique_ptr<TreeItem>> m_childItems; // std::vector z wskaźnikiem typy unique do TreeItem

		QVariantList m_itemData; // Lista QVariant'ów z danymi
		TreeItem *m_parentItem; // Rodzic obecnego TreeItemu
};

#endif // TREEITEM_HPP
