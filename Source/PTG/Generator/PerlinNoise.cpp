
#include "PerlinNoise.h"
// Core includes
#include "CoreMinimal.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

// Texture-related includes
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "Containers/Array.h"

// File system and path handling
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include<random>
#include<cmath>

#include<chrono>

#define M_PI 3.141592
#define MIN_SIZE 257

APerlinNoise::APerlinNoise()
{

}

void APerlinNoise::SetSeed(int32 _Seed)
{
	seed = _Seed;
}



float APerlinNoise::DotValue(FVector gradient_vector, int x1, int y1, float x, float y)
{
    x = x - x1;
    y = y - y1;
    return (x * gradient_vector.X + y * gradient_vector.Y);
}

float APerlinNoise::GenerateOctavePerlinValue(float _x, float _y, int32 _octaves, float _persistence, float _frequency)
{
    float total = 0.0;
    float amplitude = 1.0;
    float maxValue = 0.0;

    for (int i = 0; i < _octaves; i++)
    {
        
        total += GeneratePerlinValue(_x, _y, i, _frequency) * amplitude;
        _frequency = _frequency * 2.0;
        maxValue += amplitude;
        amplitude *= _persistence;
    }
    total *= 3.f;

    return total / maxValue;
}



float APerlinNoise::GeneratePerlinValue(float _x, float _y, int _octave, float _frequency)
{
    
    // Scale input coordinates with frequency
    _x = _x * _frequency;
    _y = _y * _frequency;
    

    int x0 = FMath::FloorToInt(_x);
    int x1 = x0 + 1;
    int y0 = FMath::FloorToInt(_y);
    int y1 = y0 + 1;

    float sx = _x - x0;
    float sy = _y - y0;
    


    // Calculate dot products
    float a1 = DotValue(GenerateVector(x0, y0, _octave), x0, y0, _x, _y);
    float a2 = DotValue(GenerateVector(x1, y0, _octave), x1, y0, _x, _y);
    float a3 = DotValue(GenerateVector(x0, y1, _octave), x0, y1, _x, _y);
    float a4 = DotValue(GenerateVector(x1, y1, _octave), x1, y1, _x, _y);
    
    // Interpolate
    float b1 = FMath::Lerp(a1, a2, ((sx*6 - 15)*sx+10)*sx*sx*sx);
    float b2 = FMath::Lerp(a3, a4, ((sx * 6 - 15) * sx + 10) * sx*sx*sx);
    float value = FMath::Lerp(b1, b2, ((sy * 6 - 15) * sy + 10) * sy*sy*sy);
    
    return value;
}

FVector APerlinNoise::GenerateVector(int _x, int _y, int _octave)
{
    std::mt19937 generator(((seed * 1518 + _x) * 1794 + _y)*1816 + _octave);
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    double theta = 2 * M_PI * uniform(generator);
    double phi = acos(2 * uniform(generator) - 1);
    double x = sin(phi) * cos(theta);
    double y = sin(phi) * sin(theta);
    double z = cos(phi);
    return FVector(x, y, z);
}


UTexture2D* APerlinNoise::GeneratePerlinNoise2D(FVector2D TextureSize, FString AssetPath)
{
    FString PackagePath = TEXT("/Game/") + AssetPath;
    UPackage* Package = CreatePackage(*PackagePath);

    // Create texture name
    FString TextureName = FPaths::GetBaseFilename(AssetPath);

    // Create the texture
    UTexture2D* NoiseTexture = NewObject<UTexture2D>(
        Package,
        *TextureName,
        RF_Public | RF_Standalone | RF_MarkAsRootSet
    );

    // Create texture source
    int32 SizeX = FMath::RoundToInt(TextureSize.X);
    int32 SizeY = FMath::RoundToInt(TextureSize.Y);
    NoiseTexture->Source.Init(SizeX, SizeY, 1, 1, TSF_BGRA8);

    // Lock the texture source for editing
    uint8* MipData = NoiseTexture->Source.LockMip(0);

    // Generate a separate grid for each octave, but keep the same size

    // Generate and fill texture data
    for (int32 y = 0; y < SizeY; y++)
    {
        for (int32 x = 0; x < SizeX; x++)
        {
            // Generate noise value for this pixel
            float NoiseValue = GenerateOctavePerlinValue(x, y,octave,persistance,frequency);

            // Convert from [-1,1] to [0,255] range
            uint8 ColorIntensity = FMath::Clamp(((NoiseValue + 1.0f) / 2)*255,0,255);

            // Calculate pixel index
            int32 Index = ((y * SizeX) + x) * 4;

            // Set pixel color (BGRA format)
            MipData[Index] = ColorIntensity;     // Blue
            MipData[Index + 1] = ColorIntensity; // Green
            MipData[Index + 2] = ColorIntensity; // Red
            MipData[Index + 3] = 255;           // Alpha
        }
    }

    // Unlock the texture source
    NoiseTexture->Source.UnlockMip(0);

    // Set texture properties
    NoiseTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    NoiseTexture->MipGenSettings = TMGS_NoMipmaps;
    NoiseTexture->SRGB = false;
    NoiseTexture->UpdateResource();

    // Mark the package as dirty for saving
    Package->MarkPackageDirty();

    // Save the package
    FString PackageFileName = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    UPackage::SavePackage(Package, NoiseTexture, *PackageFileName, SaveArgs);

    return NoiseTexture;
}
