/*
  CAT PILLOW
 
 This sketch is to be used for Maxus' Cat Pillow prototype. 
 
 The cat pillow is designed to be controlled via a 
 web app (using Arduino Yun).
 
 It features the following:
 
 4 vibration motors
 1 servo that is attached to a heater's potentiometer
 1 microphone for future use
 1 button to simulate microphone input
 
 created  23 September 2014
 By Mithru Vigneshwara for Metalworks by Maxus
 
 */

// Yun Stuff

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;


// SETTING UP PINS

int vibrator_pins[] = {
  13, 12, 11, 10
};

int heater_motor_pin = 9;

int button_pin = 2;

// Servo stuff

#include <Servo.h> 

Servo servo_motor;


void setup() {
  for(int i = 0; i < sizeof(vibrator_pins); i++) {
    pinMode(vibrator_pins[i], OUTPUT);
  }
  
  pinMode(button_pin, INPUT);

  servo_motor.attach(heater_motor_pin);

  Bridge.begin();
  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();

}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    client.setTimeout(5);
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  // is "vibe" command?
  if (command == "vibe") {
    vibeCommand(client);
  }

  // is "heater" command?
  if (command == "heater") {
    heaterCommand(client);
  }

  // is "test" command?
  if (command == "test") {
    testAllMotors();
  }
  
  if (command == "purr") {
    purrCommand(client);
  }
  
}

String callback;

void purrCommand(YunClient client){
  callback = client.readStringUntil('/');
  client.println(F("Status: 200"));
  client.println(F("Content-type: application/javascript; charset=utf-8"));
  client.println(); //mandatory blank line
  client.print(callback + "({\"ret\" : " + String(digitalRead(button_pin)) + "});");

}

/* trigger vibrator by ID
 /arduino/vibe/0
 /arduino/vibe/1
 /arduino/vibe/2
 /arduino/vibe/3
 */
 
void vibeCommand(YunClient client) {
  int pin;

  // Read pin number
  pin = client.parseInt();

  vibrateMotorID(vibrator_pins[pin]);

}


/* control heater value: 0 - 180
  /arduino/heater/0
  /arduino/heater/93
  /arduino/heater/142
  /arduino/heater/180
  */
  
void heaterCommand(YunClient client) {
  int value;

  // Read value 
  value = client.parseInt();

  servo_motor.write(value);

}

// test that all motors function alright
void testAllMotors() {
  for(int i = 0; i < sizeof(vibrator_pins); i++) {
    vibrateMotorID(vibrator_pins[i]);
    servo_motor.write(i*45);
    delay(1000);
  }
}

int vibration_duration = 100;

void vibrateMotorID(int pin) {
  digitalWrite(pin, HIGH);
  delay(vibration_duration);
  digitalWrite(pin, LOW);
}



