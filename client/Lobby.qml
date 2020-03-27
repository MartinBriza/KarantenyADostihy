import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import monopoly 1.0

Item {
    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.fill: parent
        spacing: 9

        Text {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: client.lobby ? client.lobby.name : ""
            font.pixelSize: 32
        }

        RowLayout {
            Layout.fillWidth: true
            Button {
                text: "Odejít"
                onClicked: client.leave()
            }
            Item {
                Layout.fillWidth: true
            }
            Button {
                checkable: true
                text: "Ready"
                onCheckedChanged: client.setReady(checked)
            }
            Button {
                text: "Začít"
                onClicked: client.startGame()
            }
        }

        Flow {
            Layout.fillWidth: true
            Repeater {
                model: client.players
                Rectangle {
                    width: childrenRect.width + 18
                    height: childrenRect.height + 18
                    border.color: "gray"
                    border.width: 1
                    ColumnLayout {
                        x: 9
                        y: 9
                        RowLayout {
                            Rectangle {
                                height: 10
                                width: height
                                radius: height / 2
                                color: modelData.color
                                border {
                                    color: "black"
                                    width: 1
                                }
                            }

                            Text {
                                text: name
                                font.bold: modelData.ready
                                color: modelData.ready ? "green" : "black"
                            }
                            Text {
                                text: "(you)"
                                visible: you
                            }

                            Text {
                                text: "☠️"
                                visible: leader
                            }
                        }
                        Text {
                            text: "Money: " + money
                        }
                    }
                }
            }
        }
        Chat {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
