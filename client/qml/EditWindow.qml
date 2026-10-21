
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore
import VencordLauncher

Window {
    id: editWindow

    width: 650
    height: 500
    visible: true

    required property var selectedItem

    title: selectedItem.branch
           + (selectedItem.isFlatpak ? " (Flatpak)" : "")

    SystemPalette {
        id: sysPalette
        colorGroup: SystemPalette.Active
    }

    color: sysPalette.window

    function loadSettings() {
        entryPointField.text = SettingsHelper.settings.entryPoint
        launchArgsField.text = SettingsHelper.settings.launchArgs
        notesField.text = SettingsHelper.settings.notes
        enableLogging.checked = SettingsHelper.settings.enableLogging
        enableLoggingWrap.checked = SettingsHelper.settings.enableLoggingWrap
    }

    function selectItem() {
        SettingsHelper.currentItemId = String(selectedItem.id)
        loadSettings()
    }

    Component.onCompleted: selectItem()

    onSelectedItemChanged: {
        if (selectedItem)
            selectItem()
    }

    FileDialog {
        id: entryPointFileDialog

        title: "Choose a Vencord entrypoint (patcher.js)"
        fileMode: FileDialog.OpenFile
        nameFilters: ["Patcher files (*.js)", "All files (*)"]

        onAccepted: {
            const path = selectedFile.toString().replace("file://", "")
            entryPointField.text = path
            SettingsHelper.settings.entryPoint = path
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton { text: "Discord Log" }
            TabButton { text: "Settings" }
            TabButton { text: "Notes" }
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
                            onToggled: SettingsHelper.settings.enableLogging = checked
                        }

                        CheckBox {
                            id: enableLoggingWrap
                            text: "Wrap Lines"
                            onToggled: SettingsHelper.settings.enableLoggingWrap = checked
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
                        text: "Vencord Installation"
                        font.bold: true
                        font.pixelSize: 15
                    }

                    Label {
                        text: "Custom Vencord entry point path, keep blank for default.\n\nNote: choosing a custom location will disable the update alert."
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        TextField {
                            id: entryPointField
                            Layout.fillWidth: true
                            placeholderText: "/home/user/.config/Vencord/dist/patcher.js"
                            onTextEdited: SettingsHelper.settings.entryPoint = text
                        }

                        Button {
                            text: "Browse"
                            onClicked: entryPointFileDialog.open()
                        }
                    }

                    Label {
                        text: "Launch arguments"
                        font.bold: true
                        font.pixelSize: 15
                    }

                    Label {
                        text: "Advanced users may choose to customize Discords & Electrons startup options."
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    TextField {
                        id: launchArgsField
                        Layout.fillWidth: true
                        placeholderText: "--start-minimized --enable-blink-features=MiddleClickAutoscroll"
                        onTextEdited: SettingsHelper.settings.launchArgs = text
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
                            if (activeFocus)
                                SettingsHelper.settings.notes = text
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
