#include "lcd_wrapper.h"
#include "mastermind.h"

void setup() {
    Serial.begin(9600);
    set_pin_mode();
    lcd_init();
    delay(4000);
    lcd_clear();
    restart_screen();
}

void loop() {
    // -------- PLAYING -------- //
    char* code;
    if (digitalRead(BTN_ENTER_PIN)) {
        delay(250);
        char* code = generate_code(false, 4);
        play_game(code);
    }
    //

    if (code) free(code);
}

/*
											         _nnnn_
											        dGGGGMMb
											       @p~qp~~qMb
											       M|@||@) M|
											       @,----.JM|
											      JS^\__/  qKL
											     dZP        qKRb
											    dZP          qKKb
											   fZP            SMMb
											   HZM            MMMM
											   FqM            MMMM
											 __| ".        |\dS"qML
											 |    `.       | `' \Zq
											_)      \.___.,|     .'
											\____   )MMMMMP|   .'
											     `-'       `--' 
										  ｍａｄｅ ｂｙ　ｍｉＲｘ９２３.⠀
*/
