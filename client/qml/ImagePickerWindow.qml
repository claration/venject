import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Window {
	id: pickerWindow
	width: 320
	height: 400
	title: qsTr("Select an Image")
	modality: Qt.ApplicationModal
	visible: false

	SystemPalette {
		id: sysPalette
		colorGroup: SystemPalette.Active
	}

	color: sysPalette.window

	signal imageSelected(url imageSource)

	ColumnLayout {
		spacing: 0
		anchors.fill: parent

		ScrollView {
			Layout.fillWidth: true
			Layout.fillHeight: true
			clip: true

			GridView {
				id: gridView
				width: parent.width
				cellWidth: 80
				cellHeight: 80

				model: [
					"qrc:/VencordLauncher/resources/instances/Discord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordCanary." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordDevelopment." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/Discord+Vencord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordCanary+Vencord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordDevelopment+Vencord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/Discord+Equicord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordCanary+Equicord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordDevelopment+Equicord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/Vencord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/VencordContributor." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/Equicord." + LaunchInstanceWrapper.getIconType()
				]

				delegate: Item {
					width: gridView.cellWidth
					height: gridView.cellHeight

					Button {
						width: 60
						height: 60
						anchors.centerIn: parent

						contentItem: Image {
							width: 36
							height: 36
							anchors.centerIn: parent
							source: modelData
							fillMode: Image.PreserveAspectFit
							smooth: true
						}

						onClicked: {
							pickerWindow.imageSelected(modelData)
							pickerWindow.close()
						}
					}
				}
			}
		}

		RowLayout {
			Layout.margins: 12
			Layout.alignment: Qt.AlignRight
			spacing: 10

			Item {
				Layout.fillWidth: true
			}

			Button {
				Layout.alignment: Qt.AlignHCenter
				text: qsTr("Browse")
				icon.source: "qrc:/VencordLauncher/resources/glyphs/folder.png"
				icon.color: "transparent"
				onClicked: fileDialog.open()
			}

			Button {
				text: qsTr("Cancel")
				icon.source: "qrc:/VencordLauncher/resources/glyphs/cancel.png"
				icon.color: "transparent"
				onClicked: pickerWindow.close()
			}
		}
	}

	FileDialog {
		id: fileDialog
		title: qsTr("Select an Image")
		nameFilters: [LaunchInstanceWrapper.getSupportedIconTypes()]

		onAccepted: {
			pickerWindow.imageSelected(fileDialog.selectedFile)
			pickerWindow.close()
		}
	}
}
