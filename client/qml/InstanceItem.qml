import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Button {
	id: cardRoot
	width: 110
	height: 95

	required property var modelData

	contentItem: ColumnLayout {
		anchors.fill: parent

		ColumnLayout {
			Layout.alignment: Qt.AlignHCenter
			spacing: 2

			Item {
				Layout.alignment: Qt.AlignHCenter
				Layout.preferredWidth: 38
				Layout.preferredHeight: 38

				Image {
					anchors.fill: parent
					sourceSize: Qt.size(38, 38)
					source: "file:/" + modelData.iconPath
					fillMode: Image.PreserveAspectFit
				}
			}

			Label {
				Layout.alignment: Qt.AlignHCenter
				Layout.preferredWidth: parent.parent.width - 8
				horizontalAlignment: Text.AlignHCenter
				text: modelData.name
				font.pixelSize: 13
				color: sysPalette.text
				elide: Text.ElideRight
			}
		}
	}

	onClicked: {
		modelData.launch()
	}

	MouseArea {
		anchors.fill: parent
		acceptedButtons: Qt.RightButton
		onClicked: mouse => {
			if (mouse.button === Qt.RightButton) {
				contextMenu.popup()
			}
		}
	}

	Menu {
		id: contextMenu

		MenuItem {
			text: qsTr("Launch")
			icon.source: "qrc:/VencordLauncher/resources/glyphs/launch.png"
			onTriggered: {
				modelData.launch()
			}
		}

		MenuItem {
			text: qsTr("Create Shortcut")
			icon.source: "qrc:/VencordLauncher/resources/glyphs/shortcut.png"
			onTriggered: {
				modelData.createShortcut()
			}
		}

		MenuSeparator { }

		MenuItem {
			text: qsTr("View Folder")
			icon.source: "qrc:/VencordLauncher/resources/glyphs/folder.png"
			onTriggered: {
				if (Qt.platform.os == "windows") {
					Qt.openUrlExternally("file:///" + modelData.folderPath)
				} else {
					Qt.openUrlExternally("file://" + modelData.folderPath)
				}
			}
		}

		MenuSeparator { }

		MenuItem {
			text: qsTr("Edit...")
			icon.source: "qrc:/VencordLauncher/resources/glyphs/edit.png"
			onTriggered: {
				editWindow.openFor(modelData)
			}
		}

		MenuItem {
			text: qsTr("Change name...")
			icon.source: "qrc:/VencordLauncher/resources/glyphs/change.png"
			onTriggered: {
				promptDialog.targetItem = modelData
				promptDialog.activeEditType = "Name"
				promptDialog.initialValue = modelData.name
				promptDialog.open()
			}
		}

		MenuItem {
			text: qsTr("Change group...")
			icon.source: "qrc:/VencordLauncher/resources/glyphs/change.png"
			onTriggered: {
				promptDialog.targetItem = modelData
				promptDialog.activeEditType = "Group"
				promptDialog.initialValue = modelData.groupName ? modelData.groupName : ""
				promptDialog.open()
			}
		}

		MenuSeparator { }

		MenuItem {
			text: qsTr("Delete")
			icon.source: "qrc:/VencordLauncher/resources/glyphs/delete.png"
			onTriggered: {
				modelData.removeInstance()
				root.refreshInstances()
			}
		}
	}
}
