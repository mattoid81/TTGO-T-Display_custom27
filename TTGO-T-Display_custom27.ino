
//////////////////////////////////////
//                                  //
//    INCLUDE EXTERNAL LIBRARIES    //
//                                  //
//////////////////////////////////////

#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <Button2.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <tinyxml2.h>
using namespace tinyxml2;
#include "bmp.h"

//////////////////////////////////////
//                                  //
//          DEFINE TTGO PINS        //
//                                  //
//////////////////////////////////////

#define TFT_MOSI        19
#define TFT_SCLK        18
#define TFT_CS          5
#define TFT_DC          16
#define TFT_RST         23
#define TFT_BL          4  // Display backlight control pin
#define ADC_EN          14
#define ADC_PIN         34

#define BUTTON_A_PIN  0
#define BUTTON_B_PIN  35

unsigned long lastEvent_weather = 0;   
unsigned long lastEvent_bus = 0;
unsigned long lastEvent_bus17 = 0;
unsigned long lastEvent_detailedWeather = 0;
unsigned long lastEvent_jacket = 0;   

//////////////////////////////////////
//                                  //
//         ASSIGN VARIABLES         //
//                                  //
//////////////////////////////////////

Button2 buttonA = Button2(BUTTON_A_PIN);
Button2 buttonB = Button2(BUTTON_B_PIN);

TFT_eSPI tft = TFT_eSPI(240, 320); // Invoke custom library

//////////////////////////////////////
//                                  //
//              API                 //
//                                  //
//////////////////////////////////////

const String endpointA = "http://api.openweathermap.org/data/2.5/weather?q=christchurch,NZ&APPID=";
const String keyA = " <enter key here> ";

const String endpointB = "http://rtt.metroinfo.org.nz/rtt/public/utility/file.aspx?ContentType=";
const String keyB = "SQLXML&Name=JPRoutePositionET&PlatformNo=53116";

const String endpointC = "http://rtt.metroinfo.org.nz/rtt/public/utility/file.aspx?ContentType=";
const String keyC = "SQLXML&Name=JPRoutePositionET&PlatformNo=53090";

const String endpointD = "http://api.openweathermap.org/data/2.5/forecast?q=christchurch&appid=";
const String keyD = " <enter key here> ";

//////////////////////////////////////
//                                  //
//            XML PARSER            //
//                                  //
//////////////////////////////////////

class MyVisitor : public XMLVisitor {
  public:
  const char * tripETAValue;
  const char * trip2ETAValue;
  MyVisitor() {}
  ~MyVisitor() {} 

  bool VisitEnter (const XMLElement & element, const XMLAttribute * attribute) {
    // Serial.println("VisitEnter element attribute");
    if(String(element.Name()) == String("Route")) {
      if(String(attribute->Name()) == String("RouteNo") && String(attribute->Value()) == String("O")) {
        // Serial.println(String("Element: ") + element.Name());
        // Serial.println(String("Attribute: ") + attribute->Name() + String("=") + attribute->Value());
        const XMLElement * destination = element.FirstChildElement("Destination");
        if (destination == NULL) {
          Serial.println("Destination element missing.");
          return false;
        } else {
          const XMLElement * trip = destination->FirstChildElement("Trip");
          if (trip == NULL) {
            Serial.println("Trip element is missing.");
            return false;
          }
          tripETAValue = trip->Attribute("ETA");
          Serial.println(String("First trip ETA:") + tripETAValue);
          const XMLElement * trip2 = trip->NextSiblingElement("Trip");
          if (trip2 == NULL) {
            Serial.println("Trip2 element is missing.");
            return false;
          }
          trip2ETAValue=trip2->Attribute("ETA");
          Serial.println(String("Second trip ETA:") + String(trip2->Attribute("ETA")));
        }
      } else {
        return false;
      }
    }
    return true;
  }
};

//////////////////////////////////////
//                                  //
//          XML PARSER 17           //
//                                  //
//////////////////////////////////////

class MyVisitor17 : public XMLVisitor {
  public:
  const char * tripETAValue17;
  const char * trip2ETAValue17;
  MyVisitor17() {}
  ~MyVisitor17() {} 

