#pragma once

#include "CoreInc.h"
#include "Thread.h"
#include "IO/FileStream.h"
#include "Threading/ConditionalVariable.h"
#include "Threading/Mutex.h"

namespace XE
{
    using FunLoggerProxy = TFunction<void(void * Opaque, uint32_t LogLevel, uint64_t TimeInMs, uint32_t ProcessId, uint32_t ThreadId, const Char * Data, uint32_t Length)>;
	
    class CORE_API FLogger : public FLoggerStream
    {
    public:
        FLogger();
        FLogger(FStringView FileName, ELogLevel LogLevelIn);
        FLogger(const FLogger &) = delete;
        FLogger(FLogger && Another) = delete;
        ~FLogger();

        void SetLogLevel(ELogLevel LogLevelIn) { LogLevel = LogLevelIn; }
        ELogLevel GetLogLevel() const { return LogLevel; }
        void SetProxy(void * Opaque, FunLoggerProxy LoggerProxyIn)
        {
	        LoggerProxyOpaque = Opaque;
        	LoggerProxy = LoggerProxyIn;
        }
        EError Open(FStringView FileName);
        void Close();
        void Flush();

        EError LogToBuffer(uint32_t ProcessId, uint32_t ThreadId, ELogLevel LogLevelThis, FStringView Category, FStringView String);

        EError Write(ELogLevel Level, FStringView Category, FStringView String);
        EError WriteLine(ELogLevel Level, FStringView Category, FStringView String);

    private:
        void Thread_Write();

    private:
        FMutex WriteMutex;
        FThread WriteThread;
        FConditionalVariable WriteCondition;
        std::atomic<bool> WriteExiting = false;
    	
#ifdef _DEBUG
        // write log synchronic on debug
        FMutex SyncMutex;
        FConditionalVariable SyncCondition;
#endif

        FAtomicBool NeedConsoleOutput = false;
        FAtomicBool NeedDebugOutput = false;

        ELogLevel LogLevel = ELogLevel::LogLevel_Dbg;
        FString LineTag = Str("\r\n");
        FFileStream FileStream;
        void * LoggerProxyOpaque = nullptr;
        FunLoggerProxy LoggerProxy;
        struct LogItem
        {
            ELogLevel LogLevel;
            uint64_t TimeInMs;
            uint32_t ProcessId;
            uint32_t ThreadId;
            FString Category;
            FString String;
        };
        TVector<LogItem> LogItems;
        int64_t Line = 0;
        int64_t LineCurrent = -1;
    };

    class CORE_API FGlobalLogger
    {
    public:
        FGlobalLogger();
        FGlobalLogger(FStringView FileName, ELogLevel LogLevel);
        ~FGlobalLogger();

        void SetProxy(void * Opaque, FunLoggerProxy LoggerProxy);
    };
}
