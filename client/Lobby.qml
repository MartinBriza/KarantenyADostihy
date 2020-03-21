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
            text: client.lobby.name
            font.pixelSize: 32
        }

        RowLayout {
            Layout.fillWidth: true
            Button {
                text: "Odejít"
            }
            Item {
                Layout.fillWidth: true
            }
            Button {
                checkable: true
                text: "Ready"
            }
            Button {
                text: "Začít"
            }
        }

        Flow {
            Layout.fillWidth: true
            Repeater {
                model: client.opponents
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
                                height: 8
                                width: height
                                radius: height / 2
                                color: modelData.color
                            }

                            Text {
                                text: name
                                font.bold: ready
                                color: ready ? "green" : "black"
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

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Rectangle {
                anchors.fill: parent
                anchors.margins: 3
                border.color: "gray"
                border.width: 1
                ColumnLayout {
                    anchors.fill: parent
                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: client.chat
                        delegate: RowLayout {
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
                                font.bold: urgency > 0
                                color: urgency > 1 ? "red" : "black"
                            }
                        }
                    }
                    TextField {
                        Layout.fillWidth: true
                        onAccepted: {
                            client.sendMessage(text)
                            text = ""
                        }
                    }
                }
            }
        }
    }
}
