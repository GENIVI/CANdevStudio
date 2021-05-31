import QtQuick 2.0

/*
 * This example shows how to use timers
 */

Item {
    property var frameId1 : 0x112233
    property var payload1 : 0
    property var frameId2 : 0x7ff
    property var payload2 : 0

    Timer {
        id: timer1
        interval: 50; running: false; repeat: true
        onTriggered: timer1Func()
    }

    Timer {
        id: timer2
        interval: 10; running: false; repeat: true
        onTriggered: timer2Func()
    }

    function timer1Func() {
        let b = new ArrayBuffer(4);
        let view = new DataView(b)

        // x86 architecture is little endian.
        // Indicate that we want to store vale using big endian (3rd argument)
        view.setUint32(0, payload1++, false)

        CANBusModel.sendFrame(frameId1, b)
    }

    function timer2Func() {

        let b = new ArrayBuffer(8);
        let view = new Uint8Array(b)

        // Due to differences in endianess
        // 0 in the array is number 7 in can frame
        // Let's ignore it for now and reverse array at the end
        view[0] = payload2 & 0xff
        view[1] = (payload2 & 0xff00) >> 8
        view[2] = (payload2 & 0xff0000) >> 16
        view[3] = (payload2 & 0xff000000) >> 24
        view[4] = (payload2 & 0xff00000000) >> 32
        view[5] = (payload2 & 0xff0000000000) >> 40
        view[6] = (payload2 & 0xff000000000000) >> 48
        view[7] = (payload2 & 0xff00000000000000) >> 56

        payload2++

        view = view.reverse()

        CANBusModel.sendFrame(frameId2, b)
    }

    Component.onCompleted: {
        // We have no GUI to display so LogWindow can be visible
        LogWindow.setVisible(true)

        // Connect QML functions to CANBusModel signals
        CANBusModel.simulationStarted.connect(function(){ 
            LogWindow.appendPlainText("Simulation started");

            // Reset payload1 on each simulation start
            // payload2 is persistent across simulation start/end
            payload1 = 0

            timer1.start() 
            timer2.start()
        })
        CANBusModel.simulationStopped.connect(function(){ 
            timer1.stop()
            timer2.stop()
            LogWindow.appendPlainText("Simulation stopped");
        })
    }
}


