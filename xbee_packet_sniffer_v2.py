from datetime import datetime
from time import sleep
import serial

serial.Serial()

if __name__ == "__main__":

    file_name = input("Enter output file name: ")
    while True:
        file_type = input("Enter file type: ")
        if file_type[0] != '.': print("Error: Missing dot operator on file type!\n Try Again!")
        else: break
        
    save_path = input("File location: ")
    f = open(f'{save_path}\\{file_name}{file_type}', 'w')

    lookout_phrase: str = "n"
    enable_stop = input("Enable stop (y/n): ")
    if enable_stop == "y": lookout_phrase = input("Stop lookout phrase: ")

    enable_progress = input("Enable progress counter? (y/n) [WARNING! MAY BE UNSTABLE] ")
    if enable_progress == "y":
        number_of_packets = input("How many packets are being tested? ")
        scroll_y: str = input("How long would like the scroll region? ")
        packet_indicator: str = input("Input chr or str to look out for packet info: ")
        dial: str = "/"
        percent: int = 0
    
    def parsePacketInfo(msg: str) -> int:
        index: int = 0
        check_exception: int = 0
        number_str: str = ""
        while True:
            if index == msg.__len__():
                break
            try:
                check_exception = int(msg[index])
                number_str += str(msg[index])
            except: pass
            index += 1
        try: return int(number_str)
        except: return 1

    xbee = serial.Serial(port=input("Enter serial port: "), baudrate=9600)
    
    count = 3
    for i in range(3): 
        print(f">>>starting in {count-i}", end="\r")
        sleep(1)
    count = 0

    print("\x1b[2J\x1b[H\x1b[?25l")
    if enable_progress == "y":
        print(f"\x1b[{int(scroll_y)-1}B\x1b7/ Test Completion: 0%")

    while True:
        try:
            count += 1
            data = f'{xbee.readline().decode().rstrip()}'
            f.write(f'{datetime.now()},{count},{data}\n')
            
            if (enable_progress == "y"):
                if data[0] == packet_indicator:
                    percent = round((parsePacketInfo(data)/int(number_of_packets))*100,1)
                match dial:
                    case "/": dial = "—"
                    case "—": dial = "\\"
                    case "\\": dial = "|"
                    case "|": dial = "/"
                print(f"\x1b[1;{scroll_y}r")
                print(f"\x1b[{scroll_y};H")
                print(f'{datetime.now()},{count},{data}', end="")
                print(f"\x1b8 {dial} Test Completion: {percent}%", end="")
            else: print(f'{datetime.now()},{count},{data}')

            if data == lookout_phrase:
                f.close()
                break
        except:
            f.write(f'{datetime.now()},{count}, //Error Reading Data//\n')
            if (enable_progress == "y"):
                match dial:
                    case "/": dial = "—"
                    case "—": dial = "\\"
                    case "\\": dial = "|"
                    case "|": dial = "/"
                print(f"\x1b[1;{scroll_y}r")
                print(f"\x1b[{scroll_y};H")
                print(f"{datetime.now()},{count}, //Error Reading Data//")
                print(f"\x1b8 {dial} Progress: {percent}%", end="")
            else: print(f"{datetime.now()},{count}, //Error Reading Data//")
    f.close()
