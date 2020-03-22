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
            Flow {
                Layout.fillWidth: true
                Repeater {
                    model: client.opponents
                    Rectangle {
                        width: opponentLayout.width
                        height: opponentLayout.height
                        border.width: 1
                        border.color: "gray"
                        ColumnLayout {
                            id: opponentLayout
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
                            RowLayout {
                                SpinBox {
                                    id: moveBy
                                    from: 1
                                    to: 6
                                }
                                Button {
                                    onClicked: client.move(modelData.id, modelData.position + moveBy.value)
                                    text: "Move"
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
