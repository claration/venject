import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
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

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: sysPalette.light
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Label {
                anchors.centerIn: parent
                text: "a"
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
