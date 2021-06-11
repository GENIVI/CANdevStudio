import QtQuick 2.0

/*
 * This example receives a frame or signal modifies and transmit it
 */

Item {
    function frameReceived(frameId, framePayload) {
        // Increment frame ID of received CAN
        frameId += 1

        // LogWindow is QPlainTextEdit widget exposed to QML
        // You can use it to append text or control its visibility
        LogWindow.appendPlainText("Frame: " + frameId + " Forwarded")

        // Transmit frame
        //
        // CANBusModel is an QObject exposed to QML
        // You can use it to receive and transmit frames and signals
        CANBusModel.sendFrame(frameId, framePayload)
    }

    function signalReceived(signalId, signalValue) {
        // Increment signal value
        signalValue += 1
    
        // Transmit signal
        CANBusModel.sendSignal(signalId, signalValue)
    }

    Component.onCompleted: {
        // We have no GUI to display so LogWindow can be visible
        LogWindow.setVisible(true)

        // Connect QML functions to CANBusModel signals
        CANBusModel.frameReceived.connect(frameReceived)
        CANBusModel.signalReceived.connect(signalReceived)
    }
}


