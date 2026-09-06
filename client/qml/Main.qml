import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

ApplicationWindow {
    id: root

    property var discordList: launcher.findDiscords()
    property string pendingLaunchPath: ""

    visible: true
    width: 600
    height: 420

    title: "Vencord Launcher"

    onActiveChanged: {
        if (active) {
            discordList = launcher.findDiscords()
        }
    }

	MessageDialog {
		id: updateDialog

		title: "Vencord Update Available"
		text: "A new version of Vencord is available.\n\n" +
			"Installed: " + updateInstalledHash + "\n" +
			"Latest: " + updateLatestHash + "\n\n" +
			"Would you like to update before launching?"

		buttons: MessageDialog.Yes | MessageDialog.No

		property string launchPath: ""
		property string updateInstalledHash: ""
		property string updateLatestHash: ""

		onAccepted: {
			if (launcher.updateVencord()) {
				launcher.launch(launchPath)
			} else {
				console.log("failed")
			}
		}

		onRejected: {
			launcher.launch(launchPath)
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

                    anchors.verticalCenter: parent.verticalCenter

                    source: modelData.branch.toLowerCase() === "discord canary"
                        ? "qrc:/samaraqt/resources/canary.webp"
                        : "qrc:/samaraqt/resources/stable.webp"

                    fillMode: Image.PreserveAspectFit
                }

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
						const update = launcher.checkForUpdates()

						if (update.latestHash === "Unknown") {
							launcher.launch(modelData.path)
							return
						}

						if (update.upToDate) {
							launcher.launch(modelData.path)
							return
						}

						updateDialog.launchPath = modelData.path
						updateDialog.updateInstalledHash = update.installedHash
						updateDialog.updateLatestHash = update.latestHash
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
