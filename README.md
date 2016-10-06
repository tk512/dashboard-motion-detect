## About ##
The idea started when I set up a dashboard on a Raspberry Pi 3 in our office.
Seeing the dashboard is mounted on the wall, I wanted it automatically switched off
at night, which is easy enough, but not if people are still there, as they may still
want to look at it.

So by connecting a PIR to the Raspberry, this was easily done. Now I was able to switch on
and off the screen using DPMS commands whenever there was say, 15 minutes of lack of movement.

As an additional bonus, I added a SQLite3 database to record movements in the office, and this is actually
displayed on the dashboard itself. Not very useful, but it's fun to be able to see that people were
still coding till 9PM the previous night, by just looking at the chart.

## Installation ##

First install the prerequisites:
```
#!bash
$ sudo apt-get install git-core libsqlite3-dev sqlite3
$ git clone git://git.drogon.net/wiringPi
$ cd wiringPi
$ sudo ./build  (this will install the library as well)
```

Then, grab this tool and build it:

```
#!bash
$ git clone https://bitbucket.org/heliumdevops/dashboard-motion-detect.git
$ cd dashboard-motion-detect
$ make
```

Now you can run **dashboard-motion-detect** either directly or maybe from a startup script.
Just remember that it needs to run as root, and in order to run the screen-{on,off}.sh scripts, one
needs to allow root to run X programs (e.g. via 'xhost +'). 
