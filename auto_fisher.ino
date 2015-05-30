int sensorPin=5; // light sensor input pin
unsigned int readAccum=0; // total of values from sensor input
int accumVals=30; // how many values to average
int readAVG; // average of values

int cast_delay = 2 * 1000; // amount of time between right click and the bobber having settled on screen
int reel_delay = 1 * 1000; // amount of time between right click and the bobber being at the player

boolean rod_status = false; // false = reeled, true = cast
boolean fishing = false; // should we be auto casting/reeling?

int reeled_light_value;
int cast_light_value;
double trigger_level_percent = .50; // light level in terms of percentage difference between reeled and cast light values
int trigger_light_value; // the actual light level that will act as our trigger for reeling in

int inputPin = 2;

void setup()
{
  Serial.begin(9600);
  Mouse.begin();
  pinMode(sensorPin, INPUT);
  pinMode(inputPin, INPUT);
}


int get_sensor_average(int accumVals)
{
  //capture accumVals values and average them
  readAccum=0;
  for (int i=0; i<accumVals; i++){
    readAccum += analogRead(sensorPin);
  }
  readAVG = readAccum/accumVals;
  
  return(readAVG);
}


void cast_rod(){
  if(rod_status == false){ // only do something if the rod is reeled in
    Mouse.press(MOUSE_RIGHT);
    delay(1);
    Mouse.release(MOUSE_RIGHT);
    rod_status = true;
    delay(cast_delay);
  }
}


void reel_rod(){
  if(rod_status == true){ // only do something if the rod is cast out
    Mouse.press(MOUSE_RIGHT);
    delay(1);
    Mouse.release(MOUSE_RIGHT);
    rod_status = false;
    delay(reel_delay);
  }
}


void get_fishing_bobber_values()
{
  reel_rod(); // reel it in and check
  reeled_light_value = get_sensor_average(accumVals);
  Serial.println("Reeled value:");
  Serial.println(reeled_light_value);
  
  cast_rod(); // then cast and check values
  cast_light_value = get_sensor_average(accumVals);
  Serial.println("Cast value:");
  Serial.println(cast_light_value);
  
  // compute a trigger value
  double light_level_difference = cast_light_value - reeled_light_value;
  trigger_light_value = light_level_difference * trigger_level_percent;
  trigger_light_value += reeled_light_value;

  Serial.println("Trigger value:");
  Serial.println(trigger_light_value);
}


void loop()
{
  if(digitalRead(inputPin) == HIGH){
    if(fishing == true){ // we fishin'?
      fishing = false; // now we not fishin'.
      Serial.println("Fishing stopped");
      reel_rod();
    } else {
      fishing = true; // now we fishin'.
      Serial.println("Fishing started");
      // initialize the reeled bobber light values
      get_fishing_bobber_values();

    }
  }
  
  if(fishing == true){ // we fishin'
    cast_rod();
    
    // check to see if we caught anything
    int sensor_value = get_sensor_average(accumVals);
    if (sensor_value < trigger_light_value) { // we got a bite! 
      reel_rod();
      // recompute light levels
      get_fishing_bobber_values();
    
    }
  }
}



