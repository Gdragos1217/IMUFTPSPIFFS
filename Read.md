# ESP 32 IMU/FTP/SPIFFS Server
## For data extraction purposes through FileZilla using the ICM - 20948.

To get this code working you must either have a ICM - 20948 chip connected to the ESP-32's I2C bus. This depends on each model but for the ESP32 Dev Module it should be; 
* 21 for SDA 

    and 

* 22 for SDL 

### Some essential libraries:
[Spiffs Library](https://github.com/pellepl/spiffs/blob/master/src/spiffs.h),
[FS Library](https://github.com/espressif/arduino-esp32/blob/master/libraries/FS/src/FS.h),
[ICM Library](https://github.com/sparkfun/SparkFun_ICM-20948_ArduinoLibrary/blob/master/src/ICM_20948.h),
[ESP8266 Wifi](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h), and 
[ESP8266 FTP Server](https://github.com/nailbuster/esp8266FTPServer/blob/master/ESP8266FtpServer.h).

After that is connected be sure to have *FileZilla* installed on whatever computer may be available on the network. Which you can download [here](https://filezilla-project.org/).

#### With all of the main components in order, it is time to explain the code in the most efficient way possible. 
The entirety of this code is taken originally from sparkfun for their ICM-20948, gathering the data off the main chip. After we have this data we need to store it onto the actual ESP32 with __SPIFFS__. Which writes and reads data off of the flash, in some summary. However to get this data we need to access it through _FileZilla_, and because of this, it is now using a File Transfer Protocol or an FTP server. 
This connects the ESP32 with all of the saved IMU data wirelessly to our computer using _FileZilla_ as a surrogate. 

#### Rundown of the code

``int TestingSpd = 30;`` is the literal speed in milliseconds at which the ESP32 requests data

Within the very first parts of ``void setup()`` we initialize our FTP Server, we do not need to immediately connect or rapidly connect to the ESP32 as it pings with ``int TestingSpd``. 

##### SPIFFS Basics

For any portion involving ``SPIFFS._____()`` this is where we set up the code for reading or appending any files. Taking a look into this code for example `` File file = SPIFFS.open("/file.csv", "a +");`` The first ``File`` indicates that it is indeed a file to assess. The second ``file`` on the other hand is the name of the document referenced by the code. This could be anything as later we use it to write to files, read files and delete files. Not only this but looking into the ``SPIFFS.open("/file.csv", "a +");`` there is a lot to unpack. 

The ``SPIFFS.open`` statement opens the file for reading, writing, or appending. This must be closed with a simple ``file.close``, the ``file`` is the name we gave for the compiler to read. For example, if we had; 

`` File file14 = SPIFFS.open("/file.csv", "a +");`` 

and 

`` File file12 = SPIFFS.open("/file.csv", "a +");``

We would need to have a separated ``file14.close`` and ``file12.close``, for this to even work. Along with working, we must dictate how each file will function. 

``a +`` is to append, or make changes to a file.


``w`` is to write information to a new file every time it is called.

``r`` is to read the file, and to see any traffic coming in the file.

With the knowledge of how, we must also know where, as ``"/file.csv"`` represents the file type and also its name. You can not have more than one directory. ``"/Arduino/filename.txt"`` is not possible. 

#### With that overview of __SPIFFS__ over, we can now continue with the rest of the code. 

With it we see more functions defining the path for the code to run upon, mainly dealing with chosing either a SPI or I2C.

The ICM code here actually is rather interesting, due to the fact that all of the calculations are done with the [ICM Library](https://github.com/sparkfun/SparkFun_ICM-20948_ArduinoLibrary/blob/master/src/ICM_20948.h) this it makes our jobs much easier. We can see that the function ``printScaledAGMT( myICM.agmt);`` is the main output when we have all of our things ready. This references more code at the bottom, which places the raw data into formatted and nice output. Meaning that everything we output is limited to the one previous line. 

This output is already being sent over to the flash memory on the ESP32 through __SPIFFS__ and because of this we must get the data off through *FileZilla*

### FileZilla Walkthrough

Using _FileZilla_ is rather easy once you get the hang of it. 

Firstly open up the client. Then on the front page, there will be a small icon labeled open site manager. 
![image1](/Images/1.png)

Next in the Site Manager, you should create a new site. Naming it whatever you may want. For the other things like _Host_ and _User_ we need to see what our ESP32 says. 
Look into the ESP32 code and once it starts it will state the actual _User_. 

![image2.5](/Images/2.5.png)

Once we have this we need to find out our _Host_, which we can see once we start up the program and run it on our ESP32 in the Serial Monitor. This stays the same if you stay on the same network.

![image2.75](/Images/2.75.png)

Once we have this we can input all the information we need into _FileZilla_ and get the IMU data. All we have to do after is input the password, _normally esp32_. Your settings should look like mine as well. ![image](/Images/2.png)
Once you are ready hit connect to actually start looking for the server on your end. It should connect as soon as it can, but if it does not then review the SSID and Passcode on the wifi portion of the FTP code. 


![image](/Images/3.png)

### Sending files to the ESP32
Downloading and sending files is incredibly straight forward, just drop whatever file you want into the __ESP32__ directory and bam! If sending it does not work the first time try it again and again. This is the same for downloading files, right click the file and hit download. It is a straightforward process.