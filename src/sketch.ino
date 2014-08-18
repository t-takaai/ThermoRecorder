//
//      Save time and temprature to SD card 
//
//      https://github.com/t-takaai/ThermoRecorder
//
//      by T.Takaai (takoyaj@gmail.com)
//

#include <SD.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

const int microSDpin = 8;       //pin number for Sparkfun microSD shield
unsigned long sensorValue = 0;  //original analog read value (10 bit),10bitのアナログ読み取り値 
unsigned long Temp10=0;          //value of Temperature(℃)*10, 温度(℃)×10の値、23.5℃→235とする。
unsigned long AveN=100;         // average times for analog read, アナログ入力のための平均化回数
unsigned long i=0;              // repetition, 繰り返し回数
unsigned long timeMS=0;         // time in ms unit, ミリ秒
float aveAin=0;                 // average analog read,平均化されたアナログ入力値
String dataString = "";         // data in the string mode,データ送信用の文字列
String lcdString = "";         // data in the string mode,データ送信用の文字列
int RecTime = 60000;             // recording time
  
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

void setup(){
  analogReference(INTERNAL);                         //use internal voltage(1.1 V) as the analog input reference, アナログ入力の最大電力を1.1 Vとする。
  Serial.begin(9600);                               // serial communication rate, シリアル通信のレート
  Serial.print("Initializing SD card...");          //check the Serial communication 
  pinMode(microSDpin, OUTPUT);                       //Define chipselect terminal 8 as output, チップセレクトに使う8番端子はoutputとする。 
  if (!SD.begin(microSDpin)) {                       // check the SD card is available or not, SDカードが利用可能などうか確認
    Serial.println("Card failed, or not present");  // in the case of SD card error, SDカード読み取りエラーの時のメッセージ
  }else{
  Serial.println("Card initialized.");              //in the case of SD card is available, SDカードが読み取れた時のメッセージ
  }
    // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);

  // Print a message to the LCD. We track how long it takes since
  // this library has been optimized a bit and we're proud of it :)
  int time = millis();
  //  lcd.clear();
  //  lcd.setCursor(0,0);
  lcd.print("Hello, world!   ");
  time = millis() - time;
  Serial.print("Took "); Serial.print(time); Serial.println(" ms");
  lcd.setBacklight(WHITE);
}

// main program
void loop() {  
  tmElements_t tm;

  if (RTC.read(tm)) {
      /*
	Serial.print("Ok, Time = ");
	print2digits(tm.Hour);
	Serial.write(':');
	print2digits(tm.Minute);
	Serial.write(':');
	print2digits(tm.Second);
	Serial.print(", Date (D/M/Y) = ");
	Serial.print(tm.Day);
	Serial.write('/');
	Serial.print(tm.Month);
	Serial.write('/');
	Serial.print(tmYearToCalendar(tm.Year));
	Serial.println();
      */

      sensorValue=0;                                     // initialize the sensorValue,センサー読み取り値を0に戻す(和を取るため)
      for (i=0; i<AveN; i++){                            // Repeat AveN times, AveN回繰り返す。
	  sensorValue+=analogRead(0);                        // sum of analogRead(Ain terminal=0) for AveN times, 読み取り値をAveN回足す
      }
      timeMS=millis();                                   //time (msec) after the program was initiated, プログラム開始後の経過時間(ミリ秒)
      aveAin= sensorValue/AveN;                         // average of sensor read, センサー読み取り値の平均値
      Temp10=(long) (aveAin*1100/1024);                 //calculate Temp*10, Analog read reference =1100 mV and 10 bit,Temp10の計算。1100 mVを10 bitに分割している。
   //   dataString=String(timeMS,DEC);                    //make a data to send in string mode, string conversion cannot treat float numbers, 送信用の文字列データを作成
      dataString=String(tmYearToCalendar(tm.Year),DEC);
      dataString += ",";                                // add comma, コンマを加える
      dataString += String(tm.Month,DEC);
      dataString += ",";                                // add comma, コンマを加える
      dataString += String(tm.Day,DEC);
      dataString += ",";                                // add comma, コンマを加える
      dataString += String(tm.Hour,DEC);
      dataString += ",";                                // add comma, コンマを加える
      dataString += String(tm.Minute,DEC);
      dataString += ",";                                // add comma, コンマを加える
      dataString += String(tm.Second,DEC);
      dataString += ",";                                // add comma, コンマを加える
      dataString += String(Temp10,DEC);                 // add the Temp10 data, Temp10の数字を加える。 
      PrintToFile(dataString);                          // output data to the SD cardm through the subroutine, SDカードにデータ出力するサブルーチンを呼ぶ。
      //      lcdString=String(tmYearToCalendar(tm.Year),DEC);
      //      lcdString += "/";                                // add comma, コンマを加える
      //      lcdString += String(tm.Month,DEC);
      //      lcdString += "/";                                // add comma, コンマを加える

      //      lcdString += String(tm.Day,DEC);
      //      lcdString += "/";                                // add comma, コンマを加える
      lcd.setCursor(0,0);

      lcdString += String(tm.Hour,DEC);
      lcdString += ":";                                // add comma, コンマを加える
      lcdString += String(tm.Minute,DEC);
      lcdString += ":";                                // add comma, コンマを加える
      lcdString += String(tm.Second,DEC);
      lcdString += ",";                                // add comma, コンマを加える
      lcdString += String(Temp10,DEC);                 // add the Temp10 data, Temp10の数字を加える。 
      lcdString += " ";                                // add comma, コンマを加える
      //      lcdString += "\n";                                // add comma, コンマを加える
      //      lcd.setCursor(0, 1);
      lcd.clear();
      //      delay(100);
      lcd.println(lcdString);

  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    //    delay(9000);
  }
   
   delay(RecTime);                                      //delay 1 sec, 1秒待つ。 
}

// Subroutine for writing data in SD card, SDカードへのデータ書き込みのためのサブルーチン
void PrintToFile(String dataIn){
  File dataFile = SD.open("datalog.txt", FILE_WRITE);  // define the filename, ファイル名を定義。
  if (dataFile) {                                      //if the file in the SD card was open to wrihte, SDカードの対象ファイルを開くことができれば
    dataFile.println(dataIn);                          // write data into the file, データの記入 
    dataFile.close();                                  // close the file, ファイルを閉じる
    Serial.println(dataIn);                           // print to the serial port too,シリアルポートにも出力して確認。
  }else {                                              // if the file isn't open, pop up an error message, ファイルが開けないときのエラーメッセージ
    Serial.println("error opening file");
  } 
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
