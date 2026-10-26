import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
	id: root
	spacing: 8

	required property var discordItem
	property color textColor: "#000000"
	property string emptyText: qsTr("No Discord Installations Found")

	property string text: contentText.text
	function positionToRectangle(pos) { return Qt.rect(0, 0, 0, 0) }

	function getBranchIcon(branch, isFlatpak) {
		let branchLower = branch ? branch.toLowerCase() : ""
		let iconPath = "qrc:/VencordLauncher/resources/stable.svg"

		if (branchLower.includes("canary")) {
			iconPath = "qrc:/VencordLauncher/resources/canary.svg"
		} else if (branchLower.includes("development")) {
			iconPath = "qrc:/VencordLauncher/resources/development.svg"
		}

		return iconPath
	}

	Image {
		source: root.discordItem ? getBranchIcon(root.discordItem.branch, root.discordItem.isFlatpak) : ""
		sourceSize.width: 16
		sourceSize.height: 16
		fillMode: Image.PreserveAspectFit
		visible: root.discordItem !== null
	}

	Text {
		id: contentText
		text: root.discordItem
			? (root.discordItem.branch + (root.discordItem.isFlatpak ? " (flatpak)" : ""))
			: root.emptyText
		color: root.textColor
		elide: Text.ElideRight
		verticalAlignment: Text.AlignVCenter
		Layout.fillWidth: true
	}
}
