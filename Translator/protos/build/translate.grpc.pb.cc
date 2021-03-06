// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: translate.proto

#include "translate.pb.h"
#include "translate.grpc.pb.h"

#include <grpc++/impl/codegen/async_stream.h>
#include <grpc++/impl/codegen/async_unary_call.h>
#include <grpc++/impl/codegen/channel_interface.h>
#include <grpc++/impl/codegen/client_unary_call.h>
#include <grpc++/impl/codegen/method_handler_impl.h>
#include <grpc++/impl/codegen/rpc_service_method.h>
#include <grpc++/impl/codegen/service_type.h>
#include <grpc++/impl/codegen/sync_stream.h>
namespace translator {

static const char* Translator_method_names[] = {
  "/translator.Translator/Translate",
};

std::unique_ptr< Translator::Stub> Translator::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  std::unique_ptr< Translator::Stub> stub(new Translator::Stub(channel));
  return stub;
}

Translator::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Translate_(Translator_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Translator::Stub::Translate(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::translator::TranslateReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Translate_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::translator::TranslateReply>* Translator::Stub::AsyncTranslateRaw(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::translator::TranslateReply>::Create(channel_.get(), cq, rpcmethod_Translate_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::translator::TranslateReply>* Translator::Stub::PrepareAsyncTranslateRaw(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::translator::TranslateReply>::Create(channel_.get(), cq, rpcmethod_Translate_, context, request, false);
}

Translator::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Translator_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Translator::Service, ::translator::TranslateRequest, ::translator::TranslateReply>(
          std::mem_fn(&Translator::Service::Translate), this)));
}

Translator::Service::~Service() {
}

::grpc::Status Translator::Service::Translate(::grpc::ServerContext* context, const ::translator::TranslateRequest* request, ::translator::TranslateReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace translator

