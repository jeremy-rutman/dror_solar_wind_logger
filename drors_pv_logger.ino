/*
dror alon jwewmy  PV and wind logger and pump/light controller
 http://www.arduino.cc/en/Tutorial/SerialEvent
 
 */

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

//PINS FOR DIGITAL CONTROLS - 2-7 are PWM capable
int pump1_lowside_pin=2;
int pump2_lowside_pin=3; 
int pv_lowside_pin=4; 
int high_side_pin=5;
int lights_1_24_pin=6;
int lights_2_12_pin=7;
int nmos1_pin=8;
int nmos2_pin=9;
int volume_sensor_1_pin=10;

//PINS FOR ANALOG INPUT
int battery_read_voltage_pin=0;
int wind_current_read_pin=1;
int pv_current_read_pin=2;
int wind_voltage_read_pin=3;
int pump1_on_time=0;
int pump2_on_time=0;
int max_pump_on_time=1000*60*10; //10 minutes in mS
float  battery_voltage=0.0;
float wind_shunt_voltage=0.0;
float wind_shunt_current=0.0;
float wind_voltage=0.0;
float R_wind_shunt=0.0;
float pv_shunt_voltage=0.0;
float pv_shunt_current=0.0;
float R_pv_shunt=0.0;
float volume_sensor_1_level=0.0;

enum Commands {
  CMD_READ_BATTERY_VOLTAGE=1,
  CMD_READ_WIND_CURRENT,
  CMD_READ_PV_CURRENT,
  CMD_READ_WIND_VOLTAGE,
  CMD_SET_PUMP_1_12_on,
  CMD_SET_PUMP_2_12_on,
  CMD_SET_PUMP_1_PV_on,
  CMD_SET_PUMP_2_PV_on,
  CMD_SET_PUMP_1_24_on,
  CMD_SET_PUMP_2_24_on,
  CMD_SET_PUMP_1_off,
  CMD_SET_PUMP_2_off,
  CMD_SET_VOLUME_SENSOR_1_ACTIVE,
  CMD_SET_VOLUME_SENSOR_1_INACTIVE,
  CMD_SET_VOLUME_SENSOR_2_ACTIVE,
  CMD_SET_VOLUME_SENSOR_2_INACTIVE,
  CMD_READ_VOLUME_SENSOR1,
  CMD_READ_VOLUME_SENSOR2,
  CMD_READ_LIGHT_1,
  CMD_READ_LIGHT_2,
  CMD_LIGHT_1_on,
  CMD_LIGHT_1_off,
  CMD_LIGHT_2_on,
  CMD_LIGHT_2_off,
};