  bool VisitEnter (const XMLElement & element, const XMLAttribute * attribute) {
    // Serial.println("VisitEnter element attribute");
    if(String(element.Name()) == String("Route")) {
      if(String(attribute->Name()) == String("RouteNo") && String(attribute->Value()) == String("17")) {
        // Serial.println(String("Element: ") + element.Name());
        // Serial.println(String("Attribute: ") + attribute->Name() + String("=") + attribute->Value());
        const XMLElement * destination = element.FirstChildElement("Destination");
        if (destination == NULL) {
          Serial.println("Destination element missing.");
          return false;
        } else {
          const XMLElement * trip = destination->FirstChildElement("Trip");
          if (trip == NULL) {
            Serial.println("Trip element is missing.");
            return false;
          }
          tripETAValue17 = trip->Attribute("ETA");
          Serial.println(String("First trip ETA:") + tripETAValue17);
          const XMLElement * trip2 = trip->NextSiblingElement("Trip");
          if (trip2 == NULL) {
            Serial.println("Trip2 element is missing.");
            return false;
          }
          trip2ETAValue17=trip2->Attribute("ETA");
          Serial.println(String("Second trip ETA:") + String(trip2->Attribute("ETA")));
        }
      } else {
        return false;
      }
    }
    return true;
  }
};



void setup()
{
  Serial.begin(115200);     // begin serial monitor for debugging
  delay(1000);

// Setup display

  tft.init();
  tft.setRotation(1);

  if (TFT_BL > 0) {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    }

  tft.invertDisplay(true);
  tft.setSwapBytes(true);

  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

//////////////////////////////////////
//                                  //
//              WIFI                //
//                                  //
//////////////////////////////////////

// Start WiFi

const char* ssid1 = " <WiFi Name> ";            // WiFi ID
const char* password1 = " <WiFi Password> ";          // WiFi password

const char* ssid2 = " <WiFi Name> ";                  // WiFi ID
const char* password2 = " <WiFi Password> ";        // WiFi password

const char* ssid3 = " <WiFi Name> ";                  // WiFi ID
const char* password3 = " <WiFi Password> ";          // WiFi password

while (WiFi.status() != WL_CONNECTED) {
  Serial.println("Connecting to WiFi 1...");
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(40,53);
  tft.println("Connecting to WiFi 1...");
  WiFi.begin(ssid1, password1);
  delay(1000);
  WiFi.begin(ssid1, password1);
  delay(1000);
  WiFi.begin(ssid1, password1);
  delay(1000);
  WiFi.begin(ssid1, password1);
  delay(1000);
  WiFi.begin(ssid1, password1);
  delay(5000);
  if (WiFi.status() == WL_CONNECTED) {
    break;}
  Serial.println("Connecting to WiFi 2...");
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(40,63);
  tft.println("Connecting to WiFi 2...");
  WiFi.begin(ssid2, password2);
  delay(1000);
  WiFi.begin(ssid2, password2);
  delay(1000);
  WiFi.begin(ssid2, password2);
  delay(1000);
  WiFi.begin(ssid2, password2);
  delay(1000);
  WiFi.begin(ssid2, password2);
  delay(5000);
  if (WiFi.status() == WL_CONNECTED) {
    break;}
  Serial.println("Connecting to WiFi 3...");
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(40,73);
  tft.println("Connecting to WiFi 3...");
  WiFi.begin(ssid3, password3);
  delay(1000);
  WiFi.begin(ssid3, password3);
  delay(1000);
  WiFi.begin(ssid3, password3);
  delay(1000);
  WiFi.begin(ssid3, password3);
  delay(1000);
  WiFi.begin(ssid3, password3);
  delay(5000);
  if (WiFi.status() == WL_CONNECTED) {
    break;}    
  }
 
Serial.println("Connected to the WiFi network");
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(40,53);
  tft.println("Connected to WiFi");
  delay(2000);

// Setup buttons

  buttonA.setClickHandler(click);
  buttonB.setClickHandler(click); 
}  

//////////////////////////////////////
//                                  //
//                LOOP              //
//                                  //
//////////////////////////////////////
  
