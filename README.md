# C-ARQ Protocol + Energy Harvesting 

## Background
Networking protocols are improtant for the basic communication systems that run around the world. There are many different protocols that can be choosen from. This research focuses on one in particular: Cooperative Automatic Repeat Request (C-ARQ). This netowrk protocol works on a acknolowedgement (ACK) and negative-acknowlegment (NACK) system. Where if a packet is receieved successfully by the base station, then it moves on and broadcasts and postive ACK for all nodes to hear, whereas if the node fails to deliver it's packet, the base station will broadcast a NACK. Alongside a NACK, the base station will keep trying until it has reached the maximum amount of attempts before moving on and counting it as a lost packet. The cooperative part, allows other nearby nodes to become canidates for if a node fails to deliver it pack. Essentially making it more likely for a packet to be recieved, rather than not. Combining this with the idea of energy harvesting, it can be an effective strategy in having a network run and deliver packets, without the use of hooking up any nodes to a physical power source. 

The original research was started by **Cooper J. Koch** and **Alexander H. Burkholder** under the research advisor [**Ahmed Ammar**](https://www.onu.edu/directory/ahmed-ammar) at [<ins>Ohio Northern University</ins>](https://www.onu.edu/). The research is now being continued by **Aaron J. Pohlman** under the research advisor [**Ahmed Ammar**](https://www.onu.edu/directory/ahmed-ammar) at [<ins>Ohio Northern University</ins>](https://www.onu.edu/) to further invstigate network trends and simulating trojan attacks. 

## Repository Information
This is **Version 0.2** (located in ARQ_CODE_2 branch on Github) based on the code provided by Cooper. The original code has been significantly reworked for more improved readability, efficency, and fixed major bugs. View sections below for more information. 

## Testing Information
The tests are conducted by using the following equipment:
- Arduino UNO R3 (by Elegoo)
- XBee Pro Arduino UNO R3 Shields
- XBee S2C Radios
- XBee Programming PCB

The tests use the following software to test and collect data:
- base_station.ino
- node_1.ino 
- node_2.ino 
- node_3.ino _(optional)_
- node_4.ino _(optional)_
- xbee_sniffer_v2.py
- XBee Sniffer Interface (XSI) _(optional)_
- XCTU by Digi Mesh

> [!IMPORTANT] 
> **All radio modules were programmed using the DigiMesh 2.4 (9002) Protocol with the XCTU app by Digi**

Testing parameters that change per test
| Variables         |  Unit    |  Symbol |
| :---:             | :---:    | :---:   |
| Nodes             | #        | N       | 
| Pr(Success)       | %        | p       |
| Charge Rate       | charge/s | α       |
| Trojan Time Delay | ms        | t<sub> o</sub>| 

> [!IMPORTANT] 
> During testing, there was no real trojan virus used! Instead it was opted to simply simulate it's time delay such that it acted like a trojan virus

> _NOTE: It should be stated that Pr(Success) is a simulated value in the code that only simulates whether a packet will fail at the base station. The nodes themselves are implicitly set to be always successful in recieving messages from the base station_

Test Constants
| Constants        | Unit     | Symbol  | Values    |
| :---:            | :---:    | :---:   | :---:     |
| Maximum Attempts | #        | M       | 3         |
| Time Delay       | s        | T       | 1         |
| Battery Capacity | charge   | K       | 1000      |
| Power Drained    | charge   | P       | 1         |
| Packets per Test | #        | n/a     | 5000      |

Testing data can be found in the folder named: **test_data**. Read the below seciton to learn how the folder and folder names are structured for data review. Read the rest of the sections if interested in software used to conduct tests and collect test data and updates/changes made to orginal code.

## Testing Data
All test data is stored in this folder and is updated when new curves have been completed. Folders and files should indicate the different types of tests being conducted. 

The following format applies to all file names for tests:

For network test folders, the format is applied:
| name | # of Nodes | Pr(Success) |
| ---- | ---------- | ----------- |

_Ex: networkTest_03_50_

For network file names, the format is applied:
| name | # of Nodes | Pr(Success) | High Alpha | Low Alpha | Raw or Modified | File Type |
| ---- | ---------- | ----------- | -----------| --------- | --------------- | --------- |

_Ex: networkTest_03_50_00_05_raw.csv_

- The above example shows that the test conducted was with 3 Nodes, with a 50% success probability at the base station, with a charge rate of 0.05, as a csv file. 
> _NOTE: If the charge rate were 04_00 in the file name, then it would suggest a charge rate of 4.00_

For trojan test folders, the format is applied:
| name | # of Nodes | Pr(Success) | Time Delay Added | 
| ---- | ---------- | ----------- | ---------------- |

_Ex: networkTest_03_50_500_

For trojan file names, the format is applied:
| name | # of Nodes | Pr(Success) | Time Delay Added | Trial # | High Alpha | Low Alpha | Raw or Modified | File Type |
| ---- | ---------- | ----------- | ---------------- | --------| ---------- | --------- |---------------- | --------- |

_Ex: networkTest_04_50_500_01_00_05_raw.csv_

- The above example shows that the test conducted was with 4 Nodes, with a 50% success probability at the base station, a 500ms delay at the infected node (node 1), trial #1, with a charge rate of 0.05, as a csv file. 
> _NOTE: If the charge rate were 04_00 in the file name, then it would suggest a charge rate of 4.00_

At the end of each test, it displays a set of statistics. Those are the following:
- Total Packets: **Total Test Packets Counted** _(Doesn't have to be the amount set, just close)_
- Successful Packets: **Amount of times the packet was recieved**
- Failed Packets: **Amount of times the base station moved on**
- Delivery Ratio: **The decimal percent value of successful packets divided by total packets**

> [!IMPORTANT] 
> **All test files (except betaTesting files) are in the comma seperated value (csv) format** 

## Base Station Code
Base station to send out and request data from N amount of nodes available to test with. The base station is to help the sniffer Xbeeradio module collect this data for testing purposes. Testing parameters can be changed in the Arduino file to easily simulate different situations.

- A major feature missing from the previous iteration was that the base station can now broadcast a negative acknowledgement when a packet initially fails, allowing all available nodes to become candidates to send their packets as well. 

- Currently, that base station time to send out a request and read any potential data from a node, is theoretically supposed to run at a delay witha 1s delay. However, due to complications and bugs with reading values, in reality, it has been set up to delay around 1.2s. This ensures that data is being read when it is supposed to and not sending packets when not needed.

> _NOTE: Their is only been a slight bit of effort to mitigate duplicate packets!_

## Node Code
There are currently four nodes, which means that there are four seperate arduino codes for each individual node, aptly named node_1, node_2, node_3, and node_4

- The nodes have had severe rework to fix bugs and proper indentation and code syntax to make the program run smoother and improve readability.
- Nodes now recognize that a NACK has been broadcasted by the base station
- Modified code to add up to four nodes
- Added a new variable called: `TROJAN_DELAY`, to simulate trojan attack. This can only be found in node_1.ino

## XBee packet sniffer
xbee_packet_sniffer_v2.py is a Python file was modified to include extra aspects to help make testing easier. The 
python file asks for the following parameters now:

1. File name: _enter the name of the file_ 
2. File type: _file type (e.g. .txt .csv .md)_
3. Full path to file: _path to file (e.g. c:/Users/YOURUSERNAME/DESTINATION)_
4. Enable Stop: _y/n_
	- Enter stop lookout phrase: _stop lookout phrase (currently setup for the phrase: >>>end_of_test)_
5. Enter serial port: _USB port connected to the sniffer (e.g. COM7 [Windows] /dev/ttyUSB0 [Linux Ubuntu])_

> [!TIP]
> External python modules are required: pyserial
>
> To check or attempt to install, try running: 
> ```powershell
> pip install pyserial
> ```

## XBee Sniffer Interface (XSI)
A new program devloped for this project and uses XBee Radio modules. This program was developed in the background during testing and has been continously worked on. It helps manage files easier, make test start faster, checks for specific details, and improves reliability versus the old xbee_packet_sniffer_v2.py file. The program is currently at **Version 0.2.5-beta**. To learn more, visit [here](https://github.com/a-pohlman/XBee-Sniffer-Interface/tree/xsi_windows?tab=readme-ov-file) to learn how to download, use, and test with.









