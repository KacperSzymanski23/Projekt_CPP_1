#ifndef PLAYERTREEMODEL_HPP
#define PLAYERTREEMODEL_HPP

#include "library.hpp"
#include "treeitem.hpp"
#include "treemodel.hpp"
// Qt
#include <QAbstractItemModel>
#include <QList>
#include <QModelIndex>
#include <QVariant>

class PlayerTreeModel : public TreeModel {
		Q_OBJECT

	  public:
		Q_DISABLE_COPY_MOVE(PlayerTreeModel)

		explicit PlayerTreeModel(const QList<Library::TrackMetadata> &tracks, const QVariantList &columnsNames = {}, QObject *parent = nullptr);
		~PlayerTreeModel() override;

		void updateModelData(const QList<Library::TrackMetadata> &trackMetadatas);

	  protected:
		void setupModelData(TreeItem *parent) override; // Wypełnia model danymi z listy metadanych piosenek

	  private:
		QList<Library::TrackMetadata> m_tracks;
};

#endif // PLAYERTREEMODEL_HPP
