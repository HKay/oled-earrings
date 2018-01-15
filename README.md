# oled earrings

This year I had a special gift for my girl. Earrings that feature an oled screen with animations of her favorite arcade game, Tetris.

The original idea was to have tetris blocks fall from the top of the screen to the bottom and stack up. But in the End I had to settle with a simple scrolling animation of tetris blocks, due to the limited energy budget.
It uses a rechargeable lithium coin cell (LIR2032) to power a oled screen and an ATTiny85 micro-controller driving it.

Once the power switch is flipped the display is initialized and cleared then the tetris blocks are drawn in a predetermined pattern. The display is set to auto scroll top to bottom and finally the microcontroller enters deep sleep without any way to return.

![front side of pcb with protective foil still on](https://raw.githubusercontent.com/HKay/oled-earrings/master/doc/front.jpeg)
![back side of pcb](https://raw.githubusercontent.com/HKay/oled-earrings/master/doc/backside.jpeg)
The components are hand soldered on dot grid prototyping board. The board is just a little bigger than the Display itself. The battery holder uses most of the pcb area. The rest is covered with the ATTiny85. To fit an SOIC-8 IC on a 2.54mm grid the pins have to be bent quite a lot.

![schematics](https://raw.githubusercontent.com/HKay/oled-earrings/master/doc/schematics.png)
The switch is for battery under voltage protection.
To flash the ATTiny85 I had to solder tiny wires to the 6 pins required. It's anoying but at a production volume of two quite manageable.

# Issues that have to be addressed in V2
- **Weight** - That's the show stopper. Battery runtime is pretty good however that doesn't matter if she never wears them, due to weight. 9 grams are just too much.
- There is no battery management. It has to be removed for charging and there is no battery under voltage protection.
- Only one animation.


Short youtube clip showing the earring animation: https://www.youtube.com/watch?v=d6km96m5RsY
