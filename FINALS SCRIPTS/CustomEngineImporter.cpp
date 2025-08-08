#include "CustomEngineImporter.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/DateTime.h"

UCustomEngineImporter::UCustomEngineImporter()
{
    jsonFilePath = TEXT("");
    logOutput = TEXT("");
}

UCustomEngineImporter::~UCustomEngineImporter()
{
    createdEntities.Empty();
}

//* ╔═════════════════════╗
//* ║ Non - Monobehaviour ║
//* ╚═════════════════════╝

bool UCustomEngineImporter::ImportSceneFromFile(const FString& FilePath)
{
    FString JsonContent = GetJsonContent(FilePath);
    if (JsonContent.IsEmpty())
    {
        LogMessage(TEXT("Error: No JSON content found. Please provide a valid file path."));
        return false;
    }

    return ImportScene(JsonContent);
}

bool UCustomEngineImporter::ImportSceneFromString(const FString& JsonContent)
{
    if (JsonContent.IsEmpty())
    {
        LogMessage(TEXT("Error: No JSON content provided."));
        return false;
    }

    return ImportScene(JsonContent);
}

bool UCustomEngineImporter::ImportScene(const FString& JsonContent)
{
    FSceneData SceneData;
    if (!ParseJsonToSceneData(JsonContent, SceneData))
    {
        LogMessage(TEXT("Error: Failed to parse JSON data."));
        return false;
    }

    LogMessage(FString::Printf(TEXT("Importing scene: %s (Version: %s)"), 
        *SceneData.scene_name, *SceneData.version));

    UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        LogMessage(TEXT("Error: Could not get world context."));
        return false;
    }

    AActor* SceneRoot = World->SpawnActor<AActor>();
    SceneRoot->SetActorLabel(FString::Printf(TEXT("Imported_Scene_%s"), *SceneData.scene_name));
    
    createdEntities.Empty();

    if (SceneData.entities.Num() > 0)
    {
        for (const FEntityData& EntityData : SceneData.entities)
        {
            AActor* Entity = CreateEntityWithoutTransform(EntityData, SceneRoot);
            if (Entity != nullptr)
            {
                createdEntities.Add(EntityData.name, Entity);
            }
        }

        for (const FEntityData& EntityData : SceneData.entities)
        {
            if (!EntityData.parent.IsEmpty() && 
                createdEntities.Contains(EntityData.name) && 
                createdEntities.Contains(EntityData.parent))
            {
                AActor* ChildActor = createdEntities[EntityData.name];
                AActor* ParentActor = createdEntities[EntityData.parent];
                
                ChildActor->AttachToActor(ParentActor, FAttachmentTransformRules::KeepWorldTransform);
                LogMessage(FString::Printf(TEXT("Set parent: %s -> %s"), 
                    *EntityData.name, *EntityData.parent));
            }
        }

        for (const FEntityData& EntityData : SceneData.entities)
        {
            if (createdEntities.Contains(EntityData.name))
            {
                ApplyTransform(createdEntities[EntityData.name], EntityData.transform);
            }
        }
    }

    LogMessage(FString::Printf(TEXT("Successfully imported %d entities from scene '%s'"), 
        createdEntities.Num(), *SceneData.scene_name));
    
    return true;
}

AActor* UCustomEngineImporter::CreateEntityWithoutTransform(const FEntityData& EntityData, AActor* ParentActor)
{
    UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return nullptr;
    }

    AActor* Entity = World->SpawnActor<AActor>();
    Entity->SetActorLabel(EntityData.name);
    Entity->SetActorHiddenInGame(!EntityData.enabled);

    if (ParentActor)
    {
        Entity->AttachToActor(ParentActor, FAttachmentTransformRules::KeepWorldTransform);
    }

    if (EntityData.components.Num() > 0)
    {
        for (const FComponentData& ComponentData : EntityData.components)
        {
            CreateComponent(Entity, ComponentData, EntityData.constant);
        }
    }

    LogMessage(FString::Printf(TEXT("Created entity: %s"), *EntityData.name));
    return Entity;
}

