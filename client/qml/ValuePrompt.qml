import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
	id: root

	property string promptTitle: "Value"

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
