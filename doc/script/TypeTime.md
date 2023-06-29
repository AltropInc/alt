# Time, Duration and Date

Altro provides following types for time and date:

* **time**: represents a timepoint which a [UTC time](https://en.wikipedia.org/wiki/Coordinated_Universal_Time) value represented by the amount of period away from a specific point in time, regardless of the local time zone, such as 12:00 pm on January 1, 2023, GMT. 
* **duration**: represents a period of time used to specify the difference between two relative timepoints. For examples, 3 hours, 2 seconds, etc. A duration can be negative, for example,  the duration from the midnight of today to the midnight of yesterday is -24 hours. 
* **date**: represents a calendar day, a specific 24-hour time period that begins at midnight of a day and ends at midnight the following day. For example, the date June 15, 2023. The date is time zone dependent because the midnight of a day varies in different time zones.
* * **timeinfo**: provides a tuple of values to give the time information such as the year, month, day, weekday, timezone, etc. for a specific time point in a given time zone.

## Time

The value of `time` is represented in terms of number nanoseconds before or after UNIX epoch (the time point January 1, 1970, 00:00:00 UTC). In current implementation, Altro uses a 64-bit signed integer to represent a time value, which can represents a range approximately 584 years. Therefore, the minmum year the time value can have is 1678 and the maximum year the time value can have is 2264.

A time value can be written in a foramt `yyyymmdd-hh:mm:ss`. For example, `20221031-10:15:45` represents a time value `10:15:45 on Oct 31, 2022` in local time zone. 

The class `time` is defined as below:

```altro
value class time
{
    time(year,month,mday: int; tz: string=null);  // constructs time value of the midnight of a given date
    time(year,month,mday,hour,min,sec: int; tz: string=null);  // constructs time value of the given time
    meta func now(): time;                    // gets current time
    meta func today(tz: string=null): time;   // gets midnight time of the current date in a given time zone
    func tod(tz: string=null): duration;      // gets duration elapsed from the midnight of a given time zone
    func dt(tz: string=null): datetime;       // converts to value to (date, duration since midnight) in a given time zone
    func ti(tz: string=null): timeinfo;       // converts to value to timeinfo in a given time zone
}
```
* **`time(year,month,mday: int; tz: string=null)`** --
    constructs the time value of the midnight of a given date in (year,month,mday). `tz' gives the timezone of the given date, andi f not given, local time zone is assumed<br><pre>
t := time(year=2022; month=10; mday=31; zone="America/New_York"); // get midnight of a Oct 31, 2022 New York time
println([:t"%Lc%Z"], t.ti("GMT"));  // print the time info in GMT
────────────────────────────────────────────────
Output:
Monday, Oct 31, 05:00:00, 2022 GMT
</pre>
* **`time(year,month,mday,hour,min,sec: int; tz: string=null)`** --
    constructs the time value of the given time in (year,month,mday,hour,min,sec). `tz' gives the timezone of the given time, and if not given, local time zone is assumed<br><pre>
t := time(year=2022; month=10; mday=31; hour=10; min=15; sec=45, zone="GMT"); // get time value of Oct 31, 10:15:45, 2022 GMT
println([:t"%Lc%Z"], t.ti("America/New_York")); // print the time info in New York time
────────────────────────────────────────────────
Output:
Monday, Oct 31, 06:15:45, 2022 EDT
</pre>
* **`meta func now(): time`** --
    returns the current time value. The actual time value got dependens on the clock type used in the application. See [Clock Type](Clock.md) for more information.<br><pre>
println([:t"The current time printed inlocal time zone: %lc"], time.now());
────────────────────────────────────────────────
Output:
The current time printed inlocal time zone: Thursday, Jun 15, 22:32:47, 2023
</pre>
* **`meta func today(tz: string=null): time`** --
    returns the midnight time of the current date. `tz` gives the time zone identifier ([TZ Identifer](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones)) such as `America/New_York`, `Asia/Tokyo`, and `GMT` etc, which is used to determine the date boundary. If the time zone is not given, the defualt time zone used in the system or set by the application is assumed. <br><pre>
println([:t"Today's midnight time in local: %lc%Z"], time.today());
println([:t"Today's Tokyo midnight time printed in local time: %lc%Z"], time.today("Asia/Tokyo"));
println([:t"Today's GMT midnight time printed in local time: %lc%Z"], time.today("GMT"));
────────────────────────────────────────────────
Output:
Today's midnight time in local: Friday, Jun 16, 00:00:00, 2023 CDT
Today's Tokyo midnight time printed in local time: Friday, Jun 16, 10:00:00, 2023 CDT
Today's GMT midnight time printed in local time: Thursday, Jun 15, 19:00:00, 2023 CDT
</pre>
* **`func tod(tz: string=null): duration`** --
    returns the time of the day, which is the duration elapsed from the last midnight of the time. `tz` gives the time zone identifier used to determine the date boundary. If the time zone is not given, the defualt time zone used in the system or set by the application is assumed. <br><pre>
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
* **`func dt(tz: string=null): datetime`** --
    returns the date and time of the day. `tz` gives the time zone identifier used to determine the date boundary, and if not given, the defualt time zone is the one in the system or set by the application. <br><pre>
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
* **`func ti(tz: string=null): timeinfo`** --
 returns the timeinfo. `tz` gives the time zone identifier used to determine the date boundary, and if not given, the defualt time zone is the one in the system or set by the application. <br><pre>
t1 := time(year=2022; month=10; mday=31; hour=10; min=15; sec=45, zone="GMT");   // get time value from GMT time 20221031-10:15:45
println([:t"%Lc%Z"], t.ti("Asia/Tokyo"));   // print the time info in Tokyo local time
────────────────────────────────────────────────
Output:
Monday, Oct 31, 19:15:45, 2022 JST
</pre>
