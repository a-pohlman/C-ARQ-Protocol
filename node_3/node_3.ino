/*
* C-ARQ Testing Code: Node 3
* Original Code by: Cooper J. Koch, Alexander H. Burkholder
* Reprogrammed by: Aaron J. Pohlman
* Beginning Modified Date: 01/12/2026
* Last Modified Date: 01/12/2026
* Version: 0.2
* Description: Sending back requested packets from the base station. These nodes can also become
* a canidate, based on the battery level, to send packets for other nodes. Thus, simulating the 
* idea of Automatic Repeat Request protocol. See README.txt for more information on code and folders
* NOTE: This will not have comments on it! See node_1.ino for comments and details on code!
*/

// Change below values during testing periods (or for debugging purposes)
// ----------------------------------------------------------------------
#define POWER_DRAINED 1
#define CHARGE_RATE 10

String received {""};
String baseMsg {""};
String negAckMsg {""};

unsigned long lPacket {0};
unsigned long cPacket {0};
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
  Serial.setTimeout(10);
  Serial.println(">>>start_node_3");
}

String parseMsg(String msg, int atComma) {
  String newMsg;
  int commasEncountered {0};
    
  for (unsigned int index {0} ; index < msg.length() ; index++) {
    if (msg[index] == ',') {
      commasEncountered += 1;
      if (atComma != 0) {index += 1;}
    }

    if (commasEncountered == atComma + 1) {break;}    

    if (commasEncountered == atComma || atComma == 0) { newMsg += msg[index];}
  }
  return newMsg;
}

void loop() {
  if (Serial.available() > 0) {
    
    received = Serial.readStringUntil('\n');
    Serial.readStringUntil('\n');
    
    baseMsg = parseMsg(received, 0);
    negAckMsg = parseMsg(received, 1);

    cPacket = millis(); 
    timeDif = cPacket - lPacket; 
    chargeTime = timeDif / 1000; 
    currentBattery = pastBattery + (chargeTime * CHARGE_RATE); 
    if (currentBattery > 1000) {currentBattery = 1000;} 
    waitTime = 1000 - currentBattery; 

    lPacket = cPacket; 
    cPacket = 0;
    pastBattery = currentBattery;  

    if (baseMsg == "1") {haveP1 = true;}
    else if (baseMsg == "ReqP2" || baseMsg == "ReqP3" || baseMsg == "ReqP4") {haveP1 = false;}
                          
    if (baseMsg == "2") {haveP2 = true;}
    else if (baseMsg == "ReqP1" || baseMsg == "ReqP3" || baseMsg == "ReqP4") {haveP2 = false;}

    if (baseMsg == "4") {haveP4 = true;}
    else if (baseMsg == "ReqP1" || baseMsg == "ReqP2" || baseMsg == "ReqP3") {haveP4 = false;}


    if (pastBattery-POWER_DRAINED > POWER_DRAINED && negAckMsg == "NACK" && baseMsg != "ReqP3" ) {
      Serial.flush();
      while (Serial.available() > 0) {Serial.read();}
      if (waitTime != 0) {
        delay(waitTime);
      }
      
      if (haveP1 == true && negAckMsg == "NACK" && Serial.available() == 0) { 
        pastBattery -= POWER_DRAINED;
        Serial.print("1,N3 ");
        Serial.println(pastBattery);
        Serial.readStringUntil('\n');
        Serial.flush();
      }

      if (haveP2 == true && negAckMsg == "NACK" && Serial.available() == 0) { 
        pastBattery -= POWER_DRAINED;
        Serial.print("2,N3 ");
        Serial.println(pastBattery);
        Serial.readStringUntil('\n');
        Serial.flush();
      }  

      if (haveP4 == true && negAckMsg == "NACK" && Serial.available() == 0) { 
        pastBattery -= POWER_DRAINED;
        Serial.print("4,N3 ");
        Serial.println(pastBattery);
        Serial.readStringUntil('\n');
        Serial.flush();
      }  
    }
    else if (baseMsg == "ReqP3" && baseMsg != "ReqP1" && baseMsg != "ReqP2" && baseMsg != "ReqP4" && pastBattery-POWER_DRAINED > POWER_DRAINED) {
      Serial.flush();
      while (Serial.available() > 0) {Serial.read();}

      if (negAckMsg == "NACK" && waitTime != 0) {
        delay(waitTime);
      }
      else {delay(100);}

      if (Serial.available() == 0) { 
        pastBattery -= POWER_DRAINED;
        Serial.print("3,N3 ");
        Serial.println(pastBattery);
        Serial.readStringUntil('\n');
        Serial.flush();
      }
    }
  }
}
