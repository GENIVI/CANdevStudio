import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0
import QtQml 2.0


Item {
        Rectangle {
            anchors.top: parent.top
            width: 500
            height:200
            color: "#040404FF"


            ListView {
                id: listview
                anchors.fill: parent

                model: ListModel {
                    id: messagesModel
                }

                header: RowLayout
                {
                    spacing: 0
                    Rectangle {
                        width: 200
                        height: 15
                        Text { text: "Timestamp" }
                    }

                    Rectangle {
                        width: 75
                        height: 15
                        Text {
                            text: "Type"
                            anchors.fill: parent
                        }
                    }

                    Rectangle {
                        width: 200
                        height: 15
                        Text {
                            text: "Identifier"
                            anchors.fill: parent
                        }
                    }

                    Rectangle {
                        width: 100
                        height: 15
                        Text {
                            text: "Data"
                            anchors.fill: parent
                        }
                    }
                }

                delegate: RowLayout
                {
                    spacing: 2
                    Rectangle {
                        width: 200
                        height: 15
                        color: "#100000FF"
                        Text {
                            text: new Date().toLocaleString(Qt.locale(), "yyyy-MM-dd hh:mm:ss.zzz")
                            anchors.fill: parent
                        }
                    }

                    Rectangle {
                        width: 75
                        height: 15
                        color: "#002000FF"
                        Text {
                            text: type
                            anchors.fill: parent
                        }
                    }

                    Rectangle {
                        width: 200
                        height: 15
                        color: "#002000FF"
                        Text {
                            text: identifier
                            anchors.fill: parent
                        }
                    }

                    Rectangle {
                        width: 100
                        height: 15
                        color: "#000010FF"
                        Text {
                            text: binarydata
                            anchors.fill: parent
                        }
                    }
                }


                onCountChanged: {
                    Qt.callLater( listview.positionViewAtEnd )
                }
            }
        }

    Item {
        function frameReceived(frameId, framePayload) {
            var array = new Uint8Array(framePayload)

            var hex = ''
            for (var i = 0; i < array.length; i++)
            {
                hex = hex + array[i].toString(16)
            }

            var identifier = "0x" + frameId.toString(16)

            messagesModel.append({ identifier: identifier, binarydata: hex, type: "FRAME" })


            CANBusModel.sendFrame(frameId, framePayload)
        }

        function signalReceived(signalId, signalValue) {
            messagesModel.append({ identifier: signalId, binarydata: signalValue, type: "SIGNAL"})

            CANBusModel.sendSignal(signalId, signalValue)
        }

        Component.onCompleted: {
            CANBusModel.frameReceived.connect(frameReceived)
            CANBusModel.signalReceived.connect(signalReceived)
        }
    }

}
