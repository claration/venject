import QtQuick
import QtCore
import QtQml
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import VencordLauncher

ApplicationWindow {
	id: root

	property var discordList: Launcher.findDiscords()
	property var selectedItem: null
	property bool standardCollapsed: false
	property bool flatpakCollapsed: false

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
			discordList = Launcher.findDiscords()
			if (selectedItem && !discordList.some(item => item.path === selectedItem.path)) {
				selectedItem = null
			}
		}
	}

	readonly property var sectionModel: [
		{
			name: qsTr("Config"),
			collapsed: root.standardCollapsed,
			items: discordList.filter(item => !item.isFlatpak),
			toggle: () => root.standardCollapsed = !root.standardCollapsed
		},
		{
			name: "Flatpak",
			collapsed: root.flatpakCollapsed,
			items: discordList.filter(item => item.isFlatpak),
			toggle: () => root.flatpakCollapsed = !root.flatpakCollapsed
		}
	]

	EditWindow {
		id: editWindow
		visible: false
		selectedItem: root.selectedItem
	}

	Settings {
		property alias x: root.x
		property alias y: root.y
		property alias width: root.width
		property alias height: root.height
	}

	MessageDialog {
		id: errorDialog
		title: "Vencord Launcher"
		buttons: MessageDialog.Ok
		property string errorMessage: ""
		text: errorMessage
	}

	MessageDialog {
		id: updateDialog
		title: qsTr("Vencord Update Available")
		buttons: MessageDialog.Yes | MessageDialog.No

		property string launchPath: ""
		property string updateInstalledHash: ""
		property string updateLatestHash: ""

		text: qsTr("A new version of Vencord is available.")
			+ "\n\n" + qsTr("Installed: %1", updateInstalledHash)
			+ "\n" + qsTr("Latest: %1", updateLatestHash)
			+ "\n\n" + qsTr("Would you like o update before launching?")
		// text: "A new version of Vencord is available.\n\n" + "Installed: "
		// 	+ updateInstalledHash + "\n" + "Latest: " + updateLatestHash + "\n\n"
		// 	+ "Would you like to update before launching?"

		onAccepted: {
			const result = Launcher.updateVencord()
			if (!result.success) {
				errorDialog.errorMessage = qsTr("Failed to update Vencord.") + "\n\n" + result.error
				errorDialog.open()
				return
			}
			Launcher.launchSelf(launchPath)
		}

		onRejected: {
			Launcher.launchSelf(launchPath)
		}
	}

	Connections {
		target: LaunchHelper

		function onHandleLaunchMessage(path: string) {
			const result = Launcher.checkForUpdates()

			if (!result.success) {
				errorDialog.errorMessage = qsTr("Failed to check for Vencord updates.") + "\n\n" + result.error
				errorDialog.open()
				return
			}

			if (result.upToDate) {
				Launcher.launchSelf(path)
				return
			}

			updateDialog.launchPath = path
			updateDialog.updateInstalledHash = result.installedHash
			updateDialog.updateLatestHash = result.latestHash
			updateDialog.open()
		}
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		Footer {
			id: bottomBarRoot
			Layout.fillWidth: true
			selectedItem: root.selectedItem

			showEditButton: true
			onEditRequested: {
				editWindow.selectedItem = root.selectedItem
				editWindow.show()
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
					model: root.sectionModel

					delegate: DiscordColumn {
						isVisible: modelData.items.length > 0
						width: parent.width
					}
				}
			}
		}

		Rectangle {
			Layout.fillWidth: true
			height: 1
			color: sysPalette.light
		}

		Control {
			padding: 4

			contentItem: RowLayout {
				spacing: 12

				Label {
					text: root.selectedItem
						? (selectedItem.branch
							+ (selectedItem.isFlatpak ? " (Flatpak)" : "")
							+ " — " + selectedItem.id)
						: qsTr("No installation selected")

					font.pixelSize: 12
					color: sysPalette.text
					Layout.fillWidth: true
					elide: Text.ElideRight
				}
			}
		}

	}
}
