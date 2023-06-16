# Time, Duration and Date

Altro provides following types for time and date:

* **time**: represents a time value for a specific point in time, such as 12:00 pm on January 1, 2023. 
* **duration**: represents a period of time used to specify the amount of time that has elapsed since a timepoint. For examples, 3 hours, 2 seconds, etc.
* **date**: representsa specific 24-hour time period that begins at midnight of a day and ends at midnight the following day. For example, the date June 15, 2023.
* * **timeinfo**: provides a tuple of values to give the time information such as the year, month, day, weekday, etc. for a specific point in time.

## Time

The value of `time` is represented in terms of number nanoseconds before or after UNIX epoch (the time point January 1, 1970, 00:00:00). In current implementation, Altro uses a 64-bit signed integer to represent a time value, which can represents a range approximately 584 years. Therefore, the minmum year the time value can have is 1678 and the maximum year the time value can have is 2264.

The class `time` is defined as below:

```altro
value class time
{
    meta func now(): time;                    // get current time
    meta func today(tz: string=null): time;   // get midnight time of the current date in a given time zone
    func tod(tz: string=null): duration;      // get duration elapsed from the midnight of a given time zone
    func dt(tz: string=null): datetime;       // convert to value in (date, duration since midnight) in a given time zone
}
```
* **`meta func now(): time`** --
    returns the current time. The actual time value got dependens on the clock type used in the application. See [Clock Type](Clock.md) for more information.<br><pre>
println([:t"The current time: %lc"], time.now());  // output: The current time: Thursday, Jun 15, 22:32:47, 2023
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
