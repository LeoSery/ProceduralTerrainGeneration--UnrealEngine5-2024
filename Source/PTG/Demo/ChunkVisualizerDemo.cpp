#include "ChunkVisualizerDemo.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AChunkVisualizerDemo::AChunkVisualizerDemo()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Création des composants de base
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    
    MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    MeshComponent->SetupAttachment(RootComponent);
    
    PointsContainer = CreateDefaultSubobject<USceneComponent>(TEXT("PointsContainer"));
    PointsContainer->SetupAttachment(RootComponent);
    
    // Configuration identique à votre générateur principal
    NoiseParameters.Octaves = 5;
    NoiseParameters.Frequency = 0.003f;
    NoiseParameters.Persistence = 0.5f;
    NoiseParameters.Seed = 420;
    NoiseParameters.HeightFactor = 50000;
    
    // Ajouter les paramètres de biome si nécessaire
    BiomeParameters.Octaves = 1;
    BiomeParameters.Frequency = 0.0025f;
    BiomeParameters.Persistence = 0.5f;
    BiomeParameters.Seed = 420;
    BiomeParameters.HeightFactor = 100;
    
    CurrentStage = EVisualizationStage::NoVisualization;
}

void AChunkVisualizerDemo::BeginPlay()
{
    Super::BeginPlay();
    
    // Activer la réception des entrées pour l'acteur
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        EnableInput(PC);
    }
}

void AChunkVisualizerDemo::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AChunkVisualizerDemo::NextStage()
{
    int32 NextStageValue = static_cast<int32>(CurrentStage) + 1;
    if (NextStageValue <= static_cast<int32>(EVisualizationStage::FullMesh))
    {
        CurrentStage = static_cast<EVisualizationStage>(NextStageValue);
        UpdateVisualization();
    }
}

void AChunkVisualizerDemo::PreviousStage()
{
    int32 PrevStageValue = static_cast<int32>(CurrentStage) - 1;
    if (PrevStageValue >= static_cast<int32>(EVisualizationStage::NoVisualization))
    {
        CurrentStage = static_cast<EVisualizationStage>(PrevStageValue);
        UpdateVisualization();
    }
}

void AChunkVisualizerDemo::SetStage(EVisualizationStage NewStage)
{
    if (CurrentStage != NewStage)
    {
        CurrentStage = NewStage;
        UpdateVisualization();
    }
}

void AChunkVisualizerDemo::UpdateVisualization()
{
    switch (CurrentStage)
    {
    case EVisualizationStage::GridPoints:
        ClearVisualization();
        CreateGridPoints();
        break;
            
    case EVisualizationStage::PerlinHeight:
        ApplyPerlinNoise();
        break;
            
    case EVisualizationStage::Triangulation:
        CreateTriangulation();
        break;
            
    case EVisualizationStage::TriangulatedMesh:
            ApplyTriangulatedMesh();
        break;
            
    case EVisualizationStage::FullMesh:
            for (auto* PointComp : PointVisualizers)
            {
                if (PointComp)
                {
                    PointComp->SetVisibility(false);
                }
            }
        ApplyFinalMeshWithFade();
        break;
            
    default:
        break;
    }
}

void AChunkVisualizerDemo::CreatePointsWave(int32 WaveIndex)
{
    // Le nombre total de vagues est la somme des dimensions - 1
    // (pour une grille 32x32, il y aura 63 vagues)
    int32 TotalWaves = (ChunkSize * 2) - 1;
    
    if (WaveIndex >= TotalWaves)
    {
        // Animation terminée
        bAnimatingPoints = false;
        return;
    }
    
    // Chargement du mesh de sphère
    UStaticMesh* PointMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (!PointMesh)
    {
        return;
    }
    
    // Pour chaque vague diagonale, créer les points
    // Une vague comprend tous les points où (x + y) est constant
    for (int32 x = 0; x <= WaveIndex; x++)
    {
        int32 y = WaveIndex - x;
        
        // Vérifier que les coordonnées sont dans les limites de la grille
        if (x < ChunkSize && y < ChunkSize)
        {
            int32 Index = GetVertexIndex(x, y);
            
            // Création du visualiseur de point
            UStaticMeshComponent* PointComp = NewObject<UStaticMeshComponent>(this);
            PointComp->SetStaticMesh(PointMesh);
            PointComp->SetRelativeScale3D(FVector(PointSize * 0.01f));
            if (PointMaterial)
            {
                PointComp->SetMaterial(0, PointMaterial);
            }
            PointComp->SetupAttachment(PointsContainer);
            PointComp->RegisterComponent();
            PointComp->SetWorldLocation(VertexPositions[Index]);
            
            PointVisualizers.Add(PointComp);
        }
    }
    
    // Passer à la vague suivante
    CurrentWave++;
    
    // Programmer la création de la prochaine vague
    if (CurrentWave < TotalWaves)
    {
        float WaveInterval = 0.025f;
        GetWorld()->GetTimerManager().SetTimer(
            PointsAnimationTimer,
            FTimerDelegate::CreateUObject(this, &AChunkVisualizerDemo::CreatePointsWave, CurrentWave),
            WaveInterval,
            false
        );
    }
}

