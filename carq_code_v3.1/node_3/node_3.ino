/*
* C-ARQ Testing Code: Node 3
* Developer: Aaron J. Pohlman
* Beginning Modified Date: 01/28/2026
* Last Modified Date: 04/28/2026
* Version: 0.3.1
* Description: Sending back requested packets from the base station. These nodes can also become
* a canidate, based on the battery level, to send packets for other nodes. Thus, simulating the 
* idea of Cooperative Automatic Repeat Request protocol (C-ARQ). 
* See README.md and LICENSE for more information on code and folders
*/

// Change below values during testing periods (or for debugging purposes)

#define TROJAN_DELAY 0 // t0 // Time delay to simulate an infected node with a trojan virus
#define CHARGE_RATE 10 // alpha // charge rate of the given node
#define POWER_DRAINED 1 // P // amount of power drained for a given transmission

// Values below that should NOT BE CHANGED (unless for debugging purposes)!

String recieved {""}; 
String baseMsg {""};
String negAckMsg {""};

String nodeID {"N3"};

unsigned long lPacket {0};
unsigned long nPacket {0};

unsigned long pTime {0};
unsigned long cTime {0};

float currentBattery {0};
float pastBattery {0};
float timeDif {0};
float chargeTime {0};
int waitTime {0};

bool haveP1 = false;
bool haveP2 = false;
bool haveP4 = false;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(5);
  Serial.println(">>>start_node_3");
}

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
  cTime = millis();

  if (Serial.available() > 0) {
    recieved = Serial.readStringUntil('\n');
        
    baseMsg = parseMsg(recieved, 0);
    negAckMsg = parseMsg(recieved, 1);
    
    nPacket = millis(); 
    timeDif = nPacket - lPacket; 
    currentBattery = pastBattery + ((timeDif / 1000) * CHARGE_RATE); 

    if (currentBattery > 1000) {currentBattery = 1000;} 
    waitTime = 1000 - currentBattery + TROJAN_DELAY; 

    if (negAckMsg == "NACK") {
      pTime = cTime;
    }

    lPacket = nPacket; 
    nPacket = 0;
    pastBattery = currentBattery;  

    if (baseMsg == "1") {haveP1 = true;}
    else if (baseMsg == "ReqP2" || baseMsg == "ReqP3" || baseMsg == "ReqP4") {haveP1 = false;}
                          
    if (baseMsg == "2") {haveP2 = true;}
    else if (baseMsg == "ReqP1" || baseMsg == "ReqP3" || baseMsg == "ReqP4") {haveP2 = false;}

    if (baseMsg == "4") {haveP4 = true;}
    else if (baseMsg == "ReqP1" || baseMsg == "ReqP2" || baseMsg == "ReqP3") {haveP4 = false;}

    if (baseMsg == "ReqP3" && negAckMsg != "NACK" && pastBattery-POWER_DRAINED > POWER_DRAINED) {
      pastBattery -= POWER_DRAINED;
      Serial.print("3,");
      Serial.print(nodeID);
      Serial.print(",");
      Serial.println(pastBattery);
      Serial.flush();
    }
  }
  else if (pastBattery-POWER_DRAINED > POWER_DRAINED && negAckMsg == "NACK" && cTime - pTime >= waitTime) {
    if (baseMsg == "ReqP3" && Serial.available() == 0) {
      pastBattery -= POWER_DRAINED;
      Serial.print("3,");
      Serial.print(nodeID);
      Serial.print(",");
      Serial.println(pastBattery);
    }
    else if (haveP1 == true && negAckMsg == "NACK" && Serial.available() == 0) { 
      pastBattery -= POWER_DRAINED;
      Serial.print("1,");
      Serial.print(nodeID);
      Serial.print(",");
      Serial.println(pastBattery);
    }
    else if (haveP2 == true && negAckMsg == "NACK" && Serial.available() == 0) { 
      pastBattery -= POWER_DRAINED;
      Serial.print("2,");
      Serial.print(nodeID);
      Serial.print(",");
      Serial.println(pastBattery);
    } 
    else if (haveP4 == true && negAckMsg == "NACK" && Serial.available() == 0) {
      pastBattery -= POWER_DRAINED;
      Serial.print("4,");
      Serial.print(nodeID);
      Serial.print(",");
      Serial.println(pastBattery);
    }
    Serial.flush();
    negAckMsg = ""; 
  }
}