import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
	id: root
	spacing: 10

	required property var discordItem
	property color textColor: "#000000"
	property color secondaryTextColor: Qt.lighter(textColor, 1.5)
	property string emptyText: qsTr("No Discord Installations Found")

	property string text: contentText.text
	function positionToRectangle(pos) { return Qt.rect(0, 0, 0, 0) }

	function getBranchIcon(branch, isFlatpak) {
		let branchLower = branch ? branch.toLowerCase() : ""
		let iconPath = "qrc:/VencordLauncher/resources/instances/DiscordStable." + LaunchInstanceWrapper.getIconType()

		if (branchLower.includes("canary")) {
			iconPath = "qrc:/VencordLauncher/resources/instances/DiscordCanary." + LaunchInstanceWrapper.getIconType()
		} else if (branchLower.includes("development")) {
			iconPath = "qrc:/VencordLauncher/resources/instances/DiscordDevelopment." + LaunchInstanceWrapper.getIconType()
		}

		return iconPath
	}

	Image {
		source: root.discordItem ? getBranchIcon(root.discordItem.branch, root.discordItem.isFlatpak) : ""
		sourceSize.width: 24
		sourceSize.height: 24
		fillMode: Image.PreserveAspectFit
		visible: root.discordItem !== null
		Layout.alignment: Qt.AlignVCenter
	}

	ColumnLayout {
		Layout.fillWidth: true
		spacing: 2

		Text {
			id: contentText
			text: root.discordItem
				? (root.discordItem.branch + (root.discordItem.isFlatpak ? " (flatpak)" : ""))
				: root.emptyText
			color: root.textColor
			font.bold: true
			elide: Text.ElideRight
			Layout.fillWidth: true
		}

		Text {
			text: root.discordItem ? root.discordItem.uniqueIdentifier : ""
			color: root.secondaryTextColor
			font.pixelSize: 11
			elide: Text.ElideRight
			visible: root.discordItem !== null && root.discordItem.uniqueIdentifier !== undefined
			Layout.fillWidth: true
		}
	}
}
