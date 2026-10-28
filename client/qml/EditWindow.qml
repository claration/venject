import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import VencordLauncher

Window {
	id: editWindow

	width: 600
	height: 480
	visible: true

	property var selectedItem: null

	property string lastLogContents: ""
	property var envModel: []

	title: selectedItem ? selectedItem.name : qsTr("Edit Instance")

	SystemPalette {
		id: sysPalette
		colorGroup: SystemPalette.Active
	}

	color: sysPalette.window

	function loadData() {
		if (!selectedItem) return
		launchArgsField.text = selectedItem.launchArguments || ""
		entryPointField.text = selectedItem.entryPointPath || ""
		notesField.text = selectedItem.notes || ""
		enableLogging.checked = selectedItem.enableLogging || false
		enableLoggingWrap.checked = selectedItem.enableLoggingWrap || false
		updaterGithubUrlField.text = selectedItem.updaterGithubUrl || ""
		updaterFilesField.text = selectedItem.updaterFiles || ""
		updaterOutputPathField.text = selectedItem.updaterFileOutputPath || ""

		let envs = selectedItem.environmentVariables || {}
		let arr = []
		for (let key in envs) {
			arr.push({ key: key, value: envs[key] })
		}
		envModel = arr
		updateLogPath()
	}

	function updateLogPath() {
		lastLogContents = ""
		logField.clear()
		if (enableLogging.checked) pollLog()
	}

	function pollLog() {
		if (!enableLogging.checked || !selectedItem) return
		const content = selectedItem.logPathRead
		if (content !== lastLogContents) {
			lastLogContents = content
			logField.text = content
			logField.cursorPosition = logField.length
		}
	}

	function saveEnv() {
		if (!selectedItem) return
		let map = {}
		for (let i = 0; i < envModel.length; i++) {
			if (envModel[i].key.trim() !== "") {
				map[envModel[i].key] = envModel[i].value
			}
		}
		selectedItem.environmentVariables = map
		selectedItem.saveInstance()
	}

	Component.onCompleted: loadData()
	onSelectedItemChanged: if (selectedItem) loadData()

	FileDialog {
		id: entryPointFileDialog
		title: "Choose Vencord Entrypoint"
		fileMode: FileDialog.OpenFile
		nameFilters: ["Patcher files (*.js)", "All files (*)"]
		onAccepted: {
			const path = selectedFile.toString().replace("file://", "")
			entryPointField.text = path
			if (selectedItem) {
				selectedItem.entryPointPath = path
				selectedItem.saveInstance()
			}
		}
	}

	Timer {
		id: logPollTimer
		interval: 1000
		repeat: true
		running: enableLogging.checked
		onTriggered: editWindow.pollLog()
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
			Layout.margins: 12
			spacing: 0

			TabBar {
				id: tabBar
				Layout.fillWidth: true

				leftPadding: 4

				Repeater {
					model: [
						qsTr("Log"),
						qsTr("General"),
						qsTr("Environment Variables"),
						qsTr("Updater"),
						qsTr("Notes")
					]

					TabButton {
						text: modelData
						width: implicitWidth
					}
				}
			}

			Pane {
				Layout.fillWidth: true
				Layout.fillHeight: true

				background: Rectangle {
					color: sysPalette.window
					border.color: sysPalette.light
					border.width: 1.17
					radius: 6
				}

				padding: 12

				StackLayout {
					id: stackLayout
					anchors.fill: parent
					currentIndex: tabBar.currentIndex

					// Discord Log
					ColumnLayout {
						spacing: 8

						Label { text: qsTr("Log"); font.pixelSize: 24; font.bold: true }

						ScrollView {
							Layout.fillWidth: true
							Layout.fillHeight: true
							TextArea {
								id: logField
								readOnly: true
								selectByMouse: true
								wrapMode: (selectedItem && selectedItem.enableLoggingWrap) ? TextEdit.Wrap : TextEdit.NoWrap
							}
						}

						RowLayout {
							Button {
								text: qsTr("Copy")
								icon.name: "edit-copy"
								icon.source: "qrc:/VencordLauncher/resources/glyphs/copy.svg"
								onClicked: { logField.selectAll(); logField.copy(); logField.deselect(); }
							}
							CheckBox {
								id: enableLogging
								text: qsTr("Keep updating")
								onToggled: {
									if (selectedItem) {
										selectedItem.enableLogging = checked
										selectedItem.saveInstance()
									}
									if (checked) editWindow.pollLog()
								}
							}
							CheckBox {
								id: enableLoggingWrap
								text: qsTr("Wrap Lines")
								onToggled: {
									if (selectedItem) {
										selectedItem.enableLoggingWrap = checked
										selectedItem.saveInstance()
									}
								}
							}
						}
					}

					// General
					ScrollView {
						id: scrollView
						clip: true

						Pane {
							width: scrollView.availableWidth
							padding: 0
							background: null

							ColumnLayout {
								width: parent.width
								spacing: 12

								Label { text: qsTr("General"); font.pixelSize: 24; font.bold: true }

								Label { text: qsTr("Launch Arguments"); font.bold: true }
								Label { text: qsTr("Users may choose to have custom launch options for Discord.") }
								TextField {
									id: launchArgsField
									Layout.fillWidth: true
									placeholderText: "--start-minimized --enable-blink-features=MiddleClickAutoscroll"
									onTextEdited: {
										if (selectedItem) {
											selectedItem.launchArguments = text
											selectedItem.saveInstance()
										}
									}
								}

								Label { text: qsTr("Custom Mod Entry Point"); font.bold: true }
								Label { text: qsTr("This is how the mod loader is initiated, this happens before Discord actually runs.") }
								RowLayout {
									Layout.fillWidth: true
									TextField {
										id: entryPointField
										Layout.fillWidth: true
										onTextEdited: {
											if (selectedItem) {
												selectedItem.entryPointPath = text
												selectedItem.saveInstance()
											}
										}
									}
									Button {
										text: qsTr("Browse")
										icon.name: "list-add"
										icon.source: "qrc:/VencordLauncher/resources/glyphs/folder-arrow-right.svg"
										onClicked: entryPointFileDialog.open()
									}
								}
							}
						}
					}

					// Environment Variables
					ColumnLayout {
						spacing: 8

						Label { text: qsTr("Environment Variables"); font.pixelSize: 24; font.bold: true }

						Label { text: qsTr("Choose what environment variables are passed through into Discord.") }
						RowLayout {
							Layout.fillWidth: true
							spacing: 8
							Label {
								text: qsTr("Key")
								font.bold: true
								Layout.preferredWidth: 160
							}
							Label {
								text: qsTr("Value")
								font.bold: true
								Layout.fillWidth: true
							}
							Item { Layout.preferredWidth: 40 }
						}

						ListView {
							id: envTableView
							Layout.fillWidth: true
							Layout.fillHeight: true
							clip: true
							model: editWindow.envModel
							spacing: 4

							delegate: RowLayout {
								required property var modelData
								required property int index
								Layout.fillWidth: true
								spacing: 8

								TextField {
									Layout.preferredWidth: 160
									placeholderText: "KEY"
									text: modelData.key
									onTextEdited: {
										editWindow.envModel[index].key = text
										saveEnv()
									}
								}
								TextField {
									Layout.fillWidth: true
									placeholderText: "VALUE"
									text: modelData.value
									onTextEdited: {
										editWindow.envModel[index].value = text
										saveEnv()
									}
								}
								Button {
									Layout.preferredWidth: 40
									icon.name: "application-exit"
									icon.source: "qrc:/VencordLauncher/resources/glyphs/dismiss.svg"
									onClicked: {
										let arr = Array.from(editWindow.envModel)
										arr.splice(index, 1)
										editWindow.envModel = arr
										saveEnv()
									}
								}
							}
						}

						RowLayout {
							Button {
								text: qsTr("Add")
								icon.name: "list-add"
								onClicked: {
									let arr = Array.from(editWindow.envModel)
									arr.push({ key: "", value: "" })
									editWindow.envModel = arr
								}
							}

							Button {
								text: qsTr("Clear")
								onClicked: {
									editWindow.envModel = []
									saveEnv()
								}
							}
						}
					}

					// Updater
					ScrollView {
						id: updaterScrollView
						clip: true

						Pane {
							width: updaterScrollView.availableWidth
							padding: 0
							background: null

							ColumnLayout {
								width: parent.width
								spacing: 12

								Label { text: qsTr("Updater"); font.pixelSize: 24; font.bold: true; Layout.fillWidth: true }

								Label { text: qsTr("Updater GitHub URL"); font.bold: true }
								Label { text: qsTr("NOTE: this is required to initiate the updater.") }
								TextField {
									id: updaterGithubUrlField
									Layout.fillWidth: true
									placeholderText: "https://github.com/username/repository"
									onTextEdited: {
										if (selectedItem) {
											selectedItem.updaterGithubUrl = text
											selectedItem.saveInstance()
										}
									}
								}

								Label { text: qsTr("Updater Files (comma-separated)"); font.bold: true }
								TextField {
									id: updaterFilesField
									Layout.fillWidth: true
									placeholderText: "file1.js, file2.json"
									onTextEdited: {
										if (selectedItem) {
											selectedItem.updaterFiles = text
											selectedItem.saveInstance()
										}
									}
								}

								Label { text: qsTr("Updater File Output Path"); font.bold: true }
								RowLayout {
									Layout.fillWidth: true
									TextField {
										id: updaterOutputPathField
										Layout.fillWidth: true
										onTextEdited: {
											if (selectedItem) {
												selectedItem.updaterFileOutputPath = text
												selectedItem.saveInstance()
											}
										}
									}
									Button {
										text: qsTr("Browse")
										icon.name: "list-add"
										icon.source: "qrc:/VencordLauncher/resources/glyphs/folder-arrow-right.svg"
										onClicked: updaterOutputFileDialog.open()
									}
								}
							}
						}
					}

					// Notes
					ColumnLayout {
						spacing: 8

						Label { text: qsTr("Notes"); font.pixelSize: 24; font.bold: true }

						TextArea {
							id: notesField
							Layout.fillWidth: true
							Layout.fillHeight: true
							placeholderText: qsTr("Enter notes here...")
							wrapMode: TextEdit.Wrap
							selectByMouse: true
							onTextChanged: {
								if (activeFocus && selectedItem) {
									selectedItem.notes = text
									selectedItem.saveInstance()
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

		Footer {
			Layout.fillWidth: true
			Layout.preferredHeight: 55
			selectedItem: editWindow.selectedItem

			Item { Layout.fillWidth: true }

			Button {
				text: qsTr("Close")
				onClicked: editWindow.close()
			}
		}
	}









}
