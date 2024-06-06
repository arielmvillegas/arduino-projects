#include <Arduino_FreeRTOS.h>

/*
  Programmer: Ariel Marie Villegas
  Course: COMP 522 Embedded Applications
  Date: 05/18/2023
  Description: Controls a distance measurement sysyem using an ultrasonic, control
  LEDS, and a buzzer based on the measured distances.

*/

// Define task priorities
#define DISTANCE_TASK_PRIORITY 1
#define LED_TASK_PRIORITY 2
#define BUZZER_TASK_PRIORITY 3

// Define task stack sizes
#define DISTANCE_TASK_STACK_SIZE 128
#define LED_TASK_STACK_SIZE 128
#define BUZZER_TASK_STACK_SIZE 128

// Define task handles
TaskHandle_t distanceTaskHandle;
TaskHandle_t ledTaskHandle;
TaskHandle_t buzzerTaskHandle;

// Define pin numbers
#define trigPin 6
#define echoPin 5
#define buzzerPin 2
#define greenPin 11
#define yellowPin 12
#define redPin 13

float new_delay;
long duration, distance;

// Task function for measuring distance
void distanceTask(void* pvParameters) {
  while (1) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration / 2) / 29.1;
    new_delay = (distance * 3) + 30;

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

// Task function for controlling LEDs and buzzer
void ledTask(void* pvParameters) {
  while (1) {
    if (distance < 50) {
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      vTaskDelay(new_delay / portTICK_PERIOD_MS);
      digitalWrite(buzzerPin, LOW);
    } else if (distance < 100) {
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(redPin, LOW);
      digitalWrite(buzzerPin, HIGH);
      vTaskDelay(new_delay / portTICK_PERIOD_MS);
      digitalWrite(buzzerPin, LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS); // Delay for yellow light flashing
      digitalWrite(yellowPin, LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS); // Delay for yellow light flashing
    } else {
      digitalWrite(greenPin, HIGH);
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, LOW);
      digitalWrite(buzzerPin, LOW);
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);

  xTaskCreate(distanceTask, "DistanceTask", DISTANCE_TASK_STACK_SIZE, NULL, DISTANCE_TASK_PRIORITY, &distanceTaskHandle);
  xTaskCreate(ledTask, "LEDTask", LED_TASK_STACK_SIZE, NULL, LED_TASK_PRIORITY, &ledTaskHandle);

  // Start the FreeRTOS scheduler
  vTaskStartScheduler();
}

void loop() {
  // Empty loop, the execution will be handled by FreeRTOS tasks
}