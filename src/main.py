import sys
import threading

import serial
from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QApplication, QDialog, QMainWindow

import serialConnector
from board import Ui_boardPage



class MainWindow:
    def __init__(self) -> None:
        self.main_win = QMainWindow()
        self.ui = Ui_boardPage()
        self.ui.setupUi(self.main_win)

    def show(self):
        self.main_win.show()


if __name__ == "__main__":

    app = QApplication(sys.argv)
    widget = QtWidgets.QStackedWidget()
    main_win = MainWindow()
    main_win.show()

    serial_thread = threading.Thread(target=serialConnector.receiver, args=(main_win, ))
    serial_thread.start()

    try:
        sys.exit(app.exec_())
    except:
        exit()
    
