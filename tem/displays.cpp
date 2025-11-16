#include "displays.h"
#include "op_logo.h"

Displays::Displays() {}

bool Displays::initDisplays()
{

	Adafruit_SSD1306 display0(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_Bus1, OLED_RESET);
	Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_Bus1, OLED_RESET);
	Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_Bus0, OLED_RESET);
	Adafruit_SSD1306 display3(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_Bus0, OLED_RESET);
	// Adafruit_SSD1306 updateDisplay = display0;
	U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

	// Array of screens for programatic doing things to each one
	Adafruit_SSD1306 displayArray[] = {display0, display1, display2, display3};

	// Four addresses for pointing to each oled individually over I2C
	int addressArray[] = {SCREEN_ADDRESS_1, SCREEN_ADDRESS_0, SCREEN_ADDRESS_1, SCREEN_ADDRESS_0};

	for (int i = 0; i < slots; i++)
	{

		if (!displayArray[i].begin(SSD1306_SWITCHCAPVCC, addressArray[i]))
		{
			Serial.println(F("SSD1306 allocation failed"));
			for (;;)
				; // Don't proceed, loop forever
		}
		else
		{

			drawOPLogo(displayArray[i]);
		}
	}
	delay(500);
	return true;
}

void Displays::drawOPLogo(Adafruit_SSD1306 &display)
{

	for (int y = 0; y <= LOGO_WIDTH; y += 5)
	{

		Serial.println(y);

		display.clearDisplay();

		display.drawBitmap((disp_w - y) / 2, (disp_h - y) / 2, op_white_70x70_inv, y, y, 1);
		display.display();
		delay(1);
	}
}

void Displays::spoolWeightDisplay()
{

	/*
	screenMode = "spool_weight";

	int vectorSize = spoolsVector.size();


	for (size_t i = 0; i < vectorSize; i++) {

	  displayArray[i].clearDisplay();

	  int disp_padding_top = 20;
	  int meter_w = disp_w - 20;
	  int meter_h = 20;
	  int meter_x = (disp_w - meter_w) /2;
	  int meter_y = disp_padding_top;
	  int text_center_x;
	  int text_y = (disp_padding_top + meter_h) + 20;
	  int text_w;
	  int text_h = 10;

	  u8g2_for_adafruit_gfx.begin(displayArray[i]);
	  u8g2_for_adafruit_gfx.setFont(u8g2_font_crox2hb_tr); // 10px high  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
	  u8g2_for_adafruit_gfx.setFontMode(0);                 // use u8g2 transparent mode (this is default)
	  u8g2_for_adafruit_gfx.setFontDirection(0);
	  u8g2_for_adafruit_gfx.setForegroundColor(WHITE);

	  displayArray[i].clearDisplay();
	  displayArray[i].drawRect(meter_x, disp_padding_top, meter_w, meter_h, SSD1306_WHITE);

	  int remWeightInt = spoolsVector[i]["remaining_weight"];
	  std::string remWeightStr = std::to_string(remWeightInt) + "g / 1000g";
	  const char* remWeightPtr = remWeightStr.c_str();
	  uint8_t weight_meter = map(remWeightInt, 0, 1000, 0, meter_w);
	  uint16_t weight_perc = map(remWeightInt, 0, 1000, 0, 100);
	  std::string weight_perc_str = std::to_string(weight_perc) + "%";
	  const char* weight_perc_ptr = weight_perc_str.c_str();
	  int weight_perc_y = (meter_y + meter_h) - 5;

	  displayArray[i].fillRect(meter_x, disp_padding_top, weight_meter, meter_h, SSD1306_WHITE);

	  // text_w = u8g2_for_adafruit_gfx.getUTF8Width("Weight Remaining");
	  // text_center_x = (displayArray[i].width() - text_w) /2;
	  // u8g2_for_adafruit_gfx.setCursor(text_center_x, (disp_padding_top+meter_h)+15);
	  // u8g2_for_adafruit_gfx.print("Weight Remaining");

	  text_w = u8g2_for_adafruit_gfx.getUTF8Width(remWeightPtr);
	  text_center_x = (disp_w - text_w) /2;
	  u8g2_for_adafruit_gfx.setCursor(text_center_x, text_y);
	  u8g2_for_adafruit_gfx.print(remWeightPtr);

	  text_w = u8g2_for_adafruit_gfx.getUTF8Width(weight_perc_ptr);
	  text_center_x = (disp_w - text_w) /2;
	  u8g2_for_adafruit_gfx.setCursor(text_center_x, weight_perc_y);
	  u8g2_for_adafruit_gfx.setBackgroundColor(SSD1306_BLACK);
	  u8g2_for_adafruit_gfx.print(weight_perc_ptr);

	  displayArray[i].display();
	  delay(1);

	}
	if (pageDisplays) {
	  delay(3000);
	  overviewDisplay();
	}
  }

  void Display::overviewDisplay() {

	// std::cout << "Address of spoolsVector: " << &spoolsVector << std::endl;

	screenMode = "overview";

	int vectorSize = spoolsVector.size();


	for (size_t i = 0; i < vectorSize; i++) {

	  int spoolId = spoolsVector[i]["id"];
	  int remWeight = spoolsVector[i]["remaining_weight"];
	  const char * material = spoolsVector[i]["filament"]["material"];
	  const char * name = spoolsVector[i]["filament"]["name"];

	  displayArray[i].clearDisplay();
	  u8g2_for_adafruit_gfx.begin(displayArray[i]);
	  u8g2_for_adafruit_gfx.setFont(u8g2_font_crox2hb_tr); // 10px high  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
	  u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
	  u8g2_for_adafruit_gfx.setFontDirection(0);
	  // u8g2_for_adafruit_gfx.setForegroundColor(WHITE);

	  u8g2_for_adafruit_gfx.setCursor(padding_screen_left, padding_screen_top + character_height);
	  u8g2_for_adafruit_gfx.print(name);
	  displayArray[i].startscrollright(0x02, 0x03);
	  // delay(2000);

	  u8g2_for_adafruit_gfx.setCursor(padding_screen_left, padding_screen_top + (character_height*2));
	  u8g2_for_adafruit_gfx.print(material);

	  // u8g2_for_adafruit_gfx.setCursor(padding_screen_left,  padding_screen_top + (character_height*3));
	  // u8g2_for_adafruit_gfx.print(F("Rem wt: ")); u8g2_for_adafruit_gfx.print(remWeight);

	  u8g2_for_adafruit_gfx.setCursor(disp_w-20,  padding_screen_top);
	  u8g2_for_adafruit_gfx.print(spoolId);

	  displayArray[i].invertDisplay(false);

	  displayArray[i].display();
	  delay(10);
	}

	if (vectorSize<slots) {

	  for (size_t i = vectorSize; i < slots; i++) {
		displayArray[i].clearDisplay();
		u8g2_for_adafruit_gfx.begin(displayArray[i]);
		u8g2_for_adafruit_gfx.setFont(u8g2_font_crox2hb_tr); // 10px high  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
		u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
		u8g2_for_adafruit_gfx.setFontDirection(0);
		const char *  updateMsg = "NO SPOOL";

		int16_t text_width = u8g2_for_adafruit_gfx.getUTF8Width(updateMsg);

		int16_t x1, y1;
		uint16_t w, h;
		displayArray[i].getTextBounds(updateMsg, 0, 0, &x1, &y1, &w, &h);
		int16_t text_center_x = disp_center_x - (text_width / 2);
		int16_t text_center_y = disp_center_y + (h/2);

		u8g2_for_adafruit_gfx.setCursor(text_center_x,text_center_y);     // Start at top-left corner
		// displayArray[i].invertDisplay(true);
		u8g2_for_adafruit_gfx.print(updateMsg);

		// u8g2_for_adafruit_gfx.print(updateMsg);
		displayArray[i].display();

	  }
	}

	if (pageDisplays) {
	  delay(3000);
	  spoolWeightDisplay();
	}
	  */
}

