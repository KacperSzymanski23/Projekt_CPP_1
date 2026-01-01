#ifndef COLLECTION_HPP
#define COLLECTION_HPP

// Qt
#include <QList>

template <typename ItemType>
class Collection {
	  public:
		Collection() = default;
		explicit Collection(const QString &name, QList<ItemType> items = {})
			: p_items(items)
			, p_name(name) {
		}

		void setName(const QString &name) {
				p_name = name;
		}
		QString getName() const {
				return p_name;
		}

		void setItems(QList<ItemType> &&items) {
				p_items = std::move(items);
		}
		QList<ItemType> getItems() const {
				return p_items;
		}

		ItemType getItem(uint32_t index) const {
				if (index < p_items.size()) {
						return p_items.at(index);
				}

				return {};
		}

	  protected:
		QList<ItemType> p_items;
		QString p_name;
};

#endif // COLLECTIO_HPP
