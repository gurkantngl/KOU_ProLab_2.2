#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SSD1306_WIDTH 128
#define SSD1306_LCDHEIGHT 64
#define OLED_RESET 8
Adafruit_SSD1306 display(SSD1306_WIDTH, SSD1306_LCDHEIGHT, &Wire, OLED_RESET);

const uint8_t MAX_METEORITES = 5;
const uint8_t MAX_SpaceJunk = 5;
const uint8_t MAX_BULLETS = 5;
const uint8_t MAX_PRIZE = 5;
const uint8_t MAX_IMMUNITY = 5;

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


SpaceJunk spacejunk[MAX_SpaceJunk];
Meteorites meteo[MAX_METEORITES];
Bullet fire[MAX_BULLETS];
Prize prize[MAX_PRIZE];
Immunity immunity[MAX_IMMUNITY];


// Global variables
uint8_t color = WHITE;
int bullet_button = 2;
int buttonState = 0;
int ship_margin = 10;
int amor = 3;
int shot = 3;
int score = 0;
int delayTime = 100;
int LED = 12;

// ----------------------------------------------------------
// Init Functions
// ----------------------------------------------------------
void init_display() {
  Serial.begin(9600);
  // initialize with the I2C addr 0x3D (for the 128x64)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  
  display.display();
  delay(10);
  display.clearDisplay();
  //display.fillRect(0, 0, 128, 64, SSD1306_WHITE);
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
  pinMode(LED, OUTPUT);
}

// ----------------------------------------------------------
// Display Functions
// ----------------------------------------------------------
void display_ship(int pos){
  static const unsigned char PROGMEM spaceShip_img[] = {
  0x78, 0x00, 0x7c, 0x00, 0x30, 0x00, 0x3f, 0x80, 0xfc, 0xc0, 0xfc, 
  0xc0, 0x3f, 0x80, 0x30, 0x00,0x7c, 0x00, 0x78, 0x00 };
  // Uzay aracı boyutları
  display.drawBitmap(ship_margin, pos,spaceShip_img, 10, 10, 1);
}
// Uzay aracının potansiyometreye göre hareketi
int shipPosition(){
  int potiValue = analogRead(A0);
  return map(potiValue, 40, 1024, 0, display.height()-10); 
}

