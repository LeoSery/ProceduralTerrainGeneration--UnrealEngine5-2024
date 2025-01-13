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
	/**
	 * @brief Constructor for chunk generation thread
	 * @param _chunk Chunk data to generate
	 * @param _parameters Terrain generation parameters
	 * @param _biomeParameters Biome generation parameters
	 * @details Creates and starts a new thread with below normal priority
	 */
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
