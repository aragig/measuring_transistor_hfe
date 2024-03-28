#include <Arduino.h>
#include <Button.h>
#include <PollingTimer.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>


#define VOUT1_PIN A1
#define VOUT2_PIN A0
#define VE_PIN A2
#define VC_PIN A3

// Arduino Uno
#define  TFT_SCL 13
#define TFT_SDA 11
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_SDA, TFT_SCL, TFT_RST);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

Button button(2); // D2

enum TransistorType {
    NPN,
    PNP
};

struct TransistorCurrents {
    double Ib;
    double Ic;
};

TransistorType transistorType = PNP; // 最初に一回button.pressed()が呼び出されるのでNPNになる

PollingTimer timer;


void displayNpnPnp() {
    tft.fillRect(0, 100, 128, 20, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 100);
    if(transistorType == NPN) {
        tft.println("NPN");
    } else {
        tft.println("PNP");
    }
}

void displayCBEDiagram() {
    tft.setTextColor(ST77XX_BLACK);
    tft.fillRect(0, 120, 15, 15, ST77XX_BLUE);
    tft.setCursor(5, 125);
    tft.print("C");
    tft.fillRect(15, 120, 15, 15, ST77XX_GREEN);
    tft.setCursor(20, 125);
    tft.print("B");
    tft.fillRect(30, 120, 15, 15, ST77XX_RED);
    tft.setCursor(35, 125);
    tft.print("E");
    tft.fillRect(45, 120, 15, 15, ST77XX_BLUE);
    tft.setCursor(50, 125);
    tft.print("C");
    tft.fillRect(60, 120, 15, 15, ST77XX_GREEN);
    tft.setCursor(65, 125);
    tft.print("B");
    tft.fillRect(75, 120, 15, 15, ST77XX_RED);
    tft.setCursor(80, 125);
    tft.print("E");
}


TransistorCurrents measureNPN() {
    digitalWrite(VC_PIN, HIGH);
    digitalWrite(VE_PIN, LOW);

    uint16_t vout1 = analogRead(VOUT1_PIN);
    uint16_t vout2 = analogRead(VOUT2_PIN);

    double Ib = (5.0 - (float(vout1) / 1024.0) * 5.0) / 0.51; // 510k ohm
    double Ic = (5.0 - (float(vout2) / 1024.0) * 5.0) / 0.0001; // 100 ohm
    return {Ib, Ic};
}

TransistorCurrents measurePNP() {
    digitalWrite(VC_PIN, LOW);
    digitalWrite(VE_PIN, HIGH);

    uint16_t vout1 = analogRead(VOUT1_PIN);
    uint16_t vout2 = analogRead(VOUT2_PIN);
    Serial.println(vout1);
    Serial.println(vout2);

    double Ib = (float(vout1) / 1024.0 * 5.0) / 0.51; // 510k ohm
    double Ic = (float(vout2) / 1024.0 * 5.0) / 0.0001; // 100 ohm
    return {Ib, Ic};
}

void measuringTask() {

//    tft.fillScreen(ST77XX_BLACK);
    tft.fillRect(0, 0, 128, 80, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);

    TransistorCurrents currents = (transistorType == NPN) ? measureNPN() : measurePNP();

    Serial.print("Ib: ");
    Serial.print(currents.Ib);
    Serial.println(" uA");
    tft.setCursor(0, 0);
    tft.print("Ib: ");
    tft.print(currents.Ib);
    tft.println(" uA");


    Serial.print("Ic: ");
    Serial.print(currents.Ic);
    Serial.println(" uA");
    tft.setCursor(0, 15);
    tft.print("Ic: ");
    tft.print(currents.Ic);
    tft.println(" uA");

    double hFE = currents.Ic / currents.Ib;

    Serial.print("hFE: ");
    Serial.println(hFE);
    tft.setCursor(0, 30);
    tft.print("hFE: ");
    tft.println(hFE);

}

void setup() {
    Serial.begin(115200);
    button.begin();

    pinMode(VOUT1_PIN, INPUT);
    pinMode(VOUT2_PIN, INPUT);
    pinMode(VC_PIN, OUTPUT);
    pinMode(VE_PIN, OUTPUT);

    delay(500);
    tft.initR(INITR_GREENTAB);
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(0);
    tft.setTextWrap(false);
    delay(500);


    timer.setInterval(measuringTask, 1000, true);
    timer.fire();
    measuringTask();
    displayCBEDiagram();

}

void loop() {
    timer.handler();

    if (button.pressed()) {
        Serial.println("Button pressed");
        transistorType = (transistorType == NPN) ? PNP : NPN;
        displayNpnPnp();
    }
}

