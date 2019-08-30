# Level shifter

From https://electronics.stackexchange.com/questions/173297/how-does-a-bidirectional-level-shifter-work

When the 3.3V input is pulled down to 0V, the source is at 0V, the gate is at 3.3V and the MOSFET is (more or less for this particular part) 'on'.
Therefore the driver for the TX-3.3V sinks current from R1 and R2 and the output goes to slightly above the input voltage (because Rds(on) > 0 and some current goes through R2).
If you pull the TX-5V output low, then the body diode of the MOSFET conducts and brings the TX-3.3V input (and MOSFET source) down to the TX-5V low level plus 0.6V or so. If the MOSFET has low enough threshold voltage the channel conducts further reducing the source voltage and the situation is the same as with it driven from the other side (the MOSFET channel will conduct in either direction).
Without doing a detailed evaluation, I think this particular n-channel MOSFET is pretty marginal- I suggest picking something with a guaranteed lower Vgs(th) at 1mA, something more like the BSS138.