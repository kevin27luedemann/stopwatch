# Main Ideas for Project

## Stop Watch

1. split mode

   This mode is continuing to count the time, but at times of the users choosing an snapshot of the time is saved and can be recalled.  
   Therefor it is necessary to have a infinite list of time structures.
   For this purpose I need to shorten the elements in the time structure or create a new one specific for stop watch modes.
   The clock on the other hand needs its one structure keeping the time.
   But it does not need methods for incrementing or other operations like addition.
   If enough flash is available I will make both become one in order to have a long time stop watch mode.

1. round mode

   This mode is resetting the timer after time intervals of the users choosing.
   The time before the reset is saved to RAM and is displayable with the rotary encoder in the same way as the previous mode.

1. long time mode

   With this mode the time of the RTC is saved into EEPROM and compared to the momentary time.
   With this mode a precision of about a second is fine, because the RTC has a much lower drift then the internal clock.
   It would even be possible to have an automated switch from one of the other modes to this one by always saving the RTC time to RAM and after a day change the mode.
   This can be accomplished by a software overflow specific to the stopwatch, so that it does only increment until a day is full and then calculate deltas.
   Especially because if the precision discussed below this is helpful and will also help preserve the battery.

1. different precision mode and auto precision

   The modes should be able to display the momentary time and the mode specific previous times in an suitable manner.
   This leads to better readability and possibility to skip through all times in the order they where recorded without constant setting changes.  
   *Therefore an assessment has to be made at which times a truncation can be adequate or if a fixed precision is still better.*

1. get precision by reaction test and save it to EEPROM

   A mode needs to be added to calculate the uncertainty for a user mode.
   If it is hand triggered or by external input has large impact on the uncertainty.
   This has to be saved to the EEPROM in order not to repeat this process over and over.
   But it has to be redone ones user changes are made.

1. frequency mode

   A mode for measuring the frequency of the input signal has to be implemented.
   This is just a round mode with reset every interrupt and the inverse time in Hertz.

1. speed mode

   Similar to the previous one the frequency of interrupts is calculated but multiplied to a length.
   This could be the path length for a bike tire or some thing else similar.

1. bike mode

   This is an alternate implementation if the previous one with also calculated distance, maximum velocity and other interesting values specific for bike usage.

## Counter

1. single counter with up and down counting
2. multiple counters with up and down counting
3. changing the amount of counters and the way to reach theme in order to get fast switching between them

## saving of times

If the user wishes to save the split or round times to disc a method is needed to have a rolling buffer in the EEPROM.
Therefore a implementation is needed that allows for admin defined space containing settings and user defined space containing a few sets of split and round values.
The amount of available EEPROM is therefore split up into sections of variable length with a descriptor and amount value up front and then the least amount of space per value as possible.
The controller has to keep track of the amount of saved variables and organise the data.
A Method is needed to copy the data back into RAM and delete the data from index if wished.

## Computer slave mode

If a computer is attached to the watch it has an external power supply and can keep running without using the internal battery.
This allows for some more modes and a constant communication to a terminal via USB serial ports.

## Settings

1. set the Backlight intensity
1. set the backlight on time or to permanent
1. set the time and date independently