void loop(){
  buttonA.loop();
  buttonB.loop();  
  queryWeather();
  queryBus();
  queryBus17();
  jacketNeeded();
}  

//////////////////////////////////////
//                                  //
//       QUERY WEATHER API          //
//                                  //
//////////////////////////////////////

void queryWeather(){
  unsigned long timeNow_weather = millis();
  unsigned long timeElapsed_weather = timeNow_weather - lastEvent_weather;
  if (lastEvent_weather == 0 || timeElapsed_weather > 60000) {
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
      HTTPClient http; 
      http.begin(endpointA + keyA); //Specify the URL
      int httpCodeA = http.GET();  //Make the request
      if (httpCodeA > 0) { //Check for the returning code
        String payloadA = http.getString();
        Serial.println(httpCodeA);
        Serial.println(payloadA);
                
//////////////////////////////////////
//                                  //
//           PARSE JSON             //
//                                  //
//////////////////////////////////////
    
        const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + 430;
        DynamicJsonBuffer jsonBuffer(capacity);
        JsonObject& root = jsonBuffer.parseObject(payloadA);
        JsonObject& weather_0 = root["weather"][0];
        int weather_0_id = weather_0["id"]; // 804
        const char* weather_0_main = weather_0["main"]; // "Clouds"
        const char* weather_0_icon = weather_0["icon"]; // "04d"
        JsonObject& main = root["main"];
        float main_temp = main["temp"]; // 288.15   
        Serial.println(String("Main Temp:") + (main_temp-273.15));
        int rounded_temp = main_temp-273.15;
        int main_pressure = main["pressure"]; // 1021
        int main_humidity = main["humidity"]; // 58
        int wind_deg = root["wind"]["deg"]; // 260
        int wind_speed = root["wind"]["speed"]; // 1

// Print weather to LCD
            
        tft.fillScreen(TFT_BLACK);
        if (weather_0_id == 800) {tft.pushImage(40,53,32,32,sunny);} else
        if ((weather_0_id >= 500) && (weather_0_id< 532)) {tft.pushImage(40,53,32,32,rain);} else
        if ((weather_0_id >= 200) && (weather_0_id< 233)) {tft.pushImage(40,53,32,32,tstorms);} else
        if ((weather_0_id >= 300) && (weather_0_id< 322)) {tft.pushImage(40,53,32,32,drizzle);} else
        if ((weather_0_id >= 600) && (weather_0_id< 623)) {tft.pushImage(40,53,32,32,snow);} else
        if ((weather_0_id >= 700) && (weather_0_id< 782)) {tft.pushImage(40,53,32,32,fog);} else
        if ((weather_0_id >= 801) && (weather_0_id< 805)) {tft.pushImage(40,53,32,32,cloudy);} else
        tft.pushImage(40,53,32,32,unknown);
        tft.setTextSize(2);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(80,63);
        tft.println(rounded_temp);
        tft.setCursor(105,63);
        tft.print((char)247);
        tft.setCursor(120,63);
        tft.println("C");  
        tft.setCursor(170,63);
        tft.println(wind_speed);
        tft.setCursor(200,63);
        tft.println("km/h");   
        if ((wind_deg >= 22.5) && (wind_deg< 67.5)) {tft.setCursor(140,63);tft.println("NE");} else
        if ((wind_deg >= 67.5) && (wind_deg< 112.5)) {tft.setCursor(140,63);tft.println("E");} else
        if ((wind_deg >= 112.5) && (wind_deg< 157.5)) {tft.setCursor(140,63);tft.println("SE");} else
        if ((wind_deg >= 157.5) && (wind_deg< 202.5)) {tft.setCursor(140,63);tft.println("S");} else
        if ((wind_deg >= 202.5) && (wind_deg< 247.5)) {tft.setCursor(140,63);tft.println("SW");} else
        if ((wind_deg >= 247.5) && (wind_deg< 302.5)) {tft.setCursor(140,63);tft.println("W");} else
        if ((wind_deg >= 302.5) && (wind_deg< 347.5)) {tft.setCursor(140,63);tft.println("NW");} else
        if ((wind_deg >= 347.5) && (wind_deg<= 360)) {tft.setCursor(140,63);tft.println("N");} else
        if ((wind_deg >= 0) && (wind_deg< 22.5)) {tft.setCursor(140,63);tft.println("N");} else
        tft.setCursor(140,63);tft.println("?");      

      }
    }
    lastEvent_weather = timeNow_weather;
  }
}

    //////////////////////////////////////
    //                                  //
    //       QUERY BUS API              //
    //                                  //
    //////////////////////////////////////

