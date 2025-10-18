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

		QVariant data(const QModelIndex &index, int32_t role) const override;
		Qt::ItemFlags flags(const QModelIndex &index) const override;
		QVariant headerData(int32_t section, Qt::Orientation orientation, int32_t role = Qt::DisplayRole) const override;
		QModelIndex index(int32_t row, int32_t column, const QModelIndex &parent = {}) const override;
		QModelIndex parent(const QModelIndex &index) const override;
		int32_t rowCount(const QModelIndex &parent = {}) const override;
		int32_t columnCount(const QModelIndex &parent = {}) const override;

		void setColumnsNames(const QVariantList &columnsNames);

	  private:
		static void setupModelData(const std::vector<Track> &tracks, TreeItem *parent);

		std::unique_ptr<TreeItem> m_rootItem;
};

#endif // TREEMODEL_HPP
