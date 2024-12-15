#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Pin definitions
const int trigPin = 9;
const int echoPin = 10;
const int buzzerPin = 11;  // Buzzer pin

// RGB LED pins
const int redPin = 6;
const int greenPin = 5;
const int bluePin = 4;

// DHT Sensor pin
const int dhtPin = 2;
DHT dht(dhtPin, DHT22);  // Using DHT22 sensor

// Thresholds for water levels (in cm)
int lowLevelThreshold = 30;    // Low water level
int mediumLevelThreshold = 60; // Medium water level
int highLevelThreshold = 100;  // High water level

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables for ultrasonic sensor
long duration;
int distance;

// Variables for temperature
float temperature;

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Initialize RGB LED pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Initialize DHT sensor
  dht.begin();

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Water Detection");
  delay(2000);
}

void loop() {
  // Measure distance from the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Get temperature from DHT sensor
  temperature = dht.readTemperature();  // Read temperature in Celsius

  // Check if the reading is valid
  if (isnan(temperature)) {
    Serial.println("Failed to read temperature!");
    lcd.clear();
    lcd.print("Temp Error");
    delay(1000);
    return;
  } else {
    // Print the temperature to the Serial Monitor
    Serial.print("Temperature: ");
    Serial.println(temperature);
  }

  // Display the distance and temperature on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");

  // Check the water level and temperature to infer human presence
  if (distance < lowLevelThreshold) {
    // Low water level (Red color blinking)
    blinkRed();  // Call function to blink the red LED
    tone(buzzerPin, 500);  // Low tone for low water level
    delay(500);
    noTone(buzzerPin);
    delay(500);
  } 
  else if (distance >= lowLevelThreshold && distance < mediumLevelThreshold) {
    // Medium water level (Green color blinking)
    blinkGreen();  // Call function to blink the green LED
    tone(buzzerPin, 1000);   // Medium tone for medium water level
    delay(300);
    noTone(buzzerPin);
    delay(300);
  } 
  else if (distance >= mediumLevelThreshold && distance < highLevelThreshold) {
    // High water level (Blue color blinking)
    blinkBlue();  // Call function to blink the blue LED
    tone(buzzerPin, 1500);    // High tone for high water level
    delay(200);
    noTone(buzzerPin);
    delay(200);
  } 
  else {
    // No water detected (turn off LED and buzzer)
    setColor(0, 0, 0);  // Turn off RGB LED
    noTone(buzzerPin);
  }

  // Detect if a human body is present in the water based on temperature
  if (distance < highLevelThreshold && temperature >= 37.0) {  // Detect temp >= 37°C
    Serial.println("Human detected in water!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Human Detected!");
    playFastTickSoundWithAllLEDs(); // Blink all LEDs and play fast tick-tick sound
    delay(1000);  // Pause for 1 second before repeating
  }

  delay(500);  // Delay for a short period before repeating
}

// Function to set the color of the RGB LED
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

// Function to blink the red LED
void blinkRed() {
  setColor(255, 0, 0);  // Red color
  delay(500);            // LED on for 500ms
  setColor(0, 0, 0);     // LED off
  delay(500);            // LED off for 500ms
}

// Function to blink the green LED
void blinkGreen() {
  setColor(0, 255, 0);  // Green color
  delay(500);            // LED on for 500ms
  setColor(0, 0, 0);     // LED off
  delay(500);            // LED off for 500ms
}

// Function to blink the blue LED
void blinkBlue() {
  setColor(0, 0, 255);  // Blue color
  delay(500);            // LED on for 500ms
  setColor(0, 0, 0);     // LED off
  delay(500);            // LED off for 500ms
}

// Function to generate a fast tick-tick sound and blink all LEDs
void playFastTickSoundWithAllLEDs() {
  for (int i = 0; i < 10; i++) {
    // Turn on all LEDs
    setColor(255, 255, 255); // White light by combining all colors
    tone(buzzerPin, 1000);   // Frequency for the tick sound
    delay(100);              // 100ms on

    // Turn off all LEDs
    setColor(0, 0, 0);       // Turn off RGB LED
    noTone(buzzerPin);       // Turn off sound
    delay(100);              // 100ms off (between ticks)
  }
}
