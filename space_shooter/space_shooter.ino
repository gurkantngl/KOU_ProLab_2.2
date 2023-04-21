#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SSD1306_WIDTH 128
#define SSD1306_LCDHEIGHT 64
#define OLED_RESET 7
#define LATCH 13
#define CLK 12
#define DATA 11

Adafruit_SSD1306 display(SSD1306_WIDTH, SSD1306_LCDHEIGHT, &Wire, OLED_RESET);

const uint8_t MAX_METEORITES = 1;
const uint8_t MAX_SpaceJunk = 1;
const uint8_t MAX_BULLETS = 3;
const uint8_t MAX_PRIZE = 2;
const uint8_t MAX_IMMUNITY = 1;

byte digit[10]= {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x6F};

// Meteor
struct Meteorites {
  int8_t x, y, vx, visible, count=0;
};

// Uzay Çöpü
struct SpaceJunk {
  int8_t x, y, vx, visible;
};

// Silah
struct Bullet {
  int8_t x, y;
  bool fired = false;
};

// ödül Puanı
struct Prize {
  int8_t x, y, vx, visible;
};

// dokunulmazlık
struct Immunity{
  int8_t x, y, vx, visible;
};


SpaceJunk spacejunk[MAX_SpaceJunk*5];
Meteorites meteo[MAX_METEORITES*5];
Bullet fire[MAX_BULLETS];
Prize prize[MAX_PRIZE*1];
Immunity immunity[MAX_IMMUNITY*1];


// Global variables
uint8_t color = WHITE;
uint8_t color2 = BLACK;
int bullet_button = 5;
int buttonState = 0;
int ship_margin = 10;
int amor = 3;
int shot = 3;
int score = 0;
int delayTime = 500;
int time = 0;
int count1 = 0;
int count2 = 0;
int counterS = 0;
int seviye = 0;
int s1 = 0;
int s2 = 0;
int b1 = 0;
int b2 = 0;
// ----------------------------------------------------------
// Init Functions
// ----------------------------------------------------------
void init_display() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  
  display.display();
  delay(10);
  display.clearDisplay();
  display.fillRect(0, 0, 128, 64, color2);
}

// potansiyometre
void init_pot() {
  int pot = A0;
  int potValue = 0;
  pinMode(pot, INPUT);
}

// butonlar
void init_bullet_button(){
  pinMode(bullet_button, INPUT);
}

// ----------------------------------------------------------
// Display Functions
// ----------------------------------------------------------
void display_ship(int pos){
  static const unsigned char PROGMEM spaceShip_img[] = {
  0x78, 0x00, 0x7c, 0x00, 0x30, 0x00, 0x3f, 0x80, 0xfc, 0xc0, 0xfc, 
  0xc0, 0x3f, 0x80, 0x30, 0x00,0x7c, 0x00, 0x78, 0x00 };
  // Uzay aracı boyutları
  display.drawBitmap(ship_margin, pos,spaceShip_img, 10, 10, color);
}
// Uzay aracının potansiyometreye göre hareketi
int shipPosition(){
  int potiValue = analogRead(A3);
  return map(potiValue, 40, 1024, 0, display.height()-10); 
}

// ateş etme
bool fire_bullet() {
  buttonState = digitalRead(bullet_button);
  Serial.print("buttonState: ");
  Serial.println(buttonState);
  if (buttonState == 1) {
    if (shot > 0){
      shot = shot-1;
      Serial.print("Ates");
      Serial.println(shot);
      return true;
    }
  }
  else if (buttonState == 0){
    return false;
  }
}

void bullet(bool fire) {
  if( fire == true ){
    trigger_bullet(shipPosition());
  } 
}





