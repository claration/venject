import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

ApplicationWindow {
	id: root

	property var discordList: launcher.findDiscords()

	visible: true
	width: 600
	height: 420
	minimumWidth: 600
	minimumHeight: 420

	title: "Vencord Launcher"

	onActiveChanged: {
		if (active)
			discordList = launcher.findDiscords()
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

		title: "Vencord Update Available"

		buttons: MessageDialog.Yes | MessageDialog.No

		property string launchPath: ""
		property string updateInstalledHash: ""
		property string updateLatestHash: ""

		text: "A new version of Vencord is available.\n\n" + "Installed: "
			  + updateInstalledHash + "\n" + "Latest: " + updateLatestHash + "\n\n"
			  + "Would you like to update before launching?"

		onAccepted: {
			const result = launcher.updateVencord()

			if (!result.success) {
				errorDialog.errorMessage = "Failed to update Vencord.\n\n" + result.error

				errorDialog.open()
				return
			}

			launcher.launch_self(launchPath)
		}

		onRejected: {
			launcher.launch_self(launchPath)
		}
	}

	ListView {
		id: listView

		anchors.fill: parent
		spacing: 4
		clip: true

		model: root.discordList

		delegate: Rectangle {
			width: listView.width
			height: 68
			color: "#00000000"

			Row {
				anchors.fill: parent
				anchors.leftMargin: 12
				anchors.rightMargin: 12
				spacing: 12

				Image {
					width: 44
					height: 44
					sourceSize: Qt.size(44, 44)

					anchors.verticalCenter: parent.verticalCenter

					source: modelData.branch.toLowerCase() === "discord canary"
							? "qrc:/VencordLauncher/resources/canary.svg" : modelData.branch.toLowerCase()
							  === "discord development" ? "qrc:/VencordLauncher/resources/development.svg" :
														  "qrc:/VencordLauncher/resources/stable.svg"

					fillMode: Image.PreserveAspectFit
				}
				// TODO: detect whether an injected discord is currently opened
				Column {
					width: parent.width - 44 - launchButton.width - 24

					anchors.verticalCenter: parent.verticalCenter
					spacing: 2

					Label {
						width: parent.width

						text: modelData.branch

						font.bold: true
						font.pixelSize: 14

						elide: Text.ElideRight
					}

					Label {
						width: parent.width

						text: modelData.path

						color: "#888"
						font.pixelSize: 11

						elide: Text.ElideMiddle
					}
				}

				Button {
					id: launchButton

					width: 90
					height: 36

					anchors.verticalCenter: parent.verticalCenter

					text: "Launch"

					onClicked: {
						const result = launcher.checkForUpdates()

						if (!result.success) {
							errorDialog.errorMessage = "Failed to check for Vencord updates.\n\n"
									+ result.error

							errorDialog.open()
							return
						}

						if (result.upToDate) {
							launcher.launch_self(modelData.path)
							return
						}

						updateDialog.launchPath = modelData.path
						updateDialog.updateInstalledHash = result.installedHash
						updateDialog.updateLatestHash = result.latestHash

						updateDialog.open()
					}
				}
			}
		}

		ScrollBar.vertical: ScrollBar {
			policy: ScrollBar.AsNeeded
		}
	}
}