void UCustomEngineImporter::ApplyTransform(AActor* Entity, const FTransformData& TransformData)
{
    if (!Entity)
    {
        return;
    }

    FTransform NewTransform;
    NewTransform.SetLocation(TransformData.position.ToUnrealVector());
    NewTransform.SetRotation(FQuat::MakeFromEuler(TransformData.rotation.ToUnrealVector()));
    NewTransform.SetScale3D(TransformData.scale.ToUnrealVector());

    Entity->SetActorRelativeTransform(NewTransform);

    LogMessage(FString::Printf(TEXT("Applied transform to %s: pos=%s, rot=%s, scale=%s"),
        *Entity->GetActorLabel(),
        *TransformData.position.ToUnrealVector().ToString(),
        *TransformData.rotation.ToUnrealVector().ToString(),
        *TransformData.scale.ToUnrealVector().ToString()));
}

void UCustomEngineImporter::CreateComponent(AActor* Entity, const FComponentData& ComponentData, const FConstantData& ConstantData)
{
    if (!Entity)
    {
        return;
    }

    UStaticMesh* PrimitiveMesh = nullptr;

    if (ComponentData.type == TEXT("CubeRenderer"))
    {
        PrimitiveMesh = GetPrimitiveMesh(TEXT("Cube"));
    }
    else if (ComponentData.type == TEXT("QuadRenderer"))
    {
        PrimitiveMesh = GetPrimitiveMesh(TEXT("Plane"));
    }
    else if (ComponentData.type == TEXT("SphereRenderer"))
    {
        PrimitiveMesh = GetPrimitiveMesh(TEXT("Sphere"));
    }
    else if (ComponentData.type == TEXT("CylinderRenderer"))
    {
        PrimitiveMesh = GetPrimitiveMesh(TEXT("Cylinder"));
    }
    else if (ComponentData.type == TEXT("MeshRenderer"))
    {
        PrimitiveMesh = nullptr;
        LogMessage(FString::Printf(TEXT("MeshRenderer created for model: %s"), *ComponentData.modelPath));
    }
    else if (ComponentData.type == TEXT("PhysicsComponent"))
    {
        UStaticMeshComponent* MeshComponent = Entity->FindComponentByClass<UStaticMeshComponent>();
        if (MeshComponent)
        {
            MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
            
            if (ComponentData.bodyType == 0)
            {
                MeshComponent->SetMobility(EComponentMobility::Static);
            }
            else
            {
                MeshComponent->SetMobility(EComponentMobility::Movable);
                MeshComponent->SetSimulatePhysics(true);
                MeshComponent->SetMassOverrideInKg(NAME_None, ComponentData.mass > 0.0f ? ComponentData.mass : 1.0f);
            }
        }

        LogMessage(FString::Printf(TEXT("Added physics component (mass: %f)"), ComponentData.mass));
        return;
    }

    if (ComponentData.type.Contains(TEXT("Renderer")))
    {
        UStaticMeshComponent* MeshComponent = Entity->FindComponentByClass<UStaticMeshComponent>();
        if (!MeshComponent)
        {
            MeshComponent = NewObject<UStaticMeshComponent>(Entity);
            Entity->AddInstanceComponent(MeshComponent);
            Entity->SetRootComponent(MeshComponent);
        }

        if (PrimitiveMesh != nullptr)
        {
            MeshComponent->SetStaticMesh(PrimitiveMesh);
        }

        if (ConstantData.color.x >= 0.0f && ConstantData.color.y >= 0.0f && ConstantData.color.z >= 0.0f)
        {
            UMaterialInstanceDynamic* DynamicMaterial = CreateMaterialWithColor(ConstantData.color);
            if (DynamicMaterial)
            {
                MeshComponent->SetMaterial(0, DynamicMaterial);
            }
        }

        if (ComponentData.isTextured && !ComponentData.texturePath.IsEmpty())
        {
            LogMessage(FString::Printf(TEXT("Texture path found: %s"), *ComponentData.texturePath));
        }

        LogMessage(FString::Printf(TEXT("Added %s component to entity"), *ComponentData.type));
    }
}