void collition_check(int shipPos) {
  // METEOR ÇARPMASI
  for (uint8_t i = 0; i < MAX_METEORITES*5; i++) { 
    if( meteo[i].x < ship_margin + 8 &&  meteo[i].x >= ship_margin ) {
      // in range of x-axses 
      if( meteo[i].y < shipPos + 8 &&  meteo[i].y >= shipPos ) {
        // in range of y-axses
        if ((millis() + (count2 * delayTime) - time) > 3000){
          if(amor > 0 ){
            digitalWrite(6, HIGH);
            amor --;          
          }else {
            amor = 0;
          }
          time = millis() + (count1 * delayTime);
          count1 = 0;
          meteo[i].visible = false;
        }
      }
    }
  }
  




  // UZAY ÇÖPÜ ÇARPMASI
  for (uint8_t i = 0; i < MAX_SpaceJunk*5; i++) { 
    if( spacejunk[i].x < ship_margin + 8 &&  spacejunk[i].x >= ship_margin ) {
      // in range of x-axses 
      if( spacejunk[i].y < shipPos + 8 &&  spacejunk[i].y >= shipPos ) {
        // in range of y-axses
        if((millis() + (count2 * delayTime) - time) > 3000){
          if(amor > 0 ){
            digitalWrite(6, HIGH);
            amor --;          
          }else {
            amor = 0;
          }
          time = millis() + (count1 * delayTime);
          count1 = 0;
          spacejunk[i].visible = false;
        }
      }
    }
  }




  // Ödül Puanı Alma
  for (uint8_t i = 0; i < MAX_PRIZE*5; i++) { 
    if( prize[i].x < ship_margin + 8 &&  prize[i].x >= ship_margin ) {
      // in range of x-axses 
      if( prize[i].y < shipPos + 8 &&  prize[i].y >= shipPos ) {
        // in range of y-axses
        if(amor < 3){
          prize[i].visible = false;
          amor ++;
          shot++;     
        }
      }
    }
  }



  
  // UZAY ÇÖPÜ ATEŞ
  for (uint8_t i_bu = 0; i_bu < MAX_BULLETS; i_bu++) {
    for (uint8_t i_me = 0; i_me < MAX_SpaceJunk*5; i_me++) { 
      if (fire[i_bu].x >= spacejunk[i_me].x && fire[i_bu].x < spacejunk[i_me].x + 8 && fire[i_bu].fired == true) {
        if (fire[i_bu].y >= spacejunk[i_me].y && fire[i_bu].y < spacejunk[i_me].y + 8 ) {
          spacejunk[i_me].visible = false;
          fire[i_bu].fired = false;
        }
      }
    }
  }



  // METEOR ATEŞ
  for (uint8_t i_bu = 0; i_bu < MAX_BULLETS; i_bu++) {
    for (uint8_t i_me = 0; i_me < MAX_METEORITES*5; i_me++) { 
      if (fire[i_bu].x >= meteo[i_me].x && fire[i_bu].x < meteo[i_me].x + 12 && fire[i_bu].fired == true) {
        if (fire[i_bu].y >= meteo[i_me].y && fire[i_bu].y < meteo[i_me].y + 12 ) {
          fire[i_bu].fired = false;
          meteo[i_me].count++;
          Serial.println(meteo[i_me].count);
          if(meteo[i_me].count >= 2){
            meteo[i_me].visible = false;
          }
        }
      }
    }
  }



    // Dokunulmazlık Alma
  for (uint8_t i = 0; i < MAX_IMMUNITY*1; i++) { 
    if(immunity[i].x < ship_margin + 8 &&  immunity[i].x >= ship_margin ) {
      // in range of x-axses 
      if(immunity[i].y < shipPos + 8 &&  immunity[i].y >= shipPos ) {
        // in range of y-axses
          immunity[i].visible = false;
          time = millis() + (count1*delayTime);
          count1 = 0; 
        }
      }
    }
}



void display_score(){
      digitalWrite(LATCH, LOW);
      shiftOut(DATA, CLK, MSBFIRST, ~digit[score%10]);  
      shiftOut(DATA, CLK, MSBFIRST, ~digit[(score/10)%10]); 
      shiftOut(DATA, CLK, MSBFIRST, ~digit[score/100]); 
      digitalWrite(LATCH, HIGH);

}




//METEOR
void init_meteo() {
  int x = 60;
  for (uint8_t i = 0; i < MAX_METEORITES*5; i++) {
    meteo[i].x = x;
    meteo[i].y = random(display.height()-8);;
    if(i%MAX_METEORITES==0){
      x += 32;
    }
    //meteo[i].y = random(display.height()-8);
    meteo[i].visible = true;
  }
}



