# PacketHandler
A TCP/IP stack to rebuild a TCP stream from wincap intercepted traffic

This is a ten year old project that wasn't meant to be published. It's only here so potential employers can see I have experience writing C++ and network protocol code. Back then there wasn't another library I could find to parse tcp/ip. It seemed everybody was using the tcp/ip stacks from operating systems. However you can't use that if you want to reconstruct a stream from intercepted packets.

I looked at the implementation in WireShark, but I do find an OOP representation of the layers cleaner. That being said, this wasn't meant for publication, so it's not polished, not documented or tested and not production ready. In the mean time there is better, more modern around. Have a look at the rust library [etherparse](https://crates.io/crates/etherparse).

In any case, as far as my projects go, no fully fledged readme and docs => not meant for you to use, no support, no warranty.
