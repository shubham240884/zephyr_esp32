This repository contains sample programs for esp32 with Zephyr

To run and test the programs use west to build and flash the zephyr image for esp32. Follow the following steps :

1) Activate the west environment :
   source ~/zephyrproject/.venv/bin/activate
  
2) To build the image run the following command :
   west build -p always -b esp_wrover_kit/esp32/procpu

3) flash the image:
   west flash

5) Monitor the boot sequence and results :
   west espressif monitor
     
