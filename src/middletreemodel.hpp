#ifndef MIDDLETREEMODEL_HPP
#define MIDDLETREEMODEL_HPP

#include "library.hpp"
#include "treeitem.hpp"
#include "treemodel.hpp"
// Qt
#include <QList>
#include <QModelIndex>
#include <QVariant>

class MiddleTreeModel : public TreeModel {
		Q_OBJECT

	  public:
		Q_DISABLE_COPY_MOVE(MiddleTreeModel)

		explicit MiddleTreeModel(const QList<Library::Artist> &artists, const QString &columnName = {}, QObject *parent = nullptr);
		~MiddleTreeModel() override;

		void updateModelData(const QList<Library::Artist> &artists); // Aktualizuje dane w modelu

	  protected:
		void setupModelData(TreeItem *parent) override; // Wypełnia model danymi z listy metadanych piosenek

	  private:
		QList<Library::Artist> m_artists; // Lista artystów
};

#endif // MIDDLETREEMODEL_HPP
