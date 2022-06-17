#include <Arduino.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lcd_wrapper.h"
#include "mastermind.h"

/*
*
* Change this global arrays if you add or remove LEDs or buttons, all the functions use them. 
* If you add LEDs or buttons you need to #define them in mastermind.h 
*
*/
const short buttonArr[] = {BTN_ENTER_PIN, BTN_1_PIN, BTN_2_PIN, BTN_3_PIN, BTN_4_PIN, '\0'};
const short ledsArr[] = {LED_RED_1, LED_RED_2, LED_RED_3, LED_RED_4, LED_BLUE_1, LED_BLUE_2, LED_BLUE_3, LED_BLUE_4, '\0'};
const short redArr[] = {LED_RED_1, LED_RED_2, LED_RED_3, LED_RED_4, '\0'};
const short blueArr[] = {LED_BLUE_1, LED_BLUE_2, LED_BLUE_3, LED_BLUE_4, '\0'};
//

void play_game(char* secret) {
    /*
    *
    * !!! This implementation is only capable of guessing a 4 digit secret number !!!
    *
    */

    // Prepare LCD and LEDs for new game
    turn_off_leds();
    lcd_clear();
    // Initialize some variables
    bool found_secret = false;
    int peg_a = 0;
    int peg_b = 0;

    char try_disp[] =   {'0', '1', ':', '\0'};
    char pegs_disp[] =  {'0', 'A', '0', 'B', '\0'};
    char guess[] =      {'0', '0', '0', '0', '\0'};
    char last_guess[] = {'0', '0', '0', '0', '\0'};

    // Endless game loop
    while (1) {
        // Constantly print everything that is needed in-game on LCD
        lcd_print_at(0, 0, try_disp);
        lcd_print_at(4, 0, last_guess);
        lcd_print_at(9, 0, pegs_disp);
        lcd_print_at(0, 1, "Your guess: ");
        lcd_print(guess);

        // Waiting for button input
        if (digitalRead(BTN_ENTER_PIN)) {
            // Checks if players guess is correct and displays result with LEDs for 1,5 seconds
            get_score(secret, guess, &peg_a, &peg_b);
            render_leds(peg_a, peg_b);
            delay(1500);
            turn_off_leds();
            // If number of digits guessed right on right position is 4 then the player wins
            if (peg_a == strlen(secret)) found_secret = true; 
            for (short j = 0; j < 4; j++) last_guess[j] = guess[j];
            // This resets guess number to 0000 so the player can set new input number
            for (short i = 0; i < 4; i++) guess[i] = '0'; guess[4] = '\0';
            try_disp[1]++;
            pegs_disp[0] = peg_a + '0';
            pegs_disp[2] = peg_b + '0';

            // If number of tries is 9 and player pushes ENTER, the tries counter will change to 10
            if (try_disp[1] > '9') {
            try_disp[1] = '0';
            try_disp[0]++;
            }
        }
        // Increment 1st guess number
        if (digitalRead(BTN_1_PIN)) {
            delay(250); 
            guess[0] = guess[0] + 1; 
            if (guess[0] > '9') guess[0] = '0';
        }
        // Increment 2nd guess number
        if (digitalRead(BTN_2_PIN)) {
            delay(250); 
            guess[1] = guess[1] + 1; 
            if (guess[1] > '9') guess[1] = '0';
        }
        // Increment 3rd guess number
        if (digitalRead(BTN_3_PIN)) {
            delay(250);
            guess[2] = guess[2] + 1; 
            if (guess[2] > '9') guess[2] = '0';
        }
        // Increment 4th guess number
        if (digitalRead(BTN_4_PIN)) {
            delay(250); 
            guess[3] = guess[3] + 1; 
            if (guess[3] > '9') guess[3] = '0';
        }

        // End game loop if game is won or lost
        if (found_secret || (try_disp[0] == '1' && try_disp[1] == '1') ) break;
    }

    // LEDs and LCD after winning 
    if (found_secret) {
        lcd_clear();
        lcd_print_at(0, 0, try_disp);
        lcd_print_at(4, 0, last_guess);
        lcd_print_at(9, 0, pegs_disp);
        lcd_print_at(0, 1, "Well done!");
        
        // Winning LEDs (duration of effect is 3 second) 
        win_leds();
        restart_screen();
    } else {
    // LEDs and LCD after losing
        lcd_clear();
        try_disp[1]--;
        lcd_print_at(0, 0, try_disp);
        lcd_print_at(4, 0, last_guess);
        lcd_print_at(9, 0, pegs_disp);
        lcd_print_at(0, 1, "Code was: ");
        lcd_print(secret);

        // RED losing LEDs turned on for 3 seconds (because winning LEDs effect is 3 seconds long too)
        lose_leds();
        delay(3000);
        restart_screen();
    }
}

