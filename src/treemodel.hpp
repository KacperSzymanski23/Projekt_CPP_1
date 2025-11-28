#ifndef TREEMODEL_HPP
#define TREEMODEL_HPP

#include "track.hpp"
#include "treeitem.hpp"
// Qt
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeModel : public QAbstractItemModel {
		Q_OBJECT

	  public:
		Q_DISABLE_COPY_MOVE(TreeModel)

		explicit TreeModel(const std::vector<Track> &tracks, const QVariantList &columnsNames = {}, QObject *parent = nullptr);
		~TreeModel() override;

		[[nodiscard]] QVariant data(const QModelIndex &index, int32_t role) const override;                 // Zwraca daną zawartą w podanym indeksie
		[[nodiscard]] static QVariant dataAtColumn(const QModelIndex &index, int32_t role, int32_t column); // Zwraca daną w podanym inkdeksie i kolumnie
		[[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;                         // Zwraca flagi TreItem'u dla danego indeksu
		[[nodiscard]] QVariant headerData(int32_t section, Qt::Orientation orientation, int32_t role) const override; // Zwraca dane nagłówka
		[[nodiscard]] QModelIndex
		index(int32_t row, int32_t column, const QModelIndex &parent) const override; // Tworzy indeks dla elementu o podanym wierszu i kolumnie
		[[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;    // Zwraca indeks rodzica dla TreeIte'u o podanym indeksie
		[[nodiscard]] int32_t rowCount(const QModelIndex &parent) const override;     // Zwraca liczbę wierszy/dzieci dla podanego indeksu TreeItem
		[[nodiscard]] int32_t columnCount(const QModelIndex &parent) const override;  // Zwraca liczbę kolumn TreeItem'u

		void setColumnsNames(const QVariantList &columnsNames); // Ustawia nazwy kolumn

	  private:
		static void setupModelData(const std::vector<Track> &tracks, TreeItem *parent); // Wypełnia model danymi z listy metadanych piosenek

		std::unique_ptr<TreeItem> m_rootItem; // Wskaźnik do głównego TreeItem'u przechowującego wszystekie inne TreeItem
};

#endif // TREEMODEL_HPP