// ateş etme
bool fire_bullet() {
  buttonState = digitalRead(bullet_button);
  if (buttonState == 1) {
    if (shot > 0){
      shot --;
      Serial.print("Ates");
      Serial.println(shot);
      digitalWrite(LED, HIGH);
      return true;
    }
  }
  else if (buttonState == 0){
    digitalWrite(LED, LOW);
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
  for (uint8_t i = 0; i < MAX_METEORITES; i++) { 
    if( meteo[i].x < ship_margin + 8 &&  meteo[i].x >= ship_margin ) {
      // in range of x-axses 
      if( meteo[i].y < shipPos + 8 &&  meteo[i].y >= shipPos ) {
        // in range of y-axses
        if(amor > 0 ){
          amor --;          
        }else {
          amor = 0;
        }
        meteo[i].visible = false;
      }
    }
  }
  
  // UZAY ÇÖPÜ ÇARPMASI
  for (uint8_t i = 0; i < MAX_SpaceJunk; i++) { 
    if( spacejunk[i].x < ship_margin + 8 &&  spacejunk[i].x >= ship_margin ) {
      // in range of x-axses 
      if( spacejunk[i].y < shipPos + 8 &&  spacejunk[i].y >= shipPos ) {
        // in range of y-axses
        if(amor > 0 ){
          amor --;          
        }else {
          amor = 0;
        }
        spacejunk[i].visible = false;
      }
    }
  }


  // Ödül Puanı Alma
  for (uint8_t i = 0; i < MAX_PRIZE; i++) { 
    if( prize[i].x < ship_margin + 8 &&  prize[i].x >= ship_margin ) {
      // in range of x-axses 
      if( prize[i].y < shipPos + 8 &&  prize[i].y >= shipPos ) {
        // in range of y-axses
        if(amor < 3){
          prize[i].visible = false;
          amor ++;        
        }
      }
    }
  }



  
  // UZAY ÇÖPÜ ATEŞ
  for (uint8_t i_bu = 0; i_bu < MAX_BULLETS; i_bu++) {
    for (uint8_t i_me = 0; i_me < MAX_SpaceJunk; i_me++) { 
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
    for (uint8_t i_me = 0; i_me < MAX_METEORITES; i_me++) { 
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
  for (uint8_t i_bu = 0; i_bu < MAX_BULLETS; i_bu++) {
    for (uint8_t i_me = 0; i_me < MAX_IMMUNITY; i_me++) { 
      if (fire[i_bu].x >= immunity[i_me].x && fire[i_bu].x < immunity[i_me].x + 8 && fire[i_bu].fired == true) {
        if (fire[i_bu].y >= immunity[i_me].y && fire[i_bu].y < immunity[i_me].y + 8 ) {
          immunity[i_me].visible = false;
          fire[i_bu].fired = false;
        }
      }
    }
  }


}



void display_lifes(){
static const unsigned char PROGMEM armor_img[] = {
  0xFC, // ######..
  0xFC, // ######..
  0xFC, // ######..
  0xFC, // ######..
  0x78, // .####...
  0x30, // ..##....
};
  
  display.setTextColor(color);
  display.setTextSize(1);
  display.setCursor(display.width() - 20, 0);
  display.println(amor);
  display.drawBitmap(display.width() - 30, 0, armor_img , 6, 6, 1);
}

void display_score(){
  display.setTextColor(color);
  display.setTextSize(1);
  display.setCursor(10, 0);
  display.println(score);
  
}




//METEOR


void init_meteo() {
  for (uint8_t i = 0; i < MAX_METEORITES; i++) {
    meteo[i].x = 120;
    meteo[i].y = random(display.height()-8);
    meteo[i].visible = true;
  }
}



void draw_meteo() {
  for (uint8_t i = 0; i < MAX_METEORITES; i++) {
    static const unsigned char PROGMEM meteo_img[] = {
      0x00,0x00, // ................
      0x3F,0x00, // ..######........
      0x7F,0x80, // .########.......
      0xE7,0x80, // ###..####.......
      0xE7,0x80, // ###..####.......
      0xFF,0x80, // #########.......
      0x7F,0x80, // .########.......
      0x7F,0x80, // .########.......
      0x3F,0x00, // ..######........
      0x3E,0x00 // ..#####.........
    };
    if(meteo[i].visible == true ){
      display.drawBitmap(meteo[i].x, meteo[i].y, meteo_img, 10, 10, color);
    }
  }
}

void move_meteo() {
  for (uint8_t i = 0; i < MAX_METEORITES; i++) {
    meteo[i].x -= 8;
    if (meteo[i].x < 0) {
      meteo[i].x = 120;
      meteo[i].y = random(display.height());
      meteo[i].visible = true;
    }
  }
}



//
// UZAY ÇÖPÜ
//

void init_spaceJunk() {
  for (uint8_t i = 0; i < MAX_SpaceJunk; i++) {
    spacejunk[i].x = 120;
    spacejunk[i].y = random(display.height()-8);
    spacejunk[i].visible = true;
  }
}



void draw_spaceJunk() {
  for (uint8_t i = 0; i < MAX_SpaceJunk; i++) {
    if(spacejunk[i].visible == true ){
      display.drawRect(spacejunk[i].x, spacejunk[i].y, 8, 8, color);
    }
    }
}



void move_spaceJunk() {
  for (uint8_t i = 0; i < MAX_SpaceJunk; i++) {
    spacejunk[i].x -= 8;
    if (spacejunk[i].x < 0) {
      spacejunk[i].x = display.width();
      spacejunk[i].y = random(display.height());
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
  for (uint8_t i = 0; i < MAX_METEORITES; i++) {
    meteo[i].x = 120;
    meteo[i].y = random(display.height()-8);
    meteo[i].visible = true;
  }
}


void draw_prize(){
  for (uint8_t i = 0; i < MAX_PRIZE; i++) {
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
  for (uint8_t i = 0; i < MAX_PRIZE; i++) {
    prize[i].x -= 8;
    if (prize[i].x < 0) {
      prize[i].x = display.width();
      prize[i].y = random(display.height());
      prize[i].visible = true;
    }
  }
}




//
// DOKUNULMAZLIK
//

void init_immunity() {
  for (uint8_t i = 0; i < MAX_IMMUNITY; i++) {
    immunity[i].x = 120;
    immunity[i].y = random(display.height()-8);
    immunity[i].visible = true;
  }
}



void draw_immunity() {
  for (uint8_t i = 0; i < MAX_IMMUNITY; i++) {
    if(immunity[i].visible == true ){
      display.drawTriangle(immunity[i].x, immunity[i].y, immunity[i].x+8, immunity[i].y, immunity[i].x, immunity[i].y+8, color);
    }
    }
}



void move_immunity() {
  for (uint8_t i = 0; i < MAX_IMMUNITY; i++) {
    immunity[i].x -= 8;
    if (immunity[i].x < 0) {
      immunity[i].x = display.width();
      immunity[i].y = random(display.height());
      immunity[i].visible = true;
    }
  }
}











// ----------------------------------------------------------
// SETUP
// ----------------------------------------------------------
void setup() {
  init_display();
  init_pot();
  
  init_spaceJunk();
  init_meteo();
  
  init_bullet_button();
  init_bullet();

  init_prize();
  
}




// ----------------------------------------------------------
// LOOP 
// ----------------------------------------------------------
void loop() {
  // yıldızları hareket ettir
  move_spaceJunk();

  // yıldızları çiz
  draw_spaceJunk();
  
  // uzay gemisi
  display_ship(shipPosition());
  
  // kurşun
  bullet(fire_bullet());

  // kurşunu çiz
  draw_bullet();
  // kurşunu hareket ettir
  move_bullet();

  //meteoru çiz
  draw_meteo();
  // meteoru hareket ettir
  move_meteo();

  //ödül puanı
  draw_prize();
  move_prize();

  
  // çarpışma kontrolü
  collition_check(shipPosition());
  
  // can
  display_lifes();

  // skor
  display_score();

  if (amor == 0){
    lose();
  }  
  
  display.display();
  delay(delayTime);
  score ++;
  display.clearDisplay();
  //display.fillRect(0, 0, 128, 64, SSD1306_WHITE);
}


void lose(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(color);
  display.setRotation(1);
  display.setCursor(4, 64);
  display.println("LOSE!");  
  display.display();
  for(;;);
}