void draw_meteo() {
  for (uint8_t i = 0; i < MAX_METEORITES*5; i++) {
    static const unsigned char PROGMEM meteo_img[] = {
      0x00,0x00,
      0x3F,0x00,
      0x7F,0x80,
      0xE7,0x80,
      0xE7,0x80,
      0xFF,0x80,
      0x7F,0x80,
      0x7F,0x80, 
      0x3F,0x00, 
      0x3E,0x00 
    };
    if(meteo[i].visible == true ){
      display.drawBitmap(meteo[i].x, meteo[i].y, meteo_img, 10, 10, color);
    }
  }
}

void move_meteo() {
  for (uint8_t i = 0; i < MAX_METEORITES*5; i++) {
    meteo[i].x -= 8;
    if (meteo[i].x < 0) {
      meteo[i].x = 120;
      meteo[i].y = random(display.height());;
      //meteo[i].y = random(display.height());
      meteo[i].visible = true;
    }
  }
}



//
// UZAY ÇÖPÜ
//

void init_spaceJunk() {
  int x = 40;
  for (uint8_t i = 0; i < MAX_SpaceJunk*5; i++) {
    spacejunk[i].x = x;
    spacejunk[i].y = random(display.height()-8);
    if(i%MAX_SpaceJunk==0){
      x += 32;
    }
    spacejunk[i].visible = true;
  }
}



void draw_spaceJunk() {
  for (uint8_t i = 0; i < MAX_SpaceJunk*5; i++) {
    if(spacejunk[i].visible == true ){
      display.drawRect(spacejunk[i].x, spacejunk[i].y, 8, 8, color);
    }
    }
}



void move_spaceJunk() {
  for (uint8_t i = 0; i < MAX_SpaceJunk*5; i++) {
    spacejunk[i].x -= 8;
    if (spacejunk[i].x < 0) {
      spacejunk[i].x = 120;
      spacejunk[i].y = random(display.height());;
      spacejunk[i].visible = true;
    }
  }
}




//
// Kurşun
//

void init_bullet() {
  for (uint8_t i = 0; i < MAX_BULLETS; i++) {
    fire[i].x = 20;
    fire[i].y = 0;
    fire[i].fired = false;
  }    
}

void move_bullet() {   
  for (uint8_t i = 0; i < MAX_BULLETS; i++) {
    if(fire[i].fired == true){
      fire[i].x += 8; 
    }
    if (fire[i].x < 0) {
      fire[i].x = 20;
      fire[i].fired = false;
    }
  }
}

void trigger_bullet(int shipPos) {
  for (uint8_t i = 0; i < MAX_BULLETS; i++) {
    if(fire[i].fired == false){
        fire[i].y = shipPos + 5;
        fire[i].fired = true;
        break;
    }
  }
}


void draw_bullet() {
  for (uint8_t i = 0; i < MAX_BULLETS; i++) {
    if(fire[i].fired == true){
      display.drawPixel(fire[i].x, fire[i].y, color); 
    }
  }    
}


//
//  ÖDÜL PUANI
//

void init_prize() {
  int x = 80;
  for (uint8_t i = 0; i < MAX_PRIZE*1; i++) {
    meteo[i].x = x;
    meteo[i].y = random(display.height()-8);
    meteo[i].visible = true;
    if(i%MAX_PRIZE==0){
      x += 16;
    }
  }
}


void draw_prize(){
  for (uint8_t i = 0; i < MAX_PRIZE*1; i++) {
    static const unsigned char PROGMEM armor_img[] = {
      0xFC, // ######..
      0xFC, // ######..
      0xFC, // ######..
      0xFC, // ######..
      0x78, // .####...
      0x30, // ..##....
    };
    if(prize[i].visible == true ){
      display.drawBitmap(prize[i].x, prize[i].y, armor_img, 8, 8, color);
    }
  }

}
void move_prize() {
  for (uint8_t i = 0; i < MAX_PRIZE*1; i++) {
    prize[i].x -= 8;
    if (prize[i].x < 0) {
      prize[i].x = 120;
      prize[i].y = random(display.height());
      prize[i].visible = true;
    }
  }
}




//
// DOKUNULMAZLIK
//

void init_immunity() {
  int x = 40;
  for (uint8_t i = 0; i < MAX_IMMUNITY*1; i++) {
    immunity[i].x = x;
    immunity[i].y = random(display.height()-8);
    immunity[i].visible = true;
    if(i%MAX_IMMUNITY==0){
      x += 16;
    }
  }
}



