from click import command
import settings

def serialTextParser(text: str):
    signs = text.split(" ")
    player = ""

    if signs[0] == 'U':
        print('siggggggn ', signs)
        if signs[len(signs) - 2] == 'T':
            if signs[len(signs) - 1] == '0\n':
                print("injaaaaa")
                player = 'W'
                settings.PLAYER = 'W'
            elif signs[len(signs) - 1] == '1\n':
                print("oonjaaaa")
                player = 'B'
                settings.PLAYER = 'B'
        data = signs[1]
        print("tuuuuuuuuuuuuuuuuuple", (signs[0], player, data))
        return (signs[0], player, data)
    elif signs[0] == 'I' or signs[0] == 'V':
        validity = signs[0]
        place    = signs[1]
        settings.PLAYER = player = signs[2]
        return (validity, player, place)

    elif signs[0] == 'S':
        with open("save.txt", 'w') as file:
            file.write(text)
    elif signs[0] == "L":
        with open("save.txt", 'r') as file:
            file.readline()
    
    elif signs[0] == 'R':
        settings.WINNER = 0
        command = signs[0]
        player = signs[1]
        
        return (command, player, "")



def getSquareValue(board, square):
    returnVal = 100

    r1 = board[0:8]
    r2 = board[8:16]
    r3 = board[16:24]
    r4 = board[24:32]
    r5 = board[32:40]
    r6 = board[40:48]
    r7 = board[48:56]
    r8 = board[56:64]

    if square[1] == '1':
        if square[0] == 'a':
            returnVal = r1[0]
        elif square[0] == 'b':
            returnVal = r1[1]
        elif square[0] == 'c':
            returnVal = r1[2]
        elif square[0] == 'd':
            returnVal = r1[3]
        elif square[0] == 'e':
            returnVal = r1[4]
        elif square[0] == 'f':
            returnVal = r1[5]
        elif square[0] == 'g':
            returnVal = r1[6]
        elif square[0] == 'h':
            returnVal = r1[7]
    
    if square[1] == '2':
        if square[0] == 'a':
            returnVal = r2[0]
        elif square[0] == 'b':
            returnVal = r2[1]
        elif square[0] == 'c':
            returnVal = r2[2]
        elif square[0] == 'd':
            returnVal = r2[3]
        elif square[0] == 'e':
            returnVal = r2[4]
        elif square[0] == 'f':
            returnVal = r2[5]
        elif square[0] == 'g':
            returnVal = r2[6]
        elif square[0] == 'h':
            returnVal = r2[7]

    if square[1] == '3':
        if square[0] == 'a':
            returnVal = r3[0]
        elif square[0] == 'b':
            returnVal = r3[1]
        elif square[0] == 'c':
            returnVal = r3[2]
        elif square[0] == 'd':
            returnVal = r3[3]
        elif square[0] == 'e':
            returnVal = r3[4]
        elif square[0] == 'f':
            returnVal = r3[5]
        elif square[0] == 'g':
            returnVal = r3[6]
        elif square[0] == 'h':
            returnVal = r3[7]

    if square[1] == '4':
        if square[0] == 'a':
            returnVal = r4[0]
        elif square[0] == 'b':
            returnVal = r4[1]
        elif square[0] == 'c':
            returnVal = r4[2]
        elif square[0] == 'd':
            returnVal = r4[3]
        elif square[0] == 'e':
            returnVal = r4[4]
        elif square[0] == 'f':
            returnVal = r4[5]
        elif square[0] == 'g':
            returnVal = r4[6]
        elif square[0] == 'h':
            returnVal = r4[7]
    
    if square[1] == '5':
        if square[0] == 'a':
            returnVal = r5[0]
        elif square[0] == 'b':
            returnVal = r5[1]
        elif square[0] == 'c':
            returnVal = r5[2]
        elif square[0] == 'd':
            returnVal = r5[3]
        elif square[0] == 'e':
            returnVal = r5[4]
        elif square[0] == 'f':
            returnVal = r5[5]
        elif square[0] == 'g':
            returnVal = r5[6]
        elif square[0] == 'h':
            returnVal = r5[7]
    
    if square[1] == '6':
        if square[0] == 'a':
            returnVal = r6[0]
        elif square[0] == 'b':
            returnVal = r6[1]
        elif square[0] == 'c':
            returnVal = r6[2]
        elif square[0] == 'd':
            returnVal = r6[3]
        elif square[0] == 'e':
            returnVal = r6[4]
        elif square[0] == 'f':
            returnVal = r6[5]
        elif square[0] == 'g':
            returnVal = r6[6]
        elif square[0] == 'h':
            returnVal = r6[7]

    if square[1] == '7':
        if square[0] == 'a':
            returnVal = r7[0]
        elif square[0] == 'b':
            returnVal = r7[1]
        elif square[0] == 'c':
            returnVal = r7[2]
        elif square[0] == 'd':
            returnVal = r7[3]
        elif square[0] == 'e':
            returnVal = r7[4]
        elif square[0] == 'f':
            returnVal = r7[5]
        elif square[0] == 'g':
            returnVal = r7[6]
        elif square[0] == 'h':
            returnVal = r7[7]
    
    if square[1] == '8':
        if square[0] == 'a':
            returnVal = r8[0]
        elif square[0] == 'b':
            returnVal = r8[1]
        elif square[0] == 'c':
            returnVal = r8[2]
        elif square[0] == 'd':
            returnVal = r8[3]
        elif square[0] == 'e':
            returnVal = r8[4]
        elif square[0] == 'f':
            returnVal = r8[5]
        elif square[0] == 'g':
            returnVal = r8[6]
        elif square[0] == 'h':
            returnVal = r8[7]

    print(f"squre {square}: {returnVal}")

    return returnVal
