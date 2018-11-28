#include "urho3dbase.hpp"
#include <iostream>
#include <QResizeEvent>
#include <QDebug>

using namespace Urho3D;

 Urho3DBase::Urho3DBase(QString sceneXml, Context *ctx, QWidget *parent) :
     Urho3D::Object(ctx),
     QWidget (parent) {
    timer_ = new QTimer(this);
    timer_->setInterval(20);
    connect(timer_, &QTimer::timeout, this, &Urho3DBase::OnTimeout);
    Vehicle::RegisterObject(ctx);
 }

 Urho3DBase::~Urho3DBase()
 {
     Stop();
     delete context_;
 }

void Urho3DBase::Setup() {
    // Modify engine startup parameters
    Urho3D::VariantMap engineParameters_;
    engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
    engineParameters_[EP_LOG_NAME]     = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_[EP_HEADLESS]     = false;
    engineParameters_[EP_SOUND]        = false;

    // Construct a search path to find the resource prefix with two entries:
    // The first entry is an empty path which will be substituted with program/bin directory -- this entry is for binary when it is still in build tree
    // The second and third entries are possible relative paths from the installed program/bin directory to the asset directory -- these entries are for binary when it is in the Urho3D SDK installation location
    if (!engineParameters_.Contains(EP_RESOURCE_PREFIX_PATHS)) {
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";../share/Resources;../share/Urho3D/Resources";
    }
    engineParameters_["FullScreen"] = false;
    engineParameters_[EP_WINDOW_WIDTH] = 1280;
    engineParameters_[EP_WINDOW_HEIGHT] = 720;
    engineParameters_["WindowResizable"] = true;
#ifdef Q_OS_WIN32
    engineParameters_["ForceGL2"] = true;
#endif
    engineParameters_[EP_SHADOWS] = false;

    engine_ = new Urho3D::Engine(context_);
    engineParameters_["ExternalWindow"] = (void *)winId();

    engine_->Initialize(engineParameters_);

    timer_->start();
}

void Urho3DBase::Start() {
    // Create static scene content
    CreateScene();

    // Create the controllable vehicle
    CreateVehicle();

    // Create the UI content
    CreateInstructions();

    // Subscribe to necessary events
    SubscribeToEvents();
}

void Urho3DBase::Stop() {
    if (engine_.NotNull() && engine_->IsInitialized()) {
        engine_->DumpResources(true);
        engine_->Exit();
    }
}

void Urho3DBase::CreateScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

        scene_ = new Scene(context_);

        // Create scene subsystem components
        scene_->CreateComponent<Octree>();
        scene_->CreateComponent<PhysicsWorld>();
        scene_->CreateComponent<DebugRenderer>();

        // Create camera and define viewport. We will be doing load / save, so it's convenient to create the camera outside the scene,
        // so that it won't be destroyed and recreated, and we don't have to redefine the viewport on load
        cameraNode_ = new Node(context_);
        Camera* camera = cameraNode_->CreateComponent<Camera>();
        camera->SetFarClip(500.0f);
        GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, scene_, camera));

        // Create static scene content. First create a zone for ambient lighting and fog control
        Node* zoneNode = scene_->CreateChild("Zone");
        Zone* zone = zoneNode->CreateComponent<Zone>();
        zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
        zone->SetFogColor(Color(0.5f, 0.5f, 0.7f));
        zone->SetFogStart(300.0f);
        zone->SetFogEnd(500.0f);
        zone->SetBoundingBox(BoundingBox(-2000.0f, 2000.0f));

        // Create a directional light with cascaded shadow mapping
        Node* lightNode = scene_->CreateChild("DirectionalLight");
        lightNode->SetDirection(Vector3(0.3f, -0.5f, 0.425f));
        Light* light = lightNode->CreateComponent<Light>();
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetCastShadows(true);
        light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
        light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
        light->SetSpecularIntensity(0.5f);

        // Create heightmap terrain with collision
        Node* terrainNode = scene_->CreateChild("Terrain");
        terrainNode->SetPosition(Vector3::ZERO);
        Terrain* terrain = terrainNode->CreateComponent<Terrain>();
        terrain->SetPatchSize(64);
        terrain->SetSpacing(Vector3(2.0f, 0.1f, 2.0f)); // Spacing between vertices and vertical resolution of the height map
        terrain->SetSmoothing(true);
        terrain->SetHeightMap(cache->GetResource<Image>("Textures/HeightMap.png"));
        terrain->SetMaterial(cache->GetResource<Material>("Materials/Terrain.xml"));
        // The terrain consists of large triangles, which fits well for occlusion rendering, as a hill can occlude all
        // terrain patches and other objects behind it
        terrain->SetOccluder(true);

        RigidBody* body = terrainNode->CreateComponent<RigidBody>();
        body->SetCollisionLayer(2); // Use layer bitmask 2 for static geometry
        CollisionShape* shape = terrainNode->CreateComponent<CollisionShape>();
        shape->SetTerrain();

        // Create 1000 mushrooms in the terrain. Always face outward along the terrain normal
        const unsigned NUM_MUSHROOMS = 1000;
        for (unsigned i = 0; i < NUM_MUSHROOMS; ++i)
        {
            Node* objectNode = scene_->CreateChild("Mushroom");
            Vector3 position(Random(2000.0f) - 1000.0f, 0.0f, Random(2000.0f) - 1000.0f);
            position.y_ = terrain->GetHeight(position) - 0.1f;
            objectNode->SetPosition(position);
            // Create a rotation quaternion from up vector to terrain normal
            objectNode->SetRotation(Quaternion(Vector3::UP, terrain->GetNormal(position)));
            objectNode->SetScale(3.0f);
            StaticModel* object = objectNode->CreateComponent<StaticModel>();
            object->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
            object->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
            object->SetCastShadows(true);

            RigidBody* body = objectNode->CreateComponent<RigidBody>();
            body->SetCollisionLayer(2);
            CollisionShape* shape = objectNode->CreateComponent<CollisionShape>();
            shape->SetTriangleMesh(object->GetModel(), 0);
        }
}

