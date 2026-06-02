const int flexPin = A0;			// Pin connected to voltage divider output

// Change these constants according to your project's design
const float VCC = 5;			// voltage at Ardunio 5V line
const float R_DIV = 10000.0;	// resistor used to create a voltage divider
const float flatResistance = 25000.0;	// resistance when flat
const float bendResistance = 100000.0;	// resistance at 90 deg

float voltage=0;

void setup() {
	Serial.begin(9600);
	pinMode(flexPin, INPUT);
}

void loop() {
	// Read the ADC, and calculate voltage and resistance from it
	int ADCflex = analogRead(flexPin);
	float Vflex = ADCflex * VCC / 1023.0;
	float Rflex = R_DIV * (VCC / Vflex - 1.0);
  voltage=VCC*(R_DIV/(R_DIV+Rflex));
	Serial.println("Resistance: " + String(Rflex) + " ohms");
	Serial.println("Voltage: " + String(voltage) + " volts");
	Serial.println();

	delay(500);
}