// Ivan's Arduino LC meter

//Prealocate the compartor output (L tester)
    int aco=1<<ACO;

//predefine the multiplier weights (L Tester)
    const double pi = 3.1415926535;
    const double pipi = pi * pi;
    const double C = 1.1; //capacitor

//Predefine constants for 2pin cap test
    const int OUT_PIN = A2;
    const int IN_PIN = A0;

    const int MAX_ADC_VALUE = 1023;
    const float IN_STRAY_CAP_TO_GND = 24.48;
    const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND;
    const float R_PULLUP = 34.8;

    float ZERO = 0;//0.217; //zero capacitance, fix later, add autorgne

//Switchpin for changing modes
    const int SWITCH_PIN = A5;

void setup(){
  //Initialize comparator
    DDRD=0; PORTD=0;
    ACSR=0; ADCSRB=0;

  //Initialize the output pin
    pinMode(11, OUTPUT); //pulse pin for compartor

    pinMode(OUT_PIN, OUTPUT); //pins for 2 pin cap test
    pinMode(IN_PIN, OUTPUT);

    pinMode(SWITCH_PIN, INPUT);

  //Begin Serial output
    Serial.begin(115200);

  //Internal Capacitor Compenstation
    Serial.println(F("Internal Capacitance Calibration Success"));
    MeasureCap(); //calls to show old value, some sort of bug here if I do it in one function
    delay(100);
    ZeroCap();

    //Serial.println(ZERO);
}

//Pre Zero Capacitor Value
void ZeroCap(){
    //Define the Precicision of calibration and delay
    byte precision = 20;
    byte cooldown = 100;

    //For loop to iterate conditions
    for(byte i = 0; i<precision; i++){
        delay(cooldown);

        //Define the output pins for reading the capacitance
        pinMode(IN_PIN, INPUT); digitalWrite(OUT_PIN, HIGH);
        unsigned int val = analogRead(IN_PIN);
        digitalWrite(OUT_PIN, LOW); pinMode(IN_PIN, OUTPUT);

        ZERO += (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);
  }

  ZERO /= (float) precision;

 }

//Measure Capacitor Value and apply Zeroing
void MeasureCap(){

    //Define the output pins for reading the capacitance
        pinMode(IN_PIN, INPUT); digitalWrite(OUT_PIN, HIGH);
        unsigned int val = analogRead(IN_PIN);
        digitalWrite(OUT_PIN, LOW); pinMode(IN_PIN, OUTPUT);

    float capacitance;

//smol
    if (val < 1000){
      pinMode(IN_PIN, OUTPUT);
      capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val) - ZERO;

      Serial.print(F("Capacitance Value = "));
      Serial.print(capacitance, 3);
      Serial.print(F(" pF ("));
      Serial.print(val);
      Serial.println(F(") "));
    }

//not smol and chungusy
    else{
      pinMode(IN_PIN, OUTPUT);
      delay(1);
      pinMode(OUT_PIN, INPUT_PULLUP);
      unsigned long u1 = micros();
      unsigned long t;
      int digVal;

      do{
        digVal = digitalRead(OUT_PIN);
        unsigned long u2 = micros();
        t = u2 > u1 ? u2 - u1 : u1 - u2;
      } while ((digVal < 1) && (t < 400000L));

      pinMode(OUT_PIN, INPUT);
      val = analogRead(OUT_PIN);
      digitalWrite(IN_PIN, HIGH);
      int dischargeTime = (int)(t / 1000L) * 5;
      delay(dischargeTime);
      pinMode(OUT_PIN, OUTPUT);
      digitalWrite(OUT_PIN, LOW);
      digitalWrite(IN_PIN, LOW);

      capacitance = -(float)t / R_PULLUP / log(1.0 - (float)val / (float)MAX_ADC_VALUE);

//Output Part
      Serial.print(F("Capacitance Value = "));
      if (capacitance > 1000.0){
        Serial.print(capacitance / 1000.0, 2);
        Serial.print(F(" uF"));
      }
      else{
        Serial.print(capacitance, 2);
        Serial.print(F(" nF"));
      }

      Serial.print(F(" ("));
      Serial.print(digVal == 1 ? F("Normal") : F("HighVal"));
      Serial.print(F(", t= "));
      Serial.print(t);
      Serial.print(F(" us, ADC= "));
      Serial.print(val);
      Serial.println(F(")"));
    }
    while (millis() % 1000 != 0);
}

//Measure Time of one Oscillation
double IndTest(byte &precision){
    double long temp = 0;

    //predefine timeout constant and
        unsigned long timeout=1000000;
        unsigned long t=0;

    //Variable accuracy system (running average filter)
    for(byte i=0; i<precision; i++){
        //pulse the inductor to start oscillation
            digitalWrite(11, HIGH);
            delay(50);        //charge the inductor.
            digitalWrite(11,LOW);

        //wait for a full wave to finish
        while((ACSR & aco)&& t<timeout) {t++;} //wait for prev pulse to end

        unsigned long start = micros(); //start timer

        //ACO is bit withing the ACSR that is output of compartor
            while(!(ACSR & aco)&& t<timeout) {t++;}
            while((ACSR & aco)&& t<timeout) {t++;}
            if(t>=timeout) return -1;//break if too long

        //Add data to temp
            temp += micros() - start-4;
    }

    //If no timeout occurs: return total time
        return temp / (double) precision;
}

//Produce accurate reading with lowpass filter by calling above fnct and averaging
void MeasureInd(){
    //Define Precision of filter
    byte precision = 1; //max 255
    unsigned int cooldown = 1000;

    //Process the Time and convert to L val
    double T = IndTest(precision); //time in microsec

    if(T==-1) Serial.println(F("(/) ")); //NO Inductor connected

    else {
        if(T<100) Serial.print(F("(!) ")); //Low value, recommend using larger capacitor
        //Serial.print(T); Serial.print(" ");

    //Solve for L
        T = (T*T) / ( 4.0 * pipi * C);//convert to inductance
        T *= 1; //uH
        Serial.print(T); Serial.println(F(" uH"));
    }

    delay(cooldown); //cooldown
}


void loop(){
    //measure the switch pin for mode
    byte val = digitalRead(SWITCH_PIN);

    if(val) MeasureInd(); //Ind Mode
    else MeasureCap(); //Cap Mode
}
