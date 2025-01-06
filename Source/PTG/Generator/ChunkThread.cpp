#include "PTG/Generator/ChunkThread.h"
#include "PTG/Generator/PerlinNoise.h"

bool FChunkThread::Init()
{
	//UE_LOG(LogTemp, Warning, TEXT("Generating Chunk Init"));
	return true;
}

uint32 FChunkThread::Run()
{
	//UE_LOG(LogTemp, Warning, TEXT("Generating Chunk Run"));
	int _size = Chunk.Size;
	int _x = Chunk.Coords.X;
	int _y = Chunk.Coords.Y;
	const int BatchSize = 100;
	int processedVertices = 0;
	const int32 totalVertices = _size * _size;
	TArray<FVertices> TempVertices; // Cur Vertices
	
	TempVertices.Reserve(_size * _size);

	for (int y= _y,y_scaled = _y*100; y < _y + _size; y++,y_scaled+=100)
	{
		for (int x = _x,x_scaled = _x*100; x < _x + _size; x++,x_scaled+=100)
		{
			FVertices vertex;
			vertex.Coords = FVector(x_scaled, y_scaled, (FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0, 1), UPerlinNoise::GenerateOctavePerlinValue(x, y, Parameters.Octaves, Parameters.Persistence, Parameters.Frequency, Parameters.Seed)))
				* (Parameters.HeightFactor
					* FMath::GetMappedRangeValueClamped(FVector2D(-1,1),FVector2D(0,2), UPerlinNoise::GenerateOctavePerlinValue(x, y, BiomeParameters.Octaves, BiomeParameters.Persistence, BiomeParameters.Frequency, BiomeParameters.Seed))
					* BiomeParameters.HeightFactor));
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
	
	return 1;
}

void FChunkThread::Exit()
{
	//UE_LOG(LogTemp, Warning, TEXT("Generating Chunk Exit"));
	
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
	bisOver = true;
	OnCalcOver.Broadcast(Chunk.Id, Chunk);
	});
}

void FChunkThread::Stop()
{
	bShutdown = true;
}