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

METHODS
-------

Following methods are available(It's all of methods from tcc).
For more details, you can read a source code(so, you can read it, since you are C hacker :P )

    add_file
    add_include_path
    add_library
    add_library_path
    add_sysinclude_path
    compile_string
    define_symbol
    output_file
    run
    set_output_type
    set_warning
    undefine_symbol
    get_symbol
    relocate
    add_symbol

node-tcc and node-ffi
---------------------

You can use node-tcc with node-ffi to load compiled code as a method for node.js.

    var here = require('here').here,
        ffi = require('ffi'),
        TCC = require('tcc'),
        tcc = new TCC(),
        assert = require('assert');

    tcc.compileString(''+here(/*
        int fib(int n) {
            if (n <= 2) { return 1; }
            return fib(n-1) + fib(n-2);
        }
    */));
    tcc.relocate();
    var fib_ptr = tcc.getSymbol('fib');
    assert(!fib_ptr.isNull());
    var fib = ffi.ForeignFunction(fib_ptr, 'int', ['int']);
    console.log(fib(35));

