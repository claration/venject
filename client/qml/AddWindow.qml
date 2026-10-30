import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import VencordLauncher

Window {
	id: addWindow

	property var discordList: DiscordInstallWrapper.getInstances()
	property var selectedDiscord: null
	property string selectedPreset: "vencord"
	property bool hasCustomImage: false
	property url manualImageSource: ""

	property url currentImageSource: {
		if (hasCustomImage) {
			return manualImageSource
		}

		if (selectedPreset === "vencord") {
			return "qrc:/VencordLauncher/resources/instances/Vencord." + LaunchInstanceWrapper.getIconType()
		} else if (selectedPreset === "equicord") {
			return "qrc:/VencordLauncher/resources/instances/Equicord." + LaunchInstanceWrapper.getIconType()
		}

		let iconPath = "qrc:/VencordLauncher/resources/instances/DiscordStable." + LaunchInstanceWrapper.getIconType()
		if (selectedDiscord && selectedDiscord.branch) {
			let branchLower = selectedDiscord.branch.toLowerCase()
			if (branchLower.includes("canary")) {
				iconPath = "qrc:/VencordLauncher/resources/instances/DiscordCanary." + LaunchInstanceWrapper.getIconType()
			} else if (branchLower.includes("development")) {
				iconPath = "qrc:/VencordLauncher/resources/instances/DiscordDevelopment." + LaunchInstanceWrapper.getIconType()
			}
		}
		return iconPath
	}

	width: 520
	height: 420
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

	ImagePickerWindow {
		id: imagePickerWindow

		onImageSelected: (imageSource) => {
			manualImageSource = imageSource
			hasCustomImage = true
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

			RowLayout {
				Layout.fillWidth: true
				spacing: 14

				Button {
					Layout.preferredWidth: 75
					Layout.preferredHeight: 75
					Layout.alignment: Qt.AlignTop

					contentItem: Item {
						Image {
							width: 36
							height: 36
							anchors.centerIn: parent
							source: currentImageSource
							fillMode: Image.PreserveAspectFit
							smooth: true
						}
					}

					MouseArea {
						anchors.fill: parent
						acceptedButtons: Qt.LeftButton | Qt.RightButton
						onClicked: (mouse) => {
							if (mouse.button === Qt.RightButton) {
								hasCustomImage = false
								manualImageSource = ""
							} else {
								imagePickerWindow.show()
								imagePickerWindow.raise()
							}
						}
					}
				}

				ColumnLayout {
					Layout.fillWidth: true
					spacing: 8

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

			RowLayout {
				Layout.fillWidth: true
				Layout.preferredHeight: 140
				spacing: 16

				ColumnLayout {
					Layout.fillWidth: true
					Layout.fillHeight: true
					spacing: 6

					Label {
						text: qsTr("Discord Installation")
						font.bold: true
						font.pixelSize: 16
					}

					ScrollView {
						Layout.fillWidth: true
						Layout.fillHeight: true
						clip: true

						background: Rectangle {
							color: sysPalette.base
							border.color: sysPalette.light
							border.width: 1.17
							radius: 6
						}

						ListView {
							id: discordListView
							width: parent.width
							model: addWindow.discordList
							currentIndex: {
								if (!addWindow.selectedDiscord) return -1
								return addWindow.discordList.findIndex(item => item.uniqueIdentifier === addWindow.selectedDiscord.uniqueIdentifier)
							}

							delegate: ItemDelegate {
								required property int index
								required property var modelData
								width: discordListView.width
								height: 46

								highlighted: addWindow.selectedDiscord && addWindow.selectedDiscord.uniqueIdentifier === modelData.uniqueIdentifier

								contentItem: DiscordRow {
									discordItem: modelData
									textColor: sysPalette.text
								}

								background: Rectangle {
									color: parent.highlighted
										? sysPalette.highlight
										: (parent.hovered ? Qt.rgba(sysPalette.highlight.r, sysPalette.highlight.g, sysPalette.highlight.b, 0.25) : "transparent")
									radius: 4
								}

								onClicked: {
									addWindow.selectedDiscord = modelData
								}
							}
						}
					}

				}

				ColumnLayout {
					Layout.preferredWidth: 50
					Layout.fillHeight: true
					spacing: 6

					Label {
						text: qsTr("Mod Loader")
						font.bold: true
						font.pixelSize: 16
					}

					ColumnLayout {
						Layout.fillWidth: true
						spacing: 2

						Repeater {
							model: [
								{ id: "none", label: qsTr("None") },
								{ id: "vencord", label: qsTr("Vencord") },
								{ id: "equicord", label: qsTr("Equicord") }
							]

							delegate: RadioButton {
								Layout.fillWidth: true
								text: modelData.label
								checked: addWindow.selectedPreset === modelData.id
								onToggled: if (checked) addWindow.selectedPreset = modelData.id
							}
						}
					}

					Item {
						Layout.fillHeight: true
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
					let selectedIcon = addWindow.currentImageSource

					LaunchInstanceWrapper.createInstance(
						instanceName,
						groupName,
						selectedIcon,
						selectedDiscord.uniqueIdentifier,
						selectedPreset
					)

					addWindow.close()
				}
			}
		}
	}
}
