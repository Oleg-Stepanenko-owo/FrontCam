#define HONDA_DIS_ON      analogWrite( A0, 0 ); analogWrite( A1, 0 ); analogWrite( A2, 0 ); digitalWrite(13, LOW);
#define HONDA_DIS_OFF     analogWrite( A0, 255 ); analogWrite( A1, 255 ); analogWrite( A2, 255 ); digitalWrite(13, HIGH);
#define BACKTIME  11000


static const int pkgLong = 4;
static const int pkgSize = 21;
static int valInc = 0;

const int longpushtime =  1400;
const int maxWaitNextAction = 600;

const int pkgData[pkgSize][pkgLong] = {
  { 255,   0,   0,   0 }, // 0 Scroll LEFT
  { 129,   0,   0, 126 }, // 1 Scroll RIGHT
  { 128,  64,   0,  63 }, // 2 Push
  { 128,   0,   4, 123 }, // 3 to Right
  { 128,   0,   1, 126 }, // 4 to Left
  { 128,   0,   8, 119 }, // 5 to Up
  { 128,   0,   2, 125 }, // 6 to Donw
  { 128,   0,  12, 115 }, // 7 to Right + Up
  { 128,   0,   6, 121 }, // 8 to Right + Down
  { 128,   0,   9, 118 }, // 9 to Left + Up
  { 128,   0,   3, 124 }, // 10 to Left + Down
  { 128,   0,   0, 127 }, // 11 after JOG mouse,
  { 128,   1,   0, 126 }, // 12 Map/guide
  { 128,   0,  64,  63 }, // 13 Audio
  { 128,   0,  32,  95 }, // 14 Cancel
  { 128,  32,   0,  95 }, // 15 Menu
  { 128,   4,   0, 123 }, // 16 A/C
  { 128,  16,   0, 111 }, // 17 Info
  { 128,   8,   0, 119 }, // 18 Setup
  { 128,   2,   0, 125 }, // 19 Light
  {  -1,  -1,   0,   0 }  // 20 Long Push
};

enum eActions {
  SCROLL_LEFT = 0,
  SCROLL_RIGHT,
  PUSH,
  TO_RIGHT,
  TO_LEFT,
  TO_UP,
  TO_DOWN,
  TO_RIGHT_UP,
  TO_RIGHT_DOWN,
  TO_LEFT_UP,
  TO_LEFT_DOWN, //10
  AFTER_MOUSE_DOWN,
  B_MAP,
  B_AUDIO,
  B_CANCEL,
  B_MENU,
  B_AC,
  B_INFO,
  B_SETUP,
  B_LIGHT,
  LONG_PUSH
};

//--------------------------------------------------------------------------
int pkgVal[pkgLong];
int lastAction;

unsigned long wTime;
unsigned long wIsPush;
unsigned long wIsLongPush;
unsigned long wBackTime;

bool isMainDisplay;

//---------------------------------------------------------------------------
void setup()
//---------------------------------------------------------------------------
{
  // put your setup code here, to run once:
  pinMode( 13, OUTPUT );
  pinMode( A0, OUTPUT );
  pinMode( A1, OUTPUT );
  pinMode( A2, OUTPUT );

  HONDA_DIS_ON;

  isMainDisplay = true;

  Serial.begin(4800);  //JOG
  Serial.println("Start front cam driver");

  lastAction = -1;
  wTime = 0;
  wIsPush = 0;
  wIsLongPush = 0;
  wBackTime = 0;
}

