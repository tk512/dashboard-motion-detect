## About ##

The idea started when I set up a dashboard on a Raspberry Pi 3 in our office.
Seeing the dashboard is mounted on the wall, I wanted it automatically switched off
at night, which is easy enough, but not if people are still there, as they may still
want to look at it.

![alt tag](https://raw.githubusercontent.com/tk512/dashboard-motion-detect/master/dashboard-on-wall.jpg)

So by connecting a PIR to the Raspberry, this was easily done. Now I was able to switch on
and off the screen using DPMS commands whenever there was say, 15 minutes of lack of movement.

![alt tag](https://raw.githubusercontent.com/tk512/dashboard-motion-detect/master/raspberry-in-tray.jpg)

As an additional bonus, I added a SQLite3 database to record movements in the office, and this is actually
displayed on the dashboard itself (in a tiny corner). To be fair, it's not very useful, but it's still fun to be 
able to see that people were at the office (working hard I'm sure) until 9PM the previous night, by just looking at the chart.  We can also see motion history at a weekly level:

![alt tag](https://raw.githubusercontent.com/tk512/dashboard-motion-detect/master/motion-chart.jpg)

## Installation ##

First install the prerequisites:
```
$ sudo apt-get install git-core libsqlite3-dev sqlite3
$ git clone git://git.drogon.net/wiringPi
$ cd wiringPi
$ sudo ./build  (this will install the library as well)
```

Then, grab this tool and build it:

```
$ git clone https://github.com/tk512/dashboard-motion-detect.git
$ cd dashboard-motion-detect
$ make
```

Now you can run **dashboard-motion-detect** either directly or maybe from a startup script.
Just remember that it needs to run as root, and in order to run the screen-{on,off}.sh scripts, one
needs to allow root to run X programs (e.g. via 'xhost +'). 
