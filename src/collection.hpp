#ifndef COLLECTION_HPP
#define COLLECTION_HPP

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

		void setName(const QString &name) {
				p_name = name;
		}
		[[nodiscard]] QString getName() const {
				return p_name;
		}

		void setItems(QList<ItemType> &&items) {
				p_items = std::move(items);
		}
		QList<ItemType> getItems() const {
				return p_items;
		}

		ItemType getItem(qsizetype index) const {
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
