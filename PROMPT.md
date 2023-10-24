## Initial Prompt

Can you implement a console application `workman` for time management in C++? The requirements are as follows:

- `workman c[reate] <work_name>`: Create a type of work with `work_name`
- `workman b[egin] <work_name> [start_time in hh:mm]`: Begin working on `work_name`, remember start time, if not specified, use current time
- `workman e[nd] <work_name> [end_time in hh:mm]`: End working on `work_name`, remember end time, if not specified then use current time, add `end_time - start_time` to total hours of `work_name`
- `workman s[tats]`: Print all type of works and:
  - total working hours (format all hours in hh:mm)
  - average working hours between the first day and the last day working on it
  - working hours in the past 7 days

All operations should store the result in a suitable place of local storage.
