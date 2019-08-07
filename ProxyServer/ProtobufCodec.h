#ifndef PROTOBUFCODE_H
#define PROTOBUFCODE_H

#include "muduo/net/Buffer.h"
#include "muduo/net/TcpConnection.h"
#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>

typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

class ProtobufCodec : muduo::noncopyable
{
public:

    enum ErrorCode
    {
        kNoError = 0,
        kInvalidLength,
        kCheckSumError,
        kInvalidNameLen,
        kUnknownMessageType,
        kParseError,
    };

    typedef std::function<void (const muduo::net::TcpConnectionPtr&,
                                const MessagePtr&,
                                muduo::Timestamp)> ProtobufMessageCallback;

    typedef std::function<void (const muduo::net::TcpConnectionPtr&,
                                muduo::net::Buffer*,
                                muduo::Timestamp,
                                ErrorCode)> ErrorCallback;

    explicit ProtobufCodec(const ProtobufMessageCallback& messageCb)
        : messageCallback_(messageCb),
          errorCallback_(defaultErrorCallback)
    {
    }

    ProtobufCodec(const ProtobufMessageCallback& messageCb, const ErrorCallback& errorCb)
        : messageCallback_(messageCb),
          errorCallback_(errorCb)
    {
    }

    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp receiveTime);

    void send(const muduo::net::TcpConnectionPtr& conn,
              const google::protobuf::Message& message)
    {
        // FIXME: serialize to TcpConnection::outputBuffer()
        muduo::net::Buffer buf;
        fillEmptyBuffer(&buf, message);
        conn->send(&buf);
    }

    static const muduo::string& errorCodeToString(ErrorCode errorCode);
    static void fillEmptyBuffer(muduo::net::Buffer* buf, const google::protobuf::Message& message);
    static google::protobuf::Message* createMessage(const std::string& type_name);
    static MessagePtr parse(const char* buf, int len, ErrorCode* errorCode);

private:
    static void defaultErrorCallback(const muduo::net::TcpConnectionPtr&,
                                     muduo::net::Buffer*,
                                     muduo::Timestamp,
                                     ErrorCode);

    ProtobufMessageCallback messageCallback_;
    ErrorCallback errorCallback_;

    const static int kHeaderLen = sizeof(int32_t);
    const static int kMinMessageLen = 2*kHeaderLen + 2; // nameLen + typeName + checkSum
    const static int kMaxMessageLen = 64*1024*1024; // same as codec_stream.h kDefaultTotalBytesLimit
};

#endif
