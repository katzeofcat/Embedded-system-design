#include "mbed.h"
#include "MPU6050.h"
#include "TextLCD.h"

TextLCD lcd(p15, p16, p17, p18, p19, p20); 
BusOut rows(p26,p25,p24);
BusIn cols(p14,p13,p12,p11);

char Keytable[] = { 'F', 'E', 'D', 'C',   //simulate layout of the keypad
                    '3', '6', '9', 'B',   
                    '2', '5', '8', '0',   
                    '1', '4', '7', 'A'    
                   };                  
char getKey(){
    int i,j;
    char ch=' ';
    
    for (i = 0; i <= 3; i++) {//get the key according to the row and col
        rows = i; 
        for (j = 0; j <= 3; j++) {           
            if (((cols ^ 0x00FF)  & (0x0001<<j)) != 0) {
                ch = Keytable[(i * 4) + j];
            }            
        }        
    }
    return ch;
}
int num_wd;
int i;
char b;
char array[] = {'_','_','_','_'};// to save the number that type in
int main() {
    lcd.cls();
    lcd.locate(0, 1);
    lcd.printf("Init  Code: ");
    lcd.locate(12, 1);
    for (i=0; i<4; i++){
        lcd.putc(array[i]);//display the "_" from the array
    }
    while (num_wd<4){
            b=getKey();//get the key every cycle
            if(b != ' '&& b !='F'&& b !='E'&& b !='D'&& b !='C'&& b !='B'&& b !='A'){ // Display only in number avoid any char and space
                if (array[num_wd]== '_'){
                    array[num_wd]=b;// update the array to the number that pressed
                    num_wd+=1;
                }
                lcd.locate(12, 1);
                for (i=0; i<4; i++){
                    lcd.putc(array[i]);
                }
                wait(0.5);// avoid boundcing problem by wait of each key longer
            }
    }
}

