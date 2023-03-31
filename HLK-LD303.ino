#include <Wire.h>

//Standard MaxsonarI2CXL address
#define I2C_SLAVE_ADDR  0x70  



#include <Arduino.h>

//#define DEBUG // comment to output only distance on Serial0
#define TERMINATOR '\n' // non-debug serial output terminator character/string
#define SAMPLES_TO_AVERAGE 1
#define BUFFER_SIZE 16

uint8_t buffer[BUFFER_SIZE];
uint8_t byte_count = 0;
uint32_t sample_count = 0;
uint32_t sum_distance = 0;
uint32_t status_count = 0;
uint32_t sum_k = 0;
uint32_t bad_frames = 0;

uint32_t i = 0;


//number of readings to average
const int numReadings = 10;


unsigned char data_buffer[4] = {0};
int distances = 0;
int distance = 0;
unsigned char CS;
uint8_t Index;
byte received;
int readings[numReadings]; 
int readIndex = 0; 
int total = 0;  
int average = 0; 


void readradar(){     
    buffer[byte_count] = Serial1.read();
        byte_count = (byte_count + 1) % BUFFER_SIZE;

    // check/sync frame header
    if (byte_count > 0  && buffer[0]!= 0x55) {
        bad_frames++;
        byte_count = 0;
        return;
    }

    if (byte_count > 1 && buffer[1] != 0xA5) {
        bad_frames++;
        byte_count = 0;
        return;
    }

    if (byte_count > 13) {
        bad_frames++;
        byte_count = 0;
        return;
    }

    // wait for full frame buffer
    if (byte_count != 13) return;

    // compare checksum
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < byte_count - 1; i++) {
        checksum += buffer[i];
    }
    if (buffer[byte_count - 1] != checksum) {
        bad_frames++;
        return;
    }

    // process valid frame
    sum_distance += buffer[4] << 8 | buffer[5];
    status_count += buffer[7];
    sum_k += buffer[8] << 8 | buffer[9];
    sample_count++;
    byte_count = 0;
distance = sum_distance;
    if (sample_count == SAMPLES_TO_AVERAGE) {
#ifdef DEBUG
        Serial.print("Samples: ");
        Serial.print(sample_count);
        Serial.print("\t  Status_Count: ");
        Serial.print(status_count);
        Serial.print("\t  Dist: ");
        Serial.print(sum_distance / sample_count);
        Serial.print("\t  k: ");
        Serial.println(sum_k / sample_count);

        if (bad_frames) {
            Serial.print("Bad frames: ");
            Serial.println(bad_frames);
        }
#else
        Serial.print(sum_distance / sample_count);
#ifdef TERMINATOR
        Serial.print(TERMINATOR);
#endif
#endif
        sample_count = 0;
        sum_distance = 0;
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
Wire.write (highByte(distance));
Wire.write (lowByte(distance));
}

void setup() {
 Serial.begin(115200);
 Serial1.begin(115200, SERIAL_8N1, 16, 17);
 Wire.begin(I2C_SLAVE_ADDR);
 Wire.onReceive(receiveEvent); // register event
 Wire.onRequest(requestEvent);

    const uint8_t fixed_query[] = {0x55, 0x5A, 0x02, 0xD3, 0x84};
    const uint8_t continuous[] = {0xBA, 0xAB, 0x00, 0xF6, 0x00, 0x07, 0x00, 0x55, 0xBB};
    Serial1.write(fixed_query, sizeof(fixed_query));
    Serial1.write(continuous, sizeof(continuous));

 #if CONFIG_IDF_TARGET_ESP32
  char message[64];
  snprintf(message, 64, "%u Packets.", i++);
  Wire.slaveWrite((uint8_t *)message, strlen(message));
#endif


}
void loop() {
  readradar();
}
