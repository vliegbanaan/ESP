#define GPIO_LED 4
#define GPIO_BUTTONL 15
#define GPIO_BUTTONR 19
static QueueHandle_t queue;
static void debounce_task(void *argp) {
unsigned button_gpio = *(unsigned*)argp;
uint32_t level, state = 0;
uint32_t mask = 0x7FFFFFFF;
int event, last = -999;

for (;;) {
level = !digitalRead(button_gpio);
state = (state << 1) | level;
if ( (state & mask) == mask )
event = button_gpio; // Press
else
event = -button_gpio; // Release
if ( event != last ) {
if ( xQueueSendToBack(queue,&event,1) == pdPASS )
last = event;
}
taskYIELD();
}
}
static void press_task(void *argp) {
static const uint32_t enable = (1 << GPIO_BUTTONL)
| (1 << GPIO_BUTTONR);
BaseType_t s;
int event;
uint32_t state = 0;

// Make sure press is OFF
digitalWrite(GPIO_LED,LOW);
for (;;) {
s = xQueueReceive(
queue,
&event,
portMAX_DELAY
);
assert(s == pdPASS);

if ( event >= 0 ) {
// Button press
state |= 1 << event;
} else {
// Button release
state &= ~(1 << -event);
}
if ( state == enable ) {
// Activate press when both
// Left and Right buttons are
// pressed.
digitalWrite(GPIO_LED,HIGH);
} else {
// Deactivate press
digitalWrite(GPIO_LED,LOW);
}
}
}
//
// Initialization:
//
void setup() {
int app_cpu = xPortGetCoreID();
static int left = GPIO_BUTTONL;
static int right = GPIO_BUTTONR;
TaskHandle_t h;
BaseType_t rc;
delay(2000); // Allow USB to connect
queue = xQueueCreate(40,sizeof(int));
assert(queue);
pinMode(GPIO_LED,OUTPUT);
pinMode(GPIO_BUTTONL,INPUT_PULLUP);
pinMode(GPIO_BUTTONR,INPUT_PULLUP);
rc = xTaskCreatePinnedToCore(
debounce_task,
"debounceL",
2048, // Stack size
&left, // Left button gpio
1, // Priority
&h, // Task handle
app_cpu // CPU
);
assert(rc == pdPASS);
assert(h);
rc = xTaskCreatePinnedToCore(
debounce_task,
"debouncer",
2048, // Stack size
&right, // Right button gpio
1, // Priority
&h, // Task handle
app_cpu // CPU
);
assert(rc == pdPASS);
assert(h);
rc = xTaskCreatePinnedToCore(
press_task,
"led",
2048, // Stack size
nullptr, // Not used
1, // Priority
&h, // Task handle
app_cpu // CPU
);
assert(rc == pdPASS);
assert(h);
}

// Not used:
void loop() {
vTaskDelete(nullptr);
}
