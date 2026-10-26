import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Column {
	required property var modelData
	required property bool isVisible
	spacing: 12
	visible: isVisible

	Item {
		id: sectionHeaderRoot
		width: parent.width
		height: 20

		RowLayout {
			anchors.fill: parent
			anchors.rightMargin: 16
			spacing: 10

			Label {
				text: modelData.name
				font.bold: true
				font.pixelSize: 13
				color: sysPalette.text
			}

			Rectangle {
				Layout.fillWidth: true
				height: 1
				color: sysPalette.button
			}
		}
	}

	Flow {
		width: parent.width
		spacing: 16
		visible: !modelData.collapsed

		Repeater {
			model: modelData.items
			delegate: InstanceItem {
				isSelected: root.selectedItem !== null && root.selectedItem.id === modelData.id
			}
		}
	}
}