void queryBus() {
  unsigned long timeNow_bus = millis();
  unsigned long timeElapsed_bus = timeNow_bus - lastEvent_bus;
  if (lastEvent_bus == 0 || timeElapsed_bus > 60000) {
  HTTPClient http; 
  http.begin(endpointB + keyB);   //Specify the URL
  int httpCodeB = http.GET();  //Make the request
  if (httpCodeB > 0) { //Check for the returning code
    String payloadB = http.getString();
    Serial.println(httpCodeB);
    Serial.println(payloadB);

    //////////////////////////////////////
    //                                  //
    //           PARSE XML              //
    //                                  //
    //////////////////////////////////////
  
    const char * value = (const char *)payloadB.c_str(); // casts string payloadB to a const char for parsing
        
    XMLDocument doc; // creates an XML document called doc
    doc.Parse( value ); // remove string quotes, cast to const char *
  
    MyVisitor *visitor = new MyVisitor();
    doc.Accept(visitor); 

    //////////////////////////////////////
    //                                  //
    //     PRINT BUS TIMES TO LCD       //
    //                                  //
    //////////////////////////////////////

  
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(40,90);
    tft.println("Route:");
    tft.setCursor(40,130);
    tft.println("Next:");
    tft.setCursor(40,170);
    tft.println("Then:");

    tft.setTextColor(TFT_ORANGE);
    tft.setTextSize(2);
    tft.setCursor(120,90);
    tft.println("O Hals");
    tft.setTextSize(6);       
    tft.setCursor(124,110);
    tft.println(visitor->tripETAValue);
    tft.setTextSize(4);
    tft.setCursor(144,160);
    tft.println(visitor->trip2ETAValue);


  }
  else {
    Serial.println("Error on HTTP request");
  }
  http.end(); //Free the resources                              
  lastEvent_bus = timeNow_bus;
  }  
} 

    //////////////////////////////////////
    //                                  //
    //       QUERY BUS 17 API           //
    //                                  //
    //////////////////////////////////////


void queryBus17() {
  unsigned long timeNow_bus17 = millis();
  unsigned long timeElapsed_bus17 = timeNow_bus17 - lastEvent_bus17;
  if (lastEvent_bus17 == 0 || timeElapsed_bus17 > 60000) {
  HTTPClient http; 
  http.begin(endpointC + keyC);   //Specify the URL
  int httpCodeC = http.GET();  //Make the request
  if (httpCodeC > 0) { //Check for the returning code
    String payloadC = http.getString();
    Serial.println(httpCodeC);
    Serial.println(payloadC);

    //////////////////////////////////////
    //                                  //
    //           PARSE XML 17           //
    //                                  //
    //////////////////////////////////////
  
    const char * value = (const char *)payloadC.c_str(); // casts string payloadC to a const char for parsing
        
    XMLDocument doc; // creates an XML document called doc
    doc.Parse( value ); // remove string quotes, cast to const char *
  
    MyVisitor17 *visitor17 = new MyVisitor17();
    doc.Accept(visitor17); 

    //////////////////////////////////////
    //                                  //
    //    PRINT BUS 17 TIMES TO LCD     //
    //                                  //
    //////////////////////////////////////

  
    tft.setTextColor(TFT_PINK);
    tft.setTextSize(2);
    tft.setCursor(195,90);
    tft.println("17 Bryn");
    tft.setTextSize(6);       
    tft.setCursor(207,110);
    tft.println(visitor17->tripETAValue17);
    tft.setTextSize(4);
    tft.setCursor(227,160);
    tft.println(visitor17->trip2ETAValue17);

  }
  else {
    Serial.println("Error on HTTP request");
  }
  http.end(); //Free the resources                              
  lastEvent_bus17 = timeNow_bus17;
  }  
}


    //////////////////////////////////////
    //                                  //
    //          BUTTON CLICKS           //
    //                                  //
    //////////////////////////////////////


