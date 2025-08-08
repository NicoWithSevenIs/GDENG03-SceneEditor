#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Engine/Selection.h"
#include "CustomEngineImporter.generated.h"

//* ╔═════════════════════════╗
//* ║ Data Structure Classes  ║
//* ╚═════════════════════════╝

USTRUCT(BlueprintType)
struct FVector3Data
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float x;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float y;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float z;

    FVector3Data() : x(0.0f), y(0.0f), z(0.0f) {}
    FVector3Data(float InX, float InY, float InZ) : x(InX), y(InY), z(InZ) {}

    FVector ToUnrealVector() const
    {
        return FVector(x, y, z);
    }
};

USTRUCT(BlueprintType)
struct FTransformData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector3Data position;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector3Data rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector3Data scale;

    FTransformData()
    {
        position = FVector3Data(0.0f, 0.0f, 0.0f);
        rotation = FVector3Data(0.0f, 0.0f, 0.0f);
        scale = FVector3Data(1.0f, 1.0f, 1.0f);
    }
};

USTRUCT(BlueprintType)
struct FConstantData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector3Data color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float angle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 isRandom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 hasTex;

    FConstantData() : angle(0.0f), isRandom(0), hasTex(0)
    {
        color = FVector3Data(1.0f, 1.0f, 1.0f);
    }
};

USTRUCT(BlueprintType)
struct FComponentData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isTextured;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString modelPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString texturePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 bodyType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float mass;

    FComponentData() : isTextured(false), bodyType(0), mass(1.0f) {}
};

USTRUCT(BlueprintType)
struct FEntityData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool enabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransformData transform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FConstantData constant;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString parent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FComponentData> components;

    FEntityData() : enabled(true) {}
};

USTRUCT(BlueprintType)
struct FSceneData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString scene_name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString version;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FEntityData> entities;

    FSceneData() {}
};

/**
 * Custom Engine Scene Importer for Unreal Engine 5
 * Imports JSON scenes created by the custom C++ game engine
 */
UCLASS(BlueprintType)
class GDENG03_FINALS_API UCustomEngineImporter : public UObject
{
    GENERATED_BODY()

public:
    UCustomEngineImporter();
    ~UCustomEngineImporter();

    //* ╔════════════╗
    //* ║ Components ║
    //* ╚════════════╝

    //* ╔══════════╗
    //* ║ Displays ║
    //* ╚══════════╝

    //* ╔════════╗
    //* ║ Fields ║
    //* ╚════════╝

private:
    UPROPERTY()
    FString jsonFilePath;

    UPROPERTY()
    FString logOutput;

    TMap<FString, AActor*> createdEntities;

    //* ╔════════════╗
    //* ║ Attributes ║
    //* ╚════════════╝

public:
    //* ╔═════════════════════╗
    //* ║ Non - Monobehaviour ║
    //* ╚═════════════════════╝

    UFUNCTION(BlueprintCallable, Category = "Custom Engine Importer")
    bool ImportSceneFromFile(const FString& FilePath);

    UFUNCTION(BlueprintCallable, Category = "Custom Engine Importer")
    bool ImportSceneFromString(const FString& JsonContent);

    UFUNCTION(BlueprintCallable, Category = "Custom Engine Importer")
    void ClearCurrentScene();

    UFUNCTION(BlueprintCallable, Category = "Custom Engine Importer")
    FString GetLogOutput() const { return logOutput; }

private:
    bool ImportScene(const FString& JsonContent);
    AActor* CreateEntityWithoutTransform(const FEntityData& EntityData, AActor* ParentActor);
    void ApplyTransform(AActor* Entity, const FTransformData& TransformData);
    void CreateComponent(AActor* Entity, const FComponentData& ComponentData, const FConstantData& ConstantData);
    UStaticMesh* GetPrimitiveMesh(const FString& PrimitiveType);
    UMaterialInstanceDynamic* CreateMaterialWithColor(const FVector3Data& ColorData);
    FString GetJsonContent(const FString& FilePath);
    void LogMessage(const FString& Message);
    bool ParseJsonToSceneData(const FString& JsonContent, FSceneData& OutSceneData);

    //* ╔════════════════════════════════╗
    //* ║ Virtual / Overridden Functions ║
    //* ╚════════════════════════════════╝
};
