import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import VencordLauncher

Window {
	id: addWindow

	property var discordList: DiscordInstallWrapper.getInstances()
	property var selectedDiscord: null
	property string selectedPreset: "none"

	width: 480
	height: 440
	visible: true

	title: qsTr("New Instance")

	SystemPalette {
		id: sysPalette
		colorGroup: SystemPalette.Active
	}

	color: sysPalette.window

	Component.onCompleted: {
		if (!selectedDiscord && discordList && discordList.length > 0) {
			selectedDiscord = discordList[0]
		}
	}

	onActiveChanged: {
		if (active) {
			let previousIdentifier = selectedDiscord ? selectedDiscord.uniqueIdentifier : null
			discordList = DiscordInstallWrapper.getInstances()

			if (previousIdentifier) {
				let foundItem = discordList.find(item => item.uniqueIdentifier === previousIdentifier)
				selectedDiscord = foundItem !== undefined ? foundItem : (discordList.length > 0 ? discordList[0] : null)
			} else if (!selectedDiscord && discordList.length > 0) {
				selectedDiscord = discordList[0]
			}
		}
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		Rectangle {
			Layout.fillWidth: true
			height: 1
			color: sysPalette.light
		}

		ColumnLayout {
			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.margins: 20
			spacing: 16

			ColumnLayout {
				Layout.fillWidth: true
				spacing: 6

				Label {
					text: qsTr("Instance Details")
					font.bold: true
					color: sysPalette.text
				}

				RowLayout {
					Layout.fillWidth: true
					spacing: 10

					TextField {
						id: nameField
						Layout.fillWidth: true
						placeholderText: addWindow.selectedDiscord
							? (addWindow.selectedDiscord.branch + (addWindow.selectedDiscord.isFlatpak ? " (flatpak)" : ""))
							: qsTr("Name")
					}

					TextField {
						id: groupField
						Layout.fillWidth: true
						placeholderText: qsTr("Group Name")
					}
				}
			}

			ColumnLayout {
				Layout.fillWidth: true
				spacing: 6

				Label {
					text: qsTr("Discord Installation")
					font.bold: true
					color: sysPalette.text
				}

				ComboBox {
					id: discordCombo
					Layout.fillWidth: true
					model: addWindow.discordList
					enabled: addWindow.discordList.length > 0

					currentIndex: {
						if (!addWindow.selectedDiscord) return -1
						return addWindow.discordList.findIndex(item => item.uniqueIdentifier === addWindow.selectedDiscord.uniqueIdentifier)
					}

					onActivated: (index) => {
						if (index >= 0 && index < model.length) {
							addWindow.selectedDiscord = model[index]
						} else {
							addWindow.selectedDiscord = null
						}
					}

					contentItem: DiscordRow {
						discordItem: addWindow.selectedDiscord
						textColor: sysPalette.text
					}

					delegate: ItemDelegate {
						required property int index
						required property var modelData
						width: discordCombo.width
						height: 32

						contentItem: DiscordRow {
							discordItem: modelData
							textColor: sysPalette.text
						}
						highlighted: discordCombo.highlightedIndex === index
					}
				}
			}

			ColumnLayout {
				Layout.fillWidth: true
				spacing: 6

				Label {
					text: qsTr("Preset")
					font.bold: true
					color: sysPalette.text
				}

				RowLayout {
					Layout.alignment: Qt.AlignHCenter
					spacing: 16

					PresetRow {
						presetId: "none"
						title: qsTr("None")
						symbol: "∅"
						isSelected: addWindow.selectedPreset === "none"
						sysPalette: sysPalette
						onClicked: addWindow.selectedPreset = "none"
					}

					PresetRow {
						presetId: "vencord"
						title: qsTr("Vencord")
						iconSource: "qrc:/VencordLauncher/resources/icon.png"
						isSelected: addWindow.selectedPreset === "vencord"
						sysPalette: sysPalette
						onClicked: addWindow.selectedPreset = "vencord"
					}
				}
			}

			Item {
				Layout.fillHeight: true
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
				text: qsTr("Cancel")
				onClicked: addWindow.close()
			}

			Button {
				text: qsTr("Create")
				highlighted: true
				enabled: addWindow.selectedDiscord !== null
				onClicked: {
					let instanceName = nameField.text.trim() !== "" ? nameField.text.trim() : nameField.placeholderText
					let groupName = groupField.text.trim()
					let selectedDiscord = addWindow.selectedDiscord
					let selectedPreset = addWindow.selectedPreset

					console.log(instanceName)
					console.log(groupName)
					console.log(selectedDiscord.uniqueIdentifier)
					console.log(selectedPreset)

					LaunchInstanceWrapper.createInstance(instanceName, groupName, selectedDiscord.uniqueIdentifier, selectedPreset)

					addWindow.close()
				}
			}
		}
	}
}
