#pragma once

#include "CoreMinimal.h"
#include "PTG/Generation/Terrain/ChunkData.h"

//////// DELEGATES ////////
DECLARE_MULTICAST_DELEGATE_TwoParams(FChunkDelegate, int64,FChunk);

//////// CLASS ////////
/// Thread handling async chunk generation
class FChunkThread : public FRunnable
{
public:
	//////// CONSTRUCTORS ////////
	FChunkThread(FChunk _chunk,FPerlinParameters _parameters, FPerlinParameters _biomeParameters) : Chunk(_chunk), Parameters(_parameters), BiomeParameters(_biomeParameters)
	{
		Thread = FRunnableThread::Create(this, TEXT("MyThread"), 0, TPri_BelowNormal);
	};

	//////// UNREAL LIFECYCLE ////////
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

	//////// FIELDS ////////
	/// Thread data
	FRunnableThread* Thread;
	FChunk Chunk;
	FPerlinParameters Parameters;
	FPerlinParameters BiomeParameters;
	bool bShutdown = false;
	bool bisOver = false;

	/// Delegates
	FChunkDelegate OnCalcOver;
	FCriticalSection ChunkLock;
};