void click(Button2& btn) {
    if (btn == buttonA) {
      Serial.println("A clicked");
      //tft.fillScreen(TFT_RED);
      lastEvent_weather = 0;
      lastEvent_bus=0;
      lastEvent_bus17=0;
      queryWeather();
      queryBus();
      queryBus17();
    } 
    else if (btn == buttonB) {
      Serial.println("B clicked");
      lastEvent_detailedWeather = 0;
      detailedWeather();
    }
}

void detailedWeather(){
  unsigned long timeNow_detailedWeather = millis();
  unsigned long timeElapsed_detailedWeather = timeNow_detailedWeather - lastEvent_detailedWeather;
  if (lastEvent_detailedWeather == 0 || timeElapsed_detailedWeather > 60000) {
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
      HTTPClient http; 
      http.begin(endpointA + keyA); //Specify the URL
      int httpCodeA = http.GET();  //Make the request
      if (httpCodeA > 0) { //Check for the returning code
        String payloadA = http.getString();
        Serial.println(httpCodeA);
        Serial.println(payloadA);
                
//////////////////////////////////////
//                                  //
//     PARSE DETAILED JSON          //
//                                  //
//////////////////////////////////////
    
          const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + 430;
          DynamicJsonBuffer jsonBuffer(capacity);
          JsonObject& root = jsonBuffer.parseObject(payloadA);
          float coord_lon = root["coord"]["lon"]; // 172.64
          float coord_lat = root["coord"]["lat"]; // -43.53

          JsonObject& weather_0 = root["weather"][0];
          int weather_0_id = weather_0["id"]; // 804
          const char* weather_0_main = weather_0["main"]; // "Clouds"
          const char* weather_0_description = weather_0["description"]; // "overcast clouds"
          const char* weather_0_icon = weather_0["icon"]; // "04d"

          const char* base = root["base"]; // "stations"

          JsonObject& main = root["main"];
          float main_temp = main["temp"]; // 288.15
          int main_pressure = main["pressure"]; // 1021
          int main_humidity = main["humidity"]; // 58
          float main_temp_min = main["temp_min"]; // 287.59
          float main_temp_max = main["temp_max"]; // 288.71

          int visibility = root["visibility"]; // 10000

          int wind_speed = root["wind"]["speed"]; // 1
          int wind_deg = root["wind"]["deg"]; // 260

          int clouds_all = root["clouds"]["all"]; // 100

          long dt = root["dt"]; // 1564195793

          JsonObject& sys = root["sys"];
          int sys_type = sys["type"]; // 1
          int sys_id = sys["id"]; // 7348
          float sys_message = sys["message"]; // 0.0063
          const char* sys_country = sys["country"]; // "NZ"
          long sys_sunrise = sys["sunrise"]; // 1564170527
          long sys_sunset = sys["sunset"]; // 1564204971

          long timezone = root["timezone"]; // 43200
          long id = root["id"]; // 2192362
          const char* cityname = root["name"]; // "Christchurch"
          int cod = root["cod"]; // 200      

// Display detailed weather
      
          tft.fillScreen(TFT_BLACK);
          tft.setTextColor(TFT_WHITE);
          tft.setTextSize(1);
          tft.setCursor(40,53);
          tft.println(cityname);
          tft.setCursor(40,63);
          tft.println(weather_0_main);
          tft.setCursor(40,73);
          tft.println(weather_0_description);
          tft.setCursor(40,83);
          tft.println("current temp:");  
          tft.setCursor(160,83);
          tft.println(main_temp-273.15);
          tft.setCursor(40,93);
          tft.println("min temp:");  
          tft.setCursor(160,93);
          tft.println(main_temp_min-273.15);
          tft.setCursor(40,103);
          tft.println("max temp:");  
          tft.setCursor(160,103);
          tft.println(main_temp_max-273.15);
          tft.setCursor(40,113);
          tft.println("pressure:");  
          tft.setCursor(160,113);
          tft.println(main_pressure);  
          tft.setCursor(40,123);
          tft.println("humidity:");  
          tft.setCursor(160,123);
          tft.println(main_humidity); 
          tft.setCursor(40,133);
          tft.println("wind speed:");  
          tft.setCursor(160,133);
          tft.println(wind_speed);
          tft.setCursor(40,143);
          tft.println("wind deg:");  
          tft.setCursor(160,143);
          tft.println(wind_deg);
          tft.setCursor(40,153);
          tft.println("visibility:");  
          tft.setCursor(160,153);
          tft.println(visibility);
          tft.setCursor(40,163);
          tft.println("sunrise:");  
          tft.setCursor(160,163);
          tft.println(sys_sunrise);
          tft.setCursor(40,173);
          tft.println("sunset:");  
          tft.setCursor(160,173);
          tft.println(sys_sunset);
          tft.setCursor(40,183);
          tft.println();  
          tft.setCursor(160,183);
          tft.println();
      }
    }
  }
}

