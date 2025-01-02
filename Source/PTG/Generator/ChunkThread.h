#pragma once

#include "CoreMinimal.h"
#include "ChunkData.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FChunkDelegate, int64,FChunk);


class FChunkThread : public FRunnable
{
public:

	FChunkThread(FChunk _chunk,FPerlinParameters _parameters) : Chunk(_chunk), Parameters(_parameters) {
		Thread = FRunnableThread::Create(this, TEXT("MyThread"), 0, TPri_BelowNormal);
	
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
