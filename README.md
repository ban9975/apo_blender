# My Blender
A simple blender program built for MZAPO.

## Wiring
1. Connect data cable of DC motors to MZ_APO PMOD1 port.
2. Connect LAN cable to Ethernet port on MZ_APO.
3. Connect the DC charger to DC motor and one cable from DC motor to the MZ_APO power input.
   
   <img src="https://github.com/ban9975/apo_blender/assets/55187987/fbbc0ef6-594d-4b11-bfd3-a68c81747e53" width="700">

## Build and Run
``` bash
make TARGET_IP=<YOUR_IP> run
```

## System Architecture
<img src="https://github.com/ban9975/apo_blender/assets/55187987/edccc148-a9a4-4534-9357-30bf10a7a831" width="700">

1. Setting: Speed or time haven't set yet. Set the speed by pressing red knob. Set the time by pressing green knob. If the time hasn't set yet, the time will be in red, otherwise yellow.

2. Ready: Both speed and time set. Press blue knob to start. If the speed or time are changed in this state, the system will be back to the setting state.

3. Running: DC motor is running. Rotating red and green won't change the speed and time in this state. After countdonw or if user press the blue knob, the system will be back to setting state.
