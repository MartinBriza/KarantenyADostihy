import QtQuick 2.13
import QtQuick.Layouts 1.12

Rectangle {
    clip: true
    color: modelData.color
    x: fieldPosition(index).x
    y: fieldPosition(index).y
    z: mouse.containsMouse ? 1 : 0

    scale: mouse.pressed ? 1.05 : mouse.containsMouse ? 1.1 : 1.0
    Behavior on scale { NumberAnimation { duration: 100 } }

    width: fieldWidth
    height: fieldHeight
    border.color: "black"
    border.width: 1
    ColumnLayout {
        anchors.fill: parent
        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            horizontalAlignment: Text.AlignHCenter
            text: modelData.name
        }
        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            text: modelData.price > 0 ? modelData.price : ""
        }
        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            text: modelData.fee > 0 ? modelData.fee : ""
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
    }
}
