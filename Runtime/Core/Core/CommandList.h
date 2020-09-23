#pragma once

#include "CoreInc.h"

namespace XE
{
	struct CORE_API FComandStorage
	{
	public:
		FComandStorage() = default;
		FComandStorage(const FComandStorage & That) = default;
		FComandStorage(FComandStorage && That) noexcept = default;
		FComandStorage & operator = (const FComandStorage & That) = default;
		FComandStorage & operator=(const FComandStorage && That) noexcept
		{
			Reset();
			CommandBuffers = Move(That.CommandBuffers);
			CommandIndices = Move(That.CommandIndices);
			CommandNames = Move(That.CommandNames);
			return *this;
		}

	public:
		bool IsEmpty() const { return CommandIndices.IsEmpty(); }
		void Reset()
		{
			CommandBuffers.Resize(0);
			CommandIndices.Resize(0);
			CommandNames.Resize(0);
		}

		void * Alloc(const Char * Name, size_t Size)
		{
			byte_t * CommandBuffer = CommandBuffers.AddUninitialized(Size);
			uint32_t CommandIndex = uint32_t(CommandBuffer - CommandBuffers.GetData());
			CommandIndices.Add(CommandIndex);
			CommandNames.Add(Name);
			return CommandBuffer;
		}

		template<typename CommandT, typename ...ArgsT>
		void Flush(ArgsT ...Args)
		{
			for (uint32_t CommandIndex : CommandIndices)
			{
				byte_t * CommandBuffer = CommandBuffers.GetData() + CommandIndex;
				CommandT * QueueCommand = reinterpret_cast<CommandT *>(CommandBuffer);
				QueueCommand->Excute(Forward<ArgsT>(Args)...);
			}
		}

		void Append(const FComandStorage & Another)
		{
			CommandIndices.Add(Another.CommandIndices);
			CommandNames.Add(Another.CommandNames);
			CommandBuffers.Add(Another.CommandBuffers);
		}
		
	public:
		TVector<uint32_t> CommandIndices;
		TVector<const Char *> CommandNames;
		TVector<byte_t> CommandBuffers;
	};

	template<typename ...ArgsT>
	class TCommand
	{
	public:
		virtual ~TCommand() = default;
		virtual void Excute(ArgsT && ...Args) = 0;
	};

	template<typename LambdaT, typename ...ArgsT>
	class TLambdaCommand : public TCommand<ArgsT...>
	{
	public:
		TLambdaCommand(const Char * NameIn, LambdaT && Lambda) : Name(NameIn), Lambda(Move(Lambda)) {}

		void Excute(ArgsT && ...Args)
		{
			Lambda(Forward<ArgsT>(Args)...);
		}

		const Char * Name = nullptr;
		LambdaT Lambda;
	};

	template<typename ...ArgsT>
	class TCommandList
	{
		using CommandT = TCommand<ArgsT...>;
	public:
		TCommandList() = default;
		TCommandList(const TCommandList & That) = default;
		TCommandList(TCommandList && That) noexcept = default;
		TCommandList & operator=(const TCommandList & That) noexcept = default;
		TCommandList & operator=(const TCommandList && That) noexcept
		{
			Storage.Reset();
			Storage = Move(That.Storage);
			return *this;
		}

		void Flush(ArgsT && ...Args)
		{
			for (uint32_t CommandIndex : Storage.CommandIndices)
			{
				byte_t * CommandBuffer = Storage.CommandBuffers.GetData() + CommandIndex;
				CommandT * Command = reinterpret_cast<CommandT *>(CommandBuffer);
				Command->Excute(Forward<ArgsT>(Args)...);
			}
			Storage.Reset();
		}

		void Reset()
		{
			Storage.Reset();
		}
		
		void Append(const TCommandList & Another)
		{
			Storage.Append(Another.Storage);
		}

	public:
		template<typename LambdaT>
		TLambdaCommand<LambdaT, ArgsT...> & Excute(LambdaT && Lambda)
		{
			return Excute(nullptr, Forward<LambdaT>(Lambda));
		}

		template<typename LambdaT>
		TLambdaCommand<LambdaT, ArgsT...> & Excute(const Char * Name, LambdaT && Lambda)
		{
			using LambdaCommandT = TLambdaCommand<LambdaT, ArgsT...>;
			LambdaCommandT * Command = static_cast<LambdaCommandT *>(Storage.Alloc(Name, sizeof(LambdaCommandT)));
			new (Command) LambdaCommandT(Name, Forward<LambdaT>(Lambda));
			return *Command;
		}

		bool IsEmpty() const { return Storage.CommandIndices.IsEmpty(); }

	private:
		FComandStorage Storage;
	};
}
