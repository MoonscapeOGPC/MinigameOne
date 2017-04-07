int lightPins[2][3] = {
  { 10, 9, 8 },
  { 7,  6, 5 }
};
int buttonPins[2] = {
  13, 12
};
enum state {
  WAITING,
  TIMING,
  ACTIVE
};

state currentState;
int currentButton;
int currentValue;
long currentDelay;
int currentTime;
int runGames;

void setLight(int light, int r, int g, int b){
  digitalWrite(lightPins[light][0], b);
  digitalWrite(lightPins[light][1], g);
  digitalWrite(lightPins[light][2], r);
}

void clearLight(int light){
  setLight(light, LOW, LOW, LOW);
}

void initLight(int light){
  pinMode(lightPins[light][0], OUTPUT);
  pinMode(lightPins[light][1], OUTPUT);
  pinMode(lightPins[light][2], OUTPUT);
  clearLight(light);
}

void setup() {
  for(int i = 0; i < 2; i++) initLight(i);
  for(int i = 0; i < 2; i++) pinMode(buttonPins[i], INPUT);
  randomSeed(analogRead(0));
  
  setLight(0, HIGH, HIGH, HIGH);
  setLight(1, HIGH, HIGH, HIGH);

  Serial.begin(9600);
  Serial.write('b');
  delay(1000);
  clearLight(0);
  clearLight(1);

  runGames = 0;
}

void startTiming(){
  clearLight(0);
  clearLight(1);
  currentDelay = 500 + random(1000);
  currentButton = random(2);
  currentValue = random(2);
  currentState = TIMING;
  for(int i = 0; i < 2; clearLight(i++));
}

void startActive(){
  setLight(currentButton, (currentValue) ? LOW : HIGH, (currentValue) ? HIGH : LOW, LOW);
  currentTime = 0;
  currentState = ACTIVE;
}

void succeed(){
  setLight(0, LOW, HIGH, LOW);
  setLight(1, LOW, HIGH, LOW);
  currentState = WAITING;
  Serial.write('s');
  if(runGames == 0) Serial.write('d');
  delay(250);
}

void fail(){
  setLight(0, HIGH, LOW, LOW);
  setLight(1, HIGH, LOW, LOW);
  currentState = WAITING;
  Serial.write('f');
  if(runGames == 0) Serial.write('d');
  delay(250);
}

void loop() {
  if(currentState == WAITING){
    setLight(0, LOW, LOW, HIGH);
    setLight(1, LOW, LOW, HIGH);

    if(runGames > 0) {
      runGames--;
      startTiming();
    }
    
  } else if(currentState == TIMING && currentDelay < 0){
    startActive();
  } else if(currentState == TIMING){
    currentDelay -= 25;
  } else if(currentState == ACTIVE){

    if(currentTime > 700){
      fail();
    } else {
      currentTime += 25;
      bool anyPressed = false;
      bool succeeded = true;
      for(int i = 0; i < 2; i++) anyPressed |= digitalRead(buttonPins[i]) == HIGH;
      succeeded = digitalRead(buttonPins[currentButton]) == HIGH && digitalRead(buttonPins[1 - currentButton]) == LOW;
      succeeded ^= (1 - currentValue);
      if(anyPressed) {
        if(succeeded) succeed();
        else fail();
      }
    }
    
  }

  while(Serial.available()){
    int newByte = Serial.read();
    if(newByte == 'r') {
      Serial.write('o');
      currentState = WAITING;
      runGames = 0;
    } else {
      if(runGames == 0) {
        for(int i = 0; i < 4; i++){
          setLight(0, HIGH, HIGH, HIGH);
          clearLight(1);
          delay(500);
          setLight(1, HIGH, HIGH, HIGH);
          clearLight(0);
          delay(500);
        }
        clearLight(0);
        clearLight(1);
      }
      
      runGames += newByte;
    }
  }
  delay(25);
}
