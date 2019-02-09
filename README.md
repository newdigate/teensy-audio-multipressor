# teensy audio multipressor
a multi-band audio compression sketch using teensy microcontroller and a very crude logrythmic audio compression alogrythm. 

## effect_compressor
* f(x) = log10((9 * x)+1) where 0 < x < 1
* http://mathnotepad.com/
```
f(x) = log((9 * x)+1, 10)
plot( f(x) )
```
