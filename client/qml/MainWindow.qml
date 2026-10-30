import QtCore
import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import VencordLauncher

ApplicationWindow {
	id: root

	property var instanceList: LaunchInstanceWrapper.getInstances()

	property var groupedInstances: {
		let groups = {}
		for (let i = 0; i < instanceList.length; ++i) {
			let inst = instanceList[i]
			let gName = inst.groupName && inst.groupName.length > 0 ? inst.groupName : qsTr("Default")
			if (!groups[gName]) {
				groups[gName] = {
					name: gName,
					items: []
				}
			}
			groups[gName].items.push(inst)
		}
		return Object.values(groups)
	}

	visible: true
	width: 650
	height: 500

	SystemPalette {
		id: sysPalette
		colorGroup: SystemPalette.Active
	}

	color: sysPalette.window

	onActiveChanged: {
		if (active) {
			refreshInstances()
		}
	}

	function refreshInstances() {
		instanceList = LaunchInstanceWrapper.getInstances()
	}

	Settings {
		property alias x: root.x
		property alias y: root.y
		property alias width: root.width
		property alias height: root.height
	}

	AddWindow {
		id: addWindow
		visible: false
	}

	EditWindow {
		id: editWindow
		visible: false
	}

	ValuePrompt {
		id: promptDialog
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		Rectangle {
			Layout.fillWidth: true
			height: 1
			color: sysPalette.light
		}

		Item {
			Layout.fillWidth: true
			Layout.fillHeight: true

			Column {
				anchors.centerIn: parent
				spacing: 8
				visible: root.groupedInstances.length === 0

				Label {
					text: qsTr("No instances added")
					font.pixelSize: 16
					font.bold: true
					color: sysPalette.text
					anchors.horizontalCenter: parent.horizontalCenter
				}

				Label {
					text: qsTr("Click 'Create Instance' below to get started.")
					font.pixelSize: 12
					color: sysPalette.placeholderText
					anchors.horizontalCenter: parent.horizontalCenter
				}
			}

			ScrollView {
				anchors.fill: parent
				clip: true
				contentWidth: availableWidth
				visible: root.groupedInstances.length > 0

				background: Rectangle {
					color: sysPalette.base
				}

				Column {
					width: parent.width
					spacing: 20
					padding: 16

					Repeater {
						model: root.groupedInstances

						delegate: InstanceColumn {
							isVisible: modelData.items.length > 0
							width: parent.width
						}
					}
				}
			}
		}

		Rectangle {
			Layout.fillWidth: true
			height: 1
			color: sysPalette.light
		}

		ColumnLayout {
			RowLayout {
				Layout.margins: 12
				spacing: 12

				Button {
					text: qsTr("Add Instance")
					icon.name: "list-add"
					icon.source: "qrc:/VencordLauncher/resources/glyphs/add.png"
					onClicked: {
						addWindow.show()
						addWindow.raise()
					}
				}

				Button {
					text: qsTr("Get Help")
					icon.name: "list-add"
					icon.source: "qrc:/VencordLauncher/resources/glyphs/help.png"
					onClicked: {
						Qt.openUrlExternally("https://discord.gg/vencord")
					}
				}

				Button {
					text: qsTr("Donate")
					icon.name: "list-add"
					icon.source: "qrc:/VencordLauncher/resources/glyphs/donate.png"
					onClicked: {
						Qt.openUrlExternally("https://github.com/sponsors/claration")
					}
				}
			}
		}
	}
}