void draw_immunity() {
  for (uint8_t i = 0; i < MAX_IMMUNITY*1; i++) {
    if(immunity[i].visible == true ){
      display.drawTriangle(immunity[i].x, immunity[i].y, immunity[i].x+8, immunity[i].y, immunity[i].x, immunity[i].y+8, color);
    }
  }
}



void move_immunity() {
  for (uint8_t i = 0; i < MAX_IMMUNITY*1; i++) {
    immunity[i].x -= 8;
    if (immunity[i].x < 0) {
      immunity[i].x = 120;
      immunity[i].y = random(display.height());
      immunity[i].visible = true;
    }
  }
}











// ----------------------------------------------------------
// SETUP
// ----------------------------------------------------------
void setup() {
  pinMode(LATCH, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(6, OUTPUT);
  //pinMode(A1, INPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(4, INPUT);
  pinMode(A2, INPUT);
  init_display();
  init_pot();
  
  init_spaceJunk();
  init_meteo();
  
  init_bullet_button();
  init_bullet();
  init_immunity();
  init_prize();
}



// ----------------------------------------------------------
// LOOP 
// ----------------------------------------------------------
void loop() {

  while(seviye == 0){
      display.clearDisplay();
      display.fillRect(0, 0, 128, 64, color2);
      display.setTextSize(1);
      display.setTextColor(color);
      display.setRotation(1);
      display.setCursor(20, 64);
      display.println("Seviye");
      display.setCursor(20, 74);
      display.println("Secin");
      display.display();


      b1 = digitalRead(5);
      if (b1 == 1){
        seviye = 1; 
        display.clearDisplay();
        display.setRotation(0);
      }
      b2 = digitalRead(4);
      if (b2 == 1){
        seviye = 2;
        display.clearDisplay();
        display.setRotation(0);
      } 
  }






  // yıldızları çiz
  draw_spaceJunk();
  // kurşunu çiz
  draw_bullet();
  //meteoru çiz
  draw_meteo();
  //ödül puanı
  draw_prize();
  //dokunulmazlık
  draw_immunity();
  
  // yıldızları hareket ettir
  move_spaceJunk();

  
  
  // uzay gemisi
  display_ship(shipPosition());
  
  // kurşun
  bullet(fire_bullet());

  
  // kurşunu hareket ettir
  move_bullet();

  
  // meteoru hareket ettir
  move_meteo();

  
  move_prize();

  
  move_immunity();
  
  // çarpışma kontrolü
  collition_check(shipPosition());
  

  // skor
  display_score();
  
  if (amor >= 3){
    digitalWrite(10, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(8, HIGH);
  }
  
  if (amor == 2){
    digitalWrite(10, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(8, LOW);
  }

  if (amor == 1){
    digitalWrite(10, HIGH);
    digitalWrite(9, LOW);
    digitalWrite(8, LOW);
  }

  if (amor == 0){
    digitalWrite(10, LOW);
    digitalWrite(9, LOW);
    digitalWrite(8, LOW);
    lose();
  }  

  if (shot >= 3){
    digitalWrite(0, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(2, HIGH);
  }


  if(shot == 2){
    digitalWrite(0, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
  }

  if(shot == 1){
    digitalWrite(0, HIGH);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
  }

  if(shot == 0){
    digitalWrite(0, LOW);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
  }

  int sensorValue = analogRead(A0); // LDR Sensor

  if (sensorValue > 15){
    color = BLACK;
    color2 = WHITE;
  }else{
    color = WHITE;
    color2 = BLACK;
  }
  Serial.println(sensorValue);

  display.display();
  delay(delayTime);
  if(seviye == 2 && (millis()+count2*delayTime)%10000<= 1000){
    delayTime *= 0.8;
  }
  count1++;
  count2++;
  score ++;
  if(score%45 == 0){
    if(shot<3){
      shot++;
    }
  }
  if(score%30 == 0){
    if(shot<3){
      shot++;
    }
    if(amor<3){
      amor++;
    }
  }  
  digitalWrite(6, LOW);
  display.clearDisplay();
  display.fillRect(0, 0, 128, 64, color2);
}


void lose(){
  display.clearDisplay();
  display.fillRect(0, 0, 128, 64, color2);
  display.setTextSize(2);
  display.setTextColor(color);
  display.setRotation(1);
  digitalWrite(6, LOW);
  display.setCursor(4, 64);
  display.println("LOSE!");  
  display.display();
  for(;;);
}