void Urho3DBase::CreateVehicle()
{
    Node* vehicleNode = scene_->CreateChild("Vehicle");
        vehicleNode->SetPosition(Vector3(0.0f, 5.0f, 0.0f));

        // Create the vehicle logic component
        vehicle_ = vehicleNode->CreateComponent<Vehicle>();
        // Create the rendering and physics components
        vehicle_->Init();
}

void Urho3DBase::CreateInstructions()
{

}

void Urho3DBase::SubscribeToEvents()
{
    // Subscribe to Update event for setting the vehicle controls before physics simulation
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Urho3DBase, HandleUpdate));

    // Subscribe to PostUpdate event for updating the camera position after physics simulation
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Urho3DBase, HandlePostUpdate));

    // Unsubscribe the SceneUpdate event from base class as the camera node is being controlled in HandlePostUpdate() in this sample
    UnsubscribeFromEvent(E_SCENEUPDATE);
}

void Urho3DBase::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;

        Input* input = GetSubsystem<Input>();

        if (vehicle_)
        {
            UI* ui = GetSubsystem<UI>();

            // Get movement controls and assign them to the vehicle component. If UI has a focused element, clear controls
            if (!ui->GetFocusElement())
            {
                vehicle_->controls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
                vehicle_->controls_.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
                vehicle_->controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
                vehicle_->controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));

                // Add yaw & pitch from the mouse motion or touch input. Used only for the camera, does not affect motion

                    vehicle_->controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
                    vehicle_->controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;

                // Limit pitch
                vehicle_->controls_.pitch_ = Clamp(vehicle_->controls_.pitch_, 0.0f, 80.0f);

                // Check for loading / saving the scene
                if (input->GetKeyPress(KEY_F5))
                {
                    File saveFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/VehicleDemo.xml",
                        FILE_WRITE);
                    scene_->SaveXML(saveFile);
                }
                if (input->GetKeyPress(KEY_F7))
                {
                    File loadFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/VehicleDemo.xml", FILE_READ);
                    scene_->LoadXML(loadFile);
                    // After loading we have to reacquire the weak pointer to the Vehicle component, as it has been recreated
                    // Simply find the vehicle's scene node by name as there's only one of them
                    Node* vehicleNode = scene_->GetChild("Vehicle", true);
                    if (vehicleNode)
                        vehicle_ = vehicleNode->GetComponent<Vehicle>();
                }
            }
            else
                vehicle_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT, false);
        }
}

void Urho3DBase::HandlePostUpdate(StringHash eventType, VariantMap &eventData)
{
    scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
    if (!vehicle_)
            return;

        Node* vehicleNode = vehicle_->GetNode();

        // Physics update has completed. Position camera behind vehicle
        Quaternion dir(vehicleNode->GetRotation().YawAngle(), Vector3::UP);
        dir = dir * Quaternion(vehicle_->controls_.yaw_, Vector3::UP);
        dir = dir * Quaternion(vehicle_->controls_.pitch_, Vector3::RIGHT);

        Vector3 cameraTargetPos = vehicleNode->GetPosition() - dir * Vector3(0.0f, 0.0f, 10);
        Vector3 cameraStartPos = vehicleNode->GetPosition();

        // Raycast camera against static objects (physics collision mask 2)
        // and move it closer to the vehicle if something in between
        Ray cameraRay(cameraStartPos, cameraTargetPos - cameraStartPos);
        float cameraRayLength = (cameraTargetPos - cameraStartPos).Length();
        PhysicsRaycastResult result;
        scene_->GetComponent<PhysicsWorld>()->RaycastSingle(result, cameraRay, cameraRayLength, 2);
        if (result.body_)
            cameraTargetPos = cameraStartPos + cameraRay.direction_ * (result.distance_ - 0.5f);

        cameraNode_->SetPosition(cameraTargetPos);
        cameraNode_->SetRotation(dir);
}

void Urho3DBase::OnTimeout()
{
    if (engine_.NotNull() && engine_->IsInitialized()) {
            engine_->RunFrame();
    }
}

