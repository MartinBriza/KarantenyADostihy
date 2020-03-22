import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {
    Board {
        id: board
        width: Math.min(parent.width, parent.height)
        height: parent.height
        Repeater {
            model: client.opponents
            delegate: Player {
                model: modelData

                width: board.fieldWidth / 3.5
                height: width

                x: 3 + (index % 3) * (board.fieldWidth / 3) + board.fieldPosition(modelData.position).x
                y: 3 + Math.floor(index / 3) * (board.fieldWidth / 3) + board.fieldPosition(modelData.position).y
            }
        }
    }
    Rectangle {
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            left: board.right
        }
        ColumnLayout {
            anchors.fill: parent
            RowLayout {
                Text {
                    text: "Bank"
                }
                SpinBox {
                    id: bankSpinBox
                    from: 0
                    to: 1000000
                    stepSize: 20
                    editable: true
                }
                Button {
                    text: "Give"
                    onClicked: {
                        client.giveMoney(bankSpinBox.value)
                        bankSpinBox.value = 0
                    }
                }
                Button {
                    text: "Take"
                    onClicked: {
                        client.takeMoney(bankSpinBox.value)
                        bankSpinBox.value = 0
                    }
                }
            }
            Flow {
                Layout.fillWidth: true
                spacing: 6
                Repeater {
                    model: client.opponents
                    Rectangle {
                        id: opponentDelegate
                        property variant player: modelData
                        width: opponentLayout.width + 6
                        height: opponentLayout.height + 6
                        border.width: 1
                        border.color: "gray"
                        ColumnLayout {
                            x: 3
                            y: 3
                            id: opponentLayout
                            RowLayout {
                                ColumnLayout {
                                    RowLayout {
                                        Rectangle {
                                            width: 10
                                            height: width
                                            radius: width / 2
                                            color: modelData.color
                                            border.color: "black"
                                            border.width: 1
                                        }

                                        Text {
                                            text: modelData.name
                                            font.bold: true
                                        }
                                    }
                                    Text {
                                        text: "Money: " + modelData.money
                                    }
                                }
                                Item {
                                    Layout.fillWidth: true
                                }

                                Button {
                                    text: "Detaily"
                                }
                            }
                            RowLayout {
                                Repeater {
                                    model: ["⚀", "⚁", "⚂", "⚃", "⚄", "⚅"]
                                    Button {
                                        enabled: opponentDelegate.player.you
                                        implicitWidth: implicitHeight
                                        onClicked: client.move(opponentDelegate.player.id, opponentDelegate.player.position + index + 1)
                                        Component.onCompleted: console.log("My index is " + index)
                                        Text {
                                            font.pixelSize: parent.height
                                            opacity: parent.enabled ? 1.0 : 0.3
                                            text: modelData
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            anchors.bottom: parent.bottom
                                            anchors.bottomMargin: - parent.height / 16
                                        }
                                    }
                                }
                            }
                            Item {
                                id: ownershipLayout
                                property variant player: modelData
                                Layout.fillWidth: true
                                height: 10
                                width: 10
                                Repeater {
                                    model: modelData.owns
                                    Rectangle {
                                        height: 10
                                        width: (opponentDelegate.width - 6) / ownershipLayout.player.ownsCount
                                        x: index * width
                                        color: modelData.color
                                        opacity: modelData.owns ? 1.0 : 0.4
                                        border {
                                            width: 1
                                            color: modelData.owns ? "black" : "light gray"
                                        }
                                    }
                                }
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
    CardView {
        id: cardView
        anchors.centerIn: parent
    }
}
