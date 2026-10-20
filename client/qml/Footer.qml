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

	Rectangle {
		anchors.fill: parent
		color: sysPalette.window
	}

	RowLayout {
		anchors.fill: parent
		anchors.margins: 12
		spacing: 12

		Label {
			text: root.selectedItem ? root.selectedItem.branch : qsTr("No installation selected")
			font.pixelSize: 12
			color: sysPalette.text
			Layout.fillWidth: true
			elide: Text.ElideRight
		}

		Button {
			text: qsTr("Edit")
			visible: root.showEditButton
			enabled: root.selectedItem !== null
			onClicked: root.editRequested()
		}

		Button {
			text: qsTr("Kill")
			enabled: root.selectedItem !== null
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