//---------------------------------------------------------------------------
int getAction()
//---------------------------------------------------------------------------
{
  for ( int a = 0; a < pkgSize; a++ ) {
    if ( pkgData[a][0] == pkgVal[0] && pkgData[a][1] == pkgVal[1]
         && pkgData[a][2] == pkgVal[2] && pkgData[a][3] == pkgVal[3] )
    {
      //      Serial.print("pkgVal = "); Serial.print(pkgVal[0]);
      //      Serial.print(" "); Serial.println(pkgVal[1]);
      switch ( a ) {
        case SCROLL_LEFT :
          Serial.println("SL!");
          return SCROLL_LEFT;
        case SCROLL_RIGHT :
          Serial.println("SR!");
          return SCROLL_RIGHT;
        case PUSH :
          return PUSH;
        case TO_RIGHT :
          Serial.println("JR!");
          return TO_RIGHT;
        case TO_LEFT :
          Serial.println("JL!");
          return TO_LEFT;
        case TO_UP :
          Serial.println("JU!");
          return TO_UP;
        case TO_DOWN :
          Serial.println("JD!");
          return TO_DOWN;
        case TO_RIGHT_UP :
          return TO_RIGHT_UP;
        case TO_RIGHT_DOWN :
          return TO_RIGHT_DOWN;
        case TO_LEFT_UP :
          return TO_LEFT_UP;
        case TO_LEFT_DOWN :
          return TO_LEFT_DOWN;
        case AFTER_MOUSE_DOWN :
          return AFTER_MOUSE_DOWN;
        case  B_MAP:
          Serial.println("MAP!");
          return B_MAP;
        case B_AUDIO :
          Serial.println("AUDIO!");
          return B_AUDIO;
        case B_CANCEL :
          Serial.println("CANCEL!");
          return B_CANCEL;
        case B_MENU :
          Serial.println("MENU!");
          return B_MENU;
        case  B_AC :
          Serial.println("AC!");
          return B_AC;
        case B_INFO :
          Serial.println("INFO!");
          return B_INFO;
        case B_SETUP :
          Serial.println("SETUP!");
          return B_SETUP;
        case B_LIGHT :
          Serial.println("LIGHT!");
          return B_LIGHT;
        default:
          return -1;
      }
    }
  }
}

//---------------------------------------------------------------------------
void loop()
//---------------------------------------------------------------------------
{
  if ( wBackTime && ( wBackTime < millis() ) ) {
    wBackTime = 0;
    isMainDisplay = true;
    HONDA_DIS_ON;
  }

  if ( Serial.available() ) {
    pkgVal[valInc++] = Serial.read();
    // Serial.println(pkgVal[valInc - 1]);

    if (  valInc == 1 && pkgVal[0] != 128 && pkgVal[0] != 129 && pkgVal[0] != 255  ) {
      valInc = 0;
      Serial.println("JE1");
      return;
    }
  }

  if ( (0 != wIsPush) && (wIsPush < millis()) )
  {
    // PUSH
    wIsPush = 0;
    wIsLongPush = 0;
    lastAction = 0;
    Serial.println("Push");

    if ( wBackTime ) {
      wBackTime = 0;
      Serial.println("cam infinity");
    }

  }
  else if ( ( 0 != wIsLongPush) && ( wIsLongPush < millis()) )
  {
    // Long PUSH
    wIsPush = 0;
    wIsLongPush = 0;
    Serial.println("LongPush");

    isMainDisplay = !isMainDisplay;
    //------------------- comutate ----------------
    if ( isMainDisplay ) {
      HONDA_DIS_ON;
      wBackTime = 0;
    } else {
      HONDA_DIS_OFF;
      wBackTime = (millis() + BACKTIME);
      Serial.println("cam by timer");
    }
    //---------------------------------------------

    lastAction = LONG_PUSH;
    wTime = (millis() + maxWaitNextAction );
  }

  if ( valInc == pkgLong ) {
    int iAction = getAction();
    if ( -1 != iAction ) valInc = 0;

    // Serial.print("LA:"); Serial.println(lastAction);
    if ( (lastAction == iAction) && wTime && (wTime > millis()) ) {
      valInc = 0;
      // Serial.println("wTime");
      return;
    }
    wTime = 0;

    if ( PUSH == iAction ) {
      if ( lastAction == PUSH ) {
        wIsPush = millis() + maxWaitNextAction;
      }
      else {
        // Serial.println("StartPushAct");
        wIsPush = millis() + maxWaitNextAction;
        wIsLongPush = millis() + longpushtime;
        lastAction = PUSH;
      }
      return;
    }
  }
}