char* generate_code(bool repeat, int length) {
    // Set random seed with some UNUSED analog pin
    randomSeed(analogRead(1));
    if (!length || length < 1) return NULL;
    if (!repeat && length > 10) return NULL;

    // Allocate memory for code length + terminator( \0 ), one number has size of char
    char* code = (char*)calloc(length + 1, sizeof(char));

    // Generate random number while numbers CAN repeat
    if (repeat == true) {
        for (short i = 0; i < length; i++) {
            code[i] = random(0, 10) + '0';
        }
    }

    // Generate random number while numbers CANT repeat
    if (repeat == false) {
        // Fisher–Yates shuffle algorithm
        char num_arr[10] = {'0','1','2','3','4','5','6','7','8','9',};

        for (short i = 9; i > 0; i--) {
            short j = random(0, i);
            char temp = num_arr[i];
            num_arr[i] = num_arr[j];
            num_arr[j] = temp;
        }

        for (short k = 0; k < length; k++) {
            code[k] = num_arr[k];
        }
        //
    }

    code[length] = '\0';
    
    // This print is for cheating
    Serial.println(code);

    return code;
}

void get_score(const char* secret, const char* guess, int* peg_a, int* peg_b) {
    *peg_a = 0;
    *peg_b = 0;

    // Finds out how many digits are GUESSED RIGHT AND on the RIGHT PLACE 
    for (short i = 0; secret[i] != '\0'; i++) {
        if (secret[i] == guess[i]) {
            (*peg_a)++;
        }
    }

    // Finds out how many digits are GUESSED RIGHT BUT on the WRONG PLACE 
    for (short i = 0; secret[i] != '\0'; i++) {
        for (short j = 0; guess[j] != '\0'; j++) {
            if (secret[i] == guess[j] && i != j) {
                (*peg_b)++;
            }
        }
    }
}

void render_leds(const int peg_a, const int peg_b) {
    // Light up red LEDs depending on the peg_a value
    for (short i = 0; i < peg_a; i++) {
        digitalWrite(redArr[i], HIGH);
        if (i >= 4) break;
    }

    // Light up red LEDs depending on the peg_b value
    for (short j = 0; j < peg_b; j++) {
        digitalWrite(blueArr[j + peg_a], HIGH);
        if (j + peg_a >= 4) break;
    }
}

void turn_off_leds() {
    // Turn off all LEDs
    for (short LED = 0; ledsArr[LED] != '\0'; LED++) {
        digitalWrite(ledsArr[LED], LOW);
    }
}

void set_pin_mode() {
    // Sets pinMode for all buttons except ENTER BUTTON 
    for (short BTN = 0; buttonArr[BTN] != '\0'; BTN++) pinMode(buttonArr[BTN], INPUT);

    // Sets pinMode for LEDs
    for (short LED = 0; ledsArr[LED] != '\0'; LED++) pinMode(ledsArr[LED], OUTPUT);
}

void win_leds() {
    // Blink both colors 3 times after winning game
    for (short i = 0; i <= 3; i++) {
        // BLUE LEDs BLINK
        for (short blue = 0; blueArr[blue] != '\0'; blue++) digitalWrite(blueArr[blue], HIGH);
        delay(250);

        for (short blue = 0; blueArr[blue] != '\0'; blue++) digitalWrite(blueArr[blue], LOW);
        delay(250);
        //
        // RED LEDs BLINK
        for (short red = 0; redArr[red] != '\0'; red++) digitalWrite(redArr[red], HIGH);
        delay(250);
        
        for (short red = 0; redArr[red] != '\0'; red++) digitalWrite(redArr[red], LOW);
        delay(250);
        //
    }
}

void lose_leds() {
    // Light up red LEDs after losing game
    for (short LED = 0; redArr[LED] != '\0'; LED++) digitalWrite(redArr[LED], HIGH);

    if (digitalRead(BTN_ENTER_PIN)) turn_off_leds();
}