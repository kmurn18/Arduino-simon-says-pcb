# Engineering Log - Simon Says PCB
This is a running record of all the problems I ran into in the build of this project, and how I solved them.
The problems I list here are in chronological order, as you can see with the dates. I start the log with the problems with breadboard assembly, then skeleton code alteration, PCB design in altium, and then physical problems with the PCB.

## Problem 1 : buttons not working properly on breadboard

**Problem**: 
  After loading the skeleton example code onto my arduino and quickly assembling the required materials onto my breadboard, I ran into my first problem to cause the game not to work. 
I had simply gathered the materials needed from different kits I had gotten over the college semesters.
I found 4 random tactile switches in my lab kits, and put them into the board, but I found that they were not responding with the game. I couldn't select any colors with my switches, there was no response. 

**Diagnosis**:
  I had to look into the datasheet and see how the legs of the switch (4-connector switch) were wired up. It turned out that I had put the switches in the board so that the signal and ground were connected.
  Very simple problem and fix, but I am faithfully documenting every issue I had here, and this was the first.

**Fix**:
    The fix was simple. I just rotated the button, so that the incoming signal and ground were on different rails of the breadboard. Again, very simple, but running into this issue on the first go allowed me to 
    catalog it into my head, and remember it when it came time to create the PCB wiring.

  Below is a link to some pictures to help describe what I am talking about, the actual physical switches I used and the datasheet schematic to explain the wiring.

  [Actual button used](../images/initialbuttons.jpg)
  [Button Datasheet](../images/ButtonDatasheet.jpg)

## Problem 2 : "White" LED not lighting until I isolated the right legs

**Problem**:
  With the buttons finally responding, I moved on to the LEDs. I had grabbed four LEDs from my lab kits to match my four buttons - what I thought was a yellow, red, green, and white one. But the "white" one wouldn't light up in the circuit the way the others did, and when it did light, it came out orange instead of white.

**Diagnosis**:
  This LED had three legs instead of the two on a standard single-color LED, which meant it was an RGB LED - effectively three LEDs (red, green, blue) in one housing sharing a common leg. That changed how it had to be wired. The orange I saw was the red and green elements lighting together, and the middle common leg was interfering with getting a clean single connection the way my two-leg LEDs had.

**Fix**:
  Since I didn't have a datasheet for this loose part, I worked it out by hand. I bent the middle common leg up and out of the way so it wasn't connecting to anything, then experimented with the two outer legs to find which orientation of anode and cathode actually lit the LED in my circuit. Once I had the right leg in the right spot, it worked as a placeholder for the prototype.
  For the PCB I specced a proper two-leg single-color LED so the final board would have four clean matching colors. 

  Below is a picture of the RGB LED I used initially.

  [RGB LED](../images/RGBLED.jpg)

  ## Problem 3 : COM port access denied when uploading code
**Problem**:
  As I was working through the code and uploading new versions to the Arduino to test them, I started getting upload failures. 
  The code would compile just fine, but then the upload would fail with an error saying it couldn't open the COM port, something like "Access is denied" on COM3.
  It was frustrating because nothing was wrong with the actual code, it just wouldn't get onto the board.

**Diagnosis**:
  I figured out that the problem was the Serial Monitor. 
  Earlier in the project I had used the Serial Monitor to test whether my buttons were registering, and I had left it open. 
  The Serial Monitor holds the COM port open so it can listen to the board, and while it has the port occupied, the uploader can't access that same port to push new code. 
  So the two were fighting over the same connection.

**Fix**:
  The fix was to close the Serial Monitor window before uploading. 
  Once it let go of the port, the upload went through with no issue. 
  When that didn't immediately work another time, unplugging the USB cable and plugging it back in, and double checking the right port was selected under Tools, cleared it up. 
  This was the first issue that felt like actually debugging, as the problem was a bit trickier and took more troubleshooting.

  ## Problem 4 : Display went blank after adding it to the game code

**Problem**:
  I had the OLED display working on its own with a simple test sketch, showing text with no issue.
  But when I integrated the display into my actual game code and uploaded it, the screen went blank.
  The strange part was that the code compiled without any errors and uploaded successfully, so by every normal measure it should have worked.
  The board just sat there doing nothing, with the old test message frozen on the display or nothing at all.

**Diagnosis**:
  This one took real digging because there was no error message pointing me anywhere.
  I looked back at the compiler output and noticed it was reporting my global variables using a decent chunk of dynamic memory, but that number alone wasn't the whole story.
  The Arduino Uno and Nano both use the ATmega328 chip, which only has 2KB of SRAM to work with at runtime.
  The OLED display library allocates a frame buffer that eats up about 1KB of that on its own, just to hold the screen image.
  On top of that, I had two arrays for storing the game sequences that were declared as `int` type, and each array was 100 entries long, which added up to another big chunk of memory.
  Between the display buffer, those two arrays, and all my other global variables, I was blowing past the 2KB limit.
  It turns out when an ATmega328 runs out of SRAM it doesn't throw an error, it just crashes or behaves unpredictably, which is exactly why the code compiled fine 
  but did nothing when it actually ran.

**Fix**:
  I attacked the memory problem from two angles.
  First, I dropped the maximum level of the game from 100 down to 20, since nobody was realistically going to memorize a sequence that long anyway, which shrank both arrays.
  Second, I changed those two sequence arrays from `int` type to `byte` type, since each entry only needs to store a number from 0 to 3 to represent which button,
  and a `byte` holds that fine while using half the memory of an `int`.
  Those two changes freed up enough SRAM to get the board back under the limit, and the display sprang to life on the next upload.
  This was the most satisfying problem to solve in the whole build, because it forced me to actually understand the memory budget of the chip instead of just writing code and assuming it would fit. 
  It is the kind of constraint you never think about in normal desktop programming but is a real everyday concern in embedded systems.
  
