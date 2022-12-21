// basic_tasks.ino
// MIT License (see file LICENSE)
// Change the following if you want to use

#define LED1 15 // GPIO 12
#define LED2 2// etc.
#define LED3 4

struct s_led {
  byte gpio; // LED GPIO number
  byte state; // LED state
  unsigned napms; // Delay to use (ms)
  TaskHandle_t taskh; // Task handle
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
  int app_cpu = 0; // CPU number
  delay(500); // Pause for serial setup
  app_cpu = xPortGetCoreID();
  printf("app_cpu is %d (%s core)\n",
  app_cpu,
  app_cpu > 0 ? "Dual" : "Single");
  printf("LEDs on gpios: ");
  for ( auto& led : leds ) {
    pinMode(led.gpio,OUTPUT);
    digitalWrite(led.gpio,LOW);
    xTaskCreatePinnedToCore(
    led_task_func,
    "led_task",
    2048,&led,1,&led.taskh,app_cpu);
    printf("%d ",led.gpio);
  }
  putchar('\n');
}

void loop() {
  for (int i = 0; i < 10; i++)
  {
    delay(1000);
    if (i == 9 )
    {
      for (auto& led : leds)
      {
        vTaskSuspend( led.taskh );
      }
      delay(3000);
      for (auto& led : leds)
      {
        vTaskResume( led.taskh );
      } 
    }
  }
}
