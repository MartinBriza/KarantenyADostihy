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

        if (model.type === FieldData.HORSE) {
            horseFees.model = model.effects
        }
        else {
            horseFees.model = null
        }
        if (model.type === FieldData.TRAINER) {
            trainerFees.model = model.effects
        }
        else {
            trainerFees.model = null
        }
        if (model.type === FieldData.TRANSPORT) {
            transportFees.model = model.effects
        }
        else {
            transportFees.model = null
        }
    }
    ColumnLayout {
        visible: model
        Rectangle {
            Layout.fillWidth: true
            height: 8
            color: model ? model.color : "white"
        }

        Text {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: model ? model.name : "NOPE"
            font.bold: true
            font.pixelSize: 32
        }
        Text {
            visible: model && (model.type === FieldData.HORSE || model.type === FieldData.TRAINER || model.type === FieldData.TRANSPORT)
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: "<b>Vlastník: </b> " + (model && model.owner ? model.owner.name : "<i>žádný</i>")
            textFormat: Text.RichText
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            Button {
                Layout.alignment: Qt.AlignHCenter
                visible: model && (model.type === FieldData.HORSE || model.type === FieldData.TRAINER || model.type === FieldData.TRANSPORT)
                text: "Koupit"
                enabled: model && model.owner === null
                onClicked: client.buy(model.id)
            }
            Button {
                Layout.alignment: Qt.AlignHCenter
                visible: model && model.type === FieldData.HORSE
                text: "Obsadit dostih"
                onClicked: client.upgrade(model.id)
            }
            Button {
                Layout.alignment: Qt.AlignHCenter
                visible: !client.strictGame && model && model.type === FieldData.DECK
                text: "Táhnout"
                onClicked: {
                    // ugh
                    if (model.name === "Náhoda")
                        client.drawCard(1)
                    else
                        client.drawCard(2)
                }
            }
        }

        RowLayout {
            visible: model && (model.type === FieldData.HORSE || model.type === FieldData.TRAINER || model.type === FieldData.TRANSPORT)
            Text { text: "Pořizovací cena: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model ? model.price : "" }
        }
        Repeater {
            id: horseFees
            visible: model && model.type === FieldData.HORSE
            model: null
            ColumnLayout {
                Layout.fillWidth: true
                Text {
                    visible: index == 1
                    text: "Zisk";
                    font.bold: true
                }
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: index === 0 ? "Prohlídka stáje: " :
                              index === 5 ? "Zisk z hlavního dostihu: " :
                                            "Zisk z " + index + ". dostihu: "
                    }
                    Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: modelData.amount }
                }
            }
        }

        Text {
            visible: model && model.type === FieldData.TRAINER
            text: "Majitel licencí vybírá tyto poplatky:";
            font.bold: true
        }

        Repeater {
            id: trainerFees
            visible: model && model.type === FieldData.TRAINER
            model: null
            ColumnLayout {
                Layout.fillWidth: true
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: (index + 1) + ". licence"
                    }
                    Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: modelData.amount }
                }
            }
        }

        Text {
            visible: model && model.type === FieldData.TRANSPORT
            text: "Použití přepravy:";
            font.bold: true
        }

        Repeater {
            id: transportFees
            visible: model && model.type === FieldData.TRANSPORT
            model: null
            ColumnLayout {
                Layout.fillWidth: true
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: index === 0 ? "Při vlastnění pouze karty Přeprava nebo Stáje" :
                                "Při vlastnění obou karet Přeprava i Stáje"
                    }
                    Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: modelData.amount + " x hozená hodnota" }
                }
            }
        }

        Text {
            visible: model && model.type === FieldData.HORSE
            text: "Náklady na přípravu";
            font.bold: true
        }
        RowLayout {
            visible: model && model.type === FieldData.HORSE
            Text { text: "na nový dostih: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model ? model.upgradePrice : "" }
        }
        RowLayout {
            visible: model && model.type === FieldData.HORSE
            Text { text: "na hlavní dostih: " }
            Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: model ? model.upgradePrice : "" }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 8
            color: model ? model.color : "white"
        }
    }
}