void Displays::updateDisplay(int displayId, int spoolId, int remWeight, const char *material, const char *name)
{

	screenMode = "overview";

	displayArray[displayId].clearDisplay();
	u8g2_for_adafruit_gfx.begin(displayArray[displayId]);
	u8g2_for_adafruit_gfx.setFont(u8g2_font_crox2hb_tr); // 10px high  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
	u8g2_for_adafruit_gfx.setFontMode(1);				 // use u8g2 transparent mode (this is default)
	u8g2_for_adafruit_gfx.setFontDirection(0);
	u8g2_for_adafruit_gfx.setForegroundColor(WHITE);
	int16_t disp_center_x = displayArray[displayId].width() / 2;
	int16_t disp_center_y = displayArray[displayId].height() / 2;

	int8_t font_height = 10;
	int8_t padding_screen_top = 10;
	int8_t padding_screen_left = 10;
	int8_t padding_font_bottom = 7;
	int8_t character_height = font_height + padding_font_bottom;

	u8g2_for_adafruit_gfx.setCursor(padding_screen_left, padding_screen_top);
	u8g2_for_adafruit_gfx.print(name);

	u8g2_for_adafruit_gfx.setCursor(padding_screen_left, padding_screen_top + character_height);
	u8g2_for_adafruit_gfx.print(material);

	u8g2_for_adafruit_gfx.setCursor(padding_screen_left, padding_screen_top + (character_height * 2));
	u8g2_for_adafruit_gfx.print(F("Rem wt: "));
	u8g2_for_adafruit_gfx.print(remWeight);

	u8g2_for_adafruit_gfx.setCursor(padding_screen_left, padding_screen_top + (character_height * 3));
	u8g2_for_adafruit_gfx.print(F("Spool Id: "));
	u8g2_for_adafruit_gfx.print(spoolId);

	displayArray[displayId].display();
	delay(10);
}

void Displays::begin()
{
	I2C_Bus0.begin(I2C0_SDA, I2C0_SCL, 100000);
	I2C_Bus1.begin(I2C1_SDA, I2C1_SCL, 100000);
}

void Displays::loop()
{
}

Displays::~Displays()
{
}