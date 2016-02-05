# Thread Pool

[![License](https://img.shields.io/badge/license-XFree86-blue.svg)](LICENSE)


A simple, but functional thread pool


## Example usage:

You add work/tasks to the thread pool like so:
```
p.add_work(function_pointer);
```
and you start the processing with start:
```
p.start();
```
you can add tasks at any time, and you can stop at any time:
```
p.stop();
```


Stop will be called automatically when the thread pool is destroyed. Stop and the destructor will wait for the threads to complete before exiting.


Thread Pool defaults the worker count to the number of CPUs available on the machine, or you can specify a worker count when constructed.

```