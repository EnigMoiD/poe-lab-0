int ledPins[] = {9, 10, 11, 5, 6};
int pinCount = 5;
int buttonPin = 8;

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

int getRange() {
	total -= ranges[rIndex];
	int read = analogRead(A3);
	// if (read < 80) read = 150;
	ranges[rIndex] = read;
	total += ranges[rIndex];
	rIndex = (rIndex+1)%numRanges;

	Serial.println(total/numRanges);

	return total/numRanges;
}

void setup() {
	for (int i = 0; i < pinCount; i++) {
		pinMode(ledPins[i], OUTPUT);
	}
	pinMode(buttonPin, INPUT);
	pinMode(rangePin, INPUT);

	for (int i = 0; i < numRanges; i++) {
		ranges[i] = 0;
	}

	Serial.begin(9600);
}

void loop() {
	int range = getRange();

	long currentMillis = millis();
	if (lastButtonState != buttonState) {
		if (lastButtonState == LOW && buttonState == HIGH) {
			buttonTime = millis();
		}

		else if (lastButtonState == HIGH && buttonState == LOW) {
			unsigned long delta = currentMillis - buttonTime;

			if (delta > 50) {
				ledState = (ledState+1) % ledStateCount;
			}
		}
	}

	switch (ledState) {
		case 0:
			flag = 0;
			j = 0;
			ledDo(range/(float)maxRange*255);
			break;
		case 1:
			flash(maxRange - range);
			break;
		case 2:
			if (flag == 0) {
				ledDo(0);
				flag = 1;
			}
			bounce(maxRange - range);
		break;
		case 3:
			ledDo(0);
			break;
	}

	lastButtonState = buttonState;
	buttonState = digitalRead(buttonPin); 
}

void ledDo(int input) {
	for (int i = 0; i < pinCount; i++) {
		analogWrite(ledPins[i], input);
	}
}

void ledToggle() {
	for (int i = 0; i < pinCount; i++) {
		digitalWrite(ledPins[i], !digitalRead(ledPins[i]));
	}
}

// state 2
void flash(int time) {
	if (millis() - previousTime >= time) {
		ledToggle();
		previousTime = millis();
	}
}

// state 3
void cycle(int time) {
	if (millis() - previousTime >= time) {            
		digitalWrite(ledPins[j], HIGH);
		digitalWrite(ledPins[mod(j-1, pinCount)], LOW);
		j = (j+1)%pinCount;
		previousTime = millis();
	}
}

void bounce(int time) {
	if (j == 5) {
		j--;
		direction_flag = 1;
	}

	if (millis() - previousTime >= time) {
		if (direction_flag == 0)
			forward_cycle();
		else
			backward_cycle();
	}
}

void forward_cycle() {         
	digitalWrite(ledPins[j], HIGH);
	digitalWrite(ledPins[mod(j-1, pinCount)], LOW);
	j++;
	previousTime = millis();
}

void backward_cycle() {
	digitalWrite(ledPins[j-1], HIGH);
	if (j != 0) {
		digitalWrite(ledPins[j], LOW);
		j--;
	}
	if (j == 0) {
		direction_flag = 0;
		j++;
	}
	previousTime = millis();
}
