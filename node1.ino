#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

#define RF95_FREQ 868.0
#define NODE_ID 1
#define NEXT_NODE_ID 2

RH_RF95 rf95(RFM95_CS, RFM95_INT);

uint8_t packet_number = 0;

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
  Serial.println("LoRa Sender Ready.");
}

void loop() {
  uint8_t data[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t path_index = 3; 
  data[0] = NEXT_NODE_ID;
  data[1] = NODE_ID;
  data[2] = packet_number;
  data[path_index] = NODE_ID; 
  path_index++;

  Serial.print("Sending packet ");
  Serial.print(packet_number);
  Serial.println("...");
  if (rf95.send(data, path_index)) {
    Serial.println("Packet sent!");
    packet_number++;
  } else {
    Serial.println("Packet send failed!");
  }
  rf95.waitPacketSent();
  delay(5000);
}
