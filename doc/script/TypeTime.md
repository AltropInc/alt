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
    func dt(tz: string=null): datetime;       // convert to value in (date, duration since midnight) in a given time zone
    func tod(tz: string=null): duration;      // get duration elapsed from the midnight of a given time zone
}
```
* **`meta func now(): time`** --
    gets the current time. The actual time value got dependens on the clock type used in the application. See [Clock Type](Clock.md) for more information.<br><pre>
println([:t"The current time: %lc"], time.now());  // output: The current time: Thursday, Jun 15, 22:32:47, 2023
</pre>
