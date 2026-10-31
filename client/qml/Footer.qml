import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
	id: root
	Layout.fillWidth: true
	height: 55

	default property alias contentData: customItemsLayout.data
	required property var selectedItem

	Rectangle {
		anchors.fill: parent
		color: sysPalette.button
	}

	RowLayout {
		anchors.fill: parent
		anchors.margins: 12
		spacing: 12

		RowLayout {
			id: customItemsLayout
			Layout.fillHeight: true
			spacing: 12
		}

		Button {
			text: qsTr("View Folder")
			icon.source: "qrc:/VencordLauncher/resources/glyphs/folder.png"
			icon.color: "transparent"
			enabled: root.selectedItem !== null
			onClicked: {
				if (root.selectedItem) {
					if (Qt.platform.os == "windows") {
						Qt.openUrlExternally("file:///" + selectedItem.folderPath)
					} else {
						Qt.openUrlExternally("file://" + selectedItem.folderPath)
					}
				}
			}
		}

		Button {
			text: qsTr("Launch")
			icon.source: "qrc:/VencordLauncher/resources/glyphs/launch.png"
			icon.color: "transparent"
			enabled: root.selectedItem !== null
			onClicked: {
				if (root.selectedItem) {
					selectedItem.launch()
				}
			}
		}
	}
}
