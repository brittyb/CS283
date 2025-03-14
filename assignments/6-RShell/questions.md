1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

When calling recv(), the technique used is to look for the EOF character to know if the output is fully received. If there is only a partial read it will continue reading until the EOF character is found.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A network protocol should define a character that indicates the end of a command. In our case the 0x04 character is used. If this is not handled correctly there may be partial reads and not everything will be recieved.

3. Describe the general differences between stateful and stateless protocols.

A stateful protocol stores information about previous interactions and a stateless protocol does not. Stateful will have more robust capabilities such as reliability, but will generally be slower with more overhead. Stateless will be faster, will have less overhead, and will not be fully reliable.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is stateless, so UDP is generally faster and has less overhead. It is ideal in situations where it is okay to lose a few things in the process.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

Sockets are the interface/abstraction provided by the operating system to enable applications to use network communications.
