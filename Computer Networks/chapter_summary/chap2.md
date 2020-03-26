### physical layer

notes from book *computer networks*

bandwith?
- width of frequency range w/o being strongly attenuated
    - usually, when received power has fallen by half, 3dB

twisting pair?
- why twisting? cause two parallel wires constitute a fine antenna
- signal as diff. in voltage between two wires

full duplex / half duplex

communicate over electrical wiring?
- data signal-> high frequency, power-> low frequency

optical fiber
- bottleneck: converting bet. electrical and optical signal

chromatic dispersion: light pulses spread out as propagate

ultraviolet light, X-ray, gamma ray: high frequency
- but hard to modulate, ot propagate through buildings

frequency hopping: frequency hops hundreds time a sec.

delayed wave may cancel direct wave: multipath fading

water absorbs short microwaves well

satellites: inherently broadcast media

digital modulation: converting between bits nad signals
- non-return-to-zero(inverted)
- manchester encoding: clock XOR data

4B/5B: no more than three consecutive zeros

scrambler: XORing data with pseudorandom sequence before transmission

QAM: better solution for error containment
- map bits to symbols s.t. adjacent symbols differ in only one position

CDMA: assume all chips sync. at the receiver, not approx. true in cellular networks

telephone system: local loops->trunks->switching offices
- local loops: analog information, narrow bandwith, attenuation and distortion of signals
- modem: converts between digital bits and analog signal

fiber to the home
- houses cannot hear each other-> request a time slot in the end office

FDM: requires analog circuitry cannot done by a computer, whereas TDM can

## TODO 2.7
