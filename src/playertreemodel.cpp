#include "playertreemodel.hpp"
#include "treeitem.hpp"
// Qt
#include <QStringList>
// Tracy
#include <tracy/Tracy.hpp>

PlayerTreeModel::PlayerTreeModel(const QList<Library::TrackMetadata> &tracks, const QVariantList &columnsNames, QObject *parent)
	: TreeModel(columnsNames, parent)
	, m_tracks(tracks) {

		initModel();
}

PlayerTreeModel::~PlayerTreeModel() = default;

void PlayerTreeModel::setupModelData(TreeItem *parent) {
		ZoneScopedN("setupPlayerModelData");

		for (const auto &line : m_tracks) {
				QVariantList columnData;

				columnData << line.number;
				columnData << line.title;
				columnData << line.album;
				columnData << line.artist;
				columnData << line.duration;
				columnData << line.year;
				columnData << line.bitrate;
				columnData << line.fileSize;
				columnData << line.path;

				parent->appendChild(std::make_unique<TreeItem>(columnData, parent));
		}
}
