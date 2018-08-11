#ifndef PARTICLE_H_FZHEOU7G
#define PARTICLE_H_FZHEOU7G

#include <string>
#include <vector>
#include "Node.h"
#include "Graphics.h"
#include "Texture.h"
#include "kit/math/common.h"
#include "Resource.h"
#include "Mesh.h"
#include "kit/cache/cache.h"

class Particle:
    public Node
{
    public:

        struct Unit
        {
            bool active = false;
            Color color = Color::clear(); // opacity in alpha
            //glm::vec3 pos;
            //glm::vec3 vel;
            //glm::vec3 accel;
            glm::vec3 scale = glm::vec3(1.0);
            Freq::Time life;
        };
        
        Particle(std::string fn, ResourceCache* cache);
        virtual ~Particle();

        //void timeline(const std::shared_ptr<Freq::Timeline>& tl){
        //    m_pTimeline = tl;
        //}
        
        virtual void logic_self(Freq::Time t) override;
        virtual void render_self(Pass* pass) const override;
        virtual void set_render_matrix(Pass* pass) const override;

        void mesh(std::shared_ptr<Mesh> mesh) { m_pMesh = mesh; }
        std::shared_ptr<Mesh> mesh() { return m_pMesh; }

        enum {
            UPRIGHT = kit::bit(1)
        };

        void flags(unsigned f) { m_Flags = f; }
        unsigned flags() const { return m_Flags; }
        
        virtual std::string type() const override { return "particle"; }

        virtual void scale(float f, Space s = Space::LOCAL) override;
        virtual void rescale(float f) override;
        glm::vec3 scale(Space s = Space::LOCAL) const override;
        
        void color(Color c);
        void life(Freq::Time t);

        void offset(float ofs) { m_Offset = ofs; }
        float offset() { return m_Offset; }
        
    private:

        unsigned m_Flags = 0;
        std::shared_ptr<Mesh> m_pMesh;
        Unit m_Unit;

        float m_Offset = 0.0f;

        // separate timeline to accumulate ticks
        // and advance only on render() (lazy logic)
        //std::shared_ptr<Freq::Timeline> m_pTimeline;
};

class ParticleSystem:
    public Node
{
    public:

        struct Description
        {
            bool active = true;
            Particle::Unit ref;
            glm::vec3 pos_variance;
            glm::vec3 velocity_variance;
            Color color_delta;
            //glm::vec3 variance_bias;
            float spawn_rate = 1.0f;
        };

        ParticleSystem(std::string fn, ResourceCache* cache);
        virtual ~ParticleSystem();

        virtual std::string type() const override { return "particlesystem"; }
        
    private:

        //Particle::Unit m_Reference;
        std::shared_ptr<Particle> m_pParticle;
        Description m_Description;
        
        virtual void logic_self(Freq::Time t) override;
        virtual void render_self(Pass* pass) const override;
        //virtual void set_render_matrix(Pass* pass) const override;

        virtual void lazy_logic(Freq::Time t) override;
        
        mutable Freq::Time m_Accum;
        mutable Freq::Timeline m_Timeline;
};

#endif

