#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

#define RF95_FREQ 868.0
#define NODE_ID 3
#define NEXT_NODE_ID 4

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  if (!rf95.init()) {
    Serial.println("LoRa initialization failed!");
    while (1);
  }
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("Frequency set failed!");
    while (1);
  }
  Serial.println("LoRa Receiver Ready.");
}

void loop() {
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      uint8_t dest_id = buf[0];
      uint8_t src_id = buf[1];
      uint8_t packet_number = buf[2];

      if (dest_id == NODE_ID) {
        uint8_t path_index = len; 
        buf[path_index] = NODE_ID; 
        path_index++;

        if (NEXT_NODE_ID != 0) {
          buf[0] = NEXT_NODE_ID; 
          Serial.println("Forwarding packet...");
          rf95.send(buf, path_index);
          rf95.waitPacketSent();
        } else {
          Serial.println("Final destination reached.");
          Serial.println("Packet Details:");
          Serial.print("Packet Number: ");
          Serial.println(packet_number);
          Serial.print("Path: ");
          for (uint8_t i = 3; i < path_index; i++) {
            Serial.print(buf[i]);
            if (i < path_index - 1) Serial.print(" -> ");
          }
          Serial.println();
        }
      } else {
        Serial.print("Message not for this node. Destination: ");
        Serial.println(dest_id);
      }
    }
  }
}
