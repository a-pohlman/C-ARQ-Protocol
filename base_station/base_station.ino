/*
* C-ARQ Testing Code: Base Station
* Original Code by: Cooper J. Koch, Alexander H. Burkholder
* Reprogrammed by: Aaron J. Pohlman
* Beginning Modified Date: 01/05/2026
* Last Modified Date: 01/11/2026
* Version: 0.2
* Description: Base station to expiermentally request and send packets from N amount of nodes for a number of packets to 
* see how well the protocol can perform. Based on the paper written by Cooper J. Koch, Alexander H. Burkholder, and Ahmed Ammar.
* See README.txt for more information about code and folders
*/

// Change below values during testing periods (or for debugging purposes)
// Immutable Values
#define AVAILABLE_NODES 4 // N // The amount of nodes available to trasnmit to the base station
#define MAXIMUM_ATTEMPTS 3 // M // Maximimum amount of attempts the base station attempts before moving to the next node
#define MAXIMUM_TEST_PACKETS 5000 // Maximum amount of packets to be tested by base station
#define WAIT_TIME 1000 // T // Delay the base station waits before reading antoher value
#define SUCCESS_THRESHOLD 80 // Likelyhood that a channel could fail (if at 80, then their is an 80% chance a packet will be acknowledged)

// Mutable Values
int atNode {1}; // i // The current node that the base station is on
int nodeAttempt {0}; // m // the current attempt that the base station is on at node i

// Used for statstics at the end
int successfulPackets {0}; 
int failedPackets {0};
int totalPackets {0}; // NOTE: May not end up being the same as MAXIMUM_TEST_PACKETS

String receivedMessage; // Used to store packets sent from N nodes

// Used to control other conditionals
bool negativeAck {false}; // Identifies if a negative acknlowgedment needs to be broadcasted
bool stop {false}; // Used to stop sending info into the network 

// Setup Arduino Serial information with random number seed generator
void setup() {
  Serial.begin(9600); // Set buad rate to 9600
  Serial.setTimeout(100); // Sets time to wait (for 100ms) for readStringUntil() method before continuing to read data
  randomSeed(analogRead(0)); // Sets the seed of the random number generator to the analog pin A0 on the Arduino Board
  Serial.println(">>>starting_new_test"); // Displays that the test is beginning
  delay(4000); // Wait 8 seconds for all nodes to setup along with sniffer 
}

// Main execution to run infitely
void loop() {
  if (totalPackets < MAXIMUM_TEST_PACKETS) { // Checks to see if the test is over
    for (atNode = 1; atNode < AVAILABLE_NODES+1; atNode++) { // Runs through each node
      for (nodeAttempt = 0; nodeAttempt < MAXIMUM_ATTEMPTS; nodeAttempt++) { // Runs through the set amount of attempts  
        while (Serial.available() > 0) {Serial.read();}  // Empties the serial of any bytes left (if any)
        Serial.readStringUntil('\n'); // Ensures nothing is available to read // 100ms
        receivedMessage = ""; // Resets the message to be stored 

        // Switches to check which node to request from
        switch (atNode) {
          case 1:
            switch (negativeAck) { // Checks whether to broadcast a negative acknowledgement
              case 1:
                Serial.println("ReqP1,NACK,");
                break;
              case 0:
                Serial.println("ReqP1,");
                break;
            }
            break;
            
          case 2:
            switch (negativeAck) {
              case 1:
                Serial.println("ReqP2,NACK,");
                break;
              case 0:
                Serial.println("ReqP2,");
                break;
            }
            break;

          case 3:
            switch (negativeAck) {
              case 1:
                Serial.println("ReqP3,NACK,");
                break;
              case 0:
                Serial.println("ReqP3,");
                break;
            }
            break;

          case 4:
            switch (negativeAck) {
              case 1:
                Serial.println("ReqP4,NACK,");
                break;
              case 0:
                Serial.println("ReqP4,");
                break;
            }
            break;
          }

        Serial.flush(); // Ensure all data has been sent
        delay(WAIT_TIME); // Wait time T (set at the top) to wait for a node to respond
        Serial.flush(); // Ensure all data has been sent
       
        receivedMessage = Serial.readStringUntil(','); // Read message if available from any node // 100ms

        // Check if the recieved message is the correct packet, from the correct node, and see if the channel has a failure
        if (receivedMessage == "1" && atNode == 1 && random(100) <= SUCCESS_THRESHOLD) {
          Serial.println("ACKP1,"); // Print acknolwegement for other nodes to hear
          successfulPackets++; // Increase successful packets by one
          break; // Break out of attempts for loop and move to the next node
        }
        else if (receivedMessage == "2" && atNode == 2 && random(100) <= SUCCESS_THRESHOLD) {
          Serial.println("ACKP2,");
          successfulPackets++;
          break;
        }
        else if (receivedMessage == "3" && atNode == 3 && random(100) <= SUCCESS_THRESHOLD) {
          Serial.println("ACKP3,"); 
          successfulPackets++;
          break;
        }
        else if (receivedMessage == "4" && atNode == 4 && random(100) <= SUCCESS_THRESHOLD) {
          Serial.println("ACKP4,");
          successfulPackets++;
          break;
        }
        else if (nodeAttempt < MAXIMUM_ATTEMPTS-1) {negativeAck = true;} // If none of the above are true, then broadcast negative acknowlegement and try again
      } 
      if (nodeAttempt == MAXIMUM_ATTEMPTS) {failedPackets++;} // If reached (M) the maximum attempts then increase failed packets by one
      totalPackets++; // Incrase total packets by one
      negativeAck = false; // Reset negative acknowlegement for next node

      // Display that we've completed a packet (whether it was lost or not does not matter, indicates that we are moving onto the next packet regardless)
      Serial.print("...P");
      Serial.print(totalPackets);
      Serial.println("...");
      Serial.flush(); // Ensure all data has been sent
    }
  }
  
  // If we've reached the end of the test then dispaly results below
  if (stop == false && totalPackets >= MAXIMUM_TEST_PACKETS) {
    Serial.println(">>>results");

    Serial.print("total_packets: ");
    Serial.println(totalPackets);

    Serial.print("successful_packets: ");
    Serial.println(successfulPackets);

    Serial.print("failed_packets: ");
    Serial.println(failedPackets);

    Serial.print("delivery_ratio: ");
    Serial.println(static_cast<float>(successfulPackets)/totalPackets, 3); // Ensure that Arduino is performing floating point operations with integers than other methods and send out message (C++ Method)
    // format: static_cast<data_type>(number1) / number2

    Serial.println(">>>end_of_test"); // Use this with the xbee_packet_sniffer_v2.py to end the test automatically, input as the stop_lookout_phrase
    stop = true; // Set this to true to only display the results once
    // Arduino (Base Station) will keep running after this, just not printing or sending anything out
  }
}
