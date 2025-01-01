#pragma once

#include "CoreMinimal.h"
#include "ChunkData.h"

DECLARE_DELEGATE_OneParam(FChunkDelegate, int64);


class FChunkThread : public FRunnable
{
public:

	FChunkThread(FChunk _chunk) : Chunk(_chunk) {
		Thread = FRunnableThread::Create(this, TEXT("MyThread"));
	};

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;
	
	FRunnableThread* Thread;
	FChunk Chunk;
	FPerlinParameters Parameters;
	bool bShutdown = false;
	bool bisOver = false;
	FChunkDelegate OnCalcOver;
};
