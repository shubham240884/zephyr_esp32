#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED_NODE    DT_ALIAS(myled)
#if !DT_NODE_HAS_STATUS_OKAY(LED_NODE)
#error "Unsupported board: led devicetree alias is not defined"
#endif
static const int32_t sleep_time_ms = 100;
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(myled), gpios);

int main(void)
{
	int ret;
	int state = 0;

	// Make sure that the GPIO was initialized
	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	// Set the GPIO as output
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
	if (ret < 0) {
		return 0;
	}

	// Do forever
	while (1) {

		// Change the state of the pin and print
		state = !state;
		
		// Set pin state
		ret = gpio_pin_set_dt(&led, state);
		if (ret < 0) {
			return 0;
		}

		// Sleep
		k_msleep(sleep_time_ms);
	}

	return 0;
}