UStaticMesh* UCustomEngineImporter::GetPrimitiveMesh(const FString& PrimitiveType)
{
    UStaticMesh* PrimitiveMesh = nullptr;

    if (PrimitiveType == TEXT("Cube"))
    {
        static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Engine/BasicShapes/Cube"));
        PrimitiveMesh = CubeMeshFinder.Object;
    }
    else if (PrimitiveType == TEXT("Sphere"))
    {
        static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Engine/BasicShapes/Sphere"));
        PrimitiveMesh = SphereMeshFinder.Object;
    }
    else if (PrimitiveType == TEXT("Cylinder"))
    {
        static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshFinder(TEXT("/Engine/BasicShapes/Cylinder"));
        PrimitiveMesh = CylinderMeshFinder.Object;
    }
    else if (PrimitiveType == TEXT("Plane"))
    {
        static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshFinder(TEXT("/Engine/BasicShapes/Plane"));
        PrimitiveMesh = PlaneMeshFinder.Object;
    }

    return PrimitiveMesh;
}

UMaterialInstanceDynamic* UCustomEngineImporter::CreateMaterialWithColor(const FVector3Data& ColorData)
{
    static ConstructorHelpers::FObjectFinder<UMaterial> BaseMaterialFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
    UMaterial* BaseMaterial = BaseMaterialFinder.Object;
    
    if (!BaseMaterial)
    {
        return nullptr;
    }

    UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
    if (DynamicMaterial)
    {
        FLinearColor Color(ColorData.x, ColorData.y, ColorData.z, 1.0f);
        DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
    }

    return DynamicMaterial;
}

FString UCustomEngineImporter::GetJsonContent(const FString& FilePath)
{
    FString JsonContent;
    
    if (!FilePath.IsEmpty() && FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
    {
        if (!FFileHelper::LoadFileToString(JsonContent, *FilePath))
        {
            LogMessage(FString::Printf(TEXT("Error: Could not read file: %s"), *FilePath));
        }
    }

    return JsonContent;
}

void UCustomEngineImporter::ClearCurrentScene()
{
    UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return;
    }

    TArray<AActor*> ActorsToDestroy;
    
    for (TActorIterator<AActor> ActorIterator(World); ActorIterator; ++ActorIterator)
    {
        AActor* CurrentActor = *ActorIterator;
        if (CurrentActor && CurrentActor->GetActorLabel().StartsWith(TEXT("Imported_Scene_")))
        {
            ActorsToDestroy.Add(CurrentActor);
        }
    }

    for (AActor* ActorToDestroy : ActorsToDestroy)
    {
        LogMessage(FString::Printf(TEXT("Cleared imported scene: %s"), *ActorToDestroy->GetActorLabel()));
        ActorToDestroy->Destroy();
    }

    createdEntities.Empty();
}

