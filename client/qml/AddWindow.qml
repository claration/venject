import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import VencordLauncher

Window {
	id: addWindow

	property var discordList: []
	property var selectedDiscord: null
	property string selectedPreset: "vencord"
	property bool hasCustomImage: false
	property url manualImageSource: ""

	// Reset
	function resetForm() {
		discordList = DiscordInstallWrapper.getInstances()

		let firstUnpatched = discordList.find(item => !item.isPatched)
		selectedDiscord = firstUnpatched !== undefined ? firstUnpatched : (discordList.length > 0 ? discordList[0] : null)

		selectedPreset = "vencord"
		hasCustomImage = false
		manualImageSource = ""

		if (nameField) nameField.text = ""
		if (groupField) groupField.text = ""
	}

	property url currentImageSource: {
		if (hasCustomImage) {
			return manualImageSource
		}

		let discordPrefix = selectedDiscord.branc
		let iconName = discordPrefix
		if (selectedPreset === "vencord") {
			iconName = discordPrefix + "+Vencord"
		} else if (selectedPreset === "equicord") {
			iconName = discordPrefix + "+Equicord"
		}

		return "qrc:/VencordLauncher/resources/instances/" + iconName + "." + LaunchInstanceWrapper.getIconType()
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
		resetForm()
	}

	onVisibleChanged: {
		if (visible) {
			resetForm()
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
							width: 56
							height: 56
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
								let index = addWindow.discordList.findIndex(item => item.uniqueIdentifier === addWindow.selectedDiscord.uniqueIdentifier)
								if (index !== -1 && addWindow.discordList[index].isPatched) {
									return -1
								}
								return index
							}

							delegate: ItemDelegate {
								required property int index
								required property var modelData
								width: discordListView.width
								height: 46

								enabled: !modelData.isPatched
								opacity: enabled ? 1.0 : 0.5

								highlighted: addWindow.selectedDiscord && addWindow.selectedDiscord.uniqueIdentifier === modelData.uniqueIdentifier

								contentItem: DiscordRow {
									discordItem: modelData
									textColor: sysPalette.text
								}

								background: Rectangle {
									color: !parent.enabled ? "transparent" : (parent.highlighted
										? sysPalette.highlight
										: (parent.hovered ? Qt.rgba(sysPalette.highlight.r, sysPalette.highlight.g, sysPalette.highlight.b, 0.25) : "transparent"))
									radius: 4
								}

								onClicked: {
									if (!modelData.isPatched) {
										addWindow.selectedDiscord = modelData
									}
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
				icon.source: "qrc:/VencordLauncher/resources/glyphs/cancel.png"
				onClicked: addWindow.close()
			}

			Button {
				text: qsTr("OK")
				icon.source: "qrc:/VencordLauncher/resources/glyphs/ok.png"
				highlighted: true
				enabled: addWindow.selectedDiscord !== null && !addWindow.selectedDiscord.isPatched
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
