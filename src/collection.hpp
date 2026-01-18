#ifndef COLLECTION_HPP
#define COLLECTION_HPP

#include <logs.hpp>
// Qt
#include <QList>
// STD
#include <utility>

template <typename ItemType>
class Collection {
	  public:
		Collection() = default;
		explicit Collection(QString name, QList<ItemType> items = {})
			: p_items(items)
			, p_name(std::move(name)) {
		}

		// Ustawia nazwę kolekcji
		void setName(const QString &name) {
				p_name = name;
		}

		// Pobiera nazwę kolekcji
		[[nodiscard]] QString getName() const {
				return p_name;
		}

		// Ustawia elementy kolekcji
		void setItems(QList<ItemType> &&items) {
				p_items = std::move(items);
		}

		// Pobiera elementy kolekcji
		QList<ItemType> getItems() const {
				return p_items;
		}

		// Pobiera jeden element z kolekcji
		ItemType getItem(qsizetype index) const {
				if (index < p_items.size()) {
						return p_items.at(index);
				}

				logCreate("Index out of range: " + std::to_string(index));

				return {};
		}

	  protected:
		QList<ItemType> p_items; // Elementy kolekcji
		QString p_name;          // Nazwa kolekcji
};

#endif // COLLECTIO_HPP
