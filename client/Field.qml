import QtQuick 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: fieldRoot
    clip: true
    color: modelData.color
    x: fieldPosition(index).x
    y: fieldPosition(index).y
    z: mouse.containsMouse ? 1 : 0

    property variant fieldModel: modelData

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
        /*
        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            text: modelData.fee > 0 ? modelData.fee : ""
        }
        */
        Row {
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            spacing: 1
            Repeater {
                model: modelData.upgradeLevel > 0 && modelData.upgradeLevel < 5 ? modelData.upgradeLevel : 0
                Rectangle {
                    width: fieldWidth / 5
                    height: width
                    color: "yellow"
                    radius: width / 2
                    border.width: 1
                    border.color: "black"
                }
            }
            Rectangle {
                visible: modelData.upgradeLevel === 5
                width: fieldWidth / 3.3
                height: width
                color: "yellow"
                radius: width / 2
                border.width: 1
                border.color: "black"
            }
        }
        Item {
            width: 1
            height: 1
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            fieldView.show(modelData)
        }
    }
}
