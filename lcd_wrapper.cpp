#include <Arduino.h>
#include "lcd_wrapper.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

byte customChar1[8] = {
    0b00001,
    0b00011,
    0b01111,
    0b11111,
    0b00011,
    0b00001,
    0b00001,
    0b00011
};

byte customChar2[8] = {
    0b00011,
    0b00111,
    0b00111,
    0b00111,
    0b00011,
    0b00111,
    0b00110,
    0b01100
};

byte customChar3[8] = {
    0b11000,
    0b11100,
    0b11100,
    0b11110,
    0b11110,
    0b11111,
    0b11111,
    0b11111
};

byte customChar4[8] = {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00111,
    0b01100,
    0b11000
};

byte customChar5[8] = {
    0b00000,
    0b10000,
    0b10000,
    0b10000,
    0b11000,
    0b11100,
    0b00000,
    0b00000
};

// Introduction screen
void lcd_init() {
    lcd.init();
    lcd.backlight();
    lcd.clear();

    lcd.print("<>Mastermind.");

    lcd.setCursor(3, 1);
    lcd.print("--------");

    lcd.createChar(0, customChar1);
    lcd.createChar(1, customChar2);
    lcd.createChar(2, customChar3);
    lcd.createChar(3, customChar4);
    lcd.createChar(4, customChar5);

    lcd.setCursor(13, 0);
    lcd.write((byte)0);

    lcd.setCursor(13, 1);
    lcd.write((byte)1);
    
    lcd.setCursor(14, 0);
    lcd.write((byte)2);

    lcd.setCursor(14, 1);
    lcd.write((byte)3);

    lcd.setCursor(15, 1);
    lcd.write((byte)4);
}

void lcd_clear() {
    lcd.clear();
}

void lcd_set_cursor(int y, int x) {
    lcd.setCursor(x, y);
}

void lcd_print(char* text) {
    lcd.print(text);
}

void lcd_print_at(int x, int y, char* text) {
    lcd.setCursor(x, y);
    lcd.print(text);
}

void lcd_home() {
    lcd.home();
}

// Screen that is used at start and at the end of game
void restart_screen() {
    lcd_print_at(0, 0, "Press ENTER to");
    lcd_print_at(0, 1, "start new game!");
}