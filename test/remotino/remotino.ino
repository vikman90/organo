const char STRINGS[][9] = {
  "0016D34A",
  "0016D34a",
  "0016D34B",
  "0016D34b",
  "0016D34C",
  "0016D34c",
};

void setup() {
  pinMode(13, OUTPUT);
  randomSeed(analogRead(0));
  Serial.begin(9600);

  if (random(5) < 1) {
    Serial.println("001");
    digitalWrite(13, 0);
  } else {
    Serial.println(STRINGS[random(6)]);
    digitalWrite(13, 1);
  }
}

void loop() {
}
