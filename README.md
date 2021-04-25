# teensy audio multipressor
a multi-band audio compression sketch using teensy microcontroller and a very crude logrythmic audio compression alogrythm. 

* basically a wave shaper that applies a logrithmic wave shape

## effect_compressor
* f(x) = log10((9 * x)+1) where 0 < x < 1

![input / output curve](log_curve.png)

* http://mathnotepad.com/
```
f(x) = log((9 * x)+1, 10)
plot( f(x) )
```
