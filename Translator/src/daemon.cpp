// from template example: https://github.com/grpc/grpc/blob/master/examples/cpp/helloworld/greeter_async_server.cc

#include "../lib/daemon.h"
#include "../lib/global.h"
#include "../lib/lexAnalyzator.h"
#include "../lib/parse.h"

#include <future>
#include <unistd.h>

Daemon::~Daemon()
{
    server_->Shutdown();
    // Always shutdown the completion queue after the server.
    cq_->Shutdown();
}

// There is no shutdown handling in this code.
void Daemon::Run(uint port)
{
    port_ = port;
    std::string server_address("0.0.0.0:" + std::to_string(port));

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service_" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *asynchronous* service.
    builder.RegisterService(&service_);
    // Get hold of the completion queue used for the asynchronous communication
    // with the gRPC runtime.
    cq_ = builder.AddCompletionQueue();
    // Finally assemble the server.
    server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;

    // Proceed to the server's main loop.
    HandleRpcs();
}

Daemon::CallData::CallData(Translator::AsyncService *service, ServerCompletionQueue *cq)
    : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE)
{
    // Invoke the serving logic right away.
    Proceed();
}

void Daemon::CallData::Proceed()
{
    if (status_ == CREATE)
    {
        // Make this instance progress to the PROCESS state.
        status_ = PROCESS;

        // As part of the initial CREATE state, we *request* that the system
        // start processing SayHello requests. In this request, "this" acts are
        // the tag uniquely identifying the request (so that different CallData
        // instances can serve different requests concurrently), in this case
        // the memory address of this CallData instance.
        service_->RequestTranslate(&ctx_, &request_, &responder_, cq_, cq_,
                                  this);
    }
    else if (status_ == PROCESS)
    {
        // Spawn a new CallData instance to serve new clients while we process
        // the one for this CallData. The instance will deallocate itself as
        // part of its FINISH state.
        new CallData(service_, cq_);

        // The actual processing.
        std::size_t hash = std::hash<std::string>{}(request_.code());
        
        // define file paths in /tmp directory
        std::string inputCodePath = "/tmp/" + std::to_string(hash) + ".code";
        std::string outputCodePath = "/tmp/" + std::to_string(hash) + ".output";
        std::string errorCodePath = "/tmp/" + std::to_string(hash) + ".error";

        std::ofstream inputfile;
        inputfile.open(inputCodePath, std::ofstream::out | std::ofstream::trunc);
        inputfile << request_.code() << std::endl;
        inputfile.close();

        std::system(("./Translator2 -f " + inputCodePath + " > " + outputCodePath + " 2> " + errorCodePath).c_str());
        std::ifstream errorFile(errorCodePath);
        if (errorFile.is_open())
        {
            if (errorFile.peek() == std::ifstream::traits_type::eof())
            {
                std::ifstream outputFile(outputCodePath);
                if (outputFile.is_open())
                {
                    std::string code((std::istreambuf_iterator<char>(outputFile)),std::istreambuf_iterator<char>());
                    reply_.set_code(code);
                    reply_.set_status(200);
                }
                else
                {
                    reply_.set_status(500);
                }
            }
            else
            {
                std::string code((std::istreambuf_iterator<char>(errorFile)),std::istreambuf_iterator<char>());
                reply_.set_code(code);
                reply_.set_status(403);
            }       
        }

        /* Second start for semnatic analysis */
        

        // And we are done! Let the gRPC runtime know we've finished, using the
        // memory address of this instance as the uniquely identifying tag for
        // the event.
        status_ = FINISH;
        responder_.Finish(reply_, Status::OK, this);
    }
    else
    {
        GPR_ASSERT(status_ == FINISH);
        // Once in the FINISH state, deallocate ourselves (CallData).
        delete this;
    }
}

// This can be run in multiple threads if needed.
void Daemon::HandleRpcs()
{
    // Spawn a new CallData instance to serve new clients.
    new CallData(&service_, cq_.get());
    void *tag; // uniquely identifies a request.
    bool ok;
    while (true)
    {
        // Block waiting to read the next event from the completion queue. The
        // event is uniquely identified by its tag, which in this case is the
        // memory address of a CallData instance.
        // The return value of Next should always be checked. This return value
        // tells us whether there is any kind of event or cq_ is shutting down.
        GPR_ASSERT(cq_->Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<CallData *>(tag)->Proceed();
    }
}