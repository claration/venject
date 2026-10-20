import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCore

Window {
    id: editWindow

    width: 650
    height: 500
    visible: true

    required property var selectedItem

    title: selectedItem.branch
           + (selectedItem.isFlatpak ? " (Flatpak)" : "")

	// TODO: discord icon in window titlebar

    SystemPalette {
        id: sysPalette
        colorGroup: SystemPalette.Active
    }

    color: sysPalette.window

	property string currentItemId: String(selectedItem.id)

	Component {
		id: settingsComponent

		Settings {
			property string launchArgs: ""
			property string notes: ""
			property bool enableLogging: true
			property bool enableLoggingWrap: true

			category: "discords/" + editWindow.currentItemId
		}
	}

	Loader {
		id: settingsLoader
		sourceComponent: settingsComponent
		active: true

		onLoaded: {
			launchArgsField.text = item.launchArgs
			notesField.text = item.notes
			enableLogging.checked = item.enableLogging
			enableLoggingWrap.checked = item.enableLoggingWrap
		}
	}

	onSelectedItemChanged: {
		currentItemId = String(selectedItem.id)

		settingsLoader.active = false
		settingsLoader.active = true
	}

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: sysPalette.light
        }

        TabBar {
            id: tabBar

            Layout.fillWidth: true

            TabButton {
                text: "Discord Log"
            }

            TabButton {
                text: "Settings"
            }

            TabButton {
                text: "Notes"
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // Discord Log tab
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 12

                    Label {
                        text: "Discord Log"
                        font.bold: true
                        font.pixelSize: 18
                    }

                    TextArea {
                        id: logField

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        readOnly: true
                        selectByMouse: true
                        wrapMode: TextEdit.Wrap
                    }

					RowLayout {
						spacing: 12

						CheckBox {
							id: enableLogging
							text: "Keep updating"
							checked: true
							onToggled: {
								if (settingsLoader.item)
									settingsLoader.item.enableLogging = checked
							}
						}

						CheckBox {
							id: enableLoggingWrap
							text: "Wrap Lines"
							checked: true
							onToggled: {
								if (settingsLoader.item)
									settingsLoader.item.enableLoggingWrap = checked
							}
						}

						Button {
							text: "Copy"
						}

						Button {
							text: "Clear"
							onClicked: logField.clear()
						}
					}
                }
            }

            // Settings tab
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 12

                    Label {
                        text: "Settings"
                        font.bold: true
                        font.pixelSize: 18
                    }

                    Label {
                        text: "Launch arguments"
                    }

                    TextField {
                        id: launchArgsField
                        Layout.fillWidth: true
                        placeholderText: "--start-minimized --enable-blink-features=MiddleClickAutoscroll"
						onTextEdited: {
							if (settingsLoader.item)
								settingsLoader.item.launchArgs = text
						}
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            // Notes tab
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 12

                    Label {
                        text: "Notes"
                        font.bold: true
                        font.pixelSize: 18
                    }

                    TextArea {
                        id: notesField

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        placeholderText: "Enter notes..."
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
						onTextChanged: {
							if (activeFocus && settingsLoader.item)
								settingsLoader.item.notes = text
						}
                    }
                }
            }
        }

		Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: sysPalette.light
        }

        Footer {
            Layout.fillWidth: true
            Layout.preferredHeight: 55

            selectedItem: editWindow.selectedItem
            showEditButton: false
        }
    }
}
