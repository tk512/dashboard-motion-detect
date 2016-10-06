/*
 * dashboard-motion-detect.c:
 *      Motion Detector for Office Dashboard
 *
 *      Uses ISR (interrupt) method to run interrupt handling code
 *      when GPIO pin 4 (wiring Pi 7) of the PIR goes HIGH
 *
 *      PIR = Pyroelectric Infrared Sensor aka passive motion sensor
 *
 *      How to use:
 *        Connect a standard Adafruit-like PIR to the Raspberry. Use
 *        GPIO pin 4 on the board, along with 5V and GND.
 * 
 *      July 2016, Torbjørn Kristoffersen <tk@mezzanineware.com>
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sqlite3.h>

#include <wiringPi.h>

#define WPI_PIN 7
#define IDLE_THRESHOLD 900

#define SCRIPT_PATH "dpms-scripts"
#define SCREEN_ON "screen-on.sh"
#define SCREEN_OFF "screen-off.sh"

#define DB_FILE "motion.sqlite3"
#define DB_MOTION_TABLE "create table motion (motion_timestamp datetime not null, screen_on int not null)"
#define DB_INSERT_START "insert into motion (motion_timestamp, screen_on) values ('"

bool create_motion_table() {
    sqlite3 *conn;

    int ret = sqlite3_open(DB_FILE, &conn);
    if(ret == SQLITE_ERROR) {
        fprintf(stderr, "Could not open database file: " DB_FILE "\n");
        sqlite3_close(conn);
        return false;
    }

    // Check if table already exists
    sqlite3_stmt *stmt;
    ret = sqlite3_prepare_v2(conn, "pragma table_info(motion)", -1, &stmt, NULL);
    if(ret != SQLITE_OK) {
        fprintf(stderr, "Could not check for motion table: %s\n", sqlite3_errmsg(conn));
        sqlite3_close(conn);
        return false;
    }

    ret = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (ret != SQLITE_ROW) {
        ret = sqlite3_exec(conn, DB_MOTION_TABLE, NULL, 0, 0);
        if(ret != SQLITE_OK) {
            fprintf(stderr, "Could not create motion table: %s\n", sqlite3_errmsg(conn));
            sqlite3_close(conn);
            return false;
        }
        fprintf(stderr, "Created motion table in SQLite3 database: " DB_FILE "\n");
    } else {
        fprintf(stderr, "Found motion table in SQLite3 database: " DB_FILE "\n");
    }

    // Always close
    sqlite3_close(conn);
    return true;
}

void insert_motion_table(bool is_screen_on) {
    sqlite3 *conn;

    int ret = sqlite3_open(DB_FILE, &conn);
    if(ret == SQLITE_ERROR) {
        fprintf(stderr, "Could not open database file: " DB_FILE "\n");
        sqlite3_close(conn);
        return;
    }

    time_t cur_time = time(NULL);
    if(cur_time == ((time_t)-1)) {
        perror("time");
        return;
    }

    struct tm *tmp = localtime(&cur_time);
    if(tmp == NULL) {
        perror("localtime");
        return;
    }

    char buf[64];
    if(strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmp) == 0) {
        perror("strftime");
        return;
    }

    int len = strlen(DB_INSERT_START) + strlen(buf) + 5; // quote + comma + number + ) + NULL
    char *final = malloc(len);
    if(final == NULL) {
        perror("malloc");
        return;
    }
    
    // Build query
    snprintf(final, len, "%s%s',%d)", DB_INSERT_START, buf, (is_screen_on ? 1 : 0));

    // Use string
    ret = sqlite3_exec(conn, (char*)final, NULL, 0, 0); 
    if (ret != SQLITE_OK) {
        fprintf(stderr, "Could not execute query '%s' due to: %s\n", final, sqlite3_errmsg(conn));
    } else {
        fprintf(stderr, "Executed query '%s'\n", final);
    }

    // Free memory and close DB
    free(final);
    sqlite3_close(conn);
}

static volatile int screen_on = 1;
static volatile int idle_counter = 0;

void sighandler(int signo) {
    if (signo == SIGCHLD) {
        int status;
        wait(&status);
    }
}

void turn_screen_on() {
    fprintf(stderr, "Turning screen ON\n");
    fflush(stderr);
    int pid;
    if((pid = fork()) == 0) {
        execl(SCRIPT_PATH "/" SCREEN_ON, SCREEN_ON, NULL);
    }
}

void turn_screen_off() {
    fprintf(stderr, "Turning screen OFF\n");
    fflush(stderr);

    int pid;
    if((pid = fork()) == 0) {
        execl(SCRIPT_PATH "/" SCREEN_OFF, SCREEN_OFF, NULL);
    }
}

void log_motion() {
    if(screen_on == 1) {
        insert_motion_table(true);
    } else {
        insert_motion_table(false);
    }
}

void motion_detected (void) { 
    // Reset idle count
    idle_counter = 0;

    // Log motion in SQLite3 database
    log_motion();

    if(screen_on == 1) {
        return;
    }

    turn_screen_on();
    screen_on = 1;
}


int main (void) {
    wiringPiSetup();

    wiringPiISR(WPI_PIN, INT_EDGE_RISING, &motion_detected);

    // Catch the SIGCHLD signal so we can reap the screen zombie process
    signal(SIGCHLD, sighandler);

    // Turn screen on initially
    turn_screen_on();

    // Create motion table in SQLite3 database
    if(!create_motion_table()) {
        return 1;
    }

    // CPU will spend most of its time in delay, so we don't waste CPU cycles
    for (;;) {
        delay(1000);
        if(idle_counter < IDLE_THRESHOLD) {
            idle_counter++;
        } else if(idle_counter == IDLE_THRESHOLD && screen_on == 1) {
            turn_screen_off();
            screen_on = 0;
        }
    }

    return 0;
}
