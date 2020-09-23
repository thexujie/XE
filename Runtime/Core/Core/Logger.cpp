#include "PCH.h"
#include "Logger.h"

#include <iostream>


#include "DateTime.h"
#include "IO/Directory.h"
#include "IO/Path.h"
#include "Platform/Win32/Win32.h"
#include "Threading/UniqueLock.h"

namespace XE
{
    const Char * LogLevelText[] = { Str("Dbg"), Str("Inf"), Str("War"), Str("Err") };
	
    class FNullLogger : public FLoggerStream
    {
    public:
        EError Write(ELogLevel Level, FStringView Category, FStringView String) override
        {
#ifdef _DEBUG
            FSystem::DebugOutput(Format(Str("[{}][{}] {}"), LogLevelText[AsIndex(Level)],  Category, String));
#endif
	        return EError::OK;
        }
        EError WriteLine(ELogLevel Level, FStringView Category, FStringView String) override
        {
#ifdef _DEBUG
            FSystem::DebugOutput(Format(Str("[{}][{}] {}"), LogLevelText[AsIndex(Level)], Category, String));
#endif
            return EError::OK;
        }
    };
	
    static TUniquePtr<FLogger> __GlobalLogger;
    static FMutex __GlobalLoggerMutex;
	
    FLoggerStream & Logger()
	{
        static FNullLogger NullLogger;
        if (!__GlobalLogger)
            return NullLogger;
        return *__GlobalLogger;
	}

 //   FLoggerStream::FLoggerStream(class FLogger & LoggerIn, ELogLevel LogLevelIn) : Logger(LoggerIn), LogLevel(LogLevelIn)
 //   {
 //   }

	//FLoggerStream::~FLoggerStream()
 //   {
 //       if (!Buffer.IsEmpty())
 //       {
 //           if (&Logger != nullptr)
 //           {
 //               Logger.WriteLine(LogLevel, Buffer);
 //           }
 //           Buffer.Clear();
 //       }
 //   }

    FLogger::FLogger()
    {
#ifdef _DEBUG
        NeedDebugOutput = true;
#endif
        WriteThread = FThread(&FLogger::Thread_Write, this);
    }

    FLogger::FLogger(FStringView FileName, ELogLevel LogLevelIn) : LogLevel(LogLevelIn)
    {
#ifdef _DEBUG
        NeedDebugOutput = true;
#endif
        Open(FileName);
        WriteThread = FThread(&FLogger::Thread_Write, this);
    }

    FLogger::~FLogger()
    {
        Close();
    }

    EError FLogger::Open(FStringView FileName)
    {
        if (FileName.IsEmpty())
            return EError::Args;

        FileStream.Open(FileName, EFileModes::Out);
        FileStream.Seek(EFilePosition::End, 0);
        return FileStream.IsValid() ? EError::OK : EError::IO;
    }

    void FLogger::Close()
    {
        if (WriteThread.Joinable())
        {
            WriteExiting = true;
            WriteCondition.NotifyAll();
            WriteThread.Join();
        }
        FileStream.Flush();
        FileStream.Close();
    }

    void FLogger::Flush()
    {
        FileStream.Flush();
    }

    EError FLogger::LogToBuffer(uint32_t ProcessId, uint32_t ThreadId, ELogLevel LogLevelThis, FStringView Category, FStringView String)
    {
        if (LineCurrent == Line)
            return EError::OK;

        if (!WriteThread.Joinable())
            return EError::OK;

        LineCurrent = Line;
        if (ProcessId == InvalidIndexU32)
            ProcessId = FSystem::ProcessId();

        if (ThreadId == InvalidIndexU32)
            ThreadId = FSystem::ThreadId();

        if (LogLevelThis >= LogLevel)
        {
			uint64_t TimeInMs = FDateTime::SystemNanoseconds() / 1000 / 1000;
            {
                TUniqueLock Lock(WriteMutex);
                LogItems.Emplace(LogItem{ LogLevelThis, uint64_t(TimeInMs), ProcessId, ThreadId, Category, String });
            }
            WriteCondition.NotifyAll();

#ifdef _DEBUG
            {
                TUniqueLock Lock(SyncMutex);
                SyncCondition.Wait(SyncMutex);
            }
#endif
        }
        return EError::OK;
    }

