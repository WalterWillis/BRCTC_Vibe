# BRCTC_Vibe
Our RockSat-C/RockSat-X Project code

After the Python code's SPI library left much to be desired and the poor performance of my DotNet version that I created in one weekend also failed to meet my expectation, I opted to recreate the program reusing some of the logic from a previous year's version of our experiment that I wrote in C/C++. Again, I spent a single weekend, with a coffee in hand, piece-mealing the logic together.

I was super excited when I learned the different compiler versions of C++. The first programs I created were designed and compiled simply using the default compiler. A very important part of the program, the multi-threading, only allowed the program to run on seperate threads on a single core. I didn't notice this until after the 2017 launch! After some research, I learned that version 2011 of the C++ compiler introduced a whole slew of multi-threading, multi-processing goodeness! It also looked similar to the logic I use in C#! 

I hopped right into it and experimented. The multi-threading logic works like this:

* The main loop gathers a buffer of data that represents the likely buffer size the IO can optimally handle. 
* The loop tosses the buffer into a IO thread and a Telemetry thread. 
* The loop reruns and gathers data while the threads process.
* The loop checks to see if the IO thread is ready. It waits until it is ready before tossing in more data.
* If the Telemetry thread is not ready, it is not waited on and the loop continues.

The compiler handles the multi-core logic natively and thus, a much more efficient version of our code was created! We were able to get data and timing from all devices at around 2500 samples per second!

It worked super effectively, save the telemetry. An error caused the telemetry thread to halt after a half second during the tests at WVU and during the flight. We did not have access to the data from the tests, nor the tests themselves, so we were unfortunately not aware of an issue with telemetry untile *AFTER THE LAUNCH*. This caused a headache, but fortunately the experiment survived the landing and we were able to recover the data saved via the IO thread.

Another issue was hardware-related. After experiments at WVU, our gyroscope was returned and appeared to have been bricked. This could have been the result of delivery. Either way, I had to remove the gyro code very quickly before sending the payload for final integration. Unfortunately, I also removed the timing variable from the IO thread resulting in only the accelerometer values to be saved. We were really hoping for the timing of the data for the purpose of accurately identifying events during the launch, so I was very disheartened when I learned of my mistake. A cool result did occur, though. We were able to recieve around 8000 samples per second with the gyro values gone, so our resolution of the vibration is very high! Since the gyroscope was more or less of the same type of hardware as the accelorometers, we should be able to mathematically recover gyroscopic data. 

Oddly enough, we only got around 8 minutes of data from the launch, 5 of which is of the launch itself. The investigation is still ongoing, but I believe our power supply was cut off at some point. It doesn't necessarilly matter as the 5 minutes of data appears to contain every event during the launch! We got lucky there!
