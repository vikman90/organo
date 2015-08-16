const char STRINGS[][9] = {
  "0000000A",
  "0000000a",
  "0000000B",
  "0000000b",
  "0000000D",
  "0000000d",
  "0000000H",
  "0000000h"
};

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  Serial.println(STRINGS[random(8)]);
}

void loop() {
}
