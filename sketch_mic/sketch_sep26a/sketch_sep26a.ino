int Analog_Pin = A0; 
int Digital_Pin = 3;
int Threshold = 5;

  
void setup ()
{
  pinMode(Analog_Pin, INPUT);
  pinMode(Digital_Pin, INPUT);
       
  Serial.begin (9600); 
}

int last = 0;
int avg = -1;

 
void loop ()
{
  float Analog;
 
  Analog = analogRead (Analog_Pin);
  if (avg = -1) {
    avg = Analog;
  }
  if (abs(Analog - last) > Threshold) {
    Serial.println(Analog);  
  }
  avg = int(float(Analog) * 0.2 + float(avg) * 0.8);
  last = Analog;
}
