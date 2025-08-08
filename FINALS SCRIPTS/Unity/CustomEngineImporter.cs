using System;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

namespace LilLycanLord_Official
{
    public class CustomEngineImporter : EditorWindow
    {
        //* ╔════════════╗
        //* ║ Components ║
        //* ╚════════════╝

        //* ╔══════════╗
        //* ║ Displays ║
        //* ╚══════════╝

        //* ╔════════╗
        //* ║ Fields ║
        //* ╚════════╝

        private string jsonFilePath = "";
        private TextAsset jsonFile;
        private Vector2 scrollPosition;
        private string logOutput = "";

        //* ╔════════════╗
        //* ║ Attributes ║
        //* ╚════════════╝

        //* ╔═════════════════════════╗
        //* ║ Data Structure Classes  ║
        //* ╚═════════════════════════╝

        [Serializable]
        public class SceneData
        {
            public string scene_name;
            public string version;
            public EntityData[] entities;
        }

        [Serializable]
        public class EntityData
        {
            public string name;
            public bool enabled = true;
            public TransformData transform;
            public ConstantData constant;
            public string parent;
            public ComponentData[] components;
        }

        [Serializable]
        public class TransformData
        {
            public Vector3Data position;
            public Vector3Data rotation;
            public Vector3Data scale;
        }

        [Serializable]
        public class Vector3Data
        {
            public float x,
                y,
                z;

            public Vector3 ToUnityVector3()
            {
                return new Vector3(x, y, z);
            }
        }

        [Serializable]
        public class ConstantData
        {
            public Vector3Data color;
            public float angle;
            public int isRandom;
            public int hasTex;
        }

        [Serializable]
        public class ComponentData
        {
            public string type;
            public bool isTextured;
            public string modelPath;
            public string texturePath;
            public int bodyType;
            public float mass;
        }

        //* ╔═══════════════╗
        //* ║ Editor Window ║
        //* ╚═══════════════╝

        [MenuItem("Tools/Custom Engine Importer")]
        public static void ShowWindow()
        {
            GetWindow<CustomEngineImporter>("Custom Engine Importer");
        }

        void OnGUI()
        {
            GUILayout.Label("Custom Engine JSON Importer", EditorStyles.boldLabel);
            EditorGUILayout.Space();

            GUILayout.Label("Import Method:", EditorStyles.label);

            EditorGUILayout.BeginHorizontal();
            jsonFilePath = EditorGUILayout.TextField("JSON File Path:", jsonFilePath);
            if (GUILayout.Button("Browse", GUILayout.Width(60)))
            {
                string filePath = EditorUtility.OpenFilePanel("Select JSON File", "", "json");
                if (!string.IsNullOrEmpty(filePath))
                {
                    jsonFilePath = filePath;
                }
            }
            EditorGUILayout.EndHorizontal();

            jsonFile = (TextAsset)
                EditorGUILayout.ObjectField("JSON TextAsset:", jsonFile, typeof(TextAsset), false);

            EditorGUILayout.Space();

            if (GUILayout.Button("Import Scene", GUILayout.Height(30)))
            {
                ImportScene();
            }

            EditorGUILayout.Space();

            if (GUILayout.Button("Clear Current Scene", GUILayout.Height(25)))
            {
                ClearCurrentScene();
            }

            EditorGUILayout.Space();

            GUILayout.Label("Import Log:", EditorStyles.boldLabel);
            scrollPosition = EditorGUILayout.BeginScrollView(scrollPosition, GUILayout.Height(200));
            EditorGUILayout.TextArea(logOutput, GUILayout.ExpandHeight(true));
            EditorGUILayout.EndScrollView();
        }

        //* ╔═════════════════════╗
        //* ║ Non - Monobehaviour ║
        //* ╚═════════════════════╝

        private void ImportScene()
        {
            string jsonContent = GetJsonContent();
            if (string.IsNullOrEmpty(jsonContent))
            {
                LogMessage(
                    "Error: No JSON content found. Please provide a file path or TextAsset."
                );
                return;
            }

            try
            {
                SceneData sceneData = JsonUtility.FromJson<SceneData>(jsonContent);
                if (sceneData == null)
                {
                    LogMessage("Error: Failed to parse JSON data.");
                    return;
                }

                LogMessage(
                    $"Importing scene: {sceneData.scene_name} (Version: {sceneData.version})"
                );

                GameObject sceneRoot = new GameObject($"Imported_Scene_{sceneData.scene_name}");
                Undo.RegisterCreatedObjectUndo(sceneRoot, "Import Custom Engine Scene");

                Dictionary<string, GameObject> createdEntities =
                    new Dictionary<string, GameObject>();

                if (sceneData.entities != null)
                {
                    foreach (EntityData entityData in sceneData.entities)
                    {
                        GameObject entity = CreateEntityWithoutTransform(
                            entityData,
                            sceneRoot.transform
                        );
                        if (entity != null)
                        {
                            createdEntities[entityData.name] = entity;
                        }
                    }

                    foreach (EntityData entityData in sceneData.entities)
                    {
                        if (
                            !string.IsNullOrEmpty(entityData.parent)
                            && createdEntities.ContainsKey(entityData.name)
                            && createdEntities.ContainsKey(entityData.parent)
                        )
                        {
                            createdEntities[entityData.name]
                                .transform.SetParent(createdEntities[entityData.parent].transform);
                            LogMessage($"Set parent: {entityData.name} -> {entityData.parent}");
                        }
                    }

                    foreach (EntityData entityData in sceneData.entities)
                    {
                        if (createdEntities.ContainsKey(entityData.name))
                        {
                            ApplyTransform(createdEntities[entityData.name], entityData.transform);
                        }
                    }
                }

                LogMessage(
                    $"Successfully imported {createdEntities.Count} entities from scene '{sceneData.scene_name}'"
                );
            }
            catch (Exception e)
            {
                LogMessage($"Error importing scene: {e.Message}");
            }
        }

