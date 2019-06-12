#include "mbed.h"
#include "TextLCD.h"
Serial pc(USBTX, USBRX);
TextLCD lcd(p15, p16, p17, p18, p19, p20);
BusOut rows(p26,p25,p24);
BusIn cols(p14,p13,p12,p11);
SPI sw(p5, p6, p7);
DigitalOut cs(p8);
char Keytable[] = { 'F', 'E', 'D', 'C',   //simulate layout of the keypad
                    '3', '6', '9', 'B',
                    '2', '5', '8', '0',
                    '1', '4', '7', 'A'
                  };
char getKey()
{
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
int i=1;
char b;
char array[] = {' '};// to save the number that type in
char stp[4]= {' ',' ',' ',' '};
char step_2[] = {' '};
char step_3[] = {' '};
int step = 1;
int in_array=0;
int runtime=0;
int task[9][10]= {};
int task_sor[2][9]= {};
int save[9]= {};
int t,high_pir,tsk,lcm,num,max,di,per,ary,a,k,j,x,m,tol,nmtol;
int miss=0;
int check=0;
int ms=0;
int gcd(int a, int b) // find the gcd
{
    if (b == 0) {//sort out any 0
        return a;
    }
    return gcd(b, a % b);
}
int find_lcm()
{
    lcm=1;
    for (in_array=0; in_array<(num); in_array++) { //sort the array in accending order
        for (j = in_array + 1; j < num; j++) {
            if(task[in_array][2]>task[j][2]) {
                for(i=0; i<10; i++) {
                    task_sor[0][i]=task[in_array][i];//save to stack
                    task[in_array][i]=task[j][i];
                    task[j][i]=task_sor[0][i];//put back the number
                }
            }
        }
    }
    for (in_array=0; in_array<9; in_array++) { //compute the super period(the LCM)
        if (task[in_array][2]==0) {
            continue;
        } else {
            lcm=task[in_array][2]*lcm/gcd(task[in_array][2], lcm);   //calculate the LCM from GCD
        }
    }
    return lcm;
}
void lt(int light)
{
    int pos=0;
    pos= pos+(3<<((light-1)*2));
    sw.write(pos);
    cs = 1;
    cs = 0;
}
void RMS()
{
    k=0;
    t=find_lcm();
    lcd.cls();
    while(k<(t+1)) {
        runtime=0;
        lcd.locate(0,0);
        lcd.printf("t= %d %d",k,task[1][4]);//print the time
        lcd.locate(0,1);
        lcd.printf("                ");//clear the display
        lcd.locate(6,0);
        lcd.printf("         ");
        for(ary=0; ary<num; ary++) {
            if(task[ary][3]==task[ary][1]|| check==1) {//check the complete displayed or not
                if (task[ary][7]==0) {
                    lcd.locate(0,1);
                    lcd.printf("task %d complete",task[ary][0]);
                    task[ary][7]=1;
                }
            }
        }
        check=0;
        for(ary=0; ary<num; ary++) {
            if((k % task[ary][2]==0) && (k>0) && (k!=t)) { //to check deadline missed or not and prepare for next deadline
                task[ary][5]=task[ary][5]+1;//add one when each time finish a cycle
                if (task[ary][3]==task[ary][1]) {
                    task[ary][4]=task[ary][4]+1;
                }
                if((t/task[ary][2]!=task[ary][4])&&(task[ary][3]==task[ary][1]) && (task[ary][4] == task[ary][5])) { //before finish run the following
                    task[ary][3]=0;//reset the run time
                    task[ary][7]=0;
                }
            }
            if(task[ary][4] != task[ary][5] && (k!=t)) {
                lcd.locate(6,0);
                lcd.printf("tk %d miss",task[ary][0]);
                miss=miss+task[ary][5]-task[ary][4];//check if there is multiple miss
            }
            if((task[ary][3]==task[ary][1]) && (task[ary][2]>0) && (runtime==0)) { //led go dark when task finish its task
                sw.write(0);
                cs=1;
                cs=0;
            }
            if(k==t) { // when reach the super period display finish
                for(ary=0; ary<num; ary++) {
                    if(task[ary][4] != task[ary][5]) {
                        task[ary][5]=task[ary][5]+1;
                    }
                    miss=miss+task[ary][5]-task[ary][4];//do the further increase for the end period
                }
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("total miss:%d    finish",miss);
            }
            if (task[ary][3]!=task[ary][1] && task[ary][2]>0 && runtime==0 && (k!=t)) { //when the task isn't finish lightup task[ary][3]=0 originally only one task run at a time
                runtime=1;
                lt(task[ary][0]);
                if (task[ary][4] == task[ary][5]) {//each time finish a task a check will add one
                    task[ary][3]=task[ary][3] + 1;
                } else if(task[ary][4] != task[ary][5]) {
                    task[ary][3]=task[ary][3] + 1;
                    if(task[ary][3]==task[ary][1]) {
                        task[ary][4]=task[ary][4]+1;
                        task[ary][3]=0;
                        check=1;
                        task[ary][7]=0;
                    }
                }
            }
        }
        k=k+1;//time increase by 1
        //wait(1);//delay one second
    }
}
void EDF()
{
    t=find_lcm();
    lcd.cls();
    for(ary=0; ary<num; ary++) {
        task[ary][8]=(task[ary][2]);
    }
    while(k<(t+1)) {
        runtime=0;
        lcd.locate(0,0);
        lcd.printf("%d",k);
        lcd.locate(0,1);
        lcd.printf("               ");
        lcd.locate(6,0);
        lcd.printf("         ");
        for(ary=0; ary<num; ary++) {
            if(task[ary][3]==task[ary][1]|| check==1) {//check the complete displayed or not
                lcd.locate(0,1);
                lcd.printf("task %d complete",task[ary][0]);
            }
            if((k % task[ary][2]==0) && (k>0)) { //to check deadline missed or not and prepare for next deadline
                task[ary][5]=task[ary][5]+1;//add one when each time finish a cycle
                if (task[ary][3]==task[ary][1]&& check==0) {
                    task[ary][4]=task[ary][4]+1;
                }
            }
            if(task[ary][4] != task[ary][5]) {
                ms=1;
            }
        }
        for(ary=0; ary<num; ary++) {
            if(k % task[ary][2]==0 && k>0 && task[ary][4] == task[ary][5]) {
                task[ary][8]=task[ary][8]+task[ary][2];
            } else if(ms==1 && task[ary][3]==task[ary][1]) {
                task[ary][8]=task[ary][8]+task[ary][2];
            }
            if (check==1) {
                task[ary][3]=0;
            }
        }
        for(in_array=0; in_array<num; in_array++) {//keep sort the array according to priority change
            for (j = in_array + 1; j < num; j++) {//sort the array and all element in it
                if( task[in_array][8]> task[j][8]) {
                    for(i=0; i<10; i++) {
                        task_sor[0][i]=task[in_array][i];//save to stack
                        task[in_array][i]=task[j][i];
                        task[j][i]=task_sor[0][i];//put back the number
                    }
                }
            }
        }
        check=0;
        for(ary=0; ary<num; ary++) {
            if((k % task[ary][2]==0) && (k>0)) { //prepare for next deadline
                if((t/task[ary][2]!=task[ary][4])&&(task[ary][3]==task[ary][1]) && (task[ary][4] == task[ary][5])) { //before finish run the following
                    task[ary][3]=0;//reset the run time
                }
            }
            if(task[ary][4] != task[ary][5] && (k!=t)) {
                lcd.locate(6,0);
                lcd.printf("tk %d miss",task[ary][0]);
                miss=miss+(task[ary][5]-task[ary][4]);
            }
            if((task[ary][3]==task[ary][1]) && (task[ary][2]>0) && (runtime==0)) { //led go dark when task finish its task
                sw.write(0);
                cs=1;
                cs=0;
            }
            if(k==t) { //when reach the super period display finish
                for(ary=0; ary<num; ary++) {
                    miss=miss+task[ary][5]-task[ary][4];//miss need further increase due to one increase of check at the end
                }
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("total miss:%d    finish",miss);
            }
            if (task[ary][3]!=task[ary][1] && task[ary][2]>0 && runtime==0) { //when the task isn't finish lightup task[ary][3]=0 originally only one task run at a time
                runtime=1;
                lt(task[ary][0]);
                if (task[ary][4] == task[ary][5]) {//each time finish a task a check will add one
                    task[ary][3]=task[ary][3] + 1;
                } else if(task[ary][4] != task[ary][5]) {
                    task[ary][3]=task[ary][3] + 1;
                    if(task[ary][3]==task[ary][1]) {
                        task[ary][4]=task[ary][4]+1;
                        check=1;
                    }
                }
            }
        }
        k=k+1;
        //wait(1);
    }
}
int main()
{
    lcd.cls();
    lcd.locate(0, 0);
    lcd.printf("RMS or EDF");
    lcd.locate(0, 1);
    lcd.putc(array[0]);
    while (1) {
        while (step == 1) {
            lcd.locate(0, 1);
            lcd.putc(array[0]);
            b=getKey();//get the key every cycle
            if(b != ' '&& b !='F'&& b !='E'&&  b !='B'&& b !='A') { // Display only in number avoid any char and space
                if (array[0]== ' ' && b!='D'&& b!='C') {
                    array[0]=b;// update the array to the number that pressed
                } else if (b=='D') {
                    array[0]=' ';//when press D return array to space
                    lcd.locate(2, 1);
                    lcd.printf("     ");
                } else if (b=='C') { //clear the enter
                    stp[0]=array[0];
                    if (stp[0]=='1'||stp[0]=='2' ) {//only accept 1 and 2
                        step = 2;
                        lcd.cls();
                    } else if(stp[0]!= '1'||stp[0]!= '2') { //error enter
                        lcd.locate(2, 1);
                        lcd.printf("Error");
                    }
                }
                wait(0.2);// avoid boundcing problem by wait of each key longer
            }
        }
        while (step == 2) {
            lcd.locate(0, 0);
            lcd.printf("Number of Tasks");
            lcd.locate(0, 1);
            lcd.putc(stp[1]);
            b=getKey();
            if(b != ' '&& b !='F'&& b !='E'&&  b !='B'&& b !='A') {
                if (stp[1]== ' ' && b!='D'&& b!='C') {
                    stp[1]=b;
                } else if (b=='D') {
                    stp[1]=' ';
                    lcd.locate(2, 1);
                    lcd.printf("     ");
                } else if (b=='C') {
                    if (stp[1] == '0' || stp[1] == '9' ||stp[1] == ' ') { //only number between 0-9 could run no empty space is allow
                        lcd.locate(2, 1);
                        lcd.printf("Error");
                    } else if (stp[1] != '0' ||stp[1] != '9') {
                        num = stp[1] - '0';
                        step = 3;
                        lcd.cls();
                    }
                }
                wait(0.2);
            }
        }
        while (step == 3) {
            lcd.locate(0,0);
            lcd.printf("Task %d Runtime",i);
            lcd.locate(0, 1);
            lcd.putc(stp[2]);
            b=getKey();
            if(b != ' '&& b !='F'&& b !='E'&&  b !='B'&& b !='A') {
                if (stp[2]== ' ' && b!='D'&& b!='C') {
                    stp[2]= b;
                } else if (b=='D') {
                    stp[2]=' ';
                    lcd.locate(2, 1);
                    lcd.printf("     ");
                } else if (b=='C') {
                    if (stp[2]== ' ' || stp[2]=='0') { //only number larger than 0 could run no space is allow
                        lcd.locate(2, 1);
                        lcd.printf("Error");
                    }
                    if (stp[2] != ' ' && stp[2] != '0') {
                        per = stp[2]- '0';
                        lcd.locate(0,1);
                        lcd.putc(stp[2]);
                        step=4;
                    }
                }
                wait(0.2);
            }
        }
        while (step == 4) {
            lcd.locate(0,0);
            lcd.printf("Task %d Period  ",i);
            lcd.locate(0, 1);
            lcd.putc(stp[3]);
            b=getKey();
            if(b != ' '&& b !='F'&& b !='E'&&  b !='B'&& b !='A') {
                if (stp[3]== ' ' && b!='D'&& b!='C') {
                    stp[3]=b;
                } else if (b=='D') {
                    stp[3]=' ';
                    lcd.locate(2, 1);
                    lcd.printf("     ");
                } else if (b=='C') {
                    if (stp[3] == '0'||stp[3] == ' ' ) {//no space and 0 is allow
                        lcd.locate(2, 1);
                        lcd.printf("Error");
                    } else if (stp[3] != ' ') {
                        int period = stp[3]- '0';
                        task[i-1][2]=period;//save period in array
                        task[i-1][1]=per;//save runtime in array
                        task[i-1][0]=i;// save task number in array
                        lcd.locate(0,1);
                        lcd.putc(stp[3]);
                        lcd.locate(0,1);
                        lcd.printf("     ");
                        i+=1;
                        step=3;
                        stp[2]=' ';
                        stp[3]=' ';
                        if (i==num+1) {
                            step=5;
                        }
                    }
                }
                wait(0.2);
            }
        }
        if(step==5) {
            if(stp[0]=='2') {//if in step 1 is EDF
                EDF();
                break;
            } else if(stp[0]=='1') {// if in step 1 isRMS
                RMS();
                break;
            }
        }

    }
}
