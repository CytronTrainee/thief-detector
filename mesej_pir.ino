#include <ESPert.h>

ESPert espert;

const char *host = "www.espert.io";
String smartphone_key = "4885616584032256"; //replace with your channel's espert smartphone key

//initialize pin input and output
const int buzzer1 = 2;
const int pir = 14;
const int switch1 = 12;

//initialize value
int x=0;                //calibration just for once
int clearLCD = 0;       //no blinking status on oled
int prev_state = 0;           
int calibrationTime = 10;

bool currentSwitch = true;

void setup()
{
  //pin set-up
  pinMode(pir, INPUT_PULLUP);
  pinMode(switch1, INPUT_PULLUP);
  pinMode(buzzer1, OUTPUT);

  espert.init();
  espert.oled.init();
  delay(2000);

  espert.oled.clear();
  espert.oled.println(espert.info.getId());
  espert.oled.println();

  int mode = espert.wifi.init();

  if (mode == ESPERT_WIFI_MODE_CONNECT)
  {
    espert.println(">>> WiFi mode: connected.");
    espert.oled.println("WiFi: connected.");
    espert.oled.print("IP..: ");
    espert.oled.println(espert.wifi.getLocalIP());
  }
  else if (mode == ESPERT_WIFI_MODE_DISCONNECT)
  {
    espert.println(">>> WiFi mode: disconnected.");
    espert.oled.println("WiFi: not connected.");
  }
  else if (mode == ESPERT_WIFI_MODE_SMARTCONFIG)
  {
    espert.println(">>> WiFi mode: smart config.");
  }
  else if (mode == ESPERT_WIFI_MODE_SETTINGAP)
  {
    espert.println(">>> WiFi mode: access point.");
  }
}

void loop()
{
  espert.loop();
  int state = digitalRead(switch1);
  
  if(prev_state != state){clearLCD = 0;}
  
  if(state == HIGH)   //if the main switch is ON
  {
    if(x == 0)   //for calibration of the sensor
    {
      x=1;
      clearLCD = 0;
      espert.oled.clear();
      delay(2000);
    
      //give the sensor some time to calibrate
      espert.oled.println("calibrating sensor... ");
      delay(10000);
      espert.oled.println(" done");
      espert.oled.println("SENSOR ACTIVE");
      delay(2000);
    }
    if(digitalRead(pir) == LOW)   //if the sensor detecting someone (thief)
    {
      clearLCD = 0;
      String message = "Thief" ;
      message.replace(String(" "), String("%20"));
      String path = "/MySmartphone/send?key=" + smartphone_key + "&message=" + message;
      espert.println(">>" + espert.wifi.getHTTP(host, path.c_str()) + "<<");
      espert.oled.println("Thief");

      digitalWrite(buzzer1, HIGH);
      delay (2000);
    }
    else
    {
      if(clearLCD == 0)     //if there is nobody
      {
        digitalWrite(buzzer1, LOW);
        espert.oled.clear();
        espert.oled.println("Nobody");
        clearLCD = 1;
      }
    }
  }
  else      //if the main switch is OFF
  {
    if(digitalRead(pir) == HIGH)
    {
      if(clearLCD == 0)
      {
        digitalWrite(buzzer1, LOW);
        espert.oled.clear();
        espert.oled.println("OFF");
        clearLCD = 1;
      }
    }
    else
    {
      clearLCD = 0;
    }
  }
  prev_state = state;
  delay(10);
}
