import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
	id: cardRoot
	width: 110
	height: 95

	required property var modelData
	required property bool isSelected
	readonly property bool isHovered: mouseArea.containsMouse

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		hoverEnabled: true
		cursorShape: Qt.PointingHandCursor
		onClicked: root.selectedItem = modelData
		onDoubleClicked: {
			root.selectedItem = modelData
			LaunchHelper.handleLaunch(modelData.path)
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
