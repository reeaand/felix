#define stg1 5
#define stg2 2
#define stgViteza 10

#define dr1 3
#define dr2 8
#define drViteza 11

#define ir0 A0
#define ir1 A1
#define ir2 A2
#define ir3 A3
#define ir4 A4

String senzori;
int linie = 0;
int tabla[4][4];
int liniiOcupate[4];
int jucator = 0;
int tura = 0;
int gataPiesa = 1;
int aux = 0;


void fata(int t) {
  digitalWrite(stg1, HIGH);
  digitalWrite(stg2, LOW);

  digitalWrite(dr1, HIGH);
  digitalWrite(dr2, LOW);
  
  delay(t);
}

void spate(int t) {
  digitalWrite(stg2, HIGH);
  digitalWrite(stg1, LOW);

  digitalWrite(dr2, HIGH);
  digitalWrite(dr1, LOW);
  
  delay(t);
}

void stanga(int t) {
  digitalWrite(stg1, HIGH);
  digitalWrite(stg2, LOW);

  digitalWrite(dr1, LOW);
  digitalWrite(dr2, HIGH);
    
  delay(t);
}

void dreapta(int t) {
  digitalWrite(stg1, LOW);
  digitalWrite(stg2, HIGH);

  digitalWrite(dr1, HIGH);
  digitalWrite(dr2, LOW);
  
  delay(t);
}

void stopp() {
  digitalWrite(stg1, LOW);
  digitalWrite(stg2, LOW);

  digitalWrite(dr1, LOW);
  digitalWrite(dr2, LOW); 
}

void setareViteza(int stg, int dr) {
  analogWrite(stgViteza, stg);
  analogWrite(drViteza, dr);
}

String citire_ir() {
  char irs[5];
  irs[0] = digitalRead(ir0) + '0';
  irs[1] = digitalRead(ir1) + '0';
  irs[2] = digitalRead(ir2) + '0';
  irs[3] = digitalRead(ir3) + '0';
  irs[4] =  digitalRead(ir4) + '0';

  String rezultat;
  for(int i = 0; i < 5; i++) {
    rezultat += irs[i];
  }
  return rezultat;
}

void impingePeTabla() {
  int deImpins = 4 - liniiOcupate[linie - 1];
  liniiOcupate[linie]++;

  // impinge piesa pe tabla
  for (int i = 0; i < deImpins; i++) {
    fata(75);
    //delay(200);
    stopp();
    senzori = citire_ir(); 
    while (senzori != "11111") {
      fata(20);
      stopp();
      senzori = citire_ir(); 
    }
  }

  // masina iese de pe tabla 
  for (int i = 0; i < deImpins; i++) {
    spate(75);
    //delay(200);
    stopp();
    senzori = citire_ir(); 
    while (senzori != "11111") {
      spate(20);
      stopp();
      senzori = citire_ir(); 
    }
  }
  spate(100);
  stopp();
}

// functie pentru intoarcerea pe linia principala dupa ce piesa a fost pozitionata in fata tablei de joc
void intoarcereAcasa() {
  spate(100);
  stopp();
  if (linie < 3){
    stanga(900);
    stopp();
  } else {
    dreapta(900);
    stopp();
  }
  senzori = "00000";
  while(senzori == "00000") {
      spate(100);
      stopp();
      delay(100);
      senzori = citire_ir();
      Serial.println(senzori);
  } 
  fata(300);
  stopp();
  if (linie >= 3){
    stanga(900);
    stopp();
  } else {
    dreapta(900);
    stopp();
  }
  spate(100);
  stopp();
}

// functie de line following; functioneaza pana la o banda neagra
// sau daca a fost intrerupta prin bluetooth
void urmarire(int directie, int sens) {
  int ok = 0;
  while(1) {
    senzori = citire_ir();
    Serial.println(senzori);
    if (senzori == "00100") {
      if (sens == 0) {
        fata(70);
      } else {
        spate(70);
      }
      stopp();
    } else
    if ((senzori[3] == '1' or senzori[4] == '1') 
        and (senzori[0] == '0' and senzori[1] == '0')){
      if (sens == 1) {
        stanga(80);
      } else {
        dreapta(80);
      }
      stopp();
    } else
    if ((senzori[0] == '1' or senzori[1] == '1') 
        and (senzori[3] == '0' and senzori[4] == '0')){
      if (sens == 0) {
        stanga(80);
      } else {
        dreapta(80);
      }
    } else
    if ((senzori == "00000" or senzori == "01110")) {
      if (directie == 0) {
        stanga(80);
      } else {
        dreapta(80);
      }
      stopp();
      ok = 1;
    } else 
    if (senzori == "01111" or senzori == "10111") {
      stanga(55);
      stopp();
    } else
    if (senzori == "11110" or senzori == "11101") {
      dreapta(55);
      stopp();
    } else 
    if (senzori == "11111")
    {break;}
  if (Serial.available() > 2) {
    return;
  }
  }
}

