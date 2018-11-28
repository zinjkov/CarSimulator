#ifndef URHO3DBASE_HPP
#define URHO3DBASE_HPP
#include <Urho3D/Urho3DAll.h>
#include <QWidget>
#include <QTimer>
#include "Vehicle.h"
class Urho3DBase: public QWidget, public Urho3D::Object
{
    Q_OBJECT
    URHO3D_OBJECT(Urho3DBase, Object);

public:
    Urho3DBase(QString sceneXml, Urho3D::Context *ctx, QWidget *parent = nullptr);

    virtual ~Urho3DBase();
    /// Setup before engine initialization. Modifies the engine parameters.
    virtual void Setup();
    /// Setup after engine initialization. Creates the logo, console & debug HUD.
    virtual void Start();
    /// Cleanup after the main loop. Called by Application.
    virtual void Stop();
protected:
    Urho3D::SharedPtr<Urho3D::Engine> engine_;

private:
    /// Create static scene content.
    void CreateScene();
    /// Create the vehicle.
    void CreateVehicle();
    /// Construct an instruction text to the UI.
    void CreateInstructions();
    /// Subscribe to necessary events.
    void SubscribeToEvents();
    /// Handle application update. Set controls to vehicle.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle application post-update. Update camera position after vehicle has moved.
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
    /// The controllable vehicle component.
    WeakPtr<Vehicle> vehicle_;

    QTimer *timer_;
    void OnTimeout();

    /// Scene.
    SharedPtr<Scene> scene_;
    /// Camera scene node.
    SharedPtr<Node> cameraNode_;
    /// Camera yaw angle.
    float yaw_;
    /// Camera pitch angle.
    float pitch_;

};

#endif // URHO3DBASE_HPP
