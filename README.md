node-tcc
========

TCC bindings for node.js. You can compile C code and run it on demand.

SYNOPSIS
--------

    var TCC = require('tcc'),
    var tcc = new TCC();
    tcc.compile_string('int main() { return 4649; }');
    var ret = tcc.run();
    # => 4649

INSTALATION
-----------

node-tcc requires libtcc, compiled with -fPIC. If you OS' libtcc does not compiled with -fPIC, you need to to compile by yourself.

And so, you need to run a conifgure script with following style.

  ./configure --extra-cflags="-fPIC"

FUTURE PLAN
------------

You may use a symbol from TCC by ffi.

