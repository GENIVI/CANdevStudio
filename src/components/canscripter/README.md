# Can scripting
CanDevStudio is using JSON format to automate signals sending. 

## Structure
* Json Object - Main element
    * Json Object - Timers
        * Json Array - Timer steps. Following steps are accepted:
            * name - signal name
            * value - signal value
            * value-start, value-end, value-step, duration - number of signals will be calcualted using start, end and step elements. Duration (ms) is used to calculate delay between signals. E.g. to send ten signals with values from 1 to 10 and delay 100 ms between them following configuration has to be used: 
                * value-start: 1
                * value-end: 10
                * value-step: 1
                * duration: 1000 
            * pre-dalay - wait period before sending a signal
            * post-delay - wait period after sending signal

## Timer steps types
### Delay step
#### Required elements (one of):
* pre-delay
* post-delay
#### Examples
```
{
    "pre-delay" : 100
}
```
```
{
    "post-delay" : 100
}
```
```
{
    "pre-delay" : 100,
    "post-delay" : 100
}
```
### Single signal step
#### Required elements:
* name
* value
#### Optional elements:
* pre-delay
* post-delay
#### Example
```
{
    "name" : "vehicle.battery",
    "value" : 10,
    "post-delay" : 100
}
```
```
{
    "name" : "vehicle.battery",
    "value" : 10,
    "pre-delay" : 100
}
```
### Multiple signals step
#### Required elements:
* name
* value-start
* value-end
* value-step
* duration
#### Optional elements:
* pre-delay
* post-delay
#### Examples
```
{
    "name" : "vehicle.fuel",
    "value-start" : 0,
    "value-stop" : 100,
    "value-step" : 1,
    "duration" : 1000
}
```
```
{
    "timer1" :
        [
            {
                "name" : "vehicle.battery",
                "value" : 10
            },
            {
                "name" : "vehicle.ignition",
                "pre-delay" : 1000,
                "value" : 1
            },
            {
                "name" : "vehicle.ignition",
                "pre-delay" : 1000,
                "value" : 2,
                "post-delay" : 1000
            },
            {
                "name" : "vehicle.fuel",
                "value-start" : 0,
                "value-stop" : 100,
                "value-step" : 1,
                "duration" : 1000
            }
        ],
    "timer2" :
        [
            {
                "pre-delay" : 6000,
                "name": "vehicle.speed",
                "value-start" : 0,
                "value-stop" : 200,
                "value-step" : 1,
                "duration" : 10000
            }
        ]
}
```