String abbrevs[]= {
  "RBV  ",//CMD_READ_BATTERY_VOLTAGE
  "RWC  ",//CMD_READ_WIND_CURRENT
  "RPC  ", //CMD_READ_PV_CURRENT,
  "RWV  ", //CMD_READ_WIND_VOLTAGE,
  "P1_12", //CMD_SET_PUMP_1_TO_12,
  "P2_12", //CMD_SET_PUMP_2_TO_12,
  "P1_PV", //CMD_SET_PUMP_1_TO_PV,
  "P2_PV", //CMD_SET_PUMP_2_TO_PV,
  "P1_24", //CMD_SET_PUMP_1_TO_24,
  "P2_24", //CMD_SET_PUMP_2_TO_24,
  "P1_OF", // CMD_SET_PUMP_1_off,
  "P2_OF", // CMD_SET_PUMP_2_off,
  // use something like "P1P36" to set P1 to pwm value of 36
  "VS1A ", //CMD_SET_VOLUME_SENSOR_1_ACTIVE,
  "VS1I ", //CMD_SET_VOLUME_SENSOR_1_INACTIVE,
  "VS2A ", //CMD_SET_VOLUME_SENSOR_2_ACTIVE,
  "VS2I ", //CMD_SET_VOLUME_SENSOR_2_INACTIVE,
  "RVS1 ", //CMD_READ_VOLUME_SENSOR,
 "RVS2 ", //CMD_READ_VOLUME_SENSOR,
  "RL1  ", //CMD_READ_LIGHT_1,
  "RL2  ", //CMD_READ_LIGHT_2,
  "L1on ", //CMD_LIGHT_1_on,
  "L1off", //CMD_LIGHT_1_off,
  "L2on "  //CMD_LIGHT_2_on,
  "L2off"  //CMD_LIGHT_2_off,
  // use something like "D1L36" to set L1 to pwm value of 36
};/*


void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
    pinMode(nmos1_pin, OUTPUT);      
    pinMode(nmos2_pin, OUTPUT);      
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString); 
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

//make sure pumps do not stay on more than 10 minutes
  if(millis()-pump1_on_time>max_pump_on_time)
     digitalWrite(pump1_lowside_pin, LOW); 

  if(millis()-pump2_on_time>max_pump_on_time)
     digitalWrite(pump2_lowside_pin, LOW); 
  
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  int cmd;
  String incomingStr=String("    ");
  while (Serial.available()) {
    cmd = 0;
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      if (inputString.length() != 4) {
        // illegal string, ignore
        inputString == "";
        continue;
      }
      incomingStr=inputString;
      for (int i = 0 ; i < sizeof(abbrevs) / sizeof(abbrev[0]) ; ++i) {
        if (abbrevs[i] == incomingStr) {
          cmd = i;
        }
      }
      switch (cmd) {
        case CMD_READ_BATTERY_VOLTAGE:
        battery_voltage=analogRead(battery_read_voltage_pin);
         Serial.write((int)(100*battery_voltage));
         break;
  case CMD_READ_WIND_CURRENT:
          wind_shunt_voltage=analogRead(wind_current_read_pin);
          wind_shunt_current=wind_shunt_voltage/R_wind_shunt;
         Serial.write((int)(100*wind_shunt_current));
         break;
  case CMD_READ_PV_CURRENT:
           pv_shunt_voltage=analogRead(pv_current_read_pin);
          pv_shunt_current=pv_shunt_voltage/R_pv_shunt;
         Serial.write((int)(100*pv_shunt_current));
         break;

  case CMD_READ_WIND_VOLTAGE:
         wind_voltage=analogRead(wind_voltage_read_pin)/2;
         Serial.write((int)(100*wind_voltage));
         break;
  case CMD_SET_PUMP_1_12_on:
   digitalWrite(pump1_lowside_pin, HIGH); 
   pump1_on_time=millis();
            break;
   case CMD_SET_PUMP_2_12_on:
   digitalWrite(pump2_lowside_pin, HIGH);  
      pump2_on_time=millis();
            break;

  case CMD_SET_PUMP_1_PV_on:
    digitalWrite(high_side_pin, LOW);  
   digitalWrite(pv_lowside_pin, HIGH);  
   pump1_on_time=millis();
break;

  case CMD_SET_PUMP_2_PV_on:
     digitalWrite(high_side_pin, LOW);  
   digitalWrite(nmos2_pin, HIGH);  
   pump2_on_time=millis();
break;

 case CMD_SET_PUMP_1_24_on:
     digitalWrite(high_side_pin,HIGH);  
   digitalWrite(nmos1_pin, HIGH);  
      pump1_on_time=millis();
 break;
 
 
case  CMD_SET_PUMP_2_24_on:
      digitalWrite(high_side_pin,HIGH);  
   digitalWrite(nmos2_pin, HIGH);  
    pump2_on_time=millis();
break;

 case  CMD_SET_PUMP_1_off:
   digitalWrite(nmos1_pin, LOW);  
break;

case   CMD_SET_PUMP_2_off:
     digitalWrite(nmos2_pin, LOW);  
break;

case   CMD_SET_VOLUME_SENSOR_1_ACTIVE:
     digitalWrite(nmos1_pin, HIGH);  
     digitalWrite(high_side_pin,HIGH);  
break;

case  CMD_SET_VOLUME_SENSOR_1_INACTIVE:
//do nothing
break;

case  CMD_SET_VOLUME_SENSOR_2_ACTIVE:
     digitalWrite(nmos2_pin, HIGH);  
     digitalWrite(high_side_pin,HIGH);  
break;

case  CMD_SET_VOLUME_SENSOR_2_INACTIVE:
break;

  CMD_READ_VOLUME_SENSOR:
  volume_sensor_1_level=analogRead(volume_sensor_1_pin);
  break;
  
  CMD_LIGHT_1_on:
       digitalWrite(lights_1_24_pin, HIGH);  
break;

  CMD_LIGHT_2_on,
     digitalWrite(lights_2_12_pin, HIGH);  
break;

      }

      inputString = "";
    }
    // add it to the inputString:
    inputString += inChar;
  }
}

/*
loop start


if (pumpon)
pump on
a=timer
b=timer+10

if(pumpoff)

if(pv)
sdfsdfsf

if()
dfgdfg

a=timer
if a>b pump off


end loop
*/
