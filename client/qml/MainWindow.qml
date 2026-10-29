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
	property var selectedItem: null

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
		if (selectedItem && !instanceList.some(item => item.id === selectedItem.id)) {
			selectedItem = null
		}
		console.log(instanceList)
	}

	function openEditWindow() {
		editWindow.show()
		editWindow.raise()
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
		selectedItem: root.selectedItem
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		Footer {
			id: bottomBarRoot
			Layout.fillWidth: true
			selectedItem: root.selectedItem

			Button {
				text: qsTr("Add Instance")
				icon.name: "list-add"
				icon.source: "qrc:/VencordLauncher/resources/glyphs/add.svg"
				onClicked: {
					addWindow.show()
				}
			}

			Item {
				Layout.fillWidth: true
			}

			Button {
				text: qsTr("Edit")
				icon.name: "edit-paste"
				icon.source: "qrc:/VencordLauncher/resources/glyphs/edit.svg"
				enabled: root.selectedItem !== null
				onClicked: openEditWindow()
			}
		}

		Rectangle {
			Layout.fillWidth: true
			height: 1
			color: sysPalette.light
		}

		ScrollView {
			Layout.fillWidth: true
			Layout.fillHeight: true
			clip: true
			contentWidth: availableWidth

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
}
