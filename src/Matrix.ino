#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiUdp.h>

#define PIN D8         // 选择连接WS2812点阵屏幕的引脚
#define NUM_PIXELS 256 // 8x32点阵，共256个像素

const char *ssid = "TP-LINK_1F78";
const char *password = "guet1503";
const char *ntpServer = "time1.cloud.tencent.com"; // 时间服务器

const int timeZone = 8; // 时区

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

const uint8_t colon = 0x28;
const uint8_t digits[10][3] = {
    {0x7C, 0x22, 0x7C}, // 0
    {0x00, 0x3E, 0x00}, // 1
    {0x5C, 0x2A, 0x74}, // 2
    {0x54, 0x2A, 0x7C}, // 3
    {0x70, 0x08, 0x7C}, // 4
    {0x74, 0x2A, 0x5C}, // 5
    {0x7C, 0x2A, 0x5C}, // 6
    {0x40, 0x02, 0x7C}, // 7
    {0x7C, 0x2A, 0x7C}, // 8
    {0x74, 0x2A, 0x7C}, // 9
};

const uint32_t color_black = 0x0;
const uint32_t color_white = 0xFFFFFF;
const uint32_t color_red = 0xFF0000;
const uint32_t color_green = 0x00FF00;
const uint32_t color_blue = 0x0000FF;

WiFiUDP udp;

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    // pinMode(LED_BUILTIN, OUTPUT);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    configTime(timeZone * 3600, 0, ntpServer);

    matrix.begin();
    matrix.setBrightness(30); // 设置亮度，值范围从0到255，调整为适合您的亮度
    matrix.show();            // Initialize all pixels to 'off'
}

void loop()
{
    // rainbow(100); // 调用彩虹色效果函数
    // test();
    // test2();

    time_t now = time(NULL);

    struct tm *tm_info = localtime(&now);
    int hour = tm_info->tm_hour;
    int min = tm_info->tm_min;
    int sec = tm_info->tm_sec;
    hour = (hour > 12) ? hour - 12 : hour;

    Serial.printf("%02d:%02d:%02d\n", hour, min, sec);

    show_time(hour, min, sec);
    delay(1000);
}

void show_time(uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t color = color_green;
    // show hour
    display_digit(3, hour / 10, color);
    display_digit(7, hour % 10, color);
    display_colon(11, color);
    // show min
    display_digit(13, min / 10, color);
    display_digit(17, min % 10, color);
    display_colon(21, color);
    // show sec
    display_digit(23, sec / 10, color);
    display_digit(27, sec % 10, color);

    matrix.show();
}

void display_digit(uint8_t col_start, uint8_t digit, uint32_t color)
{
    for (uint8_t y = 0; y < 3; ++y)
    {
        for (uint8_t x = 0; x < 8; ++x)
        {
            uint32_t bit_color = (digits[digit][y] & (1 << x)) ? color : color_black;
            matrix.setPixelColor(x + (y + col_start) * 8, bit_color);
        }
    }
}

void display_colon(uint8_t col, uint32_t color)
{
    for (uint8_t x = 0; x < 8; ++x)
    {
        uint32_t bit_color = (colon & (1 << x)) ? color : color_black;
        matrix.setPixelColor(x + col * 8, bit_color);
    }
}

void test2()
{
    for (int i = 0; i < 10; i++)
    {
        displayDigit(i);
        delay(1000); // 每隔一秒显示一个数字
    }
}

void displayDigit(uint8_t digit)
{
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            uint32_t color = (digits[digit][y] & (1 << x)) ? matrix.Color(255, 255, 255) : matrix.Color(0, 0, 0);
            matrix.setPixelColor(x + (y + 1) * 8, color);
        }
    }
    matrix.show();
}

void test()
{
    uint32_t black = matrix.Color(0, 0, 0);
    uint32_t green = matrix.Color(0, 255, 0);

    for (uint16_t i = 0; i < matrix.numPixels(); ++i)
    {
        uint32_t color = (i % 2 ? green : black);
        matrix.setPixelColor(i, color);
        matrix.show();
        delay(20);
    }

    for (uint16_t i = 0; i < matrix.numPixels(); ++i)
    {
        matrix.setPixelColor(i, black);
    }
    matrix.show();
}

void rainbow(uint8_t wait)
{
    uint16_t i, j;

    for (j = 0; j < 256; j++)
    {
        // delay(40);
        for (i = 0; i < matrix.numPixels(); i++)
        {
            matrix.setPixelColor(i, Wheel((i + j) & 255));
        }
        matrix.show();
        delay(wait);
    }
}

uint32_t Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return matrix.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return matrix.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return matrix.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}