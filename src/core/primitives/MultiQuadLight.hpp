#ifndef MULTIQUADLIGHT_HPP_
#define MULTIQUADLIGHT_HPP_

#include "SolidAngleBvh.hpp"
#include "QuadMaterial.hpp"
#include "QuadGeometry.hpp"
#include "EmissiveBvh.hpp"
#include "Primitive.hpp"

#include "bvh/BinaryBvh.hpp"

#include <vector>
#include <memory>

namespace Tungsten {

class BitmapTexture;

class MultiQuadLight : public Primitive
{
    struct ThreadlocalSampleInfo
    {
        std::vector<float> sampleWeights;
        std::vector<int> insideIds;
        int insideCount;
        float outsideWeight;
        Vec3f lastQuery;
    };
    struct PrecomputedQuad
    {
        Vec3f center;
        Vec3f Ngu;
    };

    QuadGeometry _geometry;
    const std::vector<QuadMaterial> &_materials;

    Box3f _bounds;
    mutable std::vector<std::unique_ptr<ThreadlocalSampleInfo>> _samplers;
    std::unique_ptr<Bvh::BinaryBvh> _bvh;
    std::unique_ptr<EmissiveBvh> _sampleBvh;
    std::unique_ptr<SolidAngleBvh> _evalBvh;
    std::unique_ptr<TriangleMesh> _proxy;
    std::vector<PrecomputedQuad> _precomputedQuads;

    void buildSampleWeights(uint32 threadIndex, const Vec3f &p) const;

    void constructSampleBounds();
    void constructSphericalBounds();

public:
    MultiQuadLight(QuadGeometry geometry, const std::vector<QuadMaterial> &materials);

    virtual void fromJson(const rapidjson::Value &v, const Scene &scene) override;
    virtual rapidjson::Value toJson(Allocator &allocator) const override;

    virtual bool intersect(Ray &ray, IntersectionTemporary &data) const override;
    virtual bool occluded(const Ray &ray) const override;
    virtual bool hitBackside(const IntersectionTemporary &data) const override;
    virtual void intersectionInfo(const IntersectionTemporary &data, IntersectionInfo &info) const override;
    virtual bool tangentSpace(const IntersectionTemporary &data, const IntersectionInfo &info,
            Vec3f &T, Vec3f &B) const override;

    virtual bool isSamplable() const override;
    virtual void makeSamplable(uint32 threadIndex) override;

    virtual float inboundPdf(uint32 threadIndex, const IntersectionTemporary &data,
            const IntersectionInfo &info, const Vec3f &p, const Vec3f &d) const override;
    virtual bool sampleInboundDirection(uint32 threadIndex, LightSample &sample) const override;
    virtual bool sampleOutboundDirection(uint32 threadIndex, LightSample &sample) const override;

    virtual bool invertParametrization(Vec2f uv, Vec3f &pos) const override;

    virtual bool isDelta() const override;
    virtual bool isInfinite() const override;

    virtual float approximateRadiance(uint32 threadIndex, const Vec3f &p) const override;

    virtual Box3f bounds() const override;

    virtual const TriangleMesh &asTriangleMesh() override;

    virtual int numBsdfs() const override;
    virtual std::shared_ptr<Bsdf> &bsdf(int index) override;

    virtual void prepareForRender() override;
    virtual void cleanupAfterRender() override;

    virtual Primitive *clone() override;

    virtual bool isEmissive() const override;
    virtual Vec3f emission(const IntersectionTemporary &data, const IntersectionInfo &info) const override;
};

}



#endif /* MULTIQUADLIGHT_HPP_ */
