/*
 * 2114 Static RAM tester
 * 
 * It has 9 address lines and 4 data lines
 * 
 * Pin assignment as follows:
 * Digital pins 0-9 are connected to the 2114 Address pins (blue wires)
 * Digital pins 10-13 are connected to the 2114 Data pins (yellow wires)
 * Analog pin A0 is used for RW (green wire) (LOW = WRITE)
 * 
 * +5V (red wire)
 * GND (black wire)
 * 2114 pin 9 (CS) is wired directly to GND (white wire)
 * 
 * 
 * Carsten Skjerk June 2021
 */

// Address Pins
const int addressPins[10] = {
  0,1,2,3,4,5,6,7,8,9
};

// Data Pins
const int dataPins[4] = {
  10,11,12,13
};

// Analog pin 0 is used as digital output for the RW signal
const int RW = 14; // Analog Pin 0

// Set Address pins to output
void setupAddressPins() {
  for (int i=0; i<10; i++) {
    pinMode(addressPins[i], OUTPUT);
  }
  delay(1); // Wait a bit for them to settle
}

// Set Data pins to output
void setDataPinsOutput() {
  for (int i=0; i<4; i++) {
    pinMode(dataPins[i], OUTPUT);
  }
  delay(1); // Wait a bit for them to settle
}

// Set Data pins to input
void setDataPinsInput() {
  for (int i=0; i<4; i++) {
    pinMode(dataPins[i], INPUT);
  }
  delay(1); // Wait a bit for them to settle
}

// Initial setup of pins and serial monitor
void setup() {
  // Initialize all pins
  setupAddressPins();
  setDataPinsOutput();
  pinMode(RW, OUTPUT);
  digitalWrite(RW, LOW); // Set READ mode
  
  // Initialize Serial Port
  Serial.begin(115200);
  Serial.println("2114 Static RAM tester by Carsten Skjerk");
}

// Set the address pins to match the specified address
void setAddressBits(int address) {
  for (int i=0; i<14; i++) {
    if (bitRead(address, i)==1) {
      digitalWrite(addressPins[i], HIGH);
    } else {
      digitalWrite(addressPins[i], LOW);
    }
  }
}

// Set the data pins to match the specified value
void setDataBits(byte value) {
  for (int i=0; i<4; i++) {
    if (bitRead(value, i) == 1) {
      digitalWrite(dataPins[i], HIGH);
    } else {
      digitalWrite(dataPins[i], LOW);
    }
  }
}

// Write data to the specified memory address
void writeData(int address, byte data) {
  // Set address bits
  setAddressBits(address);

  // Enable Write pin
  digitalWrite(RW, LOW);

  // Set data bits
  setDataBits(data);

  // Wait a bit....
  delay(1);
  
  // Disable Write pin
  digitalWrite(RW, HIGH);

}

// Read data from the specified memory address
// Note the the RW pin must already be in READ mode
byte readData(int address) {
  // Set address bits
  setAddressBits(address);

  // Read each data bit one by one
  byte result = 0;
  for (int i=0; i<4; i++) {
    bitWrite(result,i,digitalRead(dataPins[i]));
  }
  return result;
}

// Output binary value from the 4 bit data
void printBinary(byte data)
{
  for (int b = 3; b >= 0; b--) {
    Serial.print(bitRead(data, b));
  }
}

void loop() {
  int errorCount = 0;
  // Use all possible values for data patterns (4 bits => 16 patterns)
  for (byte pattern=0; pattern<16; pattern++) {
    Serial.print("Running Testpattern "); printBinary(pattern);
    Serial.println();
   
    // Loop through all addresses in the 2114
    for (int addr = 0; addr<1024; addr++) {
      // Write testpattern to the 2114
      setDataPinsOutput();
      writeData(addr,pattern);
      
      // Read data from the 2114
      setDataPinsInput();
      byte data = readData(addr);

      // Verify
      if (data != pattern) {
        errorCount++;
        Serial.print("Error at address ");
        Serial.print(addr, HEX);
        Serial.print(" - Got: "); printBinary(data);
        Serial.print(" Expected: "); printBinary(pattern);
        Serial.println();
      }
    }

  }
  Serial.println("Test complete");
  Serial.print(errorCount);
  Serial.println(" errors found.");

  // Nothing more to do, so loop indefinitely - or until Reset is pressed
  while(1) {};
}
