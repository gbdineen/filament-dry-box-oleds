#ifndef DISPLAY_CONTROL_H
#define DISPLAY_CONTROL_H

#include "spools.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <iostream>
#include <string> 
#include <vector>
#include <ArduinoJson.h>

class Displays
{


    public:
        Displays(Spools& spoolsRef);
        ~Displays();  
        bool initDisplays();
        void drawOPLogo(Adafruit_SSD1306 &display);
        void spoolWeightDisplay();
        void overviewDisplay();
        void startPageDisplays();
        void stopPageDisplays();
        void setDisplayPaging(bool paging);
        void printPersistantInfo(U8G2_FOR_ADAFRUIT_GFX &gfx, int &sid, const char * &m);
        void printMessage(int& d, const char*& msg, bool inv);
        void begin();
        void loop();


    private:

        Spools& spoolsRef;

        int SCREEN_WIDTH = 128;
        int SCREEN_HEIGHT = 64;
        int OLED_RESET = -1; // Reset pin # (or -1 if sharing Arduino reset pin)
        int SCREEN_ADDRESS_0 = 0x3C; ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
        int SCREEN_ADDRESS_1 = 0x3D;
        int SCREEN_ADDRESS_2 = 0x3C;
        int SCREEN_ADDRESS_3 = 0x3D;
        int I2C0_SDA = 9; // &Wire Bus (default )
        int I2C0_SCL = 8;
        int I2C1_SDA = 5;  // &I2C_Bus1  
        int I2C1_SCL = 4;

        TwoWire  I2C_Bus0 = TwoWire(0);
        TwoWire  I2C_Bus1 = TwoWire(1);

        U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;
        Adafruit_SSD1306 display0;
        Adafruit_SSD1306 display1;
        Adafruit_SSD1306 display2;
        Adafruit_SSD1306 display3;
        Adafruit_SSD1306 displayArray[4];
        int addressArray[4];
    
        // DISPLAY STUFF
        int slots = 4;  // Number of spool slots, also happens to be the number of displays
        std::string screenMode = "overview";
        uint8_t disp_w = 128;
        uint8_t disp_h = 64;
        uint8_t disp_center_x = disp_w/2;
        uint8_t disp_center_y = disp_h/2;
        uint8_t font_height = 10;
        uint8_t padding_screen_top = 10;
        uint8_t padding_screen_left = 10;
        uint8_t padding_font_bottom = 7;
        uint8_t character_height = font_height + padding_font_bottom;
        bool displayPaging = false; // Rotate through screen views
        long pageInterval = 3500;
        long previousMillis = 0;
      
};



#endif