    EError FLogger::Write(ELogLevel Level, FStringView Category, FStringView String)
    {
        EError err = LogToBuffer(InvalidIndexU32, InvalidIndexU32, Level, Category, String);
        ++Line;
        return err;
    }

    EError FLogger::WriteLine(ELogLevel Level, FStringView Category, FStringView String)
    {
        EError err = LogToBuffer(InvalidIndexU32, InvalidIndexU32, Level, Category, String);
        ++Line;
        return err;
    }

    void FLogger::Thread_Write()
    {
        FSystem::SetThreadDescription(Str("Thread_Write"));
        //TUniqueLock Lock(WriteMutex);
        while (!WriteExiting)
        {
            WriteMutex.Lock();
            WriteCondition.Wait(WriteMutex, [this]() { return WriteExiting || !LogItems.IsEmpty(); });
            TVector<LogItem> LogItemsWriting = Move(LogItems);
            WriteMutex.Unlock();
        	
            if (LogItemsWriting.IsEmpty())
                break;

            for (auto & LogItem : LogItemsWriting)
            {
                if (LoggerProxy)
                    LoggerProxy(LoggerProxyOpaque, LogItem.LogLevel, LogItem.TimeInMs, LogItem.ProcessId, LogItem.ThreadId, LogItem.String.Data, uint32_t(LogItem.String.Size));
                else
                {
                    time_t ms = time_t(LogItem.TimeInMs / 1000);
                    tm tm;
                    localtime_s(&tm, &ms);

                    char temp[128];
                    int32_t nchars = _snprintf_s(temp, std::size(temp), "[%04d-%02d-%02d %02d:%02d:%02d %03d][PID:%-6d][TID:%-6d][%s][%s] ",
                        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                        tm.tm_hour, tm.tm_min, tm.tm_sec, uint32_t(LogItem.TimeInMs % 1000),
                        LogItem.ProcessId, LogItem.ThreadId,
                        (const char *)LogItem.Category.Data,
                        (const char *)LogLevelText[std::clamp((int32_t)LogItem.LogLevel, 0, (int32_t)std::size(LogLevelText))]);

                    FString log_text((const char8_t *)temp, nchars);
                    log_text += LogItem.String;

                    if (NeedDebugOutput)
                        FSystem::DebugOutput(log_text);

                    if (log_text.IsEmpty() || log_text[log_text.GetSize() - 1] != u8'\n')
                        log_text += LineTag;

                    if (NeedConsoleOutput)
                        std::wcout << Strings::ToStdWString(LogItem.String);
                	
                    FileStream.Write(log_text.Data, log_text.Size);
                }
            }
            FileStream.Flush();
#ifdef _DEBUG
            {
                TUniqueLock Lock(SyncMutex);
                SyncCondition.NotifyAll();
            }
#endif
        }
    }


    FGlobalLogger::FGlobalLogger()
    {
        TUniqueLock Lock(__GlobalLoggerMutex);
        if (!__GlobalLogger)
            __GlobalLogger = MakeUnique<FLogger>();
    }

    FGlobalLogger::FGlobalLogger(FStringView FileName, ELogLevel LogLevel)
    {
        FStringView LogFileDirectory = FPath::GetDirectory(FileName);
        if (!FDirectory::Exists(LogFileDirectory))
            FDirectory::Create(FPath::Combine(FPath::GetWorkingDirectory(), LogFileDirectory));
        TUniqueLock Lock(__GlobalLoggerMutex);
        if (!__GlobalLogger)
            __GlobalLogger = MakeUnique<FLogger>(FileName, LogLevel);
    }

    FGlobalLogger::~FGlobalLogger()
    {
        TUniqueLock Lock(__GlobalLoggerMutex);
        __GlobalLogger.Reset();
    }

    void FGlobalLogger::SetProxy(void * Opaque, FunLoggerProxy LoggerProxy)
    {
        TUniqueLock Lock(__GlobalLoggerMutex);
        if (__GlobalLogger)
            __GlobalLogger->SetProxy(Opaque, LoggerProxy);
    }
}
