/*
* C-ARQ Testing Code: Node 1
* Developer: Aaron J. Pohlman
* Beginning Modified Date: 01/28/2026
* Last Modified Date: 04/28/2026
* Version: 0.3.1
* Description: Sending back requested packets from the base station. These nodes can also become
* a canidate, based on the battery level, to send packets for other nodes. Thus, simulating the 
* idea of Cooperative Automatic Repeat Request protocol (C-ARQ). 
* See README.md and LICENSE for more information on code and folders
* NOTE: This will be the only node with comments on code!
*/

// Change below values during testing periods (or for debugging purposes)

#define TROJAN_DELAY 0 // t0 // Time delay to simulate an infected node with a trojan virus (ms)
#define CHARGE_RATE 10 // alpha // charge rate of the given node
#define POWER_DRAINED 1 // P // amount of power drained for a given transmission
#define SPOOFING false // Turns on or off spoofing mode

// Values below that should NOT BE CHANGED (unless for debugging purposes)!
String recieved {""}; // full message recieved either from the base station or other nodes
String baseMsg {""}; // first part of the message
String negAckMsg {""}; // detects if a NACK flag has been sent out by base station

String nodeID {"N1"}; // Sets the nodeID

unsigned long lPacket {0}; // gets the time the last packet was read
unsigned long nPacket {0}; // gets the time the current packet was read

unsigned long pTime {0}; // gets the time previous time 
unsigned long cTime {0}; // gets the current time

// battery calculations
float currentBattery {0};
float pastBattery {0};
float timeDif {0};
int waitTime {0};

// detects which packet the node got or has
bool haveP2 = false;
bool haveP3 = false;
bool haveP4 = false;
// this can be easily extend for more than 4 nodes

void setup() {
  Serial.begin(9600); // set the baud rate to 9600 (symbols per second)
  Serial.setTimeout(5); // set the max time before moving on (5ms)
  Serial.println(">>>start_node_1"); // broadcast that node 1 has started 
  if (SPOOFING) {nodeID = "NM";} // Checks to see if spoofing is enabled
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

void loop() {
  cTime = millis(); // get the current time

  if (Serial.available() > 0) { // if anything is available in the serial then:
    recieved = Serial.readStringUntil('\n'); // get a full message
        
    baseMsg = parseMsg(recieved, 0); // parse the base message
    negAckMsg = parseMsg(recieved, 1); // parse if a NACK flag has been sent
    
    nPacket = millis(); // get the current time that a packet was sent now
    timeDif = nPacket - lPacket; // calculate the time difference between now and last time a packet was sent
    currentBattery = pastBattery + ((timeDif / 1000) * CHARGE_RATE); // calculate current battery

    if (currentBattery > 1000 || SPOOFING == true) {currentBattery = 1000;} // check to make sure that we are not going over capacity
    waitTime = 1000 - currentBattery + TROJAN_DELAY; // calculate the wait time for canidate status

    if (negAckMsg == "NACK") { // if the NACK flag is true, then:
      pTime = cTime; // set previous time to current time
    }

    // reset packet time values
    lPacket = nPacket;
    nPacket = 0;
   
    pastBattery = currentBattery; // set current battery to past battery

    // check to see if we have packet 2
    if (baseMsg == "2") {haveP2 = true;}
    else if (baseMsg == "ReqP1" || baseMsg == "ReqP3" || baseMsg == "ReqP4") {haveP2 = false;}

    // check to see if we have packet 3                 
    if (baseMsg == "3") {haveP3 = true;}
    else if (baseMsg == "ReqP1" || baseMsg == "ReqP2" || baseMsg == "ReqP4") {haveP3 = false;}

    // check to see if we have packet 4
    if (baseMsg == "4") {haveP4 = true;}
    else if (baseMsg == "ReqP1" || baseMsg == "ReqP2" || baseMsg == "ReqP3") {haveP4 = false;}

    // if we have no NACK flag, then we are likely on the first Req
    if (baseMsg == "ReqP1" && negAckMsg != "NACK" && pastBattery-POWER_DRAINED > POWER_DRAINED && SPOOFING == false) { // check we are on the first Req (will not send if spoofing is enabled!)
      pastBattery -= POWER_DRAINED; // drain battery for transmission cost
      // build transmission and send
      Serial.print("1,");
      Serial.print(nodeID);
      Serial.print(",");
      Serial.println(pastBattery);
      Serial.flush(); // ensure all data has been sent
    }
  }
  // This part is simulating the "timer." Check whether the NACK flag is true, that we have reached the wait time
  else if (pastBattery-POWER_DRAINED > POWER_DRAINED && negAckMsg == "NACK" && cTime - pTime >= waitTime) {
    // If we can transmit, then, check which one we have become a canidate for
    // Ensure that we are either a canidate for ourselves or another node and that no other node has sent anything
    
    if (baseMsg == "ReqP1" && negAckMsg == "NACK") {
      pastBattery -= POWER_DRAINED; // drain battery for transmission cost
      // build transmission and send
      Serial.print("1,");
      Serial.print(nodeID);
      Serial.print(","); 
      Serial.println(pastBattery);
    }
    else if (haveP2 == true && negAckMsg == "NACK") { 
      pastBattery -= POWER_DRAINED;
      Serial.print("2,");
      Serial.print(nodeID);
      Serial.print(",");
      Serial.println(pastBattery);
    }
    else if (haveP3 == true && negAckMsg == "NACK") { 
      pastBattery -= POWER_DRAINED;
      Serial.print("3,");
      Serial.print(nodeID);
      Serial.print(",");
      Serial.println(pastBattery);
    } 
    else if (haveP4 == true && negAckMsg == "NACK") {
      pastBattery -= POWER_DRAINED;
      Serial.print("4,");
      Serial.print(nodeID);
      Serial.print(",");
      Serial.println(pastBattery);
    }
    Serial.flush(); // ensure all data has been sent
    negAckMsg = ""; // reset NACK flag so this does not infintely run (very important)!
  }
}