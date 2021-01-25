# BBB controlled electric smoker

NOTE: This repository contains instructions which must not tested in real
operation. These are provided only for academic interest. Electricity can
kill and these designs are NOT approved for any use. Do not try them.

Furthermore - there is zero security effort put in this device. If you use
even just the software described here you are risking the privacy of your
WIFI network.

Some documentation:
https://raw.githubusercontent.com/M-Vaittinen/beagle-dev-tools/bbq-smoker/smoker-project/doc/smoker_doc.pdf

PCB Schematics:
(NOTE issue with meat temperature measurement)
https://raw.githubusercontent.com/M-Vaittinen/beagle-dev-tools/bbq-smoker/smoker-project/schematics/smoker_schematics.pdf

Please note. I haven't uploaded complete control SW for BBB. I will try to work
getting this repository up-to-date and oranized. This will take a while though.

Feel free to drop me a note at mazziesaccount@gmail.com if you find this
interesting :)


## BBB SW requirements not in this repository:

Network connection.
 - You need to figure out a way to connect your BBB to WIFI when it boots.

Web interface:
 - Web server + php. (AFAIR I installed httpd and php from beagle apt repositories)
 
Plotting temperatures.
  - gnuplot. (AFAIR I installed gnuplot from beagle apt repositories)

I should some day revise the software and also add commands to install them...

--Matti
