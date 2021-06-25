/**
 * @file    daemon.h
 * @author  Erik Kelemen <xkelem0100@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Simulation service
 * from template example: https://github.com/grpc/grpc/blob/master/examples/cpp/helloworld/greeter_async_server.cc
 */

#include <grpc++/grpc++.h>
#include <grpc/support/log.h>
#include "translate.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using translator::TranslateReply;
using translator::TranslateRequest;
using translator::Translator;

class Daemon final
{
public:
    ~Daemon();

    // There is no shutdown handling in this code.
    void Run(uint port);

private:
    // Class encompasing the state and logic needed to serve a request.
    class CallData
    {
    public:
        // Take in the "service" instance (in this case representing an asynchronous
        // server) and the completion queue "cq" used for asynchronous communication
        // with the gRPC runtime.
        CallData(Translator::AsyncService *service, ServerCompletionQueue *cq);

        void Proceed();

    private:
        // The means of communication with the gRPC runtime for an asynchronous
        // server.
        Translator::AsyncService *service_;
        // The producer-consumer queue where for asynchronous server notifications.
        ServerCompletionQueue *cq_;
        // Context for the rpc, allowing to tweak aspects of it such as the use
        // of compression, authentication, as well as to send metadata back to the
        // client.
        ServerContext ctx_;

        // What we get from the client.
        TranslateRequest request_;
        // What we send back to the client.
        TranslateReply reply_;

        // The means to get back to the client.
        ServerAsyncResponseWriter<TranslateReply> responder_;

        // Let's implement a tiny state machine with the following states.
        enum CallStatus
        {
            CREATE,
            PROCESS,
            FINISH
        };
        CallStatus status_; // The current serving state.
    };

    // This can be run in multiple threads if needed.
    void HandleRpcs();

    std::unique_ptr<ServerCompletionQueue> cq_;
    Translator::AsyncService service_;
    std::unique_ptr<Server> server_;
    uint port_;
};