#include "PTG/Generation/Terrain/ChunkThread.h"
#include "PTG/Generation/Utils/PerlinNoise.h"

/**
 * @file ChunkThread.cpp
 * @brief Implementation of asynchronous chunk generation thread
 * @details Handles terrain calculation in a separate thread to avoid blocking the game thread
 */

bool FChunkThread::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Generating Chunk Init"));
	return true;
}

/**
 * @brief Main thread execution method for chunk generation
 * @return Thread completion status (1 for success)
 * @details Generates terrain vertices using Perlin noise in batches
 *          with sleep intervals to prevent thread hogging
 */
uint32 FChunkThread::Run()
{
	UE_LOG(LogTemp, Warning, TEXT("Generating Chunk Run"));
	int _size = Chunk.Size;
	int _x = Chunk.Coords.X;
	int _y = Chunk.Coords.Y;
	float min = 0.0f;
	float max = 0.0f;
	const int BatchSize = 100;
	int processedVertices = 0;
	const int32 totalVertices = _size * _size;
	TArray<FVertices> TempVertices; // Cur Vertices
	
	TempVertices.Reserve(_size * _size);

	for (int y= _y,y_scaled = _y*100; y < _y + _size; y++,y_scaled+=100)
	{
		for (int x = _x,x_scaled = _x*100; x < _x + _size; x++,x_scaled+=100)
		{
			//float Z = UPerlinNoise::GenerateOctavePerlinValue(x, y, Parameters.Octaves, Parameters.Persistence, Parameters.Frequency, Parameters.Seed) * Parameters.HeightFactor; //Old noise
			float Z =  100004.0 * UPerlinNoise::GenerateOctavePerlinSmoothed(x, y, Parameters.Octaves, Parameters.Persistence, Parameters.Frequency, Parameters.Seed, 3.0f, 0.9f, FVector2D(1.0f / 64.0f));

			FVertices vertex;
			vertex.Coords = FVector(x_scaled, y_scaled, Z);

			if (vertex.Coords.Z > max) { max = vertex.Coords.Z; }
			else if (vertex.Coords.Z < min) { min = vertex.Coords.Z; }
			vertex.Normal = FVector(0.0f, 0.0f, 1.0f);

			TempVertices.Add(vertex);
			processedVertices++;
			
			if (processedVertices % BatchSize == 0)
			{
				FPlatformProcess::Sleep(0.001f);
			}
		}
	}

	{
		FScopeLock Lock(&ChunkLock);
		Chunk.VertexArray.Append(TempVertices);
	}
	
	UE_LOG(LogTemp, Error, TEXT("min : %f"),min);
	UE_LOG(LogTemp, Error, TEXT("max : %f"),max);
	return 1;
}

/**
 * @brief Cleanup method called when thread completes
 * @details Broadcasts completion event and cleans up thread resources
 */
void FChunkThread::Exit()
{
	UE_LOG(LogTemp, Warning, TEXT("Generating Chunk Exit"));
	
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		bisOver = true;
		OnCalcOver.Broadcast(Chunk.Id, Chunk);

		delete Thread;
		Thread = nullptr;
		delete this;
	});
}

void FChunkThread::Stop()
{
	bShutdown = true;
}
