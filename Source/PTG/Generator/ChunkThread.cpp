#include "PTG/Generator/ChunkThread.h"
#include "PTG/Generator/PerlinNoise.h"


bool FChunkThread::Init() {
	/* Should the thread start? */
	return true;
}

uint32 FChunkThread::Run() {

	int _size = Chunk.Size;
	int _x = Chunk.Coords.X;
	int _y = Chunk.Coords.Y;

	for (int y = _y; y < _y + _size; y++) {
		for (int x = _x; x < _x + _size; x++) {
			FVertices vertex;
			vertex.Coords = FVector(x * 100, y * 100, (UPerlinNoise::GenerateOctavePerlinValue(x, y, Parameters.Octaves, Parameters.Persistence, Parameters.Frequency, Parameters.Seed)) * 100);
			/* Put Normal Vector Calc here*/
			Chunk.VertexArray.Add(vertex);
		}
	}
	return 1;
}

void FChunkThread::Exit() {
	bisOver = true;
	OnCalcOver.Execute(Chunk.Id);
}

void FChunkThread::Stop() {
	bShutdown = true;
}