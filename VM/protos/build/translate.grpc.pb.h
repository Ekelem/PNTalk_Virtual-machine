// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: translate.proto
#ifndef GRPC_translate_2eproto__INCLUDED
#define GRPC_translate_2eproto__INCLUDED

#include "translate.pb.h"

#include <grpc++/impl/codegen/async_stream.h>
#include <grpc++/impl/codegen/async_unary_call.h>
#include <grpc++/impl/codegen/method_handler_impl.h>
#include <grpc++/impl/codegen/proto_utils.h>
#include <grpc++/impl/codegen/rpc_method.h>
#include <grpc++/impl/codegen/service_type.h>
#include <grpc++/impl/codegen/status.h>
#include <grpc++/impl/codegen/stub_options.h>
#include <grpc++/impl/codegen/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace translator {

class Translator final {
 public:
  static constexpr char const* service_full_name() {
    return "translator.Translator";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    // Sends a greeting
    virtual ::grpc::Status Translate(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::translator::TranslateReply* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::translator::TranslateReply>> AsyncTranslate(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::translator::TranslateReply>>(AsyncTranslateRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::translator::TranslateReply>> PrepareAsyncTranslate(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::translator::TranslateReply>>(PrepareAsyncTranslateRaw(context, request, cq));
    }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::translator::TranslateReply>* AsyncTranslateRaw(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::translator::TranslateReply>* PrepareAsyncTranslateRaw(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status Translate(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::translator::TranslateReply* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::translator::TranslateReply>> AsyncTranslate(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::translator::TranslateReply>>(AsyncTranslateRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::translator::TranslateReply>> PrepareAsyncTranslate(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::translator::TranslateReply>>(PrepareAsyncTranslateRaw(context, request, cq));
    }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    ::grpc::ClientAsyncResponseReader< ::translator::TranslateReply>* AsyncTranslateRaw(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::translator::TranslateReply>* PrepareAsyncTranslateRaw(::grpc::ClientContext* context, const ::translator::TranslateRequest& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_Translate_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    // Sends a greeting
    virtual ::grpc::Status Translate(::grpc::ServerContext* context, const ::translator::TranslateRequest* request, ::translator::TranslateReply* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_Translate : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_Translate() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_Translate() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Translate(::grpc::ServerContext* context, const ::translator::TranslateRequest* request, ::translator::TranslateReply* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestTranslate(::grpc::ServerContext* context, ::translator::TranslateRequest* request, ::grpc::ServerAsyncResponseWriter< ::translator::TranslateReply>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_Translate<Service > AsyncService;
  template <class BaseClass>
  class WithGenericMethod_Translate : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_Translate() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_Translate() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Translate(::grpc::ServerContext* context, const ::translator::TranslateRequest* request, ::translator::TranslateReply* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_Translate : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_Translate() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler< ::translator::TranslateRequest, ::translator::TranslateReply>(std::bind(&WithStreamedUnaryMethod_Translate<BaseClass>::StreamedTranslate, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_Translate() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status Translate(::grpc::ServerContext* context, const ::translator::TranslateRequest* request, ::translator::TranslateReply* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedTranslate(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::translator::TranslateRequest,::translator::TranslateReply>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_Translate<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_Translate<Service > StreamedService;
};

}  // namespace translator


#endif  // GRPC_translate_2eproto__INCLUDED