bool UCustomEngineImporter::ParseJsonToSceneData(const FString& JsonContent, FSceneData& OutSceneData)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContent);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        return false;
    }

    OutSceneData.scene_name = JsonObject->GetStringField(TEXT("scene_name"));
    OutSceneData.version = JsonObject->GetStringField(TEXT("version"));

    const TArray<TSharedPtr<FJsonValue>>* EntitiesArray;
    if (JsonObject->TryGetArrayField(TEXT("entities"), EntitiesArray))
    {
        for (const TSharedPtr<FJsonValue>& EntityValue : *EntitiesArray)
        {
            TSharedPtr<FJsonObject> EntityObject = EntityValue->AsObject();
            if (EntityObject.IsValid())
            {
                FEntityData EntityData;
                EntityData.name = EntityObject->GetStringField(TEXT("name"));
                EntityData.enabled = EntityObject->GetBoolField(TEXT("enabled"));
                EntityData.parent = EntityObject->GetStringField(TEXT("parent"));

                TSharedPtr<FJsonObject> TransformObject = EntityObject->GetObjectField(TEXT("transform"));
                if (TransformObject.IsValid())
                {
                    TSharedPtr<FJsonObject> PositionObject = TransformObject->GetObjectField(TEXT("position"));
                    if (PositionObject.IsValid())
                    {
                        EntityData.transform.position.x = PositionObject->GetNumberField(TEXT("x"));
                        EntityData.transform.position.y = PositionObject->GetNumberField(TEXT("y"));
                        EntityData.transform.position.z = PositionObject->GetNumberField(TEXT("z"));
                    }

                    TSharedPtr<FJsonObject> RotationObject = TransformObject->GetObjectField(TEXT("rotation"));
                    if (RotationObject.IsValid())
                    {
                        EntityData.transform.rotation.x = RotationObject->GetNumberField(TEXT("x"));
                        EntityData.transform.rotation.y = RotationObject->GetNumberField(TEXT("y"));
                        EntityData.transform.rotation.z = RotationObject->GetNumberField(TEXT("z"));
                    }

                    TSharedPtr<FJsonObject> ScaleObject = TransformObject->GetObjectField(TEXT("scale"));
                    if (ScaleObject.IsValid())
                    {
                        EntityData.transform.scale.x = ScaleObject->GetNumberField(TEXT("x"));
                        EntityData.transform.scale.y = ScaleObject->GetNumberField(TEXT("y"));
                        EntityData.transform.scale.z = ScaleObject->GetNumberField(TEXT("z"));
                    }
                }

                TSharedPtr<FJsonObject> ConstantObject = EntityObject->GetObjectField(TEXT("constant"));
                if (ConstantObject.IsValid())
                {
                    TSharedPtr<FJsonObject> ColorObject = ConstantObject->GetObjectField(TEXT("color"));
                    if (ColorObject.IsValid())
                    {
                        EntityData.constant.color.x = ColorObject->GetNumberField(TEXT("x"));
                        EntityData.constant.color.y = ColorObject->GetNumberField(TEXT("y"));
                        EntityData.constant.color.z = ColorObject->GetNumberField(TEXT("z"));
                    }
                    EntityData.constant.angle = ConstantObject->GetNumberField(TEXT("angle"));
                    EntityData.constant.isRandom = ConstantObject->GetIntegerField(TEXT("isRandom"));
                    EntityData.constant.hasTex = ConstantObject->GetIntegerField(TEXT("hasTex"));
                }

                const TArray<TSharedPtr<FJsonValue>>* ComponentsArray;
                if (EntityObject->TryGetArrayField(TEXT("components"), ComponentsArray))
                {
                    for (const TSharedPtr<FJsonValue>& ComponentValue : *ComponentsArray)
                    {
                        TSharedPtr<FJsonObject> ComponentObject = ComponentValue->AsObject();
                        if (ComponentObject.IsValid())
                        {
                            FComponentData ComponentData;
                            ComponentData.type = ComponentObject->GetStringField(TEXT("type"));
                            ComponentData.isTextured = ComponentObject->GetBoolField(TEXT("isTextured"));
                            ComponentData.modelPath = ComponentObject->GetStringField(TEXT("modelPath"));
                            ComponentData.texturePath = ComponentObject->GetStringField(TEXT("texturePath"));
                            ComponentData.bodyType = ComponentObject->GetIntegerField(TEXT("bodyType"));
                            ComponentData.mass = ComponentObject->GetNumberField(TEXT("mass"));

                            EntityData.components.Add(ComponentData);
                        }
                    }
                }

                OutSceneData.entities.Add(EntityData);
            }
        }
    }

    return true;
}

void UCustomEngineImporter::LogMessage(const FString& Message)
{
    FDateTime Now = FDateTime::Now();
    FString TimeStamp = Now.ToString(TEXT("%H:%M:%S"));
    
    logOutput += FString::Printf(TEXT("[%s] %s\n"), *TimeStamp, *Message);
    UE_LOG(LogTemp, Log, TEXT("Custom Engine Importer: %s"), *Message);
}

//* ╔════════════════════════════════╗
//* ║ Virtual / Overridden Functions ║
//* ╚════════════════════════════════╝
