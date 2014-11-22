Measure Your Typing Speed
=========================

There are “how fast can you type” tests online, e.g. [10 Fast
Fingers](http://10fastfingers.com/), but these measure how fast you
can type a random collection of words that you have no prior knowledge
of. – But how fast do you type in your typical activities? When you’re
entering commands at the shell prompt? When chatting on IRC? When
drafting an E-Mail or a LaTeX document?

This is a simple Linux Kernel providing a file `/proc/typespeed`
which, every time you read it (e.g. with `cat`), will give you your
current typing speed and the number of characters typed in total since
loading the module. Since I’m hardly ever typing at full speed for 60
seconds without pause, I decided to print out three intermediate “per
minute” stats:

1. Keypresses in last 10 seconds * 60 / 10
2. Keypresses in last 30 seconds * 60 / 30
3. Keypresses in last 60 seconds

Thus, you get three values, adjusted for “keys per minute”. The fourth
value will increase monotonically. The format is:

    10s-average SP 30s-average SP 60s-average SP total NL

E.g. right now, while I’m writing this README file, my file displays:

    $ cat /proc/typespeed
    336 272 222 2429

(Yeah, I know. It’s slow. Slower than I thought.)

WARNING
=======

This should be obvious, but I’ll say it anyway: This is a Kernel
module. One that registers itself to listen to keyboard and mouse
events. Be careful with this stuff. In particular, it’s probably a
very bad idea to just type `make` and `sudo insmod ./typespeed.ko`.
This could be malicious code… Sending your precious SSH
passwords to a public pastebin site. Or installing itself in the
initrd so that it can get your encrypted hard drive passphrase, then
pasting *that* to some pastebin site. Just saying.
