#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QList>
#include <raytracing/film.h>
#include <scene/scene.h>
#include <scene/camera.h>
#include <scene/transform.h>

#include <scene/lights/pointlight.h>
#include <scene/lights/spotlight.h>

#include "scene/geometry/ignoreGeoLight.h"

class PointLight;
class SpotLight;

class JSONReader
{
public:
    void LoadSceneFromFile(QFile &file, const QStringRef &local_path, Scene &scene);
    bool LoadGeometry(QJsonObject &geometry, QMap<QString, std::shared_ptr<Material>> mtl_map, const QStringRef &local_path, QList<std::shared_ptr<Primitive>> *primitives, QList<std::shared_ptr<Drawable> > *drawables);
    bool LoadLights(QJsonObject &geometry, QMap<QString, std::shared_ptr<Material>> mtl_map, const QStringRef &local_path, QList<std::shared_ptr<Primitive>> *primitives, QList<std::shared_ptr<Light>> *lights, QList<std::shared_ptr<Drawable>> *drawables);
    bool LoadMaterial(QJsonObject &material, const QStringRef &local_path, QMap<QString, std::shared_ptr<Material> > *mtl_map);
    Camera LoadCamera(QJsonObject &camera);
    Transform LoadTransform(QJsonObject &transform);
    glm::vec3 ToVec3(const QJsonArray &s);
    glm::vec3 ToVec3(const QStringRef &s);
};