//////////////////////////////////////
//                                  //
//       JACKET ALGORITHM           //
//                                  //
//////////////////////////////////////


void jacketNeeded(){

   unsigned long timeNow_jacket= millis();
   unsigned long timeElapsed_jacket = timeNow_jacket - lastEvent_jacket;   
   
   if (lastEvent_jacket == 0 || timeElapsed_jacket > 60000) { // 1 minute
    
  Serial.println("Running void jacketNeeded()");
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
      HTTPClient http; 
      http.begin(endpointD + keyD); //Specify the URL
      int httpCodeD = http.GET();  //Make the request
      if (httpCodeD > 0) { //Check for the returning code
        String payloadD = http.getString();
        Serial.println(httpCodeD);
        Serial.println(payloadD);
                     
//////////////////////////////////////
//                                  //
//       PARSE JACKET JSON          //
//                                  //
//////////////////////////////////////

    
          const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + 430;
          DynamicJsonBuffer jsonBuffer(capacity);
          JsonObject& root = jsonBuffer.parseObject(payloadD);
          int popNow = root["list"][0]["pop"];
          const char* dtNow = root["list"][0]["dt_txt"];
          float tempNow = root["list"][0]["main"]["feels_like"];
          int pop3hrs = root["list"][1]["pop"];
          const char* dt3hrs = root["list"][1]["dt_txt"];
          float temp3hrs = root["list"][1]["main"]["feels_like"];
          int pop6hrs = root["list"][2]["pop"];
          const char* dt6hrs = root["list"][2]["dt_txt"];
          float temp6hrs = root["list"][2]["main"]["feels_like"];
          int pop9hrs = root["list"][3]["pop"];
          const char* dt9hrs = root["list"][3]["dt_txt"];
          float temp9hrs = root["list"][3]["main"]["feels_like"];
          int pop12hrs = root["list"][4]["pop"];
          const char* dt12hrs = root["list"][4]["dt_txt"];
          float temp12hrs = root["list"][4]["main"]["feels_like"];

// Calculate jacket algorithm

          int tempNowC = tempNow -273.15;
          int temp3hrsC = temp3hrs -273.15;
          int temp6hrsC = temp6hrs -273.15;
          int temp9hrsC = temp9hrs -273.15;
          int temp12hrsC = temp12hrs -273.15;


          bool chanceRain = false;

          if (popNow > 0 && pop3hrs > 0 && pop6hrs > 0 && pop9hrs > 0 && pop12hrs > 0){
              chanceRain = true;
          }
          else {
              chanceRain = false;
          }

          bool feelsLess12C = false;

          if (tempNowC < 12 && temp3hrsC < 12 && temp6hrsC < 12 && temp9hrsC < 12 && temp12hrsC < 12){
              feelsLess12C = true;
          }
          else {
              feelsLess12C = false;
          }

          bool jacket = false;

          if (feelsLess12C == true || chanceRain == true)  {
              jacket = true;
          }
          else {
              jacket = false;
          }

            Serial.println("DO I NEED TO TAKE A JACKET?");
            Serial.println(jacket);

        if (jacket == true ) {tft.pushImage(250,53,32,30,jacket_black);} else
        tft.pushImage(250,53,32,30,no_jacket_black);        
         
        }
            lastEvent_jacket = timeNow_jacket;         
    }
}    
}    
