int ledPins[] = {5, 6, 9, 10, 11};
int pinCount = 5;
int buttonPin = 13;

int rangePin = A3;
int rIndex = 0;
const int numRanges = 10;
int ranges[numRanges];
int total = 0;
const unsigned maxRange = 600;

int ledStateCount = 4;

int ledState = 0;
int buttonState = 0;
int lastButtonState = 0;

unsigned long buttonTime = 0;
unsigned long previousTime = 0;

int flag = 0;

int mod(int a, int b) {
	return (((a % b) + b) % b);
}

int j = 0;
int direction_flag = 0;

// Reads the analog input from the range sensor and processes it
int getRange() {
	// Keeping a running total
	total -= ranges[rIndex];
	
	ranges[rIndex] = analogRead(A3);
	total += ranges[rIndex];
	rIndex = (rIndex+1)%numRanges;

	// Average to smooth out the raw input
	return total/numRanges;
}

// Set the pin modes and zero out the ranges array
void setup() {
	for (int i = 0; i < pinCount; i++) {
		pinMode(ledPins[i], OUTPUT);
	}
	pinMode(buttonPin, INPUT);
	pinMode(rangePin, INPUT);

	for (int i = 0; i < numRanges; i++) {
		ranges[i] = 0;
	}
}

void loop() {
	int range = getRange();

	long currentMillis = millis();

	// Store the previous and current button state
	lastButtonState = buttonState;
	buttonState = digitalRead(buttonPin);

	// Sensing a change in the state of the button
	if (lastButtonState != buttonState) {
		// Get the time when the button pin "rises"
		// The button reads high when it is being pressed down
		// So the physical button is doing the opposite of the button pin signal
		if (lastButtonState == LOW && buttonState == HIGH) {
			buttonTime = millis();
		}
		// Catch the button pin "falling"
		// The user letting go of the button
		else if (lastButtonState == HIGH && buttonState == LOW) {
			unsigned long delta = currentMillis - buttonTime;

			// If this delay has passed, we're good; increment the state
			// And mod the state by the total number of states
			if (delta > 50) {
				ledState = (ledState+1) % ledStateCount;
			}
		}
	}

	// A case structure for the state based on button presses
	switch (ledState) {
		// Distance-based on
		case 0:
			flag = 0;
			j = 0;
			// Squaring the range corrects for
			// The nonlinear response of the sensor
			ledDo(pow(range/(float)maxRange, 2)*255);
			break;
		// Distance-based flash
		case 1:
			flash(maxRange - range);
			break;
		// Distance-based bounce
		case 2:
			if (flag == 0) {
				ledDo(0);
				flag = 1;
			}
			bounce(maxRange - range);
			break;
		// Simply off
		case 3:
			ledDo(0);
			break;
	}
}

// Writes the analog input to each of the LEDs
void ledDo(int input) {
	for (int i = 0; i < pinCount; i++) {
		analogWrite(ledPins[i], input);
	}
}

// Toggles the LED by "flipping" their current digital state
void ledToggle() {
	for (int i = 0; i < pinCount; i++) {
		digitalWrite(ledPins[i], !digitalRead(ledPins[i]));
	}
}

// Flashes the LEDs with the millisecond interval "time"
void flash(int time) {
	if (millis() - previousTime >= time) {
		ledToggle();
		previousTime = millis();
	}
}

// Bounces the LEDs with the millisecond interval "time"
void bounce(int time) {
	// If we're at the far end of the bounce
	if (j == 5) {
		// Decrement to avoid hanging on one end for an extra cycle
		j--;
		// Flip the direction flag
		direction_flag = 1;
	}

	if (millis() - previousTime >= time) {
		if (direction_flag == 0)
			forward_cycle();
		else
			backward_cycle();
	}
}

// Cycles the LEDs one time
void forward_cycle() {
	// Turn on one LED, and turn off the one before it
	digitalWrite(ledPins[j], HIGH);
	digitalWrite(ledPins[j-1], LOW);
	j++;
	previousTime = millis();
}

// Same as forward cycle, but in the other direction
// Also has checks for this end of the bounce
void backward_cycle() {
	digitalWrite(ledPins[j-1], HIGH);
	// Checks to avoid hanging for an extra cycle on one end
	if (j != 0) {
		digitalWrite(ledPins[j], LOW);
		j--;
	}
	// If we're at one end, flip the direction and increment
	if (j == 0) {
		direction_flag = 0;
		j++;
	}
	previousTime = millis();
}
