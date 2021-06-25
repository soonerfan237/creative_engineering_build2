#include <Adafruit_NeoPixel.h>

int LEDPIN = 6; //the arduino pin connected to the pixels
int NUMPIXELS = 100; //number of pixels in the build
int canvas_size = 10; //Setting canvas size
int num_of_turns = 10; //Setting number of turns
int infection_rate = 4; //Setting infection rate (scale of 0-10)
int vaccination_rate = 0; //Setting vaccination rate (scale of 0-10)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800); //Setting up pixel library
int turn_delay = 1000; //delay for half a second
int sim_infect_count = 0; //for reporting the number infected during each simulation

//this holds the pixel id at each array position
int pixel_pos_array[10][10] = { 
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, 
  {19,18,17,16,15,14,13,12,11,10},
  {20,21,22,23,24,25,26,27,28,29},
  {39,38,37,36,35,34,33,32,31,30},
  {40,41,42,43,44,45,46,47,48,49},
  {59,58,57,56,55,54,53,52,51,50},
  {60,61,62,63,64,65,66,67,68,69},
  {79,78,77,76,75,74,73,72,71,70},
  {80,81,82,83,84,85,86,87,88,89},
  {99,98,97,96,95,94,93,92,91,90}
};

//this holds the color at each array position
//green=0, red=1, blue=2, yellow=3
//initializing all pixels to green
int pixel_color_array[10][10] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

//function to retrieve the pixel position id from the board coordinates
int get_pixel_pos_from_array_coords(int pixel_row, int pixel_col){ //takes the row and column of the array as input
  int pixel_pos; //variable to hold pixel position
  pixel_pos = pixel_pos_array[pixel_row][pixel_col]; //gets the pixel position from the position array
  return pixel_pos; //returns pixel position
}

//function to print out a readable version of the matrix to the console
void print_color_array() {
  Serial.print('\n');
  for (int row = 0; row < canvas_size; row++) {
    for (int col = 0; col < canvas_size; col++) {
      Serial.print(pixel_color_array[row][col]);
      Serial.print(" ");
    }
    Serial.print('\n');
  }
}

void setup() {
  pixels.begin(); //This initializes the NeoPixel library.
  Serial.begin(115200); //for writing to console
  randomSeed(analogRead(0)); //using analog input to generate a unique random seed each time
  sim_infect_count = 1; //initializing infected count to 1 during each sim, since we start with one infected
}

void loop() {
  Serial.print("BEGIN");
  
  //initializing all pixels to green
  for (int row = 0; row < canvas_size; row++) {
    for (int col = 0; col < canvas_size; col++) {
      pixels.setPixelColor(get_pixel_pos_from_array_coords(row,col), pixels.Color(0,100,0));
      pixel_color_array[row][col] = 0;
    }
  }
  //initializing vaccinated pixels to blue
  for (int row = 0; row < canvas_size; row++) {
    for (int col = 0; col < canvas_size; col++) {
      if (random(10) < vaccination_rate) {
        pixels.setPixelColor(get_pixel_pos_from_array_coords(row,col), pixels.Color(0,0,100));
        pixel_color_array[row][col] = 2;
      }
    }
  }

  //initializing an infected pixel to red
  int initial_row = random(canvas_size);
  int initial_col = random(canvas_size);
  pixels.setPixelColor(pixel_pos_array[initial_row][initial_col], pixels.Color(100,0,0)); //red
  pixel_color_array[initial_row][initial_col] = 1;

  pixels.show(); //displays the pixel colors
  
  print_color_array(); //printing the color array to the console

  for (int i = 0; i <= num_of_turns; i++) { //loop through each turn
    Serial.print('\n');
    Serial.print('\n');
    Serial.print("turn: ");
    Serial.print(i); //printing the current turn number
    
    //this part is going through each pixel and infecting nearby pixels
    for (int row = 0; row < canvas_size; row++) { //loop through rows
      for (int col = 0; col < canvas_size; col++) { //loop through columns
        if (pixel_color_array[row][col] == 1) { //if find an infected pixel
          for (int row_radius = -1; row_radius <= 1; row_radius++) { //row search radius
            for (int col_radius = -1; col_radius <= 1; col_radius++) { //column search radius
              if (row + row_radius >= 0 && row + row_radius < canvas_size && col + col_radius >= 0 && col + col_radius < canvas_size) { //checking that the search pixel is in bounds
                if (pixel_color_array[row + row_radius][col + col_radius] != 2 && pixel_color_array[row + row_radius][col + col_radius] != 1) { //if the neighboring pixel is not vaccinated or currently infected
                  if (random(10) < infection_rate) { //random roll to see if it becomes infected
                    pixels.setPixelColor(get_pixel_pos_from_array_coords(row + row_radius,col + col_radius), pixels.Color(100,0,0)); //setting pixel to red
                    pixel_color_array[row + row_radius][col + col_radius] = 3; //setting color array to yellow as intermediate step
                  }
                }
              }
            }
          }
        }
      }
    }

    //this part is fixing the color array from yellow to red for the newly infected pixels
    for (int row = 0; row < canvas_size; row++) { //loop through rows
      for (int col = 0; col < canvas_size; col++) { //loop through columns
        if (pixel_color_array[row][col] == 3) { //if find a yellow pixel
          pixel_color_array[row][col] = 1; //setting color array to red now that the turn is done
          sim_infect_count = sim_infect_count + 1; //counting the total number of infected during the sim
        }
      }
    }

    pixels.show(); //displays the pixel colors
    print_color_array(); //printing the pixel array to the console
    delay(turn_delay); //delay between turns
  }

  Serial.print("NUMBER INFECTED: ");
  Serial.print(sim_infect_count); //printing total infected count during the sim to the console
  delay(turn_delay*3); //delay between turns

  //flashing all pixels to white when the animation is done
  for (int row = 0; row < canvas_size; row++) {
    for (int col = 0; col < canvas_size; col++) {
      pixels.setPixelColor(get_pixel_pos_from_array_coords(row,col), pixels.Color(100,100,100));
    }
  }
  pixels.show(); //displays the pixel colors
  delay(turn_delay/2); //delay between turns

  //iterating through vaccination rates
  vaccination_rate = vaccination_rate + 4; //increasing vaccination rate by 4
  if (vaccination_rate > 10) { //ensuring the vaccination rate does not exceed 10
    vaccination_rate = 0; //resetting to 0 if its already over 10
  }

  sim_infect_count = 1; //resetting sim infection count to 1 between each sim
  
}
