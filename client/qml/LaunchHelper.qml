import QtQuick

pragma Singleton

Item {
	signal handleLaunchMessage(path: string)

	function handleLaunch(path) {
		handleLaunchMessage(path)
	}
}
