import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Window {
	id: shortcutWindow
	width: 380
	height: 180
	title: qsTr("Create Shortcut")
	modality: Qt.ApplicationModal
	flags: Qt.Window | Qt.WindowTitleHint | Qt.WindowCloseButtonHint

	SystemPalette {
		id: sysPalette
		colorGroup: SystemPalette.Active
	}

	color: sysPalette.window

	property var selectedItem: null

	function openFor(item) {
		selectedItem = item
		destinationCombo.currentIndex = 0
		show()
		raise()
	}

	FileDialog {
		id: saveFileDialog
		title: qsTr("Save Shortcut As")
		fileMode: FileDialog.SaveFile
		nameFilters: Qt.platform.os === "windows" ? [qsTr("Shortcut files (*.lnk)")] : [qsTr("Desktop files (*.desktop)")]
		currentFile: "file://" + StandardPaths.writableLocation(StandardPaths.DocumentsLocation) + "/" + (selectedItem ? selectedItem._name : "shortcut") + (Qt.platform.os === "windows" ? ".lnk" : ".desktop")

		onAccepted: {
			if (selectedItem && selectedFile) {
				let tempPath = selectedItem.createShortcut(LaunchInstance.Custom)
				if (tempPath) {
					let destPath = selectedFile.toString().replace("file://", "")
					if (Qt.platform.os === "windows" && destPath.startsWith("/")) {
						destPath = destPath.substring(1)
					}
					selectedItem.moveFile(tempPath, destPath)
					shortcutWindow.close()
				}
			}
		}
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: 16
		spacing: 16

		ComboBox {
			id: destinationCombo
			Layout.fillWidth: true
			model: [
				qsTr("Applications Menu"),
				qsTr("Desktop"),
				qsTr("Autostart (Windows Only)"),
				qsTr("Custom (Choose Location...)")
			]
		}

		Label {
			Layout.fillWidth: true
			text: qsTr("NOTE: Moving the shortcut from a default location will make the shortcut not be automatically deleted when deleting an instance!")
			wrapMode: Label.WordWrap
		}

		Item {
			Layout.fillHeight: true
		}

		RowLayout {
			Layout.alignment: Qt.AlignRight
			spacing: 10

			Item {
				Layout.fillWidth: true
			}

			Button {
				text: qsTr("Create")
				icon.source: "qrc:/VencordLauncher/resources/glyphs/folder.png"
				icon.color: "transparent"
				highlighted: true

				onClicked: {
					if (!selectedItem) return

					if (destinationCombo.currentIndex === 3) {
						saveFileDialog.open()
					} else {
						let targetLocation = LaunchInstance.Applications
						if (destinationCombo.currentIndex === 1) targetLocation = LaunchInstance.Desktop
						else if (destinationCombo.currentIndex === 2) targetLocation = LaunchInstance.Autostart

						let createdPath = selectedItem.createShortcut(targetLocation)
						if (createdPath) {
							shortcutWindow.close()
						}
					}
				}
			}

			Button {
				text: qsTr("Cancel")
				icon.source: "qrc:/VencordLauncher/resources/glyphs/cancel.png"
				icon.color: "transparent"
				onClicked: shortcutWindow.close()
			}
		}
	}
}
