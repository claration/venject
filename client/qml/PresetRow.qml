import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
	id: root
	width: 90
	height: 90

	required property string presetId
	required property string title
	property string iconSource: ""
	property string symbol: ""
	property bool isSelected: false
	property var sysPalette

	signal clicked()

	color: isSelected ? sysPalette.highlight : sysPalette.base
	border.color: isSelected ? sysPalette.highlight : sysPalette.light
	radius: 8

	MouseArea {
		anchors.fill: parent
		onClicked: root.clicked()
	}

	ColumnLayout {
		anchors.centerIn: parent
		spacing: 6

		Item {
			Layout.alignment: Qt.AlignHCenter
			width: 32
			height: 32

			Image {
				anchors.centerIn: parent
				source: root.iconSource
				sourceSize.width: 32
				sourceSize.height: 32
				fillMode: Image.PreserveAspectFit
				visible: root.iconSource !== ""
			}

			Text {
				anchors.centerIn: parent
				text: root.symbol
				font.pixelSize: 28
				color: root.isSelected ? root.sysPalette.highlightedText : root.sysPalette.text
				visible: root.iconSource === ""
			}
		}

		Label {
			text: root.title
			font.bold: true
			font.pixelSize: 13
			color: root.isSelected ? root.sysPalette.highlightedText : root.sysPalette.text
			Layout.alignment: Qt.AlignHCenter
		}
	}
}
