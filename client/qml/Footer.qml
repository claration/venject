import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
	id: root
	Layout.fillWidth: true
	height: 55

	required property var selectedItem

	Rectangle {
		anchors.fill: parent
		color: sysPalette.window
	}

	RowLayout {
		anchors.fill: parent
		anchors.margins: 12
		spacing: 12

		Label {
			text: root.selectedItem ? root.selectedItem.branch : "No installation selected"
			font.pixelSize: 12
			color: sysPalette.text
			Layout.fillWidth: true
			elide: Text.ElideRight
		}

		Button {
			text: "Edit"
			enabled: root.selectedItem !== null
		}


		Button {
			text: "Kill"
			enabled: root.selectedItem !== null
		}

		Button {
			text: "Launch"
			enabled: root.selectedItem !== null
			onClicked: {
				if (root.selectedItem) {
					LaunchHelper.handleLaunch(root.selectedItem.path)
				}
			}
		}
	}
}
