# Author: Rembie01
# Date: 8 dec 2020
# Source:https://forum.arduino.cc/index.php?topic=225329.msg1810764#msg1810764
import serial
import time


class ArduinoCom:
    def __init__(self, port="/dev/ttyACM0", baudrate=115200):
        self.ser = serial.Serial(port, baudrate, timeout=1)
        self._startmarker = 60
        self._endmarker = 62

    def send_to_arduino(self, string):
        self.ser.write(string.encode('utf-8'))

    def receive_from_arduino(self):
        ck = ""
        x = "z"
        bytecount = -1

        while ord(x) != self._startmarker:
            x = self.ser.read()

        while ord(x) != self._endmarker:
            if ord(x) != self._startmarker:
                ck = ck + x.decode("utf-8")
                bytecount += 1
            x = self.ser.read()

        return ck

    def wait_for_arduino(self):
        msg = ""
        while msg.find("Arduino is ready") == -1:
            while self.ser.inWaiting() == 0:
                pass
            msg = self.receive_from_arduino()
            print(msg)

    def send(self, string):
        wait_for_reply = False

        if not wait_for_reply:
            self.send_to_arduino(string)
            print("Sent from PC -- " + string)
            wait_for_reply = True

        if wait_for_reply:
            while self.ser.inWaiting() == 0:
                pass

            received = self.receive_from_arduino()
            print("Reply Received -- " + received)
