import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {
    Connections {
        target: client
        onDisplayCard: {
            cardView.show(header, message)
        }
    }
    Board {
        id: board
        width: Math.min(parent.width, parent.height)
        height: parent.height
        Repeater {
            model: client.players
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
                RowLayout {
                    id: bankLayout
                    visible: !client.strictGame
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
                Item {
                    height: 1
                    Layout.fillWidth: true
                }
                Button {
                    text: "Odejít"
                    onClicked: {
                        client.leave();
                    }
                }
            }
            Flow {
                Layout.fillWidth: true
                spacing: 6
                Repeater {
                    model: client.players
                    Rectangle {
                        id: opponentDelegate
                        property variant player: modelData
                        Connections {
                            target: player
                            onDiceChanged: {
                                console.warn("AHOJ")
                                console.warn(player.dice)
                            }
                        }
                        width: opponentLayout.width + 6
                        height: opponentLayout.height + 6
                        border {
                            width: player.onTurn ? 2 : 1
                            color: player.onTurn ? "green" : "gray"
                        }
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
                                        text: "Peníze: " + modelData.money
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
                                enabled: player.you
                                Repeater {
                                    model: ["⚀", "⚁", "⚂", "⚃", "⚄", "⚅"]
                                    Button {
                                        visible: !client.strictGame
                                        enabled: opponentDelegate.player.you
                                        implicitWidth: implicitHeight
                                        onClicked: client.move(opponentDelegate.player.id, opponentDelegate.player.position + index + 1)
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
                                Button {
                                    visible: client.strictGame && !(player.dice.length > 0 && player.dice[player.dice.length - 1] !== 6) && !player.diceMoved
                                    enabled: player.onTurn
                                    text: "Hodit kostkou"
                                    onClicked: client.dice();
                                }
                                Button {
                                    visible: client.strictGame && (player.dice.length > 0 && player.dice[player.dice.length - 1] !== 6) && !player.diceMoved
                                    enabled: player.onTurn
                                    text: "Jít"
                                    onClicked: client.diceMove();
                                }
                                Button {
                                    visible: client.strictGame && player.diceMoved
                                    text: "Konec tahu"
                                    onClicked: client.endTurn();
                                }
                                ColumnLayout {
                                    visible: client.strictGame
                                    spacing: 0
                                    Text { text: "Hozeno" }
                                    Text {
                                        Layout.fillWidth: true
                                        horizontalAlignment: Qt.AlignHCenter
                                        font.bold: true
                                        font.pixelSize: 16
                                        text: String(player.dice)
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
    FieldView {
        id: fieldView
        anchors.centerIn: parent
    }
    CardView {
        id: cardView
        anchors.centerIn: parent
    }
}
