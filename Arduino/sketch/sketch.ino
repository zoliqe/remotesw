#include <WebUSB.h>

const WebUSBURL URLS[] = {
  { 1, "zoliqe.github.io/remotesw/" },
  { 0, "127.0.0.1:8081" },
};

const uint8_t ALLOWED_ORIGINS[] = { 1, 2 };

WebUSB WebUSBSerial(URLS, 2, 1, ALLOWED_ORIGINS, 2);

#define Serial WebUSBSerial

const int ledPin = 13;

bool state = false;
unsigned long timeOut;

void setup() {
  while (!Serial) {
    ;
  }
  Serial.begin(9600);
  Serial.println("READY");
  Serial.flush();
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, state);

  Serial1.begin(9600);
}

void loop() {
  if (Serial && Serial.available()) {
    int val = Serial.read();
    // Serial.write(val);
    if (val == 'H') {
      on();
    } else if (val == 'L') {
      off();
    } else if (val == 'N') {
      swstate();
    }
//    Serial.write("\r\n> ");
  }
  if (state) {
    if (millis() > timeOut) {
      off();
    } else {
      info();
    }
  }
  delay(1000);
}

void on() {
  state = HIGH;
  timeOut = millis() + (10 * 60000); // 10 minutes
  swstate();
}

void off() {
  if (!state) {
    return;
  }
  state = LOW;
  swstate();
}

void swstate() {
  Serial.println(state ? "HIGH" : "LOW");
  digitalWrite(ledPin, state);
  Serial.flush();
}

void info() {
  unsigned long remains = timeOut - millis();
  unsigned int mins = remains / 60000;
  unsigned int secs = (remains - mins * 60000) / 1000;
  Serial.print(mins < 10 ? "T0" : "T");
  Serial.print(mins);
  Serial.print(secs < 10 ? ":0" : ":");
  Serial.println(secs);
  Serial.flush();

  if (Serial1 && Serial1.available()) {
    iqrg();
  }
}

void iqrg() {
    uint8_t buff[11];
    uint8_t len = 0;
    Serial.print("CIV=");
    while (Serial1.available() && len < 11) {
      buff[len] = Serial1.read();
      Serial.print(buff[len]);
      Serial.print(',');
      len++;
    }
    Serial.println("");
    Serial.flush();
    if (buff[4] == 0 || buff[4] == 3) {
      uint8_t MHZ_100 = (buff[8]);
      MHZ_100 = MHZ_100 - (((MHZ_100 / 16) * 6));
      uint8_t MHZ = (buff[7]);
      MHZ = MHZ - (((MHZ / 16) * 6));
      uint8_t KHZ = buff[6];
      KHZ = KHZ - (((KHZ / 16) * 6));
      uint8_t HZ = buff[5];
      HZ = HZ - (((HZ / 16) * 6));
      unsigned long QRG = ((MHZ * 10000) + (KHZ * 100) + (HZ * 1));
      Serial.print("QRG=");
      Serial.println(QRG);
    }
}

