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

## Functions
``` c
struct fbuf *fb_init();
// Create a frame buffer and allocate memory to it.
fb_clear(struct fbuf *fb, int color);
// Fill the whole lcd with color.
void fb_update(struct fbuf *fb);
// Render the data inside fb to lcd screen.
void fb_draw_pixel(struct fbuf *fb, int x, int y, int c);
// Change the color of data inside fb to given position with color c.
int fb_draw_char(struct fbuf *fb, int x, int y, font_descriptor_t *font, char ch, int c);
// Draw a character ch with given font to given position with color c and return the width of the character.
void draw_background(struct fbuf *fb, int c);
// Draw background (the frame and the title of blender) with color c.
void draw_cur_time(struct fbuf *fb, int t, int c);
// Draw the countdown of the blender with color c.
```

## User Manual
1. Connect MZAPO and DC motor in the way described in [Wiring](https://github.com/ban9975/apo_blender/blob/main/README.md#wiring) part.
2. By turning the red knob, you can adjust the speed of the blender. The LED line indicates current speed. Press red knob to confirm the speed.
3. By turning the green knob, you can adjust the time of the blender. The lcd screen shows current time. The maximum time is 59 seconds. Press green knob to confirm the time. If the time is set, it will be shown in yellow, otherwise red.
4. After both speed and time set, the RGB led will change from yellow to green. This mean the blender is ready to start. Press blue knob to start the blender. If you adjust either speed or time at this state, it will go back to setting state. This can be detected by RGB led turns back to yellow. Only the changed parameter needs to be re-confirmed.
5. During the running state, the RGB led is in red. In this state, turning both red and green knobs won't change the setting. If you press the blue knob, the blender will stop and go back to setting state.
6. After countdown, the blender will be back to setting state.

## Link of the repo
[apo_blender](https://github.com/ban9975/apo_blender)

## Reference
1. [MZAPO Manual from APO course website](https://cw.fel.cvut.cz/wiki/courses/b35apo/en/documentation/mz_apo/start)
2. [pysimCoder-examples/Linux-mzapo/DCmotor/](https://github.com/robertobucher/pysimCoder-examples/tree/main/Linux-mzapo/DCmotor)
