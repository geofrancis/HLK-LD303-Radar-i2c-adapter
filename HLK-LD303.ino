#include <Wire.h>
#include <Arduino.h>


//Standard MaxsonarI2CXL address
#define I2C_SLAVE_ADDR  0x70  
byte received;


#define UPDATE_RATE_HZ 2

uint32_t last_update = 0;
uint32_t sample_count = 0;
uint32_t sum_dist = 0;
uint32_t status_count = 0;
uint32_t sum_k = 0;
uint32_t bad_frames = 0;


void readsonar(){     
    uint8_t buf[16];
    uint8_t count = 0;
    while (Serial.available()) {
        buf[count] = Serial.read();
        count++;
    }

    // check frame header
    if (count != 13) return;
    if (buf[0]!= 0x55) return;
    if (buf[1] != 0xA5) return;

    // compare checksum
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < count - 1; i++) {
        checksum += buf[i];
    }
    if (buf[count - 1] != checksum) {
        bad_frames++;  // for debugging
        return;
    }

    // process valid frame
    sum_dist += buf[4] << 8 | buf[5];
    status_count += buf[7];
    sum_k += buf[8] << 8 | buf[9];
    sample_count++;

    // print to serial0 at UPDATE_RATE_HZ
    uint32_t now = millis();
    if (now - last_update > 1000 / UPDATE_RATE_HZ) {
  
        last_update = now;
        sample_count = 0;
        sum_dist = 0;
        status_count = 0;
        sum_k = 0;
 }
}

//look for i2c read read request
   void receiveEvent(int howMany) {
      while (Wire.available()) { // loop through all but the last
        {         
      received = Wire.read(); 
      if (received == 0x51)
      {

    }
  }
}
}
//send results over i2c
void requestEvent() 
{  
Wire.write (highByte(sum_dist));
Wire.write (lowByte(sum_dist));
}

void setup() {
 Serial.begin(115200);
 Wire.begin(I2C_SLAVE_ADDR);
 Wire.onReceive(receiveEvent); // register event
 Wire.onRequest(requestEvent);
}
void loop() {
  readsonar();
}
