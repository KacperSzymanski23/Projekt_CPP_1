#include "middletreemodel.hpp"
// Qt
#include <QStringList>
// Tracy
#include <tracy/Tracy.hpp>

MiddleTreeModel::MiddleTreeModel(const QList<Library::Artist> &artists, const QString &columnName, QObject *parent)
	: TreeModel(QVariantList{columnName}, parent)
	, m_artists(artists) {

		initModel();
}

MiddleTreeModel::~MiddleTreeModel() = default;

void MiddleTreeModel::setupModelData(TreeItem *parent) {
		ZoneScopedN("setupMiddleModelData");

		for (const auto &artist : m_artists) {
				auto artistItem = std::make_unique<TreeItem>(QVariantList{artist.getName()}, parent);

				for (const auto &album : artist.getAlbumsList()) {
						artistItem->appendChild(std::make_unique<TreeItem>(QVariantList{album.getTitle()}, artistItem.get()));
				}
				parent->appendChild(std::move(artistItem));
		}
}