void AChunkVisualizerDemo::StartPointsAnimation()
{
    CurrentWave = 0;
    bAnimatingPoints = true;
    
    // Intervalle entre chaque vague diagonale (en secondes)
    float WaveInterval = 0.025f; // Ajustez pour une animation plus rapide ou plus lente
    
    GetWorld()->GetTimerManager().SetTimer(
        PointsAnimationTimer,
        FTimerDelegate::CreateUObject(this, &AChunkVisualizerDemo::CreatePointsWave, CurrentWave),
        WaveInterval,
        false
    );
}

void AChunkVisualizerDemo::UpdateHeightAnimation()
{
    if (!bAnimatingHeight)
        return;
    
    // Progression de l'animation
    HeightAnimationProgress += 0.02f; // Ajustez pour une animation plus rapide ou plus lente
    
    // Animation terminée?
    if (HeightAnimationProgress >= 1.0f)
    {
        FinishHeightAnimation();
        return;
    }
    
    // Courbe d'accélération/décélération pour un mouvement plus naturel
    // (accélération au début, décélération à la fin)
    float EasedProgress = FMath::SmoothStep(0.0f, 1.0f, HeightAnimationProgress);
    
    // Mettre à jour chaque point par interpolation
    for (int32 i = 0; i < VertexPositions.Num(); i++)
    {
        // Interpolation entre position plate et position avec bruit
        VertexPositions[i].Z = FMath::Lerp(FlatPositions[i].Z, NoisePositions[i].Z, EasedProgress);
        
        // Mettre à jour la position du visualiseur
        if (i < PointVisualizers.Num() && PointVisualizers[i])
        {
            PointVisualizers[i]->SetWorldLocation(VertexPositions[i]);
        }
    }
}

void AChunkVisualizerDemo::StartHeightAnimation()
{
    HeightAnimationProgress = 0.0f;
    bAnimatingHeight = true;
    
    // Intervalle de mise à jour (en secondes)
    float UpdateInterval = 0.01f; // 100 fois par seconde
    
    GetWorld()->GetTimerManager().SetTimer(
        HeightAnimationTimer,
        this,
        &AChunkVisualizerDemo::UpdateHeightAnimation,
        UpdateInterval,
        true // Répéter automatiquement
    );
}

