# Time, Duration and Date

Altro provides following types for time and date:

* **time**: represents a time point which is a [UTC time](https://en.wikipedia.org/wiki/Coordinated_Universal_Time) value represented by the amount of period away from a specific point in time, regardless of the local time zone, such as 12:00 pm on January 1, 2023, GMT. 
* **duration**: represents a period of time used to specify the difference between two relative timepoints. For example, 3 hours, 2 seconds, etc. A duration can be negative, for example,  the duration from the midnight of today to the midnight of yesterday is -24 hours. 
* **date**: represents a calendar day, a specific 24-hour time period that begins at midnight of a day and ends at midnight the following day. For example, the date June 15, 2023. The date, when converted to time, is timezone dependent because the midnight of a day varies in different timezones.
* **timeinfo**: provides a tuple of values to give the time information such as the year, month, day, weekday, timezone, etc. for a specific time point in a given time zone.

## Time

The value of `time` is represented in terms of the number of nanoseconds before or after the UNIX epoch (the time point January 1, 1970, 00:00:00 UTC). In current implementation, Altro uses a 64-bit signed integer to represent a time value, which can represent a range approximately 584 years. Therefore, the minimum year the time value can have is 1678 and the maximum year the time value can have is 2264.

The class `time` is defined as below:

```altro
value class time is comparable
{
    // predefined time constants 
    meta const MAX_YEAR := 2264;
    meta const MIN_YEAR := 1678;

    time();  // constructs time value of the current time
    time(year,month,mday: int; tz: string=null);  // constructs time value of the midnight of a given date
    time(year,month,mday,hour,min,sec: int; tz: string=null);  // constructs time value of the given time
    time(ticks: long);  // constructs time value of the number of ticks
    meta func now(): time;                    // gets current time
    meta func today(tz: string=null): time;   // gets midnight time of the current date in a given time zone
    const func ticks (): long;                      // returns number of ticks of the time value
    const func tod(tz: string=null): duration;      // gets duration elapsed from the midnight of a given time zone
    const func dt(tz: string=null): datetime;       // converts to value to (date, duration since midnight) in a given time zone
    const func ti(tz: string=null): timeinfo;       // converts to value to timeinfo in a given time zone
    const func <=> (other: date): int;              // compare with date
    const func <=> (other: datetime): int;          // compare with datetime
    const func + (duration): time;                  // performs add operations involving a time point
    const func - (duration): time;                  // performs subtract operations involving a time point
    func += (duration): time;                 // add the offset by the duration to this time point
    func -= (duration): time;                 // subtract the offset by the duration from this time point
}
```
You can constrcut a time value uing one of its constuctors or meta functions:
```altro
t1 := time();  // current time
t2 := time(year=2022; month=10; mday=31; hour=10; min=15; sec=45);
t3 := (2022, 10, 31, 10, 15, 45, "GMT");
t4 := time.now()(); // current time
t5 := time.today(); // midnight of the current time in local time zone
t6 := time.today("Asia/Tokyo"); // Tokyo midnight of the current time
```
You can also use the format `yyyymmdd-hh:mm:ss.nnnnnnnnn` to represent a time value. For example, `20221031-10:15:45` represents a time value `10:15:45 on Oct 31, 2022` in the local time zone. This special format can only be used when a time value is expected. For example:
```altro
t7: time = 20221031-10:15:45;
t8 := 20221031-10:15:45; // Error: Semicolon expected
```
Here `t7` gets the time value that represents `10:15:45 on Oct 31, 2022` because the initialization expression for `t7` expects a time value. However, the name declaration `t8` generates a compiler error because we do not know the type of `t8` and we do not expect a time value from the initialization. To use the format in situition where it is unknown if a time value is required, a type qualifier with double colon `::` can be used:
```altro
t8 := time::20221031-10:15:45;
t9 := time::20221031;   // local midnight of Oct 31, 2022
t10: time = 20221031-10:15:45.023173494;   // time stamp with nanoseconds
```
To represent a time in a specific timezone, the timezone information can be attached at the end of the time format. The timezone information can be specified as a string for a full timezone name, such as `"America/New_York"` and `"Asia/Tokyo"`, or GMT offset such as `-05` and `+09`, GMT offset plus timezone code, for example, `+09JST`. Note that timezone code alone (execpt for `GMT`) cannot be used for timezone info because timezone code is not unique. Here are some example of time representations with timezone information:
```altro
20221031-10:15:45 GMT;
20221031-10:15:45 "Asia/Tokyo";
20221031-10:15:45+09;
20221031-10:15:45+09 JST;
```
Note that the value for timezone on the Windows platform works only when it is "GMT" or is null for the local timezone, and all other timezone values are ignored so that the local timezone will be assumed. 

* **`time()`** --
    constructs the time value of the current time<br><pre>
t := time(); // get current time
println([:t"%Lc%Z"], "Current time: ", t);
println([:t"%Lc%Z"], "Current time in GMT: ", t.ti("GMT"));
────────────────────────────────────────────────
Output:
Current time: Thursday, Jun 29, 10:28:34, 2023 CDT
Current time in GMT: Thursday, Jun 29, 15:28:34, 2023 GMT
</pre>

* **`time(year,month,mday: int; tz: string=null)`** --
    constructs the time value of the midnight of a given date in (year,month,mday). `tz' gives the timezone of the given date, andi f not given, local time zone is assumed<br><pre>
t := time(year=2022; month=10; mday=31; tz="America/New_York"); // get midnight of a Oct 31, 2022 New York time
println([:t"%Lc%Z"], t.ti("GMT"));  // print the time info in GMT
────────────────────────────────────────────────
Output: (GMT is 4 hours ahead of EDT):
Monday, Oct 31, 04:00:00, 2022 GMT
</pre>

* **`time(year,month,mday,hour,min,sec: int; tz: string=null)`** --
    constructs the time value of the given time in (year,month,mday,hour,min,sec). `tz' gives the timezone of the given time, and if not given, local time zone is assumed<br><pre>
t := time(2022, 10, 31, 10, 15, 45, "GMT");     // get time value of Oct 31, 10:15:45, 2022 GMT
println([:t"%Lc%Z"], t.ti("America/New_York")); // print the time info in New York time
────────────────────────────────────────────────
Output (EDT is 4 hours behind of GMT):
Monday, Oct 31, 06:15:45, 2022 EDT
</pre>

* **`time(ticks: long)`** --
    constructs the time value from number of ticks.<br><pre>
t := time.now();
t1 := time(t.ticks() + 1);
</pre>

* **`meta func now(): time`** --
    returns the current time value. The actual time value depends on the clock type used in the application. See [Clock Type](Clock.md) for more information.<br><pre>
println([:t"The current time printed in local time zone: %lc"], time.now());
────────────────────────────────────────────────
Output:
The current time printed in local time zone: Thursday, Jun 15, 22:32:47, 2023
</pre>

* **`meta func today(tz: string=null): time`** --
    returns the midnight time of the current date. `tz` gives the time zone identifier ([TZ Identifer](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones)) such as `America/New_York`, `Asia/Tokyo`, and `GMT` etc, which is used to determine the date boundary. If the time zone is not given, the default time zone used in the system or set by the application is assumed. <br><pre>
println([:t"Today's midnight time in local: %lc%Z"], time.today());
println([:t"Today's Tokyo midnight time printed in local time: %lc%Z"], time.today("Asia/Tokyo"));
println([:t"Today's GMT midnight time printed in local time: %lc%Z"], time.today("GMT"));
────────────────────────────────────────────────
Output:
Today's midnight time in local: Friday, Jun 16, 00:00:00, 2023 CDT
Today's Tokyo midnight time printed in local time: Friday, Jun 16, 10:00:00, 2023 CDT
Today's GMT midnight time printed in local time: Thursday, Jun 15, 19:00:00, 2023 CDT
</pre>

* **`const func ticks (): long;`** --
    returns number of ticks (nanoseconds away from Unix epoch) of the time value.

* **`func tod(tz: string=null): duration`** --
    returns the time of the day, which is the duration elapsed from the last midnight of the time. `tz` gives the time zone identifier used to determine the date boundary. If the time zone is not given, the default time zone used in the system or set by the application is assumed. <br><pre>
t :time = 20221031-10:15:45;
println("Time of the day in t: ", t.tod());
println("Tokyo's time of the day in t: ", t.tod("Asia/Tokyo"));
println("GMT's time of the day in t: ", t.tod("GMT"));
────────────────────────────────────────────────
Output:
Time of the day in t: 10:15:45
Tokyo's time of the day in t: 00:15:45
GMT's time of the day in t: 15:15:45
</pre>

* **`const func dt(tz: string=null): datetime`** --
    returns the date and time of the day. `tz` gives the time zone identifier used to determine the date boundary, and if not given, the default time zone is the one in the system or set by the application. <br><pre>
t :time = 20221031-10:15:45;
println("Time of the day in t: ", t.tod());
println("Tokyo's time of the day in t: ", t.tod("Asia/Tokyo"));
println("GMT's time of the day in t: ", t.tod("GMT"));
────────────────────────────────────────────────
Output:
Time of the day in t: 10:15:45
Tokyo's time of the day in t: 00:15:45
GMT's time of the day in t: 15:15:45
</pre>

* **`const func ti(tz: string=null): timeinfo`** --
 returns the timeinfo. `tz` gives the time zone identifier used to determine the date boundary, and if not given, the default time zone is the one in the system or set by the application. <br><pre>
t1 := time(year=2022; month=10; mday=31; hour=10; min=15; sec=45, zone="GMT");   // get time value from GMT time 20221031-10:15:45
println([:t"%Lc%Z"], t.ti("Asia/Tokyo"));   // print the time info in Tokyo local time
────────────────────────────────────────────────
Output:
Monday, Oct 31, 19:15:45, 2022 JST
</pre>

* **`const func <=> (other: date): int;`**<br>
**`const func <=> (other: datetime): int;`** --
 compares the time value with a date value or a datetime value. Local time zone is always assumed for date or datetime value. Because the `time` class implements the `comparable` interface, time values are already comparable values. These two functions are provided for comparing a time value with a value of different type. Here are some comparison examples <br><pre>
t1 := time();
t2 := time();
midnight := time.today();
println(t1 < t2);
println(t1 > midnight);
t3: time = 20221031-10:15:45;
d1: date = 20221031;
println(t3 > d1);
────────────────────────────────────────────────
Output:
true
true
true
</pre>To compare a time value with date or datetime in different time zone, you will need to convert the date or datetime value into time value. Consider:<br><pre>
println([:t"Local timezone is: %Z"], time());
t1 := time::20230629-11:32:00;
d1 := date::20230630;
println([:t"The t1 value in GMT: %Lc%Z"], t1.ti("GMT"));
println([:t"The CDT midnight of 20230630 in GMT: %Lc%Z"], d1.time().ti("GMT"));
println([:t"The JST midnight of 20230630 in GMT: %Lc%Z"], d1.time("Asia/Tokyo").ti("GMT"));
println(t1 > d1);
println(t1 > d1.time("Asia/Tokyo"));
────────────────────────────────────────────────
Output:
Local timezone is: CDT
The t1 value in GMT: Thursday, Jun 29, 16:32:00, 2023 GMT
The CDT midnight of 20230630 in GMT: Friday, Jun 30, 05:00:00, 2023 GMT
The JST midnight of 20230630 in GMT: Thursday, Jun 29, 15:00:00, 2023 GMT
false
true
</pre>

* **`const func + (duration): time;`**<br>
**`const func - (duration): time;`** --<br><pre>
    returns the time value after or before a duration from this time value<br><pre>
tomorrow := time.today() + duration::1 day;
yesterday := time.today() - duration::1 day;
</pre>

* **`func += (d: duration): time;`**<br>
**`func -= (d: duration): time;`** --<br><pre>
    applies the offset `d` to this time value and returns the modified time value<br><pre>
t := time.today();
t += duration::2 hours 30 minutes;
</pre>

## Duration

The value of `duration` is represented in terms of the number of nanoseconds, which is used to specify the distance between two relative timepoints. In current implementation, Altro uses a 64-bit signed integer to represent a duration, which can represent a duration up to approximately ±292 years.

The class `duration` is defined as below:

```altro
value duration time implements comparable, addable, incrementable, negatable
{
    // predefined duration constants 
    meta const ONE_MICROSEC, ONE_MILLISEC, ONE_SEC, ONE_MIN, ONE_HOUR, ONE_DAY, ONE_WEEK: duration;

    duration();  // constructs duration value of zero
    duration(hour,min,sec: int);  // constructs duration value of the given hours, minutes, and seconds
    duration(ticks: long);  // constructs duration value by number of ticks (nanoseconds)
    const func ticks (): long;                     // returns number of ticks of the duration
    const func * (double): duration;               // returns multiplied duration
    const func / (double): duration;               // returns fractionized duration
    const func / (duration): long;                 // divide the duration by another duration
    const func div (duration): (long, duration);    // divide the duration by another duration and get the quotient and the remainder
    func *= (double): duration;                    // multiplies this duration
    func /= (double): duration;                    // fractionizes this duration
}
```
You can construct a duration value using one of its constructors or predefined duration constants:
```altro
d1 := duration();
d2 := duration(hour=10; min=15; sec=45);
d3 := duration(1000);  // one thousand ticks
d4 := duration::ONE_DAY;
```
You can also use the duration format `<h>:<m>:<s>.<ns>` to represent a time value. For example, `123:15:45` represents a duration value of 123 hours 15 minutes and 45 seconds. This special format can only be used when a duration value is expected, otherwise, a type qualifier with double colon `::` must be used:. For example:
```altro
t7: duration = 10:15:45;
t8 := duration::10:15:45;
```
A duration can also be represented by a sequence of integers with special suffixes such as hour, minutes, days, etc. The following special suffixes can be used for duration values:
| suffix                               | meaning                   |
|:------------------------------------ |:------------------------- |
| w, week, weeks                       | number of weeks           |
| d, day, days                         | number of days            |
| h, hour, hours                       | number of hours           |
| m, min, minute, minutes              | number of minutes         |
| s, sec, second, seconds              | number of seconds         |
| millisec, millisecond, milliseconds  | number of milliseconds    |
| microsec, microsecond, microseconds  | number of microseconds    |
| n, ns, nsec, nanosecond, nanoseconds | number of nanoseconds     |

You can use combined duration units, for example:
```altro
d1: duration = 1 day 12 hours 30 minutes;
d2: duration = 1 day -12 hours;
d2 += 1 hour 20 minutes;
```
Because `duration` implements the interfaces of comparable, addable, incrementable, and negatable, you can use all functions provided in these interfaces. For example:
```altro
d1: duration = 30 minutes;
d2: duration = 1000 seconds;
println(d1 > d2);
d2 += 800 seconds;
println(d1 == d2);
d2++;
println(d2);
d2 = -d2;
println(d2);
d2 = |d2|;
println(d2);
────────────────────────────────────────────────
Output:
true
true
0:30:0.000000001
-0:30:0.000000001
0:30:0.000000001
```

The class `duration` does not implement functions required by the `scalable` interface. However, it has its own special version of functions to define how duration values can be multiplied or divided:
```altro
d1: duration = 30 minutes;
d2 := d1 * 2;   // d2 gets 60 minutes, two times more than d1
d3 := d2 / 3;   // d3 gets 20 minutes, one third of d2
d4 := d2 / d3;  // d4 gets 3, which is not a duration, but the number of how many times d1 is more than d3 
d5 := d1 div d3;  // d5 gets (1, 10 minutes), which means d1 is equal to 1 part of d3 plus 10 minutes
println("d1 = ", d1);
println("d2 = ", d2);
println("d3 = ", d3);
println("d4 = ", d4);
println("d5 = ", d5);
────────────────────────────────────────────────
Output:
d1 = 0:30:0
d2 = 1:0:0
d3 = 0:20:0
d4 = 3
d5 = (1, 0:10:0)
```

* **`duration()`** --
    constructs a zero duration value<br><pre>
println(duration());
────────────────────────────────────────────────
Output:
0:0:0
</pre>

* **`duration(hour,min,sec: int; ns:int=0)`** --
    constructs the duration value by (hours,minutes,seconds,nanoseconds).<br><pre>
println(duration(10, 15, 45));
────────────────────────────────────────────────
Output:
10:15:45
</pre>

* **`duration(ticks: long)`** --
    constructs the duration value from number of ticks.<br><pre>
println(duration(1000000));   // 1 millisecond
────────────────────────────────────────────────
Output:
0:0:0.001
</pre>

* **`const func ticks (): long`** --
    returns number of ticks (nanoseconds) of the duration.

* **`const func * (double): duration`**<br>
**`const func / (double): duration`** --
    returns the returns duration multiplied or fractionized by the given number.<br><pre>
d1: duration = 30 minutes;
d2 := d1 * 2;    // d2 gets 60 minutes, which is d1 multiplied by 2
d3 := d1 * 0.5;  // d3 gets 15 minutes, which is d1 multiplied by 0.5
d4 := d1 / 3;    // d4 gets 10 minutes, which is d1 fractionized by 3
d5 := d1 / 0.3;  // d5 gets 100 minutes, which is d1 fractionized by 0.3
println("d2 = ", d2);
println("d3 = ", d3);
println("d4 = ", d4);
println("d5 = ", d5);
────────────────────────────────────────────────
Output:
d2 = 1:0:0
d3 = 0:15:0
d4 = 0:10:0
d5 = 1:40:0
</pre>

* **`const func / (duration): long`** --
    divides the duration by another duration and returns the integral quotient.<br><pre>
d1: duration = 30 minutes;
d2: duration = 10 minutes;
d3: duration = 20 minutes;
println ("d1/d2 gets ", d1/d2);
println ("d1/d3 gets ", d1/d3);
────────────────────────────────────────────────
Output:
d1/d2 gets 3
d1/d3 gets 1
</pre>

* **`const func div (duration): (long, duration)`** --
    divides the duration by another duration and get the quotient and the remainder.<br><pre>
d1: duration = 30 minutes;
println ("d1 div 15 minutes = ", d1 div duration:: 15 minutes);
println ("d1 div 20 minutes = ", d1 div duration:: 20 minutes);
println ("d1 div 4.25 minutes = ", d1 div duration:: 4 minutes 15 seconds);
────────────────────────────────────────────────
Output:
d1 div 15 minutes = (2, 0:0:0)
d1 div 20 minutes = (1, 0:10:0)
d1 div 4.25 minutes = (7, 0:0:15)
</pre>

* **`const func *= (double): duration`**<br>
**`const func /= (double): duration`** --
    multipliesfractionizes this duration and returns the modified value.<br><pre>
d1: duration = 30 minutes;
println (d1 *= 2);
println (d1 /= 3);
────────────────────────────────────────────────
Output:
1:0:0
0:20:0
</pre>

## Date

The value of `date` is represented in an integer in the format of `yyyymmdd`. For example, 20230612 represents the date June 12, 2023. Because the year zero in the astronomical year numbering system coincides with the Julian year 1 BC, the value `00001231` represents December 31, 1 BC, one day before `00010101` (January 1, 1 AD)..

The class `date` is defined as below:

```altro
value class date implements comparable, incrementable
{
    date();                                      // constructs date value of the current date in the local time zone
    date(int);                                   // constructs date value from an integer
    date(year,month,mday: int);                  // constructs date value of year, month and the day of the month
    meta func today(tz: string=null): date;      // gets current date in the given time zone
    const func days (): int;                     // returns number of days since January 1, 1 AD (00010101), negative for BC years
    const func start(tz: string=null): time;     // returns the starting time (midnight) of the date in the given time zone
    const func ti(tz: string=null): timeinfo;    // returns to value to timeinfo to represent the midnight of the datein a given time zone
    const func + (int): date;                    // returns the date after the given number of days
    const func - (int): date;                    // returns the date before the given number of days
    const func - (date): int;                    // returns number of days from the given date
    func += (int): date;                         // add the offset by the duration to this time point
    func -= (int): date;                         // subtract the offset by the duration from this time point
}
```
The class `date` implements the comparable and incrementable interfaces. Therefore, date values can be compared and can be modified by the incremental operators `++` and `--`.

Note that the value for timezone in the Windows platform works only when it is "GMT" or is null for the local timezone, and all other timezone values are ignored so that the local timezone will be assumed. 

You can construct a date value using one of its constructors or meta functions:
```altro
d1 := date();             // today in local time zone
d2 := date(20221031);     // date Oct. 31, 2022
d2 := date(101);          // date Jan. 1, 1 BC. Note that you cannot have leading zeros otherwise the integer is octal
d2 := date(-10101);       // date Jan. 1, 2 BC.
d3 := date(2022, 10, 31); // date Oct. 31, 2022
d4 := date.today();       // today in local time zone
d5 := date.today("Asia/Tokyo"); // today in Tokyo time
```
You can also use the format `yyyymmdd` to represent a date value. This special format can only be used when a date value is expected. For example:
```altro
d6: date = 19691031;    // d6 gets a date value in Oct 31, 1969
d7 := 6691031;          // d7 gets an integer 6691031, not a date
d8 := date::6691031;    // d8 gets a date value in Oct 31, 1969
```
Also in the format of `yyyymmdd-hh`, you need to remove all leading zeros unless you use the type qualifier. Otherwise, the integer will be read as an octal number. For example,
```altro
d9: date = 06691031;     // Error: Invalid octal digit
d10 := date::06691031;   // Okay, d10 gets an date value in Oct 31, 669
```

* **`date()`** --
    constructs the date value for today in the local time zone<br><pre>
d := date();
println ([:t"%Lx %Z"], d);
────────────────────────────────────────────────
Output:
Friday, Jul 7, 2023 CDT
</pre>

* **`date(int)`** --
    constructs date value from an integer in the format of `yyyymmdd` with leading zeros removed<br><pre>
println ([:t%Lx], date(20221031));
────────────────────────────────────────────────
Output:
Monday, Oct 31, 2022
</pre>

* **`date(year,month,mday: int)`** --
    constructs date value of year, month and the day of the month.<br><pre>
println ([:t%Lx], date(2022, 10, 31));
────────────────────────────────────────────────
Output: (GMT is 4 hours ahead of EDT):
Monday, Oct 31, 04:00:00, 2022 GMT
</pre>

* **`meta func today(tz: string=null): date`** --
    returns the current date. `tz` gives the time zone identifier ([TZ Identifer](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones)) such as `America/New_York`, `Asia/Tokyo`, and `GMT` etc, which is used to determine the date boundary. If the time zone is not given, the default time zone used in the system or set by the application is assumed. <br><pre>
println ([:t"%Lx"], date.today());
println ([:t"%Lx"], date.today("Asia/Tokyo"));
────────────────────────────────────────────────
Output:
Friday, Jul 7, 2023
Saturday, Jul 8, 2023
</pre>

* **`const func days (): int;`** --
    returns number of days since January 1, 1 AD (00010101), negative for BC years. <br><pre>
println ("Today is ", date.today().days() ," days after 'January 1, 1 AD'.");
println (date::00001231, " is ", -date::00001231.days(), " day before 'January 1, 1 AD'.");
println (date::-00011231, " is ", -date::-00011231.days(), " days before 'January 1, 1 AD'.");
────────────────────────────────────────────────
Output:
Today is 738707 days after 'January 1, 1 AD'.
00001231 is 1 day before 'January 1, 1 AD'.
-00011231 is 366 days before 'January 1, 1 AD'.
</pre>

* **`func start(tz: string=null): time`** --
    returns the starting time (midnight) of the day. `tz` gives the time zone identifier used to determine the date boundary. If the time zone is not given, the default time zone used in the system or set by the application is assumed. <br><pre>
d := date::20230707;
println([:t"%Lx"], d, " in local (US Central) started at ", [:t"%Lc%Z"], d.start());
println([:t"%Lx"], d, " in Tokyo started at ", [:t"%Lc%Z"], d.start("Asia/Tokyo"));
println([:t"%Lx"], d, " in Tokyo started at ", [:t"%Lc%Z"], d.start("Asia/Tokyo").ti("Asia/Tokyo"));
────────────────────────────────────────────────
Output:
Sunday, Jul 7, 2023 in local (US Central) started at Friday, Jul 7, 00:00:00, 2023 CDT
Sunday, Jul 7, 2023 in Tokyo started at Thursday, Jul 6, 10:00:00, 2023 CDT
Sunday, Jul 7, 2023 in Tokyo started at Friday, Jul 7, 00:00:00, 2023 JST
</pre>

* **`const func ti(tz: string=null): timeinfo`** --
 returns the timeinfo. `tz` gives the time zone identifier used to determine the date boundary, and if not given, the default time zone is the one in the system or set by the application. <br><pre>
println([:t"%Lc%Z"], date().ti("Asia/Tokyo"))
────────────────────────────────────────────────
Output:
Monday, Oct 31, 19:15:45, 2022 JST
</pre>

* **`const func + (int): date;`**<br>
**`const func - (int): date;`** --<br><pre>
    returns the date in number of days after or before this date<br><pre>
d1 : date = 20230323;
println ([:t"%Lx"], "30 days after ", d1, " is ", d1 + 30);
println ([:t"%Lx"], "30 days before ", d1, " is ", d1 - 30);
────────────────────────────────────────────────
30 days after Thursday, Mar 23, 2023 is Saturday, Apr 22, 2023
30 days before Thursday, Mar 23, 2023 is Tuesday, Feb 21, 2023
</pre>

* **`const func - (date): int;`** --<br><pre>
    returns the number of days from the given date to this date<br><pre>
d1 : date = 20230323;
d2 : date = 20230531;
println ([:t"%Lx"], "There are ", d2-d1, " days from ", d1, " to ", d2);
────────────────────────────────────────────────
Output:
There are 69 days from Thursday, Mar 23, 2023 to Wednesday, May 31, 2023
</pre>

* **`func += (d: int): date;`**<br>
**`func -= (d: int): date;`** --<br><pre>
    applies the offset `d`, as number of days, to this date value and returns the modified date value<br><pre>
d := date.today();
d += 30;
</pre>

## Time Info Tuple

The tuple type `timeinfo` gives detailed information about time:
```altro
type timeinfo = value tuple
    ( sec, min, hour: tiny;
      month, mday, wday: tiny;
      isdst: bool;
      yday:  short;
      year:  short;
      gmtoff: short;
      zone: symbol8
    );
```
The tuple contains 11 members, which are explained as below:

| Member  | Type      | Meaning                            | Range       |
|:------- |:--------- |:---------------------------------- |:----------- |
| sec     | tiny int  | seconds after the minute           | 0-61        |
| min     | tiny int  | minutes after the hour             | 0-59        |
| hour    | tiny int  | hours since midnight               | 0-23        |
| month   | tiny int  | months since January               | 0-11        |
| mday    | tiny int  | day of the month                   | 1-31        |
| wday    | tiny int  | days since Sunday                  | 0-6         |
| isdst   | tiny int  | Daylight Saving Time flag          | 0, 1, or -1 |
| yday    | short int | days since January 1               | 0-365       |
| year    | short int | years since or before 1900         |             |
| gmtoff  | short int | time zone offset to GMT in minites |             |
| zone    | symbol8   | time zone code upto 8 characters   |             |

The Daylight Saving Time flag (isdst) is 1 if Daylight Saving Time is in effect, 0 if Daylight Saving Time is not in effect, and -1 if the information is not available. The member `sec` is generally 0-59 and the extra range is to accommodate for leap seconds in certain systems.

The [formatter for date and time](OutputFormatting.md) `:t`, if present, is used for printing `timeinfo` tuple. If the formatter for date and time is absent, then the [formatter for elements of containers](OutputFormatting.md), if any, is used. For example:
For example:
```altro
tinfo: timeinfo = time.now().ti("Asia/Taipei");
setlang("中文", "台灣");
println("Time format in Chinese: ", [:t"%Lc%Z"], tinfo);
println("Time into elements:\n", [:C1], tinfo);
────────────────────────────────────────────────
Output:
Time format in Chinese: 二零二三年七月九日星期日零點十七分十三秒CST
Time into element:
sec = 13
min = 17
hour = 0
month = 6
mday = 9
wday = 0
isdst = false
yday = 189
year = 123
gmtoff = 28800
zone = CST
```

## Date and Time

The class `datetime` is used to represent a time point in terms of date, duration since the midnight of the date, and the time zone info for the date and time. `datetime` can represent a range of time much more broader than the type `time`, and it prereserves the timezone information.
```altro
value class datetime implements comparable
{
    datetime(tz:string=null);                      // constructs date time value of the current in the given timezone
    datetime(t:time; tz:string=null);              // constructs date time value from the given timepoint and the timezone
    datetime(d:date; t: duration; tz:string=null); // constructs date time value from the given date, time of day and the timezone
    const func d(): date;                          // returns the date
    const func tod(): duration;                    // returns the time of the day
    const func t(): time;                          // returns the time value
    const func ti(): timeinfo;                     // returns the time info
    const func +(duration): datetime;              // returns the date and time after the given duration
    const func -(duration): datetime;              // returns the date and time before the given duration
    const func +=(duration): datetime;             // adds the offset by the duration to this date time and returns the result
    const func -=(duration): datetime;             // subtracts the offset by the duration to this date time and returns the result
    const func gmtoff(): int;                      // returns number of seconds for the GMT offset of the datetime value
    const func tzname(): string;                   // returns the timezone name of the datetime value
    const func tzcode(): string;                   // returns the timezone code of the datetime value
}
```
The class `datetime` implements the comparable interfaces. Therefore, datetime values can be compared.
```altro
dt1 := datetime::20221101-00:15:45"Asia/Tokyo";
dt2 := datetime::20221031-10:15:45"America/Chicago";
dt3 := datetime::20221031-10:15:46"America/Chicago";
println(dt1==dt2);
println(dt1 < dt3);
────────────────────────────────────────────────
Output:
true
true
```
You can construct a datetime value using one of its constructors or meta functions:
```altro
d1 := datetime();              // current date and time in local timezone
d2 := datetime("Asia/Tokyo");  // current date and time in Tokyo timezone
d3 := datetime(time::now(), "Asia/Tokyo");            // current date and time in Tokyo timezone
d4 := datetime(date::today()-1, duration::12:00:00);  // date and time of yesterday noon in local timezone
```
You can also use the time format `yyyymmdd` to represent a datetime value. For example:
```altro
d5 := datetime::20221031-10:15:45 GMT;
d6 := datetime::20221031-10:15:45 "Asia/Tokyo";
d7 := datetime::20221031-10:15:45-05;
```
* **`datetime(tz:string=null)`** --
    constructs the datetime value for the current time in the given time zone<br><pre>
dt := datetime("Asia/Tokyo");
println([:t"%Lc%Z"], dt);
────────────────────────────────────────────────
Output:
Sunday, Aug 27, 13:00:14, 2023 JST
</pre>
* **`datetime(t:time; tz:string=null)`** --
    constructs the datetime value for the given epoch time in the given time zone<br><pre>
dt := datetime(time::20230826-11:10:45 GMT, "Asia/Tokyo");
println([:t"%Lc%Z"], dt);
────────────────────────────────────────────────
Output:
Sunday, Aug 27, 13:00:14, 2023 JST
</pre>
* **`datetime(d:date; t: duration; tz:string=null)`** --
    constructs the datetime value for the given epoch time in the given time zone<br><pre>
dt := datetime(date::20230827, duration::13:00:14, "Asia/Tokyo");
println([:t"%Lc%Z"], dt);
────────────────────────────────────────────────
Output:
Sunday, Aug 27, 13:00:14, 2023 JST
</pre>
* **`const func d(): date`** --
    returns the date of the datetime value<br><pre>
println([:t"%Lc%Z"], datetime("Asia/Tokyo").d());
println([:t"%Lc%Z"], datetime("GMT").d());
────────────────────────────────────────────────
Output:
Saturday, Aug 26, 2023
Sunday, Aug 27, 2023
</pre>* **`const func tod(): duration`** --
    returns the time of the day in terms of duration since midnight<br><pre>
println(datetime("Asia/Tokyo").tod());
println(datetime("GMT").tod());
────────────────────────────────────────────────
Output:
20:18:55.72878585
11:18:55.72977905
</pre>
* **`const func t(): time`** --
    converts this datetime value to epoch time.<br><pre>
println([:t"%Lc%Z"], "The current datetime in local timezone: ", datetime("Asia/Tokyo").t());
println([:t"%Lc%Z"], "The current datetime in local timezone: ", datetime("GMT").t());
────────────────────────────────────────────────
Output:
The current datetime in local timezone: Saturday, Aug 26, 06:13:39, 2023 CDT
The current datetime in local timezone: Saturday, Aug 26, 06:13:39, 2023 CDT
</pre>
* **`const func + (duration): datetime;`**<br>
**`const func - (duration): datetime;`** --
    returns the datetime after or before this date by the given duration<br><pre>
d := datetime::20221031-15:45:30 + duration::10 hours 30 minutes;;
println(d);
────────────────────────────────────────────────
Output:
20221101-02:15:30.000000
</pre>
* **`func += (d: duration): datetime;`**<br>
**`func -= (d: duration): datetime;`** --<br><pre>
    applies the offset `d` in duration to this datetime value and returns the modified datetime value<br><pre>
d := datetime();
d += duration::30 days;
println("set d to 30 days after the time of today: ", d);
────────────────────────────────────────────────
Output:
set d to 30 days after the time of today: 20230927-08:20:20.953611200
</pre>
* **`const func gmtoff(): int`** --
    returns GMT offset in number of minutes for this datetime value.<br><pre>
d := datetime("Asia/Tokyo");
println(d.gmtoff());
────────────────────────────────────────────────
Output:
540
</pre>
* **`const func tzname(): string`** --
    returns the timezone name of the datetime value.<br><pre>
d := datetime("Asia/Tokyo");
println(d.tzname());
────────────────────────────────────────────────
Output:
Asia/Tokyo
</pre>
* **`const func tzcode(): string`** --
    returns the timezone code of the datetime value.<br><pre>
d := datetime("Asia/Tokyo");
println(d.tzcode());
────────────────────────────────────────────────
Output:
JST
</pre>

