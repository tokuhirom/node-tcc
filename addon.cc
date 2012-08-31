#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <v8.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <libtcc.h>

#define REQ_EXT_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsExternal()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be an external"))); \
Local<External> VAR = Local<External>::Cast(args[I]);
#define REQ_STR_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsString()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be a string"))); \
String::Utf8Value VAR(args[I]->ToString());
#define REQ_OBJ_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsObject()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be a object"))); \
Local<Object> VAR = Local<Object>::Cast(args[I]);

#define STATE (Unwrap<NodeTCC>(args.This())->tcc_)

using namespace v8;
using namespace node;

class NodeTCC : ObjectWrap {
public:
    TCCState * tcc_;

    static void Init(Handle<Object> target) {
        Local<FunctionTemplate> t = FunctionTemplate::New(NodeTCC::New);
        NODE_SET_PROTOTYPE_METHOD(t, "compile_string", NodeTCC::compile_string);
        NODE_SET_PROTOTYPE_METHOD(t, "run", NodeTCC::run);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        target->Set(String::New("TCC"), t->GetFunction());
    }
    NodeTCC() {
        tcc_ = tcc_new();
    }
    static Handle<Value> New(const Arguments& args) {
        HandleScope scope;

        if (!args.IsConstructCall())
            return args.Callee()->NewInstance();
        (new NodeTCC())->Wrap(args.Holder());
        return scope.Close(args.Holder());
    }

    static Handle<Value> compile_string(const Arguments& args) {
        HandleScope scope;
        REQ_STR_ARG(0, src);
        return scope.Close(Integer::New(tcc_compile_string(STATE, *src)));
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
};

extern "C"
void init(v8::Handle<v8::Object> target) {
    NodeTCC::Init(target);
}
NODE_MODULE(tcc, init)