void AChunkVisualizerDemo::FinishHeightAnimation()
{
    bAnimatingHeight = false;
    GetWorld()->GetTimerManager().ClearTimer(HeightAnimationTimer);
    
    // Assurer que tous les points sont à leur position finale
    VertexPositions = NoisePositions;
    
    for (int32 i = 0; i < VertexPositions.Num(); i++)
    {
        if (i < PointVisualizers.Num() && PointVisualizers[i])
        {
            PointVisualizers[i]->SetWorldLocation(VertexPositions[i]);
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Height animation completed"));
}

void AChunkVisualizerDemo::UpdateWireframeAnimation()
{
    if (!bAnimatingWireframe)
        return;
    
    // Progression de l'animation
    WireframeAnimationProgress += 0.01f; // Ajustez pour une animation plus rapide ou plus lente
    
    // Animation terminée?
    if (WireframeAnimationProgress >= 1.0f)
    {
        FinishWireframeAnimation();
        return;
    }
    
    // Pour une révélation progressive de haut en bas, nous inversons le pourcentage
    // pour commencer par le plus haut et descendre
    float InversedProgress = 1.0f - WireframeAnimationProgress;
    
    // Calculer la hauteur de coupure actuelle
    float CurrentCutoff = HighestPoint - (HighestPoint - LowestPoint) * (1.0f - InversedProgress);
    
    // Mettre à jour le matériau dynamique
    UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(0));
    if (DynamicMaterial)
    {
        DynamicMaterial->SetScalarParameterValue("HeightCutoff", CurrentCutoff);
    }
    else
    {
        // Si nous n'avons pas le paramètre dans le matériau, essayons une approche alternative
        // Recréer le mesh avec les triangles visibles jusqu'à une certaine hauteur
        
        // Dans ce cas, nous allons créer des sections de mesh temporaires
        // et les afficher progressivement
        
        // Cette partie est plus complexe et nécessite des modifications plus importantes
        // du matériau et de la logique - je vais proposer une solution alternative
    }
}

void AChunkVisualizerDemo::StartWireframeAnimation()
{
    WireframeAnimationProgress = 0.0f;
    bAnimatingWireframe = true;
    
    // Intervalle de mise à jour (en secondes)
    float UpdateInterval = 0.01f; // 100 fois par seconde
    
    GetWorld()->GetTimerManager().SetTimer(
        WireframeAnimationTimer,
        this,
        &AChunkVisualizerDemo::UpdateWireframeAnimation,
        UpdateInterval,
        true // Répéter automatiquement
    );
}

void AChunkVisualizerDemo::FinishWireframeAnimation()
{
    bAnimatingWireframe = false;
    GetWorld()->GetTimerManager().ClearTimer(WireframeAnimationTimer);
    
    // S'assurer que tout le mesh est visible
    UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(0));
    if (DynamicMaterial)
    {
        DynamicMaterial->SetScalarParameterValue("HeightCutoff", LowestPoint - 100.0f); // Bien en-dessous du point le plus bas
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Wireframe animation completed"));
}

void AChunkVisualizerDemo::StartMaterialFadeAnimation()
{
    MaterialFadeProgress = 0.0f;
    bAnimatingMaterialFade = true;
    
    float UpdateInterval = 0.01f;
    
    GetWorld()->GetTimerManager().SetTimer(
        MaterialFadeAnimationTimer,
        this,
        &AChunkVisualizerDemo::UpdateMaterialFadeAnimation,
        UpdateInterval,
        true
    );
}

void AChunkVisualizerDemo::UpdateMaterialFadeAnimation()
{
    if (!bAnimatingMaterialFade)
        return;
    
    MaterialFadeProgress += 0.01f;
    
    if (MaterialFadeProgress >= 1.0f)
    {
        FinishMaterialFadeAnimation();
        return;
    }
    
    // Calculer la hauteur de révélation actuelle
    float CurrentRevealHeight = HighestPoint - (HighestPoint - LowestPoint) * MaterialFadeProgress;
    
    // Mettre à jour le paramètre de révélation
    UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(0));
    if (DynamicMaterial)
    {
        DynamicMaterial->SetScalarParameterValue("RevealHeight", CurrentRevealHeight);
    }
}

void AChunkVisualizerDemo::FinishMaterialFadeAnimation()
{
    bAnimatingMaterialFade = false;
    GetWorld()->GetTimerManager().ClearTimer(MaterialFadeAnimationTimer);
    
    UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(0));
    if (DynamicMaterial)
    {
        DynamicMaterial->SetScalarParameterValue("Opacity", 1.0f);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Material fade animation completed"));
}

void AChunkVisualizerDemo::ApplyTriangulatedMesh()
{
    if (Triangles.Num() == 0)
    {
        CreateTriangulation();
    }
    
    // Créer un matériau gris uni (différent du wireframe)
    UMaterialInstanceDynamic* SimpleMeshMaterial = nullptr;
    
    // Si tu as un matériau de base pour cette étape
    if (FinalMaterial) // Utilisez le FinalMaterial comme base
    {
        // Créer un matériau gris simple
        SimpleMeshMaterial = UMaterialInstanceDynamic::Create(BaseShapeMaterial, this);
        if (SimpleMeshMaterial)
        {
            SimpleMeshMaterial->SetScalarParameterValue("RevealHeight", HighestPoint + 1000.0f);
        }
    }
    
    // Appliquer le matériau
    if (SimpleMeshMaterial)
    {
        MeshComponent->SetMaterial(0, SimpleMeshMaterial);
    }
}

void AChunkVisualizerDemo::ApplyFinalMeshWithFade()
{
    UE_LOG(LogTemp, Warning, TEXT("Starting material reveal animation"));
    
    // Préparer le matériau final
    UMaterialInstanceDynamic* FinalMaterialInstance = nullptr;
    
    if (FinalMaterial)
    {
        FinalMaterialInstance = UMaterialInstanceDynamic::Create(FinalMaterial, this);
        // Commencer avec tout caché (valeur très haute)
        FinalMaterialInstance->SetScalarParameterValue("RevealHeight", HighestPoint + 1000.0f);
        MeshComponent->SetMaterial(0, FinalMaterialInstance);
    }
    
    // Démarrer l'animation de révélation
    StartMaterialFadeAnimation();
}

