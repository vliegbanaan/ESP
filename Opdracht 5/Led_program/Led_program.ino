//basic_tasks.ino

#define LED1 12  // pin 12
#define LED2 13  // pin 13
#define LED3 14  // pin 14

struct s_led {
  byte          gpio;         //LED GPIO NUMMER
  byte          state;        // LED STATE
  unsigned      napms;        // Delay to use(ms)
  TaskHandle_t  taskh;         //task handle  
};

static s_led leds[3] = {
  { LED1, 0, 500, 0 },
  { LED2, 0, 200, 0 },
  { LED3, 0, 750, 0 }
};

static void led_task_func(void *argp) {
  s_led *ledp = (s_led*)argp;
  unsigned stack_hwm = 0, temp;

  delay(1000);

  for (;;) {
    digitalWrite(ledp->gpio,ledp->state ^= 1);
    temp = uxTaskGetStackHighWaterMark(nullptr);
    if ( !stack_hwm || temp < stack_hwm ) {
      stack_hwm = temp;
      printf("Task for gpio %d has stack hwm %u\n",
        ledp->gpio,stack_hwm);
    }
    delay(ledp->napms);
  }
}

void setup() {
  int app_cpu = 0;  //cpu counter

  delay(500);  //pause for serial setup

  app_cpu = xPortGetCoreID();
  printf("app_CPU is %d (%s core)\n",
  app_cpu,
  app_cpu > 0 ? "dual" : "Single");

  printf("Leds on gpios: ");
  for ( auto& led : leds ) {
    pinMode(led.gpio,OUTPUT);
    digitalWrite(led.gpio,LOW);
    xTaskCreatePinnedToCore(
      led_task_func,
      "led_task",
      2048,
      &led,
      1,
      &led.taskh,
      app_cpu
     );
     
  printf("%d ",led.gpio);     
  }
  putchar('\n');
}



void loop() {

int counter = 0;

  delay(1000);
  counter++;

  if (counter == 10){

    printf("Als bron heb ik het boek FreeTROS for ESP32 arduino gebruikt....\n");
    printf("Om specifiek te zijn, bladzijde 47 Listing 2-4. the Task_Suspend/task.suspend.ino program.");
    printf("\n");
    
    printf(".... Suspending de task, counter is 10....\n");
    vTaskSuspend(leds[0].taskh);  //suspend de task als counter 10 is.
    vTaskSuspend(leds[1].taskh);
    vTaskSuspend(leds[2].taskh);
    delay(5000);                  //wacht 5 seconden.
    printf(".... Resuming de task, Counter wordt 0 gemaakt....\n");
    vTaskResume(leds[0].taskh);   //resume de taak 
    vTaskResume(leds[1].taskh);
    vTaskResume(leds[2].taskh);              
    counter = 0;                  // Zet counter op 0.
  }
}