// impinge piesa pe linia care trebuie
void selecteazaLinia() {
  int x;
  if (linie == 1 or linie == 4) {
    x = 2;
  } else {
    x = 1;
  }

  for (int i = 0; i < x; i++) {
      fata(75);
      stopp();
      senzori = citire_ir(); 
      while (senzori != "11111") {
       fata(20);
       stopp();
       senzori = citire_ir(); 
      }
   }
}

// calculeaza punctajul total al fiecarui jucator si afiseaza castigatorul
void calculeaza_castigatorul() {
  int rez[2] = {0, 0};
  for (int i = 0; i < 4; i++) {
      if (tabla[i][0] == tabla[i][1] and tabla[i][2] == tabla[i][1]) {
        rez[tabla[i][0] - 1]++;
      }
      if (tabla[i][3] == tabla[i][1] and tabla[i][2] == tabla[i][1]) {
        rez[tabla[i][3] - 1]++;
      }
      if (tabla[0][i] == tabla[1][i] and tabla[2][i] == tabla[1][i]) {
        rez[tabla[0][i] - 1]++;
      }
      if (tabla[3][i] == tabla[1][i] and tabla[2][i] == tabla[1][i]) {
        rez[tabla[3][i] - 1]++;
      }
  }
  if (tabla[0][0] == tabla[1][1] and tabla[1][1] == tabla[2][2]) {
    rez[tabla[0][0] - 1]++;
  }
  if (tabla[3][3] == tabla[1][1] and tabla[1][1] == tabla[2][2]) {
    rez[tabla[3][3] - 1]++;
  }
  if (tabla[0][1] == tabla[1][2] and tabla[1][2] == tabla[2][3]) {
    rez[tabla[0][1] - 1]++;
  }
  if (tabla[1][0] == tabla[2][1] and tabla[2][1] == tabla[3][2]) {
    rez[tabla[1][0] - 1]++;
  }
  
  if (tabla[0][3] == tabla[1][2] and tabla[2][1] == tabla[1][2]) {
    rez[tabla[0][3] - 1]++;
  }
  if (tabla[3][0] == tabla[1][2] and tabla[2][1] == tabla[1][2]) {
    rez[tabla[3][0] - 1]++;
  }
  if (tabla[0][2] == tabla[1][1] and tabla[1][1] == tabla[2][0]) {
    rez[tabla[0][2] - 1]++;
  }
  if (tabla[1][3] == tabla[2][2] and tabla[2][2] == tabla[3][1]) {
    rez[tabla[1][3] - 1]++;
  }

  Serial.print("Alb: ");
  Serial.println(rez[0]);
  Serial.print("Roz: ");
  Serial.println(rez[1]);
  if (rez[0] < rez[1]) {
    Serial.println("A castigat roz!"); 
  } else {
    Serial.println("A castigat alb!");
  }
}

void setup() {
  pinMode(ir0, INPUT);
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir4, INPUT);
  
  pinMode(stg1, OUTPUT);
  pinMode(stg2, OUTPUT);
  pinMode(stgViteza, OUTPUT);

  pinMode(dr1, OUTPUT);
  pinMode(dr2, OUTPUT);
  pinMode(drViteza, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  senzori = citire_ir();

  setareViteza(150, 150);

  if (Serial.available()) {
    aux = (aux + 1) % 3;
    char directie = Serial.read();
    if (directie >= '1' and directie <= '4' and gataPiesa == 1) {
      if (jucator == 0) {
        Serial.println("Alege o piesa alba!");
      } else {
        Serial.println("Alege o piesa roz!");
      }
      linie = directie - '0'; 
      tabla[liniiOcupate[linie]][linie] = jucator + 1;
      jucator ^= 1;
      gataPiesa = 0;
      tura++;
    }

    if (aux == 1) {
      Serial.println(directie);
      jucator ^= 1;
      if (directie > '4' or directie < '0') { 
      switch (directie)
      {
      case 'a' :
        stanga(130);
        break;
      case 's' :
        spate(150);
        break;
      case 'd' :
        dreapta(130);
        break;
      case 'w' : 
        fata(150);
        break;
      case 't' :
        impingePeTabla();
        Serial.println("Piesa a fost mutata in locul ales de jucator!");
        gataPiesa = 1;
        if (tura == 16) {
          calculeaza_castigatorul();
        }
        break;
      case 'i' :
        intoarcereAcasa();
        Serial.println("Felix s-a intors pe linie!");
        break;
      case 'u' :
        urmarire(1 - (linie-1) / 2, 0);
        break;
      default:
        Serial.println("Nu e o instructiune sau o linie valida!");
      }
      stopp();
      }
    }
  }
}
