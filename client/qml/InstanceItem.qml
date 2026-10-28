import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
	id: cardRoot
	width: 110
	height: 95

	required property var modelData
	required property bool isSelected
	readonly property bool isHovered: mouseArea.containsMouse

	Menu {
		id: contextMenu

		MenuItem {
			text: qsTr("Launch")
			icon.name: "media-playback-start"
			icon.source: "qrc:/VencordLauncher/resources/glyphs/play.svg"
			onTriggered: {
				if (root.selectedItem) {
					root.selectedItem.launch()
				}
			}
		}

		MenuItem {
			text: qsTr("Edit...")
			icon.name: "edit-paste"
			icon.source: "qrc:/VencordLauncher/resources/glyphs/edit.svg"
			onTriggered: {
				if (root.selectedItem) {
					root.openEditWindow()
				}
			}
		}

		MenuItem {
			text: qsTr("Delete")
			icon.name: "edit-paste"
			icon.source: "qrc:/VencordLauncher/resources/glyphs/delete.svg"
			onTriggered: {
				if (root.selectedItem) {
					root.selectedItem.removeInstance()
					root.refreshInstances()
				}
			}
		}

		MenuItem {
			text: qsTr("Create Shortcut")
			icon.name: "edit-paste"
			icon.source: "qrc:/VencordLauncher/resources/glyphs/add.svg"
			onTriggered: {
				if (root.selectedItem) {
					root.selectedItem.createShortcut()
				}
			}
		}
	}

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		hoverEnabled: true
		cursorShape: Qt.PointingHandCursor
		acceptedButtons: Qt.LeftButton | Qt.RightButton

		onClicked: mouse => {
			root.selectedItem = modelData

			if (mouse.button === Qt.RightButton) {
				contextMenu.popup()
			}
		}

		onDoubleClicked: {
			root.selectedItem = modelData
			modelData.launch()
		}
	}

	ColumnLayout {
		anchors.centerIn: parent
		spacing: 6
		width: parent.width

		Item {
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: 48
			Layout.preferredHeight: 48

			Rectangle {
				anchors.fill: parent
				radius: 6
				color: isSelected ? sysPalette.highlight : "transparent"
			}

			Image {
				anchors.centerIn: parent
				width: 38
				height: 38
				sourceSize: Qt.size(38, 38)
				source: "qrc:/VencordLauncher/resources/icon.png"
				fillMode: Image.PreserveAspectFit
			}
		}

		Item {
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: parent.width - 8
			Layout.preferredHeight: 24

			Rectangle {
				anchors.fill: parent
				radius: 4
				color: isSelected ? sysPalette.highlight : sysPalette.mid
			}

			Label {
				anchors.centerIn: parent
				width: parent.width - 6
				horizontalAlignment: Text.AlignHCenter
				text: modelData.name
				font.pixelSize: 13
				color: isSelected ? sysPalette.highlightedText : sysPalette.text
				elide: Text.ElideRight
			}
		}
	}
}
