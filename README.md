megahal-server
==============

HTTP server for Meghal conversation simulator


You'll need a couple things to start.  First, you'll need the Megahal simulator itself.  The latest is 9.1.1 even though the console says Megahal 8.  Then you'll need Mongoose (version 4.0 only) which is a tiny embdded HTTP server.  You should place these directly inside the meghal-server directory. Like this:

    $ git clone https://github.com/lerouxt/megahal-server.git
    $ cd megahal-server
    $ git clone https://github.com/pteichman/megahal.git
    $ git clone https://github.com/cesanta/mongoose.git
    $ ( cd mongoose && git checkout 4.0 )
    $ make

At this point you'll see some warnings but it'll compile and give you a megahal-server command.

Run it like so to use the default megahal brains:

    $ ./megahal-server --port=8080 --dir=megahal

TODO:
* Return JSON instead of text?
* What's up with saving brains periodically - does that work?
