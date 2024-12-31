#include "PTG/Generator/ChunkThread.h"
#include "PTG/Generator/PerlinNoise.h"


bool FChunkThread::Init() {
	/* Should the thread start? */
	return true;
}

uint32 FChunkThread::Run() {

	int _size = chunk.size;
	int _x = chunk.Coords.X;
	int _y = chunk.Coords.Y;

	for (int y = _y; y < _y + _size; y++) {
		for (int x = _x; x < _x + _size; x++) {
			FVertices vertex;
			vertex.Coords = FVector(x * 100, y * 100, (UPerlinNoise::GenerateOctavePerlinValue(x, y, parameters.octaves, parameters.persistence, parameters.frequency, parameters.seed)) * 100);
			/* Put Normal Vector Calc here*/
			chunk.vertexArray.Add(vertex);
		}
	}
	return 1;
}

void FChunkThread::Exit() {
	isOver = true;

	OnCalcOver.Execute(FString::SanitizeFloat(chunk.Coords.X) + FString::SanitizeFloat(chunk.Coords.Y));
}

void FChunkThread::Stop() {
	bShutdown = true;
}