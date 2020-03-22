import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import monopoly 1.0

Dialog {
    modal: true
    property variant model: null
    function show(obj) {
        model = obj
        visible = true
    }
    ColumnLayout {
        Rectangle {
            Layout.fillWidth: true
            height: 8
            color: model.color
        }

        Text {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: model ? model.name : "NOPE"
            font.bold: true
            font.pixelSize: 32
        }
        Text {
            visible: model.type === FieldData.HORSE || model.type === FieldData.TRAINER || model.type === FieldData.TRANSPORT
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: "<b>Vlastník: </b> " + (model.owner ? model.owner.name : "<i>žádný</i>")
            textFormat: Text.RichText
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            Button {
                Layout.alignment: Qt.AlignHCenter
                visible: model.type === FieldData.HORSE || model.type === FieldData.TRAINER || model.type === FieldData.TRANSPORT
                text: "Koupit"
                enabled: model.owner === null
                onClicked: client.buy(model.id)
            }
            Button {
                Layout.alignment: Qt.AlignHCenter
                visible: model.type === FieldData.HORSE
                text: "Obsadit dostih"
            }
            Button {
                Layout.alignment: Qt.AlignHCenter
                visible: model.type === FieldData.DECK
                text: "Táhnout"
            }
        }

        RowLayout {
            visible: model.type === FieldData.HORSE || model.type === FieldData.TRAINER || model.type === FieldData.TRANSPORT
            Text { text: "Pořizovací cena: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model.price }
        }
        RowLayout {
            visible: model.type === FieldData.HORSE
            Text { text: "Prohlídka stáje: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model.price }
        }

        Text {
            visible: model.type === FieldData.HORSE
            text: "Zisk";
            font.bold: true
        }
        RowLayout {
            visible: model.type === FieldData.HORSE
            Text { text: "z 1 dostihu: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model.price }
        }
        RowLayout {
            visible: model.type === FieldData.HORSE
            Text { text: "ze 2 dostihů: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model.price }
        }
        RowLayout {
            visible: model.type === FieldData.HORSE
            Text { text: "ze 3 dostihů: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model.price }
        }
        RowLayout {
            visible: model.type === FieldData.HORSE
            Text { text: "ze 4 dostihů: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model.price }
        }
        RowLayout {
            visible: model.type === FieldData.HORSE
            Text { text: "z hlavního dostihu: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model.price }
        }
        Text {
            visible: model.type === FieldData.HORSE
            text: "Náklady na přípravu";
            font.bold: true
        }
        RowLayout {
            visible: model.type === FieldData.HORSE
            Text { text: "na nový dostih: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model.upgradePrice }
        }
        RowLayout {
            visible: model.type === FieldData.HORSE
            Text { text: "na hlavní dostih: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model.upgradePrice }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 8
            color: model.color
        }
    }
}
