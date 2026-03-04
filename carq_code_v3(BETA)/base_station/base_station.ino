/*
* C-ARQ Testing Code: Base Station
* Developer: Aaron J. Pohlman
* Beginning Modified Date: 01/28/2026
* Last Modified Date: 03/03/2026
* Version: 0.3
* Description: Base station to expiermentally request and send packets from N amount of nodes for a number of packets to 
* see how well the protocol can perform. Based on the paper written by Cooper J. Koch, Alexander H. Burkholder, and Ahmed Ammar.
* See README.md and LICENSE for more information about code and folders
*/

// Change below values during testing periods (or for debugging purposes)

#define AVAILABLE_NODES 4 // N // The amount of nodes available to transmit to the base station
#define MAXIMUM_ATTEMPTS 3 // M // Max amount of attempts the base station can fail before moving on
#define MAXIMUM_TEST_PACKETS 5000 // Max amount of packets to be tested before sending results // Use this in combination 
// with the auto end feature in XSI
#define WAIT_TIME 1000 // T // Time the Serial should theoretically wait before taking its read value, actual wait time is around this + 200ms
#define SUCCESS_THRESHOLD 80 // p // Likelyhood that a channel could fail (if at 80, then their is an 80% chance a packet will be acknowledged)

// Values below that should NOT BE CHANGED (unless for debugging purposes)!

int atNode {1}; // i // current node the loop is at
int attempt {1}; // m // current attempt the loop is at

String received {""}; // stores a full message from any node here
long convertedPacket {0}; // converts the received message as a converted integer

unsigned int randomSuccess {0}; // stores random numbers
unsigned long pTime {0}; // previous time // MUST BE UL, otherwise rounding and math errors could occur!
unsigned long cTime {0}; // current time // MUST BE UL, otherwise rounding and math errors could occur!

// statistics 
int successfulPackets {0};
int failedPackets {0};
int totalPackets {0};
int successAttempt {0};

// loop booleans
bool negativeAck {false};
bool stop {false};

void setup() {
  Serial.begin(9600); // Begin serial connection // 9600 symbols per second
  Serial.setTimeout(WAIT_TIME+200); // Set the max amount of time that any read function will take before moving 
  // on in the base station // blocking function
  randomSeed(analogRead(0)); // Set the random seed generator to a open analog pin on Arduino board
  delay(4000); // wait 4000ms before starting test
  Serial.println(">>>start_test"); // broadcast that test has started at the base station 
}

/**
 * Description: Prints out the packet number to the serial
*/
void printPacketNumber(void) {
  Serial.print("...P");
  Serial.print(totalPackets);
  Serial.println("...");
}

/**
 * Description: Parses a given string at a selected comma (must be in csv form)
 * @param msg : The string to be parsed
 * @param atComma : The comma to begin parsing at
 * @return : The message that was parsed at given comma
*/
String parseMsg(String msg, int atComma) {
  String newMsg;
  int commasEncountered {0};
    
  for (unsigned int index {0} ; index < msg.length() ; index++) {
    if (msg[index] == ',') {
      commasEncountered += 1;
      if (atComma != 0) {
        index += 1;
      }
    }
    if (commasEncountered == atComma + 1) {break;}
    if (commasEncountered == atComma || atComma == 0) {newMsg += msg[index];}
  }
  return newMsg;
}

// Run Arduino boards mainloop
void loop() {
  // NOTE: these variables are updating as fast at the Arduino can physically run; not blocked by anything!
  if (!stop) { // stop condition ensures that the arduino will not run forever, especially this code
    cTime = millis(); // get the current time
    randomSuccess = random(100); // get a random number
  }
  
  if (cTime - pTime >= WAIT_TIME && !stop) { // if the wait time has been reached run the following:
    Serial.print("ReqP"); // start building message 
    Serial.print(atNode); // at the current node

    switch (negativeAck) { // check if a neck acknowlegement has been requested
      case 0: // if not, put a comma
        Serial.println(",");
        break;
      case 1: // if so, attach the NACK to the ReqP(atNode) of the message
        Serial.println(",NACK,");
        break;
    }

    Serial.flush(); // ensure all data has been sent
    received = Serial.readString(); // read any incoming messages for 1200ms
    received = parseMsg(received,0); // parse only the first part of the message
    convertedPacket = received.toInt(); // convert the message to an integer

    // checks whether the packet is correct for the node and if the channel has a success
    if (convertedPacket == atNode && randomSuccess <= SUCCESS_THRESHOLD) { // If so
      Serial.println("ACK"); // broadcast acknowledgment
      negativeAck = false; // set NACK flag to false
      successfulPackets++; // up successful packets by one
      totalPackets++; // up total packets by one
      atNode++; // up which node will run next
      attempt = 1; // set attempt flag back to 1
      if (atNode > AVAILABLE_NODES) {atNode = 1;} // check to make sure we are not going over max number of nodes
      printPacketNumber(); // print packet number that was just finished
    }
    else if (attempt == MAXIMUM_ATTEMPTS) { // if the max attempts has been readched, then:
      negativeAck = false; // set NACK flag to false
      failedPackets++; // up failed packets
      totalPackets++; // up total packets
      atNode++; // up which node will run next
      attempt = 1; // set attempt flag back to 1
      if (atNode > AVAILABLE_NODES) {atNode = 1;} // check to make sure we are not going over max number of nodes
      printPacketNumber(); // print packet number that was just finished
    }
    else { // if max attempts has not been reached but channel still failed, then:
      negativeAck = true; // set NACK flag to true
      attempt++; // increase the amount of attempts that have been made
      }

    pTime = cTime; // set previous time to the last read current time for next round
  }

  // If the test has reached its max packets then print the results of the test:
  if (!stop && totalPackets >= MAXIMUM_TEST_PACKETS) {
    Serial.println(">>>results_of_test");
    Serial.print("total_packets: ");
    Serial.println(totalPackets);
    
    Serial.print("successful_packets: ");
    Serial.println(successfulPackets);

    Serial.print("failed_packets: ");
    Serial.println(failedPackets);

    Serial.print("deilvery_ratio: ");
    Serial.println(static_cast<float>(successfulPackets)/totalPackets, 3);
    //             ^^^^^^^^^^^^^^^^^^                   ^              ^
    //             Helps convert our division of two integers into floats, and prints to 3 decimals

    Serial.println(">>>end_of_test"); 

    stop = true; // set stop flag to true
    // the code will now continously run, but wont do anything after this!
  }
}