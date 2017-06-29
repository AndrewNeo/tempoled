#include <SH1106.h>

#define UPDATE_PERIOD 30*1000
#define HALF 128/2


// Initialize the OLED display
SH1106 display(0x3c);

long lastUpdate, lastDraw;
String indoor, outdoor;
static const String f_degF = "%s°F";
static const String f_lastUpdate = "Updated %d min ago";


// 123,-234
int tempUpdate(String data) {
  lastUpdate = millis();

  int commapos = data.indexOf(",");
  if (commapos > -1) {
    indoor = data.substring(0, commapos).substring(0, 3);
    outdoor = data.substring(commapos+1).substring(0, 3);
  } else {
    indoor = data.substring(0, 3);
    outdoor = "XX";
  }

  drawTempDisplay();

  return 0;
}


void drawWaitingDisplay() {
  display.clear();

  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Waiting for update.");

  display.display();

  delay(10);
}

void drawTempDisplay() {
  display.clear();

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Indoor");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 12, String::format(f_degF, indoor.c_str()));

  display.setFont(ArialMT_Plain_10);
  display.drawString(HALF-2, 0, "Outdoor");
  display.setFont(ArialMT_Plain_24);
  display.drawString(HALF-2, 12, String::format(f_degF, outdoor.c_str()));

  int lastMin = (millis() - lastUpdate) / 60 / 1000;
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 50, String::format(f_lastUpdate, lastMin));

  display.display();
}


void setup() {
  Particle.function("update", tempUpdate);

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  display.init();
  display.flipScreenVertically();

  drawWaitingDisplay();
}

void loop() {

  if (lastUpdate > 0) {
    drawTempDisplay();
  }

  delay(UPDATE_PERIOD);  
}
