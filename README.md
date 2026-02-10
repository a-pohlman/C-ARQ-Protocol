/*=============== C-ARQ + Energy Harvesting README.txt ===============*/

Date Originally Made: 01/11/2026
Final Modified Date: 01/11/2026

- This document serves to provide more information as to the files 
contained within. This is version 0.2 based on the code provided by 
Cooper J. Koch, Alexander H. Burkholder under the research advisor 
Ahmed Ammar. The code has been significantly rewritten with new comments
and variable changes to the Base Station and 3 Nodes, written by 
Aaron J. Pohlman. Any other code or files have been either modified or 
collected by Aaron J. Pohlman. 

>>>>>>> base_station / base_station.ino <<<<<<<
- Base station to send out and request data from N amount of nodes 
available to test with. The base station is to help the sniffer Xbee
radio module collect this data for testing purposes. Testing parameters
can be changed in the Arduino file to easily simulate different situations.

- A major features missing from the previous iteration written be Cooper,
is that the base station can now broadcast a negative acknowledgement when 
a packet initially fails, allowing all available nodes to become candidates 
to send their packets as well. 

- Currently, that base station time to send out a request and read any 
potential data from a node, is theoretically supposed to run at a delay with
a 1s delay. However, due to complications and bugs with reading values, 
in reality, it has been set up to delay around 1.2s; this ensures that data
is being read when it is supposed to and not sending packets when not needed.

NOTE: Their is only been a slight bit of effort to mitigate duplicate packets!

>>>>>>> node_N / node_N.ino <<<<<<<
- Node N (for a specific amount of nodes available) sends packets to the 
base station when requested. The sniffer Xbee radio module also reads values
that the nodes send back to the base station, combining both when collecting 
data for further data research.

- The nodes have had severe rework to fix bugs and proper indentation to make
the code run better and simplify down the code. From what could be observed from
the previous iteration of the code, the nodes seemingly never selected any 
candidates for a negative acknowledgement or could never select any as they 
never used this during testing. 

>>>>>>> test_data <<<<<<<
- Where all test data should be stored, folders and files should indicate 
the different types of tests being conducted. The following format applies 
to all file names for tests:

# <test name>_<amount of nodes>_<percent success set>_<charge rate set>_<if its modified>.<file type>
# Ex: betaTesting_03_80_0.15_raw.txt

- At the end of each test, it displays a set of statistics. Those are the following:
# Total Packets: <amount of total packets>
# Successful Packets: <packets that passed and broadcasted an ACK>
# Failed Packets: <packets that failed more than three times at a node>
# Delivery Ratio: <successful packets / total packets> (The percentage of packets that actually passed versus total packets sent out)

>>>>>>> xbee_packet_sniffer_v2.py <<<<<<<

- Python file modified by Aaron J. Pohlman to include extra aspects to help make testing easier. The 
python file asks for the following parameters now:
# File name: <enter the name of the file> 
# File type: <file type> (e.g. .txt .csv .md)
# Full path to file: <path to file> (e.g. c:/Users/YOURUSERNAME/OneDrive/Documents/Arduino/arq_code_v2/test_data)
# Enable Stop: <y/n>
	# If enabled stop, Enter stop lookout phrase: <stop lookout phrase> (currently setup for the phrase: >>>end_of_test)
# Enter serial port: <USB port connected to the sniffer> (e.g. COM7 [Windows] /dev/ttyUSB0 [Linux Ubuntu))

- External Python modules required:
	+ pyserial
	==> pip install pyserial


