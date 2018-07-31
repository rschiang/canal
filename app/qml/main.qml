import QtQuick 2.10 // 2.11
import QtQuick.Controls 2.3 // 2.4
import QtQuick.Window 2.2 // 2.11
import QtQuick.Dialogs 1.2

Window {
    width: 400
    height: 300
    visible: true

    Button {
        anchors.centerIn: parent
        text: "Hello world"
        onClicked: dialog.open()
    }

    MessageDialog {
        id: dialog
        title: "Information"
        text: "I can eat glass and it doesn't hurt me!"
        onAccepted: text += " +1"
    }
}