void AChunkVisualizerDemo::GenerateNewChunk()
{
    if (bAnimatingPoints)
    {
        GetWorld()->GetTimerManager().ClearTimer(PointsAnimationTimer);
        bAnimatingPoints = false;
    }
    
    if (bAnimatingHeight)
    {
        GetWorld()->GetTimerManager().ClearTimer(HeightAnimationTimer);
        bAnimatingHeight = false;
    }

    if (bAnimatingWireframe)
    {
        GetWorld()->GetTimerManager().ClearTimer(WireframeAnimationTimer);
        bAnimatingWireframe = false;
    }
    
    // Réinitialise avec une nouvelle seed
    NoiseParameters.Seed = FMath::RandRange(1, 10000);
    CurrentStage = EVisualizationStage::NoVisualization;
    ClearVisualization();
    
    // Passe automatiquement à la première étape
    NextStage();
}

void AChunkVisualizerDemo::CreateGridPoints()
{
    if (VertexPositions.Num() > 0)
        return;
    
    UE_LOG(LogTemp, Warning, TEXT("Creating grid points. ChunkSize: %d"), ChunkSize);
    
    VertexPositions.Empty();
    VertexPositions.SetNum(ChunkSize * ChunkSize);
    
    // Charge un mesh de sphère pour visualiser les points
    UStaticMesh* PointMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (!PointMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load sphere mesh for points"));
        return;
    }
    
    for (int32 Y = 0; Y < ChunkSize; Y++)
    {
        for (int32 X = 0; X < ChunkSize; X++)
        {
            int32 Index = GetVertexIndex(X, Y);
            
            // Position de base (grille plate)
            float XPos = X * 100.0f;
            float YPos = Y * 100.0f;
            float ZPos = 0.0f;
            
            VertexPositions[Index] = FVector(XPos, YPos, ZPos);
        }
    }

    StartPointsAnimation();
    
    UE_LOG(LogTemp, Warning, TEXT("Created %d point visualizers"), PointVisualizers.Num());
}

void AChunkVisualizerDemo::ApplyPerlinNoise()
{
    UE_LOG(LogTemp, Warning, TEXT("Starting Perlin height animation"));
    
    // S'assurer qu'il y a des points à animer
    if (VertexPositions.Num() == 0 || PointVisualizers.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No vertices to apply Perlin noise to"));
        return;
    }
    
    // Sauvegarder les positions actuelles (planes)
    FlatPositions = VertexPositions;
    
    // Calculer les positions finales avec le bruit
    NoisePositions.SetNum(VertexPositions.Num());
    
    for (int32 Y = 0; Y < ChunkSize; Y++)
    {
        for (int32 X = 0; X < ChunkSize; X++)
        {
            int32 Index = GetVertexIndex(X, Y);
            
            // Calculer la valeur de bruit
            float NoiseValue = UPerlinNoise::GenerateOctavePerlinSmoothed(
                X, Y, 
                NoiseParameters.Octaves, 
                NoiseParameters.Persistence, 
                NoiseParameters.Frequency,
                NoiseParameters.Seed, 
                3.0f, 0.9f, 
                FVector2D(1.0f / 64.0f)
            );
            
            // Calculer la position finale
            NoisePositions[Index] = FlatPositions[Index];
            NoisePositions[Index].Z = (NoiseValue - 0.5f) * NoiseParameters.HeightFactor;
        }
    }
    
    // Démarrer l'animation
    StartHeightAnimation();
}

