#include <SH1106.h>
#include "OLEDDisplayUi2.h"

#define UPDATE_PERIOD 10 * 1000
#define HALF DISPLAY_WIDTH / 2
#define BALL_COUNT 3

// Initialize the OLED display
SH1106 display(0x3c);
OLEDDisplayUi2 ui(&display);

// Global variables
long lastUpdate;
char *dataBuffer = new char[20];
char *indoorF, *outdoorF, *outdoorPrecipProb, *outdoorAQI;

// Const strings
static const String f_degF = "%s°F";
static const String f_percent = "%s%%";
static const String f_lastUpdate = "Updated %d min ago";
static const String f_curTime_date = "%a %b %e";
static const String f_curTime_time = "%l:%M%p";
static const String f_curTime = f_curTime_date + "  " + f_curTime_time;
static const char f_dataDelim[] = ",";

// Function callbacks

// 123,-234,123,123
int tempUpdate(String data)
{
  lastUpdate = millis();

  data.toCharArray(dataBuffer, 20);

  indoorF = strtok(dataBuffer, f_dataDelim);
  outdoorF = strtok(NULL, f_dataDelim);
  outdoorPrecipProb = strtok(NULL, f_dataDelim);
  outdoorAQI = strtok(NULL, f_dataDelim);

  ui.invalidateCurrentFrame();

  return 0;
}

// Display frames

void drawClock(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  // Show current time
  time_t time = Time.now();
  display->setFont(ArialMT_Plain_16);

  display->drawString(0 + x, 0 + y, Time.format(time, f_curTime_date));
  display->drawString(0 + x, 20 + y, Time.format(time, f_curTime_time));

  state->isFrameDoneRendering = true;
}

void drawTempDisplay(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  if (lastUpdate > 0)
  {
    display->drawString(0 + x, 0 + y, "Indoor");
    display->setFont(ArialMT_Plain_24);
    display->drawString(0 + x, 12 + y, String::format(f_degF, indoorF));

    display->setFont(ArialMT_Plain_10);
    display->drawString((HALF - 2) + x, 0 + y, "Outdoor");
    display->setFont(ArialMT_Plain_24);
    display->drawString((HALF - 2) + x, 12 + y, String::format(f_degF, outdoorF));

    // Show current time
    display->setFont(ArialMT_Plain_10);
    display->drawString(0 + x, 38 + y, Time.format(Time.now(), f_curTime));

    // Show last update in minutes
    int lastMin = (millis() - lastUpdate) / 60 / 1000;
    display->setFont(ArialMT_Plain_10);
    display->drawString(0 + x, 50 + y, String::format(f_lastUpdate, lastMin));
  }
  else
  {
    display->drawString(0 + x, 0 + y, "Waiting for update.");
    display->drawString(0 + x, 38 + y, Time.format(Time.now(), f_curTime));
  }

  state->isFrameDoneRendering = true;
}

void drawWeatherDisplay(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  if (lastUpdate > 0)
  {
    display->drawString(0 + x, 0 + y, "Rain chance");
    display->setFont(ArialMT_Plain_24);
    display->drawString(0 + x, 12 + y, String::format(f_percent, outdoorPrecipProb));

    display->setFont(ArialMT_Plain_10);
    display->drawString((HALF - 2) + x, 0 + y, "AQI");
    display->setFont(ArialMT_Plain_24);
    display->drawString((HALF - 2) + x, 12 + y, outdoorAQI);

    // Show current time
    display->setFont(ArialMT_Plain_10);
    display->drawString(0 + x, 38 + y, Time.format(Time.now(), f_curTime));

    // Show last update in minutes
    int lastMin = (millis() - lastUpdate) / 60 / 1000;
    display->setFont(ArialMT_Plain_10);
    display->drawString(0 + x, 50 + y, String::format(f_lastUpdate, lastMin));
  }
  else
  {
    display->drawString(0 + x, 0 + y, "Waiting for update.");
    display->drawString(0 + x, 38 + y, Time.format(Time.now(), f_curTime));
  }

  state->isFrameDoneRendering = true;
}

struct BouncyBall
{
  uint8_t bounceX, bounceY;
  int8_t bounceVecX, bounceVecY;
};
BouncyBall balls[BALL_COUNT];

void drawBounce(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  if (state->isFirstTick)
  {
    // Reset bounce vectors
    for (uint8_t i = 0; i < BALL_COUNT; i++)
    {
      balls[i].bounceX = random(0, DISPLAY_WIDTH);
      balls[i].bounceY = random(0, DISPLAY_HEIGHT);
      balls[i].bounceVecX = random(2, 6);
      balls[i].bounceVecY = random(1, 5);
    }
  }

  if (state->frameState == FIXED && state->ticksSinceLastStateSwitch > 0)
  {
    for (uint8_t i = 0; i < BALL_COUNT; i++)
    {
      // Bounce against walls
      if (((balls[i].bounceX + balls[i].bounceVecX) > DISPLAY_WIDTH) || ((balls[i].bounceX + balls[i].bounceVecX) < 0))
      {
        balls[i].bounceVecX = -balls[i].bounceVecX;
      }
      if (((balls[i].bounceY + balls[i].bounceVecY) > DISPLAY_HEIGHT) || ((balls[i].bounceY + balls[i].bounceVecY) < 0))
      {
        balls[i].bounceVecY = -balls[i].bounceVecY;
      }

      // Move at determined vector
      balls[i].bounceX += balls[i].bounceVecX;
      balls[i].bounceY += balls[i].bounceVecY;

      // Draw
      display->fillCircle(balls[i].bounceX + x, balls[i].bounceY + y, 2);
    }
  }
}

FrameCallback frames[] = {drawClock, drawTempDisplay, drawBounce, drawWeatherDisplay};
int frameCount = 4;

// Main loop

void setup()
{
  Particle.function("update", tempUpdate);
  Time.zone(-8); // TODO: Don't hardcode this
  Time.beginDST();

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  ui.setTargetFPS(30);
  ui.setFrameAnimation(SLIDE_LEFT);
  ui.setFrames(frames, frameCount);
  ui.enableAutoTransition();
  ui.setTimePerFrame(10 * 1000);
  ui.init();

  display.flipScreenVertically();
}

void loop()
{
  int8_t remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0)
  {
    // Do extra stuff if we need to

    // Delay for the rest
    delay(remainingTimeBudget);
  }
}
