import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
	id: root

	property string promptTitle: "Value"
	property string activeEditType: ""
	property var targetItem: null

	title: "Edit " + root.promptTitle

	standardButtons: Dialog.Ok | Dialog.Cancel
	modal: true
	focus: true

	property string initialValue: ""
	property alias text: textField.text

	onOpened: {
		textField.text = root.initialValue
		textField.forceActiveFocus()
		textField.selectAll()
	}

	onAccepted: {
		if (!targetItem) return

		if (root.activeEditType === "Name") {
			targetItem.name = textField.text
		} else if (root.activeEditType === "Group") {
			targetItem.groupName = textField.text
		}
		targetItem.saveInstance()
	}

	contentItem: ColumnLayout {
		spacing: 12

		Label {
			text: "Enter new " + root.promptTitle.toLowerCase() + ":"
			Layout.fillWidth: true
		}

		TextField {
			id: textField
			Layout.fillWidth: true

			Keys.onReturnPressed: root.accept()
			Keys.onEnterPressed: root.accept()
		}
	}
}
