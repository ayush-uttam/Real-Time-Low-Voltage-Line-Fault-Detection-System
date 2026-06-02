#define joyX D2
#define joyY D3

void setup() {
  // put your setup code here, to run once:
  pinMode(joyX,INPUT);
  pinMode(joyY,INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int valX=analogRead(joyX);
  int valY=analogRead(joyY);
  Serial.println(valX);
  delay(500);
}
