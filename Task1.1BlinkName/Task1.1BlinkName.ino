#define ARRAYLEN 26

String alphabet[ARRAYLEN] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
String morse[ARRAYLEN] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Enter a name to blink as morse code: ");
  while(Serial.available()==0){
  }
  String name = Serial.readString();
  blinkMorse(name);
}

void blinkMorse(String name){
  name.toLowerCase();
  //Create an array containing the morse convertion of each letter of the name string at seperate indices.
  String morseName[name.length()];
  for(int i=0; i<name.length(); i++){
    for(int j=0; j<ARRAYLEN; j++){
      if(name.substring(i,i+1) == alphabet[j]){
        morseName[i] = morse[j];
      }
    }
  }
  //Blink the correct morse code based on previously created array.
  for(int i=0; i<name.length(); i++){
    for(int j=0; j<morseName[i].length(); j++){
      if(morseName[i].substring(j,j+1) == "."){
        dot();
      }
      else{
        dash();
      }
      delay(200);
    }
  }
  delay(600);
}

void dot(){
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
}

void dash(){
  digitalWrite(13, HIGH);
  delay(300);
  digitalWrite(13, LOW);
  delay(100);
}

