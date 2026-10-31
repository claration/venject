import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Window {
	id: pickerWindow
	width: 380
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
		spacing: 15
		anchors.fill: parent
		anchors.margins: 15

		ScrollView {
			Layout.fillWidth: true
			Layout.fillHeight: true
			clip: true

			GridView {
				id: gridView
				width: parent.width
				cellWidth: 70
				cellHeight: 70

				model: [
					"qrc:/VencordLauncher/resources/instances/DiscordStable." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordCanary." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordDevelopment." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordStable+Vencord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordCanary+Vencord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordDevelopment+Vencord." + LaunchInstanceWrapper.getIconType(),
					"qrc:/VencordLauncher/resources/instances/DiscordStable+Equicord." + LaunchInstanceWrapper.getIconType(),
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

		Button {
			Layout.alignment: Qt.AlignHCenter
			text: qsTr("Browse")
			icon.source: "qrc:/VencordLauncher/resources/glyphs/folder.png"
			onClicked: fileDialog.open()
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
