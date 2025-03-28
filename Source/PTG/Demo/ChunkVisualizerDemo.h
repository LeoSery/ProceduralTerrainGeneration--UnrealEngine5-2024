#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PTG/Generation/Terrain/ChunkData.h"
#include "PTG/Generation/Utils/PerlinNoise.h"
#include "ChunkVisualizerDemo.generated.h"

UENUM(BlueprintType)
enum class EVisualizationStage : uint8
{
    NoVisualization,
    GridPoints,
    PerlinHeight,
    Triangulation,
    TriangulatedMesh,
    FullMesh
};

UCLASS()
class PTG_API AChunkVisualizerDemo : public AActor
{
    GENERATED_BODY()
    
public:
    AChunkVisualizerDemo();
    
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // Méthode pour passer à l'étape suivante
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void NextStage();
    
    // Méthode pour revenir à l'étape précédente
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void PreviousStage();
    
    // Méthode pour définir directement une étape
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SetStage(EVisualizationStage NewStage);
    
    // Méthode pour générer un nouveau chunk
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void GenerateNewChunk();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Visualization")
    UProceduralMeshComponent* MeshComponent;
    
    UPROPERTY(VisibleAnywhere, Category = "Visualization")
    USceneComponent* PointsContainer;
    
    UPROPERTY(EditAnywhere, Category = "Visualization")
    int32 ChunkSize = 32;
    
    UPROPERTY(EditAnywhere, Category = "Visualization")
    float PointSize = 10.0f;
    
    UPROPERTY(EditAnywhere, Category = "Visualization")
    UMaterial* PointMaterial;
    
    UPROPERTY(EditAnywhere, Category = "Visualization")
    UMaterial* WireframeMaterial;

    UPROPERTY(EditAnywhere, Category = "Visualization")
    UMaterial* BaseShapeMaterial;
    
    UPROPERTY(EditAnywhere, Category = "Visualization")
    UMaterial* FinalMaterial;
    
    UPROPERTY(EditAnywhere, Category = "Terrain Generation")
    FPerlinParameters NoiseParameters;
    
    UPROPERTY(EditAnywhere, Category = "Terrain Generation")
    FPerlinParameters BiomeParameters;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    EVisualizationStage CurrentStage;
    
    TArray<FVector> VertexPositions;
    TArray<UStaticMeshComponent*> PointVisualizers;
    TArray<int32> Triangles;
    
private:
    // Fonctions d'aide pour chaque étape
    void CreateGridPoints();
    void ApplyPerlinNoise();
    void CreateTriangulation();
    void ApplyFinalMesh();
    void ClearVisualization();
    
    // Utilitaires
    void UpdateVisualization();
    int32 GetVertexIndex(int32 X, int32 Y) const { return X + Y * ChunkSize; }

    // Step one animations
    FTimerHandle PointsAnimationTimer;
    int32 CurrentWave;
    bool bAnimatingPoints;
    void CreatePointsWave(int32 WaveIndex);
    void StartPointsAnimation();

    // Step two animations
    FTimerHandle HeightAnimationTimer;
    bool bAnimatingHeight;
    float HeightAnimationProgress;
    TArray<FVector> FlatPositions;
    TArray<FVector> NoisePositions;
    void UpdateHeightAnimation();
    void StartHeightAnimation();
    void FinishHeightAnimation();

    //Step three animations
    FTimerHandle WireframeAnimationTimer;
    bool bAnimatingWireframe;
    float WireframeAnimationProgress;
    float HighestPoint;
    float LowestPoint;
    void UpdateWireframeAnimation();
    void StartWireframeAnimation();
    void FinishWireframeAnimation();
    
    // Step four animations
    FTimerHandle MaterialFadeAnimationTimer;
    bool bAnimatingMaterialFade;
    float MaterialFadeProgress;
    void StartMaterialFadeAnimation();
    void UpdateMaterialFadeAnimation();
    void FinishMaterialFadeAnimation();

    void ApplyTriangulatedMesh();
    void ApplyFinalMeshWithFade();
};