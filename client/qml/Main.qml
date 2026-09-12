import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.settings
import QtQuick.Dialogs

ApplicationWindow {
	id: root

	property var discordList: launcher.findDiscords()
	property var selectedItem: null
	property bool standardCollapsed: false
	property bool flatpakCollapsed: false

	visible: true
	width: 650
	height: 500

	title: "Vencord Launcher"

	SystemPalette {
		id: sysPalette
		colorGroup: SystemPalette.Active
	}

	color: sysPalette.window

	onActiveChanged: {
		if (active) {
			discordList = launcher.findDiscords()
			if (selectedItem && !discordList.some(item => item.path === selectedItem.path)) {
				selectedItem = null
			}
		}
	}

	readonly property var sectionModel: [
		{
			name: "Config",
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

	function handleLaunch(path) {
		const result = launcher.checkForUpdates()

		if (!result.success) {
			errorDialog.errorMessage = "Failed to check for Vencord updates.\n\n" + result.error
			errorDialog.open()
			return
		}

		if (result.upToDate) {
			launcher.launch_self(path)
			return
		}

		updateDialog.launchPath = path
		updateDialog.updateInstalledHash = result.installedHash
		updateDialog.updateLatestHash = result.latestHash
		updateDialog.open()
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

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

					delegate: Column {
						required property var modelData
						width: parent.width
						spacing: 12
						visible: modelData.items.length > 0


						Item {
							id: sectionHeaderRoot
							width: parent.width
							height: 20

							RowLayout {
								anchors.fill: parent
								anchors.rightMargin: 16
								spacing: 10

								Label {
									text: (modelData.collapsed ? "▶ " : "▼ ") + modelData.name
									font.bold: true
									font.pixelSize: 13
									color: sysPalette.text
								}

								Rectangle {
									Layout.fillWidth: true
									height: 1
									color: sysPalette.button
								}
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onClicked: modelData.toggle()
							}
						}

						Flow {
							width: parent.width
							spacing: 16
							visible: !modelData.collapsed

							Repeater {
								model: modelData.items
								delegate: Item {
									id: cardRoot
									width: 110
									height: 95

									required property var modelData
									readonly property bool isSelected: root.selectedItem && root.selectedItem.path === modelData.path
									readonly property bool isHovered: mouseArea.containsMouse

									MouseArea {
										id: mouseArea
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: root.selectedItem = modelData
										onDoubleClicked: {
											root.selectedItem = modelData
											handleLaunch(modelData.path)
										}
									}

									ColumnLayout {
										anchors.centerIn: parent
										spacing: 6
										width: parent.width

										Item {
											Layout.alignment: Qt.AlignHCenter
											Layout.preferredWidth: 48
											Layout.preferredHeight: 48

											Rectangle {
												anchors.fill: parent
												radius: 6
												color: isSelected ? sysPalette.highlight : "transparent"
											}

											Image {
												anchors.centerIn: parent
												width: 38
												height: 38
												sourceSize: Qt.size(38, 38)
												source: {
													const branch = modelData.branch.toLowerCase()
													if (branch.includes("canary"))
														return "qrc:/VencordLauncher/resources/canary.svg"
													if (branch.includes("development") || branch.includes("ptb"))
														return "qrc:/VencordLauncher/resources/development.svg"
													return "qrc:/VencordLauncher/resources/stable.svg"
												}
												fillMode: Image.PreserveAspectFit
											}
										}

										Item {
											Layout.alignment: Qt.AlignHCenter
											Layout.preferredWidth: parent.width - 8
											Layout.preferredHeight: 24

											Rectangle {
												anchors.fill: parent
												radius: 4
												color: isSelected ? sysPalette.highlight : sysPalette.mid 
											}

											Label {
												anchors.centerIn: parent
												width: parent.width - 6
												horizontalAlignment: Text.AlignHCenter
												text: modelData.branch
												font.pixelSize: 13
												color: isSelected ? sysPalette.highlightedText : sysPalette.text
												elide: Text.ElideRight
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		Rectangle {
			Layout.fillWidth: true
			height: 1
			color: sysPalette.light
		}

		Item {
			id: bottomBarRoot
			Layout.fillWidth: true
			height: 55

			Rectangle {
				anchors.fill: parent
				color: sysPalette.window
			}

			RowLayout {
				anchors.fill: parent
				anchors.margins: 12
				spacing: 12

				Label {
					text: root.selectedItem ? root.selectedItem.branch : "No installation selected"
					font.pixelSize: 12
					color: sysPalette.text
					Layout.fillWidth: true
					elide: Text.ElideRight
				}

				Button {
					text: "Edit"
					enabled: root.selectedItem !== null
				}


				Button {
					text: "Kill"
					enabled: root.selectedItem !== null
				}

				Button {
					text: "Launch"
					enabled: root.selectedItem !== null
					onClicked: {
						if (root.selectedItem) {
							handleLaunch(root.selectedItem.path)
						}
					}
				}
			}
		}
	}
}
