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

void MiddleTreeModel::updateModelData(const QList<Library::Artist> &artists) {
		ZoneScopedN("updateMiddleModelData");

		m_artists = artists;

		initModel();
}

void MiddleTreeModel::setupModelData(TreeItem *parent) {
		ZoneScopedN("setupMiddleModelData");

		for (const auto &artist : m_artists) {
				auto artistItem = std::make_unique<TreeItem>(QVariantList{artist.getName()}, parent);

				// Dodaje listę albumów dla danego artyst jako elementy podrzędne dla elementu artysty
				for (const auto &album : artist.getItems()) {
						artistItem->appendChild(std::make_unique<TreeItem>(QVariantList{album.getName()}, artistItem.get()));
				}

				// Dodaje element artysty do modelu
				parent->appendChild(std::move(artistItem));
		}
}
