import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
	id: root
	Layout.fillWidth: true
	height: 55

	required property var selectedItem
	property bool showEditButton: false
	signal editRequested()

	RowLayout {
		anchors.fill: parent
		anchors.margins: 12
		spacing: 12

		Button {
			text: qsTr("Add Instance")
			visible: root.showEditButton
		}

		Item {
			Layout.fillWidth: true
		}

		Button {
			text: qsTr("View Folder")
			enabled: root.selectedItem !== null
			onClicked: {
				if (root.selectedItem) {
					Qt.openUrlExternally("file://" + selectedItem.id) 
				}
			}
		}

		Button {
			text: qsTr("Edit")
			visible: root.showEditButton
			enabled: root.selectedItem !== null
			onClicked: root.editRequested()
		}

		Button {
			text: qsTr("Kill")
			enabled: false
		}

		Button {
			text: qsTr("Launch")
			enabled: root.selectedItem !== null
			onClicked: {
				if (root.selectedItem) {
					LaunchHelper.handleLaunch(root.selectedItem.path)
				}
			}
		}
	}
}
