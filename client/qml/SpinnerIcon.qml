import QtQuick

Item {
	id: root

	property string sourceImage: ""
	property bool spinning: false

	Image {
		id: spinnerImage
		anchors.fill: parent
		sourceSize: parent.size
		source: root.sourceImage
		fillMode: Image.PreserveAspectFit
		opacity: 0.4

		transform: Rotation {
			id: spinnerRotation
			origin.x: spinnerImage.width / 2
			origin.y: spinnerImage.height / 2
			angle: 0
		}

		NumberAnimation {
			target: spinnerRotation
			property: "angle"
			from: 0
			to: 360
			duration: 1000
			loops: Animation.Infinite
			running: root.spinning
		}
	}
}
