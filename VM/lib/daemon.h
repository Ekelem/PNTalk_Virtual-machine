#include <grpc++/grpc++.h>
#include <grpc/support/log.h>
#include "simulate.grpc.pb.h"
#include "translate.grpc.pb.h"

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using virtualmachine::SimulateReply;
using virtualmachine::SimulateRequest;
using virtualmachine::Simulator;
using translator::TranslateReply;
using translator::TranslateRequest;
using translator::Translator;

class TranslateClient {
 public:
  explicit TranslateClient(std::shared_ptr<Channel> channel)
      : stub_(Translator::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  TranslateReply translate(const std::string& code) {
    // Data we are sending to the server.
    TranslateRequest request;
    request.set_code(code);

    // Container for the data we expect from the server.
    TranslateReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq;

    // Storage for the status of the RPC upon completion.
    Status status;

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    std::unique_ptr<ClientAsyncResponseReader<TranslateReply> > rpc(
        stub_->PrepareAsyncTranslate(&context, request, &cq));

    // StartCall initiates the RPC call
    rpc->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the integer 1.
    rpc->Finish(&reply, &status, (void*)1);
    void* got_tag;
    bool ok = false;
    // Block until the next result is available in the completion queue "cq".
    // The return value of Next should always be checked. This return value
    // tells us whether there is any kind of event or the cq_ is shutting down.
    GPR_ASSERT(cq.Next(&got_tag, &ok));

    // Verify that the result from "cq" corresponds, by its tag, our previous
    // request.
    GPR_ASSERT(got_tag == (void*)1);
    // ... and that the request was completed successfully. Note that "ok"
    // corresponds solely to the request for updates introduced by Finish().
    GPR_ASSERT(ok);

    // Act upon the status of the actual RPC.
    if (status.ok()) {
      return reply;
    } else {
        reply.set_status(status.error_code());
        reply.set_code(status.error_message());
        return reply;
    }
  }

 private:
  std::unique_ptr<Translator::Stub> stub_;
};

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
        CallData(Simulator::AsyncService *service, ServerCompletionQueue *cq, TranslateClient* trans);

        void Proceed();

    private:
        // The means of communication with the gRPC runtime for an asynchronous
        // server.
        Simulator::AsyncService *service_;
        // The producer-consumer queue where for asynchronous server notifications.
        ServerCompletionQueue *cq_;
        TranslateClient *translator_;
        // Context for the rpc, allowing to tweak aspects of it such as the use
        // of compression, authentication, as well as to send metadata back to the
        // client.
        ServerContext ctx_;

        // What we get from the client.
        SimulateRequest request_;
        // What we send back to the client.
        SimulateReply reply_;

        // The means to get back to the client.
        ServerAsyncResponseWriter<SimulateReply> responder_;

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
    Simulator::AsyncService service_;
    std::unique_ptr<Server> server_;
    std::unique_ptr<TranslateClient> translator_;
    uint port_;
};

