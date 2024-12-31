#pragma once

#include "CoreMinimal.h"
#include <PTG/Generator/Chunk_Type.h>

DECLARE_DELEGATE_OneParam(FChunkDelegate, FString);


class FChunkThread :
	public FRunnable
{

public:

	FChunkThread(FChunk _chunk) {
		chunk = _chunk;
		Thread = FRunnableThread::Create(this, TEXT("MyThread"));
	};

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;



	FRunnableThread* Thread;
	FChunk chunk;
	FPerlinParameters parameters;
	bool bShutdown = false;
	bool isOver = false;
	FChunkDelegate OnCalcOver;
};