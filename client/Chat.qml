import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {
    Rectangle {
        clip: true
        rotation: 180
        anchors.fill: parent
        anchors.margins: 3
        border.color: "gray"
        border.width: 1
        ColumnLayout {
            anchors.fill: parent
            TextField {
                rotation: 180
                Layout.fillWidth: true
                onAccepted: {
                    client.sendMessage(text)
                    text = ""
                }
            }
            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: client.chat
                delegate: RowLayout {
                    rotation: 180
                    width: parent.width
                    Text {
                        text: time
                    }
                    Text {
                        visible: from.length > 0
                        font.bold: true
                        text: from
                    }
                    Text {
                        Layout.fillWidth: true
                        text: message
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        font.bold: urgency > 0
                        color: urgency > 1 ? "red" : urgency > 0 ? "black" : "gray"
                    }
                }
            }
        }
    }
}
