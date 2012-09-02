#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <libtcc.h>

#include "nodeutil.h"

#define STATE (Unwrap<NodeTCC>(args.This())->tcc_)

using namespace v8;
using namespace node;

void wrap_pointer_cb(char *data, void *hint) {
    //fprintf(stderr, "wrap_pointer_cb\n");
}

Handle<Value> WrapPointer(char *ptr, size_t length) {
    void *user_data = NULL;
    Buffer *buf = Buffer::New(ptr, length, wrap_pointer_cb, user_data);
    return buf->handle_;
}

Handle<Value> WrapPointer(void *ptr) {
    size_t size = 0;
    return WrapPointer(static_cast<char*>(ptr), size);
}


class NodeTCC : ObjectWrap {
public:
    TCCState * tcc_;

    static void Init(Handle<Object> target) {
        Local<FunctionTemplate> t = FunctionTemplate::New(NodeTCC::New);
        NODE_SET_PROTOTYPE_METHOD(t, "addFile", NodeTCC::add_file);
        NODE_SET_PROTOTYPE_METHOD(t, "addIncludePath", NodeTCC::add_include_path);
        NODE_SET_PROTOTYPE_METHOD(t, "addLibrary", NodeTCC::add_library);
        NODE_SET_PROTOTYPE_METHOD(t, "addLibraryPath", NodeTCC::add_library_path);
        NODE_SET_PROTOTYPE_METHOD(t, "addSysincludePath", NodeTCC::add_sysinclude_path);
        NODE_SET_PROTOTYPE_METHOD(t, "compileString", NodeTCC::compile_string);
        NODE_SET_PROTOTYPE_METHOD(t, "defineSymbol", NodeTCC::define_symbol);
        NODE_SET_PROTOTYPE_METHOD(t, "outputFile", NodeTCC::output_file);
        NODE_SET_PROTOTYPE_METHOD(t, "run", NodeTCC::run);
        NODE_SET_PROTOTYPE_METHOD(t, "setOutputType", NodeTCC::set_output_type);
        NODE_SET_PROTOTYPE_METHOD(t, "setWarning", NodeTCC::set_warning);
        NODE_SET_PROTOTYPE_METHOD(t, "undefineSymbol", NodeTCC::undefine_symbol);
        NODE_SET_PROTOTYPE_METHOD(t, "getSymbol", NodeTCC::get_symbol);
        NODE_SET_PROTOTYPE_METHOD(t, "relocate", NodeTCC::relocate);
        NODE_SET_PROTOTYPE_METHOD(t, "addSymbol", NodeTCC::add_symbol);
        SET_ENUM_VALUE(t, TCC_OUTPUT_MEMORY);
        SET_ENUM_VALUE(t, TCC_OUTPUT_EXE);
        SET_ENUM_VALUE(t, TCC_OUTPUT_DLL);
        SET_ENUM_VALUE(t, TCC_OUTPUT_OBJ);
        SET_ENUM_VALUE(t, TCC_OUTPUT_PREPROCESS);
        SET_ENUM_VALUE(t, TCC_OUTPUT_FORMAT_ELF);
        SET_ENUM_VALUE(t, TCC_OUTPUT_FORMAT_BINARY);
        SET_ENUM_VALUE(t, TCC_OUTPUT_FORMAT_COFF);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        target->Set(String::New("TCC"), t->GetFunction());
    }
    NodeTCC() {
        tcc_ = tcc_new();
    }
    ~NodeTCC() {
        tcc_delete(tcc_);
    }
    static Handle<Value> New(const Arguments& args) {
        HandleScope scope;

        if (!args.IsConstructCall())
            return args.Callee()->NewInstance();
        (new NodeTCC())->Wrap(args.Holder());
        return scope.Close(args.Holder());
    }

    static Handle<Value> add_file(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, fname);
        return scope.Close(Integer::New(tcc_add_file(STATE, *fname)));
    }
    static Handle<Value> add_include_path(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, path);
        return scope.Close(Integer::New(tcc_add_include_path(STATE, *path)));
    }
    static Handle<Value> add_library(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, lib);
        return scope.Close(Integer::New(tcc_add_library(STATE, *lib)));
    }
    static Handle<Value> add_library_path(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, libpath);
        return scope.Close(Integer::New(tcc_add_library_path(STATE, *libpath)));
    }
    static Handle<Value> add_sysinclude_path(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, path);
        return scope.Close(Integer::New(tcc_add_sysinclude_path(STATE, *path)));
    }
    static Handle<Value> compile_string(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, src);
        return scope.Close(Integer::New(tcc_compile_string(STATE, *src)));
    }
    static Handle<Value> define_symbol(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, sym);
        ARG_STR(1, val);
        tcc_define_symbol(STATE, *sym, *val);
        return scope.Close(Undefined());
    }
    static Handle<Value> output_file(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, filename);
        return scope.Close(Integer::New(tcc_output_file(STATE, *filename)));
    }
    static Handle<Value> run(const Arguments& args) {
        HandleScope scope;
        int argc = args.Length();
        char ** argv = new char*[args.Length()];
        for (int i=0; i<args.Length(); ++i) {
            String::Utf8Value buf(args[i]->ToString());
            char * cbuf = strdup(*buf);
            if (!cbuf) {
                for (int j=0; j<i; ++j) {
                    free(argv[j]);
                }
                delete [] argv;
                return ThrowException(Exception::Error(String::New("Cannot allocate memory.")));
            }
            argv[i] = cbuf;
        }
        int retval = tcc_run(STATE, argc, argv);
        for (int i=0; i<args.Length(); ++i) {
            free(argv[i]);
        }
        delete [] argv;
        return scope.Close(Integer::New(retval));
    }
    static Handle<Value> set_output_type(const Arguments& args) {
        HandleScope scope;
        ARG_INT(0, type);
        return scope.Close(Integer::New(tcc_set_output_type(STATE, type)));
    }
    static Handle<Value> set_warning(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, name);
        ARG_INT(1, val);
        return scope.Close(Integer::New(tcc_set_warning(STATE, *name, val)));
    }
    static Handle<Value> undefine_symbol(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, sym);
        tcc_undefine_symbol(STATE, *sym);
        return scope.Close(Undefined());
    }
    static Handle<Value> get_symbol(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, sym);
        void * ptr = tcc_get_symbol(STATE, *sym);
        return scope.Close(WrapPointer(ptr));
    }
    static Handle<Value> relocate(const Arguments& args) {
        HandleScope scope;
        return scope.Close(Integer::New(tcc_relocate(STATE)));
    }
    static Handle<Value> add_symbol(const Arguments& args) {
        HandleScope scope;
        ARG_STR(0, name);
        ARG_BUF(1, ptr);
        return scope.Close(Integer::New(tcc_add_symbol(STATE, *name, ptr)));
    }
};

extern "C"
void init(v8::Handle<v8::Object> target) {
    NodeTCC::Init(target);
}
NODE_MODULE(tcc, init)
