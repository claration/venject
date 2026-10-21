pragma Singleton
import QtQuick
import QtCore

QtObject {
    property string currentItemId: ""

    property Settings settings: Settings {
        category: "discords/" + currentItemId

        property string entryPoint: ""
        property string launchArgs: ""
        property string notes: ""
        property bool enableLogging: true
        property bool enableLoggingWrap: true
    }
}