void AChunkVisualizerDemo::CreateTriangulation()
{
    UE_LOG(LogTemp, Warning, TEXT("Creating triangulation"));
    
    Triangles.Empty();
    
    for (int32 Y = 0; Y < ChunkSize - 1; Y++)
    {
        for (int32 X = 0; X < ChunkSize - 1; X++)
        {
            int32 BottomLeft = GetVertexIndex(X, Y);
            int32 BottomRight = GetVertexIndex(X + 1, Y);
            int32 TopLeft = GetVertexIndex(X, Y + 1);
            int32 TopRight = GetVertexIndex(X + 1, Y + 1);
            
            // Premier triangle
            Triangles.Add(BottomLeft);
            Triangles.Add(TopLeft);
            Triangles.Add(BottomRight);
            
            // Second triangle
            Triangles.Add(TopLeft);
            Triangles.Add(TopRight);
            Triangles.Add(BottomRight);
        }
    }
    
    HighestPoint = -FLT_MAX;
    LowestPoint = FLT_MAX;
    
    for (const FVector& Vertex : VertexPositions)
    {
        HighestPoint = FMath::Max(HighestPoint, Vertex.Z);
        LowestPoint = FMath::Min(LowestPoint, Vertex.Z);
    }
    
    // Crée un material dynamique basé sur le matériau wireframe
    UMaterialInstanceDynamic* DynamicWireframeMaterial = nullptr;
    if (WireframeMaterial)
    {
        DynamicWireframeMaterial = UMaterialInstanceDynamic::Create(WireframeMaterial, this);
        DynamicWireframeMaterial->SetScalarParameterValue("HeightCutoff", HighestPoint + 1000.0f);
    }
    
    // Générer des normales temporaires
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;
    Normals.Init(FVector(0, 0, 1), VertexPositions.Num());
    UV0.Init(FVector2D(0, 0), VertexPositions.Num());
    
    // Créer le mesh avec le matériau dynamique
    MeshComponent->CreateMeshSection_LinearColor(0, VertexPositions, Triangles, Normals, UV0,TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
    
    if (DynamicWireframeMaterial)
    {
        MeshComponent->SetMaterial(0, DynamicWireframeMaterial);
    }
    
    // Démarrer l'animation
    StartWireframeAnimation();
}

void AChunkVisualizerDemo::ApplyFinalMesh()
{
    UE_LOG(LogTemp, Warning, TEXT("Applying final mesh"));
    
    // S'assurer que les triangles existent
    if (Triangles.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No triangles to create final mesh"));
        CreateTriangulation();  // Créer les triangles si nécessaire
    }
    
    // Calcule les normales correctes
    TArray<FVector> Normals;
    Normals.Init(FVector(0, 0, 0), VertexPositions.Num());
    
    // Calcul des normales basé sur les triangles
    for (int32 i = 0; i < Triangles.Num(); i += 3)
    {
        if (Triangles[i] < VertexPositions.Num() && 
            Triangles[i+1] < VertexPositions.Num() && 
            Triangles[i+2] < VertexPositions.Num())
        {
            const FVector& V0 = VertexPositions[Triangles[i]];
            const FVector& V1 = VertexPositions[Triangles[i + 1]];
            const FVector& V2 = VertexPositions[Triangles[i + 2]];
            
            const FVector Edge1 = V1 - V0;
            const FVector Edge2 = V2 - V0;
            const FVector TriangleNormal = FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();
            
            Normals[Triangles[i]] += TriangleNormal;
            Normals[Triangles[i + 1]] += TriangleNormal;
            Normals[Triangles[i + 2]] += TriangleNormal;
        }
    }
    
    // Normalise les normales
    for (FVector& Normal : Normals)
    {
        if (!Normal.IsNearlyZero())
        {
            Normal.Normalize();
        }
        else
        {
            Normal = FVector(0, 0, 1);
        }
    }
    
    // Génère des UVs
    TArray<FVector2D> UV0;
    UV0.SetNum(VertexPositions.Num());
    
    for (int32 Y = 0; Y < ChunkSize; Y++)
    {
        for (int32 X = 0; X < ChunkSize; X++)
        {
            int32 Index = GetVertexIndex(X, Y);
            float U = static_cast<float>(X) / (ChunkSize - 1);
            float V = static_cast<float>(Y) / (ChunkSize - 1);
            UV0[Index] = FVector2D(U, V);
        }
    }
    
    // Mise à jour du mesh avec les normales finales
    MeshComponent->UpdateMeshSection(0, VertexPositions, Normals, UV0, 
        TArray<FColor>(), TArray<FProcMeshTangent>());
    
    // Application du matériau final
    if (FinalMaterial)
    {
        MeshComponent->SetMaterial(0, FinalMaterial);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FinalMaterial is not set"));
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Final mesh applied"));
}

void AChunkVisualizerDemo::ClearVisualization()
{
    UE_LOG(LogTemp, Warning, TEXT("Clearing visualization"));
    
    // Nettoie les points
    for (auto* PointComp : PointVisualizers)
    {
        if (PointComp)
        {
            PointComp->DestroyComponent();
        }
    }
    PointVisualizers.Empty();
    
    // Nettoie le mesh
    MeshComponent->ClearAllMeshSections();
    
    // Réinitialise les données
    VertexPositions.Empty();
    Triangles.Empty();
    
    UE_LOG(LogTemp, Warning, TEXT("Visualization cleared"));
}