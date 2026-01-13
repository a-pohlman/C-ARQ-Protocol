/*
* C-ARQ Testing Code: Node 1
* Original Code by: Cooper J. Koch, Alexander H. Burkholder
* Reprogrammed by: Aaron J. Pohlman
* Beginning Modified Date: 01/12/2026
* Last Modified Date: 01/12/2026
* Version: 0.2
* Description: Sending back requested packets from the base station. These nodes can also become
* a canidate, based on the battery level, to send packets for other nodes. Thus, simulating the 
* idea of Automatic Repeat Request protocol. See README.txt for more information on code and folders
* NOTE: This will be the only Node with comments on code!
*/

// Change below values during testing periods (or for debugging purposes)
// ----------------------------------------------------------------------
// Immutable Values
#define POWER_DRAINED 1 // P // Sets the amount of power drained per transmission of a packet
#define CHARGE_RATE 10 // alpha // amount that the nodes harvest energy per second 

// Mutable Values 
// --------------
// Message strings
String received {""}; // Original message sent from other nodes are base station are stored here (store like ReqP1,NACK or 1,N1 300.12)
String baseMsg {""}; // The first part of the received message (nothing to do with the base station)
String negAckMsg {""}; // The second part of the received message that checks whether a negative acknowlegement has been broadcasted

// Calculation vairables
unsigned long lPacket {0}; // Last time slot that the packet was received
unsigned long cPacket {0}; // Current time slot that a packet was received
float currentBattery {0}; // The current battery capacity
float pastBattery {0}; // The previous battery capacity (from the last read time slot)
float timeDif {0}; // The time difference between cPacket and lPacket
float chargeTime {0}; // tau // The total time the node had time to charge (in seconds)
int waitTime {0}; // The wait time the node should delay itself for (unless otherwise zero)

// Other nodes packets
// bool haveP1 = true; <<< Deleted as we assume that node 1 always has its own packet (same for other nodes)
bool haveP2 = false; // Checks whether we have node 2's packet
bool haveP3 = false; // Checks whether we have node 3's packet

// Serial information setup
void setup() {
  Serial.begin(9600); // Set baud rate to 9600
  Serial.setTimeout(10); // Set the time to read strings before moving on as 10ms
  Serial.println(">>>starting_node_1"); // Display that node 1 has started
}

/**
 * Description: Parses a given string at a selected comma (must be in csv form)
 * @param msg : The string to be parsed
 * @param atComma : The comma to begin parsing at
 * @return : The message that was parsed at given comma
*/
String parseMsg(String msg, unsigned int atComma) {
  String newMsg;
  int commasEncountered {0};
    
  for (unsigned int index {0} ; index < msg.length() ; index++) {
    if (msg[index] == ',') {
      commasEncountered += 1;
      if (atComma != 0) {index += 1;}
    }

    if (commasEncountered == atComma + 1) {break;}    
    
    if (commasEncountered == atComma || atComma == 0) {newMsg += msg[index];}
  }
  return newMsg;
}

// Main loop to be executed during runtime
void loop() {
  if (Serial.available() > 0) { // Checks to see if there is any current transimissions
    
    received = Serial.readStringUntil('\n'); // Get message broadcasted from either other nodes or base station
    Serial.readStringUntil('\n'); // Empty any other potential message
    
    baseMsg = parseMsg(received, 0); // Parse the first part of the message
    negAckMsg = parseMsg(received, 1); // Parse the second part of the message

    cPacket = millis(); // Get the current time that the packet was received
    timeDif = cPacket - lPacket; // Calculate the different from the last time a packet was received
    chargeTime = timeDif / 1000; // Divide by 1000ms to convert the time difference into seconds
    currentBattery = pastBattery + (chargeTime * CHARGE_RATE); // Calculate the current battery level using the formula: past battery level + (alpha*tau)
    if (currentBattery > 1000) {currentBattery = 1000;} // Check to see if battery is at capacity K
    waitTime = 1000 - currentBattery; // Calculate wait time

    // Reset past values to current values for next packet 
    lPacket = cPacket; 
    cPacket = 0;
    pastBattery = currentBattery;  

    // Check to see if any other nodes have broadcasted their packets; 'store' them for later; 'delete' them later
    if (baseMsg == "2") {haveP2 = true;}
    else if (baseMsg == "ReqP1" || baseMsg == "ReqP3") {haveP2 = false;}
                          
    if (baseMsg == "3") {haveP3 = true;}
    else if (baseMsg == "ReqP1" || baseMsg == "ReqP2") {haveP3 = false;}

    // Check to see if this node is a canidate for anthor node
    if (pastBattery-POWER_DRAINED > POWER_DRAINED && negAckMsg == "NACK" && baseMsg != "ReqP1" ) { // Must have enough battery, negative acknowlegement broadcasted, and not requesting our own packet
      Serial.flush(); // Ensure all data has been sent
      while (Serial.available() > 0) {Serial.read();} // Empty serial of any bytes
      if (waitTime != 0) { // Wait unless time is zero 
        delay(waitTime); 
      }
      
      // Check to see if we have the packet and a negative acknowlegement has been broadcasted
      if (haveP2 == true && negAckMsg == "NACK" && Serial.available() == 0) { // Make sure another node has not already broadcasted
        pastBattery -= POWER_DRAINED; // Drain battery for transmission
        // Build statement for transmission and send
        Serial.print("2,N1 ");
        Serial.println(pastBattery);
        Serial.readStringUntil('\n');
        Serial.flush(); // Ensure all data has been sent
      }

      if (haveP3 == true && negAckMsg == "NACK" && Serial.available() == 0) { 
        pastBattery -= POWER_DRAINED;
        Serial.print("3,N1 ");
        Serial.println(pastBattery);
        Serial.readStringUntil('\n');
        Serial.flush();
      }  
    }
    // If a request for our packet is coming from the base station, or we are a canidate for our own packet, then check that it's not other requests, and that we have the battery capacity to transmit
    else if (baseMsg == "ReqP1" && baseMsg != "ReqP2" && baseMsg != "ReqP3" && pastBattery-POWER_DRAINED > POWER_DRAINED) {
      Serial.flush(); // Ensure all data has been sent
      while (Serial.available() > 0) {Serial.read();} // Empty serial of any bytes

      // Check to see if we are the canidate for our own packet, otherwise don't wait to transmit
      if (negAckMsg == "NACK" && waitTime != 0) {
        delay(waitTime);
      }
      else {delay(100);} // Only if battery level is at complete zero, this helps mitgate triplicate transmissions

      // Make sure nothing else has been transmitted 
      if (Serial.available() == 0) { 
        pastBattery -= POWER_DRAINED; // Drain battery
        // Build message and transmit
        Serial.print("1,N1 "); 
        Serial.println(pastBattery);
        Serial.readStringUntil('\n');
        Serial.flush(); // Ensure all data has been sent
      }
    }
  }
}