        private GameObject CreateEntityWithoutTransform(
            EntityData entityData,
            Transform parentTransform
        )
        {
            GameObject entity = new GameObject(entityData.name);
            entity.transform.SetParent(parentTransform);
            entity.SetActive(entityData.enabled);

            if (entityData.components != null)
            {
                foreach (ComponentData componentData in entityData.components)
                {
                    CreateComponent(entity, componentData, entityData.constant);
                }
            }

            Undo.RegisterCreatedObjectUndo(entity, "Create Entity");
            LogMessage($"Created entity: {entityData.name}");
            return entity;
        }

        private void ApplyTransform(GameObject entity, TransformData transformData)
        {
            if (transformData != null)
            {
                if (transformData.position != null)
                    entity.transform.localPosition = transformData.position.ToUnityVector3();
                if (transformData.rotation != null)
                    entity.transform.localEulerAngles = transformData.rotation.ToUnityVector3();
                if (transformData.scale != null)
                    entity.transform.localScale = transformData.scale.ToUnityVector3();

                LogMessage(
                    $"Applied transform to {entity.name}: pos={transformData.position?.ToUnityVector3()}, rot={transformData.rotation?.ToUnityVector3()}, scale={transformData.scale?.ToUnityVector3()}"
                );
            }
        }

        private void CreateComponent(
            GameObject entity,
            ComponentData componentData,
            ConstantData constantData
        )
        {
            Mesh primitiveMesh = null;

            switch (componentData.type)
            {
                case "CubeRenderer":
                    primitiveMesh = GetPrimitiveMesh(PrimitiveType.Cube);
                    break;
                case "QuadRenderer":
                    primitiveMesh = GetPrimitiveMesh(PrimitiveType.Quad);
                    break;
                case "SphereRenderer":
                    primitiveMesh = GetPrimitiveMesh(PrimitiveType.Sphere);
                    break;
                case "CylinderRenderer":
                    primitiveMesh = GetPrimitiveMesh(PrimitiveType.Cylinder);
                    break;
                case "MeshRenderer":
                    // For custom meshes, we'll add components but leave mesh null for now
                    primitiveMesh = null;
                    LogMessage($"MeshRenderer created for model: {componentData.modelPath}");
                    break;
                case "PhysicsComponent":
                    // Add physics components
                    Rigidbody rigidBody = entity.AddComponent<Rigidbody>();
                    if (componentData.bodyType == 0)
                        rigidBody.isKinematic = true;
                    rigidBody.mass = componentData.mass > 0 ? componentData.mass : 1.0f;

                    MeshRenderer existingRenderer = entity.GetComponent<MeshRenderer>();
                    if (existingRenderer == null)
                        entity.AddComponent<BoxCollider>();
                    else
                        entity.AddComponent<MeshCollider>();

                    LogMessage($"Added physics component (mass: {componentData.mass})");
                    return;
            }

            if (componentData.type.Contains("Renderer"))
            {
                MeshRenderer meshRenderer = entity.GetComponent<MeshRenderer>();
                if (meshRenderer == null)
                    meshRenderer = entity.AddComponent<MeshRenderer>();

                MeshFilter meshFilter = entity.GetComponent<MeshFilter>();
                if (meshFilter == null)
                    meshFilter = entity.AddComponent<MeshFilter>();

                if (primitiveMesh != null)
                    meshFilter.mesh = primitiveMesh;

                if (constantData?.color != null)
                {
                    Material material = new Material(Shader.Find("Universal Render Pipeline/Lit"));
                    material.color = new Color(
                        constantData.color.x,
                        constantData.color.y,
                        constantData.color.z,
                        1.0f
                    );
                    meshRenderer.material = material;
                }

                if (componentData.isTextured && !string.IsNullOrEmpty(componentData.texturePath))
                {
                    LogMessage($"Texture path found: {componentData.texturePath}");
                }

                LogMessage($"Added {componentData.type} component to entity");
            }
        }

        private Mesh GetPrimitiveMesh(PrimitiveType primitiveType)
        {
            GameObject temporaryGameObject = GameObject.CreatePrimitive(primitiveType);
            Mesh primitiveMesh = temporaryGameObject.GetComponent<MeshFilter>().sharedMesh;
            DestroyImmediate(temporaryGameObject);
            return primitiveMesh;
        }

        private string GetJsonContent()
        {
            if (jsonFile != null)
            {
                return jsonFile.text;
            }

            if (!string.IsNullOrEmpty(jsonFilePath) && File.Exists(jsonFilePath))
            {
                return File.ReadAllText(jsonFilePath);
            }

            return null;
        }

        private void ClearCurrentScene()
        {
            GameObject[] rootObjects = UnityEngine
                .SceneManagement.SceneManager.GetActiveScene()
                .GetRootGameObjects();

            foreach (GameObject rootObject in rootObjects)
            {
                if (rootObject.name.StartsWith("Imported_Scene_"))
                {
                    Undo.DestroyObjectImmediate(rootObject);
                    LogMessage($"Cleared imported scene: {rootObject.name}");
                }
            }
        }

        private void LogMessage(string message)
        {
            logOutput += $"[{System.DateTime.Now:HH:mm:ss}] {message}\n";
            Debug.Log($"Custom Engine Importer: {message}");
            Repaint();
        }

        //* ╔════════════════════════════════╗
        //* ║ Virtual / Overridden Functions ║
        //* ╚════════════════════════════════╝
    }
}
