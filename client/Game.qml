import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {
    Board {
        id: board
        width: Math.min(parent.width, parent.height)
        height: parent.height
        Repeater {
            model: game.players
            delegate: Player {
                model: modelData

                x: 3 + (index % 2) * 32 + board.fieldPosition(modelData.position).x
                y: 3 + (index % 2) * 32 + board.fieldPosition(modelData.position).y
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
        ListView {
            anchors.fill: parent
            model: game.players
            delegate: Rectangle {
                width: parent.width
                height: 64
                ColumnLayout {
                    RowLayout {
                        Rectangle {
                            width: 8
                            height: width
                            radius: width / 2
                            color: modelData.color
                        }

                        Text {
                            text: "Player " + modelData.name + " has " + modelData.money + " monies."
                        }
                    }
                    RowLayout {
                        SpinBox {
                            id: moveBy
                            from: 1
                            to: 6
                        }
                        Button {
                            onClicked: modelData.move(moveBy.value)
                            text: "Move"
                        }
                    }
                }
            }
        }
    }
}
