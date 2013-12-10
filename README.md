megahal-server
==============

HTTP server for Meghal conversation simulator


You'll need a couple things to start.  First, you'll need the Megahal simulator itself.  The latest is 9.1.1 even though the console says Megahal 8.  Then you'll need Mongoose (version 4.0 only) which is a tiny embdded HTTP server.  You should placed these directly inside the meghal-server directory. Like this:

    $ git clone https://github.com/pteichman/megahal.git
    $ git clone https://github.com/cesanta/mongoose.git
    $ ( cd mongoose && git checkout 4.0 )
    $ Make

At this point you'll see a lot of warnings but it'll compile and give you a megahal-server command.

Megahal will load brains from the present working directory. Best way to make this work:

    $ cd brains-directory-of-your-choice
    $ path/to/megahal-server

TODO:
* Return JSON instead of text
* Pass text in URL instead of query parameter
* Port number in cmdline params
* What's up with saving brains periodically - does that work?
* Run as service instead of interactive
* Parameter that controls brain directory
