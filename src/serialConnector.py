from click import command
import serial
import threading
import settings
# import showBoard
import board

from boardParser import serialTextParser

# on Linux portname = "/dev/ttyUSB0"
port = serial.Serial("COM9", baudrate=38400, timeout=3.0)

if not port.isOpen:
    port.open()

# port.write(b'Azfsdf ')
# # debug
# counter = 0

def receiver(main_win):

    while True:
        command = ""
        player = ""
        text = port.readline()
        text = text.decode("utf-8") 
        if text != '':
            command, player, data = serialTextParser(text=text)
      
        print(text)
        # command, player, data = serialTextParser(text=text)

    # command = 'U'
    # player = 'B'
    # data = [
    #     2, 2, 2, 1, 1, 2, 0, 2,
    #     2, 1, 1, 1, 1, 1, 0, 2,
    #     0, 0, 0, 1, 1, 2, 2, 1,
    #     2, 1, 2, 1, 2, 2, 2, 2,
    #     0, 0, 0, 0, 0, 0, 0, 0,
    #     0, 0, 2, 1, 1, 1, 0, 1,
    #     1, 1, 0, 0, 1, 0, 0, 0,
    #     1, 1, 1, 1, 1, 1, 1, 1,
    # ]

        if command == 'U':
            data = list(map(lambda x: int(x), data))
            # board.setBoard(data)
            main_win.ui.setBoard(board=data, player=player)
            main_win.ui.setScores(data=data)

            with open("../save/save.txt", 'w') as file: 
                file.write(text)

        elif command == 'V':
            pass
            # main_win.ui.moveValidated(data, player)
        elif command == 'I':
            main_win.ui.moveInvalid(data, player)

        elif command == 'R':
            main_win.ui.setResult(winner=player)


def sender(command: str):
    try:
        port.write(f"{command}\n".encode())
    except:
        print(f"writing in port was failed!. Expected command: {command}\n")