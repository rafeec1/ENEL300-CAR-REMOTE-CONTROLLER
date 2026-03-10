#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SCK    9
#define OLED_MOSI   10
#define OLED_DC     12
#define OLED_CS     13
#define OLED_RST    11

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS);

void setup() {
    SPI.begin(OLED_SCK, -1, OLED_MOSI, OLED_CS);

    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW);
    delay(20);
    digitalWrite(OLED_RST, HIGH);
    delay(20);
    if(!display.begin(SSD1306_SWITCHCAPVCC)) {
        while(1); // loop forever of OLED ain't found
    }
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("waowi yaoi");
    display.display();
}

void loop() {

}